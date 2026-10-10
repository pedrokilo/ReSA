# WeaponFinal2 - banco de las nueve funciones de CWeapon que PickWeap1 dejo fuera

Banco por pares (hook desactivado = EXE original, hook activado = DLL) para las ultimas funciones pendientes de `CWeapon`
(GTA SA 1.0 US): `ProcessLineOfSight` 0x73B300, `DoWeaponEffect` 0x73E690, `FireOneInstantHitRound` 0x73AF00,
`GenerateDamageEvent` 0x73A530, `DoBulletImpact` 0x73B550, `TakePhotograph` 0x73C1F0, `FireInstantHitFromCar2` 0x73CBA0,
`FireSniper` 0x73AAC0 y `FireProjectile` 0x741360. Estructura copiada de PickWeap1 / PadMouse1 (corre dentro del juego completo,
se engancha en `CGame::Process`). `GAME` se redirige en memoria a `E:/JUEGOS/GTA San Andreas - CVehicle Test` en `build_probe.py` y
`run_local.py` (`runtime_runner.py` no se toca). No se ha tocado nada bajo `reversing/PickWeap1`.

## Uso

    bash reversing/_handoff/bench.sh WeaponFinal2 3

El analizador exige: todos los pares iguales, `state_restored`, `layout_ok` y el total de pares de la tabla `fns` (7808).

## Pares por funcion

| Funcion | Casos |
|---|---|
| ProcessLineOfSight | 256 |
| DoWeaponEffect | 512 |
| FireOneInstantHitRound | 768 |
| GenerateDamageEvent | 1024 |
| DoBulletImpact | 2048 |
| TakePhotograph | 640 |
| FireInstantHitFromCar2 | 640 |
| FireSniper | 640 |
| FireProjectile | 1280 |

## Archivos

- `Common.hpp`: copia de PickWeap1 (trazas, `Patches`/EntryPatch, `Snap/RestoreRegions`, `Body()/FollowIn`, `FindHook`).
- `Doubles.hpp`: memoria falsa (entidades, inteligencia y datos de jugador por ped, tareas, clump, asociaciones de animacion, fx, matrices,
  pool de referencias), vtables falsas, resumen de eventos y los dobles comunes.
- `Cases1.hpp` / `Cases2.hpp` / `Cases3.hpp`: fixtures y dobles propios de cada funcion.
- `LiveComparison.hpp`: tabla `fns`, calculo de los cuerpos de la DLL, comprobacion de disposicion/virtuales, bucle principal.

## Como se prueban las funciones que PickWeap1 no pudo probar

- **Eventos** (`CEventGunShot`, `CEventGunShotWhizzedBy`, `CEventDamage`, `CEventVehicleDamageWeapon`) y `CPedDamageResponseCalculator`: los
  constructores y destructores se ejecutan DE VERDAD (en el EXE y en la DLL) sobre entidades falsas. `CReferences::pEmptyList` se intercambia
  por una lista falsa de 64 nodos (como en el banco de CAESound) y se restaura; el estado de referencias (orden de la lista libre y numero de
  referencias vivas de cada entidad falsa) forma parte de la comparacion.
- `CEventGroup::Add` (0x4AB420) se dobla y registra un resumen campo a campo del evento (el vptr y el relleno no se comparan);
  `GetEventGlobalGroup` (0x4ABA50) devuelve un grupo falso, asi no se toca el pool real de eventos ni el grupo global.
- **Virtuales**: el vehiculo falso tiene una vtable con `BurstTyre` en la ranura 0x2C, el objeto con `SetIsStatic` en la ranura 4 y la tarea con
  `GetTaskType` en la ranura 4 (las llamadas `call [edx+0B0h]` / `[edx+10h]` del EXE). El banco comprueba antes de empezar que la DLL usa las
  mismas ranuras (evento `layout`). `CEventDamage::AffectsPed` se dobla por la ranura 5 de la vtable del EXE (0x85AD78) en la DLL.
- **Pools de peds y objetos** (TakePhotograph): se intercambian temporalmente `0xB74490` y `0xB7449C` por cabeceras falsas (6 peds de 0x7C4 bytes,
  6 objetos de 0x19C) y se restauran.
- **Animacion**: `CAnimManager::AddAnimation/BlendAnimation`, `RpAnimBlendClumpGetAssociation/FirstAssociation` y `CAnimBlendAssociation::SetCurrentTime`
  se doblan; las asociaciones son buffers falsos que se rellenan con 0xCD y se comparan al final. El clump falso tiene el byte de tipo 2 (rpCLUMP).
- `CPed::GetBonePosition` se dobla en el EXE (0x5E4280, version con puntero) y en la DLL (la version que devuelve `CVector` por puntero oculto; misma pila).
- `tColLighting::GetCurrentLighting`: el EXE (0x59F0C0) es una funcion cdecl `(valor, escala)` y la DLL un miembro const: dos dobles distintos con la misma traza.
- `CCamera::IsSphereVisible(const CVector&, float)` (0x420D40) se dobla; es la sobrecarga de dos argumentos, distinta de la de PickWeap1 (0x420C40).
- `CWorld::ResetLineTestOptions` y `CVector::Normalise` se ejecutan de verdad.

## Reglas aplicadas

- Todos los `Body()` se calculan antes de crear ningun EntryPatch, con los hooks en su estado inicial; cada doble respeta el `ret N` del EXE.
- Los hilos ajenos no trazan ni cambian estado (`Mine()`); x87 a 24 bits; NaN en salud, estadisticas y vectores segun el asm (`fcomp + test ah`);
  literales float del EXE verificados contra el EXE (todos son el float mas cercano salvo 0x859878 = 0x42652EE0, usado con `std::bit_cast`).
- Los fixtures no hacen que el EXE lea locales sin inicializar: el CColPoint solo se mira si hubo impacto, las entidades siempre llevan matriz donde el
  EXE reservaria una REAL (`AllocateMatrix`), y los tipos de arma se limitan a 0..46 y 49..58 (47/48 hacen assert en `GetWeaponInfoIndex`).
- Se guardan y restauran: camaras, tiempo, jugadores, flags 0xC8A7C0, pools de peds/objetos, cruceta 0, `MoonSize`, `CWorld::pIgnoreEntity` y compania,
  `RsGlobal`, nivel del `CLoadMonitor`, lista de referencias.

## Suposiciones a revisar al compilar

- `&RwMatrixCreate` / `&RwMatrixDestroy` son direcciones de funcion (como en Fx25) y `Body()` de un miembro virtual no se usa: `AffectsPed` sale de la vtable.
- Los miembros usados en el evento `layout` son publicos (los usa el propio Weapon.cpp); si alguno no lo fuera habria que pasar a desplazamientos crudos.
- `CCopPed` mide 0x7C4 y `CCutsceneObject` 0x19C (zancadas de los pools del EXE); se comprueba en `layout`.
- `GetPassengers()` recorre `m_nMaxPassengers` (0x488), que es lo que lee el EXE.

## Aprobación (09/10/2026)

Corrida `20261009-181216-969478`: passed=true, 7808 pares, 9 funciones con 0 fallos, evento complete (state_restored, layout_ok), evento layout correcto, saves y configuraciones intactos. Registrada con `register_weaponfinal2.py` (evidencia en `runs/`, `validation-summary.json`). Limites: `tColLighting::GetCurrentLighting` doblada y fuera de alcance; eventos, animaciones y colision doblados; entidades, pools y lista de referencias falsos; tipos de arma 0..46 y 49..58; colaboradores doblados sin credito.
