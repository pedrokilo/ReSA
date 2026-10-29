<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HeliGen74: CHeli::GenerateHeli (0x006C6520)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-3`, HEAD f13a750cf + `source.patch`). 21504 pares (familias 0..15 de 1024 casos + 5 familias extra de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`: modelo Python de la funcion con aritmetica x87 exacta, seno y coseno con 64 bits de mantisa y memoria de las arenas). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-heligen74\20261027-043920-349113`.

`source.patch`: Heli.cpp, Heli.h (`GetRopeId`), X87Math.cpp y X87Math.h; es el mismo parche de los tres bancos del lote 74 (HeliSwat74, HeliGen74, HeliBlow74). `GenerateHeli` no anade ayudantes x87 nuevos: usa `TrigCompoundCosMulAdd`, `TrigCompoundSinMulAdd`, `TrigSimpleSinCpp` y `TrigSimpleCosCpp`.

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| GenerateHeli | 0x006C6520 | CHeli,GenerateHeli | S |

Estatica, no virtual. cdecl `(CPed* objetivo, bool noticias)`; el llamador (0x6C7ACF, `CHeli::UpdateHelis`) limpia 8 bytes y usa EAX (el helicoptero) para registrarlo en `pHelis`. El EXE lee `noticias` como byte (`mov al,[esp+14h]` y `mov al,[esp+30h]`): el banco pasa basura en los tres bytes altos. EAX se compara (puntero etiquetado); ECX/EDX solo se registran.

## Revision contra el EXE (0x6C6520..0x6C6882)
- `CVehicle::operator new(0xA18)` (0x6E2D50) y constructor de `CHeli` (0x6C4190) con modelo 0x1E8 (noticias) o 0x1F1 (policia) y creado-por 4 (permanente). Solo se comprueba el nulo de la reserva; el EXE se cae despues al usar el helicoptero si el pool esta agotado (no se prueba).
- Posicion base: matriz + 0x30 o posicion compacta +4, copia cruda de tres palabras. `angulo = float(float(rand() & 0xFF) * 0x859C44)` (rand del CRT del EXE, 0x821B1E); `px = float(fcos(angulo) * 250.0 + base.x)`, `py = float(fsin(angulo) * 250.0 + base.y)` (fsin/fcos con precision completa, producto y suma a 24 bits).
- Si `px < -3000`, `px > 3000`, `py < -3000` o `py > 3000` (ordenados, un NaN cuenta como dentro): `angulo = float(angulo + pi)` (0x858CB8 = 0x40490FDB) y se repite con la posicion base releida.
- `z candidata = float(base.z + 50.0)`; si `sqrt((py + 1653)² + (px + 2322)²) < 350` (alrededor del Monte Chiliad; comparacion ordenada) usa la posicion base y `z = lifted > 560 ? float(base.z + 200) : 560` (`fcom` + `test ah,41h`: igual o NaN dan 560); si no, usa `(px, py)`.
- `suelo = FindGroundZForCoord(x, y)`; `z = candidata > suelo + 20 ? candidata : float(suelo + 20)` (un NaN o la igualdad dan suelo + 20). `matriz->SetTranslate(x, y, z)` (0x59AF40).
- Bandera `bIsLocked` (+0x428 bit 3), estado FISICO (+0x36), mision 0x29/0x28 (noticias/policia), velocidad de crucero 0x23/0x46, `FindPlayerPed(-1)` solo con noticias (el valor se sobrescribe con el objetivo), alturas +0x9A8/+0x9B0 (30/27 o 20/12), velocidad del rotor +0x84C = 0x3E28F5C3, rumbo = `float(angulo + pi)`, `right = (sin, -cos, 0)`, `forward = (cos, sin, 0)`, `up = (0, 0, 1)`, `CWorld::Add`, `SetUpDriver(-1, false, false)`.
- El bucle sobre `pHelis` de 0x6C6790 no tiene efecto (su resultado no se usa) y no se reproduce.

## Defectos hallados por el banco
Defectos de PRODUCCION: ninguno en `GenerateHeli` (la primera corrida compilada dio cero diferencias EXE/DLL). Defecto latente corregido durante la revision: `CHeli::GetRopeId()` (Heli.h, usado por `SendDownSwat`) hacia aritmetica de punteros de objetos; no afecta a esta funcion.
Defectos del BANCO: la primera corrida salio FALLO por el oraculo (no por EXE/DLL: ambos lados eran iguales entre si): el oraculo cargaba el entero de `rand() & 0xFF` como si fuera un patron de float en lugar de un entero (`fild`); corregido en `fixture_oracle.py` (`load_int`). La corrida aprobada es la posterior. Ninguno relajo compuertas ni cobertura.

## Banco
Fixtures: 21504 casos, 21 familias: 0 base, 1 limites ±3000 (rama del angulo + pi), 2 posiciones muy fuera, 3 alrededor del Monte Chiliad (z: 360 ± ulp, 560 - 200, NaN), 4 altura final (candidata frente a suelo + 20, igualdad y ± ulp), 5 los 256 valores del byte bajo de `rand` con basura arriba, 6 el argumento bool con basura en los bytes altos, 7 objetivo con y sin matriz, 8 especiales en la posicion (NaN no canonicos, infinitos, ceros con signo, desnormales), 9 especiales en el suelo, 10 altura base especial dentro y fuera del Chiliad, 11 mezcla de rangos, 12 exponentes extremos de la base (redondeo de `cos * 250 + base`), 13 bases desnormales y diminutas, 14 frontera exacta del cuadrado ±3000 con angulo 0, 15 frontera exacta de la distancia 350 con angulo 0 (triplete 210-280-350), 16 bases enormes, 17 altura NaN/infinita, 18 suelo enorme/infinito, 19 NaN no canonicos y de señal, 20 angulos extremos con la posicion en el limite.
Arenas propias: CHeli completo (0xA18), objetivo y dos matrices. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran sus argumentos EN ORDEN), y quien valida a cada uno (docs/hooks.csv y reversing/validation-registry.json):
- `rand` 0x821B1E (CRT del EXE): sin hook; la DLL lo llama por su direccion.
- CVehicle::operator new 0x6E2D50: DependenciesVehicle12 (2048 pares); constructor de CHeli 0x6C4190: Helpers66D (4096); se localiza siguiendo la primera llamada de la envoltura `Constructor` del hook y se construye con `::new (mem) CHeli(...)` sin parentesis.
- CWorld::FindPlayerPed 0x56E210 y CWorld::Add 0x563220: WldC (256 y 768); CWorld::FindGroundZForCoord 0x569660: WldB (128); CVehicle::SetUpDriver 0x6D1A50: CVehB (256).
- CMatrix::SetTranslate 0x59AF40: sin hook en la tabla (CMatrix::InjectHooks esta comentado) y sin banco propio; falso en la direccion del EXE y en el cuerpo de la DLL (se localiza por el puntero a miembro). No validada aqui.
- Las funciones de seno y coseno de X87Math (TrigCompoundCosMulAdd, TrigCompoundSinMulAdd, TrigSimpleSinCpp, TrigSimpleCosCpp) corren de verdad en el candidato; el banco las compara bit a bit con el `fsin`/`fcos` del EXE a traves de la posicion, de `SetTranslate` y de la matriz.
Se comparan eventos, bytes cambiados de las 4 arenas, el puntero del objetivo guardado en +0x41C (etiquetado y oculto de los bytes cambiados), EAX, compuertas de ABI (pila, no volatiles, DF, SEH), ruta (el EXE salta a la DLL solo en el candidato), estado x87 y restauracion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json`.

Prueba negativa (sobre el Heli.cpp anterior a la correccion de la sirena en BlowUpCar, que no toca esta funcion; produccion mutada y restaurada despues): `< 350.0f` por `<= 350.0f` en la distancia al Monte Chiliad: 680 diferencias de resultado en 21504 pares (los casos con distancia exactamente 350: la posicion elegida cambia de la de salida a la del objetivo), sin caida: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-heligen74\20261027-035459-457679`.

No validado: el pool de vehiculos agotado (el EXE se cae), el valor de ECX/EDX de salida (nadie los lee), `CMatrix::SetTranslate` (sin banco) y la validez del resto de colaboradores (la prueban sus propios bancos).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh HeliGen74`; `python -I fixture_oracle.py <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).
