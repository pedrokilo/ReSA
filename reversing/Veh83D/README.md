<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 08/11/2026 (Ciudad de México):** 868 grupos / 9463 funciones reconstruidas; 9463 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veh83D: CVehicle::ProcessBikeWheel 0x6D73B0 (lote 83D)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD 30c8df96e + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y microtests de la x87 del hardware.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh83d/20261101-061533-966790` (el manifiesto de construccion es el de esta corrida).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CVehicle::ProcessBikeWheel(CVector&, CVector&, CVector&, CVector&, int32, float, float, float, float, int8, float*, tWheelState*, eBikeWheelSpecial, uint16)` (antes `plugin::CallMethod<0x6D73B0,...>` con adhesion y destabTraction intercambiados, sin hook) | 0x6D73B0 | `CVehicle` / `ProcessBikeWheel` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 6 ayudantes x87 `Veh83D*` en `X87Math.cpp/.h` (bloque "Lote 83D", solo anadido) |

Fila de `docs/hooks.csv`: `CVehicle,ProcessBikeWheel,0x006D73B0,1,0,1,S`.

## Revision contra el EXE (0x6D73B0..0x6D7BBF, thiscall, 14 dwords, `ret 0x38`, void)
Argumentos: wheelFwd*, wheelRight*, wheelContactSpeed*, wheelContactPoint*, wheelsOnGround, thrust, brake, **adhesion (8o: el agarre; se multiplica por el paso de tiempo)**, **destabTraction (9o: factor de deslizamiento)**, wheelId (byte), wheelSpeed* (no se lee), wheelState*, special, wheelStatus (word). El envoltorio anterior los pasaba intercambiados y `CBike::ProcessControl` los daba al reves para compensarlo; ahora la firma sigue el orden del EXE y `Bike.cpp` pasa `grip * traccion` primero y el deslizamiento despues.
1. Estaticas del EXE: 0xC1CDB2 (freno: `brake != 0`), 0xC1CDB1 (empuje: sin freno y `thrust != 0`), 0xC1CDB0 (sin freno y `thrust < 0`), 0xC1CDAF (ya patinaba: se pone a 1 si `*wheelState != 0` y no se borra aqui).
2. v = (fy*cy + cz*fz) + fx*cx, guardada como float. Agarre = paso de tiempo * adhesion (guardado), por la perdida de traccion del CHandlingData (+0xA4) si ya patinaba. `*wheelState = 0`.
3. Lateral (special != 2 y 3): r = (rz*cz + ry*cy) + rx*cx; si no es 0, `lateral = -(float(r) / ruedas)`; con `wheelStatus == 1` le suma ruido: `((rand() * 0x38000100) * (0.05 - (-0.05)) + (-0.05)) * min(v, 0.12) + lateral` (rand() del CRT del EXE, 0x821B1E).
4. Con empuje: fuerza = thrust (bits intactos) y lateral acotado a +-agarre. Sin empuje y con v != 0: fuerza = -v / ruedas; sin freno y con |acelerador| < 0.01 el freno pasivo es `friccion * 0.5 / (masa + 200)` (BMX, solo si |fuerza| < 0.05), `friccion * 0.6 / (masa + 200)` (tipo moto) o `friccion / masa` (x0.2 con masa < 500 o RC Bandit); si el freno <= agarre se acota la fuerza a +-freno, si no y |v| > 0.005 la rueda queda fija (3).
5. Circulo de agarre: con `lateral^2 + fuerza^2 > agarre^2` el estado pasa a 1 (empuje y v < 0.1) o 2 (salvo fija) y se reescala por `(perdida / raiz(m2)) * agarre` (perdida 1.0 si ya patinaba), con `destab < 1` ademas el lateral se multiplica por destab; dentro del circulo y con `destab < 1` se reduce el lateral si `agarre^2 * destab^2 < m2`.
6. Sin fuerza ni lateral sale. Fuerza total = lateral * derecha + fuerza * adelante (la z lateral sin guardar), `Normalise` (0x59C910), `CrossProduct(&brazo, &punto, &dir)` (0x59C730), `ApplyMoveForce(dir * masa * longitud)` y `ApplyTurnForce` una o dos veces (la primera se salta con wheelId == 1 y freno o marcha atras).

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(ProcessBikeWheel, 0x6D73B0)`. `Bike.cpp`: orden de los dos argumentos en las dos llamadas. `X87Math.cpp/.h`: `Veh83DMul/Add/Sub/Div/Sqrt` (registro de la x87: double ya redondeado a 24 bits con `Player82ARound24`) y `Veh83DStore` (`fstp dword`, desnormales incluidos).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: CVector::Normalise, CrossProduct, ApplyMoveForce(CVector) y ApplyTurnForce; los de salida escriben los valores del caso. 18432 pares, 18 familias (16384 + 2x1024): marcha, freno, sin empuje (BMX / moto / otros), special, rueda pinchada (ruido), circulo superado, destab, estado previo, rueda 1, sin fuerza, umbrales con ulps, valores especiales, numero de ruedas (0, negativo, enorme), basura en bytes altos, aleatorio, aleatorio con valores de pila, NaN/infinitos y desnormales. Cada lado coincide con `fixture_oracle.py` (Python, fracciones exactas, x87 a 24 bits; el `fchs` invierte el signo del NaN) y hay cobertura de ramas con minimos (`coverage.json`). El oraculo y el C++ coincidieron con el EXE a la primera.

## Prueba negativa
Produccion mutada y restaurada (sha256 identico al bueno despues): factor de la fuerza de giro 2.0 -> 2.5, umbral de giro 0.1 -> 0.2 y `wheelId != 1` -> `wheelId != 0`. Fallan por diferencia de resultado, sin caida (`resultados-veh83d/20261101-061301-460797`: **15351/18432 diferencias**, el lado original coincide con el oraculo en las 15351).

## Bancos repetidos (citan 0x6D73B0)
BikeControl355 (`CBike::ProcessControl`): adaptado, ahora falsea tambien el cuerpo de la DLL de `ProcessBikeWheel`, y de `CCarCtrl::ScanForPedDanger` y `CVehicle::ProcessSirenAndHorn`, que ya eran C++ y estaban sin parchear en la DLL (la corrida daba un acceso invalido en `SlowCarDownForPedsSectorList`). Aprobado y refrescado en el registro (`resultados-bike-control355/20261101-062000-130263`).

## Fuera de los fixtures
NaN con cargas distintas (las fixtures usan un unico patron), estaticas 0xC1CDAF..0xC1CDB2 con valores distintos de 0/1, `wheelSpeed` (no se lee) y salidas de `Normalise`/`CrossProduct` incoherentes con su entrada (los falsos devuelven lo que dice el caso; su calculo real esta validado en sus propios bancos).
