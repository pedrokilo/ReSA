#requires -Version 7
<#
.SYNOPSIS
  Asistente de biseccion interactiva (BIS1): aisla que funcion(es) reconstruida(s) provocan el fallo
  "mundo en blanco / texto de mision fijo / Cargando... / posicion NaN / entidades a z=-100 / ha caido a traves del mapa".

.DESCRIPTION
  Cada ronda apaga (usa el codigo original del EXE) un grupo de rutas Categoria/Clase (despues, de funciones) con
  --unhook-file, lanza el juego en la COPIA de pruebas (copia de seguridad y restauracion por hash de ASI/PDB/INI),
  te deja jugar y decide solo si hubo error. El estado se guarda en estado.json: Ctrl+C y vuelve a ejecutar para seguir.

.EXAMPLE
  pwsh -File biseccion.ps1              # modo interactivo (juegas tu)
  pwsh -File biseccion.ps1 -Auto        # modo automatico: la DLL roba coches sola (--robar), sin intervencion
  pwsh -File biseccion.ps1 -Restaurar   # solo restaura la copia de pruebas
  pwsh -File biseccion.ps1 -Simular     # prueba todo el circuito SIN jugar (oraculo simulado)
  pwsh -File biseccion.ps1 -Reiniciar   # empieza de cero (guarda el estado anterior como estado.json.bak)
#>
[CmdletBinding()]
param(
    [switch]$Auto,                 # la DLL roba coches sola (--robar) y la decision es automatica
    [switch]$Simular,              # sin juego: oraculo simulado + jugar.ps1 -Simular (copia/restauracion/argumentos reales)
    [switch]$Restaurar,
    [switch]$Reiniciar,
    [string]$Estado = '',
    [int]$MaxSegundos = 300,       # el juego se cierra solo a los N segundos (interactivo: 5 minutos)
    [string]$Robar = '20,15,15,5', # -Auto: ciclos,periodo_s,espera_s,conducir_s
    [int]$MinRobosAuto = 12,       # -Auto: robos conseguidos para que una ronda sin error cuente como OK
    [int]$MinSegundos = 60,        # una ronda mas corta sin senal de error no cuenta
    [string]$SimCulpables = '',    # -Simular: culpables ocultos (rutas separadas por coma); vacio = 2 al azar
    [double]$SimRuido = 0.10,
    [string]$Carpeta = 'E:\JUEGOS\GTA San Andreas - CVehicle Test'
)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$jugar = Join-Path $aqui 'jugar.ps1'
$motor = Join-Path $aqui 'ddmin.py'
if (-not $Estado) { $Estado = Join-Path $aqui $(if ($Simular) { 'estado_simulado.json' } else { 'estado.json' }) }
$rondasDir = Join-Path $aqui 'rondas'
New-Item -ItemType Directory -Force -Path $rondasDir | Out-Null
$ErrCrash = @('-1073741819', '3221225477')   # 0xC0000005

function Py { & python -I $motor @args }   # Py @arreglo
function PyJson { $o = Py @args; if ($LASTEXITCODE -ne 0) { throw "ddmin.py fallo: $o" }; $o | ConvertFrom-Json }
function Jugar([string[]]$a) { & pwsh -NoProfile -ExecutionPolicy Bypass -File $jugar @a 2>&1 }

if ($Restaurar) { Jugar @('-Restaurar') | ForEach-Object { Write-Host $_ }; return }
if ($Reiniciar -and (Test-Path $Estado)) { Move-Item -Force $Estado ($Estado + '.bak'); Write-Host "Estado anterior guardado en $Estado.bak" -ForegroundColor Yellow }
if (-not (Test-Path $Estado)) {
    $r = PyJson $Estado 'iniciar'
    Write-Host ("Estado nuevo: {0} grupos candidatos ({1} excluidos: arranque/ventana/memoria y clases sin hooks). Se esperan unas {2} rondas por fase." -f $r.candidatos, $r.excluidas, $r.estimadas) -ForegroundColor Cyan
}

# DLL: la de BIS1 (con --unhook-file y --robar) si existe; en -Simular se admite la original para probar el circuito
$dllBis = Join-Path $aqui 'dll\Debug.asi'
$variante = 'A'
if (-not (Test-Path $dllBis)) {
    if ($Simular) { $variante = 'O'; Write-Host 'Aviso: no hay dll\Debug.asi de BIS1; el simulacro usa la DLL original (O).' -ForegroundColor Yellow }
    else { throw "Falta $dllBis (la DLL con --unhook-file/--robar). Ver README, seccion Compilacion." }
}

# ---------- simulacro: oraculo oculto ----------
$simC = @(); $rngSim = [System.Random]::new(4242)
function Oraculo-Simulado($offRutas, $U) {
    $on = @($simC | Where-Object { $offRutas -notcontains $_ })
    if ($on.Count -gt 0 -and $rngSim.NextDouble() -ge $SimRuido) { return 'E' }
    return 'O'
}

function Pedir-Tecla($texto, $validas) {
    Write-Host $texto -ForegroundColor Yellow
    while ($true) {
        $k = [Console]::ReadKey($true).KeyChar.ToString().ToLower()
        if ($validas -contains $k) { Write-Host "  -> $k"; return $k }
    }
}

function Escribir-Resultado($st) {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $dllBis -ErrorAction SilentlyContinue).Hash
    $rondas = @($st.rondas | Where-Object { $_.resultado -in @('E', 'O', 'D') })
    $l = New-Object System.Collections.Generic.List[string]
    $l.Add('# Resultado de la biseccion BIS1'); $l.Add('')
    $l.Add("Generado: $(Get-Date -Format 's'). DLL de las pruebas: ``dll\Debug.asi`` SHA256 ``$hash``. Modo: $(if ($Auto) {'automatico (--robar)'} else {'interactivo'})$(if ($Simular) {' (SIMULACRO: resultado sin valor real)'}).")
    $l.Add(''); $l.Add('## Grupos culpables (Categoria/Clase)'); $l.Add('')
    if ($st.culpables_rutas.Count) { $st.culpables_rutas | ForEach-Object { $l.Add("- ``$_``") } } else { $l.Add('- (ninguno hallado)') }
    $l.Add(''); $l.Add('## Funciones culpables (clase::funcion, direccion del EXE)'); $l.Add('')
    if ($st.culpables_funciones.Count) {
        $l.Add('| Clase::funcion | Direccion | Ruta para `--unhook` |'); $l.Add('|---|---|---|')
        foreach ($f in $st.culpables_funciones) { $i = $st.info_funciones.$f; $l.Add("| $($i.clase)::$($i.funcion) | $($i.direccion) | ``$f`` |") }
    } else { $l.Add('- (no se llego al nivel de funcion)') }
    $l.Add(''); $l.Add('## Que hacer ahora (agente siguiente)'); $l.Add('')
    $l.Add('1. Para cada funcion culpable, comparar el codigo C++ con el EXE (desensamblado en la direccion indicada; fila en `docs/direcciones.csv`, definicion y registro en las dos ultimas columnas).')
    $l.Add('2. Corregir la divergencia y repetir la prueba con la DLL corregida: `pwsh -File biseccion.ps1 -Auto` no hace falta; basta una ronda con esas funciones encendidas.')
    $l.Add('3. Si el culpable cambia de una ejecucion a otra, hay interaccion entre funciones: apagarlas juntas confirma el conjunto (esta lista es MINIMA para eliminar el fallo, no necesariamente cada una por separado).')
    $l.Add(''); $l.Add("## Historial de rondas ($($rondas.Count))"); $l.Add('')
    $l.Add('| Ronda | Fase | Apagados | Resultado | Duracion s | Caidas | Robos | Decision | Hora |'); $l.Add('|---|---|---|---|---|---|---|---|---|')
    foreach ($r in $rondas) { $l.Add("| $($r.n) | $($r.fase) | $($r.n_apagados) | $($r.resultado) | $($r.duracion) | $($r.caidas) | $($r.robos) | $($r.decision) | $($r.hora) |") }
    $l.Add(''); $l.Add('Resultado: E = el fallo sigue (ERROR); O = el fallo no aparecio (OK, solo con >=3 robos confirmados); D = descartada.')
    Set-Content -LiteralPath (Join-Path $aqui $(if ($Simular) { 'resultado_simulado.md' } else { 'resultado.md' })) -Value $l -Encoding utf8
}

# ---------- bucle principal ----------
$terminado = $false
try {
    if ($Simular) {
        $st0 = Get-Content -Raw $Estado | ConvertFrom-Json
        $simC = if ($SimCulpables) { @($SimCulpables -split ',') } else { @($st0.U | Get-Random -Count 2 -SetSeed 7) }
        Write-Host ("SIMULACRO: culpables ocultos = {0}; ruido de falsos negativos = {1:P0}" -f ($simC -join ', '), $SimRuido) -ForegroundColor Magenta
    }
    while (-not $terminado) {
        $sig = PyJson $Estado 'siguiente'
        if ($sig.accion -eq 'terminado') { break }
        if ($sig.accion -eq 'fin') {
            if ($sig.culpables.Count -eq 0 -or $sig.motivo -like 'NO CONFIRMADO*' -or $sig.motivo -like 'con todo*' -or $sig.motivo -like 'demasiadas*') {
                Write-Host "La busqueda no pudo concluir: $($sig.motivo)" -ForegroundColor Red; break
            }
            Write-Host ("`n== Fase '{0}' terminada ({1}). Culpables:" -f $sig.fase, $sig.motivo) -ForegroundColor Green
            $sig.culpables | ForEach-Object { Write-Host "   $_" }
            $av = PyJson $Estado 'avanzar'
            if ($av.hay_fase_siguiente) {
                Write-Host ("`nBajando al nivel de FUNCION: {0} funciones en esas clases." -f $av.n) -ForegroundColor Cyan
                if ($Simular) { $st1 = Get-Content -Raw $Estado | ConvertFrom-Json; $simC = @($st1.U | Get-Random -Count 1 -SetSeed 3); Write-Host ('SIMULACRO: culpable oculto a nivel de funcion = ' + ($simC -join ', ')) -ForegroundColor Magenta }
                if ($av.n -le 1) { $av = PyJson $Estado 'siguiente' }
                continue
            }
            $terminado = $true; break
        }
        $n = $sig.ronda
        $ruta = Join-Path $rondasDir ('r{0:D3}.unhook.txt' -f $n)
        [void](Py $Estado 'unhook' $ruta)
        $af = Join-Path $rondasDir ('r{0:D3}.args' -f $n)
        $linea = if ($sig.apagar.Count) { "--unhook-file=$ruta" } else { '' }
        Set-Content -LiteralPath $af -Value $linea -Encoding ascii
        Write-Host ''
        Write-Host ("== Ronda {0} de ~{1}  (fase: {2}; apagados: {3}; {4})" -f $n, $sig.estimadas, $sig.fase, $sig.apagar.Count, $sig.motivo) -ForegroundColor Cyan
        if (-not $Auto -and -not $Simular) {
            Write-Host '   Que hacer: ROBA coches CON OCUPANTES junto a la casa de CJ (sal de la casa y roba el primero que veas).' -ForegroundColor Green
            Write-Host ("   Cuando salga el error (mundo en blanco, texto fijo, avisos) cierra la ventana; si no, el juego se cierra solo a los {0} s." -f $MaxSegundos) -ForegroundColor Green
            Write-Host '   Ctrl+C para parar; al volver a ejecutar sigue donde lo dejaste. La copia de pruebas se restaura siempre.' -ForegroundColor DarkGray
        } elseif ($Auto) {
            Write-Host '   Modo automatico: la DLL crea coches con conductor y los roba sola (nada que hacer).' -ForegroundColor Green
        }
        $ja = @('-Variante', $variante, '-Etiqueta', ('-r{0:D3}' -f $n), '-ArgsFile', $af)
        if ($Simular) { $ja += '-Simular' }
        else {
            $ja += @('-Auto', $(if ($Auto) { [math]::Max($MaxSegundos, 480) } else { $MaxSegundos }), '-Sonda')
            if ($Auto) { $ja += @('-TpExit', '-Robar', $Robar) } else { $ja += @('-ParaAlCaer', '-MinCaidas', 3) }
        }
        $t0 = Get-Date
        while ($true) {
            $salida = Jugar $ja
            $txt = ($salida | Out-String)
            if ($txt -match 'esta tomado por otra prueba' -or $txt -match 'ya esta en marcha') {
                Write-Host '   Esperando a que otra prueba libere el juego (candado)...' -ForegroundColor Yellow; Start-Sleep -Seconds 20; continue
            }
            break
        }
        $dir = Get-ChildItem (Join-Path $aqui 'salidas') -Directory -Filter "$variante-r$('{0:D3}' -f $n)-*" | Sort-Object Name | Select-Object -Last 1
        if (-not $dir -or -not (Test-Path (Join-Path $dir.FullName 'resumen.json'))) { Write-Host $txt; throw 'jugar.ps1 no dejo resumen.json' }
        $r = Get-Content -Raw (Join-Path $dir.FullName 'resumen.json') | ConvertFrom-Json
        if (-not $r.restauracion_verificada_por_hash) { Write-Host 'ATENCION: la restauracion no se verifico por hash; ejecuta -Restaurar.' -ForegroundColor Red; throw 'restauracion no verificada' }
        $dur = [double]$r.duracion_segundos; $caidas = [int]$r.avisos_caida_a_traves_del_mapa
        $crash = $ErrCrash -contains "$($r.codigo_salida)"
        $sonda = Join-Path $dir.FullName 'sonda_pool.txt'
        $nanPos = (Test-Path $sonda) -and ((Select-String -LiteralPath $sonda -Pattern 'pos=(-?nan|nan)' -Quiet) -or (Select-String -LiteralPath $sonda -Pattern 'pos=[^ ]*nan' -Quiet))
        $robos = [int]$r.robar_robados
        $nanDll = [int]$r.robar_ciclos_con_NaN; $zBajo = [int]$r.robar_ciclos_con_z_bajo
        $extra = @("duracion=$dur", "caidas=$caidas", "robos=$robos", "crash=$([int]$crash)", "nan=$([int]($nanPos -or $nanDll -gt 0))", "salida=$($dir.Name)")
        $res = $null; $decision = ''
        if ($Simular) {
            $off = if (Test-Path $ruta) { @(Get-Content $ruta | Where-Object { $_ }) } else { @() }
            # comprobacion extremo a extremo: los argumentos que vio jugar.ps1 contienen el archivo de la ronda
            $argsOk = (-not $linea) -or ($r.argumentos -like "*$linea*")
            if (-not $argsOk -or $r.simulado -ne $true) { throw "Argumentos de la ronda incorrectos: '$($r.argumentos)' (esperado '$linea')" }
            $res = Oraculo-Simulado $off $null; $decision = 'simulada'
            Write-Host ("   [simulacro] apagados={0} args='{1}' ASI={2} restauracion OK -> {3}" -f $off.Count, $r.argumentos, $r.asi_sha256.Substring(0, 12), $res)
        }
        elseif ($Auto -and ($nanPos -or $nanDll -gt 0 -or $zBajo -gt 0)) {
            # Modo automatico: SOLO la senal neutral (z < -50 y NaN del jugador/coche/matriz, medidos por la sonda y por AutoSteal).
            # Los avisos "ha caido a traves del mapa", Pool.h y el crash dependen de codigo de la DLL y no valen como senal.
            $res = 'E'; $decision = 'automatica-neutral'
            Write-Host ("   ERROR (senal neutral): ciclos con z bajo={0}, con NaN={1}, NaN en la sonda={2}; duracion {3:N0} s." -f $zBajo, $nanDll, [int]$nanPos, $dur) -ForegroundColor Red
        }
        elseif (-not $Auto -and ($caidas -ge 3 -or $nanPos -or $nanDll -gt 0 -or $zBajo -gt 0 -or $crash)) {
            $res = 'E'; $decision = 'automatica'
            $mot = @(); if ($caidas -ge 3) { $mot += "$caidas avisos de caida" }; if ($nanPos -or $nanDll) { $mot += 'posicion NaN' }; if ($zBajo) { $mot += 'z < -50' }; if ($crash) { $mot += 'crash 0xC0000005' }
            Write-Host ("   ERROR detectado automaticamente ({0}); duracion {1:N0} s." -f ($mot -join ', '), $dur) -ForegroundColor Red
        }
        elseif ($dur -lt $MinSegundos -and -not $Auto) {
            $res = 'D'; $decision = 'corta'
            Write-Host ("   Ronda de {0:N0} s (< {1} s) sin senal de error: no cuenta; se repite." -f $dur, $MinSegundos) -ForegroundColor Yellow
        }
        elseif ($Auto) {
            if ($robos -ge $MinRobosAuto) { $res = 'O'; $decision = 'automatica'; Write-Host ("   Sin error con {0} robos automaticos en {1:N0} s: OK." -f $robos, $dur) -ForegroundColor Green }
            else { $res = 'D'; $decision = 'pocos robos'; Write-Host ("   Solo {0} robos automaticos (minimo {1}): ronda descartada." -f $robos, $MinRobosAuto) -ForegroundColor Yellow }
        }
        else {
            $k = Pedir-Tecla '   Sin senal automatica. ¿Robaste al menos 3 coches con ocupantes y viste el error? [s=error / n=sin error (y robe >=3) / d=descartar la ronda]' @('s', 'n', 'd')
            $res = @{ s = 'E'; n = 'O'; d = 'D' }[$k]; $decision = 'usuario'
        }
        $argvRes = @($Estado, 'resultado', $res) + $extra + @("decision=$decision")
        [void](Py @argvRes)
        Write-Host ("   Ronda {0}: {1} ({2}). Estado guardado en {3}." -f $n, @{ E = 'ERROR (el fallo sigue)'; O = 'OK (sin fallo)'; D = 'descartada' }[$res], $decision, (Split-Path $Estado -Leaf)) -ForegroundColor DarkCyan
    }
}
finally {
    # Siempre dejar la copia de pruebas como estaba (jugar.ps1 ya lo hace; esto cubre un corte entre medias)
    $o = Jugar @('-Restaurar'); if ($o -match 'RESTAURACION FALLIDA|No se pudo') { Write-Host ($o | Out-String) -ForegroundColor Red }
}

$st = Get-Content -Raw $Estado | ConvertFrom-Json
if ($st.terminado -or $terminado) {
    Escribir-Resultado $st
    Write-Host ''
    Write-Host '=== RESULTADO ===' -ForegroundColor Green
    Write-Host 'Grupos culpables:'; $st.culpables_rutas | ForEach-Object { Write-Host "   $_" }
    Write-Host 'Funciones culpables (clase::funcion, direccion):'
    foreach ($f in $st.culpables_funciones) { $i = $st.info_funciones.$f; Write-Host ("   {0}::{1}  {2}" -f $i.clase, $i.funcion, $i.direccion) }
    Write-Host ("Rondas jugadas: {0}. Detalle en resultado.md" -f @($st.rondas | Where-Object { -not $_.heredada }).Count)
} else {
    Write-Host 'Busqueda sin terminar: vuelve a ejecutar este script para continuar (o -Reiniciar).' -ForegroundColor Yellow
}
