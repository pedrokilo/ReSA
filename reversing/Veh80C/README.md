# Veh80C: CVehicle::FireFixedMachineGuns 0x73DF00 (lote 80C)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wz-3`, HEAD e4f077aea + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87 en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-veh80c/20261028-115753-632762` (run id `20261028-115753-632762`; primera corrida, sin defectos de produccion).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CVehicle::FireFixedMachineGuns()` (antes `plugin::CallMethod<0x73DF00>`, sin hook) | 0x73DF00 | `CVehicle` / `FireFixedMachineGuns` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` + 2 ayudantes x87 en `X87Math.cpp/.h` (bloque "Lote 80C", solo anadido) |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,FireFixedMachineGuns,0x0073DF00,1,0,1,S`.

## Revision contra el EXE (0x73DF00..0x73E235, 822 bytes = 0x336, coincide con lo medido)
thiscall sin argumentos, `ret` simple, void. Unico llamador: `jmp 0x73DF00` en 0x73F459 (CVehicle::DoFixedMachineGuns); nadie lee EAX/ECX/EDX de salida.
1. `now` = reloj (0xB7CB84); si `now <= +0x4D4 + 150` (sin signo, con desbordamiento) sale sin tocar nada.
2. Matriz `+0x14` SIN comprobar nulo: `len = sqrt(top.y*top.y + top.x*top.x)`; si `0.1f > len` (ordenado) `len = 0.1f`; `inv = 1.0f/len`; `nx = float(inv*top.x)`, `ny = float(inv*top.y)` (se guardan como float); `nx*60.0f`, `ny*60.0f`.
   El primer canon usa el producto que queda en el registro x87 y el segundo el guardado como float (matematicamente identicos, ver abajo). `+0x4D4 = now` ANTES de la primera llamada.
3. Por cada canon (desplazamientos (2.0, 2.5, 1.0) y (-2.0, 2.5, 1.0)): `0x59C890` (matriz x vector; el segundo canon RELEE la matriz `+0x14`), tres `rand()` (el 1.o es el ruido en y, el 2.o en x, el 3.o en z; solo se usa el byte bajo menos 0x80),
   extremo = (dir.x + p.x, p.y + dir.y, p.z) guardado como float + ruido (x, y: `entero*0.015f` guardado como float; z: `entero*0.02f` en el registro), `DoTankDoomAiming(this, conductor RELEIDO +0x460, &p, &extremo)` y `FireOneInstantHitRound(p, extremo, 15)` con el extremo ya modificado.
4. `ReportWeaponEvent(0x95 = AE_WEAPON_FIRE_PLANE, 0x1F = WEAPON_M4, this)`; `--byte[+0x4AB]`; si llega a 0: `+0x4AB = 20` y `+0x4D4 = reloj RELEIDO + 1400`.

## Cambios de produccion (`source.patch`)
`Vehicle.cpp`: funcion en C++ puro + `RH_ScopedInstall(FireFixedMachineGuns, 0x73DF00)`. `X87Math.cpp/.h`: `Veh80CDirCpp` (normalizacion a 24 bits) y `Veh80CEndCpp` (extremo + ruido), `const float&` y memcpy implicito de bits (sin silenciar sNaN). Rand: `CGeneral::GetRandomNumber()` (rand del CRT).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran argumentos EN ORDEN con el contenido de los vectores de pila y la hora del arma en cada evento): `operator*(CMatrix,CVector)` 0x59C890, `DoTankDoomAiming` 0x73D1E0, `FireOneInstantHitRound` 0x73AF00, `CAudioEngine::ReportWeaponEvent` 0x506F40 (via MakeCollab/HookRoute, hooks apagados) y `rand` 0x821B1E (cuerpo DLL `CGeneral::GetRandomNumber`).
18432 pares, 18 familias (16384 + 2x1024): tiempos en el borde (-1/0/+1/+2, desbordamiento de `gun+150`), sin disparo con matriz nula, longitud horizontal alrededor de 0.1, NaN/inf/ceros/desnormales/enormes en la direccion y en los resultados de la multiplicacion,
cancelaciones exactas, enteros de rand en los bordes con basura en los bits altos, cargador (1, 0, 2, 255), y un **barrido de mutaciones**: tras cada uno de los 13 eventos, el falso cambia la matriz (a otra arena), el conductor, el cargador, la hora del arma o el reloj global (comprueba las relecturas, incluida la hora de la recarga); `DoTankDoomAiming` puede reescribir inicio y/o extremo. Un unico patron de NaN (0xFFC00000).
Cada lado coincide con `fixture_oracle.py` (Python, aritmetica x87 24 bits con Fractions, raiz cuadrada exacta) y cobertura de ramas con minimos (100; 20 por evento de mutacion).

## Pruebas negativas
Produccion mutada y restaurada (`Vehicle.cpp` identico al bueno despues); ambas fallan por diferencia de resultado, sin caida:
1. El segundo canon usa la matriz leida al principio (cache) en vez de releer `+0x14`: **661/18432 diferencias** (`resultados-veh80c/20261028-123930-326887`).
2. La recarga usa `now + 1400` en vez de releer el reloj: **186/18432 diferencias** (`resultados-veh80c/20261028-124357-575174`).

## Regresion
Defecto de banco antiguo corregido: `CarCtrlAir63` sustituia 0x73DF00 solo en la direccion del EXE (la DLL habria ejecutado el codigo real; con el hook encendido fallaba `collaborators_dll`); ahora usa `mk({"CVehicle","FireFixedMachineGuns",...})` (ruta RH + cuerpo de la DLL). Repetido: CarCtrlAir63 APROBADO (`resultados-carctrlair63/20261028-122930-420311`; la corrida previa sin corregir dio FALLO 4096/4096 por `collaborators_dll`) y CVehE (parchea EXE y cuerpo DLL) APROBADO (`resultados-cvehe/20261028-125045-107399`).

## No cubre
sNaN y NaN con cargas/signos distintos (el orden de operandos que elige el compilador cambiaria la carga con dos NaN); matriz nula con disparo (el EXE se cae); EAX/ECX/EDX de salida; cuerpos de los colaboradores (sus bancos); diferencia registro/float del primer canon (`nx*60` nunca difiere del valor guardado: nx es multiplo de 2^-149 y |nx*60| no desborda).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Veh80C`; `python -B reversing/Veh80C/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
