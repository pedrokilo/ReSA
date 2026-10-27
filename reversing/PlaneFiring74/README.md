# PlaneFiring74: CVehicle::GetPlaneWeaponFiringStatus (0x006E3440)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`, HEAD f13a750cf + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-planefiring74\20261027-031058-711567`. (Las corridas 025527 y 025846 fallaron solo por minimos de cobertura de ramas propios del banco, ya reforzados con fixtures; no hubo diferencias.)

`source.patch`: Vehicle.cpp, X87Math.cpp y X87Math.h (`git apply --check -R` verificado).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| GetPlaneWeaponFiringStatus | 0x006E3440 | CVehicle,GetPlaneWeaponFiringStatus | S |

No es virtual. thiscall `ret 8` (bool& estado, eOrdnanceType& tipo); **EAX = objetivo** (lo lee ProcessWeapons, `mov ebx,eax`). ECX/EDX los sobrescribe el llamador (solo se registran). Unico llamador: 0x6E39E3.

## Revision contra el EXE (0x6E3440..0x6E37ED; tabla de saltos 0x6E37F0, indices 0x6E3804)
- Siempre primero `FindPlayerSlotWithVehiclePointer(this)` y `GetPad(slot)`. Despues tabla por modelo: indice = (int16 modelo - 425), sin signo > 0x97 -> defecto. 0: Hunter(425) y 576; 1: 447, 464, 487, 497, 548; 2: Rustler 476; 3: Hydra 520.
- Escrituras exactas (el bool se escribe SIEMPRE como un byte: ProcessWeapons lee ese dword entero y reenvia los bytes altos como basura de pila): tipo 2: L1 (GetCarGunFired==2) -> bool=1; tipo tras eso=0. Hunter: L1 -> bool=1 y sale sin tocar el tipo; si no, CarGunJustDown==1 -> tipo=1 sin tocar el bool; si no, bool=0 y tipo=0. Grupo 1: L1 -> bool=1; RC Baron (0x1D0) con bytes globales 0xC1CC00 y 0xB6EC2E no nulos relee GetCarGunFired y ==1 -> bool=1; tipo=0. Defecto: bool=0, tipo=0.
- Hydra: tipo=0; si CarGunJustDown==1 y `ahora > +0x4D4 + 2000` (sin signo, con desbordamiento): bengala = `AddProjectile(this, 58, pos + (adelante*-2.5 + arriba*-1.0), 0.0, &matriz+0x10, nulo)` y +0x4D4 = ahora (matriz usada sin comprobar nulo). Luego 2.o CarGunJustDown: ==2 -> `IsLockedOn(0)` y +0x9DC != 0 -> `Scan(+0x9E0)`; si objetivo no nulo, igual a +0x9E0 y `ahora - +0x9DC > 1500` (sin signo): tipo=2, mira 0 (color FF,0,0, rotacion 1.0, tiempo 0) y devuelve el objetivo; si no, tipo=1. Si no es 2: GetEnterTargeting -> +0x9DC=ahora, +0x9E0=0; si no, GetTarget -> (+0x9DC==0 -> ahora), Scan(+0x9E0), si nulo o distinto -> +0x9DC=ahora, mira: tiempo 0 y (>1500: FF,0,0 rotacion 1.0; si no FF,FF,FF rotacion 0.0), +0x9E0=objetivo; si no, ambos a 0. Siempre devuelve nulo salvo el caso de disparo guiado.
- Bengala (x87 24 bits, exponente extendido): `t1=float(adelante*-2.5)`, `t2=float(arriba*-1.0)`, `s=float(t2+t1)`, `salida=float(s+pos)`; helper `PlaneFiring74FlareCpp` (bloque «Lote 74» de X87Math.cpp), con las reglas de NaN de HeatFire73 validadas aqui contra el hardware.

## Defectos hallados
Ninguno de PRODUCCION tras la primera compilacion (el banco paso a la primera). Defectos del BANCO: minimos de cobertura de ramas propios no alcanzados al principio (`hydra_target_edge_1500/1501`, `out_negzero`, `sum_zero_cancel`), corregidos reforzando fixtures (no se relajo ningun minimo); la mutacion `>=` en el intervalo de bengalas provocaba una caida con matriz nula en el borde exacto, corregido con matriz valida en el borde (la mutacion se ve ahora por diferencia de resultado). Ninguno relajo compuertas ni cobertura.

## Banco
18432 pares, 18 familias (`n % 16` mas 16 y 17, 1024 casos cada una): Hunter, grupo 1, RC Baron con banderas, Rustler, bengala (moderada, borde del intervalo, valores especiales, extremos, dos NaN por componente, ceros/cancelaciones), L1 y no-L1 del Hydra con bordes de 1500 ms y desbordamiento, modelos fuera de tabla, ranuras de pad arbitrarias, matriz nula sin bengala, aleatorias. Retornos de los falsos con basura en los bits altos (AX/AL). Arenas: CPlane completo, objetivo, otra entidad, matriz, salidas (vecinos del bool/tipo vigilados); globales: hora, 0xC1CC00, 0xB6EC2E y gCrossHair[2] (rellena con 0xA5 y restaurada).
Colaboradores sustituidos por falsos identicos en la direccion del EXE Y en el cuerpo de la DLL (registran argumentos EN ORDEN) y quien valida a cada uno (docs/hooks.csv y reversing/validation-registry.json): CWorld::FindPlayerSlotWithVehiclePointer 0x564000: WldC; CPad::GetCarGunFired 0x53FF90, CarGunJustDown 0x53FFE0, GetEnterTargeting 0x5406B0, GetTarget 0x540670: RequestedPadSelectors; CWeaponEffects::IsLockedOn 0x742BD0: Ped9; CVehicle::ScanAndMarkTargetForHeatSeekingMissile 0x6E0400: HeatSeek72; CProjectileInfo::AddProjectile 0x737C80: ProjectileAdd355. CPad::GetPad (inline, 0x53FB70) se ejecuta de verdad.

## Pruebas
- Oraculo independiente: `python -I -B fixture_oracle.py <carpeta>/comparisons.jsonl` -> 18432 casos, 0 lados distintos del modelo (tambien integrado en `run_local.py`).
- Hardware x87: `python -B flare_nan_hw.py` -> 26 175 616 combinaciones (todas las ternas de 56 patrones, 20 M aleatorias densas en NaN, 6 M sin NaN), 0 discrepancias; control negativo (regla de empate invertida en una copia): 7 641 912 discrepancias. `run_local.py --nan-gate` lo ejecuta antes del banco.
- Prueba negativa (produccion mutada y restaurada; `Vehicle.cpp` identico al bueno despues): (1) `>` por `>=` en el intervalo de bengalas (+0x4D4+2000): 547 diferencias de resultado, sin caida; (2) `>` por `>=` en el umbral de 1500 ms: 211 diferencias, sin caida.
- VehicleWeapons67 repetido con el falso tambien en el cuerpo de la DLL: ver su README.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh PlaneFiring74`; `python register_hook.py <carpeta-de-evidencia>` (coordinador). Fila de hooks.csv: `CVehicle,GetPlaneWeaponFiringStatus,0x006E3440,1,0,1,S`.

No validado: la rama de posicion compacta (this+4) con matriz nula en la bengala (el EXE se cae); ECX/EDX de salida (nadie los lee); cuerpos de los colaboradores (sus bancos); el microtest de hardware compila el bloque en otra unidad de traduccion (mismas opciones).
