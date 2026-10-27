<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GarageUpdate63: CGarage::Update (0x44AA50)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wf-1`, HEAD 35e7b9d4b + parche del banco Garage63 + fuente del lote 63b). 16384 pares, cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauración
en ambos lados y salida de cada lado igual a la del oráculo independiente (eventos, cambios de la arena y del estado real); cobertura mínima cumplida (88 claves, una por etiqueta del EXE). Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-garageupdate6361020-101454-817244`.

Banco independiente del de las nueve funciones de CGarage (`reversing/Garage63`, no se toca). `source.patch` contiene SOLO los cambios de `Update` (y sus ayudantes x87) respecto de HEAD + `reversing/Garage63/source.patch`
(`git apply --check` verificado sobre esa base).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CGarage::Update(int32 garageId)` | 0x44AA50 | Simple | 16384 (16 familias) | 47 (ver abajo) |

## Qué hace la función (resumen del desensamblado)

`Update` corre una vez por garaje y fotograma desde `CGarages::Update` (0x44C8C0). Tiene tres tramos:

1. **0x44AA50..0x44ABB6, contabilidad del jugador**: salvo el tipo 13, con estado ≤ 5, jugador existente y sin la bandera 0x20, mira el vehículo del jugador (o el del peatón si su modelo es 0x23B, o el propio peatón)
   con `IsEntityEntirelyInside3D(x, 0.25f)` (pone `bCamShouldBeOutside` y 0xB6F844), `IsEntityEntirelyOutside(vehículo, 0.0f)` (0xB6F848) y, para el modelo 0x1A7, un rectángulo estricto ampliado 0.5f.
2. **0x44ABB6, banderas y salto**: `flags & 2` con estado 0 termina; con `flags & 8` actualiza el bit 6 (puerta cerrada: estado 1, o estado 3 con puerta > 0.4f, lo borran); tipo − 1 (sin signo, 0..0x2C) → tabla de 45 bytes
   (0x44C80C) → 13 casos.
3. **Un autómata por familia de tipos** (tablas de estado en 0x44C83C..0x44C8BC): tipo 5 (Pay'n'Spray: 5 estados, reparación + repintado + 10 partículas + cobro + aviso a la policía de la zona), tipos 2/3/4 (bombas),
   tipo 1 (objetivo de misión), tipo 11, tipos 14/21, tipos 15/19, refugios (16..18, 24..32, 39..42, 44, 45: puertas, `CountCarsInHideoutGarage`, ayuda de texto), tipo 20, tipo 23, embargos 33..35,
   talleres 36..38 y tipo 43. Colas compartidas: 0x44B3C1 (`CWorld::CallOffChaseForArea` con el rectángulo ±10), 0x44BCBE (abrir puerta), 0x44C662/0x44C69D/0x44C6AC/0x44C7C0.

## Fuente (Garage.cpp / X87Math.cpp/.h)

`CGarage::Update` se reconstruye en C++ conservando la estructura de saltos del EXE: etiquetas `L_xxxx` con la dirección del EXE y `goto` (muchas ramas comparten colas y se saltan de una a otra). Orden y NÚMERO de llamadas a
`FindPlayerVehicle` / `FindPlayerPed` / `FindPlayerCoors` / `FindPlayerWanted` idénticos a los del EXE (cada lectura de posición del jugador es una llamada distinta). Campos sin nombre en las cabeceras por desplazamiento (`GaField`).
Colaboradores sin reconstruir (`SlideDoorOpen` 0x44A660, `SlideDoorClosed` 0x44A750, `IsPlayerOutsideGarage` 0x448E50, `IsAnyCarBlockingDoor` 0x4494F0, `CountCarsWithCenterPointWithinGarage` 0x4495F0, `IsAnyOtherPedTouchingGarage`
0x449100, `IsAnyOtherCarTouchingGarage` 0x447720, `ThrowCarsNearDoorOutOfGarage` 0x449220, `CalcDistToGarageRectangleSquared` 0x447D80, `RestoreCarsForThisHideOut` 0x448550, `NeatlyLineUpStoredCars` 0x448330,
`RestoreCarsForThisImpoundingGarage` 0x4485C0, `StoreAndRemoveCarsForThisImpoundingGarage` 0x449A50, `FindDoorsWithGarage` 0x449FF0) y el `rand` del CRT del EXE (0x821B1E) se llaman por dirección; el resto, en C++.
Ayudantes x87 nuevos (bloque «Lote 63b» al final de X87Math.cpp/.h): `Garage63CenterDistSqCpp` (distancia² del jugador al centro, 5 ramas), `Garage63DoorDistSqCpp` (distancia² a las puertas del refugio) y
`Garage63TopMinus2GreaterCpp` (embargos). Se añade `#include "Garages.h"` y `__declspec(noinline)` a seis métodos de CGarage (para que los falsos del banco intercepten también la DLL).

## Registros (EAX/ECX/EDX)

El único llamador del EXE (0x44C910, dentro de `CGarages::Update`) conserva solo ESI y EDI a través de la llamada (callee-saved) y sobrescribe ECX justo después: **ninguno lee EAX/ECX/EDX**; solo se registran.

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. Los 47 colaboradores son falsos idénticos en ambos lados que registran EN ORDEN sus argumentos (punteros como fichas simbólicas
`G`, `V<i>`, `PED`, `W`, `L`) y devuelven lo que dicta el caso (máscaras por llamada). El garaje, el jugador (peatón), 8 vehículos con matrices y el `CWanted` son objetos del banco (arena con guardas, puesta a cero salvo los campos
especificados, de modo que el oráculo conoce cada palabra). El estado real del EXE que lee o escribe `Update` (reloj 0xB7CB84, contador de fotogramas 0xB7CB4C, jugador en foco y su dinero, `CGarages` 0x96C008/9/A, 0x96BFDC,
0x96BFE0, 0x96BFE4, cámara 0xB6F844/48, tienda de bombas 0x8E6940 y el mando 0: 0xB73562 y 0xB73566) se fija por caso, se compara tras cada lado y se restaura. Se comparan: eventos de los falsos, cambios de toda la arena
(región `X` = cualquier cambio inesperado), cambios del estado real y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado, ruta del hook, entradas de los colaboradores sin tocar, restauración del estado real).

`fixture_oracle.py` modela la función como un autómata de etiquetas con las direcciones del EXE, con aritmética x87 a 24 bits emulada con fracciones exactas; `python -I fixture_oracle.py` ejecuta la autoprueba.
`fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 16384 casos por el oráculo, fija las constantes de disposición y comprueba que el `Rand` de C++ coincide con el de Python. La cobertura
(`minimums.json`, ≈50 % de lo medido) incluye una bandera por etiqueta del EXE alcanzada (71 claves) y 256 casos por familia.

## Revisión propia contra el EXE (fase 2)

Se desensambló `0x44AA50..0x44C8BF` completo y se descodificaron las 8 tablas de salto (`0x44C80C` bytes; `0x44C7D4`, `0x44C83C`, `0x44C850`, `0x44C864`, `0x44C874`, `0x44C88C`, `0x44C89C`, `0x44C8AC` dwords).
El código se contrastó etiqueta por etiqueta. Defectos halladas durante la revisión y corregidos antes de compilar: (1) `0x44B8CE` es `je` (termina si el jugador YA va en el coche objetivo), no `jne`; (2) el vehículo del jugador se
relee con una llamada nueva en cada acceso (12 en el giro del coche, 4 por puerta); (3) la lectura de `+0x42F` en `0x44B38D` debe ser una sola llamada; (4) el `rand` del CRT y los `CGarage::IsPointInside*` no se sustituyen por los de la DLL.
Defectos reales de producción hallados por el banco: **uno**, en la fuente que acababa de escribir y corregido antes de la corrida aprobada: `0x44B38D` leía `+0x42F` con dos llamadas a `FindPlayerVehicle` (el EXE hace una); la primera corrida con el hook encendido mostró una llamada de más en 2 de los primeros 100 casos. No hubo más diferencias EXE/DLL: la corrida aprobada dio 0 en 16384. Los demás fallos intermedios fueron del banco (matriz nula del vehículo del jugador desreferenciada por el EXE, `GivePlayerDetonator` de la DLL sin hook que había que parchear por puntero a miembro, la macro `near` de windows.h).

## Suposiciones y límites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integración real con jugador, pools, audio, HUD, texto, estadísticas ni partículas.
- `CalcDistToGarageRectangleSquared` devuelve en ST0 un float; la DLL lo lee como `float` (el EXE lo guarda con `fst dword` en una rama y compara el valor de ST0 sin redondear en otra: iguales salvo exponentes fuera del rango de float).
- No se prueban los caminos en que el EXE desreferencia sin comprobar: matriz nula del vehículo del jugador en la rama de reparación (tipo 5, estado 0) y `IsStaticPlayerCarEntirelyInside` / `IsAnyOtherCarTouchingGarage`
  verdaderos sin vehículo del jugador.
- Los NaN de las fixtures son silenciosos (0x7FC00000): el EXE pasa algunos valores por el x87 (`fld`/`fstp`), que silenciaría un NaN señalizador.
- El modelo de vehículo de las fixtures es siempre 0, 0x1A7, 0x23B o 5 (el EXE nunca desreferencia la ficha del modelo: solo la pasa a `ChooseVehicleColour`, sustituida).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 pwsh -NoProfile -File D:\Original\wf-1\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh GarageUpdate63       # compila la sonda y corre en el juego (candado global)
python -I reversing/GarageUpdate63/fixture_oracle.py       # autoprueba del oráculo
python -I reversing/GarageUpdate63/fixture_mirror.py       # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/GarageUpdate63/run_local.py --analyze <comparisons.jsonl>
python reversing/GarageUpdate63/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Depuración: un archivo `garageupdate63-cases.txt` en la carpeta del juego con «primero ultimo» limita la corrida a ese rango de casos (el analizador final fallará por el número de pares; solo para diagnóstico).

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
