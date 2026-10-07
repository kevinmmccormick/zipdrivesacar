# SPDX-License-Identifier: GPL-2.0-or-later
# Updated 2026-10-07: portable tool lookup, isolated output, ROM size check.
param([string]$DasmExe = 'dasm')

$ErrorActionPreference = 'Stop'
$assembler = Get-Command $DasmExe -CommandType Application -ErrorAction Stop
Push-Location $PSScriptRoot
try {
    $null = New-Item -ItemType Directory -Force -Path 'build'
    & $assembler.Source main.asm -f3 '-obuild/zipdrivesacar.bin' '-lbuild/zipdrivesacar.lst' '-sbuild/zipdrivesacar.sym'
    if ($LASTEXITCODE -ne 0) { throw "DASM failed with exit code $LASTEXITCODE." }
    $rom = Get-Item 'build/zipdrivesacar.bin'
    if ($rom.Length -ne 4096) { throw "Expected a 4096-byte ROM; got $($rom.Length) bytes." }
    $rom | Select-Object Name, Length | Format-Table -AutoSize
    Get-FileHash $rom.FullName -Algorithm SHA256
} finally {
    Pop-Location
}
