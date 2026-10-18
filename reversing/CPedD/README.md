<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed, grupo D: ciclo de vida, estado y consultas (26 funciones)

Fecha: 06/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `python reversing/CPedD/build_probe.py` y `python reversing/CPedD/run_local.py`).
Prefijo de caso `cpedd`, espacio de nombres `CPedDLive`, filtro opcional `cpedd-only.txt` con `CPed::Función`.

| Función | Hook / vtable | Casos |
|---|---|---|
| `Constructor` | 0x5E8030 | 512 |
| `Destructor` | 0x5E8620 | 512 |
| `Initialise` | 0x5DEBB0 | 16 |
| `Update` | 0x5DEBE0 | 16 |
| `Save` / `Load` | vtable 0x86C358, huecos 24 / 25 | 128 / 128 |
| `FlagToDestroyWhenNextProcessed` | vtable 0x86C358, hueco 21 | 1024 |
| `CanBeArrested`, `CanBeDeletedEvenInVehicle`, `CanPedReturnToState`, `CanSetPedState`, `CanStrafeOrMouseControl`, `IsAlive`, `IsPedShootable` | 0x5DF060, 0x5DF150, 0x5DF000, 0x5DF030, 0x5DF090, 0x5E0170, 0x5DEFD0 | 256 cada una |
| `CanBeDeleted` | 0x5DF100 | 512 |
| `IsPedInControl` | 0x5E3960 | 512 |
| `IsPlayer` | 0x5DF8F0 | 128 |
| `IsPointerValid` | 0x5E4220 | 1024 |
| `SetPedState` | 0x5E4500 | 512 |
| `SetPedStats` | 0x5DEBC0 | 128 |
| `SetPedDefaultDecisionMaker` | 0x5E06E0 | 256 |
| `SetStayInSamePlace` | 0x481090 | 128 |
| `SetRadioStation` | 0x5DFD90 | 512 |
| `GetBikeRidingSkill` | 0x5DF510 | 512 |
| `UpdateStatEnteringVehicle` / `UpdateStatLeavingVehicle` | 0x5E01A0 / 0x5E01B0 | 16 / 16 |

Total: 8.384 pares.

## Diferencias reales corregidas (fuente)

| Función | Corrección (evidencia en el EXE) |
|---|---|
| `IsPedShootable` | El EXE hace `cmp [estado],0x2E / setle` (0x5DEFD0): comparación con signo, los estados negativos también valen. La DLL usaba un `switch` sin los negativos. Ahora `m_nPedState <= PEDSTATE_STATES_CAN_SHOOT`. |
| `CanPedReturnToState` | 0x5DF000: `jg 0x26` devuelve `false` con signo (los negativos pasan) y solo excluye {2, 0xC, 0x10, 0x11, 0x16, 0x1F}. La DLL devolvía `false` para los negativos. |
| `GetBikeRidingSkill` | 0x5DF510: la habilidad (estadística 0xE5 / 1000) se recorta a **1.0** (0x858624), no a 1000, y un NaN se conserva (`fcomp` + `test ah,5`). La DLL usaba `std::min(1000.f, ...)`. |
| `SetRadioStation` | 0x5DFDC5: `cmp rand,0x3FFF / jge` elige la segunda emisora con `rand >= 0x3FFF`; la DLL usaba `<= RAND_MAX / 2` (con 0x3FFF exacto elegía la primera). |
| `IsPointerValid` | 0x5E4220: solo comprueba que el índice por división con signo (`(ptr - almacenamiento) / 0x7C4`) esté en 0..capacidad-1 (0x8C), sin mirar si el hueco está ocupado ni si el puntero es del pool. La DLL usaba `IsObjectValid` (exigía hueco ocupado y puntero dentro del pool; `GetIndex` además hace `assert`). Se añadió `CPool::GetIndexUnchecked` (`source/game_sa/Core/Pool.h`). |
| `CPed::CPed` | (1) `bHasACamera` es verdadero cuando `rand & 3 == 0` (0x5E82EA: `and al,3 / neg al / sbb eax,eax / inc eax`); la DLL tenía `% 4 != 0` (al revés). (2) 0x5E8411 escribe solo el **byte** bajo de +0x460; la DLL ponía a cero `m_roadRageWith` (4 bytes). |
| `CPed::~CPed` | 0x5E879F: tras `CleanUpOldReference(&m_pLookTarget)` el EXE **no** pone el puntero a nulo; la DLL usaba `ClearReference` (lo ponía a nulo). Ahora `SafeCleanUpRef`. |
| `Load` | Se quitó el `assert(size == sizeof(save))`: el EXE lee el tamaño (4 bytes) y lo ignora (0x5D4640). |

## Sin diferencias (MATCH, con la evidencia del EXE)

* `Initialise`: `call 0x608E40 (CPedType::Initialise)` + `jmp 0x64F860 (SetAnimOffsetForEnterOrExitVehicle)`.
* `Update`, `UpdateStatEnteringVehicle`, `UpdateStatLeavingVehicle`: `ret` en 0x5DEBE0 / 0x5E01A0 / 0x5E01B0.
* `Save` (0x5D5730): `CPedSaveStructure::Construct`, `SaveDataToWorkBuffer(&0x18C, 4)` y `SaveDataToWorkBuffer(save, 0x18C)`, devuelve 1. `Load` (0x5D4640): `LoadDataFromWorkBuffer(&tam, 4)`, `LoadDataFromWorkBuffer(save, 0x18C)`, `Extract`, devuelve 1.
* `CanSetPedState` (estados 0x36, 0x37, 0x3F, 0x3B, 0x39, 0x3C), `CanBeArrested` (0x36, 0x37, 0x3F, 0x3B, 0x3D), `CanStrafeOrMouseControl` (1, 9, 8, 0, 0x16, 0x10, 0x11, 0x29, 0x24), `IsAlive` (0x36, 0x37), `IsPedInControl` (máscara 0x600 de +0x46C y estados 0x36, 0x37, 0x3F), `IsPlayer` (tipo 0 ó 1).
* `CanBeDeleted` (0x5DF100: bit 8 de +0x46C, `FindPlayerPed(-1)` + grupo `+0x38 * 0x2D4 + 0xC09928`, `IsFollower` 0x5F69E0, y el `createdBy` 2/3 devuelve `false`) y `CanBeDeletedEvenInVehicle` (0x5DF150).
* `SetPedState` (0x5E4500): estados 0x36/0x37 liberan el punto de cobertura (0x698EF0) y, con el bit 13 de +0x474, borran el blip (`ClearBlipForEntity(2, ref)`); el EXE escribe el estado al final y la DLL al principio (no observable: ni la liberación de la cobertura ni el borrado del blip leen el estado).
* `SetPedStats` (0x5DEBC0: `[0xC0BBEC] + tipo * 0x34`), `SetPedDefaultDecisionMaker` (0x5E06E0: -2 jugador, -1 misión, `movsx` del byte +0x32 de la estadística) y `SetStayInSamePlace` (0x481090: bit 21 de +0x46C con el bit 0 del argumento).
* `FlagToDestroyWhenNextProcessed` (0x5E7B70): mismo orden de llamadas (`CleanUpOldReference(&veh->m_pDriver)` o `RemovePassenger`, estado ABANDONED solo para jugadores con vehículo no destrozado, `IsVehiclePointerValid`, estado DEAD/NONE según `createdBy == 2`).

## Método

* El peatón probado es un búfer del tamaño de `CPed` (0x79C) dentro del **almacenamiento de un pool de peatones falso** (140 huecos de 0x7C4 bytes, estados de hueco propios). El puntero del pool (0xB74490) se cambia solo durante la llamada probada: así `GetRef`/`GetIndex` de la DLL (que hacen `assert` de pertenencia) y el cálculo inlinado del EXE (`(ptr - almacenamiento) / 0x7C4`) trabajan sobre el mismo objeto. El "jugador" que devuelven `FindPlayerPed`/`FindPlayerGroup` es otro hueco falso con `CPlayerPedData` propio; los vehículos, la inteligencia, las tareas, la estadística, la tabla de relaciones (acquaintance) y los model infos (`ms_modelInfoPtrs[19990/19991]`) son búferes falsos. Nunca se toca un peatón real, el jugador ni el pool real.
* **Constructor**: se llama en 0x5E8030 sobre el hueco relleno con 0xCD (incluida la guarda de 0x28 bytes tras el `CPed`); se compara el peatón entero (palabra a palabra) más la guarda, el retorno (`this`) y la traza. La base `CPhysical` (0x542260 y el constructor de la DLL, localizado como primer llamado de `CPed::CPed`, contrastado con el de `CPhysical::Constructor`) se sustituye por un doble que rellena 0..0x138 con bytes por caso. Se simulan y trazan: `Initialise` de los audios (0x4E6AA0, 0x4E0E80), `GetPedTypeAcquaintances`, `GiveWeapon`, `operator new`/constructor de `CPedIntelligence`, `CTask::operator new`, `CTaskManager::SetTask/SetTaskSecondary` (con el tipo de la tarea y sus palabras +4..+0x1C en vez de la vptr), `CPopulation::UpdatePedCount`, `rand`/`GetRandomNumber`, y para el truco `CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD`: `FindPlayerPed`, `GetPedFlag`, `SetAsAcquaintance`, `CEventGroup::Add` (tipo y peatón del evento), `RegisterReference` y `CleanUpOldReference`. Los constructores de `CPedIK`, de la voz (0x4E4F10), el vector de `CWeapon` y las tareas de cara/quieto se ejecutan de verdad en ambos lados.
* **Destructor**: se llama en 0x5E8620 sobre un peatón con bytes aleatorios (puntero de vehículo, fuego, cobertura, inteligencia, objetivo de mirada, índice de cerebro de script 0..69, `bMiamiViceCop` y `CPopulation::NumMiamiViceCops` por caso, `ScriptsForBrains` aleatorio) con los tres subobjetos de audio a cero y la vptr de la voz apuntando a una vtable falsa (hueco 2 = `Terminate` trazado). Se simulan: `RecordPedDeleted`, `SetMissionDoesntRequireModel`, `RemoveFromWaitingForScriptBrainArray` (solo cuentan los 16 bits bajos: el EXE empuja el índice con cero extendido, la DLL con signo), `CWorld::Remove`, `ClearBlipForEntity`, `RemoveConversationForPed`, `CleanUpOldReference`, `CFire::Extinguish`, `ReleaseCoverPointForPed`, `ClearWeapons`, `UpdatePedCount`, los `Terminate` de los tres audios, `~CPedIntelligence` y su `operator delete`, y `~CPhysical`.
* La vptr de 0x0 (y las del audio 0x138, 0x1E0, 0x294, 0x394) se comparan por tipo: la del EXE debe estar en 0x850000..0x880000 (EXE) y la del candidato dentro del módulo de la DLL.
* Enmascarado en el constructor: los bits 118..127 de los flags (palabra +0x478, bits 22..31; el EXE conserva lo que había, la DLL no nombra esos bits) y los 2 bytes de relleno de cada `CWeapon` (+0x16/+0x17).
* `IsPointerValid` recorre punteros dentro del almacenamiento (con desplazamiento dentro del hueco), huecos fuera de 0..139 y punteros por debajo del inicio (nunca con índice 0 y puntero fuera del almacenamiento, que desreferenciaría memoria ajena).
* Se guardan y restauran en cada ejecución: el puntero del pool de peatones, los trucos (0x969130), `CPopulation::NumMiamiViceCops`, `ms_apPedStats`, `ScriptsForBrains` y dos model infos. Los valores del reloj/RNG se simulan (tabla de 16 valores con 0x3FFF, 0x3FFE, 0x4000...).

## No cubierto y por qué

* **Subobjetos de audio del constructor** (`m_pedAudio` 0x138, `m_pedSpeech` 0x294, `m_weaponAudio` 0x394): la DLL los construye con los constructores C++ (valor-inicializan `m_tempSound`, `m_Sounds`, etc.) y el EXE solo escribe unos pocos campos. En el constructor la comparación de 0x138..0x43C es **informativa** (campos `audio_diffs` y `audio_extra_writes` de cada registro; no cuenta para el fallo). Quitarlo exige tocar las clases de audio (los miembros son protegidos y los envoltorios `Constructor()` privados). Diferencia conocida con valor: `CAEPedSpeechAudioEntity()` pone `m_PedAudioType = PED_TYPE_UNK (-1)` y el EXE `PED_TYPE_GEN (0)` (`Initialise` lo reescribe luego). El **destructor** sí compara 0x138..0x43C estrictamente (salvo las cuatro vptr).
* Los constructores y destructores de `CPhysical`, `CPedIntelligence`, los de los audios (`Initialise`/`Terminate`) y el resto de colaboradores se simulan: su validación es de sus bancos. Tampoco se prueba la llamada real a `CTaskManager::ChangeTaskInSlot` ni al pool de tareas (se simula `CTask::operator new` con dos búferes de 0xCD).
* No se prueba `CTask::operator new` / `CPedIntelligence::operator new` devolviendo `nullptr` (pool lleno): el EXE comprueba el nulo y sigue; con la DLL el comportamiento depende de cómo compile el `new` con un `operator new` propio, y el pool real no se toca.

## Supuestos a comprobar al compilar y ejecutar

1. Compilación en Debug sin inlinado (`/Ob0`): `CTaskManager::SetTask/SetTaskSecondary` (en línea en la cabecera), `CEntity::RegisterReference`, `CPed::GetAE` y las funciones de `Ped.h` deben ser funciones reales para poder parchearlas.
2. El primer llamado con prólogo `push ebp / mov ebp,esp` dentro de `CPed::CPed` es `CPhysical::CPhysical` y coincide con el de `CPhysical::Constructor` (evento `warning` si no). Igual para el constructor/destructor de `CPedIntelligence` y el cuerpo de `CAEPedSpeechAudioEntity::Terminate`.
3. `sizeof(CPed) = 0x79C`, `sizeof(CCopPed) = 0x7C4` (`static_assert`), los desplazamientos de la DLL (evento `layout` con `layout_ok`), `sizeof(CPedIntelligence) = 0x294` (la traza de `operator new` lo compara) y `sizeof(CTaskComplexFacial) = sizeof(CTaskSimpleStandStill) = 0x20`.
4. El nombre de las categorías de hooks `CPed`, `CPhysical`, `CPedIntelligence` y `CAEPedSpeechAudioEntity` (`FindHook`).
5. Los constructores de tareas (`CTaskComplexFacial`, `CTaskSimpleStandStill`) escriben los mismos bytes que los del EXE: el banco compara las palabras +4..+0x1C que ven `SetTask`/`SetTaskSecondary` y el resto del búfer de la tarea (sin la vptr); una diferencia ahí es del constructor de la tarea, no de `CPed`.
6. El destructor del evento (`CEventAcquaintancePedHate`, truco del precio por la cabeza) llama a `CleanUpOldReference` igual en el EXE (0x4AF890) y en la DLL; si no, el banco lo mostrará en la traza.

Repetir: `python reversing/CPedD/build_probe.py` y `python reversing/CPedD/run_local.py` (el caso `cpedd` aún no está registrado en `reversing/cases.json`).
