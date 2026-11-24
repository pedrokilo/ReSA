#requires -Version 7
<#
Mismo comparador que ejecutar_g68.ps1 pero con el ASI sobre el gta_sa.exe ORIGINAL (copia de pruebas de FnBIS1\jugar.ps1, que toma el candado
D:\Original\gta_lock y restaura siempre su copia). Los objetos DatosExe viven en gta_reversed.asi (direccion = base del modulo + RVA del PDB);
las direcciones del EXE son las reales. Solo se toca el gta_sa.exe que cuelga de ESTE lanzador (se busca por proceso padre).
Uso: pwsh -File ejecutar_asi_g68.ps1 -Etiqueta a1 -Asi <gta_reversed.asi> [-Auto 140] [-T 45,85,120] [-Jugar <jugar.ps1>]
#>
param([string]$Etiqueta = 'a', [string]$Asi, [int]$Auto = 140, [string]$T = '45,85,120',
      [string]$Jugar = 'D:\Original\wv-8\reversing\FnBIS1\jugar.ps1', [string]$ArgsFile = 'D:\Original\wv-8\reversing\FnBIS1\cal_mal.args')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$repo = (Resolve-Path (Join-Path $aqui '..\..')).Path
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$pdb = [IO.Path]::ChangeExtension($Asi, '.pdb')
$gpid = $null
$t0 = Get-Date
while (-not $gpid -and ((Get-Date) - $t0).TotalMinutes -lt 360) {
    # jugar.ps1 no espera el candado (sale con error si esta tomado): se reintenta
    $j = Start-Process pwsh -ArgumentList '-NoProfile', '-File', "`"$Jugar`"", '-Variante', 'X', '-AsiPath', "`"$Asi`"", '-ArgsFile', "`"$ArgsFile`"", '-Auto', $Auto, '-NoEnfocar', '-Etiqueta', "`"$Etiqueta`"" -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'jugar.txt') -RedirectStandardError (Join-Path $sal 'jugar_err.txt')
    $t1 = Get-Date
    while (-not $j.HasExited -and ((Get-Date) - $t1).TotalSeconds -lt 900) {
        $c = Get-CimInstance Win32_Process -Filter "Name='gta_sa.exe' AND ParentProcessId=$($j.Id)" -ErrorAction SilentlyContinue
        if ($c) { $gpid = @($c)[0].ProcessId; break }
        Start-Sleep 2
    }
    if (-not $gpid) { Start-Sleep 30 }
}
if (-not $gpid) { 'no se encontro gta_sa.exe hijo'; exit 3 }
"gta_sa.exe pid $gpid"
$son = Start-Process python -ArgumentList '-I', "`"$(Join-Path $repo 'contrib\auditoria_datos\comparar_objetos.py')`"", '--pdb', "`"$pdb`"", '--bin', "`"$Asi`"", '--modulo', 'gta_reversed.asi', '--repo', "`"$repo`"", '--pid', $gpid, '--salida', "`"$sal`"", '--t', $T, '--plan', "`"$(Join-Path $repo 'docs\DatosExe\plan-objetos.csv')`"" -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'comparar.txt') -RedirectStandardError (Join-Path $sal 'comparar_err.txt')
$j.WaitForExit(($Auto + 300) * 1000) | Out-Null
if (-not $son.WaitForExit(30000)) { $son.Kill() }
$sal
