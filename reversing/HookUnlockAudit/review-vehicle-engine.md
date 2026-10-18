<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de locks: vehículos, scripts, colisión y subsistemas

Fecha: 10/10/2026. Alcance: las 178 filas `locked=1` catalogadas para estas 12 clases. No se ha ejecutado el juego, compilado, alterado una fuente ni retirado un lock en esta revisión.

**Hallazgo:** los registros actuales de estas 178 funciones carecen de `.locked=true`. `HookInstallOptions` en `source/reversiblehooks/ReversibleHooks.h:103-106` inicializa `reversed=true`, `enabled=true`, `locked=!reversed` (false). Los comentarios generados y `docs/hooks.csv` son históricos; no demuestran que exista actualmente ese bloqueo dentro del juego. Hay que obtener el inventario runtime antes de cambiar un solo flag del catálogo.

El CSV adyacente contiene una fila por hook: registro exacto y línea, dirección EXE resuelta incluso para virtuales, evidencia histórica, número de callsites directos del mapa binario, riesgos y plan de conmutación. Los callsites son del export existente; no incluyen despacho indirecto y no constituyen una revisión exhaustiva de ABI.

| Clase | Filas catalogadas bloqueadas | Riesgo específico |
|---|---:|---|
| CWeapon | 6 | estado ped/cámara, daño, RNG, raycast, referencias y creación de FX |
| CProjectileInfo | 11 | pool de proyectiles, referencias entidades y ownership de FX |
| CGenericGameStorage | 14 | handle de fichero, buffer global, offsets/checksum y estado de carga/guardado |
| CCollision | 59 | buffers temporales, cache de planos, colmodels y llamadas anidadas de collision |
| CClothesBuilder | 8 | refcounts RenderWare, geometría/skin, buffers globales y ownership de texturas |
| CCarEnterExit | 5 | tareas y asociaciones anim, puertas/pasajeros, referencias; ABI de llamadores EXE |
| CTrain | 20 | nodos de pista, lista enlazada de vagones, pools y referencias pasajeros |
| CPlane | 9 | layout CPlane/CAutomobile, tren de aterrizaje, controles, streaming y FX |
| CMonsterTruck | 6 | layout heredado, arrays de ruedas y suspensión, despacho virtual a CAutomobile |
| CBike | 18 | layout CBike/CVehicle, suspensiones, referencias ped y audio, despacho virtual híbrido |
| FxEmitterBP_c | 7 | listas partículas, manager/pool FX, estructuras RenderWare y estado render |
| CRunningScript | 15 | IP bytecode, parámetros globales, listas de scripts, tareas y referencias ped |

## Prueba requerida para cada fila

1. Leer `Locked()`/`Hooked()` del hook real, guardar bytes originales, dirección de cuerpo DLL antes de parchear y slot virtual. Registrar resultado de la API normal, sin forzar `LockState(false)` para declarar éxito.
2. Dentro del juego completo y con estado válido aislado, ejecutar ON→OFF→ON y ambas rutas de llamada. Combinar con dependencias activas y desactivadas; ningún patch del banco debe ocultar el hook auditado ni cambiar un cuerpo ya redirigido.
3. Verificar retorno/estado/traza por par, bytes/vtable tras cada transición, estado restaurado, no crash/hang y ningún resultado nulo. Incluir frames de continuación para recursos persistentes. Una comparación antigua no demuestra estas transiciones ni el estado híbrido con callers EXE.
4. Si el runtime ya informa lock=false, registrar discrepancia documental y la conmutación real; si informa lock=true, conservarlo hasta prueba específica segura. Todo fallo permanece visible en la tabla.

## Precauciones concretas

- `CCarEnterExit.cpp:35-51` conserva EDX mediante `ComputePassengerIndexFromCarDoor_KeepEdx`: el comentario documenta `SetPedInCarDirect` EXE en `0x650356` y `0x650360 push edx`. El flujo híbrido necesita probar puerta/pasajero; no retirar el wrapper.
- `FxEmitterBP_c::CreateInstance` tiene comentario `// bad` en el registro (`0x4A2B40`): señal histórica sin explicación técnica, exige recrear/actualizar/liberar la instancia entre rutas. No equivale a lock activo.
- Virtuales Bike/Train/Plane/Monster: mantener las direcciones y counts de vtable actuales; verificar que los cuerpos derivados no comparten target con hook heredado. `Bike::ResetSuspension` está registrado como simple, por lo que hay que comprobar también llamadores virtuales/heredados.
- Storage usa estado de fichero/guardado; dobles aislados cubren equivalencia pero no prueban una partida cargada completa ni interoperabilidad de saves. No introducir pruebas destructivas sobre archivos reales.
- Scripts: el IP y los parámetros compartidos sobreviven a llamadas híbridas; el sistema de command handlers personalizado puede esconder diferencias si se deja un vecino stubbed.
- Bancos `ProjectileCore355`, `StorageCore355` y `TrainQueries355` llaman `LockState(false)` y restauran el flag después: su evidencia no prueba que la API normal permita conmutar. El CSV indica individualmente si el banco actual vinculado contiene esa instrucción.
- FX/clothes: comparar ownership/refcounts y cleanup además del resultado; no desactivar funciones de render sobre texturas/cámaras falsas sin doubles adecuados.

## Candidatos

Priorizar consultas de tren, `CProjectileInfo::GetProjectileInfo`, `CGenericGameStorage::GetCurrentVersionNumber` y `CClothesBuilder::PreprocessClothesDesc` usando sus fixtures históricos. Son prioridades de experimentación, no funciones demostradas seguras. Procesamiento de mundo, render, creación/destrucción y IO requieren pruebas propias del subsistema. El resto queda especificado individualmente en CSV.
