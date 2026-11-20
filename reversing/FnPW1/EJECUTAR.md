# Cómo ejecutar el banco FnPW1 (CVehicle::ProcessWheel, 0x6D6C00)

Estado: ya ejecutado en esta sesión (corrida sin corrección: 5401 pares distintos; con la corrección: 20480 pares, cero diferencias; rondas negativas 1 y 2 fallan; ver README.md). Este documento solo explica cómo repetirlo. Ejecutarlo lanza `gta_sa.exe` de la copia de pruebas (`E:\JUEGOS\GTA San Andreas - CVehicle Test`); nunca el juego de `E:\JUEGOS\GTA San Andreas`.

Reglas del lanzamiento (ya comprobadas en `run_local.py` y `runtime_runner.run_case`):
- Sin `--unhook-all`: el runner usa `--unhook=Audio,Global/SurfaceInfos_c`, de modo que `RenderWare/WindowedModeHooks` queda siempre encendido (modo ventana; no cambia la resolución de los monitores).
- El runner cierra el juego con `process.terminate()` (nunca `taskkill`) y aborta si GTA queda sin progreso, con un diálogo o "No responde" 15 s sin que crezca el registro.
- No debe haber otro `gta_sa.exe` abierto (`reversed-tests.lock`).

## Paso 1 (solo si falta la sonda instalada): compilar la sonda con el candado de compilación

```bash
sh /d/Original/_build_PW1b.sh /d/Original/_PW1_fix.patch
```
(o sin el parche si la corrección ya está en `wv-int`: `sh /d/Original/_build_PW1b.sh`; el parche `_PW1_fix.patch` solo se aplica mientras dura la compilación)
(toma `/d/Original/compile_lock`, compila `wv-int` con 1 hilo, instrumenta `Game.cpp` con `PW1Live::Update()` y lo restaura; instala `_baseline/gta_reversed-fnpw1-probe.asi` en la copia de pruebas; resultado en `/d/Original/_build_PW1.log`, termina con `EXIT=0`.)
La carpeta `reversing/FnPW1` debe estar copiada en `wv-int` (el script compila `wv-int`): `cp -r /d/Original/wv-10/reversing/FnPW1/. /d/Original/wv-int/reversing/FnPW1/`.

## Paso 2: lanzar el banco (una línea)

PowerShell:
```powershell
cd D:\Original\wv-int; python -B reversing\FnPW1\run_local.py
```
bash:
```bash
cd /d/Original/wv-int && python -B reversing/FnPW1/run_local.py
```
Escribe el informe en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fnpw1\<AAAAMMDD-HHMMSS-micros>\` (`summary.json`, `RESULTADO.txt`, `comparisons.jsonl`). `APROBADO` = 0 diferencias, 20480 pares, cada lado igual al oráculo.

Si falla, ver las diferencias con el oráculo (sin juego): `python -B reversing/FnPW1/fixture_oracle.py "<carpeta>/comparisons.jsonl"`.

## Paso 3 (solo si aprobó): prueba negativa (2 rondas) y registro

Cada ronda N (1 y 2): `sh /d/Original/_build_PW1b.sh /d/Original/_PW1_fix.patch N` (aplica el parche, las mutaciones de `mutate.py`, compila y lo restaura todo) → `python -B reversing/FnPW1/run_local.py` (debe FALLAR) → recompilar la sonda buena (`_build_PW1b.sh` sin ronda) y repetir el banco.
Después, con la corrida aprobada de la sonda buena: `python reversing/FnPW1/register_hook.py "<carpeta de la corrida aprobada>"` (la fila de `docs/hooks.csv` ya traía `validated=1`; `sync_validated_flags.py` reescribe miles de filas del catálogo, no ejecutarlo para esto).
