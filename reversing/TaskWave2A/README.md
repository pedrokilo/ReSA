<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TaskWave2A: CTaskComplexKillCriminal, CTaskComplexKillPedOnFootArmed y CTaskSimplePlayerOnFoot (parcial)

Banco simple (estilo Small2/PadMouse1/TaskFinal2, juego completo con partida cargada). Compilado y ejecutado: aprobado el 09/10/2026 (ver "Aprobacion" al final).

Uso: `bash reversing/_handoff/bench.sh TaskWave2A 3`. Filtro opcional: `taskwave2a-only.txt` en la carpeta del juego con `Clase::Funcion`.

## Funciones (21) y casos: 12736 pares

| Clase | Funcion | EXE | Casos |
|---|---|---|---|
| CTaskComplexKillCriminal | Constructor / Destructor | 0x68BE70 / 0x68BF30 | 256 / 256 |
| | Clone | vtable 0x870A00 hueco 1 (0x68CE50) | 128 |
| | CreateSubTask | 0x68C050 | 1024 |
| | FindNextCriminalToKill | 0x68C3C0 | 1024 |
| | ChangeTarget | 0x68C6E0 | 1024 |
| | MakeAbortable | hueco 6 (0x68DAD0) | 1024 |
| | CreateNextSubTask | hueco 8 (0x68E4F0) | 1024 |
| CTaskComplexKillPedOnFootArmed | Constructor / Destructor | 0x621190 / 0x621250 | 128 / 128 |
| | Clone | vtable 0x86D918 hueco 1 (0x6234C0) | 128 |
| | MakeAbortable | hueco 6 (0x6212B0) | 256 |
| | IsPedInLeaderFiringLine | 0x621300 | 1024 |
| | LineOfSightClearForAttack | 0x621500 | 1024 |
| | CreateFirstSubTask | hueco 9 (0x62BF00) | 1024 |
| | ControlSubTask | hueco 10 (0x62CCE0) | 1024 |
| CTaskSimplePlayerOnFoot | Constructor / Destructor | 0x685750 / 0x6857D0 | 64 / 64 |
| | Clone | vtable 0x8708EC hueco 1 (0x68AFF0) | 64 |
| | MakeAbortable | hueco 6 (0x6857E0) | 1024 |
| | ProcessPed | hueco 7 (0x688810) | 1024 |

## Correcciones de fuente (direccion + instruccion del EXE)

CTaskComplexKillCriminal
- Constructor 0x68BEE7 `cmp eax,18h / jge`: el tipo de ped >= 24 (cualquier MISSIONn) anula al criminal; antes solo MISSION1. El EXE guarda el puntero y solo registra la referencia si pasa los filtros (0x68BEF6 `call 571B70`); +0x1B..+0x1D no se escriben (se quitaron los `{}` de m_OrigDrivingMode, m_OrigMission y m_OrigCruiseSpeed).
- Destructor 0x68BFCD `jne 68C01F`: la sirena (`and byte [veh+42Dh],7Fh`, 0x68C018) solo se limpia dentro de la rama del conductor; sobraba la limpieza fuera de ella.
- CreateSubTask 0x68C361 / 0x68C333: `SetCurrentWeapon(22)` (0x68C37A, `call 5E6280`) va DESPUES de construir la tarea, tambien en GANG_DRIVEBY (0x68C308 comparte el final 0x68C36A); antes iba antes y GANG_DRIVEBY no la llamaba. Tipos sin caso (0x68C0BA `ja`, 0x68C2EE `jne`) devuelven 0 en vez de `NOTSA_UNREACHABLE`.
- FindNextCriminalToKill 0x68C3C0..0x68C6D2: no es "el mas cercano"; la distancia de referencia ([esp+0Ch]) es la del criminal actual si `!any` y esta vivo (si no 0.0) y NO se actualiza; gana el ULTIMO candidato valido con d < referencia (o cualquiera si la referencia es 0); d^2 = (dz*dz + dy*dy) + dx*dx; vivo = health > 0 (`fcomp 0.0 / test ah,41h / jne`).
- ChangeTarget 0x68C760..0x68C777 `fcomp ... test ah,41h / je`: el objetivo debe estar en la lista Y vivo; 0x68C7C6 `FindTaskByType(0x451)` (0x600EE0) sobre la inteligencia del companero y se llama a ChangeTarget sin mas comprobaciones.
- MakeAbortable 0x68DC02 `fcomp 625.0 / test ah,5 / jp`: `d^2 < 625` estricto (antes `<=`); `GetSourceEntity` se vuelve a llamar en cada uso (0x68DB63, 0x68DB72, 0x68DB86, 0x68DBA7, 0x68DC0F); 0x68DBB3 `test eax,eax / je`: el -1 de AddCriminalToKill cuenta como exito (se quito la rama FIX_BUGS).
- CreateNextSubTask 0x68E547 `mov byte [esi+1Ah],1`: ENTER_CAR_AS_DRIVER sin estar en vehiculo pone m_CantGetInCar; 0x68E5D8 `test dword [ecx+46Ch],ebp / je 68E5EA` (criminal en vehiculo => 0x68E635, perseguir en coche; a pie => d^2 <= 625 va a pie): la logica estaba invertida; 0x68E785 `cmp [edi+58Ch]`: LEAVE_CAR comprueba m_pVehicle, no bInVehicle; subtareas sin caso (0x68E936) devuelven 0.

CTaskComplexKillPedOnFootArmed
- Constructor 0x621190: m_strafeDir (+0x31) no se escribe (quitado `{}`).
- IsPedInLeaderFiringLine 0x621398..0x6214E6: direccion lider->objetivo con z = 0 y normalizada (`mov [esp+18h],0` + `call 59C910`); proyeccion = dir*dot (+ lider) con dot (y+z)+x; 4.0 y 100.0 con `test ah,5 / jp` (estrictos); el producto escalar final usa el vector SIN normalizar (z+y)+x con `fcomp 0.0 / test ah,41h / jne`.
- LineOfSightClearForAttack 0x62152C..0x62179B: umbrales dinamicos (10000 ms / 9.0 por defecto; 5000 ms y 4.0 o 9.0 si el ped esta en un interior [ped+2Fh], o el objetivo es jugador y ([0xB72914] != 0 o creado por mision), segun [0xB7298C] >= 2); `ja` en 0x62159F (solo se vuelve a comprobar si paso MAS que el umbral); objetivo nulo tolerado en la comprobacion de movimiento; agachado (bit 26 de +46Ch, 0x62163A) usa posicion + 0.25 y de pie la cabeza (estaba al reves); las posiciones se calculan antes de desactivar la colision del vehiculo del objetivo.
- CreateFirstSubTask 0x62BF22 `mov byte [edi+34h],0` (m_newTarget = false, antes de mirar el objetivo); 0x62BFD7 `call 821B1E / test al,1` (no CGeneral::RandomBool); 0x62BFEF/0x62C004 distancia (fsqrt) > 30.0 o (> 6.0 y armado), no al cuadrado; 0x62C06A llamada incondicional a FindCoordinatesCoverPoint y 0x62C06F `fadd 1.0` (faltaba coverPos.z += 1); 0x62C10B `fmul [858C84h]`: umbral (rango*rango)*0.25 (antes (rango/4)^2); quitada la rama FIX_BUGS.
- ControlSubTask 0x62CDB3 `fcomp 400.0 / test ah,5 / jp`: primero la distancia^2 estricta y SOLO entonces `call 407180` (0..100) < probabilidad (sin signo, `jae`); antes la rama de ducking consumia el aleatorio siempre y con `<=`; 0x62CDFB igual (RandomBool -> 407180); GUN_CTRL 0x62CEC2..0x62CF26 usa la distancia (`call 4082C0`, con raiz) con `dist > rango`, `rango*0.5 < dist && !stay && ahora > ultimoAtaque + 0x9C4`, `dist > 4.0 && !stay && ahora > shootTimer` (antes cuadrados, `>=` y 2000); 0x62CFBA y 0x62D020 `test [edi+46Ch],200000h`: la rama de strafe es `ahora < lastStrafe && !stay` y el avance recto `dist < 4.0 && !stay` (la bandera estaba invertida); 0x62CFC9/0x62D048 `ja`: direccion de strafe > 3 no hace nada (antes `NOTSA_UNREACHABLE`); SEEK_ENTITY 0x62D12D..0x62D1F6: `rango*0.5 > dist` (Magnitude) y (`dot(velocidad, adelante) < 0` o `ahora - ultimoAtaque > 2000`), luego lider -> (cerca o quieto y LOS) -> (sumergido y LOS) con hasta dos llamadas a LOS; 0x62CE3C quitada la rama FIX_BUGS de m_lastDuckTime.
- MATCH: Destructor 0x621250, Clone 0x6234C0 (copia m_aimImmediate +0x35), MakeAbortable 0x6212B0.

CTaskSimplePlayerOnFoot (revisadas, MATCH, sin cambios de fuente): Constructor 0x685750, Destructor 0x6857D0 (vptr + salto a 0x61A3A0), Clone 0x68AFF0, MakeAbortable 0x6857E0, ProcessPed 0x688810.

## Fuera del banco (CTaskSimplePlayerOnFoot, 5 funciones) y por que

PlayerControlDucked 0x687F30, PlayerControlZeldaWeapon 0x687C20, PlayerControlZelda 0x6883D0, ProcessPlayerWeapon 0x6859A0 y PlayIdleAnimations 0x6872C0 NO se han revisado contra el EXE ni se prueban: ejecutan el pad real del jugador (CPad::GetPad, botones y raton), la camara real (TheCamera, modos de arma y apuntado), el gestor de animaciones y el IK (RpAnimBlendClumpGetAssociation, BlendAnimation, CIKChainManager), el sistema de armas del jugador y eventos de dano con crimenes (CCrime::ReportCrime, CPedDamageResponseCalculator): ~12 KB de codigo (0x6859A0..0x688810) que depende de un CPlayerPed y un CPad vivos y escribe en singletons del juego. Doblarlos con seguridad exigiria simular decenas de colaboradores y no se puede comparar en una sola trama sin tocar estado real (regla 17). ProcessPed SI esta en el banco con esas cinco funciones simuladas (se traza cual se llama y con que argumentos).

## Intercepcion

Mismo stub en la direccion del EXE y en el simbolo de la DLL (entrada incremental y cuerpo, calculados ANTES de crear ningun `EntryPatch`), solo activos en el hilo del banco (`owner`/`Mine()`): ver la cabecera de `LiveComparison.hpp`. Para las estaticas de CTaskSimplePlayerOnFoot el EXE usa `ret 4` y la DLL cdecl, asi que cada lado lleva su stub. `CTaskComplexKillPedOnFootArmed::CreateSubTask` (0x626FC0, ya validada) se simula y devuelve la tarea falsa 9.

## Fixtures

Peatones (cop con CCopPed completo), criminal, companero, extras, lider del grupo, vehiculos, inteligencia, datos de jugador, grupo falso (lider en +0x28), tareas falsas con vtable trazada (GetSubTask, GetTaskType, MakeAbortable, destructor) y silenciosas (las que busca la inteligencia por tipo: duck, use gun, fight; el numero de lecturas de GetTaskType no es comparable), evento falso con vtable trazada (tipo, prioridad, GetSourceEntity) y objetos de tarea con desplazamientos crudos del EXE. Fronteras exactas: d^2 = 625 (MakeAbortable, CreateNext), 4.0, 6.0, 30.0 y rango/2 (CreateFirstSubTask, ControlSubTask), 2.0 y 10.0 (IsPedInLeaderFiringLine), umbrales de tiempo 2000, 2500, 3000, 5000 y 10000 ms (LOS, Control). Listas de criminales con huecos y duplicados; en MakeAbortable el hueco 0 siempre esta ocupado y queda uno libre (AddCriminalToKill nunca devuelve 0 ni lista llena). Los globales 0xB72914 y 0xB7298C se modifican durante la llamada y se restauran.

## Que se compara

Retorno, memoria completa del objeto probado (y del segundo objeto, la tarea KillCriminal del companero), tareas nuevas del pool (tipo + palabras sin vptr), memoria de las tareas falsas, huella de todos los buffers (referencias enmascaradas), listas de referencias antes y despues del borrado, traza completa (orden y numero de llamadas virtuales, rand y colaboradores), palabra de control x87 y pila x87 vacia. El x87 corre a 24 bits.

## Supuestos a revisar al compilar

- Nombres usados en el banco: `AREA_CODE_1`, `AREA_CODE_NORMAL_WORLD`, `VEHICLE_TYPE_BIKE`, `EVENT_*`, `TASK_SIMPLE_USE_GUN`; si alguno no existe, sustituir por el valor numerico (1, 0, 9...).
- `Addr(&clase::miembro)` requiere punteros a miembro de 4 bytes (miembros no virtuales); `CCamera::Using1stPersonWeaponMode` es `const`.
- Los constructores y destructores de las subtareas creadas por KillCriminal (EnterCar, CarDrive, LeaveCar, GangDriveBy, KillPedOnFoot) se ejecutan de verdad sobre un vehiculo falso del tamano completo: solo registran y limpian referencias; CTaskComplexCarDrive escribe en `m_autoPilot`.
- Si el lider del grupo no esta en +0x28 del grupo falso (membresia +8, lider +0x20), `IsPedInLeaderFiringLine` solo ejercitaria la ruta de grupo ausente.
- El EXE usa `fsqrt` a 24 bits; `std::sqrt(float)` de la DLL debe dar el mismo resultado.
- El constructor de KillCriminal ya no pone a cero +0x1B..+0x1D y el de KillPedOnFootArmed no escribe +0x31: las comparaciones dependen del relleno 0xCD de los huecos del pool y del buffer del objeto.

## Enmascarados del banco (09/10/2026, tras la primera ejecucion)
- Constructores: se compara el tipo de tarea (GetTaskType) en lugar de la vptr (EXE 0x870A00 vs vtable de la DLL).
- Destructores: se enmascaran la vptr final (vtable base del EXE vs DLL) y el retorno (EAX sin definir en el EXE).
- KillCriminal::MakeAbortable: si no se registra el cop (+0x10) se pone a 0; antes quedaba 0xCDCDCDCD y el EXE (0x68DB98..0x68DBAE) lo desreferenciaba -> violacion de acceso en CCopPed::AddCriminalToKill (fallo del banco, no de la fuente).

## Aprobacion (09/10/2026)

Corrida aprobada `20261009-024531-244414`: 12736 pares, 0 diferencias, state_restored, saves y configuraciones intactos; sonda 17b6765e... Evidencia en `runs/20261009-024531-244414/` (jsonl en .gz con hash verificado), resumen en `validation-summary.json`, registro con `register_taskwave2a.py` (21 funciones) y documentacion con `refresh_documentation_cut216.py`. El banco ejecutado incluye ProcessPed de CTaskSimplePlayerOnFoot (5 de 10 funciones; la cabecera de `LiveComparison.hpp`, fijada por hash en el manifiesto, aun dice 4 de 10). Quedan COMPLETAS segun hooks.csv CTaskComplexKillCriminal y CTaskComplexKillPedOnFootArmed; CTaskSimplePlayerOnFoot conserva 5 pendientes, FUERA del banco y SIN credito: PlayerControlDucked 0x687F30, PlayerControlZelda 0x6883D0, PlayerControlZeldaWeapon 0x687C20, ProcessPlayerWeapon 0x6859A0 y PlayIdleAnimations 0x6872C0. Limites: vptr y retorno enmascarados en constructores y destructores (se compara el tipo), fixtures corregidos tras la primera corrida (KC_ABORT con cop registrado, P_CLUMP con clump falso), sqrt a 24 bits, colaboradores trazados sin credito. La corrida fallida anterior (`previous.jsonl`) no se conserva.
