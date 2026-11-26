# Lectura del mando de GInputSA 1.11 (Silent): sticks, curva de sensibilidad, vibración y motor de expresiones regulares

Desviación propia **15.A** ([DesviacionesConocidas.md](../DesviacionesConocidas.md)). Módulo `source/game_sa/PadAnalog.{h,cpp}` (código C++ propio) enganchado por hunks mínimos en `CPad::ProcessPad`, `CPad::Update`, `CPad::StopShaking`/`StopPadsShaking` y los getters `AimWeaponUpDown`, `LookAroundLeftRight`, `LookAroundUpDown` de `Pad.cpp`.

Autorización: el usuario pidió en el chat que el standalone tenga «la rutina de lectura del mando (curvas y vibración), los coeficientes de la curva de sensibilidad y el motor de expresiones regulares» de GInputSA 1.11 (Silent) y confirmó que los agentes estudien `GInputSA.asi`. Análisis estático con `pefile`/`capstone`, **sin ejecutar el `.asi`**; no se copian bloques de código (las fórmulas están reimplementadas) y no se porta la capa de importación de XInput de GInput, ni Steam/Guide, ni SIXAXIS/SCP. Atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md).

Todas las direcciones de este documento son del `.asi` (base `0x10000000`) salvo que se diga lo contrario. Herramientas y bancos: `contrib/pad_analog/` (sección 9). Catálogo previo: [ginput/](ginput/) (resumen, modificaciones, estructuras, tablas).

## 1. Qué viene del `.asi` y qué es decisión propia (resumen)

| Tema | Del `.asi` (direcciones) | Decisión propia |
| --- | --- | --- |
| Lectura de sticks y gatillos de XInput | `0x10006EA0` (ruta XInput estándar), `0x10007550` (ruta SCP), `0x10007D70` (llama a ambas por pad) | La lectura de XInput sigue siendo la nuestra (`XInputPad::Read` en `Pad.cpp`, `XInputGetState` cargado en tiempo de ejecución); PadAnalog recibe el valor crudo |
| Zona muerta | `(\|v\|-dz)/(1-dz)`, `0x10006F16..0x10006FB2` | Con las claves sin configurar se mantiene el corte fijo actual del proyecto (0,3 sin reescalar) |
| Curva de sensibilidad | `0x10006FB6..0x10007020`: coeficientes exactos (sección 4) | Opción `SensitivityBothAxes` (GInput solo curva el eje X de cada stick) |
| Inversiones/permutaciones | Banderas por pad `0x1003AB80`, `0x1003ABD8`, `0x1003ABC0`, `0x1003AB64`, `0x1003ABBC`, `0x1003ABDC` | Se combinan (XOR) con las preferencias del menú del juego (`m_bInvertPadX1`…) |
| Southpaw, InvertLook, SwapSticksDuringAiming | `0x1003ACE8`, `0x1003AC94`, `0x1003ACC8` y los getters `0x10002790/2850/2920/29C0` | Southpaw se aplica a los sticks ya procesados (intercambio completo de los dos sticks); InvertLook/SwapSticksDuringAiming alternan el valor que ya tiene el juego |
| Gatillos | `0x100072A6..0x100072FD`: `(t-30)*1,1333`, 0..254 | Solo con la opción `AnalogTriggers` (por defecto apagada: hoy L2/R2 son digitales) |
| FaceButtonsSensitivity | `0x10009F5C..0x10009FB7` y `0x10007835..` (solo ruta SCP) | Implementado y probado en el banco; **sin efecto con XInput** (botones digitales; GInput tampoco lo usa en su ruta XInput estándar) |
| Vibración | `0x100074C1..0x10007546`, `0x100151D0` | Se activa por defecto (GInput `Vibration=1`); la preferencia de vibración del menú sigue mandando |
| `MapPadOneToPadTwo` | `0x10007D70`, `0x100155A6`, `0x1001511B` | `CPad::padNumber` pasa a 1 durante `ProcessPad` |
| Motor de regex | STL de MSVC enlazada estáticamente (sección 8) | Se usa `std::regex` (ECMAScript) de nuestro propio código, como ya hacía `PadButtonIcons.cpp` |

## 2. Mapa de la rutina de lectura (`.asi`)

- **`0x10007D70`** (por fotograma): `if (!MapPadOneToPadTwo) leer(0); leer(1)` (`cmp byte [0x1003AB5A]`). Con `MapPadOneToPadTwo=1` solo se lee el pad 1, que obtiene el estado del primer mando XInput (usuario 0: `0x100155A6..0x100155B7` y `0x1001511B..0x10015128`).
- **`0x10007550`** (`ecx` = índice de pad): primero llama a `0x10015580`, que invoca `XInputGetExtended` de la DLL de SCP (`GetProcAddress(xinput1_3, "XInputGetExtended")`, cadena en `0x100347D0`). Esa función solo existe en la DLL de SCP Driver Package: devuelve floats por botón (0..1) y por eje (−1..1) en el objeto del pad (`+0x1C..+0x6C`). Si no existe (la DLL de Windows) devuelve `0x32` y `0x10007550` salta (`jmp 0x10006EA0`, línea `0x10007D5A`): **ese es el camino que se usa con un mando Xbox normal y el que se porta**.
- **`0x10006EA0`**: llama a `0x100150C0`, que obtiene `XInputGetStateEx` por **ordinal 100** (`GetProcAddress(hXInput, MAKEINTRESOURCE(0x64))`, `0x100150F2`) y, si falta, el `XInputGetState` importado (`[0x1002C148]`). *(El catálogo previo decía «no usa el ordinal 100»: lo importa estáticamente solo por los ordinales 2 y 3, pero además obtiene el 100 en tiempo de ejecución con `GetProcAddress`; el 100 solo añade el botón Guía, `0x400` de `wButtons`, que se guarda en `+0xD0` y no se porta.)* El estado queda en `objeto+0x0C` (`XINPUT_STATE`); `objeto+0x14..0x1A` = `sThumbLX/LY/RX/RY`, `+0x10` = `wButtons`, `+0x12`/`+0x13` = gatillos.
- Escribe en **`CPad+0xA8` (`PCTempJoyState`)** del pad `edi = CPad::Pads + índice*0x134`: LSX `+0xA8`, LSY `+0xAA`, RSX `+0xAC`, RSY `+0xAE`, L1 `+0xB0`, L2 `+0xB2`, R1 `+0xB4`, R2 `+0xB6`, ↑ `+0xB8`, ↓ `+0xBA`, ← `+0xBC`, → `+0xBE`, Start `+0xC0`, Select `+0xC2`, ■ `+0xC4`, ▲ `+0xC6`, ✕ `+0xC8`, ○ `+0xCA`, L3 `+0xCC`, R3 `+0xCE`, Guía `+0xD0`.
- Botones (ruta estándar, `0x10007304..0x1000749E`): digitales, `neg/sbb/and 0xFF` → 0 ó 255. Asignación idéntica a la de `XInputPad::Read` de nuestro proyecto: A=✕, B=○, X=■, Y=▲, LB=L1, RB=R1, Back=Select, Start, L3, R3 y cruceta. **GInput escribe los botones directamente en esos campos, sin pasar por la asignación de acciones del juego; nuestro proyecto sí pasa** (decisión previa del proyecto, no se cambia).
- Al final: `if (pad==0) llamar a 0x10002320(CPad+0xA8)` (copia, no analizada) y la parte de vibración (sección 6).

## 3. Fórmulas exactas (ruta XInput estándar, `0x10006ED7..0x1000729A`)

Entrada: `sThumb*` (int16) × `3,051850947599719e-05` (`qword 0x10034800` = 1/32767). `dz` = zona muerta del pad (float `[0x1003ABD0+4*pad]` izquierda, `[0x1003AB84+4*pad]` derecha); `s` = sensibilidad (`[0x1003AC8C+4*pad]` izquierda, `[0x1003ABC4+4*pad]` derecha).

1. **Zona muerta** por eje: si `dz > |v|` → 0; si no, `v > 0 ? (v-dz) : (v+dz)`, dividido por `(1-dz)`. (Un `|v| == dz` entra en la rama de reescala y da 0.)
2. **Curva** (solo al eje **X** de cada stick; el Y no pasa por ella, ni en la ruta estándar ni en la SCP):
   - `s <= 1`: `f(x) = s·x + (1−s)·x³` (0 = cúbica pura, 1 = lineal **exacta**).
   - `s > 1`: `f(x) = (s−1)·cbrt(x) + (2−s)·x` (2 = raíz cúbica pura), con `cbrt` = `0x10017846`.
   - La condición del `.asi` es `fcom` de `1,0` contra `s` (`0x10006FB8`): `s > 1` va a la rama con `cbrt`. *(El catálogo previo decía lo contrario («si 1 > s se usa pow»): invertido, y la función no es `pow` sino `cbrt`.)*
3. **`0x10017846` es una `cbrt`**: normaliza el exponente a múltiplo de 3 (`idiv 3` sobre el exponente de `frexp`), aproximación racional `(0,24379·x²+0,95807·x+0,07892)/(x+0,27962)` (constantes `0x1002D7C8/D8/C0/D0`), dos iteraciones de Halley y `ldexp(.., e/3)`. Evaluado el tramo `0x100178CB..0x1001791D` con el intérprete de `contrib/pad_analog/x87_interp.py` sobre 2000 valores de `[0,125, 1]`: error relativo máximo frente a `cbrt` de **2,2e-16**.
4. **Inversión**: X se niega si `InvertXAxis != 0`; Y se niega si `InvertYAxis == 0` (el convenio del juego: arriba = negativo).
5. **Tope** a `[-1, 1]` (comparaciones con `-1,0` en `0x10034848` y `+1,0`).
6. **Permutación**: con `SwapAxes`, `CControllerState.LeftStickX` recibe el eje Y y `LeftStickY` el X.
7. **Escala**: `trunc(valor × 127)` con `_ftol2` (`0x1002AF10`, truncamiento hacia cero). Rango resultante −127..127.
8. **Gatillos** (`0x100072A6`): `t` (0..255) ≤ 30 → 0; si no `trunc((t − 30) × 1,1333333253860474)` (`float 0x1003480C`). Máximo 254 (255 da 254,99998).

Constantes del INI (lector `0x10009CE0`, «× 0,01f» con redondeo x87 a float y topes): zona muerta 0..**0,75** (`0x10034808`), sensibilidad 0..**2,0** (`0x10034814`), por defecto `LeftStickDeadzone=24` (`push 0x18`), `RightStickDeadzone=27` (`push 0x1B`), `Left/RightStickSensitivity=100` (`push 0x64`), `SixaxisSensitivity=150`, `FaceButtonsSensitivity=50` (`push 0x32`).

### Ruta SCP (`0x10007550`, solo para completar el análisis; no se porta)
Misma zona muerta/curva/inversión/permutación pero sobre floats (sin tope ±1: con `s>1` puede pasar de 127) y con `s>1` **sin** `cbrt`: usa el polinomio `s·x + (1−s)·x³` también para `s>1` (`0x10007002..0x1000701E`; da por ejemplo 138 para x=0,8 y s=2). Botones analógicos: `trunc(b × factor)` con tope 255 y `factor = max(255, 25500/FaceButtonsSensitivity)` (`FaceButtonsSensitivity=0` → factor infinito, `0×∞` = NaN → 0). Gatillos SCP: `t > 30/255` → `trunc((t − 30/255) × 289)`.

## 4. Tabla de la curva (salida en [−127, 127] para el eje X, tras la zona muerta; generada por `bench_core`)

| entrada | s=0 | 25 | 50 | 75 | **100** | 125 | 150 | 175 | 200 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0,05 | 0 | 1 | 3 | 4 | **6** | 16 | 26 | 36 | 46 |
| 0,10 | 0 | 3 | 6 | 9 | **12** | 24 | 35 | 47 | 58 |
| 0,20 | 1 | 7 | 13 | 19 | **25** | 37 | 49 | 62 | 74 |
| 0,30 | 3 | 12 | 20 | 29 | **38** | 49 | 61 | 73 | 85 |
| 0,40 | 8 | 18 | 29 | 40 | **50** | 61 | 72 | 82 | 93 |
| 0,50 | 15 | 27 | 39 | 51 | **63** | 72 | 82 | 91 | 100 |
| 0,60 | 27 | 39 | 51 | 64 | **76** | 83 | 91 | 99 | 107 |
| 0,70 | 43 | 54 | 66 | 77 | **88** | 94 | 100 | 106 | 112 |
| 0,80 | 65 | 74 | 83 | 92 | **101** | 105 | 109 | 113 | 117 |
| 0,90 | 92 | 98 | 103 | 108 | **114** | 116 | 118 | 120 | 122 |
| 1,00 | 127 | 127 | 127 | 127 | **127** | 127 | 127 | 127 | 127 |

Con las claves por defecto de GInput (zona muerta 24 %/27 %) la entrada de la tabla es `(|v|−dz)/(1−dz)`: p. ej. una inclinación del 50 % del recorrido con `dz=0,24` da `x=0,342` → 43 (s=100).

## 5. Inversiones, Southpaw, InvertLook y SwapSticksDuringAiming

- Banderas por pad (tabla del INI): `InvertLeftXAxis`→`0x1003AB80`, `InvertLeftYAxis`→`0x1003ABD8`, `SwapLeftAxes`→`0x1003ABC0`, `InvertRightXAxis`→`0x1003AB64`, `InvertRightYAxis`→`0x1003ABBC`, `SwapRightAxes`→`0x1003ABDC`, `Southpaw`→`0x1003ACE8`, `SwapSticksDuringAiming`→`0x1003ACC8`, `InvertLook`→`0x1003AC94` (**se guarda `valor == 0`**).
- **Getters leídos completos** (filas 68-71 del catálogo): `AimWeaponLeftRight` (`0x10002790`) y `AimWeaponUpDown` (`0x10002850`) eligen entre el stick derecho y el izquierdo (Southpaw con el pad activo los intercambia), con la regla de pasajero/atado (`ped+0xFC`, `ped+0x58C`, `ped+0x46C & 0x100`) de «usar el de mayor valor absoluto»; `AimWeaponUpDown` niega el resultado si el byte de `InvertLook` es distinto de 0 (es decir, con `InvertLook=0` del INI). `LookAroundUpDown` (`0x10002920`) y `LookAroundLeftRight` (`0x100029C0`): el stick primario es el derecho si `SwapSticksDuringAiming=0` y el izquierdo si es 1; en `Mode == 1` (juego de controles 2) el secundario vale 0; sin el indicador `[0x1003AE0C]` se elige el de mayor valor absoluto; `LookAroundUpDown` aplica el mismo `InvertLook`.
- **En nuestro código**: `Pad<n>.InvertLook` y `Pad<n>.SwapSticksDuringAiming` **alternan** (XOR) el valor que ya tiene el juego (`bInvertLook4Pad`, `byte_8CD782`) en `AimWeaponUpDown`, `LookAroundUpDown` y `LookAroundLeftRight`; sin configurar no cambia nada. No se portó `SwapSticksDuringAiming` a los demás getters que GInput toca con ese byte (`0x10002DB7`, `0x10002E87`, `0x1000895C`, `0x10014D7B`: zoom del francotirador y similares).
- **Southpaw** de GInput se hace getter a getter (`0x1003ACE8` en ~40 funciones); el nuestro intercambia los dos sticks ya procesados en `PCTempJoyState`, lo que equivale a «todo lo asignado al stick izquierdo pasa al derecho y viceversa» (la descripción del INI), pero también afecta a la cámara y a la conducción, donde GInput solo intercambia las acciones que cita.

## 6. Vibración

- `StopShaking`/`StopPadsShaking` (`0x10004660`, `0x100046D0`): ponen `ShakeFreq`/`ShakeDur` a 0 y llaman a `XInputSetState(pad, 0, 0)`.
- Por fotograma (`0x100074C1..0x10007546`, idéntico en la ruta SCP `0x10007CD0..0x10007D4F`): si `ShakeDur != 0` y el pad está activo (`objeto+0x7D`: el último dispositivo usado es el mando) y `CTimer::m_snTimeInMilliseconds − CPad::LastTimeTouched <= 60 000` (comparación con signo): potencia = `ShakeFreq(byte) × 0x101` (0..65535) **solo en el motor izquierdo** (`0x100151D0` construye `XINPUT_VIBRATION{left, 0}`); después `ShakeDur += trunc(CTimer::ms_fTimeStep × −20,0)`; si `ShakeDur > 0` sigue, si no `ShakeDur = 0` y envía 0.
- `0x100151D0` solo llama a `XInputSetState` si la potencia cambia (`cmp [pad+0x78]`); con `MapPadOneToPadTwo` y pad 1 usa el usuario XInput 0.
- INI `Vibration` → `FrontEndMenuManager+0x20` (`m_PrefsUseVibration`); el constructor del menú ya no lo fuerza a 1 (parche `0x5744A3`).
- En el EXE original/nuestro proyecto no había **ninguna** salida de vibración al mando (`StopShaking` es un NOP y nada llama a `XInputSetState`); PadAnalog es la primera.

## 7. Decisiones propias de PadAnalog

- Opciones (entorno > `gta-reversed.ini` > defecto): `GTASA_PAD_ANALOG` / `PadAnalog` (defecto **1**), `GTASA_PAD_ANALOG_GINPUT` / `PadAnalogGInput` (defecto 0), `GTASA_PAD1_<CLAVE>` y `GTASA_PAD2_<CLAVE>` / `Pad1.<Clave>`, `Pad2.<Clave>` con las claves de `GInputSA.ini` (`LeftStickDeadzone`, `RightStickDeadzone`, `LeftStickSensitivity`, `RightStickSensitivity`, `InvertLeftXAxis`, `InvertLeftYAxis`, `SwapLeftAxes`, `InvertRightXAxis`, `InvertRightYAxis`, `SwapRightAxes`, `Southpaw`, `SwapSticksDuringAiming`, `InvertLook`, `FaceButtonsSensitivity`), más `AnalogTriggers` y `SensitivityBothAxes` (propias), y globales `GTASA_PAD_VIBRATION` / `Vibration` (defecto 1) y `GTASA_PAD_MAPPADONETOPADTWO` / `MapPadOneToPadTwo` (defecto 0).
- **Valores por defecto = comportamiento de hoy** (el usuario validó «ya funcionan bien los controles»): sin zona muerta configurada se conserva el corte fijo de `CPad::ProcessPad` (`|pos| <= 0,3` sin reescalar, escala ×128), curva lineal, sin inversión extra, L2/R2 digitales. Con las claves por defecto `ProcessSticks` devuelve `false` y se ejecuta el código anterior **sin tocar** (comprobado por banco y en el juego). Los valores de GInput (24/27, escala ×127, gatillos analógicos, …) se activan con `PadAnalogGInput=1` o dando la clave.
- La zona muerta del INI es **por pad** y se aplica al valor crudo de XInput; si solo se cambia una clave (p. ej. la sensibilidad) con la zona muerta por defecto, se usa la zona muerta actual del proyecto (hard 0,3) con la curva encima.
- Las preferencias de inversión/permutación del **menú del juego** siguen funcionando y se combinan por XOR con las del INI.
- `AnalogTriggers`: `CPad::Update` (justo tras `ReconcileTwoControllersInput`, que pone todos los botones a 255) sustituye el 255 de L2/R2 por el valor analógico solo si vienen del mando (ni teclado ni ratón) y la asignación de botones del juego los ha puesto a no-cero. Por defecto apagado (cambiaría el manejo de los vehículos).
- `SensitivityBothAxes`: GInput solo curva el eje X (error probable del mod); con la opción también el Y. Por defecto 0 (igual que GInput).
- **Vibración encendida por defecto** (GInput `Vibration=1`): es una función nueva para el standalone (hoy no había ninguna). Se para sola con el menú abierto, sin mando, al salir y con `StopShaking`/`StopPadsShaking`; `GTASA_PAD_VIBRATION=0` o la preferencia de vibración del menú del juego la anulan.
- No se ha portado: la detección de «pad activo» de GInput (`0x10015230`, 60 s sin entrada), ni la lectura SCP/SIXAXIS, ni Guía, ni los getters de Setup 1/2 por `Mode`.

## 8. Motor de expresiones regulares (`0x10009680`): STL de MSVC enlazada estáticamente

**Conclusión: no hay motor propio; es `std::basic_regex<char>` de la STL de MSVC enlazada estáticamente en el `.asi`.** Por tanto no se porta nada: se usa `std::regex` de nuestro compilador (la misma STL de MSVC).

Evidencia (todas verificables con `pefile`):
1. **Mensajes**: en `.rdata` (`0x1002D060..0x1002D570`) están los 15 textos de `regex_error` de la STL de MSVC, uno por código, **con su formato literal** («regex_error(error_collate): The expression contained an invalid collating element name.», `error_ctype`, `error_escape`, `error_backref`, `error_brack`, `error_paren`, `error_brace`, `error_badbrace`, `error_range`, `error_space`, `error_badrepeat`, `error_complexity`, `error_stack` y los dos finales), indexados por una función de selección en `0x10015A64` (la tabla de punteros está inline en `0x10015A77..0x10015AD9`).
2. **RTTI** (`.data` `0x10039848..0x10039ABC`): `std::regex_error` (derivada de `std::runtime_error`), `std::locale::facet`, `std::locale::_Locimp`, `std::_Facet_base`, `std::ctype<char>`, `std::collate<char>`, `std::error_category`/`_Generic_error_category`/`_System_error_category`/`_Iostream_error_category`, `std::bad_alloc`, `std::length_error`, `std::out_of_range`: la maquinaria de `<regex>` (traits con `ctype`/`collate`/`locale`).
3. **Estructura**: `0x1000D730` es `basic_regex(const char*)` (`push 1; call 0x10015D13` = `locale()`, `strlen`, llamada a `_Reset`); `0x1000E430` es el constructor de `_Parser` (guarda `_Pfirst/_Pcurrent/_Plast`, construye el `_Nfa` con `0x1000EC50`); `0x1000E2F0` es `_Reset`, que cuenta referencias del AFN. Las cadenas son `std::string` de MSVC con SSO (`_Myres = 0xF`, `0x10`) en `0x10004CE0/0x10004EF0`.
4. **Uso** (`0x10009680`, el `CText::Get` encadenado): CRC32 de la clave GXT en mayúsculas (tabla CRC `0x10033100`), `bsearch` (`0x1001794F`) sobre 124 entradas de 8 bytes (hash, puntero) con comparador `0x10009520`, y por entrada (struct de `0x28` bytes en `0x100384D0`: clave de 8 caracteres, puntero al patrón, puntero al sustituto, regex compilada, resultado) compila el patrón con `basic_regex` y aplica la sustitución con el algoritmo de `regex_replace` (`0x1000E080`), con cacheo del resultado. El patrón y el sustituto son los de la tabla de 124 entradas ([ginput-tablas.md](ginput/ginput-tablas.md), sección 1); `$1` es la sintaxis de sustitución de ECMAScript.
5. **Dialecto**: los patrones usan solo literales, `.+`, `(.+)`, el carácter `~` y `$1`: subconjunto idéntico en ECMAScript y en cualquier motor con retroceso; no hay clases, rangos, anclas ni referencias hacia atrás. Las banderas exactas del constructor (`icase`, `ECMAScript`) **no se pudieron fijar** leyendo el `.asi` (el segundo argumento de `0x1000D730` se pasa pero el cuerpo lo ignora en la rama analizada y el constructor de `_Parser` sobrescribe su cuarto argumento); se comprobó que **no importan para esta tabla**: con `ECMAScript` y con `ECMAScript|icase` los 478 casos dan el mismo resultado.

### Verificación (banco `contrib/pad_analog/bench_regex.cpp`)
- `regex_tabla.py` compara las tres fuentes de la tabla: las 68 entradas estáticas leídas del `.asi`, el catálogo (124) y `kGxtFixes` de `source/game_sa/PadButtonIcons.cpp` (124): **0 diferencias**.
- Entrada típica: el texto **real** de cada una de las 124 claves en el `american.gxt` del juego (solo lectura), más un texto sintético con la secuencia, otro sin ella y otro con ella duplicada (478 casos). Esperado: calculado con `re` de Python (otro motor); el banco aplica `std::regex_search` + `std::regex_replace` con la misma secuencia que `PadButtonIcons::ApplyGxtFix`. Resultado: **478/478 iguales (ECMAScript y con `icase`)**. De los 124 textos reales, 122 contienen la secuencia del patrón; dos no (`DSERT8` tiene las teclas en el otro orden y `RUN_2P` usa `~H~left analog stick~w~`): `ApplyGxtFix` devuelve el texto original, igual que `regex_replace` en el `.asi` (sin coincidencia no cambia nada).
- Diferencias de dialecto entre el motor del `.asi` y el nuestro: ninguna observable (misma STL de MSVC; el `.asi` es de una versión anterior de la STL, pero la gramática ECMAScript y el algoritmo de reemplazo son los mismos).

## 9. Pruebas y herramientas (`contrib/pad_analog/`, `reversing/FnJuego/`)

- `x87_interp.py`: intérprete mínimo de x87 y enteros que evalúa **instrucción a instrucción** los bytes desensamblados con `capstone` (no ejecuta el `.asi`). `ginput_read_oracle.py` lo usa para la ruta XInput (`0x10006EED..0x100074A5`) y la ruta SCP (`0x1000758A..0x10007AC1`).
- `gen_oracle.py` + `bench_core.cpp` (`build_bench.cmd`): 7 504 vectores del `.asi` (6 000 de la ruta XInput con zonas muertas, sensibilidades, 64 combinaciones de banderas y gatillos aleatorios y de borde; 1 500 de botones/gatillos SCP; casos de factor infinito) frente a `PadAnalog.h`: **idénticos en 6 000/6 000 y 1 504/1 504**. Además comprueba que la tubería con la configuración por defecto reproduce el código actual de `ProcessPad` (64 combinaciones de inversión/permutación del menú × 4 001 valores = 256 064 casos, 0 diferencias) y valores concretos de Southpaw, inversión, permutación, curva de un solo eje o de ambos, motores y duración de la vibración.
- Escenario del juego `padanalog` (`reversing/FnJuego/escenario_padanalog.py`, con el `xinput1_4.dll` falso de `xinput_falso.c`, que ahora anota también las llamadas a `XInputSetState`): inyecta 9 vectores de sticks y gatillos y compara `CPad::Pads[n].NewState`; modos `actual`, `apagado`, `ginput`, `custom` y `map`; y comprueba la vibración escribiendo `ShakeFreq`/`ShakeDur` en memoria. Resultados en la tabla siguiente y en `reversing/FnJuego/ejecuciones/pa_*` (`sonda.txt`, `xinput_falso_vibracion.txt`).
- `regex_tabla.py` + `bench_regex.cpp` (`build_regex.cmd`): sección 8.

### Resultados del escenario del juego (standalone enlazado con esta rama; 9 vectores de sticks y gatillos por modo)

| modo (entorno) | qué se espera | sticks/gatillos | vibración (200 ⇒ motor izquierdo 51 400; se escribió `ShakeDur`=600 ms) |
| --- | --- | --- | --- |
| `actual` (por defecto) | el código actual (corte 0,3, ×128, L2/R2 = 255) | 9/9 | 562 ms, motores 51 400/0 y después 0/0 |
| `apagado` (`GTASA_PAD_ANALOG=0`) | lo mismo que `actual` y ninguna llamada a `XInputSetState` | 9/9 | 0 llamadas |
| `ginput` (`GTASA_PAD_ANALOG_GINPUT=1`) | la rutina del `.asi` (zonas muertas 24/27, ×127, gatillos analógicos 1..254) | 9/9 | 609 ms |
| `custom` (`PAD1_LEFTSTICKDEADZONE=24`, `..SENSITIVITY=50`, `INVERTLEFTXAXIS`, `SWAPRIGHTAXES`, `INVERTRIGHTYAXIS`, `SOUTHPAW`, `ANALOGTRIGGERS`) | la rutina del `.asi` con esas banderas y el intercambio de sticks | 9/9 | 609 ms |
| `map` (`GINPUT=1` + `MAPPADONETOPADTWO=1`) | los sticks llegan a `Pads[1]`, `Pads[0]` queda a 0 | 9/9 | 563 ms, `XInputSetState` al usuario 0 |

Verificación final de la rama (25/11/2026): standalone enlazado con `enlazar.cmd` (0 símbolos sin resolver); `audit_hook_catalog.py --strict` OK; `tamanos_objetos.py lint` OK; humo con la ASI de la rama (`prueba_ropero.py` y `prueba_marcadores.py` con `cal_mal.args`, `AGUA_ASI` = ASI enlazada con los `.obj` de `wv-int` y los de esta rama): **VEREDICTO BIEN** en ambos; ejecución del standalone de 140 s con `PadAnalogGInput=1` y el XInput falso: sin caída, `excepciones=0` (`reversing/FnJuego/ejecuciones/pa_120s-*`). Nota: en algunas ejecuciones (también con `PadAnalog=0`) el log trae `UNHANDLED EXCEPTION` en el arranque (controlador de la gráfica Intel integrada, `igd12um32xel`, durante la enumeración de adaptadores D3D) y el latido marca `excepciones=1`: es ajeno a este cambio (se repite la ejecución cuando ocurre).

El «esperado» de los modos `ginput`/`custom`/`map` lo calcula `ginput_read_oracle.py` (el `.asi` interpretado) con las preferencias de ejes del menú del juego leídas de la memoria (en esta instalación `m_bInvertPadY2` está activada, y se combina por XOR). Los modos `actual`/`apagado` se comparan con una réplica en Python del código anterior de `ProcessPad`.


## 10. Lo no verificado

- **Mando físico no probado.** Todo se probó con el XInput falso (`xinput_falso.c`) y con el intérprete del `.asi`; no se comprobó el tacto de la vibración ni de la curva con un mando real.
- El intérprete evalúa en doble precisión; el `.asi` corre con la precisión x87 del proceso del juego. En 6 000 vectores no hubo ninguna diferencia, pero no se descarta un caso límite de ±1 en el truncamiento.
- La detección de «pad activo» de GInput (`0x10015230`, `objeto+0x7D`) y `0x10002320` no se analizaron; la vibración usa «mando XInput conectado» como equivalente.
- Los demás getters de Setup 1/2 (`GetSteering*`, `GetCarGun*`, `GetWeapon`…) no se leyeron línea a línea (ver las filas del catálogo en la sección 11 del informe).
- `0x10015230` y la cuarta dirección de `_Reset` (banderas del `basic_regex`) quedan sin fijar.
