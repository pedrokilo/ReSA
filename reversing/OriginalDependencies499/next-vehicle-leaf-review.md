<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Próxima propuesta: dos hojas CCarCtrl aún pendientes

Revisión estática, sin cambiar fuentes, instalar hooks, compilar ni ejecutar el juego. Ambas entradas siguen pendientes en `work-queue.csv` y no figuran en `approved.json`. Quaternion Conjugate/Scale/Copy son pequeños, pero no pertenecen a la cohorte inicial de 499; no se proponen para reducir artificialmente esa cola.

| Método | EXE | Fuente actual | Pares propuestos |
| --- | --- | --- | --- |
| `CCarCtrl::SwitchVehicleToRealPhysics` | `0x423FC0` | `source/game_sa/CarCtrl.cpp:1090` | 1.024 |
| `CCarCtrl::UpdateCarCount` | `0x424000` | `source/game_sa/CarCtrl.cpp:1120` | 1.024 |

Los dos son cdecl, sin colaboradores ni acceso a vtable. La clase CCarCtrl ya tiene instalación activa; se pueden añadir únicamente sus hooks después de implementar y probar. Ninguna propuesta precisa constructor de vehículo, pool, modelos cargados o un puntero nulo para eludir su cuerpo.

## SwitchVehicleToRealPhysics

`0x423FC4` lee el byte `vehicle+0x36`, conserva solo sus tres bits bajos (`and cl,7`) y establece status 3 (`or cl,18h`). `0x423FD2` limpia el byte `+0x3BB` de autopilot; `0x423FD8` lee el timer `0xB7CB84`, le suma 2.000 y escribe `+0x3AC`. **Vuelve a leer el timer** en `0x423FEA`, y lo escribe en `+0x3B0`; `0x423FF6` limpia `+0xB8`, `m_nFakePhysics`. El retorno `0x423FFC` es cdecl sin ajuste de argumentos.

La propuesta debe usar `SetStatus(STATUS_PHYSICS)` o el campo comprobado, `m_autoPilot.m_nTempAction`, `m_nTimeToStartMission`, `m_nTimeSwitchedToRealPhysics` y `m_nFakePhysics`, tras comprobar offsets exactos. Las dos lecturas de timer y el overflow unsigned del deadline deben conservarse. No basta una rutina equivalente que solo cambie el status.

Fixtures: los 256 bytes posibles de status/type × cuatro timers (0, 1.999, frontera de overflow al sumar 2.000 y UINT32_MAX). Otros bytes del vehículo, incluyendo los campos a limpiar, llevan patrones distintos por caso; un buffer suficientemente grande con guardas ofrece el layout real leído/escrito, sin constructor/destructor ni llamadas virtuales. Cada lado compara el buffer completo y solo permite escribir `+0x36`, `+0x3BB`, `+0x3AC..3AF`, `+0x3B0..3B3` y `+0xB8`. Timer y buffer se restauran antes/después de cada llamada y antes de bombear mensajes.

## UpdateCarCount

`0x424000` lee exclusivamente el byte de decrease; cualquier valor no cero elige decremento (`0x424013 jne`). El byte createdBy `+0x4A4` selecciona una tabla de cuatro ramas después de restarle 1; los valores fuera de 1..4 no hacen nada (`0x424019 ja 0x4240C8`).

Para incremento, las entradas son `0x424026` (random), `0x42403C` (mission), `0x42405B` (parked) y `0x424062` (permanent). Random incrementa `0x969094` y, si el bit 0 de `vehicle+0x428` está activo, `0x969098`. Mission **limpia ese bit** y decrementa `0x969098` sin saturación (`0x42404E`), además de incrementar `0x96909C`. Los otros contadores son `0x9690A0` y `0x9690A4`.

Las ramas de decremento son `0x424076`, `0x42409C`, `0x4240AB` y `0x4240BA`. Cada decremento satura a cero únicamente cuando el resultado de DEC tiene signo negativo (`jns` en `0x424085/093/0A2/0B1/0C0`). Es preciso conservar la aritmética de 32 bits: un `--int32` C++ sobre INT32_MIN produce overflow indefinido y `max(0,value-1)` tampoco reproduce ese caso. Usar unsigned para las operaciones y probar el bit de signo del resultado. En incremento de mission, el decremento de law-enforcer no se satura: omitir esta excepción altera estado real.

Fixtures: ocho createdBy (1..4, 0, 5, 128, 255) × ocho decrease (0, 1, 2, 17, 127, 128, 254, 255) × dos estados del bit law-enforcer × ocho conjuntos de contador (0, 1, 2, -1, INT32_MIN, INT32_MAX, UINT32_MAX y un patrón mixto). Total 1.024. Los cinco contadores se guardan como bytes, incluidos sus signos; se compara todo el vehículo con guardas y no se permite cambiar nada fuera del bit 0 de `+0x428`. Los cinco contadores y el buffer se restauran exactamente en cada par.

## Condiciones de admisión futuras

El banco futuro necesita original/candidate no nulos, la misma entrada EXE RH OFF/ON realmente registrado, preservación de ESP y EBX/ESI/EDI/EBP, bytes de código y estados Enabled/Locked restaurados, todos los casos únicos, resúmenes por función y final completo. Debe revisarse también cada caller original: el gate cdecl por sí solo no demuestra que los callers no consuman registros volátiles o flags preservados accidentalmente por el EXE. Ninguna de estas condiciones se declara probada por este informe estático.
