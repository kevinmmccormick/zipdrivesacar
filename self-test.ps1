# SPDX-License-Identifier: GPL-2.0-or-later
# Updated 2026-10-07: portable tool paths and build output; hidden automated launches.
param(
    [string]$DasmExe = "dasm",
    [string]$StellaExe = "stella",
    [string]$RomPath = ".\build\zipdrivesacar.bin",
    [int]$TimeoutSeconds = 20
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Build {
    & ".\build.ps1" -DasmExe $DasmExe
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE."
    }
}

function New-DebugScriptText {
    param(
        [int]$SelectPresses
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add("frame 2")
    $lines.Add("saveState 0")

    for ($i = 0; $i -lt $SelectPresses; $i++) {
        $lines.Add("swchb FD")
        $lines.Add("frame 1")
        $lines.Add("swchb FF")
        $lines.Add("frame 1")
    }

    $lines.Add("saveState 1")   # title state for selected mode
    $lines.Add("swchb FE")      # press reset to start
    $lines.Add("frame 1")
    $lines.Add("swchb FF")      # release reset
    $lines.Add("frame 8")
    $lines.Add("saveState 2")   # early gameplay

    # steer right
    $lines.Add("joy0Right 1")
    $lines.Add("frame 12")
    $lines.Add("joy0Right 0")
    $lines.Add("frame 2")
    $lines.Add("saveState 3")

    # drift left (fire + left)
    $lines.Add("joy0Left 1")
    $lines.Add("joy0Fire 1")
    $lines.Add("frame 10")
    $lines.Add("joy0Left 0")
    $lines.Add("joy0Fire 0")
    $lines.Add("frame 2")
    $lines.Add("saveState 4")

    # horn/meow tap + continue sim
    $lines.Add("joy0Fire 1")
    $lines.Add("frame 1")
    $lines.Add("joy0Fire 0")
    $lines.Add("frame 16")
    $lines.Add("saveState 5")

    # let world evolve (spawns, comfort drain, collisions opportunities)
    $lines.Add("frame 180")
    $lines.Add("saveState 6")

    return ($lines -join "`r`n") + "`r`n"
}

function Invoke-Scenario {
    param(
        [string]$Name,
        [int]$SelectPresses,
        [string]$StellaExePath,
        [string]$RomFullPath,
        [string]$ScenarioDir,
        [int]$TimeoutSec
    )

    $null = New-Item -ItemType Directory -Force -Path $ScenarioDir
    $stateDir = Join-Path $ScenarioDir "state"
    $null = New-Item -ItemType Directory -Force -Path $stateDir
    # Each run uses a unique directory; retain prior artifacts.

    $scriptPath = Join-Path $ScenarioDir "autoexec.script"
    $scriptText = New-DebugScriptText -SelectPresses $SelectPresses
    Set-Content -Path $scriptPath -Value $scriptText -NoNewline

    $args = @(
        "-basedir", "`"$ScenarioDir`"",
        "-debug",
        "`"$RomFullPath`""
    )
    $proc = Start-Process -FilePath $StellaExePath -ArgumentList $args -WindowStyle Hidden -PassThru
    $deadline = (Get-Date).AddSeconds($TimeoutSec)

    $expected = @(
        "zipdrivesacar.st0",
        "zipdrivesacar.st1",
        "zipdrivesacar.st2",
        "zipdrivesacar.st3",
        "zipdrivesacar.st4",
        "zipdrivesacar.st5",
        "zipdrivesacar.st6"
    )

    while ((Get-Date) -lt $deadline) {
        $present = Get-ChildItem $stateDir -Filter "zipdrivesacar.st*" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Name
        $allPresent = $true
        foreach ($file in $expected) {
            if ($present -notcontains $file) {
                $allPresent = $false
                break
            }
        }
        if ($allPresent) { break }
        Start-Sleep -Milliseconds 250
    }

    if (-not $proc.HasExited) {
        Stop-Process -Id $proc.Id -Force
    }

    $states = @{}
    foreach ($file in $expected) {
        $path = Join-Path $stateDir $file
        if (Test-Path $path) {
            $states[$file] = (Get-FileHash -Algorithm SHA256 -Path $path).Hash
        }
    }

    $missing = @()
    foreach ($file in $expected) {
        if (-not $states.ContainsKey($file)) { $missing += $file }
    }

    $stepDiffChecks = @(
        @("zipdrivesacar.st1", "zipdrivesacar.st2"),
        @("zipdrivesacar.st2", "zipdrivesacar.st3"),
        @("zipdrivesacar.st3", "zipdrivesacar.st4"),
        @("zipdrivesacar.st4", "zipdrivesacar.st5"),
        @("zipdrivesacar.st5", "zipdrivesacar.st6")
    )
    $stepDiffPass = $true
    foreach ($pair in $stepDiffChecks) {
        $a = $pair[0]
        $b = $pair[1]
        if ((-not $states.ContainsKey($a)) -or (-not $states.ContainsKey($b))) {
            $stepDiffPass = $false
            break
        }
        if ($states[$a] -eq $states[$b]) {
            $stepDiffPass = $false
            break
        }
    }

    $scenarioPass = ($missing.Count -eq 0) -and $stepDiffPass

    [PSCustomObject]@{
        Scenario = $Name
        SelectPresses = $SelectPresses
        Pass = $scenarioPass
        MissingStates = ($missing -join ",")
        UniqueStateHashes = (($states.Values | Sort-Object -Unique).Count)
        TitleStateHash = $states["zipdrivesacar.st1"]
        StateDir = $stateDir
    }
}

if (-not (Get-Command $StellaExe -CommandType Application -ErrorAction SilentlyContinue)) {
    throw "Stella not found at '$StellaExe'."
}

Invoke-Build

if (-not (Test-Path $RomPath)) {
    throw "ROM not found at '$RomPath' after build."
}
$romFull = (Resolve-Path $RomPath).Path

$artifactsBase = Join-Path (Resolve-Path ".\").Path "artifacts"
$null = New-Item -ItemType Directory -Force -Path $artifactsBase
$runRoot = Join-Path $artifactsBase ("self-test-" + (Get-Date -Format "yyyyMMdd-HHmmss-ffff"))
$null = New-Item -ItemType Directory -Force -Path $runRoot

$scenarios = @(
    @{ Name = "Cruise"; SelectPresses = 0 },
    @{ Name = "Chaos";  SelectPresses = 1 },
    @{ Name = "Zen";    SelectPresses = 2 }
)

$results = @()
foreach ($s in $scenarios) {
    $scenarioDir = Join-Path $runRoot $s.Name.ToLowerInvariant()
    $results += Invoke-Scenario -Name $s.Name -SelectPresses $s.SelectPresses -StellaExePath $StellaExe -RomFullPath $romFull -ScenarioDir $scenarioDir -TimeoutSec $TimeoutSeconds
}

$results | Format-Table Scenario,SelectPresses,Pass,MissingStates,UniqueStateHashes -AutoSize

# Cross-scenario mode-selection validation:
# Title save-state hash should differ between Cruise/Chaos/Zen.
$titleHashes = $results | Select-Object -ExpandProperty TitleStateHash
$distinctTitleHashes = ($titleHashes | Sort-Object -Unique).Count
$modeSelectPass = $distinctTitleHashes -eq 3

Write-Host "mode_select_hash_distinct=$distinctTitleHashes"
Write-Host "artifacts=$runRoot"

$failedCount = @($results | Where-Object { -not $_.Pass }).Count
$allPass = ($failedCount -eq 0) -and $modeSelectPass
if (-not $allPass) {
    exit 1
}
exit 0
