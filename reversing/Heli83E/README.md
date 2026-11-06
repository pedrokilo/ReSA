<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 05/11/2026 (Ciudad de México):** 837 grupos / 9153 funciones reconstruidas; 9153 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Heli83E: CHeli::ProcessControlInputs 0x6C4830 (lote 83E)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD 2c71b35d2 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`; el pow solo con tolerancia) y microtests de la x87 del hardware.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-heli83e/20261101-065320-967839` (el manifiesto de construccion es el de esta corrida).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CHeli::ProcessControlInputs(uint8 playerNum)` (antes `plugin::CallMethod<0x6C4830,...>`, sin hook) | 0x6C4830 | `CHeli` / `ProcessControlInputs` / V (`RH_ScopedVMTInstall`: entrada de la vtabla y salto directo de la entrada del EXE) | `Entity/Vehicle/Heli.cpp` (ayudantes `Veh83D*` y `PowScaleFloatCpp` ya existentes; ningun ayudante nuevo) |

Fila de `docs/hooks.csv`: `CHeli,ProcessControlInputs,0x00000000,1,0,1,V` (los hooks virtuales se anotan con direccion 0 y tipo V; `approvals.json` usa 0x006C4830).

## Revision contra el EXE (0x6C4830..0x6C4E50, 0x621 bytes, thiscall, 1 dword, `ret 4`, void)
Argumento: numero de mando (`movzx edi, byte`: solo cuenta el byte bajo). `CPad::GetPad` (0x53FB70) es `Pads + n * 0x134` y no se falsea (corre de verdad en ambos lados).
1. `0x998 = (GetAccelerate - GetBrake) * 1/255` (ambos AX con signo, `fild` + `fmul` a 24 bits, guardado como float).
2. Con `0xB6EC2E` (raton en 3a persona) y `0xC1CC03` (vuelo con raton): modo raton si el raton se movio (`0xB73424` o `0xB73428` != 0, NaN cuenta) o, con algun eje fijado (`|0x994| > 0` o `|0x990| > 0`), el modo `0xC1CC04` es 2 y el mando esta en reposo (`GetSteeringLeftRight` == 0 y, solo entonces, `GetSteeringUpDown` == 0). Si no es modo raton se vuelve a preguntar al mando (izquierda/derecha, luego arriba/abajo): si alguno es distinto de cero o el modo no es 2 se actualiza como teclado; si no, no cambia nada. Sin las banderas siempre es teclado.
3. Teclado: `0xC1CC04 = 0`, `0x990 = GetSteeringUpDown / 128`, `0x994 = -GetSteeringLeftRight / 128` (en ese orden de llamadas).
4. Modo raton: `0xC1CC04 = 2`; si `m_bVehicleMouseLook` (palabra en `pad+0x2C`) es 0: `0x994 -= x * 0.0025` y `0x990 += y * 0.0025`; despues, para `0x994` y luego `0x990`, si `|eje| < 0.5` el eje se multiplica por `0.98 ^ paso` (pow del CRT, 0x822130).
5. Ambos ejes se acotan a [-1, 1] (`1 < v` -> 1, `-1 > v` -> -1; con NaN se conserva).
6. `0x98C = GetLookRight` (byte) y `-1.0` si `GetLookLeft` (byte). Con `GetHorn` (byte) y `matriz.arriba.z > 0` (nivelar el helicoptero): `0x98C = 0`; `CrossProduct(&v, &(0,0,1), matriz.derecha)` + `Normalise`, `0x990 = clamp2(((v.y*vy + v.z*vz) + v.x*vx) * estabilidad_cabeceo[+0x24])`; `CrossProduct(&w, matriz.adelante, &(0,0,1))` copiado a un local + `Normalise`, `0x994 = clamp2(... * estabilidad_balanceo[+0x1C])` con `clamp2` a [-2, 2] y el valor sin guardar decidiendo la cota.
7. Siempre: bit 5 de `0x428` a 0, `0x494 = 0`, `0x4A0 = 1.0`, `0x49C = 0`. Con la palabra `pad+0x10E` (DisablePlayerControls) distinta de cero: `FindPlayerPed(-1)->KeepAreaAroundPlayerClear()` y la velocidad (`0x44..0x4C`) se limita a 0.28 (`sqrt((x*x + y*y) + z*z)` a 24 bits, factor `0.28 / longitud` y producto guardado como float).
8. Con la salud (`0x4C0`) < 250: `0x998 = -0.1` y `0x98C += 0.5`.

## Cambios de produccion (`source.patch`)
`Heli.cpp`: funcion en C++ puro + `RH_ScopedVMTInstall(ProcessControlInputs, 0x6C4830)` (antes llamaba al EXE). Sin cambios en `X87Math`: reutiliza `Veh83DMul/Add/Sub/Div/Sqrt/Store` (registro de la x87 a 24 bits) y `PowScaleFloatCpp`.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (once): `CPad::GetAccelerate` 0x5403F0, `GetBrake` 0x540080, `GetSteeringLeftRight` 0x53FB80, `GetSteeringUpDown` 0x53FBD0, `GetLookRight` 0x53FE10, `GetLookLeft` 0x53FDD0, `GetHorn` 0x53FEE0 (devuelven el EAX del caso, con basura en los bits altos), `FindPlayerPed` 0x56E210, `CPlayerPed::KeepAreaAroundPlayerClear` 0x60C1E0, `CVector::Normalise` 0x59C910 y `CrossProduct` 0x59C730 (escriben las dos salidas del caso). Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, punteros a `CPad::Pads` por mando, locales de pila por su marca). `HookRoute` se construye con el cuerpo de la DLL tomado de la entrada de la vtabla (`Virtual::GetHookOurAddress`); no hizo falta tocar `HookRoute.hpp`.
18432 casos, 18 familias (16384 + 2x1024): sin raton / sin bandera de vuelo, raton movido, ejes fijados con mando en reposo, mando en movimiento, ejes nulos (sin cambios), modo distinto de raton, decaimiento junto a 0.5 y pasos de tiempo raros, bocina con el helicoptero boca arriba, bocina con arriba.z en el borde, cotas de +-1 y +-2, DisablePlayerControls y tope de velocidad, umbral de salud, bytes del mando, basura en las mitades altas de EAX, valores especiales, aleatorio general, NaN/infinitos y desnormales. Cada lado coincide con `fixture_oracle.py` (Python, fracciones exactas, x87 a 24 bits) y hay cobertura de ramas con minimos (`coverage.json`).

## Prueba negativa
Produccion mutada y restaurada (sha256 identico al bueno despues, `51055f7f...`): divisor del teclado 1/128 -> 1/64 (`kInv128`), factor de estabilidad de cabeceo -> balanceo en la primera nivelacion y umbral de salud 250 -> 251. Fallan por diferencia de resultado, sin caida (`resultados-heli83e/20261101-064955-240724`: **9407/18432 diferencias**, el lado original coincide con el oraculo en las 9407).

## Bancos repetidos (citan 0x6C4830)
Ninguno: `grep` de la direccion en `reversing/` solo la encuentra en este banco y en `_handoff/PROMPT_SIGUIENTE_CHAT.md`. Los bancos de `CHeli` ya registrados (HeliFly75, HeliPre75...) no ejecutan esta funcion.

## Fuera de los fixtures
NaN con cargas distintas (las fixtures usan un unico patron), numero de mando distinto de 0 y 1 (`CPad::GetPad` apuntaria fuera de `Pads`), estaticas booleanas `0xB6EC2E`/`0xC1CC03` distintas de 0/1, puntero de matriz o de datos de manejo en vuelo nulo, y salidas de `Normalise`/`CrossProduct` incoherentes con su entrada (los falsos devuelven lo que dice el caso; su calculo real esta validado en sus propios bancos). El pow del CRT solo se contrasta con el oraculo por tolerancia (1e-5); su igualdad bit a bit entre EXE y DLL si se comprueba en todos los casos.
