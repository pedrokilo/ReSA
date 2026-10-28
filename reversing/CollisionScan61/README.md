<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CollisionScan61: CCollisionEventScanner::ScanForCollisionEvents (0x604500)

Estado: **APROBADO** en el juego (4096 pares, EXE contra DLL, ambos lados contra el oraculo independiente).
Ultima corrida aprobada: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-collisionscan61/20261019-235131-985269`.
No se ha ejecutado `register_hook.py` ni se ha tocado `docs/`, `hooks.csv` ni los registros.

## Comandos

```
cd D:\Original\wd-3
python reversing/CollisionScan61/build_probe.py            # compila la sonda (errores: grep "error C" reversing/CollisionScan61/artifacts/probe-build.log)
bash reversing/_handoff/bench_here.sh CollisionScan61      # compila y corre en el juego con candado global; APROBADO / FALLO
python reversing/CollisionScan61/fixture_oracle.py         # autoprueba del oraculo (imprime ok)
python -B reversing/CollisionScan61/run_local.py --analyze <carpeta>/comparisons.jsonl   # solo analiza (None si el registro no existe aun)
python reversing/CollisionScan61/register_hook.py <carpeta de evidencia>                 # SOLO el coordinador, tras revisar SCOPE_NOTE
```

## Produccion

- `source/game_sa/Collision/CollisionEventScanner.cpp/.h`: la funcion pasa de envoltura al EXE a C++ (sin `__asm`, sin `plugin::Call*` hacia el EXE para ella) y se anade
  `static void InjectHooks()` con `RH_ScopedClass(CCollisionEventScanner); RH_ScopedCategoryGlobal(); RH_ScopedInstall(ScanForCollisionEvents, 0x604500);`.
- `source/InjectHooksMain.cpp`: linea `CCollisionEventScanner::InjectHooks();` (justo despues de `CPedStuckChecker::InjectHooks();`). Esa linea NO esta en `source.patch` (el parche solo lleva los dos archivos de la clase).
- `source.patch` = `git diff HEAD` de esos dos archivos; comprobado con `git apply --check` contra los blobs de HEAD en un directorio temporal.
- Fila esperada de `docs/hooks.csv` (la anade el coordinador): `CCollisionEventScanner,ScanForCollisionEvents,0x00604500,1,0,1,S`.

## Que hace la funcion (desensamblado 0x604500..0x6052E0; thiscall, ECX = escaner de 1 byte, 2 argumentos de pila, `ret 8`, void)

Sale si el peaton (victima) no tiene entidad danadora (`+0xDC`) o su intensidad (`+0xD8`) es exactamente 0.0 (`fcomp` + `test ah,44h` + `jnp`: solo el igual salta; con NaN sigue).
Obtiene `ms` = `[tarea+8]` si la tarea activa (`GetSimplestActiveTask`) es de tipo GoTo (`IsGoToTask`), o 1 si no. Segun `[danador+0x36] & 7` (tabla de salto 0x6052E4):

- edificio (1): `CEventBuildingCollision` -> `Add` al grupo recibido.
- vehiculo (2) (si el byte del escaner es 0): velocidad al cuadrado `(x*x+y*y)+z*z` contra 0x3B23D70B; atropello de un tren sobre un peaton quieto y atascado; si rapido: dano con producto escalar
  (`E4*[554]+E0*[550]`, masa, tope 20) y, para el jugador, geometria con `GetHeading`, `GetColModel` x2, caja, `LimitRadianAngle(heading - atan2(-dx, dy))`, `atan2` de la caja y `Normalise` para decidir
  `KillPedWithCar`; no jugador: sujeto a un vehiculo -> `CEventDamage` + `AffectsPed` + `ComputeDamageResponse`, si no `KillPedWithCar`; si lento: igual sujeto, o (no jugador) `CEventVehicleCollision`.
- peaton (3): evento segun jugador (victima: `CEventPlayerCollisionWithPed` + `Say`; danador jugador: `CEventPedCollisionWithPlayer` + ira + `Say` + odio; ninguno: `CEventPedCollisionWithPed`), y si el otro
  peaton no tiene danador, el evento inverso (impacto por -1.0) en el grupo de eventos del otro peaton (`intel+0x68`).
- objeto (4): producto escalar, umbral 1.0 / 2.0 (con datos de jugador), `GetLocalDirection` + `GenerateDamageEvent(0x36, _ftol2(dano/limite*10), 3, dir)` y `[ped+0x128] = [ped+0xDC]`, o `CEventObjectCollision`.
- cualquier otro tipo: nada. Despues (salvo salidas rapidas): jugador con pasamontanas contra edificio/objeto -> `CEventSoundQuiet` al grupo global con dos temporizadores estaticos (0xC0B1B0 / 0xC0B1B4).
- Siempre al final: `[escaner] = 0`.

## Revision contra el EXE: discrepancias halladas y corregidas

1. **Defecto real de la primera version (banco)**: el umbral de velocidad al cuadrado (0x86C920 y 0x86C924, ambos) es `0x3B23D70B`, un ulp por encima de `0.0025f` (`0x3B23D70A`). Con `0.0025f` 8 de 4096 pares
   divergian (velocidad = 0x3B23D70B: el EXE la tomaba como lenta). Corregido en produccion (`std::bit_cast<float>(0x3B23D70Bu)`); el oraculo usa los mismos bits.
2. El encargo decia que la salida rapida era `dano <= 0.0 o NaN`: en realidad el EXE solo sale con igualdad ordenada a 0.0 (±0); con NaN la funcion sigue.
3. El unico llamador del EXE es `CPedIntelligence::Process` en 0x6073D7 (no 0x6026xx).
4. `0x406DA0` es `SquaredMagnitude` (no la magnitud); `0x4119D0` divide 1.0/2.0 y multiplica (exactamente *0.5); `0x420800` es `(a > b) ? a : b` (con `a = 0`); `0x59C890` evalua cada componente con su propio orden de suma
   (distinto en x que en y/z), reproducido tal cual. En la rama `rel <= 0` el EXE calcula ademas `dot(right, adelante del peaton)` sin efecto observable (los fixtures con la matriz del peaton nula evitan esa rama porque leeria [0x10] sin matriz).
5. `GetIsStatic` (0x4633E0) y `GetIsTypePhysical` (0x4DA030) son en la DLL funciones inline; no se sustituyen (se ejecutan reales en ambos lados y concuerdan por las banderas/tipo de la entidad).
6. `CEventDamage` fuerza `m_bWitnessedInVehicle = true` en el constructor (el volcado de `AffectsPed` fija `8` en el byte `+0x25`), con independencia del parametro `bPedInVehicle` que pasa el EXE.

## Que se compara

Cada caso se ejecuta con el hook apagado (EXE) y con SOLO este hook encendido (DLL), con 22 colaboradores sustituidos por falsos identicos en ambos lados (parche en la direccion del EXE y en el cuerpo de la DLL; los sin hook solo en el EXE):
`CEventGroup::Add` (volcado completo del evento: tipo, prioridad, tiempo, validez, campos editables y los propios de cada clase), `GetSimplestActiveTask`, `IsGoToTask` (sin hook), `GetMoveStateFromGoToTask`,
`GetColModel`, `RegisterReference` / `CleanUpOldReference` (constructores y destructores de los eventos de pila), `IsPlayer` (por peaton), `GenerateDamageEvent`, `Normalise` (sin hook; el falso multiplica por 0.5),
`LimitRadianAngle`, `KillPedWithCar`, `GetHeading`, `CEventDamage::AffectsPed` (virtual; hook con salto en 0x4B35A0; se vuelca el evento), `ComputeDamageResponse` (con el desplazamiento de la respuesta dentro del evento),
`ComputePedHitSide`, `Say`, `IncrementAngerAtPlayer`, `GetAcquaintances`, `GetPedFlag`, `GetIsWearingBalaclava`, `GetLocalDirection`. Quedan reales: constructores/destructores de eventos y del calculador, `GetEventGlobalGroup`,
`GetIsStatic`/`GetIsTypePhysical` y la aritmetica de vectores (el EXE ejecuta 0x406DA0, 0x40FE60, 0x40FE90, 0x40FDB0, 0x4119D0 y 0x59C890; la DLL las operaciones equivalentes).

Registro por lado: secuencia ordenada de llamadas con argumentos (punteros etiquetados `V`, `D`, `DI`, `I`, `G`, `GG`, ...), byte del escaner, cambios del peaton (solo debe cambiar `+0x128`, que se etiqueta),
temporizadores estaticos 0xC0B1B0 / 0xC0B1B4 tras la llamada, hora sin cambios, todas las demas arenas intactas, guardas intactas, y compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado
con FCW a 24 bits y pila vacia, MXCSR), `route_ok` (la entrada salta a la DLL solo en el candidato), `collaborators_dll`, `globals_restored` y `floating_state_restored`. EAX/ECX/EDX solo se registran
(`original_residue` / `candidate_residue`; difieren en todos los casos porque el EXE y la DLL dejan residuos distintos).

## Fixtures (4096 casos, 16 familias x 256; `Fixtures.hpp`)

0 salidas rapidas (danador nulo, intensidad 0 / -0 / NaN / 1e-30, tipos 0 y 5..7, byte de estado crudo) · 1 edificio · 2 objeto: umbrales 1/2 (bordes con nextafter), estatico, contacto, sujeto ·
3 objeto: producto escalar, masa, danio por caida (`_ftol2` incl. >= 2^63), `GetLocalDirection` · 4 peaton con victima jugadora · 5 peaton con danador jugador (odio si/no) · 6 peaton contra peaton, evento inverso ·
7 vehiculo rapido, no jugador (sujeto o no) · 8 vehiculo rapido, jugador: angulos en los bordes de `|rel| < caja`, `> pi - caja`, rel > 0 / <= 0 / NaN, velocidades especiales ·
9 vehiculo lento (alrededor del umbral 0x3B23D70B, NaN, sujeto, jugador, evento) · 10 tren y atropello del peaton quieto · 11 byte de estado puesto y tipos sin rama · 12 ruido de pasamontanas (edificio/objeto, temporizadores) ·
13 ruido con objeto fisico (bit 20h, giro 0.04) · 14..15 mezcla. Cada caso: arenas con guardas (suma a mano, volcada en `begin` y comparada con el oraculo), semillas de pila, EAX/EDX por caso, bytes de bool no canonicos
(`RawBool`), matriz de la victima nula (25 %), globales fijados y restaurados por caso. Cobertura minima en `fixture_oracle.py` (`MINIMUMS`, ~50 % de lo medido en la primera corrida buena); por ejemplo: vehiculo rapido jugador 387,
`dot_front` 108 / `dot_back` 77 / `side_pos` 106 / `side_neg` 96 / `side_nan` 9, `train_kill` 21, `ped_hate` 247, `phase2_vplayer` 120, `obj_hard` 125, `tail_phys_noise` 66, `tail_nonphys_noise40/30` 155 / 81.

## Suposiciones y riesgos

- Evidencia limitada a los fixtures: floats moderados (sin productos fuera del rango de float, sin denormales; el x87 conserva exponente extendido y el juego corre con FTZ), con NaN/inf/-0/0 solo donde el resultado no sale de rango.
  Los `atan2` se calculan en la DLL con `Atan2FromCpp` / `TrigAtan2Cpp` (double, un solo redondeo a float) y en el oraculo con `math.atan2`; los argumentos de `LimitRadianAngle` se comparan en bits (NaN canonicalizado).
- Los colaboradores sustituidos no se validan aqui (sus cuerpos reales no se ejecutan). La aritmetica de vectores concuerda por el resultado (EXE real contra C++ de la DLL), no por falsos.
- La rama `rel <= 0` del EXE calcula `dot(right, adelante del peaton)` con la matriz del peaton sin comprobar nulo; la DLL no lo hace (sin efecto observable salvo un fallo del EXE con matriz nula, que el banco evita).
- Compilacion del archivo con las opciones por defecto del proyecto (SSE, `/fp:precise`): cada float redondea a 24 bits como el x87 con control de 24 bits; no se probo en otra configuracion de compilacion.
- Llamadores del EXE: solo `CPedIntelligence::Process` (0x6073D7); tras la llamada hace `mov ecx,[esi]` y no lee EAX/ECX/EDX (EDX/EAX se reescriben antes de usarse). Por eso solo se registran.
- `m_bAlreadyHitByCar` nunca se pone a 1 dentro de esta funcion (solo se lee en la rama de vehiculo y se borra al final).
