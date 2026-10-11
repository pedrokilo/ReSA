<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PickWeap1 - banco de CPickups + CWeapon

Banco por pares (hook desactivado = EXE original, hook activado = DLL) para las funciones pendientes de CPickups y CWeapon.
Estructura copiada de PadMouse1 / PoolsWave4. GAME redirigido en memoria a `E:/JUEGOS/GTA San Andreas - CVehicle Test`
(build_probe.py y run_local.py; runtime_runner.py no se toca).

## Uso

    bash reversing/_handoff/bench.sh PickWeap1 3

El analizador exige: todos los pares iguales, `state_restored`, `layout_ok` y el total de pares de la tabla `fns` (11020).

## Archivos

- Common.hpp: trazas (T/Name), Patches (EntryPatch), Snap/RestoreRegions, Body()/FollowIn.
- PickupCases.hpp: dobles y casos de CPickups (pool de pickups falso en 0x9788C0 con copia y restauracion).
- WeaponCases.hpp / WeaponCases2.hpp: dobles y casos de CWeapon.
- LiveComparison.hpp: tabla `fns`, despacho, comprobaciones de vtables y diseno, bucle principal.

## Reglas aplicadas

- Todos los Body() se calculan antes de crear ningun EntryPatch, con los hooks en su estado inicial.
- Cada doble respeta el `ret N` de su stub; los hilos ajenos no trazan ni alteran estado.
- Padding y bytes sin inicializar enmascarados; literales float leidos del EXE con std::bit_cast; x87 a 24 bits; fsin/fcos/fpatan por __asm.
- Los fixtures no hacen que el EXE lea locales sin inicializar.
- No se tocan pools, pickups ni armas reales: objetos falsos o intercambios temporales de punteros (Players[0].m_pPed, matriz de camara, pool de vehiculos, datos de col de perdigones, g_fx.m_GunShell, ModelInfo) que se restauran siempre.

## Suposiciones a revisar al compilar

- Los wrappers `_ExeAbi` de Pickups (tPickupReference devuelto en EAX por el EXE) devuelven int32.
- `Add(&evento, false)` en Weapon.cpp resuelve a `Add(CEvent*, bool)`.
- Bloques `__asm` MSVC (x86): nombres de operandos sin colisiones con registros.
- Comprobar nombres de miembros usados por los casos (CPickup, CWeaponInfo, CPlayerPedData).

## Retorno de CWeapon::Fire

El EXE devuelve `bl` crudo: en el `default` del switch (0x742794 `mov bl,[esp+60h]`) bl es el byte bajo del puntero targetPosn y sale por
0x7429FB `mov al,bl`. La DLL devuelve bool, asi que el banco compara `al!=0` en Fire (no el byte exacto).

## Fuera del banco (desensamblado con dis.sh sobre gta-sa.exe 1.0 US)

Ninguna de estas es imposible de doblar en teoria; se dejan fuera porque cada una exige dobles NUEVOS de estado global real que no se
pueden verificar sin compilar/ejecutar, y un doble mal calibrado corromperia pools del juego en curso. Primera instruccion que lo impide:

- FireSniper 0x73AAC0: 0x73AE39 `call 4B1FD0` (ctor CEventGunShotWhizzedBy en pila; el de CEventGunShot en 0x73ADA4 `call 4AC610`) y
  0x73AE54/0x73ADC2 `call 4AB420` sobre `4ABA50` (CEventGroup::Add en el grupo GLOBAL: clona el evento en el pool real de eventos).
  Ademas 0x73AC4B `call 736010` (AddBullet: array real de balas) y 0x73AE61 `call 602590` (CInterestingEvents::Add). Hace falta doblar
  4AB420/4ABA50/736010/602590/53F9A0/50A970 y gestionar las CReference reales que registran los ctores sobre peds falsos; el DLL usa
  vtables DLL en los eventos, asi que el doble solo puede comparar campos.
- DoBulletImpact 0x73B550: 0x73B817 `call dword ptr [edx+0B0h]` (virtual del victim, ranura 0x2C) con victim falso sin vtable real;
  0x73BB00 `call 71C0D0` (CGlass::WasGlassHitByBullet sobre el pool real de cristales); 0x73BBB6/0x73BBE1/0x73BBFF/0x73BD20
  `call 5A0D90` (CObject::ObjectDamage: destruye objetos reales); 0x73BDB4/0x73BE26 `call 4AB420` (CEventGroup::Add del vehiculo);
  0x73BF24/0x73C04B `call 49F3D0` y 0x73BADE `call 49EB00` (g_fx.AddBulletImpact/AddBlood: particulas reales); 0x73C11B `call 726AF0`
  (CBulletTraces::AddTrace, array real); 0x73BA50 `call 73A530` (GenerateDamageEvent).
- TakePhotograph 0x73C1F0: 0x73C20D `call 4A9BE0` (FxManager_c::CreateFxSystem real en 0xA9AE80) + 0x73C218 `call 4AA3D0`;
  0x73C265..0x73C26B `mov ebx,[B74490h]` recorre el pool REAL de peds (stride 0x7C4) y luego vehiculos/objetos con
  0x73C4C0/0x73C6D0 `call 56BA00` (ProcessLineOfSight real); escribe 0xC8A7C0/C1 (flags globales). Seria doblable intercambiando el
  puntero del pool con uno falso, pero no se ha construido el fixture de pool de peds/vehiculos/objetos.
- FireInstantHitFromCar2 0x73CBA0: 0x73CC78/0x73CC7F `call 4ABA50`/`4AB420` (grupo global, evento CEventGunShot ctor 0x73CC61
  `call 4AC610`), 0x73CC8C `call 602590`, 0x73CCFD `call 7000E0`, 0x73CD66 `call 56BA00` (ProcessLineOfSight) seguido de 0x73CD92
  `call 73B550` (DoBulletImpact, ver arriba) y 0x73CDA6 `call 4AC720` (dtor de evento).
- GenerateDamageEvent 0x73A530: 0x73A571 `call 4AD3F0` (CPedDamageResponseCalculator), 0x73A5AC `call 4AD830` (ctor CEventDamage),
  0x73A641 `call 4D4610` y 0x73A783 `call 4D3AA0` (CAnimManager::BlendAnimation/AddAnimation sobre el RpClump del ped: jerarquia
  de animacion real), 0x73A895 `call 4AB420` (CEventGroup::Add del grupo del ped victima) y 0x73A6C5/0x73A6D5 FindPlayerPed+ReportCrime.
  Requiere un clump con jerarquia de animacion real o doblar 4D6A70/4D4610/4D3AA0/4D68B0 y todas las llamadas virtuales `[edx+10h]`
  de tareas (0x73A60B, 0x73A814, 0x73A87B).
- FireProjectile 0x741360: 0x7413A4/0x74168C/0x74194D `call 54F560` (CPlaceable::AllocateMatrix toma una matriz de la lista GLOBAL
  CMatrixLinkList si la entidad `[esi+14h]` no tiene matriz; evitable dando matriz a todos los fixtures), 0x741886/0x7418F5
  `call 56A490` (GetIsLineOfSightClear), 0x7419CE `call 569E20` (TestSphereAgainstWorld), 0x741A53 `call 737C80` (AddProjectile:
  crea un CProjectile real en el pool de objetos y escribe el array de ProjectileInfo), 0x741928 `call 737C00`, 0x7416EA `call 742BD0`,
  0x74175A `call 73F910` (en el banco), 0x741BC1 `call 4AB420` (grupo global). Todo doblable salvo la gestion de CReference/eventos
  como en FireSniper; no se incluyo por riesgo sin poder compilar.
- Update de pool de pickups (CPickups::Update 0x458DE0; CPickup::Update 0x457410 ya esta validada): 0x45902E `call 457410`
  (CPickup::Update por cada pickup real), 0x458E70/0x458E8A `call 563220/563280` (CWorld::Add/Remove de objetos reales), 0x458E64
  `call 4567E0` (GiveUsAPickUpObject: CObject real), 0x458E9C `call dword ptr [eax]` (virtual del objeto), 0x4590DA `call 4588B0`,
  0x4591BC `call 458A80` (CreatePickupCoorsCloseToCoors: sondeos de colision/mundo), 0x45924A `call 459180`, 0x4590F3 `call 56D2B0`,
  0x459315 `call 5E6320` (CPed::ClearWeapons), 0x458F2C `call 600EE0` (FindTaskByType del jugador) y 0x458EAC/0x458F07/0x458EB6 pad
  (`53FB70`, `540670`, `540A70`). Recorre el pool real en 0x9788C0: se haria con el pool falso del banco y 14 dobles; pendiente.

## Aprobación (09/10/2026)

Corrida `20261009-162447-757920`: passed=true, 11020 pares, 60 funciones con 0 fallos (34 de CPickups y 26 de CWeapon), evento complete (state_restored, layout_ok), evento layout correcto, saves y configuraciones intactos. Registrada con `register_pickweap1.py` (evidencia en `runs/`, `validation-summary.json`). Solo se registran las funciones con casos en la corrida; siguen PENDIENTES CPickups::Update y CWeapon::FireSniper, DoBulletImpact, TakePhotograph, FireInstantHitFromCar2, GenerateDamageEvent, FireProjectile, DoWeaponEffect, FireOneInstantHitRound y ProcessLineOfSight (estas tres no figuran en el README del banco ni tienen casos). Límites: `CWeapon::Fire` compara `al!=0` (el EXE devuelve un byte basura); pool de pickups, armas, peds y vehículos falsos; colaboradores doblados sin crédito.
