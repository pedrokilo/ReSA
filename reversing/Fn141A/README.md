<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn141A: 24 funciones diminutas del EXE (lote 141A)

Estado: **APROBADO dentro del juego** (09/11/2026, worktree `wv-3` sobre main c8001f26f9 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn141a/20261109-141516-145271` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn141A-20261109-141516-145271`; `comparisons.jsonl.gz` de 46,4 MB, bajo el limite de GitHub). 24 x 16384 = **393216 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `3a1980b91a810dde2f0a717cd1c235f1bd6988447cc14cf9f1549119175e8b66`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 24 direcciones tenia hook (se comprobo el codigo y `docs/hooks.csv`); ninguna estaba en otro worktree. Las clases del listado eran inexactas en varias: los nombres reales salen del desensamblado, de las vtables leidas del EXE y de los llamadores (ver `fixture_oracle.py`).

| # | Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CCustomBuildingDNPipeline::AdvanceWorkingIndex` (nueva) | 0x5D6830 | 25 | S |
| 1 | `CCustomBuildingDNPipeline::GetExtraNightColors` (nueva) | 0x5D6E90 | 14 | S |
| 2 | `CDummy` constructor (cuerpo) | 0x532540 | 28 | S |
| 3 | `CDummy` destructor (cuerpo) | 0x532560 | 11 | S |
| 4 | `CWeaponInfo::GetAnimLoopStart` (listado: CPedType) | 0x608F00 | 20 | S |
| 5 | `CWeaponInfo::GetAnimLoopEnd` | 0x608F20 | 20 | S |
| 6 | `CTaskComplexObserveTrafficLightsAndAchieveHeading` constructor | 0x631910 | 34 | S |
| 7 | idem `GetTaskType` (0xE1) | 0x631940 | 6 | V |
| 8 | `CQuadBike::ScalarDeletingDestructor` | 0x6CE440 | 30 | S |
| 9 | `CTrailer` destructor (cuerpo; listado: CQuadBike) | 0x6CED10 | 11 | S |
| 10 | `CTaskAllocatorKillThreatsBasicRandomGroup::ScalarDeletingDestructor` | 0x5F6900 | 30 | S |
| 11 | idem destructor (cuerpo) | 0x5F6920 | 11 | S |
| 12 | `CTaskComplexAttractorPartnerWait` destructor (cuerpo) | 0x633290 | 11 | S |
| 13 | `CTaskComplexUseAttractorPartner::ScalarDeletingDestructor` (listado: Wait) | 0x638FD0 | 30 | S |
| 14 | `CTaskComplexDriveToPoint` destructor (cuerpo) | 0x63CE70 | 11 | S |
| 15 | `CTaskComplexDrivePointRoute::ScalarDeletingDestructor` (listado: DriveToPoint) | 0x645350 | 30 | S |
| 16 | `CTaskComplexEnterAnyCarAsDriver` destructor (cuerpo) | 0x63D100 | 11 | S |
| 17 | `CTaskComplexShuffleSeats::ScalarDeletingDestructor` (listado: EnterAnyCar) | 0x6436E0 | 30 | S |
| 18 | `CTaskComplexGoToPointAndStandStill` destructor (cuerpo) | 0x668630 | 11 | S |
| 19 | `CTaskComplexGoToPointAiming::ScalarDeletingDestructor` (listado: GoToPointAndStandStill) | 0x66DD50 | 30 | S |
| 20 | `CTaskComplexUseEffectRunning::ScalarDeletingDestructor` | 0x636830 | 30 | S |
| 21 | idem destructor (cuerpo) | 0x636850 | 11 | S |
| 22 | `CTaskComplexUseEffectSprinting::ScalarDeletingDestructor` | 0x636910 | 30 | S |
| 23 | idem destructor (cuerpo) | 0x636930 | 11 | S |

Tipo S: `RH_ScopedInstall` o `RH_ScopedNamedGlobalInstall` (los cuerpos de destructor y constructor, que escriben la vtable del EXE y no pueden ser ranura virtual); V: `RH_ScopedVMTInstall`.

## Funciones saltadas
Ninguna: las 24 son viables en C++ puro (sin `__asm`).

## Revision contra el EXE
- **0x5D6E90 no es `GetExtraVertColourPtr`**: el EXE hace `mov eax,[ecx+eax]` (LEE el dword de la geometria en el desplazamiento del plugin, es decir `NightColors`); la funcion de la DLL devuelve la direccion. Se anadio `GetExtraNightColors` y no se engancho la otra. `0x5D6830` avanza `[0xC02C14]=(x+1)&0xF` y guarda el argumento en `[0xC02C18]` (el `Update` de `CCustomBuildingRenderer` lo lleva expandido y no se toco).
- **CDummy**: constructor = `CEntity::Constructor` (0x532A90, hook) y `byte[+0x36]=(b&0xFD)|5`, vtable 0x8638C0; destructor (cuerpo) = vtable 0x8638C0 y `CEntity::Destructor` (0x535E90, hook). Ambos llaman a las envolturas de `CEntity`, ahora publicas y `noinline` (si no, el banco no podia sustituir el colaborador en la DLL).
- **CWeaponInfo**: floats en +0x40/+0x44 (tabla 1) y +0x4C/+0x50 (tabla 2) elegidos por el BYTE bajo del argumento; devuelven en ST0 (`fld dword`, los SNaN salen como QNaN). Se comprobaron los desplazamientos con `VALIDATE_OFFSET`.
- **Observe...Heading**: `[+0xC]=duracion`, vtable 0x86DFC4, `[+0x10]` = rumbo copiado como dword (sin pasar por el x87). `GetTaskType` = 225 (`static_assert`).
- **Destructores con borrado** (8, 10, 13, 15, 17, 19, 20, 22): `call cuerpo; test byte [esp+8],1; operator delete; mov eax,esi; ret 4` como en los lotes 125A/128A. Colaboradores: cuerpo con hook (envolturas `Destructor()` pasadas a `noinline`) y `CTask::operator delete` (0x61A5B0) / `CTaskAllocator::operator delete` (0x69D990), con hook; `CQuadBike` (cuerpo 0x6CDC30) y `CVehicle::operator delete` (0x6E2D90) no tienen hook: el falso va en la funcion propia de la DLL (`Lote141AQuadBikeDestructorBody`, `&CVehicle::operator delete`), como `CMonsterTruck` en Fn125A.
- **Cuerpos de destructor** (3, 9, 11, 12, 14, 16, 18, 21, 23): `mov [ecx],vtable; jmp base`. Bases: `~CEntity`, `~CAutomobile` (0x6A61E0), `~CTaskAllocatorKillThreatsBasic` (0x69C780), `~CTaskComplex` (0x61A3D0), `~CTaskComplexCarDrive` (0x63CA40), `~CTaskComplexUseEffect` (0x632270), todos con hook; las envolturas de las bases pasaron a publicas (y `noinline` donde no lo eran).
- Los destructores con borrado de `UseAttractorPartner`, `DrivePointRoute`, `ShuffleSeats` y `GoToPointAiming` llaman a las envolturas `Destructor()` ya enganchadas (0x633300, 0x643490, 0x63D1A0, 0x668870).

## Banco
Una funcion por ruta: original = bytes del EXE con el hook apagado; candidato = DLL con SOLO ese hook. Objetos sinteticos en una arena propia (ventana de 4 KiB con guardas de 0x40; `this` alineado, desalineado, al borde o cerca del inicio, relleno aleatorio/ceros/unos/0xCD) y memoria global real para las dos del pipeline (0xC02C14..+8 y 0x8D12BC, se guardan y restauran). Se observa: bytes cambiados de la arena y de la memoria global, eventos de los falsos (direccion, ECX, dword de [ECX] al entrar y, en el constructor de CDummy, el byte +0x36), valor devuelto (EAX o etiqueta de `this`), x87 (vacia, o un solo valor en ST0 con la etiqueta y los 80 bits esperados para las dos de CWeaponInfo). **EAX/ECX/EDX/EFLAGS tras la llamada** quedan en `original_residue`/`candidate_residue` y no forman parte de la igualdad (ningun llamador los lee); difieren en las funciones de cuerpo no trivial porque la DLL es codigo de depuracion (`pairs_with_different_eax_ecx_edx_info_only` del `summary.json`). Los cuerpos con vtable observan la vtable escrita (el falso de la base registra el dword de [ECX] al entrar).

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, y la exactitud de los colaboradores falseados (cada uno tiene su banco).

## Prueba negativa
Dos rondas de 24 mutaciones (`mutate.py`, 2048 casos por funcion con `FN141A_CASES`): **las 24 funciones fallan en ambas rondas** (`negative-runs/`, con `diferencias-por-funcion.txt`; rondas `20261109-143930-460193` y `20261109-144400-099482`). Fuentes restauradas con SHA256 igual a las copias buenas.

## Bancos repetidos
Citan estas direcciones: `Fn130A` (0x532560, ahora con hook: adaptado para usar `F` / colaborador con hook en lugar de la funcion propia) y `CBDNRenderCB30` (0x5D6E90 aparece en el marcador de `host_check.py`, adaptado). Resultados en la seccion final del informe del lote.

**Revalidacion conjunta 141R (09/11/2026):** repetido sin cambios sobre la base con 140A integrado y aprobado: 393216 pares, cero diferencias (`reversing/CppTwelveLevel6Final/runs/Fn141A-20261109-182342-890731`).
