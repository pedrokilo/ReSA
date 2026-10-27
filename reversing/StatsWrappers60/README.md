<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco StatsWrappers60: CStats::FindLeastFavoriteRadioStation (0x559010), BuildStatLine (0x559230), UpdateRespectStat (0x55BC50) y ConstructStatLine (0x55A780)

**Estado: PREPARADO** (no compilado dentro del proyecto ni ejecutado en el juego; el coordinador compila y corre en serie). Verificado SIN juego con el arnes offline de `offline/` (ver abajo): el codigo
del EXE canonico y las funciones C++ de `Stats.cpp` dan el mismo resultado en las 24576 fixtures del banco y en una ampliacion de 240000 casos mas (los mismos generadores con indices de caso 4096.. / 6144.. / 6144.. / 8192.., 60000 por funcion), y ambos coinciden con
`fixture_oracle.py`. Worktree de trabajo: `D:\Original\wc-5` (basado en HEAD `26c881718`). Un solo banco para las cuatro funciones: 4096 + 6144 + 6144 + 8192 = **24576 pares**.

Plantillas: `reversing/FileLoaderLoad2dEffect27`, `reversing/DecisionMakeDecision27` y `reversing/ModifySpeed27` (compuertas, rutas, parches de entrada, `register_hook.py`) y el contrato
de `reversing/_handoff/BRIEF.md` + adendas de `BRIEF28.md` (lotes 29 y 30). El banco usa `__asm` en `Gate.hpp`/`Fakes.hpp` (solo en el banco; la produccion no lo usa).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh StatsWrappers60            # compila la sonda + corre + resume diferencias
# o a mano, siempre en serie:
python -B reversing/StatsWrappers60/build_probe.py
python -B reversing/StatsWrappers60/run_local.py
python -B reversing/StatsWrappers60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo analisis (None si el registro aun no existe o no ha terminado)
python -I reversing/StatsWrappers60/fixture_oracle.py                                            # autoprueba del oraculo (sin juego; imprime «ok»)
python -B reversing/StatsWrappers60/register_hook.py <carpetaEvidencia>                          # solo tras aprobar (revisar antes SCOPE_NOTE)
```
Requiere el ASI del arbol con los cuatro `RH_ScopedInstall` de `Stats.cpp` (categoria/clase `CStats`, hook `Simple`) y el bloque `Stw60` de `X87Math.cpp/.h`: aplicar `source.patch`
(`git apply`, solo `Stats.cpp`, `X87Math.cpp`, `X87Math.h`; verificado con `git apply --check` contra HEAD). Archivo de arranque en el clon: `statswrappers60-runtime.start`; salida
`statswrappers60-runtime.jsonl`. `docs/hooks.csv` no tiene las filas `CStats,<funcion>,<direccion>,1,0,1,S` (las anade el coordinador al registrar).

### Arnes offline (sin GTA; opcional, util para reproducir)
```
bash reversing/StatsWrappers60/offline/run_build.sh <carpeta de trabajo>      # extrae las funciones de Stats.cpp/X87Math (gen_port.py), compila harness.dll + harness.exe (x86)
<carpeta>/harness.exe <find|build|resp|cons> <primer caso> <cantidad> <salida.jsonl>
python -I reversing/StatsWrappers60/offline/check_rows.py <salida.jsonl> [--stats]   # ambos lados contra el oraculo
python -I reversing/StatsWrappers60/offline/calibrate.py <carpeta> [--write]         # minimos de cobertura de run_local.py
python -I reversing/StatsWrappers60/offline/make_fake_log.py <carpeta> <registro.jsonl>   # registro sintetico para probar run_local.py --analyze (NO es evidencia)
```
`host.exe` (sin CRT) esta enlazado en 0x400000 y reserva en su seccion `.gta` el rango 0x401000..0xCB1000; `harness.dll` copia encima las secciones de `D:\Grand Theft Auto San Andreas\gta-sa.exe`
(el EXE canonico, solo lectura), instala los MISMOS falsos que el banco (`Fakes.hpp`) en el EXE y en los cuerpos de la funcion C++ y compara EXE contra C++ con las mismas compuertas de ABI
(`Gate.hpp`) y el mismo estado de casos (`Cases.hpp`). No ejecuta nada del juego.

## Que hace cada funcion (desensamblado del EXE)

- **FindLeastFavoriteRadioStation (0x559010..0x55907D)**, sin argumentos, cdecl: recorre las estaciones 1..12 de `FavoriteRadioStationList` (0xB78E58) desenrollado de 6 en 6; el minimo inicial es la
  entrada 1 y gana la primera estrictamente menor (con signo). Devuelve el indice en EAX.
- **BuildStatLine(line, pValue1, metrics, pValue2, type) (0x559230..0x55950E)**, cdecl, 5 argumentos: sin `line` no hace nada; si no, `gString2[0] = 0` y rellena `gString2` con el sprintf del EXE
  (0x821BB5): `type == 1` -> `"%d:0%d"` / `"%d:%d"` (segundo valor < 10 o no; ambos punteros se leen sin comprobar); si no, con `pValue2` solo las metricas 0 (`" %d %s %d"` con `FEST_OO`),
  1 (`"%.2f %s %.2f"`) y 3 (`"$%.2f %s $%.2f"`); con un solo valor las metricas 0..9 sin signo (0 `%d`, 1 `%.2f`, 2 `%0.2f%%`, 3 `$%.2f`, 4 `%d|`, 5 peso `%dkgs` con un **double** a un `%d`
  (x87: `fild * 0.4536f`) o `%dlbs` segun `CLocalisation::Metric`, 6 `%.2f %s` con `ST_MILE`, 7 `%.2fm`, 8 `%.2fft` (`* 3.3333333f` en la x87), 9 `%d %s` con `ST_SECS`). Los reales
  pasan al sprintf como double (`fld` + `fstp qword`: un sNaN sale silenciado). Cola: `CText::Get(line)` -> `gGxtString` (`GxtCharStrcpy`), `0x719240` (quita marcas `~`), `AsciiToGxtChar(gString2, gGxtString2)`.
- **UpdateRespectStat(arg0) (0x55BC50..0x55BF1B)**, cdecl, un argumento (solo el byte bajo): con `arg0 != 0` fija 0x8CDEC4 y 0x8CDEC8 a -99.0f. Si no, calcula el respeto total en la x87 a 24
  bits (`0.4*respeto + 1000*misiones/max(1,total)*0.36 + 0.03*novias`, `+ max(0, territorio-0.2)*1250*0.05`, dinero de `Players[PlayerInFocus]` * 1e-7 limitado a 1.0,
  `GetPercentageTagged()*10*0.05`, `0.03*[0xB793DC]`, `0.03*[0xB79488]`; negativo -> 0.0f; el truco `CHEAT_MAX_RESPECT` (0x96917F) lo fija a 1000.0f). Si `ftol(alto mostrado) != ftol(respeto)`:
  guarda 0xB79480, llama a `CheckForStatsMessage` y decide entre las rutas «baja» / «sube» (con `CheckForThreshold(&bajo, respeto)` y `DisplayScriptStatUpdateMessage(estado, 0x40, respeto)`) y actualiza
  los valores mostrados.
- **ConstructStatLine(arg0, arg1) (0x55A780..0x55B6D3)**, cdecl, 2 argumentos (arg1: solo el byte bajo): elige la tabla de lineas por `arg1` (0..6; mayores usan 0x8CDE08; entradas de 6 bytes hasta `-99`),
  cuenta las lineas visibles y compone la `arg0`-esima en `gString`, `gString2`, `gGxtString` y `gGxtString2` (devuelve 0), o devuelve el numero de lineas. Una entrada se muestra si el valor
  de la estadistica es > 0 o su indicador «siempre» es distinto de cero; con el juego aleman se omiten ciertas estadisticas (tabla 0x55B6F8). Formatos 1..10 de la estadistica (las tablas reales solo
  usan 0, 3, 4, 5, 7, 9 y 10; el resto lo ejercitan las tablas sinteticas) y 13 manejadores especiales (0, 143, 144, 145, 164, 169, 173, 174, 175, 213, 231, 241, 243), 252..257 y 320..337.
  Llama a `BuildStatLine` (horas:minutos). Ningun llamador del EXE lee ECX/EDX (ver «Registros»).

## Fuente (hunks; `source.patch`)

`Stats.cpp`: los cuatro cuerpos (sustituyen a `plugin::Call*`), cuatro `RH_ScopedInstall` y los `#include "GangWars.h"` / `"TagManager.h"`. `X87Math.cpp/.h` (al FINAL, bloque `Stw60`):
`Stw60RespectPartialCpp`, `Stw60RespectFinishCpp` (aritmetica de `UpdateRespectStat` en dos mitades, como el EXE guarda dos float en la pila antes de llamar a `GetPercentageTagged`), `Stw60ScaledFtolCpp`
(`ftol(fild(v) * k)`), `Stw60MulCpp` (`ST0 * k` como double), `Stw60SubClampCpp` (`max(0, a - b)` como double). Los literales float son exactamente las constantes del EXE (`static_assert` con `std::bit_cast`);
se evita `std::bit_cast` dentro de las funciones porque con `float_control(precise)` no se inserta en linea. Decisiones de fidelidad:
- `sprintf` del EXE por direccion (0x821BB5), igual que `sscanf` (0x8220AD) en este archivo; `GxtCharToAscii` (0x69F7E0) y 0x719240 tambien por direccion. El resto de colaboradores en C++ (`CText::Get`,
  `GxtCharStrcpy`, `AsciiToGxtChar`, `CStats::*`, `FindPlayerPed`, `CPed::GetWeaponSkill`, `CAudioEngine::GetRadioStationNameKey`, `CTagManager::GetPercentageTagged`...).
- **Enteros exactos**: el EXE pasa a la x87 el entero de una estadistica entera con `fild` (exacto); `CStats::GetStatValue` de la DLL lo devuelve como `float` (redondea por encima de 2^24). `ConstructStatLine`
  llama a `GetStatValue` (el EXE lo hace y el banco traza la llamada) pero toma el entero de la tabla (`StatLineValue`), asi `%d`, `%.2f`, `$%.2f`... imprimen lo mismo que el EXE. Las fixtures incluyen enteros > 2^24.
- Para `BuildStatLine` los reales se pasan al sprintf como `double` (cvtss2sd = `fld`+`fstp qword`, incluido el silenciado de sNaN), y los dos productos (`* 0.4536f`, `* 3.3333333f`) van en la x87 (`Stw60MulCpp`).
- `ConstructStatLine`: orden exacto de efectos: la clave `STATnnn` se forma antes de la comprobacion del juego aleman; `m_ThisStatIsABarChart` se pone a 0 en cada entrada procesada y al id en el formato 10
  (antes de comparar la linea); `Metric` se consulta antes de comparar la linea en el formato 7; las entradas de dos lineas (143, 323, 326, 327, 328, 329, 330, 331..333) cuentan su cabecera con `++count`.

## Revision contra el EXE (fase 2) y discrepancias

Se desensamblo cada funcion completa (`dis.sh`, tablas de saltos 0x55B6D4, 0x55B6F0/0x55B6F8, 0x55B724, 0x55B74C/0x55B784, 0x55B878, 0x55B898, 0x55B8E0, 0x559510, y los datos de las tablas de lineas 0x8CD8A0..0x8CDE56
leidos del EXE) y se contrasto de tres maneras: (1) relectura linea a linea; (2) arnes offline: **ejecucion del codigo del EXE contra la funcion C++** con los mismos falsos, ESP/EBX/ESI/EDI/EBP/DF/x87
comprobados; (3) el oraculo Python (otra transcripcion del desensamblado, con modelo x87 de 24 bits). Resultado: 0 diferencias EXE/C++ y 0 diferencias EXE/oraculo en los 24576 casos del banco y en los 240000 de la ampliacion offline. Hallazgos y correcciones:
1. Los borradores sin commit del arbol principal (`git diff HEAD -- Stats.cpp`) NO se usaron: `BuildStatLine` formateaba con `std::sprintf` de la DLL y calculaba kg / pies en double de 53 bits; `UpdateRespectStat`
   evaluaba toda la cadena en double de SSE (sin los redondeos a 24 bits ni los dos float guardados en pila); `ConstructStatLine` seguia como `plugin::CallAndReturn`. Las fixtures detectan ese tipo de defecto
   (mutaciones del oraculo): sin redondeo a 24 bits el modelo discrepa del EXE en 1240 de 6144 casos de `UpdateRespectStat`, y con el ultimo bit de `0.4536f` cambiado, en 248 de 6144 de `BuildStatLine`.
2. Mi primera transcripcion de `ConstructStatLine` leia `[0xB79060]` con el indice de estadistica equivocado (manejador 144); detectado en la relectura antes de ejecutar y corregido (lee directamente 0xB79060).
3. Error del propio banco, hallado por el arnes: el falso de `GetRadioStationNameKey` escribia 11 bytes en un buffer local de 8 del EXE (pisaba la direccion de retorno de `ConstructStatLine`); ahora escribe
   «RK» + 5 digitos + NUL.
4. `GetStatValue` de la DLL redondea los enteros > 2^24 a float; `ConstructStatLine` no depende de ello (ver arriba). Esto es una diferencia ya existente del hook `GetStatValue` (-OG) con el EXE para cualquier
   otro llamador del juego; no se toca aqui.
5. Sin discrepancias entre C++ y EXE en el resto (tablas de saltos, cuentas de lineas, orden de colaboradores, x87).
6. Ajustes del propio banco hallados con la ampliacion offline (no del codigo del juego): la arena del jugador falso era demasiado corta para la ranura +127 (el EXE direcciona hasta +5000 bytes del jugador;
   `kPedAt` pasa de 4096 a 3072 con `static_assert`); la traza admitia 160 eventos y las 86 entradas de la tabla 6 dan mas (ahora 512; `run_local.py` rechaza `trace_dropped`); el oraculo modelaba mal el
   desempate del x87 entre dos NaN de igual significando (gana el de signo positivo; los 12 casos de la ampliacion lo mostraron; con las fixtures del banco ambas reglas dan el mismo resultado).

## Que se compara (por lado, en bits; original == candidato == oraculo)

- **Salida de cada funcion**: FindLeast: EAX (el indice). BuildStatLine: los cuatro buffers de texto (`gString2`, `gString`, `gGxtString`, `gGxtString2`: hash FNV-64 del buffer entero y los primeros 64
  bytes), la memoria de argumentos (linea, valores) intacta. UpdateRespectStat: 0xB79480 (respeto total), 0x8CDEC4 y 0x8CDEC8. ConstructStatLine: EAX, `m_ThisStatIsABarChart` (0xB794CC), los cuatro buffers
  y la arena del jugador falso intacta.
- **Traza de colaboradores** (idéntica en ambos lados y al oraculo): tipo + argumentos de cada llamada a los falsos (23 kinds: `Get`, `GxtCharToAscii`, `GxtCharStrcpy`, 0x719240, `AsciiToGxtChar`, sprintf (con la
  direccion de destino como etiqueta, el hash del formato y los argumentos: enteros, hash de cadenas y bits de los double), `GetStatValue`, `Metric`, `GermanGame`, `BuildStatLine`, `ConvertToMins/Secs`,
  `GetPercentageProgress`, `Populate`, `FindMost/Least`, `GetRadioStationNameKey`, `FindPlayerPed`, `GetWeaponSkill`, `GetPercentageTagged`, `CheckForThreshold`, `DisplayScriptStatUpdateMessage`, `CheckForStatsMessage`).
- **Nada fuera de lo permitido**: la memoria de doce regiones del juego (`Fakes.hpp`: buffers de texto, region de estadisticas 0xB78E20..0xB794E0, temporizador, `Players[2]`, bandas/territorio, truco, valores
  mostrados, tablas de lineas...) solo cambia donde la funcion puede escribir; y se restaura entera tras cada caso (`globals_restored`).
- ABI: ESP (`popBytes` = 0 / 20 / 4 / 8), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW completa, TOP, tags y ST validos: ninguno debe quedar). FCW fijada a 24 bits, redondeo al mas cercano y
  todas las excepciones enmascaradas (estado del juego).
- Rutas (`route_ok`): original -> el hook de la funcion NO salta a la DLL; candidato -> si, con el cuerpo en el ASI; **en ambos lados** 0x69F7E0, 0x719240 y 0x821BB5 no estan enganchados. Se vigilan 16 bytes de las
  25 entradas (las cuatro funciones, esas tres y 18 colaboradores parcheados) y deben seguir siendo el codigo original al volver.

## Registros (EAX/ECX/EDX de salida)

`python reversing/Dependencies25A/residue_scan.py <dir>` (y desensamblado a mano de los llamadores): **FindLeast** un llamador (0x55B3A3, dentro de `ConstructStatLine`) que lee solo EAX; **BuildStatLine** cuatro
llamadores (todos en `ConstructStatLine`), ninguno lee registros; **UpdateRespectStat** 0x442ADA y 0x573383, ninguno lee registros; **ConstructStatLine** 0x57494E y 0x57DFDA leen solo EAX (los avisos
`CALL@...:unwritten=ecx,edx` son llamadas a `0x719490`, `0x823A30`, `_ftol2` y `0x588BB0`, que no leen ECX/EDX de entrada; revisado a mano). Por tanto EAX se compara solo donde es el valor de retorno (`ret`) y
ECX/EDX/EAX de las demas solo se registran (`original_residue`, `candidate_residue`; `kCompareVolatileRegisters=false`); **el revisor debe confirmarlo**.

## Colaboradores

**Sustituidos** (mismo falso en ambos lados, parche de entrada en la direccion del EXE **y** en el cuerpo de la DLL, resuelto antes de parchear; activos solo durante la llamada), 23 entradas:
`CStats::GetStatValue` 0x558E40 (falso naked: `fld` del float o `fild` del entero exacto, como el original, mas traza), `CLocalisation::Metric` 0x56D220 / `GermanGame` 0x56D200,
`PopulateFavoriteRadioStationList` 0x558EC0, `FindMostFavoriteRadioStation` 0x558FA0, `FindLeastFavoriteRadioStation` 0x559010 (como colaborador de `ConstructStatLine`), `GetPercentageProgress` 0x5591E0,
`ConvertToMins/Secs` 0x559540/0x559560, `BuildStatLine` 0x559230 (colaborador de `ConstructStatLine`), `CText::Get` 0x6A0050, `GxtCharToAscii` 0x69F7E0 (solo EXE), `GxtCharStrcpy` 0x718660, 0x719240 (solo EXE),
`AsciiToGxtChar` 0x718600, sprintf del EXE 0x821BB5 (solo EXE), `GetRadioStationNameKey` 0x507010, `FindPlayerPed` 0x56E210, `CPed::GetWeaponSkill` 0x5E3B60, `GetPercentageTagged` 0x49CDA0,
`CheckForThreshold` 0x5595F0, `DisplayScriptStatUpdateMessage` 0x55B980 y `CheckForStatsMessage` 0x559760. Consecuencia: **no se valida aqui** el cuerpo real de ninguno de ellos ni el formato real del sprintf del
EXE; solo que cada funcion los llama igual (mismos argumentos, mismo orden) y trata igual sus resultados. Los falsos de texto escriben una representacion fija (`G[clave]`, `A[texto]`, `%d` decimal, `%.2f`
como `{16 digitos hexadecimales de los bits del double}`) que reproduce el oraculo. `collaborators_stubbed=true` en `begin`.
**Reales**: nada mas (las cuatro funciones no llaman a otras cosas; `_ftol2` 0x821B40 corre de verdad en el lado EXE y el equivalente del compilador en el lado C++).
Estado de los hooks de los colaboradores: los parches se escriben sobre el cuerpo ya resuelto (con el hook de la ruta de cada funcion apagado, la entrada del cuerpo de la DLL es un salto al EXE; el parche lo pisa y
lo restaura). `BuildStatLine` y `FindLeastFavoriteRadioStation` son a la vez funciones bajo prueba y colaboradores de `ConstructStatLine`: en esa ruta se parchean sus dos entradas y ninguno de los dos hooks se enciende.

## Fixtures (`Fixtures.hpp`, deterministas): 4096 / 6144 / 6144 / 8192 casos, 16 / 24 / 16 / 16 familias

- **FindLeast (4096)**: aleatorio, pequenos, todos iguales, crecientes, decrecientes, minimo unico en 1..12, empates (gana el primero), minimo solo en las posiciones 0 y 13 (se ignoran), extremos INT_MIN/INT_MAX,
  casi todo igual, ceros, todos negativos, caidas.
- **BuildStatLine (6144 = 24 grupos x 256)**: `type == 1` (segundo valor < 10, >= 10, negativos, metricas cualesquiera), dos valores con metricas 0/1/3 y otras, un valor con metricas 0..9, metricas fuera de
  rango (negativas, >= 10), punteros nulos, linea nula, `type` parecido a 1; valores float de un conjunto de 48 notables (ceros con signo, denormales, FLT_MAX, infinitos, qNaN y **sNaN** con carga,
  enteros grandes) y enteros notables; claves de 0..100 caracteres con `~`, espacios y vacias; `Metric` verdadero y falso.
- **UpdateRespectStat (6144)**: respeto objetivo (de 0 a 1e6) con las estadisticas que lo producen, con `GetPercentageTagged` negativo/enorme, dinero hasta 1e8 (limitado a 1.0), misiones/total (incluido total < 1),
  territorio < 0.2, truco, suma negativa (0.0f), NaN/inf en cada campo; valores mostrados (bajo/alto) cerca del respeto, -99, NaN, 2.0 (frontera), con `CheckForThreshold` verdadero/falso, `arg0 != 0`.
- **ConstructStatLine (8192)**: las 7 tablas reales y la 0x8CDE08 real (53 % de los casos), y tablas **sinteticas** de 1..12 entradas en 0x8CDE08 (47 %) con formatos 0..255, indicadores «siempre» y «especial»
  cualesquiera e identificadores de todos los manejadores (0..337) para ejercitar los formatos 1, 2, 6 y 8 que las tablas reales no usan; estadisticas con probabilidad variable de ser > 0, enteros de 0 a 2^31
  (incluidos > 2^24), floats notables, juego aleman, metrica, jugador falso (ranura -128..127, tipo de arma 0x15..0x21 con la palabra alta valida/invalida, blindaje), niveles de habilidad (EAX con basura
  en los bytes altos), horas de vuelo/jetpack en todos los tramos, bandas, emisoras, `arg0` -3..110, `arg1` con basura en los bytes altos.
- Seguridad: el EXE desreferencia sin comprobar, asi que las fixtures cumplen sus reglas (punteros validos cuando se leen, ids < 344, ranura de jugador dentro de la arena de 8 KB); `fixture_oracle.py` rechaza
  como fixture invalido (`Fail`) cualquiera que no las cumpla (ninguno en el volcado).
- **Cobertura**: `run_local.py` exige minimos por funcion calibrados con el conteo exacto del oraculo sobre el volcado del generador C++ (60 % de lo observado; las fixtures son deterministas, no hay margen de
  RNG). Tramos de `ST_PR04, 06, 08, 10..15, 17, 18` de la escalera de piloto (`ConstructStatLine`, 330) son **inalcanzables** (exigen minutos negativos con horas > 0): el codigo esta transcrito, no ejercitable.

## Suposiciones y riesgos

- **No se compilo el proyecto ni se ejecuto el banco en el juego.** Comprobado con `cl /Zs` (solo sintaxis, con los includes y definiciones reales de `gta_reversed.vcxproj` y `/Zc:preprocessor`) que
  `Stats.cpp`, `X87Math.cpp` y `LiveComparison.hpp` (con `Core.hpp`/`Cases.hpp`/`Fakes.hpp`/`Gate.hpp`) no dan errores; y compilado y ejecutado el arnes offline (x86, MSVC). Lo que solo puede mostrar la sonda:
  que los hooks RH se registran con el nombre esperado (`CStats` / `FindLeastFavoriteRadioStation`...), que no hay conflictos de parche con otros hooks, y que el comportamiento coincide en el proceso real.
- Los parches de entrada exigen >= 5 bytes por funcion y que el cuerpo de la DLL no este en linea: se asume que las funciones colaboradoras no se insertan en linea (Stats.cpp se compila `/Zi` con las opciones por
  defecto del proyecto, sin `/O2`); si una llamada se insertara, faltarian eventos de la traza y el caso fallaria (no habria falso positivo).
- `FakeGetStat` (inline asm, naked) y `Gate` usan `__asm`: solo en el banco.
- `CPed::GetWeaponSkill(eWeaponType)`, `CText::Get`, `FindPlayerPed` y `GetRadioStationNameKey` se resuelven con conversiones de puntero (`static_cast<...>(&...)` + `std::bit_cast`) en `LiveComparison.hpp`; si algun
  nombre sobrecargado cambia, fallara la compilacion de la sonda, no el resultado.
- El EXE canonico es el de `D:\Grand Theft Auto San Andreas\gta-sa.exe` (SHA256 `72ae59e4...ecc9b`); el arnes offline lo carga tal cual en 0x400000 (sin relocaciones) y sustituye todo lo que podria llamar
  a la API de Windows.
- `StatTypesFloat`/enteros fuera de las regiones modeladas (ids >= 344) no se ejercitan (el EXE lee memoria arbitraria alli).
- En la sonda, `Players[PlayerInFocus]` se limita a 0/1 (el EXE indexa sin comprobar); 0xB7CD98 se sustituye por el jugador falso y se restaura.

## Archivos

`Fixtures.hpp` (generadores), `Fakes.hpp` (falsos, traza, regiones de memoria), `Cases.hpp` (entrada/preparacion/observacion de cada funcion), `Gate.hpp` (ABI y x87), `Core.hpp` (rutas, parches, ejecucion),
`LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `README.md`; `offline/` (`host.cpp`, `harness.cpp`, `x87.cpp`, `build_offline.bat`, `run_build.sh`,
`gen_port.py`, `check_rows.py`, `calibrate.py`, `make_fake_log.py`).
