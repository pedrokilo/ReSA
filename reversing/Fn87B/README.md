<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 879 grupos / 9814 funciones reconstruidas; 9814 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn87B: `CPlayerPed::FindNextWeaponLockOnTarget` (lote 87B)

Estado: **APROBADO dentro del juego** (03/11/2026, worktree `wc-2` sobre main 1c0043488 + los cambios del lote). 20480 pares (16 familias de 1280 casos), cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn87b/20261103-224912-766127` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S, 0x60E530; antes llamada por direccion).

## Funcion
thiscall `ret 8` (CEntity* actual, bool), devuelve bool. Evalua peatones, objetos y (solo cooperativo) vehiculos con `EvaluateNeighbouringTarget` (0x60D1C0) y fija el mejor en `m_pTargetedObject` (+0x71C) con `RegisterReference`, byte +0x85 de los datos del jugador a 1. Si el mejor es un peaton y el arma activa no es cuerpo a cuerpo, avisa con un `CEventGunAimedAt` (a su grupo con `CEventGroupEvent` + `AddEvent`, o a la inteligencia del peaton con `CEventGroup::Add`).
Detalles del EXE conservados: `GetATanOfXY` con el vector hacia delante se llama y se descarta; los bucles de objetos y vehiculos piden el multiplicador de alcance con el ultimo peaton recorrido (EBP) y no con la entidad evaluada; ranura del arma con signo sin comprobar rango.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (ver `SCOPE_NOTE` de `register_hook.py`). `CEvent::operator new` (0x4B5620) entrega una ranura **real** del pool de eventos (el destructor virtual del EXE inlinea el borrado del pool y no admite memoria ajena); `CEvent::operator delete` corre real; el pool (cabecera y bytes de estado) se guarda y restaura tras cada lado. Pools de peatones, vehiculos y objetos, camara y jugador son arenas o regiones restauradas.
Fixtures: un objeto nunca tiene tipo peaton (leeria la inteligencia en +0x47C fuera de su hueco); los vehiculos llevan el puntero a la inteligencia en +0x47C.

## Fuera de los fixtures
Ranuras activas fuera de rango, jugador sin datos, pools con huecos fuera de los modelados, objetos o vehiculos con tipo peaton sin grupo.

## Prueba negativa
Produccion mutada en ocho puntos (prioridad inicial -10000 -> -9999, mascara 4 -> 8 del byte +0x42D, mascara 0x40 -> 0x80 del byte +0x140 de objetos, multiplicador de objetos con el objeto en vez del ultimo peaton, condicion de arma cuerpo a cuerpo invertida, byte +0x85 = 2, quitar la exclusion del objetivo actual, subtipo BMX -> BIKE): el banco FALLA con 16628 de 20480 pares con diferencias (`resultados-fn87b/20261103-230622-744418`); fuente restaurado despues.

## Bancos repetidos
`FootWave4` cita 0x60E530 (falso en la direccion del EXE y en el cuerpo de la DLL por `Addr`, como FindWeaponLockOnTarget): no hizo falta adaptarlo; repetido con el hook activo, aprobado (`resultados-footwave4/20261103-225557-566394`).
