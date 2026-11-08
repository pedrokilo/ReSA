<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 865 grupos / 9345 funciones reconstruidas; 9345 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veh83B: CVehicle::InflictDamage 0x6D7C90 (lote 83B)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD 5bf419e97 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y 576 microtests de FCOMP del hardware.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh83b/20261101-042717-377646` (antes: `20261101-041150-932860`, tambien aprobada; el manifiesto de construccion es el de la corrida final).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CVehicle::InflictDamage(CEntity*, eWeaponType, float, CVector)` (antes `((void(__thiscall*)(...))0x6D7C90)(this, ...)`, sin hook) | 0x6D7C90 | `CVehicle` / `InflictDamage` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 2 ayudantes x87 (`Veh83BScaleCpp`, `Veh83BSubCpp`) en `X87Math.cpp/.h` (bloque "Lote 83B", solo anadido) |

Fila de `docs/hooks.csv`: `CVehicle,InflictDamage,0x006D7C90,1,0,1,S`.

## Revision contra el EXE (0x6D7C90..0x6D82BA, 0x62A bytes + tabla de saltos en 0x6D82C0/0x6D82D0)
thiscall con seis dwords (atacante, arma, intensidad, x, y, z; `ret 0x18`), void. Orden de la funcion:
1. `CanVehicleBeDamaged(atacante, arma, &fuego)` (0x6D1280); AL == 0 -> sale. Estado 0 y `GetPercentageProgress() >= 100.0` -> intensidad *= 0.5 (x87 24 bits, guardado como float).
2. Intensidad > 10: atacante == `FindPlayerPed(-1)` o `FindPlayerVehicle(-1, false)` (el segundo solo si el primero no coincide) y estado != 5 (destruido): `Players[PlayerInFocus]` +0x140 += 2, +0x148 += 1.0f, `IncrementStat(0x7D, 5 + rand() % 20)`.
3. Atacante peaton y vehiculo automovil o moto: probabilidad por arma (tabla de 31 bytes + 4 destinos: 5, 64, 10 o 0; indice = arma - 22 sin signo, > 30 = 0); `IsPlayer` la anula; atacante en moto (subtipo 9) la limita a 1; si no, moto de policia 0x20B con conductor policia sale; sin probabilidad o con ruedas indestructibles sale; `(rand() & 0x7F) < probabilidad` -> moto: `FindTyreNearestPoint(x, y)` y `BurstTyre(rueda + 13, 0)`; coche con `GetVehicleAppearance() == 1`: `BurstTyre(rueda + 13, 1)`. El EXE llama a `BurstTyre` sin recargar ECX (la funcion real no lo toca).
4. Deposito (+0x42E bit 6, fuego, atacante peaton, `IsPlayer`): dummy 8 de la estructura del modelo != 0 -> punto = matriz * dummy (0x59C890); si |coord - punto| < 0.25 la intensidad pasa a `min(vida, 1100.0)` (0x404330 devuelve el segundo con NaN).
5. Subtipo 3 o 4 sin RC (+0x42C bit 7) y arma distinta de 0x33, 0x10, 0x13 y 0x14: intensidad *= 0.4.
6. Vida > 0: arma (+0x508) y entidad de dano (+0x50C, con `RegisterReference`, solo si no es nula). Si vida > intensidad: vida -= intensidad; con estado 0, 2 o 3 y atacante peaton, un `CEventVehicleDamageWeapon` (ctor 0x61C2E0, `CEventGroup::Add(&evento, false)` 0x4AB420, dtor) para el conductor y para cada pasajero hasta `m_nMaxPassengers`; si la vida cruza 250 hacia abajo en un automovil: `SetEngineStatus(225)` y +0x928 = atacante (aunque sea nulo, con `RegisterReference` si no lo es). Si no: vida = 0; atacante == `FindPlayerPed(-1)`: `ReportCrime` (12 helicoptero no RC, 16 avion no RC, 0x14 el resto); explosion (0x33): +0x4DE = (rand() & 0x7FF) + 0x3E8 y +0x4E0 = atacante; si no, `BlowUpCar(atacante, 0)`.
7. Al final (tambien con vida <= 0): +0x428 bit 0 (de la ley) y atacante == `FindPlayerPed(-1)` -> `FindPlayerPed(-1)->SetWantedLevelNoDrop(1)`.

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(InflictDamage, 0x6D7C90)` (el rand() es el del EXE, 0x821B1E, como en Heli.cpp; offset crudo solo para +0x928 de CAutomobile). `X87Math.cpp/.h`: `Veh83BScaleCpp` y `Veh83BSubCpp` (24 bits con exponente extendido y guardado `fstp dword` emulado a mano en la zona desnormal, porque el juego tiene FTZ activo). La matriz*punto usa `Hp65MatrixTimesPointCpp` y la resta del vector `Veh81BSub3Cpp`.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: CanVehicleBeDamaged, GetPercentageProgress, FindPlayerPed, FindPlayerVehicle, IncrementStat, IsPlayer, GetVehicleAppearance, FindTyreNearestPoint (naked: devuelve ECX intacto), RegisterReference, CleanUpOldReference, CEventGroup::Add, ReportCrime, SetEngineStatus y SetWantedLevelNoDrop; BlowUpCar y BurstTyre por la vtabla falsa. Corren de verdad: rand() del EXE (semilla fijada por lado), el evento (ctor y dtor), Magnitude, matriz*punto, `CVector !=` y min.
18432 pares, 18 familias (16384 + 2x1024): CanVehicleBeDamaged falso, progreso, jugador/vehiculo del jugador (caos, estadistica, rand), ruedas de coche y de moto (armas, ruedas indestructibles, moto del atacante, moto de policia, aspecto), deposito (coordenadas junto al borde 0.25 calculadas con la matriz), helicopteros y aviones, eventos del conductor y pasajeros, cruce de 250 (motor), letal (crimen, explosion, BlowUpCar), vida <= 0 o NaN (solo la ley), flotantes especiales, NaN/inf repartidos y desnormales/bordes. Cada lado coincide con `fixture_oracle.py` (Python, aritmetica x87 24 bits con Fractions; el redondeo de la raiz de Magnitude se modela por su umbral exacto) y hay cobertura de ramas con minimos (`coverage.json`).

## Pruebas negativas
Produccion mutada y restaurada (hashes identicos a los buenos despues): `& 0x7F` -> `& 0x3F` en la rueda, `0.4f` -> `0.5f` en aviones y helicopteros, y `ReportCrime` helicoptero/avion intercambiados. Fallan por diferencia de resultado, sin caida (`resultados-veh83b/20261101-042332-862207`: **4011/18432 diferencias**, el lado original coincide con el oraculo en las 4011).

## Bancos repetidos (citan 0x6D7C90) y bancos ajustados
Data23 (13904 pares) y Fx43 (15552) solo parcheaban la direccion del EXE: se anadio el parche del cuerpo de la DLL (ahora la DLL ejecuta C++ real). WeaponFinal2 (7808) y WldE (8512) ya parcheaban ambas. Los cuatro aprobados y refrescados en el registro.

## Fuera de los fixtures
`m_nMaxPassengers` > 8 (el EXE lee mas alla de `m_apPassengers`), atacantes con tipo de entidad invalido, indices de modelo fuera de los 8 del banco (el EXE indexa con el word con signo), NaN con cargas distintas (las fixtures usan un unico patron) y `bool` con bytes distintos de 0/1 en la salida de `CanVehicleBeDamaged`.
