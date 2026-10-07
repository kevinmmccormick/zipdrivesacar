# SPDX-License-Identifier: GPL-2.0-or-later
# Updated 2026-10-07: portable tool paths and build output; hidden automated launches.
param(
    [string]$StellaExe = "stella",
    [string]$RomPath = ".\build\zipdrivesacar.bin",
    [int]$Runs = 3,
    [int]$RunSeconds = 3
)

if (-not (Get-Command $StellaExe -CommandType Application -ErrorAction SilentlyContinue)) {
    Write-Error "Stella not found at '$StellaExe'."
    exit 1
}

if (-not (Test-Path $RomPath)) {
    Write-Error "ROM not found at '$RomPath'. Build first with .\\build.ps1"
    exit 1
}

$romFull = (Resolve-Path $RomPath).Path
$results = @()
$failed = $false

for ($i = 1; $i -le $Runs; $i++) {
    $p = Start-Process -FilePath $StellaExe -ArgumentList @("""$romFull""") -WindowStyle Hidden -PassThru
    Start-Sleep -Milliseconds 900

    $cpuStart = 0.0
    try {
        $cpuStart = (Get-Process -Id $p.Id -ErrorAction Stop).CPU
    } catch {
        $failed = $true
        $results += [PSCustomObject]@{
            Run = $i
            Pid = $p.Id
            AliveAfterRun = $false
            CpuStart = $cpuStart
            CpuEnd = $cpuStart
            Pass = $false
        }
        continue
    }

    Start-Sleep -Seconds ([Math]::Max(1, $RunSeconds - 1))
    $proc = Get-Process -Id $p.Id -ErrorAction SilentlyContinue

    if ($null -eq $proc) {
        $failed = $true
        $results += [PSCustomObject]@{
            Run = $i
            Pid = $p.Id
            AliveAfterRun = $false
            CpuStart = $cpuStart
            CpuEnd = $cpuStart
            Pass = $false
        }
        continue
    }

    $cpuEnd = $proc.CPU
    $alive = $true
    $cpuAdvanced = $cpuEnd -gt $cpuStart
    $pass = $alive -and $cpuAdvanced
    if (-not $pass) { $failed = $true }

    Stop-Process -Id $p.Id -Force

    $results += [PSCustomObject]@{
        Run = $i
        Pid = $p.Id
        AliveAfterRun = $alive
        CpuStart = [Math]::Round($cpuStart, 3)
        CpuEnd = [Math]::Round($cpuEnd, 3)
        Pass = $pass
    }
}

$results | Format-Table -AutoSize

if ($failed) { exit 1 }
exit 0
