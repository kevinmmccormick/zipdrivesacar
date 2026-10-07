# SPDX-License-Identifier: GPL-2.0-or-later
# Updated 2026-10-07: portable tool paths and build output; hidden automated launches.
param(
    [string]$StellaExe = "stella",
    [string]$RomPath = ".\build\zipdrivesacar.bin"
)

if (-not (Get-Command $StellaExe -CommandType Application -ErrorAction SilentlyContinue)) {
    Write-Error "Stella not found at '$StellaExe'. Pass -StellaExe with the correct path."
    exit 1
}

if (-not (Test-Path $RomPath)) {
    Write-Error "ROM not found at '$RomPath'. Build first with .\\build.ps1"
    exit 1
}

& $StellaExe $RomPath
