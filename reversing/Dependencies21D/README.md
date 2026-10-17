<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21D: CTaskSimpleGangDriveBy (constructor y tres ayudantes de ProcessPed)

PENDIENTE: banco nuevo, escrito y verificado solo por compilacion y por un arnes local fuera del juego (ver mas abajo); NO se ha ejecutado dentro de GTA.
No constituye validacion hasta que el coordinador lo compile (`build_probe.py`) y lo corra (`run_local.py`).

## Funciones

| Funcion | EXE | Hook RH | Pares |
| --- | --- | --- | --- |
| `CTaskSimpleGangDriveBy::Constructor` (antes `FUN_006217d0`, llamada desde GangWars.cpp por direccion) | `0x6217D0` | `RH_ScopedInstall(Constructor, 0x6217D0)` | 1024 |
| `CTaskSimpleGangDriveBy::PlayerTarget` (objetivo del jugador) | `0x621960` | `RH_ScopedInstall(PlayerTarget, 0x621960)` | 2048 |
| `CTaskSimpleGangDriveBy::AITarget` (objetivo de un NPC) | `0x627600` | `RH_ScopedInstall(AITarget, 0x627600)` | 4096 |
| `CTaskSimpleGangDriveBy::AimGun` (apuntado con `CPedIK::PointGunInDirection`; NO dispara: el disparo es `0x627CC0`) | `0x628350` | `RH_ScopedInstall(AimGun, 0x628350)` | 4096 |

Total: 11264 pares. Cada par ejecuta la MISMA entrada del EXE con el hook apagado (bytes originales) y activo (version de la DLL), a traves de `OriginalDependencies499::HookRoute`.

## Que se compara (todo en bits; cualquier diferencia rechaza el banco)

- Valor de retorno: EAX del constructor. En los tres ayudantes (`ret 4`, sin valor) NO se compara EAX/ECX/EDX: ningun llamador los lee (evidencia: los unicos llamadores del EXE son `0x62D59B` y `0x62D5A2` (PlayerTarget/AITarget; siguen `jmp`/`cmp [esi+0x28], edi` y `push ebp; mov ecx, esi`, que escriben ECX y solo leen EDI, no volatil) y `0x62D813` (AimGun; sigue `mov di, [esi+0x16]`)). Los 13 llamadores del constructor solo usan EAX (el resto de registros se sobrescribe en las 4-6 instrucciones siguientes; ver gd-classification.md).
- Memoria tocada: arena completa (tarea, ped 0x7A0, vehiculo 0xA20, datos del jugador, PlayerInfo, entidad objetivo 0xA40, arma, asociacion de animacion, cuatro matrices, vector de salida) con guardas de 16 bytes (0xA5) entre objetos, mas la `CCamera` real completa (`sizeof(CCamera)`), como lista de rangos modificados (desplazamiento y bytes nuevos). En el constructor se excluye el puntero de vtable (EXE: `0x86D944`; DLL: vtable de la DLL), que se comprueba aparte (`vptr_ok`: original == `0x86D944`, candidato dentro del modulo de la DLL).
- Traza ordenada de colaboradores con todos sus argumentos como bits (NaN con carga distinta, señalizantes, inf, ceros, subnormales llegan intactos), y etiquetas de puntero relativas a la arena (la pila del llamador se etiqueta `local`).
- `errno` del EXE (`_errno()` en `0x82B181`): el `asin` del EXE (`0x821E70`) y `ExeAsinDoubleCpp` ponen EDOM (33) con argumentos fuera de dominio.
- ABI: ESP, EBX/ESI/EDI/EBP, `fs:[0]` (cadena SEH; el constructor la instala) antes y despues.
- x87: `FNSAVE` tras cada lado (`kCompareDeadX87Registers = true`, sin relajar): palabra de control sin cambios, TOP igual, pila vacia (`ftw == 0xFFFF`), palabra de estado completa (`C0..C3` y las excepciones pegajosas `IE/DE/ZE/OE/UE/PE`) y contenido crudo de los ocho registros ST, tambien los vacios. Antes de cada llamada la pila se carga con un residuo conocido y distinto por registro (`BuildPrime`, ocho `FSTP ST(0)` en `CallOne`/`CallSix`) y con uno de cuatro FSW iniciales (TOP, `C0..C3`, `PE`), asi que se ve lo que cada lado deja o no escrito. FIP/FCS/FDP/FDS/FOP no se comparan (cada lado ejecuta su ultima instruccion x87 en otra direccion).
- Como el C++ deja la x87 como el EXE: las comparaciones del EXE (`fld [a]; fcomp [b]`) se hacen en `X87Math.cpp` con `GangDriveBy{Gt,Lt,Ge,Le}Cpp` (en este archivo el codigo es SSE y no tocaria la x87); `GangDriveByAimPointCpp`, `GangDriveByAiGeometryCpp`, `GangDriveByAimRangeCpp` y `GangDriveByBikeElevationCpp` escriben al final, con aritmetica neutra (`X87PlantT*Cpp`), los registros muertos que deja el EXE; el truncado de `_ftol2` (R y v - R) lo hace `Ftol2RegsDblCpp`; ver `g2de-informe.md` de la entrega.
- Restauracion: arena, camara, `errno` y estado FNSAVE/MXCSR vuelven a su valor inicial tras cada par; el hook se restaura al final.

## Colaboradores falsos (EntryPatch en la direccion del EXE y en la entrada de la DLL, mismo fake)

`CPed::IsPlayer` (0x5DF8F0), `CGameLogic::IsCoopGameGoingOn` (0x441390), `CCamera::SetNewPlayerWeaponMode` (0x50BFB0), `CCam::Get_TwoPlayer_AimVector` (0x513E40), `CPlayerPed::GetPlayerInfoForThisPlayerPed` (0x609FF0), `CCamera::Find3rdPersonCamTargetVector` (0x514970), `CPed::GetBonePosition(CVector*, eBoneTag, bool)` (0x5E4280), `CGeneral::GetRadianAngleBetweenPoints` (0x53CBE0), `CPlaceable::AllocateMatrix` (0x54F560), `CSimpleTransform::UpdateMatrix` (0x54F1B0), `CPedIK::PointGunInDirection` (0x5FDC00), `CEntity::RegisterReference(CEntity**)` (0x571B70). Solo en la direccion del EXE (la DLL los llama por direccion con `plugin::`): `0x621B10` (comprobacion de linea de vision / alcance del propio `CTaskSimpleGangDriveBy`, dependencia SIN catalogar ni reconstruir; falsa) y `0x59C890` (`CMatrix * CVector`, real del EXE: es pura). Ademas son reales en los dos lados: `_ftol2` (0x821B40 / el de la DLL), `_CIasin` (0x821E70, frente a `ExeAsinDoubleCpp`), `fsin/fcos/fpatan/fsqrt` por hardware. Los fakes reciben los float como bits (`uint32`) porque el EXE los empuja con `mov` enteros.

## Perfiles (cobertura exigida por `run_local.py`)

- Constructor: con/sin entidad objetivo, con/sin posicion, estilos 0..8, frecuencia 0..255, asiento 0/1, distancia de aborto de la reserva de estres.
- PlayerTarget: 8 modos de camara (0x37, 0x31, 0x35, 0, 0x30, 0x41), 3 indices de camara activa, cooperativo si/no, jugador si/no, vehiculo con matriz o con heading, datos de jugador si/no, asiento derecho si/no; las ocho animaciones 231..238 deben aparecer.
- AITarget: estilos 0..8 mas 9, 0x7F, 0x80, 0xFE, 0xFF; sin objetivo / ped / vehiculo / objeto, matriz del objetivo creada o existente, ped y vehiculo con o sin matriz, linea de vision si/no, ya alcanzado el rango de aborto, zona de tiro dentro/fuera (`m_bInRangeToShoot` 0 y 1), aborto (`m_nNextCommand == 7`), las ocho animaciones mas 191 (sin cambio).
- AimGun: jugador (camara 0x37 / 0x31 con rango `GangDriveByAimRangeCpp`, otros modos: salida sin hacer nada) y NPC (sin objetivo, coordenadas cero o no, ped (hueso 3), vehiculo/objeto (posicion o matriz), matriz creada); moto (tipo 9) y no moto; ped sin vehiculo; las animaciones 0xE7..0xEF (bandera de `PointGunInDirection` 0 y 1, giro de ±pi/2 / pi); NaN/inf/subnormales en posiciones, matrices, `forward[2]` (clamp + `asin` con EDOM) y en los valores que devuelve el falso de `GetRadianAngleBetweenPoints`.
- Estres comun: ~22 % de los float de entrada salen de una reserva con NaN con carga distinta (+/-), señalizantes (`0x7FA00000`, `0xFFA00001`, `0x7F800001`), +/-inf, +/-0, subnormales, +/-1e30, +/-FLT_MAX y +/-1 ulp de los umbrales (15°, 75°, 105°, 165°, pi/4, 3pi/4, +/-pi).

## Evidencia previa (arnes local fuera del juego)

`harness/` ejecuta los bytes REALES del EXE (imagen mapeada en sus direcciones en un proceso hijo de 32 bits, con los mismos colaboradores falsos y la x87 a 24 bits) frente a las funciones de la DLL (`TaskSimpleGangDriveBy.cpp` con tipos falsos del mismo desplazamiento y `X87Math.cpp` compilado con las opciones del proyecto): 500 000 casos por funcion (5 semillas x 100 000) con cero diferencias en memoria, traza de colaboradores, errno, ESP, registros no volatiles, SEH, TOP, FCW, pila vacia, FSW completa y los diez bytes de cada registro ST (con residuo inicial distinto en cada uno y cuatro FSW iniciales); mutaciones del codigo (sumando 3 en vez de 4 por asiento derecho, sustituyendo la suma de pi/2 por una resta) se detectan. NO sustituye a este banco: no usa los hooks ni los objetos reales del juego.

## Limites

- Los colaboradores falsos delimitan la equivalencia: no se valida la integracion con IA real, camara real, animaciones, pools ni audio. `0x621B10` y `0x59C890` quedan como dependencias del EXE (sin credito).
- Rama de moto de AimGun: el EXE lee `forward[2]` sin comprobar la matriz; los fixtures de moto siempre llevan matriz (sin ella el EXE y la DLL acceden a `nullptr+0x18`). Los fixtures de AITarget siempre llevan arma (`m_pWeaponInfo`) y vehiculo; los de AimGun asocian siempre una animacion (`m_pAnimAssoc`).
- Bytes altos de la bandera empujada a `PointGunInDirection` (`push eax` con la pila sin inicializar en el EXE): no se comparan (solo el byte bajo).
- Los argumentos `bool`/`int8`/`eDrivebyStyle` del constructor se pasan con el valor limpio (0/1, 0..8, 0..255), que es lo que empujan los 13 llamadores del EXE; el constructor guarda el byte bajo en el EXE y en la DLL, sin normalizar.

## Ejecucion

Desde la raiz del repositorio, con el proyecto compilado con la fuente convertida (`TaskSimpleGangDriveBy.cpp/.h`, `GangWars.cpp`, `X87Math.cpp/.h`):

```
python -B reversing/Dependencies21D/build_probe.py
python -B reversing/Dependencies21D/run_local.py
python -B reversing/Dependencies21D/run_local.py --analyze <comparisons.jsonl>
```

o `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies21D`. Antes de dar por buena una corrida: `python -B contrib/audit_register_residue.py` (esta tanda no anade objetos: ver gd-audit.txt).
