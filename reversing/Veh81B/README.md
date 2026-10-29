<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veh81B: CVehicle::UpdateTractorLink 0x6E0050 (lote 81B)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wv-2`, HEAD ad52bea16 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y 576 microtests de FCOMP del hardware contra el mismo modelo.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh81b/20261028-203916-124228` (run id `20261028-203916-124228`; segunda corrida valida: la primera, `20261028-203033-494347`, fallo por un defecto del BANCO, ver abajo; la produccion no cambio).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CVehicle::UpdateTractorLink(bool, bool)` (antes `((void(__thiscall*)(CVehicle*,bool,bool))0x6E0050)(this, ...)`, sin hook) | 0x6E0050 | `CVehicle` / `UpdateTractorLink` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 6 ayudantes x87 en `X87Math.cpp/.h` (bloque "Lote 81B", solo anadido) |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,UpdateTractorLink,0x006E0050,1,0,1,S`.

## Revision contra el EXE (0x6E0050..0x6E03FD, 941 bytes = 0x3AD, coincide con lo medido)
thiscall con dos `bool` empujados como dwords (`ret 8`), void. `this` es el tractor; el remolque es el puntero de `+0x4C8` (`m_pVehicleBeingTowed`). Nadie lee EAX/ECX/EDX de salida (void).
1. `T = [this+0x4C8]`, nulo -> sale. `T->GetTowHitchPos(&B, 1, this)` (virtual +0xEC) y, con `T` RELEIDO, `this->GetTowBarPos(&A, 1, T)` (virtual +0xF0); ambos vectores arrancan a cero y solo se prueba AL (basura en los bits altos de EAX).
2. `C = B - A` (float). Si el word `+0x22` es 0x20D o 0x213 (grua y tractor): sale si `word [+0x86C] > 20000 - 100` (comparacion con signo de 32 bits; 20000 = `[0x8D313C]`, constante en la DLL).
3. `T = [this+0x4C8]` (relee): `B -= pos(T)` y `A -= pos(this)` (matriz `+0x14` + `0x30`, o `+0x04` si la matriz es nula). `T->GetSpeed(B)` (con el `T` de este paso) y `this->GetSpeed(A)`; `rel = vT - vV`.
4. `arg0 == 0`: `T = [this+0x4C8]` (relee); con `arg1`: `rel = (0.1f * C) / max(1.0f, CTimer::ms_fTimeStep)` (0x40FE90, 0x420800, 0x4119D0: el `1/m` se guarda como float salvo en la componente z, que usa el del registro x87); sin `arg1`: `rel *= (1.0f - mV / (mT + mV)) * 0.5f`.
5. `T = [this+0x4C8]` (relee): si `[T+0x594] == 11` y el dword `[T+0x9EC] == 0xC47A0000` (remolque de equipaje): `at = [T+0x14] + 0x20` SIN comprobar nulo; `rel -= at * ((rz*az + ry*ay) + rx*ax)`.
6. `D = Multiply3x3([this+0x14] (relee, SIN comprobar nulo), this+0xA4)` (0x59C790); `R = rel; R.Normalise()` (0x59C910); `E = A - D`; `F = CrossProduct(&F, &E, &R)` (0x59C730; el resultado se lee por el puntero que DEVUELVE);
   `k = 1 / ((((Fx*Fx + Fy*Fy) + Fz*Fz) / turnMass) + 1 / mass)` (en el registro, sin guardar); `force = rel * k`; `this->ApplyForce(force, A, true)` (0x542B50); `byte [this+0xB8] = 0`.

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(UpdateTractorLink, 0x6E0050)` (offsets crudos para los campos que no tienen nombre probado: +0x594, +0x9EC, +0x86C, +0xB8). `X87Math.cpp/.h`: `Veh81BSub3Cpp`, `Veh81BScaleCpp`, `Veh81BDampCpp`, `Veh81BProjectCpp`, `Veh81BMul3x3Cpp` y `Veh81BForceCpp`
(24 bits con exponente extendido, `volatile float` donde el EXE guarda con `fstp`/`fst` y `double` donde queda en el registro; `const float&` para no silenciar sNaN). Las llamadas a 0x40FE90, 0x4119D0, 0x420800, 0x406D70 y 0x59C790 (sin hook) son aritmetica propia de la funcion en la DLL.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran argumentos EN ORDEN con el contenido de los vectores de pila): `CPhysical::GetSpeed` 0x542CE0 (thiscall, resultado oculto, `ret 0x10`), `CVector::Normalise` 0x59C910, `CrossProduct(out,a,b)` 0x59C730 y `CPhysical::ApplyForce` 0x542B50 (`ret 0x1C`).
Los dos virtuales los atiende la vtabla falsa de las arenas (ranuras 59 y 60; cualquier otra ranura cuenta como virtual inesperado). Normalise y CrossProduct no los encuentra `FindHook` (categoria "Core"): el cuerpo de la DLL se resuelve con `Body(&CVector::Normalise)` / `Body(&CrossProduct)` (como Helpers66D).
18432 pares, 18 familias (16384 + 2x1024): sin remolque, enganche o barra con AL = 0 (basura en los bits altos), gancho de grua/tractor alrededor de 19900, `arg0`/`arg1`/ninguno con basura en los bits altos de los dwords, remolque de equipaje (y con centinela o subtipo distintos), matrices nulas (posicion de reserva; la matriz aparece mas tarde por un falso),
paso de tiempo (1.0, vecinos, 0, negativo, NaN, inf, desnormal, enorme), masas especiales, NaN/inf/ceros/desnormales en todos los vectores (un unico patron de NaN, 0xFFC00000), `CrossProduct` que devuelve otro buffer, y un **barrido de mutaciones**: tras cada uno de los 7 eventos el falso cambia el remolque, una matriz (o su puntero), una masa,
el paso de tiempo, el modelo, el gancho o los campos de equipaje (14 tipos), para comprobar todas las relecturas. Cada lado coincide con `fixture_oracle.py` (Python, aritmetica x87 24 bits con Fractions) y hay cobertura de ramas con minimos (`coverage.json`; 20 por tipo/evento de mutacion).

## Pruebas negativas
Produccion mutada y restaurada (`Vehicle.cpp`/`X87Math.cpp` identicos a los buenos despues, comprobado con los hashes del manifiesto); las tres fallan por diferencia de resultado, sin caida (`resultados-veh81b/20261028-205455-879083`: **2471/18432 diferencias**, el lado original coincide con el oraculo en las 2471):
1. `ForceCpp` lee el resultado de `CrossProduct` del buffer de salida en lugar del puntero devuelto (1417 + 953 + 29 casos, los de `cross_mode`).
2. La componente z de `DampCpp` usa el `1/m` guardado como float en lugar del del registro x87 (953 + 27 casos).
3. La masa del remolque del factor de masas usa el remolque anterior sin releer (45 + 29 casos con la mutacion "remolque").
(Una variante previa de la negativa 3 que tambien dejaba sin releer el remolque de equipaje cayo con un acceso invalido tras 309 diferencias, porque el remolque antiguo tiene la matriz nula: se sustituyo por la anterior para fallar solo por resultado; corrida `20261028-205007-682502`.)

## Defectos hallados
- **Banco** (corrida `20261028-203033-494347`, 14516/18432 diferencias = todos los casos completos): `FindHook` no encontraba `CVector::Normalise` ni `CrossProduct`, asi que la DLL ejecutaba el codigo real mientras el EXE llamaba a los falsos. Corregido resolviendo el cuerpo de la DLL con `Body(...)`. Produccion sin cambios.
- **Banco antiguo `AutomobileFinal4`**: sustituia `0x6E0050` solo en la direccion del EXE (`E(...)`); ahora `H("CVehicle","UpdateTractorLink",...)` (ruta RH + cuerpo de la DLL). Ver "Regresion".
- Entorno: el disco E: estaba al 100 % (la copia de la sonda y la restauracion de `scripts/gta_reversed.pdb` fallaban con WinError 112/2); se movieron 82 sondas antiguas de `_baseline` a `D:\Original\_baseline_archive_wv2` (no se borro nada) y se restauro `scripts/gta_reversed.pdb` desde `normal.pdb` de la corrida PlayerMouse81D que la dejo sin PDB.

## Regresion
Bancos antiguos que citan 0x6E0050 (grep en `reversing/*/`): `AutomobileFinal4` y `Sys13`.
- `AutomobileFinal4` sustituia 0x6E0050 solo en la direccion del EXE (`E(0x6E0050,...)` en `CasesG5_a.hpp`): con el hook encendido la DLL habria ejecutado el codigo real. Ahora `H("CVehicle","UpdateTractorLink",0x6E0050,...)` (ruta RH + cuerpo de la DLL). Repetido: APROBADO, 24192 pares (`resultados-automobilefinal4/20261028-211358-516418`).
- `Sys13` ya parchea la direccion del EXE y la de la DLL (`Addr(&CVehicle::UpdateTractorLink)`). Su corrida cae con un acceso invalido en `CTrailer::PreRender` -> `CVehicle::PreRenderDriverAndPassengers` (codigo de la DLL que el banco de 02/10 no sustituye), tanto con el hook de este lote encendido (`resultados-sys13/20261028-212108-877400`) como SIN el (`20261028-214812-896290`): fallo anterior a este lote, tras 8994 pares sin diferencias (BreakTowLink, Constructor, GetTowBarPos, GetTowHitchPos); no llega a `SetTowLink`.

## No cubre
sNaN y NaN con cargas/signos distintos (el orden de operandos que elige el compilador cambiaria la carga con dos NaN); matriz del tractor nula en 0x6E02E8 o remolque nulo tras la primera lectura (el EXE se cae); EAX/ECX/EDX de salida; cuerpos de los colaboradores (sus bancos) y de los virtuales GetTowHitchPos/GetTowBarPos; el limite `[0x8D313C]` (constante en la DLL, el banco no lo cambia);
la lectura (muerta) de las masas cuando `arg1` esta activo (no es observable).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Veh81B`; `python -B reversing/Veh81B/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
