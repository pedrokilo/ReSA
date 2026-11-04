<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 04/11/2026 (Ciudad de México):** 834 grupos / 9077 funciones reconstruidas; 9077 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veh83C: CVehicle::AddSingleWheelParticles 0x6DE880 (lote 83C)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD a59d24b67 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y microtests de la x87 del hardware.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh83c/20261101-053150-162755` (el manifiesto de construccion es el de esta corrida).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `bool CVehicle::AddSingleWheelParticles(tWheelState, uint32, float, float, CColPoint*, CVector*, float, int32, uint32, bool*, uint32)` (antes `((bool(__thiscall*)(...))0x6DE880)(this, ...)`, sin hook) | 0x6DE880 | `CVehicle` / `AddSingleWheelParticles` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 13 ayudantes x87 `Veh83C*` en `X87Math.cpp/.h` (bloque "Lote 83C", solo anadido) |

Fila de `docs/hooks.csv`: `CVehicle,AddSingleWheelParticles,0x006DE880,1,0,1,S`.

## Revision contra el EXE (0x6DE880..0x6DF3B9, 0xB48 bytes; thiscall, 11 dwords, `ret 0x2C`; devuelve siempre false)
Argumentos (dwords): wheelState (0 normal, 1 giro, 2 derrape, 3 fijo; otros = rama por omision), wheelStatus (1 = pinchada), arg2 (solo pasa si < 1 o NaN), arg3 (velocidad), CColPoint*, posicion de la rueda, arg6 (sin uso), indice de rueda, tipo de huella, `bool*` de sangre y banderas (1 sangre, 2 barro, 4 sin derrape trasero).
Los desplazamientos 0x49C y 0x42D son `m_GasPedal` y `vehicleFlags.bAlwaysSkidMarks` (bit 0), no el angulo de volante.
1. `FindPlayerVehicle(-1, false)`. Entidad invisible (+0x1C bit 17) -> sale. (dz^2 + dy^2) + dx^2 de camara - posicion (matriz +0x30 o, sin matriz, +4): > 625 sin `bAlwaysSkidMarks` -> sale. Cadencia: > 400 una de cada 4, > 64 (o sin vehiculo del jugador) una de cada 2, con la suma del byte bajo del modelo y `CTimer::m_FrameCounter`. `arg2 >= 1` -> sale.
2. En el agua: `IsWater(superficie B)` o, con +0x40 bit 27, `GetWaterLevel(x, y, z) >= z`. `produce = arg3 < 1`.
3. Chispas: `wheelStatus == 1`, (`arg3 > 0.1` o estado 1) y `GetFrictionEffect == 1`: `AddSparks` (direccion velocidad * -50 + 2.5 en z, o con estado 1 y `arg3 < 0.2` el vector adelante * pedal * -12 + 2.5, 10 chispas; fuerza = `NormaliseAndMag`) y `ReportCollision(this, FindPlayerPed(0), ...)`.
4. `MakeDirty`. Segun el estado: 1 -> `AddWheelDirtAndWater(.., 1, ..)` y una particula (4 sorteos de rand: x, y, `% 10000` y color); 2 (sale si banderas & 4) y 3 -> con `arg3 > 0.03`, `AddWheelDirtAndWater(.., 0, ..)` y rastro de max(1, (int)(|v * paso| * escala)) particulas (escala 2, 3 en los subtipos 9, 2 y 10; un sorteo por particula); resto -> `AddWheelDirtAndWater` con `arg3 > 0.03` y solo sigue si `bAlwaysSkidMarks` con rueda impar y velocidad plana > 0.04, o sangre, o barro.
5. Huella (salvo `wheelStatus == 1` o en el agua): quad (0x1D7) desplaza la posicion 0.15 por el eje x de la matriz (+ delanteras, - traseras), `FindWheelWidth(idx & 1)` (virtual +0xFC) y `CSkidmarks::RegisterOne(this + idx, pos, forward.x, forward.y, tipo, &sangre, ancho)`.

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(AddSingleWheelParticles, 0x6DE880)` (rand del EXE, 0x821B1E, como en InflictDamage). `X87Math.cpp/.h`: `Veh83CMulStore`, `Veh83CMul2Store`, `Veh83CMulAddStore`, `Veh83CAddStore`, `Veh83CSubStore`, `Veh83CFtol` (_ftol2 del EXE: 0 fuera de rango), `Veh83CDistSq`, `Veh83CMagnitude`, `Veh83CMagnitude2D`, `Veh83CTrailCount`, `Veh83CNormalise` (0x59C970) y `Veh83CRandStore` (0x41BD90): 24 bits por operacion con guardado `fstp dword` emulado en la zona desnormal (el juego tiene FTZ activo en SSE).
Primera version con aritmetica float normal: 236 diferencias de 18432, todas en las familias de valores especiales (desnormales y conversion a entero fuera de rango) y ninguna en el resto.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: FindPlayerVehicle, IsWater, GetWaterLevel, GetFrictionEffect, AddSparks, FindPlayerPed, ReportCollision, MakeDirty, AddWheelDirtAndWater, FxSystem_c::AddParticle y CSkidmarks::RegisterOne; FindWheelWidth por la vtabla falsa. Los ganchos RH de 0x41BD90, 0x4082C0, 0x59C970 y 0x4AB290 se apagan durante el banco: el lado original ejecuta los bytes del EXE y el rand() es el del EXE (sin esto 0x41BD90 saltaba al rand() de la DLL).
18432 pares, 18 familias (16384 + 2x1024): estados 0..3, chispas, compuertas de distancia (625/400/64 con ulps), arg2, agua, umbrales de velocidad (0.03, 0.04, 0.1, 0.15, 0.2, 1.0), quad, subtipos, rama por omision, valores especiales (NaN, infinitos, desnormales), muchas particulas (hasta 450), aleatorio y estados/indices fuera de rango, NaN e infinitos, desnormales. Cada lado coincide con `fixture_oracle.py` (Python, fracciones exactas, x87 a 24 bits) y hay cobertura de ramas con minimos (`coverage.json`).

## Prueba negativa
Produccion mutada y restaurada (sha256 identico al bueno despues): desplazamiento del quad 0.15 -> 0.16, `phase & 3` -> `phase & 1` (cadencia > 400) y alfa de la particula de giro 0.3 -> 0.35. Fallan por diferencia de resultado, sin caida (`resultados-veh83c/20261101-051950-934320`: **3763/18432 diferencias**, el lado original coincide con el oraculo en las 3763).

## Bancos repetidos (citan 0x6DE880)
AutoPreRender60 (8192 pares; adaptado: ahora tambien falsea el cuerpo de la DLL, `kNoDllBody = 0`), BikePreRender355 y Misc61b (ya falseaban ambas direcciones). Los tres aprobados y refrescados en el registro.

## Fuera de los fixtures
Vehiculo sin matriz (el EXE la usa sin comprobar nulo), valores de `bool` distintos de 0/1 en el retorno de `AddWheelDirtAndWater` (el EXE comprueba EAX completo), NaN con cargas distintas (las fixtures usan un unico patron), bytes altos de los argumentos de superficie y de bool (el EXE los pasa con basura de pila; la funcion solo lee los bajos), mas de unas 450 particulas por llamada y `FindWheelWidth` fuera de la vtabla del vehiculo.
