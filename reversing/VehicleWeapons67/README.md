<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# VehicleWeapons67: CVehicle::ProcessWeapons (0x006E3950)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 1ca520153 + Vehicle.cpp/Vehicle.h). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-vehicleweapons6761026-070059-718608`. `coverage.json` calibrado al 50 % de la cobertura medida en esa corrida (el candado huerfano se libero y la corrida en cola termino).

`source.patch`: solo Vehicle.cpp y Vehicle.h (`git apply --check -R` verificado contra el arbol).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| ProcessWeapons | 0x006E3950 | CVehicle,ProcessWeapons | S |

No es virtual. thiscall sin argumentos, `ret` simple. Llamadores del EXE: 0x6C7917 (CHeli) y 0x6C9348 (CPlane::ProcessControl); ninguno lee EAX/ECX/EDX tras la llamada (`residue_scan.py`), asi que solo se registran.

## Revision contra el EXE (0x6E3950..0x6E3AE2)

- Si `m_nVehicleSubType (+0x594) == 4` y `this == FindPlayerVehicle(-1, false)` y el dword `+0x9DC` (CPlane::field_9DC) es 0: `CWeaponEffects::ClearCrossHairImmediately(0)`. Despues, si `m_nPhysicalFlags (+0x40) & 0x20000000` (PHYSICAL_DESTROYED) sale (sin cola).
- `bool firing = false; ordnance = 0` (locales inicializados en 0). Estado (`byte +0x36 >> 3`) 0 o 8: `target = GetPlaneWeaponFiringStatus(firing, ordnance)` (0x6E3440, **devuelve el objetivo en EAX**: la declaracion pasa de `void` a `CEntity*`) y `SelectPlaneWeapon(firing, ordnance)` (0x6D4900). Resto de estados: si `bFireGun` (`+0x42C` bit 4): `firing = true`, `ordnance = 1` (2 si modelo 0x208 Hydra y arma `+0x513 == 3`); si no, ambos a 0.
- `switch (byte +0x513)` (tabla de saltos 0x6E3AE4, solo 1..4): 1 `if (firing) FirePlaneGuns()`; 2 `if (ordnance) PossiblyDropFreeFallBombForPlayer(ordnance, true)`; 3 `if (ordnance)`: si el estado NO es 0 ni 8, `target = FindPlayerVehicle(-1, false)` cuando la mision (`+0x3BA`) es 35, 59 o 63 y `m_autoPilot.m_TargetEntity (+0x41C)` si no; `if (target) FireHeatSeakingMissile(target, ordnance, true)` (0x6E05C0); 4 `if (ordnance) FireUnguidedMissile(ordnance, true)`.
- Cola: lista de fogonazos = `+0xA0C` (subtipo 3, CHeli::m_ppGunflashFx) o `+0x9D4` (subtipo 4, CPlane::m_pGunParticles); si no es nula, `n = GetPlaneNumGuns()` (una sola vez) y, para `i < n` (con signo), cada fogonazo no nulo recibe `SetMatrix(matriz)` con `matriz = clump ? clump->parent + 0x10 : 0` (el clump se relee en cada vuelta; no se comprueba su tipo ni el marco).
- SelectPlaneWeapon (0x6D4900) **no escribe EDX** y el original aprovecha `mov cl,dl` tras la llamada para recuperar el bool; en C++ el bool es una variable local. El falso del banco para 0x6D4900 es un envoltorio desnudo que conserva EDX (solo en el banco).
- Los bits altos del dword que el EXE pasa como `bChange` a SelectPlaneWeapon son basura de pila; el callee solo lee el byte bajo (el banco registra el byte).
- Los desplazamientos se comprueban en compilacion con `static_assert(offsetof(...))` dentro de la funcion.

## Banco

Fixtures: 16384 casos, 16 familias (`n % 16`), vector `v` de 32 palabras (Fixtures.hpp). Arena propia con el vehiculo falso (0xA40 bytes), clump con marco padre, lista de fogonazos de 8 entradas y entidades falsas; codigos de puntero (nulo, otra entidad, el propio vehiculo, marco, clump, fogonazos, basura). Colaboradores sustituidos por falsos identicos en ambos lados que registran sus argumentos EN ORDEN: FindPlayerVehicle 0x56E0D0, ClearCrossHairImmediately 0x742CA0, GetPlaneWeaponFiringStatus 0x6E3440 (sin hook en la DLL), SelectPlaneWeapon 0x6D4900, FirePlaneGuns 0x6D4D30, PossiblyDropFreeFallBombForPlayer 0x6E07E0, FireHeatSeakingMissile 0x6E05C0 (sin hook en la DLL), FireUnguidedMissile 0x6D5110, GetPlaneNumGuns 0x6D3F30 y FxSystem_c::SetMatrix 0x4AA630. Se comparan eventos (orden, argumentos, valores previos de las salidas por referencia), memoria de la arena modificada, compuertas de ABI (pila, no volatiles, DF, SEH), ruta (el EXE salta a la DLL solo en el candidato), colaboradores originales en el EXE, estado x87 y restauracion de hooks y entradas. Cobertura de ramas medida sobre entradas y eventos con minimos fijos (`COVER_FLOOR`) y, tras la primera corrida, calibrados en `coverage.json`.

Comandos: `bash reversing/_handoff/bench_here.sh VehicleWeapons67`; `python -B run_local.py --analyze <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).

## 2026-10-27: FireHeatSeakingMissile (0x6E05C0) con hook desde el lote 73
Desde el lote 73 `CVehicle::FireHeatSeakingMissile` tiene `RH_ScopedInstall` (banco HeatFire73). El banco ya la sustituia por `FkHeat` en la direccion del EXE; `MakeCollab` devuelve ahora el cuerpo de la DLL y lo parchea con el mismo falso. Cambio: 0x6E05C0 (`_HEAT`) entra en `DLL_HOOKED` de `run_local.py` (se exige `dll=true`, `hooked` sigue a false). Compuertas y cobertura sin relajar. Repeticion APROBADA: 16384 pares, cero diferencias, evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-vehicleweapons6761027-000151-286152` (fuentes: HEAD + `reversing/HeatFire73/source.patch`). Quien valida FireHeatSeakingMissile: HeatFire73.

## 2026-10-27: GetPlaneWeaponFiringStatus (0x6E3440) con hook desde el lote 74
Desde el lote 74 `CVehicle::GetPlaneWeaponFiringStatus` tiene `RH_ScopedInstall` (banco PlaneFiring74). El banco ya la sustituia por `FkFiring` en la direccion del EXE; ahora tambien en el cuerpo de la DLL: 0x6E3440 entra en `DLL_HOOKED` de `run_local.py` (se exige `dll=true`, `hooked` sigue a false). Compuertas y cobertura sin relajar. Repeticion APROBADA: 16384 pares, cero diferencias, evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-vehicleweapons67\20261027-032345-980855` (una corrida previa, 20261027-031438-210317, completo los 16384 pares sin fallos pero GTA termino con 0xC0000409 despues de `complete` y el runner la rechazo; se repitio). Quien valida GetPlaneWeaponFiringStatus: PlaneFiring74.
