<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HeatFire73: CVehicle::FireHeatSeakingMissile (0x006E05C0)

Estado: **APROBADO dentro del juego** (26-27/10/2026, worktree `D:\Original\wy-2`, HEAD 429302be1 + `source.patch`). 16384 pares (ampliado el 27/10/2026 a 21504, ver abajo), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, aritmetica exacta). Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-heatfire7361027-000013-470057`.

`source.patch`: Vehicle.cpp, X87Math.cpp y X87Math.h (`git apply --check -R` verificado contra el arbol).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| FireHeatSeakingMissile | 0x006E05C0 | CVehicle,FireHeatSeakingMissile | S |

No es virtual. thiscall `ret 0Ch` (objetivo, tipo, bool). Unico llamador del EXE: 0x6E3A72 (ProcessWeapons), que no lee EAX/ECX/EDX: solo se registran. Hasta ahora ProcessWeapons la llamaba por la direccion del EXE; con el hook, VehicleWeapons67 pone el mismo falso en el cuerpo de la DLL.

## Revision contra el EXE (0x6E05C0..0x6E07CE)
- Con el byte bajo de `arg2` distinto de 0: sale si `now <= GetPlaneOrdnanceRateOfFire(tipo) + ultimo` (sin signo, con desbordamiento); ultimo = +0x4FC si tipo == 1 (dword), +0x500 si no.
- Lee la posicion (matriz + 0x30, o this + 4) ANTES de pedir `GetPlaneOrdnancePosition(tipo)`. La matriz nula solo es viable si el intervalo corta el disparo: despues el EXE lee matriz + 0x10 y la velocidad sin comprobar nulo.
- Indice de ordenanza (bits 2-3 del byte +0x4A9): distinto de 0 niega la x de la ordenanza y pasa a 0; 0 no niega y pasa a 1.
- Origen del misil (x87, 24 bits, exponente extendido): `dot = (vz*dz + vy*dy) + vx*dx` sin guardar; `fs = dot < 0 ? 0 : dot`; `P = fs*dx` sin guardar; `ty = float(fs*dy)`, `tz = float(fs*dz)`; `A = P*paso`, `B = ty*paso` sin guardar; `Cz = float(tz*paso)`; `X = float(A + ordx)`; `Y = ordy + B`, `Z = ordz + Cz` sin guardar; salida `float(pos.x + X)`, `float(pos.y + Y)`, `float(pos.z + Z)`. Implementado en `HeatFire73SpawnCpp` (X87Math.cpp, bloque «Lote 73», /arch:IA32): cada paso en una variable volatile para impedir que el compilador reasocie; los NaN se siguen aparte con las reglas de la x87.
- `AddProjectile(this, 20, origen, 0.0f, &copia_local_de_la_direccion, objetivo)`: el EXE copia la direccion a un local y pasa su direccion (no la de la matriz); la DLL hace lo mismo.
- Temblor: `IsPlayer()` del conductor y tipo de ped 0/1 -> `GetPad(tipo)->StartShake(240, 160, 0)`; despues la hora en +0x4FC/+0x500.

## Defectos hallados por el banco
Defectos de PRODUCCION (corregidos durante el desarrollo): el signo de la negacion de la x de la ordenanza estaba invertido; el puntero de direccion pasado a AddProjectile era el de la matriz y no una copia local; el compilador reasociaba y reordenaba la x87 (se fuerza el orden con variables volatile); la carga de los NaN dependia del orden de operandos (se sigue con las reglas de la x87: un NaN de señal en memoria frente a un NaN en registro gana el del registro, entre dos NaN gana la mayor mantisa y a igualdad el de signo positivo); el paso del temporizador pasaba por valor (la copia via x87 silencia un NaN de señal): se pasa por puntero a `CTimer::ms_fTimeStep`.
Defectos del BANCO: el puntero de direccion de AddProjectile es de pila en el EXE (se registra como «fuera de las arenas»); el `static_assert` de la ventana del ped exigia sizeof(CPed). Ninguno relajo compuertas ni cobertura.

## Banco
Fixtures: 16384 casos, 16 familias (`n % 16`): intervalo (corta, frontera, sin comprobar), tipo, indice de ordenanza, velocidad negativa/positiva, valores especiales (NaN de señal y silenciosos no canonicos, infinitos, desnormales, ±0, magnitudes extremas), paso del temporizador, conductor jugador 0/1/otro/ninguno y mezcla. Arenas propias: CPlane completo (0xA04), objetivo, otra entidad, matriz y ped. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran sus argumentos EN ORDEN), y quien valida a cada uno (docs/hooks.csv y reversing/validation-registry.json):
- CVehicle::GetPlaneOrdnanceRateOfFire 0x6D4590 y CVehicle::GetPlaneOrdnancePosition 0x6D46E0: CVehE (256 pares cada una).
- CProjectileInfo::AddProjectile 0x737C80: ProjectileAdd355 (16384 pares).
- CPad::StartShake 0x53F920: RequestedPadMutationRemaining3Hex (89600 pares).
- CPed::IsPlayer 0x5DF8F0: CPedD (128 pares); se ejecuta de verdad en ambos lados, igual que CPad::GetPad 0x53FB70 (inline, sin hook).
Se comparan eventos, bytes cambiados de las 5 arenas, indice de ordenanza leido por el miembro real, compuertas de ABI (pila, no volatiles, DF, SEH), ruta (el EXE salta a la DLL solo en el candidato), estado x87 y restauracion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json`.

Prueba negativa (produccion mutada y restaurada despues): (1) `scaledY` sin guardar como float en HeatFire73SpawnCpp: 18 diferencias; (2) `<=` por `<` en el intervalo: el EXE/DLL se cae en los casos con matriz nula (AV dentro de GTA, el banco falla); (3) hora de disparo guardada en el campo equivocado (`type != 1`): 14463 diferencias.

No validado: la rama de posicion compacta (this + 4) con matriz nula (el EXE se cae al leer matriz + 0x10), el valor de EAX/ECX/EDX de salida (nadie lo lee) y la validez de los colaboradores (la prueban sus propios bancos).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh HeatFire73`; `python -I fixture_oracle.py <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).

## 2026-10-27: ampliacion del banco (21504 pares)
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-heatfire73\20261027-004516-852192` (APROBADO, 21504 pares, 0 diferencias; oraculo independiente `fixture_oracle.py`: 0 lados distintos del modelo). Sin cambios en produccion.
- Familias nuevas 16..20 (1024 casos cada una): 16 dot exactamente +-0 (ortogonalidad con cancelacion exacta, multiplos de potencia de dos, tres productos -0, ceros firmados); 17 intermedios (fs, ty, tz, A, B, Cz, X, Y, Z) y salidas desnormales; 18 dot negativo diminuto (se fija a 0) y positivo diminuto; 19 ceros firmados en velocidad/direccion y direcciones infinitas; 20 borde exacto `now == ultimo + rate` con matriz valida.
- Borde exacto: todo caso con intervalo comprobado y `time == limite` lleva ahora matriz valida (la familia 0 conserva la matriz nula solo con `time` estrictamente menor que el limite). Asi la mutacion `<=` -> `<` se ve como diferencia de resultado y no como caida.
- Cobertura medida (antes -> ahora): dot exactamente cero 2 -> 1410 (+0: 1207, -0: 203, por cancelacion: 674, negativo diminuto fijado a 0: 1023); salidas desnormales 44 -> 461 (x: 54, y/z: 417); intermedios diminutos: ty 1163, tz 1168, Cz 1055, X guardado 194, A 874, B 1203, Y 586, Z 604, fs 97; borde exacto con matriz valida 1116 (borde exacto con matriz nula 0 por diseno). `coverage.json`: minimos nuevos al 50 % de lo medido; dot_zero 705, out_denormal 230, out_denormal_x 27, out_denormal_y_or_z 208; ningun minimo anterior se bajo.
- Prueba negativa `<=` -> `<` en el intervalo (produccion mutada y restaurada, `git diff` vacio despues): 1116 diferencias de resultado (familias 1, 14 y 20), sin caida.
- Rama de posicion compacta (this + 4) con matriz nula: confirmada inalcanzable por el desensamblado (0x6E0602..0x6E0607 elige this + 4 y despues 0x6E0657 lee [edi+0x10..] y [esi+0x44..] con edi = 0): el EXE se cae. No se cubre; la matriz nula solo se prueba cuando el intervalo corta antes de leerla.
- VehicleWeapons67 repetido: APROBADO, 16384 pares, 0 diferencias, `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-vehicleweapons67\20261027-004833-019943`.
