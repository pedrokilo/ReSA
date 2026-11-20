# Ejecutar el banco FnS7A1A (solo el agente principal: lanza el juego)

Comando de una linea (desde `D:\Original\wv-12`; bash; toma `gta_lock` y el candado de compilacion; compila la sonda, corre 21 x 16384 pares, y si aprueba hace las dos rondas de prueba negativa de 2048 casos y recompila con las fuentes buenas):

```
cd /d/Original/wv-12 && KEEP_LOG= bash reversing/FnS7A1A/chainS7A1A.sh
```

Seguimiento: `reversing/FnS7A1A/chain.out`. Solo la corrida principal (sin rondas negativas): sustituir el ultimo paso por `python -B reversing/FnS7A1A/run_local.py` tras `python reversing/FnS7A1A/build_probe.py`.

Si aprueba (`APROBADO` en `reversing/FnS7A1A/run_main.txt`):

```
python reversing/FnS7A1A/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fns7a1a/<AAAAMMDD-HHMMSS-micros>"
```

(pone `validated=1` en las 21 filas de `docs/hooks.csv`, archiva la corrida y escribe `validation-summary.json`). Antes revisar `scope_note.txt`.

Comprobaciones previas sin juego (ya pasadas): `python -I reversing/FnS7A1A/emu_check.py 2048` (EXE emulado frente al oraculo, 43008 casos sin diferencias), `python -I reversing/FnS7A1A/fixture_selfcheck.py` (344064 casos por el oraculo, cobertura de ramas >= 50) y `python -I reversing/FnS7A1A/mutate.py check` (las 42 mutaciones apuntan a un texto unico).

## Notas
- `build_probe.py` instala el ASI de la sonda en la copia de pruebas (`_baseline`): es el comportamiento de todas las sondas; no se ejecuto aqui.
- Cada mutacion de `mutate.py` esta condicionada a un objeto o puntero desalineado, que el juego real no produce; las rondas negativas esperan **21 de 21 funciones con diferencias** en ambas rondas.
- Si una funcion falla en la corrida principal, `run_local.py` muestra la primera diferencia (`fixture_oracle.py` dice si discrepa el EXE o la DLL). Funciones con mas riesgo: #11 `FillCallback` (orden de relectura del byte +0x59 y division) y #19/#20 (eventos de los colaboradores).

## Bancos anteriores que citan estas direcciones (`bancos-citan.txt`)
Al sustituir la direccion del EXE por la funcion propia, cambian los punteros que guardan algunos objetos; repetir (o canonizar el puntero) los bancos de cada direccion:
- 0x4E7B80 (`CAEMFDecoder::Initialise` ahora guarda la funcion de la DLL): `MFDec`.
- 0x61DAE0: `FightProcessPed31`, `Fn85I`. 0x61FD10 y 0x61FF30: `Fn206A`. 0x621BE0: `LeaveCar67`. 0x675DD0: `Fn204A`. 0x6D2950: `CVehC`.
- 0x4D1390: unos 20 bancos de tareas y animaciones (lista completa en `bancos-citan.txt`): ahora el callback por defecto es `CDefaultAnimCallback::DefaultAnimCB` de la DLL (con hook en 0x4D1390); los bancos que comparaban el puntero guardado con 0x4D1390 deben canonizar.
- 0x749000..0x7490F0: `Fn226A` (`RpAnisotPluginAttach` ahora registra las funciones propias en lugar de 0x749000...: el banco ya falsea `RwEngineRegisterPlugin` & co. y compara los argumentos, que cambian de la direccion del EXE a la de la DLL; canonizar o repetir).
- 0x734850, 0x735140, 0x7352D0, 0x4637C0, 0x4CF9E0: ningun banco las cita.
