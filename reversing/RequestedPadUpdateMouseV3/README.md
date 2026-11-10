<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9938 funciones reconstruidas; 9938 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPadUpdateMouseV3 (CPad::UpdateMouse 0x53F3C0)

## Estado: REPETIBLE y APROBADO (10/11/2026, caso CASO-CONGELADOS)

`bash reversing/_handoff/bench_here.sh RequestedPadUpdateMouseV3` compila la sonda (con la prueba estatica de las rutas de llamada) y corre el banco: **9.216 pares EXE/DLL, cero diferencias y cero discrepancias con el modelo (`model.py`)**, salida natural, restauracion exacta de 17.692+ rangos de hooks, tablas, banderas y reloj. Corrida `20261110-050129-866358` (`E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pad-update-mouse-v3/`), archivada en `reversing/CppTwelveLevel6Final/runs/RequestedPadUpdateMouseV3-20261110-050129-866358`; resumen en `validation-summary.json`. SHA256 de la sonda ejecutada: `20e0bc205cabcffad3f194a5f8480ffecdd882639d5b12a5c57b30728aad23fc`.

### Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Tras los lotes posteriores (`CEventInteriorUseInfo` con hooks virtuales) el preflight volvia a parar con `unknown_vtable_category CEventInteriorUseInfo` (falta `{"CEventInteriorUseInfo",{0x008701A8,16}}` en `VTableRanges.hpp`). Arreglo solo en el banco: `reversing/_handoff/vtable_ranges_sync.py --apply` (extrae direccion y ranuras de todos los `RH_ScopedVirtualClass` de `source/`, incluidas clases plantilla) y `refreeze_generic.py` para el congelado. Repetido con `bench_here.sh`: **9216 pares, cero diferencias**, corrida `20261110-105856-006252` (archivada en `reversing/CppTwelveLevel6Final/runs/RequestedPadUpdateMouseV3-20261110-105856-006252`; `validation-summary.json` regenerado con `register_run.py`). Sin cambios de produccion. (Un primer intento, `20261110-105647-327874`, fallo en el preflight con `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` porque la primera version del guion de sincronizacion no leia las filas de clases plantilla y la elimino del fichero; corregido y verificado que no se pierde ninguna fila.)

### Por que estaba "congelado y no repetible" (Fn135A) y que se hizo

El paquete se preparo y congelo el 08/10/2026 (`prepared-freeze.json`) y sus dos corridas del mismo dia (`runs/20261008-*`, V3 incluido) terminaron en FALLO con 0 comparaciones. Causas, todas de arnes salvo la ultima, y arreglos (solo en el banco, salvo el punto 7):
1. **Hashes del congelado.** `prepared-freeze.json` guardaba los SHA256 de los ficheros con fin de linea CRLF (copia de trabajo de Windows) y de las versiones de octubre de `reversing/ProbeRuntime.hpp` y `reversing/runtime_runner.py`. Git guarda LF (`* text=auto eol=lf`), asi que en cualquier copia limpia `build_probe.py` abortaba antes de compilar. Nuevo `refreeze.py`: recalcula los hashes (anota fecha y causa; no toca corpus, modelo ni alcance).
2. **Metadatos de vtables.** `VTableRanges.hpp` no tenia las 16 clases `CEvent*` con hooks virtuales registradas despues del 08/10 (`CEventChatPartner`, `CEventDeadPed`, `CEventDraggedOutCar`, `CEventFireNearby`, `CEventGotKnockedOverByCar`, `CEventGroupEvent`, `CEventGunAimedAt`, `CEventInWater`, `CEventPedEnteredMyVehicle`, `CEventPedToChase`, `CEventPedToFlee`, `CEventPotentialGetRunOver`, `CEventRevived`, `CEventSexyPed`, `CEventSexyVehicle`, `CEventSpecial`) y el preflight paraba con `unknown_vtable_category`. Anadidas con direccion y numero de ranuras sacados de `RH_ScopedVirtualClass` (la misma extraccion reproduce las 450 filas existentes sin diferencias).
3. **`RsGlobal.ps` nulo.** El preflight corre al principio de `NOTSA_WinMain`, antes de `psInitialize` (que fija `RsGlobal.ps = &PsGlobal`); el EXE y la DLL leen `PSGLOBAL(diMouse)` a traves de ese puntero y el primer caso que pasaba de `IsForegroundApp` terminaba en 0xC0000005. `LiveComparison.hpp` apunta `RsGlobal.ps` (0xC17054) a la estructura estatica (0xC8CF88) y lo restaura con el resto de globales; ademas instala `ReversingProbe::InstallFailureCapture` (un acceso invalido deja traza).
4. **Prueba estatica de rutas.** `Diagnostics/verify_actual_calls.py` contaba como llamadas las lineas de continuacion de dumpbin y los datos que siguen al ultimo `ret` (tabla de marcos de las comprobaciones RTC, que dumpbin decodifica como `call`), y fallaba segun el diseno de la compilacion; ahora solo cuenta lineas `call` hasta el ultimo `ret`. Esa prueba y `inspect_compiled.py` se generan desde `build_probe.py` (antes eran un paso manual aparte), de modo que `bench_here.sh` basta.
5. **Constante del analizador.** `run_local.py` exigia `snapshot_ranges == 17692`; ahora son 20.466 con 9.819 hooks. Se exige `snapshot_ranges >= 2 * hooks_snapshotted` (una entrada del EXE y otra de la DLL por hook como minimo) y se sigue exigiendo `changed_ranges == []`.
6. **Compilacion en serie.** Respeta `_CL_` (compilacion con `-MP1` bajo los candados) en lugar de forzar `/MP6`.
7. **Defecto real de produccion encontrado y corregido** (`source/game_sa/Pad.cpp`, `CPad::UpdateMouse`). Con las lecturas satisfactorias (rutas 3 y 4: 3.072 de los 9.216 pares) la DLL difería del EXE en dos bytes: el byte de relleno +7 de `NewMouseControllerState` y de `OldMouseControllerState`. El EXE (0x53F4E9-0x53F509) copia los estados con dos `rep movs` de 20 bytes (`Old = New`, `New = Temp`), relleno incluido; `OldMouseControllerState = std::exchange(NewMouseControllerState, TempMouseControllerState)` es una asignacion miembro a miembro que no lo copia. Nadie lee ese byte, pero es una diferencia de bytes con el EXE (`PadMouse1` la documento el 09/10 como relleno que "el compilador no copia" y la enmascara a cero en su comparacion; aqui se compara sin mascara). Arreglo minimo: dos `std::memcpy` de `sizeof(CMouseControllerState)`. Tras el cambio se repitieron los bancos que citan `CPad::UpdateMouse`: `UnlockInput` (6.144 pares) y `PadMouse1` (9.216 pares), ambos aprobados.

Alcance, lo que cubre y lo que no, y comandos: sin cambios respecto a la seccion "Historia" de abajo (el corpus, el modelo y los falsos son los de V2 con las correcciones de V3). `refreeze.py` debe ejecutarse tras tocar cualquier fichero congelado.

### Historia (texto original de V3, 08/10/2026)

Preparado, congelado, NO compilado y NO ejecutado.

V3 reemplaza al banco `RequestedPadUpdateMouseV2`, que quedó RECHAZADO y congelado
(`reversing/RequestedPadUpdateMouseV2/runs/20261008-232556-013193`). Ese fallo se debió a dos
defectos del arnés en `LiveComparison.hpp`, no a la función reversada.

## Defectos del arnés corregidos

1. `Update()` ejecutaba `check->LockState(false); check->State(false)` ANTES de calcular
   `checkOwn`/`checkBody`. Desenganchar reescribe nuestro thunk con `E9 -> 0x53F270`, de modo que
   `Body(checkOwn)` terminaba en 0x53F270 (nativo), `pureCandidateKnown` era falso,
   `helpers_known=false` y el bucle de 9216 casos se omitía (`comparisons=0`).
   Ahora `checkOwn` y `checkBody` se calculan antes de desenganchar (con el hook activo se lee el
   thunk intacto; si ya estaba desenganchado se decodifica `m_LibOriginalFunctionContent`).
2. `checkNativeBefore` (128 bytes de 0x53F270) se capturaba con el hook ya quitado, y la
   restauración final lo comparaba contra el estado reenganchado, dando falso negativo aunque la
   restauración fuera correcta. Ahora se captura además `checkHookedNative` (estado inicial, con
   hook) y es el que se usa en la comparación final. `checkNativeBefore` (desenganchado) se
   conserva para `checkUnchanged`.

## Alcance (idéntico a V2)

- 9.216 casos finitos y aislados en CPU de `CPad::UpdateMouse`; mismo corpus y mismo oráculo
  (`model.py`), sin cambios.
- Cubre: CPad completo + mouse, guards, dispositivo, preferencias, reloj; `CheckForInput` nativo
  real usado por ambos lados; identidad y ABI de helpers trazadas; puertas estrictas de FP y
  restauración de los 17692 rangos globales.
- NO cubre: NaN/Inf/INT_MIN, COM/DirectInput real, dispositivos reales, entrada real, integración
  con actores ni acreditación de los helpers.

## Renombres V2 -> V3

Carpeta, prefijo `pad-update-mouse-v3` (ASI/PDB, log `-runtime.jsonl`, harness, lock,
`resultados-*`). Fuentes de `source/` sin cambios. `prepared-freeze.json` regenerado.

## Comandos

```
python -B reversing/RequestedPadUpdateMouseV3/build_probe.py
python -B reversing/RequestedPadUpdateMouseV3/inspect_compiled.py
python -B reversing/RequestedPadUpdateMouseV3/Diagnostics/verify_actual_calls.py
python -B reversing/RequestedPadUpdateMouseV3/run_local.py
```
