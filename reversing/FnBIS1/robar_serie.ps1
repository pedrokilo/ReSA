#requires -Version 7
<#
  BIS1: serie de ejecuciones del modo automatico -Robar (la DLL crea un coche con conductor junto al jugador y lo roba, N ciclos).
  Mide la TASA de fallo: ciclos con NaN / z<-50 / avisos de caida / crash, por ejecucion.
  Uso: pwsh -File robar_serie.ps1 -Variante A -Veces 3 [-Robar 20,15,15,5] [-Auto 480] [-ArgsFile ruta]
       -Variante B = --unhook-all (control). Espera si el candado de juego esta tomado.
#>
param([string]$Variante = 'A', [int]$Veces = 3, [string]$Robar = '20,15,15,5', [int]$Auto = 480, [string]$ArgsFile = '', [string]$Etiqueta = 'serie')
$ErrorActionPreference = 'Stop'
$jugar = Join-Path $PSScriptRoot 'jugar.ps1'
for ($i = 1; $i -le $Veces; $i++) {
    $et = "-$Etiqueta$Variante$i"
    while ($true) {
        $a = @('-NoProfile', '-File', $jugar, '-Variante', $Variante, '-Etiqueta', $et, '-Auto', $Auto, '-Sonda', '-TpExit', '-Robar', $Robar)
        if ($ArgsFile) { $a += @('-ArgsFile', $ArgsFile) }
        $o = & pwsh @a 2>&1 | Out-String
        if ($o -match 'esta tomado por otra prueba' -or $o -match 'ya esta en marcha') { Write-Host 'candado ocupado, espero 20 s...'; Start-Sleep 20; continue }
        break
    }
    $dir = Get-ChildItem (Join-Path $PSScriptRoot 'salidas') -Directory -Filter "$Variante$et-*" | Sort-Object Name | Select-Object -Last 1
    $r = Get-Content -Raw (Join-Path $dir.FullName 'resumen.json') | ConvertFrom-Json
    Write-Host ("[{0}{1}] dur={2}s salida={3} ciclos={4} robados={5} ciclos_nan={6} ciclos_zbajo={7} caidas_dll={8} NaNlineas={9} | {10}" -f $Variante, $i, $r.duracion_segundos, $r.codigo_salida, $r.robar_ciclos, $r.robar_robados, $r.robar_ciclos_con_NaN, $r.robar_ciclos_con_z_bajo, $r.avisos_caida_a_traves_del_mapa, $r.lineas_con_NaN, $r.robar_fin)
}
