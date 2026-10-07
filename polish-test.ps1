# SPDX-License-Identifier: GPL-2.0-or-later
# Updated 2026-10-07: portable tool paths and build output; hidden automated launches.
param(
    [string]$DasmExe = "dasm",
    [string]$StellaExe = "stella",
    [string]$RomPath = ".\build\zipdrivesacar.bin",
    [int]$TimeoutSeconds = 30
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Step {
    param(
        [string]$Name,
        [scriptblock]$Action
    )
    Write-Host "== $Name =="
    & $Action
    if ($LASTEXITCODE -ne 0) {
        throw "$Name failed with exit code $LASTEXITCODE."
    }
}

function New-ScenarioScriptText {
    param(
        [int]$SelectPresses
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $modeHex = "{0:X2}" -f $SelectPresses
    $lines.Add("swchb FF")
    $lines.Add("frame 2")
    $lines.Add("ram 80 01")    # force STATE_TITLE
    $lines.Add("ram 81 $modeHex")
    $lines.Add("frame 2")
    $lines.Add("saveSnap")     # title snapshot for selected mode
    $lines.Add("swchb FE")     # reset press
    $lines.Add("frame 1")
    $lines.Add("swchb FF")     # reset release
    $lines.Add("frame 8")
    $lines.Add("saveSnap")     # early gameplay

    $lines.Add("joy0Right 1")
    $lines.Add("frame 12")
    $lines.Add("joy0Right 0")
    $lines.Add("frame 2")
    $lines.Add("saveSnap")

    $lines.Add("joy0Left 1")
    $lines.Add("joy0Fire 1")
    $lines.Add("frame 10")
    $lines.Add("joy0Left 0")
    $lines.Add("joy0Fire 0")
    $lines.Add("frame 2")
    $lines.Add("saveSnap")

    $lines.Add("frame 120")
    $lines.Add("saveSnap")
    $lines.Add("exitRom")

    return ($lines -join "`r`n") + "`r`n"
}

function Get-ImageMetrics {
    param(
        [string]$Path
    )

    Add-Type -AssemblyName System.Drawing
    $bmp = [System.Drawing.Bitmap]::FromFile($Path)
    $width = $bmp.Width
    $height = $bmp.Height
    $hash = [System.Security.Cryptography.SHA256]::Create()
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    $digest = [System.BitConverter]::ToString($hash.ComputeHash($bytes)).Replace("-", "")

    $colorSet = New-Object System.Collections.Generic.HashSet[string]
    $nonBlack = 0
    $carLike = 0

    $x0 = [int]($bmp.Width * 0.38)
    $x1 = [int]($bmp.Width * 0.70)
    $y0 = [int]($bmp.Height * 0.60)
    $y1 = [int]($bmp.Height * 0.95)

    for ($y = 0; $y -lt $bmp.Height; $y++) {
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            $c = $bmp.GetPixel($x, $y)
            $null = $colorSet.Add(("{0:X2}{1:X2}{2:X2}" -f $c.R, $c.G, $c.B))
            if (($c.R + $c.G + $c.B) -gt 12) {
                $nonBlack++
            }

            if ($x -ge $x0 -and $x -le $x1 -and $y -ge $y0 -and $y -le $y1) {
                if ($c.R -gt 200 -and $c.G -gt 110 -and $c.B -lt 130) {
                    $carLike++
                }
            }
        }
    }

    $bmp.Dispose()
    $hash.Dispose()

    [PSCustomObject]@{
        Path = $Path
        Width = $width
        Height = $height
        UniqueColors = $colorSet.Count
        NonBlackPixels = $nonBlack
        CarLikePixels = $carLike
        Sha256 = $digest
    }
}

function Invoke-VisualScenario {
    param(
        [string]$Name,
        [int]$SelectPresses,
        [string]$StellaExePath,
        [string]$RomFullPath,
        [string]$ScenarioRoot,
        [int]$TimeoutSec
    )

    $null = New-Item -ItemType Directory -Force -Path $ScenarioRoot
    $snapDir = Join-Path $ScenarioRoot "snaps"
    $stateDir = Join-Path $ScenarioRoot "state"
    $null = New-Item -ItemType Directory -Force -Path $snapDir
    $null = New-Item -ItemType Directory -Force -Path $stateDir

    $scriptPath = Join-Path $ScenarioRoot "autoexec.script"
    Set-Content -Path $scriptPath -Value (New-ScenarioScriptText -SelectPresses $SelectPresses) -NoNewline

    $args = @(
        "-basedir", "`"$ScenarioRoot`"",
        "-snapsavedir", "`"$snapDir`"",
        "-debug",
        "`"$RomFullPath`""
    )
    $proc = Start-Process -FilePath $StellaExePath -ArgumentList $args -WindowStyle Hidden -PassThru
    $deadline = (Get-Date).AddSeconds($TimeoutSec)

    do {
        $snapCount = @(Get-ChildItem $snapDir -Filter "*.png" -ErrorAction SilentlyContinue).Count
        if ($snapCount -ge 5) { break }
        Start-Sleep -Milliseconds 250
    } while ((Get-Date) -lt $deadline)

    if (-not $proc.HasExited) {
        Stop-Process -Id $proc.Id -Force
    }

    $snaps = @(Get-ChildItem $snapDir -Filter "*.png" | Sort-Object LastWriteTime)
    if ($snaps.Count -lt 5) {
        return [PSCustomObject]@{
            Scenario = $Name
            Pass = $false
            Reason = "MissingSnapshots"
            Snapshots = $snaps.Count
            TitleHash = ""
            GameplayHash = ""
            CarLikePixels = 0
            UniqueColors = 0
            ScenarioDir = $ScenarioRoot
        }
    }

    $titleMetrics = Get-ImageMetrics -Path $snaps[0].FullName
    $playMetrics = Get-ImageMetrics -Path $snaps[1].FullName

    $pass = ($titleMetrics.UniqueColors -ge 3) -and ($playMetrics.UniqueColors -ge 4) -and ($playMetrics.CarLikePixels -ge 14)
    $reason = if ($pass) { "OK" } else { "VisualThresholdFail" }

    [PSCustomObject]@{
        Scenario = $Name
        Pass = $pass
        Reason = $reason
        Snapshots = $snaps.Count
        TitleHash = $titleMetrics.Sha256
        GameplayHash = $playMetrics.Sha256
        CarLikePixels = $playMetrics.CarLikePixels
        UniqueColors = $playMetrics.UniqueColors
        ScenarioDir = $ScenarioRoot
    }
}

if (-not (Get-Command $DasmExe -CommandType Application -ErrorAction SilentlyContinue)) {
    throw "DASM not found at '$DasmExe'."
}
if (-not (Get-Command $StellaExe -CommandType Application -ErrorAction SilentlyContinue)) {
    throw "Stella not found at '$StellaExe'."
}

Invoke-Step -Name "Build" -Action { & ".\build.ps1" -DasmExe $DasmExe }
Invoke-Step -Name "Smoke" -Action { & ".\smoke-test.ps1" -StellaExe $StellaExe -RomPath $RomPath }
Invoke-Step -Name "Self" -Action { & ".\self-test.ps1" -DasmExe $DasmExe -StellaExe $StellaExe -RomPath $RomPath }

if (-not (Test-Path $RomPath)) {
    throw "ROM not found at '$RomPath' after build."
}

$romFull = (Resolve-Path $RomPath).Path
$artifactsBase = Join-Path (Resolve-Path ".\").Path "artifacts"
$null = New-Item -ItemType Directory -Force -Path $artifactsBase
$runRoot = Join-Path $artifactsBase ("polish-test-" + (Get-Date -Format "yyyyMMdd-HHmmss-ffff"))
$null = New-Item -ItemType Directory -Force -Path $runRoot

$scenarios = @(
    @{ Name = "Cruise"; SelectPresses = 0 },
    @{ Name = "Chaos";  SelectPresses = 1 },
    @{ Name = "Zen";    SelectPresses = 2 }
)

$results = @()
foreach ($s in $scenarios) {
    $scenarioRoot = Join-Path $runRoot $s.Name.ToLowerInvariant()
    $results += Invoke-VisualScenario -Name $s.Name -SelectPresses $s.SelectPresses -StellaExePath $StellaExe -RomFullPath $romFull -ScenarioRoot $scenarioRoot -TimeoutSec $TimeoutSeconds
}

$results | Format-Table Scenario,Pass,Reason,Snapshots,CarLikePixels,UniqueColors -AutoSize

$titleDistinctCount = ($results | Select-Object -ExpandProperty TitleHash | Sort-Object -Unique).Count
$playDistinctCount = ($results | Select-Object -ExpandProperty GameplayHash | Sort-Object -Unique).Count
$modeTitleDistinct = ($titleDistinctCount -eq 3)
$modeGameplayDistinct = ($playDistinctCount -eq 3)
Write-Host "mode_title_hash_distinct_count=$titleDistinctCount"
Write-Host "mode_gameplay_hash_distinct_count=$playDistinctCount"
Write-Host "mode_gameplay_hash_distinct=$modeGameplayDistinct"

$reportPath = Join-Path $runRoot "report.json"
$results | ConvertTo-Json -Depth 4 | Set-Content -Path $reportPath
Write-Host "artifacts=$runRoot"
Write-Host "report=$reportPath"

$failed = @($results | Where-Object { -not $_.Pass }).Count
if (($failed -gt 0) -or (-not $modeTitleDistinct) -or (-not $modeGameplayDistinct)) {
    exit 1
}

exit 0
