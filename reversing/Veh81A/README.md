<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veh81A: CVehicle::UpdateTrailerLink 0x6DFC50 (lote 81A)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wv-1`, HEAD ad52bea16 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87 en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh81a/20261028-212620-432598` (run id `20261028-212620-432598`).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CVehicle::UpdateTrailerLink(bool, bool)` (antes envoltura `((void(__thiscall*)(CVehicle*,bool,bool))0x6DFC50)`, sin hook) | 0x6DFC50 | `CVehicle` / `UpdateTrailerLink` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 6 ayudantes x87 en `X87Math.cpp/.h` (bloque "Lote 81A", solo anadido) |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,UpdateTrailerLink,0x006DFC50,1,0,1,S`.

## Revision contra el EXE (0x6DFC50..0x6E0049, 1017 bytes = 0x3F9 medidos con `dis.sh`; `UpdateTractorLink` 0x6E0050 es la siguiente y NO se toca)
thiscall con dos `bool` (dwords en la pila), `ret 8`, void. `this` es el REMOLQUE (tipo 11; estado 10/11 = remolcado) y `+0x4C4` (`m_pTowingVehicle`) es el TRACTOR (en `CAutomobile::ProcessControl` se llama como `UpdateTrailerLink(false, false)` y `(false, true)` con `m_pTowingVehicle`). Llamadores: `CAutomobile::ProcessControl` (dos veces) y `CAutomobile::SetTowLink` (`UpdateTrailerLink(true, false)`).
1. Dos vectores locales a cero (enganche del remolque, barra del tractor). Sin tractor, estado (`byte +0x36 >> 3`) distinto de 10 y 11, `GetTowHitchPos` virtual (+0xEC, con `this` y el tractor del primer acceso) o `GetTowBarPos` virtual (+0xF0, sobre el tractor RELEIDO) falsos: `BreakTowLink` virtual (+0xF8) y fin.
2. C = barra - enganche (x, y con `fsub + fstp`; z con `fst` y `fmul` del valor guardado); `(z*z + y*y) + x*x` guardado como float; t = max(paso `0xB7CB5C`, 0.7f) (`> 0.7f` ordenado, NaN y <= eligen la constante); si `1.0f * t < sqrt(suma)` ordenado (`0x871DF0` = 1.0f en `.rdata`), rompe el enlace.
3. Tractor RELEIDO (ebx) y matrices (`+0x14`, SIN comprobar nulo): dot de ejes `+0x10` (suma z, y, x) < -0.3f, o dot de ejes `+0x20` (`DotProduct 0x40FDB0`, sin guardar) < 0.0f: rompe el enlace.
4. Modelo del tractor (word `+0x22`) 525 o 531: si el word `+0x86C` > (word `0x8D313C`) - 100 (dwords con signo) retorna SIN romper.
5. `hitch -= posicion(this)` (matriz `+0x30` o compacta `+4`), `bar -= posicion(tractor)` (releida); `GetSpeed 0x542CE0(hitch)` y, sobre el tractor releido, `GetSpeed(bar)`; `rel = velTractor - velRemolque`.
6. Solo `!arg0 && arg1`: `rel = (0.3f (0x871DF4) * C) / max(1.0f, paso)` (`0x420800`, `0x40FE90`, `0x4119D0`: z con el reciproco del registro, y/x con el guardado como float).
7. Si `this+0x594` (dword) == 11 y `this+0x9EC` == -1000.0f (`0x6D0AF0`): `rel -= (rel . up) * up`.
8. `m = 1 / (|(hitch - Multiply3x3(matriz, +0xA4)) x Normalise(rel)|^2 / masaGiro(+0x90) + 1 / masa(+0x8C))` y `ApplyForce 0x542B50(rel * m, hitch, true)`.

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(UpdateTrailerLink, 0x6DFC50)`. `X87Math.cpp/.h`: `Veh81ATooFarCpp`, `Veh81ABentCpp`, `Veh81ASubCpp`, `Veh81ADampCpp`, `Veh81AProjectCpp`, `Veh81AForceCpp` (aritmetica x87 a 24 bits, `const float&` y memcpy de bits; reutilizan `Veh80CSqrt`). Las lecturas de memoria que pueden cambiar entre colaboradores (tractor, matrices, masas, subtipo, `+0x9EC`, paso, limite de la grua) se hacen en C++ en el mismo punto que el EXE.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (via MakeCollab, hooks RH apagados): `CPhysical::GetSpeed` 0x542CE0 (thiscall, puntero de resultado oculto + vector por valor, ret 0x10) y `CPhysical::ApplyForce` 0x542B50 (ret 0x1C). Los tres virtuales (+0xEC, +0xF0, +0xF8) son entradas de una vtabla falsa compartida (EXE y DLL llaman por la vtabla; el resto de entradas es una trampa que registra un evento 9). La aritmetica NO se sustituye (EXE: su codigo; DLL: los ayudantes x87). `DotProduct`, `CrossProduct` y `Normalise` no tienen hook RH vivo en esta compilacion (bytes originales vigilados en la entrada), asi que el lado original ejecuta el codigo del EXE.
Los falsos registran sus argumentos EN ORDEN (contenido de los vectores de pila, etiquetas de arena) y, tras el evento `mutAt`, mutan el remolque, el tractor, los globales (paso `0xB7CB5C`, word `0x8D313C`) o los campos que la funcion lee despues (12 tipos, ver `Fixtures.hpp`).
18432 pares, 18 familias (16384 + 2x1024): flujo normal, las cuatro combinaciones de bool con basura en los bytes altos, borde de la distancia (`1.0f * max(paso, 0.7f)` contra `sqrt` con +-4 ulps), borde de los dos productos escalares (-0.3f exacto, +-ulps, cancelaciones y todos los terminos -0), salidas anticipadas (sin tractor, los 32 estados, virtuales falsos con basura sobre AL), modelo grua/tractor contra el limite (limite negativo incluido), subtipo 11 y `+0x9EC` (-1000.0f +-1 bit), amortiguacion con paso 0, negativo, <1, >1, inf, NaN, posiciones/matrices/velocidades/masas/centro de masa especiales (NaN, inf, desnormales, ceros con signo, enormes), velocidad relativa nula (Normalise de longitud 0), fuerza desbordada, un barrido de mutaciones (5 eventos x 12 tipos, cada uno unas 17 veces) y dos familias de NaN/inf y desnormales/cancelaciones. Un unico patron de NaN (0xFFC00000).
Cada lado coincide con `fixture_oracle.py` (Python, aritmetica x87 24 bits con Fractions, raiz exacta; no usa constantes ni ramas de produccion) y la cobertura de ramas se mide con minimos (100; 20 por evento de mutacion).

## Pruebas negativas
Produccion mutada y restaurada (los hashes de `Vehicle.cpp` y `X87Math.cpp` coinciden con los buenos despues); ambas fallan por diferencia de resultado, sin caida:
1. `GetTowBarPos` sobre el tractor del primer acceso en vez del releido: **98/18432 diferencias** (`resultados-veh81a/20261028-213804-837800`).
2. `Veh81ABentCpp` con `<=` en vez de `<` en el borde -0.3f: **133/18432 diferencias** (`resultados-veh81a/20261028-214630-968052`).

## Defectos
Produccion: ninguno (la primera corrida compilada ya dio cero diferencias EXE/DLL y coincidio con el oraculo). Del banco: umbrales de cobertura (-0 en el producto del eje superior, vector relativo nulo y fuerza desbordada) afinados anadiendo casos a las familias 4, 8 y 13; un corte "GTA sin progreso" aislado se repitio. Del codigo previo: `AutomobileFinal4/CasesG5_a.hpp` parcheaba `UpdateTrailerLink` solo en la direccion del EXE (la DLL habria ejecutado el codigo real); ahora usa `H("CVehicle","UpdateTrailerLink",...)`.

## No cubre
sNaN y NaN con cargas/signos distintos (los ayudantes x87 compilados pueden cambiar el orden de operandos); la rama de posicion compacta (`+4`) cuando la matriz es nula: es inalcanzable porque el EXE desreferencia ambas matrices en los productos escalares antes (el tractor se relee entre medias sin colaboradores); matrices nulas con las comprobaciones previas superadas (el EXE se cae); EAX/ECX/EDX de salida; los cuerpos de los colaboradores (`GetSpeed`, `ApplyForce`, los virtuales, `DotProduct`...: sus bancos).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Veh81A`; `python -B reversing/Veh81A/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
