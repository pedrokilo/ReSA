<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 01/11/2026 (Ciudad de México):** 820 grupos / 8987 funciones reconstruidas; 8987 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fx83A: Fx_c::AddWheelSpray (lote 83A)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD 87b80ac15 + `source.patch`). 16384 pares (16 familias de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, 0 lados distintos).
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fx83a\20261101-032620-582625`.

| Funcion | Direccion | Hook (clase,nombre) | Tipo | Firma del EXE |
|---|---|---|---|---|
| AddWheelSpray | 0x0049FB30 | Fx_c,AddWheelSpray | S | thiscall `ret 1Ch` (CVehicle*, CVector pos por valor, bool giro, bool agua, float luz); `this` no se usa |

Era una envoltura que llamaba al EXE; ahora es C++ puro (`Fx.cpp` + helpers x87 de `X87Math.cpp/.h`, bloque "Lote 83A") y esta enganchada con `RH_ScopedInstall`. Mide 0x3F0 bytes (desde 0x49FB30 hasta el `ret 1Ch` de 0x49FF18).

## Que hace (desensamblado leido del EXE)
1. `FindPlayerVehicle(-1, false)` (0x56E0D0) y distancia a la camara `s = (dz*dz + dy*dy) + dx*dx` (24 bits, en registro; camara = matriz+0x30 o 0xB6F02C). Sale si `s > 625` o NaN.
2. Cadencia: `clave = byte bajo del modelo (+0x22) + CTimer::m_FrameCounter`; `s > 400`: sale si `clave & 3`; si no, con `s > 64` o sin vehiculo del jugador sale si `clave & 1`.
3. `|v| = sqrt((x*x + y*y) + z*z)` de la velocidad (+0x44); si no es mayor que 0.01f y el byte de giro es 0, sale.
4. Multiplicador `FxPrtMult_c(1, 1, 1, 0.05, 0, 1.0, 0)` (0x4AB290): alfa = `(t + 1) * (0.2f agua | 0.15f)`, tamano `(t + 1) * 0.2f`, vida 0.08f, donde `t` = 1.0 si giran y si no `min(2*|v|, 1)` (el NaN conserva `2*|v|`).
5. `u = (t + 1) * 10` y `w = 30 - u` (floats); un `rand()` previo (`m = (r * 3.05e-5) * ((u + 30) - w) + w`; velocidad inicial `m * v` guardada como float).
6. Numero de particulas `n = max(1, EAX de _ftol2(sqrt((z'*z' + y'*y') + x'*x')))` con `v' = paso * v` en registro; `invN = float(1.0 / n)`.
7. Por particula (un `rand()` cada una): posicion `pos - float(float(invN*v*i) * paso)` por componente con `z + 0.25f`; velocidad = la inicial con `vz = ((r * 3.05e-5) + 1) * t + vz`; `AddParticle` sobre `g_fx.m_BoatSplash` (0xA9AE04, releido en cada vuelta), luz `lightMult`, limite 0.6f.

## Banco
Fixtures: 16384 casos, 16 familias (rangos 64/400/625 y sumas exactas, NaN, infinitos, desnormales, umbrales de velocidad 0.01 y 2*|v| = 1, contador/modelo extremos con envoltura de 32 bits, paso de tiempo especial, velocidad especial, bytes de basura en los argumentos bool, luz especial, sorteos de borde). El numero de particulas se acota en los fixtures (un valor entre 2^31 y 2^63 da un entero de 32 bits cualquiera y hasta 2000 millones de vueltas).
Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`MakeCollab`/`HookRoute`): `FxSystem_c::AddParticle(v3d)` 0x4AA440 y `FindPlayerVehicle` 0x56E0D0. Corren de verdad en ambos lados: `rand()` del CRT del EXE 0x821B1E (semilla fijada por lado y comparada al final), `FxPrtMult_c` 0x4AB290, `_ftol2`.
Oraculo independiente (`fixture_oracle.py`, enteros exactos con la x87 a 24 bits y exponente extendido): 16384 casos, 0 lados distintos (`python -B fixture_oracle.py <carpeta>/comparisons.jsonl`). Microtests de la x87 por hardware en `begin`.

## No cubre
- Mas de ~20000 particulas por llamada (mismo `for`/`jl`).
- Comportamiento de `AddParticle` y `FindPlayerVehicle` como funciones propias (falsos identicos; tienen su banco).
- Punteros nulos de vehiculo o posicion (el EXE se cae), EAX/ECX/EDX de salida (void).
- La integracion con el llamador real (`CVehicle::DoBoatSplashes`/`AddWheelParticles`): ver banco CVehD.
