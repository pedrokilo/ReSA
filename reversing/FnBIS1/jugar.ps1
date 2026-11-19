#requires -Version 7
<#
.SYNOPSIS
  Lanzador BIS1 (copia ampliada de FnPL1\jugar.ps1): instala una variante de la DLL en la copia de pruebas, lanza el juego, espera a que
  termine y restaura SIEMPRE lo original. Deja un resumen en salidas\<variante>-<hora>\.

.EXAMPLE
  pwsh -File jugar.ps1 -Variante B
  pwsh -File jugar.ps1 -Restaurar          # solo restaura (si una ejecucion anterior se corto)
  pwsh -File jugar.ps1 -Variante B -Simular # prueba el respaldo/restauracion SIN lanzar el juego
#>
[CmdletBinding()]
param(
    [ValidateSet('A', 'B', 'C', 'D', 'E', 'F', 'O', 'X')][string]$Variante,
    [switch]$Restaurar,
    [switch]$Simular,          # no lanza el juego: instala, espera 3 s, restaura (prueba del propio lanzador)
    [switch]$SinCandado,       # no tomar D:\Original\gta_lock
    [string]$Carpeta = 'E:\JUEGOS\GTA San Andreas - CVehicle Test',   # SOLO la copia de pruebas
    [string]$Salidas = '',
    [int]$Auto = 0,            # PL1: si > 0, el lanzador cierra SU propio juego tras N segundos (para pruebas automaticas)
    [string[]]$ExtraArgs = @(),# PL1: argumentos extra para gta_sa.exe (p. ej. --unhook=Clase)
    [string]$ArgsFile = '',    # PL1: archivo con los argumentos extra (separados por espacios; evita problemas de linea de comandos)
    [string]$AsiPath = '',     # PL1: DLL concreta (variante X)
    [switch]$Sonda,            # PL1: durante la ejecucion automatica lee de la memoria el pool de CEntryExit
    [string]$Etiqueta = '',
    [switch]$TpExit,           # PL1: la sonda coloca al jugador en el marcador de salida del interior
    [switch]$Tp,               # PL1: la sonda saca al jugador de la casa (solo pruebas automaticas)
    [switch]$ParaAlCaer,       # PL1: con -Auto, cierra el juego propio en cuanto aparece un aviso de caida a traves del mapa
    [int]$MinCaidas = 1,       # BIS1: con -ParaAlCaer, nº de avisos de caida que dispara el cierre
    [string]$Robar = '',       # BIS1: prueba automatica de robo (en la DLL): 'ciclos,periodo_s,espera_s,conducir_s' (p. ej. 20,20,20,6)
    [switch]$NoEnfocar,        # BIS1: no intentar traer la ventana del juego al frente (por defecto SI en ejecuciones con -Auto, solo si el usuario lleva >= 10 min inactivo)
    [switch]$Enfocar,          # BIS1: trae la ventana del juego a primer plano cada 5 s (el bucle original se pausa en segundo plano)
    [switch]$NoParaSenal,      # BIS1: con -Robar no cerrar el juego al primer ciclo con z bajo/NaN
    [switch]$Silencioso        # BIS1: menos salida por consola (lo usa biseccion.ps1)
)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
if (-not $Salidas) { $Salidas = Join-Path $aqui 'salidas' }
$pendiente = Join-Path $aqui 'respaldo_pendiente'
$candado = 'D:\Original\gta_lock'
$pl1 = (Resolve-Path (Join-Path $aqui '..\FnPL1')).Path

# Proteccion: jamas tocar la instalacion original.
if ($Carpeta.TrimEnd('\') -ieq 'E:\JUEGOS\GTA San Andreas') { throw 'Esa es la instalacion ORIGINAL; este lanzador solo trabaja en la copia de pruebas.' }

# Lo unico que se reemplaza (rutas relativas a la carpeta del juego). Nada de partidas ni userfiles.
$archivos = @('scripts\gta_reversed.asi', 'scripts\gta_reversed.pdb', 'gta-reversed.ini', 'scripts\gta-reversed.ini')
if ($Simular) { $archivos = @($archivos | Where-Object { $_ -notlike '*.pdb' }) }   # BIS1: el simulacro no copia el PDB (450 MB) para ir rapido

function Hash($p) { if (Test-Path -LiteralPath $p -PathType Leaf) { (Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLower() } else { $null } }

function Restaurar-Desde($dirRespaldo) {
    $m = Get-Content -LiteralPath (Join-Path $dirRespaldo 'respaldo.json') -Raw | ConvertFrom-Json
    $carp = $m.carpeta
    if ($carp.TrimEnd('\') -ieq 'E:\JUEGOS\GTA San Andreas') { throw 'Respaldo apunta a la instalacion original; abortado.' }
    $ok = $true
    foreach ($f in $m.archivos) {
        $dest = Join-Path $carp $f.ruta
        if ($f.existia) {
            Copy-Item -LiteralPath (Join-Path $dirRespaldo ('copia\' + $f.ruta)) -Destination $dest -Force
            (Get-Item -LiteralPath $dest).LastWriteTimeUtc = ([datetime]$f.fecha_utc).ToUniversalTime()
            $h = Hash $dest
            if ($h -ne $f.sha256) { $ok = $false; Write-Warning "RESTAURACION FALLIDA: $($f.ruta) (hash $h, esperado $($f.sha256))" }
        } else {
            Remove-Item -LiteralPath $dest -Force -ErrorAction SilentlyContinue
            if (Test-Path -LiteralPath $dest) { $ok = $false; Write-Warning "No se pudo quitar $($f.ruta)" }
        }
    }
    return $ok
}

# 1) Si una ejecucion anterior se corto, restaurar primero.
if (Test-Path -LiteralPath (Join-Path $pendiente 'respaldo.json')) {
    if (-not $SinCandado -and ((Get-Process -Name gta_sa -ErrorAction SilentlyContinue) -or (Test-Path -LiteralPath $candado))) { throw "El candado $candado esta tomado por otra prueba (hay un respaldo pendiente que se restaurara cuando termine; no lo toco ahora)." }
    Write-Host 'Hay un respaldo pendiente de una ejecucion anterior: restaurando primero...' -ForegroundColor Yellow
    if (Restaurar-Desde $pendiente) {
        Remove-Item -LiteralPath $pendiente -Recurse -Force
        Write-Host 'Restaurado y verificado por hash.' -ForegroundColor Green
    } else { throw ('La restauracion pendiente fallo; revisa a mano (respaldo en ' + $pendiente + ').') }
}
if ($Restaurar) { Write-Host 'Nada mas que restaurar.'; return }
if (-not $Variante) { throw 'Falta -Variante A|B|C|D|E|F|X (o -Restaurar).' }

# 2) Definicion de la variante
$dllOrig = Join-Path $pl1 'dll\Debug.asi'            # la DLL del usuario (ff45...)
$dllDebug = Join-Path $aqui 'dll\Debug.asi'             # BIS1: la misma + modo --robar
$hashNueva = if (Test-Path (Join-Path $aqui 'dll\Debug.sha256')) { (Get-Content (Join-Path $aqui 'dll\Debug.sha256') -Raw).Trim() } else { '' }
$dllFix = Join-Path $pl1 'dll\Corregida.asi'
$hashFix = if (Test-Path (Join-Path $pl1 'dll\Corregida.sha256')) { (Get-Content (Join-Path $pl1 'dll\Corregida.sha256') -Raw).Trim() } else { '' }
$hashDebug = 'ff45cdb1ffca993bb66520db34fbe8dfcd2c0c97e0f09d1a19651db2b72ea278'
$raiz = (Resolve-Path (Join-Path $aqui '..\..')).Path
$dllRelease = Join-Path $pl1 'dll\Release.asi'
if (-not (Test-Path $dllRelease)) { $dllRelease = Join-Path $raiz 'bin\Release\gta_reversed.asi' }
$ini = Join-Path $pl1 'variantes\ini-actual.ini'
if ($ArgsFile) { $ExtraArgs = @((Get-Content -LiteralPath $ArgsFile -Raw).Trim() -split '\s+' | Where-Object { $_ }) }
$args_ = @($ExtraArgs)
$asi = $dllDebug; $pdb = Join-Path $aqui 'dll\Debug.pdb'; $descr = ''
if ($Robar) { $args_ += "--robar=$Robar" }
switch ($Variante) {
    'A' { $descr = 'DLL BIS1 (Debug + modo --robar) completa + INI actual (FastLoader, ranura 2).' }
    'O' { $asi = $dllOrig; $pdb = Join-Path $pl1 'dll\Debug.pdb'; $descr = 'DLL ORIGINAL del usuario (ff45...) + INI actual; no admite --robar.' }
    'B' { $args_ = @('--unhook-all') + $args_; $descr = 'MISMA DLL con --unhook-all (solo codigo original; la DLL solo hace de cargador) + INI actual. CONTROL decisivo.' }
    'C' { $ini = Join-Path $pl1 'variantes\ini-C-menu.ini'; $descr = 'DLL Debug completa con FastLoader DESACTIVADO (arranque normal por menu).' }
    'D' {
        $asi = $dllRelease; $pdb = [IO.Path]::ChangeExtension($dllRelease, '.pdb')
        $descr = 'DLL Release completa + INI actual.'
        if (-not (Test-Path $asi)) { throw "Variante D no disponible: no existe la DLL Release ($asi). Ver README (seccion Release)." }
    }
    'F' { $asi = $dllFix; $pdb = Join-Path $aqui 'dll\Corregida.pdb'; $descr = 'DLL Debug CORREGIDA (PL1) + INI actual (FastLoader, ranura 2).'
        if (-not (Test-Path $asi)) { throw "Falta $asi" }
        if ((Hash $asi) -ne $hashFix) { throw 'La DLL corregida no coincide con dll\Corregida.sha256' } }
    'X' { if (-not $AsiPath) { throw 'Variante X: falta -AsiPath' }; $asi = $AsiPath; $pdb = [IO.Path]::ChangeExtension($AsiPath, '.pdb'); $descr = "DLL arbitraria $AsiPath (banco PL1) + INI actual." }
    'E' { $ini = Join-Path $pl1 'variantes\ini-E-ranura1.ini'; $descr = 'DLL Debug completa, FastLoader con la ranura 1 (sin forzar versiones).' }
}
if ($asi -eq $dllOrig) {
    if (-not (Test-Path $asi)) { throw "Falta $asi" }
    if ((Hash $asi) -ne $hashDebug) { throw "El ASI original no coincide con el hash esperado $hashDebug" }
}
if ($asi -eq $dllDebug) {
    if (-not (Test-Path $asi)) { throw "Falta la DLL BIS1 ($asi). Compila el proyecto (ver README) o usa -Variante O." }
    if ($hashNueva -and (Hash $asi) -ne $hashNueva) { throw "La DLL BIS1 no coincide con dll\Debug.sha256" }
}
foreach ($p in @($asi, $ini)) { if (-not (Test-Path -LiteralPath $p)) { throw "No existe el archivo origen: $p" } }
if (-not (Test-Path -LiteralPath (Join-Path $Carpeta 'gta_sa.exe'))) { throw "No hay gta_sa.exe en $Carpeta" }
if (-not $Simular -and (Get-Process -Name gta_sa -ErrorAction SilentlyContinue)) { throw 'gta_sa.exe ya esta en marcha; cierralo antes.' }

$hora = Get-Date -Format 'yyyyMMdd-HHmmss'
$dirSalida = Join-Path $Salidas "$Variante$Etiqueta-$hora"
New-Item -ItemType Directory -Force -Path $dirSalida | Out-Null

# 3) Candado compartido con el resto de pruebas de juego (mkdir atomico; nunca se borra uno ajeno)
$tengoCandado = $false
if (-not $SinCandado) {
    try { New-Item -ItemType Directory -Path $candado -ErrorAction Stop | Out-Null; $tengoCandado = $true }
    catch { throw "El candado $candado esta tomado por otra prueba (no lo borro). Espera o usa -SinCandado si estas seguro." }
    Set-Content -LiteralPath (Join-Path $candado 'owner_bis1') -Value "BIS1 $Variante $hora"
}

$restaurado = $true; $resumen = [ordered]@{}
try {
    # 4) Respaldo con hashes (ANTES de tocar nada) + marca de respaldo pendiente
    $respaldo = Join-Path $dirSalida 'respaldo'
    $lista = @()
    foreach ($r in $archivos) {
        $p = Join-Path $Carpeta $r
        $e = Test-Path -LiteralPath $p -PathType Leaf
        $o = [ordered]@{ ruta = $r; existia = $e; sha256 = $null; tamano = $null; fecha_utc = $null }
        if ($e) {
            $d = Join-Path $respaldo ('copia\' + $r); New-Item -ItemType Directory -Force -Path (Split-Path $d) | Out-Null
            Copy-Item -LiteralPath $p -Destination $d -Force
            $o.sha256 = Hash $p; $o.tamano = (Get-Item -LiteralPath $p).Length
            $o.fecha_utc = (Get-Item -LiteralPath $p).LastWriteTimeUtc.ToString('o')
            if ((Hash $d) -ne $o.sha256) { throw "La copia de seguridad de $r no coincide por hash; no se toca nada." }
        }
        $lista += [pscustomobject]$o
    }
    $manif = [ordered]@{ carpeta = $Carpeta; variante = $Variante; hora = $hora; archivos = $lista }
    $manif | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $respaldo 'respaldo.json') -Encoding utf8
    # marca pendiente = misma copia (si se corta todo, la proxima ejecucion restaura)
    if (Test-Path $pendiente) { Remove-Item -Recurse -Force $pendiente }
    New-Item -ItemType Directory -Force -Path $pendiente | Out-Null
    Copy-Item -Recurse -Force -Path (Join-Path $respaldo '*') -Destination $pendiente

    # Partidas: huella antes (solo lectura) para comprobar que no cambian
    $uf = Join-Path $Carpeta 'userfiles'
    $partidasAntes = @{}
    if (Test-Path $uf) { Get-ChildItem $uf -Filter 'GTASAsf*.b' | ForEach-Object { $partidasAntes[$_.Name] = Hash $_.FullName } }

    # 5) Instalar la variante
    Copy-Item -LiteralPath $asi -Destination (Join-Path $Carpeta 'scripts\gta_reversed.asi') -Force
    if ($Simular) { }
    elseif (Test-Path -LiteralPath $pdb) { Copy-Item -LiteralPath $pdb -Destination (Join-Path $Carpeta 'scripts\gta_reversed.pdb') -Force }
    else { Remove-Item -LiteralPath (Join-Path $Carpeta 'scripts\gta_reversed.pdb') -Force -ErrorAction SilentlyContinue }
    Copy-Item -LiteralPath $ini -Destination (Join-Path $Carpeta 'gta-reversed.ini') -Force
    Copy-Item -LiteralPath $ini -Destination (Join-Path $Carpeta 'scripts\gta-reversed.ini') -Force
    $hashAsi = Hash (Join-Path $Carpeta 'scripts\gta_reversed.asi')

    $log = Join-Path $Carpeta 'scripts\logs\log.log'
    $logIni = if (Test-Path $log) { (Get-Item $log).Length } else { 0 }

    Write-Host ''
    Write-Host "== Variante $Variante : $descr" -ForegroundColor Cyan
    Write-Host "   Argumentos: $($args_ -join ' ')   ASI SHA256: $hashAsi"
    $t0 = Get-Date
    $codigo = $null
    if ($Simular) {
        Write-Host '   (SIMULADO: no se lanza el juego)' -ForegroundColor Yellow
        Start-Sleep -Seconds 3; $codigo = 'simulado'
    } else {
        Write-Host '   Lanzando gta_sa.exe. Juega; al cerrar el juego se restaura todo solo.' -ForegroundColor Green
        $pr = Start-Process -FilePath (Join-Path $Carpeta 'gta_sa.exe') -WorkingDirectory $Carpeta -ArgumentList $args_ -PassThru
        if ($Auto -gt 0) {
            $sondaLog = Join-Path $dirSalida 'sonda_pool.txt'; $ultEnfoque = [datetime]::MinValue; $avisado = $false
            while (-not $pr.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt $Auto) {
                Start-Sleep -Seconds 5
                if (($ParaAlCaer -or $Robar) -and (Test-Path $log)) {
                    try {
                        $fs2 = [IO.File]::Open($log, 'Open', 'Read', 'ReadWrite'); $null = $fs2.Seek([math]::Min($logIni, $fs2.Length), 'Begin')
                        $txt2 = (New-Object IO.StreamReader($fs2, [Text.Encoding]::UTF8)).ReadToEnd(); $fs2.Dispose()
                        $nc = ([regex]::Matches($txt2, 'a través del mapa')).Count
                        if ($ParaAlCaer -and $nc -ge $MinCaidas) { Start-Sleep -Seconds 2; break }
                        if ($Robar -and $txt2.Contains('[ROBAR] FIN')) { Start-Sleep -Seconds 3; break }
                        if ($Robar -and ($txt2.Contains('z_bajo=SI') -or $txt2.Contains('nan=SI')) -and -not $NoParaSenal) { Start-Sleep -Seconds 2; break }
                        if ($txt2.Contains('[ROBAR] NaN') -and $ParaAlCaer) { Start-Sleep -Seconds 2; break }
                    } catch {}
                }
                if (($Enfocar -or -not $NoEnfocar) -and -not $pr.HasExited -and (((Get-Date) - $ultEnfoque).TotalSeconds -ge 30 -or $ultEnfoque -eq [datetime]::MinValue)) {
                    $ultEnfoque = Get-Date
                    try { $oe = (& python -I (Join-Path $aqui 'enfocar.py') $pr.Id 2>&1) -join ' '; Add-Content -LiteralPath (Join-Path $dirSalida 'enfocar.txt') -Value ('{0:N0}s {1}' -f ((Get-Date) - $t0).TotalSeconds, $oe); if ($oe -like 'AVISO*' -and -not $avisado) { $avisado = $true; Write-Host "   $oe" -ForegroundColor Yellow } } catch {}
                }
                if ($Sonda -and -not $pr.HasExited) { try { $o = & python -I (Join-Path $pl1 'sonda_pool.py') $pr.Id $(if ($TpExit -and ((Get-Date) - $t0).TotalSeconds -gt 25) { '--tpexit' } elseif ($Tp -and ((Get-Date) - $t0).TotalSeconds -gt 25) { '--tp' }) 2>&1; Add-Content -LiteralPath $sondaLog -Value ('{0:N0}s {1}' -f ((Get-Date) - $t0).TotalSeconds, ($o -join ' ')) } catch {} }
            }
            if (-not $pr.HasExited) { [void]$pr.CloseMainWindow(); if (-not $pr.WaitForExit(8000)) { $pr.Kill() } }
            $pr.WaitForExit(); $codigo = $pr.ExitCode
        } else { $pr.WaitForExit(); $codigo = $pr.ExitCode }
    }
    $dur = (Get-Date) - $t0

    # 6) Analisis del log nuevo (solo lo escrito durante esta ejecucion), en flujo
    $rCiclos = 0; $rRobados = 0; $rNaN = 0; $rCaida = 0; $rFin = ''; $caidas = 0; $nan = 0; $pool = 0; $noSePudo = 0; $reactivados = 0; $partida = @(); $cola = New-Object 'System.Collections.Generic.Queue[string]'
    if (Test-Path $log) {
        $fs = [IO.File]::Open($log, 'Open', 'Read', 'ReadWrite')
        try {
            if ($fs.Length -lt $logIni) { $logIni = 0 }
            $null = $fs.Seek($logIni, 'Begin')
            $sr = New-Object IO.StreamReader($fs, [Text.Encoding]::UTF8)
            while ($null -ne ($ln = $sr.ReadLine())) {
                if ($ln.Contains('a través del mapa')) { $caidas++ }
                if ($ln.Contains('NaN')) { $nan++ }
                if ($ln.Contains('[ROBAR] ciclo ') -and $ln.Contains('robado=')) { $rCiclos++; if ($ln.Contains('robado=si')) { $rRobados++ }; if ($ln.Contains('nan=SI')) { $rNaN++ }; if ($ln.Contains('z_bajo=SI')) { $rCaida++ } }
                if ($ln.Contains('[ROBAR] FIN')) { $rFin = $ln }
                if ($ln.Contains('Pool.h:207')) { $pool++ }
                if ($ln.Contains('No se pudo reactivar') -or $ln.Contains('No se pudo desactivar')) { $noSePudo++ }
                if ($ln.Contains('Hook reactivado') -or $ln.Contains('Hook desactivado')) { $reactivados++ }
                if ($partida.Count -lt 10 -and ($ln.Contains('La partida es') -or $ln.Contains('La ranura de guardado') -or $ln.Contains('Cargando la partida'))) { $partida += $ln }
                $cola.Enqueue($ln); if ($cola.Count -gt 300) { [void]$cola.Dequeue() }
            }
        } finally { $fs.Dispose() }
        $cola | Set-Content -LiteralPath (Join-Path $dirSalida 'ultimas-300-lineas-log.txt') -Encoding utf8
    }
    $partidasDespues = @{}
    if (Test-Path $uf) { Get-ChildItem $uf -Filter 'GTASAsf*.b' | ForEach-Object { $partidasDespues[$_.Name] = Hash $_.FullName } }
    $partidasIguales = ((($partidasAntes.Keys + $partidasDespues.Keys) | Sort-Object -Unique | Where-Object { $partidasAntes[$_] -ne $partidasDespues[$_] }).Count -eq 0)

    $resumen = [ordered]@{
        variante = $Variante; descripcion = $descr; hora_inicio = $t0.ToString('s')
        argumentos = ($args_ -join ' '); asi_sha256 = $hashAsi; ini_usado = (Split-Path $ini -Leaf)
        duracion_segundos = [math]::Round($dur.TotalSeconds, 1); codigo_salida = "$codigo"
        avisos_caida_a_traves_del_mapa = $caidas; lineas_con_NaN = $nan; lineas_Pool_h_207 = $pool; lineas_no_se_pudo = $noSePudo; lineas_hook_cambiado = $reactivados
        lineas_de_carga_de_partida = $partida
        partidas_sin_cambios_tras_jugar = $partidasIguales
        robar_ciclos = $rCiclos; robar_robados = $rRobados; robar_ciclos_con_NaN = $rNaN; robar_ciclos_con_z_bajo = $rCaida; robar_fin = $rFin
        simulado = [bool]$Simular
    }
}
finally {
    # 7) Restaurar SIEMPRE
    try {
        if (Test-Path -LiteralPath (Join-Path $dirSalida 'respaldo\respaldo.json')) {
            $restaurado = $false
            $restaurado = Restaurar-Desde (Join-Path $dirSalida 'respaldo')
            if ($restaurado -and (Test-Path $pendiente)) { Remove-Item -Recurse -Force $pendiente }
        }
    } catch { Write-Warning "Error al restaurar: $_  (la proxima ejecucion lo intentara de nuevo; o: jugar.ps1 -Restaurar)" }
    if ($tengoCandado) {
        Remove-Item -LiteralPath (Join-Path $candado 'owner_bis1') -Force -ErrorAction SilentlyContinue
        Remove-Item -LiteralPath $candado -Force -ErrorAction SilentlyContinue
    }
    if ($resumen.Count) {
        $resumen['restauracion_verificada_por_hash'] = $restaurado
        $resumen | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $dirSalida 'resumen.json') -Encoding utf8
        $txt = $resumen.GetEnumerator() | ForEach-Object { '{0}: {1}' -f $_.Key, (($_.Value) -join ' | ') }
        $txt | Set-Content -LiteralPath (Join-Path $dirSalida 'resumen.txt') -Encoding utf8
        Write-Host ''; Write-Host '== Resumen' -ForegroundColor Cyan; $txt | ForEach-Object { Write-Host "   $_" }
        Write-Host "   Carpeta: $dirSalida"
    }
    if ($restaurado) { Write-Host 'Copia de pruebas restaurada y verificada por hash.' -ForegroundColor Green }
    else { Write-Host 'ATENCION: la restauracion no se pudo verificar. Ejecuta: pwsh -File jugar.ps1 -Restaurar' -ForegroundColor Red }
}
