<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarLink74: CCarCtrl::ClipTargetOrientationToLink (0x00422760)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-2`, HEAD f13a750cf + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, aritmetica exacta). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carlink74\20261027-030041-956731`.

`source.patch`: CarCtrl.cpp, CarCtrl.h (renombre `ClitargetOrientationToLink` -> `ClipTargetOrientationToLink`; sin otros llamadores en source/), X87Math.cpp y X87Math.h (bloque «Lote 74», `CarLink74ClipCpp`).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| ClipTargetOrientationToLink | 0x00422760 | CCarCtrl,ClipTargetOrientationToLink | S |

No es virtual. cdecl, `ret` (CVehicle*, CCarPathLinkAddress, int8, float*, float, float), sin valor de retorno. Cuatro llamadores en el EXE (0x435EDB, 0x435F6E, 0x4360E3, 0x4361F6): empujan el enlace como `movzx cx,word` y el int8 como `mov al,byte` (bytes altos a cero), dejan la pila x87 vacia en la llamada y no leen EAX/ECX/EDX despues (los reconstruyen con xor/mov); EAX/ECX/EDX solo se registran. Ningun llamador en source/.

## Revision contra el EXE (0x422760..0x422B18)
- Area `link >> 10`: si `ThePaths.m_pPathNodes[area]` es nulo sale sin hacer nada; si no, el enlace es `m_pNaviNodes[area] + (link & 0x3FF) * 14` (sin mas comprobaciones).
- Direccion del enlace (int8 en +8 y +9) * 0.01f * arg3 (int8 con signo; solo el byte bajo), cada producto guardado como float (A = x, B = y).
- Primera llamada a GetATanOfXY(arg5 - pos.x, arg6 - pos.y) con el resultado DESCARTADO (`fstp st(0)`): hay que hacerla igualmente (el falso la registra). Pos = matriz+0x30 o, sin matriz, this+4, releida en cada uso.
- Carriles (byte +0xB: bits 0-2 = a, bits 3-5 = b, ancho en +0xA): a == 0 -> M = S = b * 0.5; b == 0 -> M = S = a * 0.5; si no, arg3 == 0: M = ancho * 0.011574074f + a, S = b; arg3 != 0: M = ancho * 0.011574074f + b, S = a. Q = M - 0.3f (en registro), P = float(S - 0.3f).
- Puntos: X1 = float(arg5 + (B*Q)*5.4f), Y1 = arg6 - (Q*A)*5.4f (sin guardar), X2 = float(arg5 - (P*B)*5.4f), Y2 = float((P*A)*5.4f + arg6). Segunda llamada (x = X1 - pos.x, y = Y1 - pos.y; Y1 sin guardar antes de restar), resultado guardado como float (a1); tercera llamada (X2, Y2), resultado en ST0 SIN guardar (a2; con la atan real es el fpatan sin redondear a float).
- D1 = float(a1 - *arg4) (se guarda y se recarga), D2 = a2 - *arg4 (registro). Ambos a [-pi, pi] con bucles `> pi: -= 2pi` y `< -pi: += 2pi` (D1 se guarda de nuevo como float si hubo vueltas). Si D1 < 0 y D2 < 0: *arg4 = float(max(D1, D2) + *arg4) (D1 si D1 > D2, si no D2); si D1 > 0 y D2 > 0: min (D1 si D1 < D2, si no D2); con signos mezclados, ceros o NaN no se escribe. Con NaN nunca se escribe un NaN.
- Siempre, al final: *arg4 < 0 -> `+= 2pi` hasta >= 0; *arg4 > 2pi -> `-= 2pi` hasta <= 2pi (lee *arg4 de memoria en cada comparacion).
- Constantes: 0x858C58 = 0.01f, 0x858B8C = 0.5f, 0x858EEC = 0.011574074f (0x3C3DA12F), 0x858C24 = 0.3f, 0x858C50 = 5.4f, 0x858CB8/BC/C0 = pi, 2pi, -pi.
- Nombre: se usa `ClipTargetOrientationToLink` (el de la DLL era `Clitarget...`); sin otros llamadores en source/, asi que no rompe nada. Las tablas generadas de docs/ aun llevan el nombre antiguo hasta regenerarlas.
- Con |D| enorme o infinito los bucles del EXE no terminan: las fixtures mantienen |*arg4| y los retornos del atan por debajo de 1000 (los infinitos van solo en posiciones y argumentos).

## Implementacion
`CCarCtrl::ClipTargetOrientationToLink` (CarCtrl.cpp) comprueba el area y localiza el enlace; el cuerpo x87 esta en `CarLink74ClipCpp` (X87Math.cpp, /arch:IA32, `float_control(precise, on)`): los guardados del EXE van en `volatile float`, lo que el EXE deja en registro va en `double` (con la palabra de control a 24 bits cada operacion se redondea a 24 bits y el exponente cabe en un double), y las comparaciones son ordenadas (NaN no escribe). Las tres llamadas son a `CGeneral::GetATanOfXY`. Sin `__asm` ni naked.

## Defectos hallados por el banco
Ninguno de produccion: la reconstruccion paso a la primera (solo una advertencia C4244 de compilacion, double a float, corregida con un cast explicito). Ningun defecto del banco; no se relajo ninguna compuerta ni cobertura.

## Banco
Fixtures: 18432 casos, 18 familias de 1024 (`n / 1024`): 0 area de nodos nula (sin llamadas); 1 normal; 2 D1 y D2 negativos (incluye empate); 3 D1 y D2 positivos (con empate); 4 signos mezclados, ceros exactos y con signo; 5 vueltas de normalizacion de D1/D2 y fronteras ±pi (±1 ulp); 6 *arg4 final fuera de [0, 2pi]; 7 los 256 valores del byte de carriles x 4 variantes de arg3 (0, 1, -1, aleatorio); 8 posiciones y argumentos especiales (NaN de señal y silenciosos no canonicos, infinitos, desnormales, ±0, magnitudes extremas); 9 magnitudes diminutas; 10 retornos del atan especiales (NaN no canonicos, desnormales, ±0, pi, 2pi); 11 *arg4 inicial especial (NaN, ±0, desnormal, pi, 2pi±ulp, ±100, 1000); 12 D diminutos (±ulp, desnormales); 13 sin matriz (posicion compacta this+4); 14 atan REAL moderado; 15 atan REAL con ceros con signo, destino coincidente y desplazamientos minimos; 16 extremos de arg3 y de la direccion (-128, 127, 0) con basura en los bytes altos; 17 mezcla.
Arenas propias: vehiculo (+4 y +0x14), matriz (+0x30), destino de arg4, enlace (14 bytes) y las entradas ThePaths.m_pPathNodes[area]/m_pNaviNodes[area] (regiones restauradas tras cada lado). El dword de la direccion del enlace lleva basura en los 16 bits altos y arg3 en los 24 bits altos (el EXE solo lee 16 y 8 bits).
Colaborador y quien lo valida:
- CGeneral::GetATanOfXY 0x53CC70 (con hook RH en la DLL, apagado durante el banco): falso identico en la direccion del EXE y en el cuerpo de la DLL que registra (llamada, x, y) en orden y devuelve el valor del caso en ST0; lo valida Core9 (1024 pares). En las familias 14 y 15 (2048 casos) NO se sustituye: se ejecuta el atan real en cada lado (EXE original y DLL con hook apagado) y el oraculo comprueba el rango del resultado, lo que ademas ejercita el fpatan sin redondear de la tercera llamada.
Se comparan eventos, bytes cambiados de las 4 arenas, resultado de *arg4, compuertas de ABI (pila, no volatiles, DF, SEH), ruta (el EXE salta a la DLL solo en el candidato), estado x87 y restauracion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json` (incluye NaN dobles en x o y, ramas neg/pos/ninguna, bucles de D1/D2 y del final, desnormales e infinitos en argumentos de llamada).

Prueba negativa (produccion mutada y restaurada despues): en `CarLink74ClipCpp`, `d1 < d2` por `d1 > d2` en la rama de D positivos: 3487 diferencias de RESULTADO (corrida `...\resultados-carlink74\20261027-025734-318493`), sin caida; restaurado y repetida la corrida aprobada (la de arriba).

No validado: la validez de GetATanOfXY (la prueba Core9; aqui solo sus argumentos y su retorno en ST0), los bucles con |D| enorme o infinito (en el EXE no terminan), el valor de EAX/ECX/EDX de salida (nadie los lee) y los NaN devueltos por el atan REAL (los NaN solo se probaron con el falso).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarLink74`; `python -I fixture_oracle.py <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).
