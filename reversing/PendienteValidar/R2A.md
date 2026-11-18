<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11707 con evidencia histórica y 734 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote R2A: reconstrucción sin validar

Rama `reconstruccion-sin-validar`. Nueve direcciones del encargo, de RenderWare 3.7 enlazado en el EXE (no son del juego): módulo RtAnim (herramientas de animación), un ayudante de RpUVAnim y RtSlerp (interpolación esférica de cuaterniones). **No hay banco**: no se ha ejecutado el juego ni se ha tocado `approvals.json`, `validation-registry` ni `gta_lock`. En `docs/hooks.csv` entran con `validated=0`. Cada cuerpo se revisó contra el desensamblado (`dumpbin /disasm` del EXE) instrucción por instrucción.

Ninguna de las nueve estaba enganchada en la rama (`git grep` de las direcciones y filas de `docs/hooks.csv`). Siete eran envolturas por dirección fija (`0x7CCCA0`, `0x7CCD40`, `0x7CCE40`, `0x7CCF10`, `0x7CD520`, `0x7CD8D0` en `rtanim.cpp` y `0x7EC220` en `rtslerp.cpp`) y se sustituyeron por el cuerpo en C++; `0x7CCC50` no tenía envoltura y se creó.

## Funciones

| Dirección | Nombre | Bytes | Tipo | Archivo | Riesgos que debe mirar el banco |
| --- | --- | ---: | :---: | --- | --- |
| `0x007CCC50` | `RpUVAnimLinearKeyFrameDataFromMatrix` (clase **RpUVAnim**, no RtAnim) | 43 | S | `source/game_sa/RenderWare/rw/rpuvanim.cpp` (decl. en `rpuvanim.h`) | Copia seis dwords de un `RwMatrix` (+0x00, +0x04, +0x10, +0x14, +0x30, +0x34) a `uv[0..5]` y devuelve el primer argumento (EAX). Clase corregida: la llama `0x7CC810` (RpUVAnim) y está entre funciones RpUVAnim y RtAnim; el listado la daba por RtAnim con confianza baja. Copia por dwords (no pasa por la x87): comprobar con NaN/denormales. |
| `0x007CCCA0` | `RtAnimInitialize` | 35 | S | `rtanim.cpp` | `RwEngineRegisterPlugin(0, 0x1B7, 0x7CCCD0, 0x7CCD10)` y devuelve `(resultado > 0)` con signo. Constructor y destructor del plugin por dirección fija (no reconstruidos). Comparar los cuatro argumentos y el valor devuelto con resultados 0, 1 y negativo. |
| `0x007CCD40` | `RtAnimRegisterInterpolationScheme` | 160 | S | `rtanim.cpp` | Tabla global en `0xC9B920` (16 entradas de 0x30) y contador en `0xC97900`. Orden: primero límite (`>= 16`: `RWERROR(1)`), luego búsqueda del `typeID` (duplicado: `RWERROR(0)`), después copia de 0x30 bytes y contador + 1. Los errores se emiten con `RwError{pluginID = 0x1B7, errorCode = _rwerror(código)}` + `RwErrorSet` (0x808820). Banco: tabla vacía, con duplicado, llena (16). |
| `0x007CCE40` | `RtAnimAnimationCreate` | 202 | S | `rtanim.cpp` | Búsqueda del `typeID` en la tabla (`RWERROR(2)` y NULL si no está). `RwMalloc(animKeyFrameSize*numFrames + customDataSize + 0x18, 0x3001B)` por el puntero del motor (+0x134); NO comprueba el resultado ni inicializa los fotogramas. `duration` se copia como dword en el EXE (en C++ es una copia de float: comparar bits con NaN). `customData` = tras los fotogramas si `customDataSize > 0` (con signo), si no NULL. |
| `0x007CCF10` | `RtAnimAnimationDestroy` | 26 | S | `rtanim.cpp` | `RwFree` del motor (+0x138) y TRUE. Trivial. |
| `0x007CCF6C` | **saltada** | 490 | — | — | Ver "Saltadas". |
| `0x007CD520` | `RtAnimInterpolatorCreate` | 108 | S | `rtanim.cpp` | `RwMalloc(numNodes*maxInterpKeyFrameSize + 0x4C, 0x3001B)` sin comprobar. Orden de los argumentos: `(numNodes, maxInterpKeyFrameSize)`. Rellena: +0x2C numNodes, +0x24 y +0x20 = maxInterp, +0x28 = -1, +0x14 = -1.0f (0xBF800000), `parentAnimation` (+0x38) = el propio, resto a 0 (incluidos los cuatro callbacks +0x3C..+0x48 y +0x30/+0x34). Comparar el bloque completo de 0x4C bytes (los fotogramas interpolados que siguen no se inicializan). |
| `0x007CD8D0` | `RtAnimInterpolatorAddAnimTime` | 471 | S | `rtanim.cpp` | **x87**: float puro (la x87 a 24 bits redondea cada operación igual que un float); comparaciones ordenadas (NaN las hace falsas) como `fnstsw + test ah,5/41h/100h`. `time <= 0` o NaN: TRUE sin tocar nada. `callbackFired` = `cur < cb && time + cur >= cb`, y se acumula en cada resta de duración (`do/while newTime > duration`). Bucle: `SetCurrentAnim(anim, pCurrentAnim)` (0x7CD5A0, ya en C++ y enganchada) y el tiempo restante se restaura DESPUÉS. Sin `pNextFrame`: reinicia conservando el tiempo. Búsqueda del fotograma interpolado por `keyFrame2 == prevFrame` SIN límite (si no aparece, no termina, como el EXE). Después `keyFrameInterpolateCB(out, kf1, kf2, currentTime, customData)` por nodo (cdecl, float por pila), `pAnimCallBack(anim, data)` (si devuelve 0 se anula) y `pAnimLoopCallBack(anim, data)` si hubo bucle. Siempre devuelve TRUE. Banco: animación con 1 y N nodos, cruzando la duración una y varias veces, con y sin callbacks (que devuelvan 0 y no 0) y con `pNextFrame` nulo. Colaboradores: SetCurrentAnim y los callbacks (comparar la secuencia). |
| `0x007EC220` | `RtQuatSetupSlerpCache` | 949 | S | `source/game_sa/RenderWare/rw/rtslerp.cpp` + ayudante `RtSlerpSetupCacheCpp` en `source/game_sa/X87Math.cpp/.h` | **x87 a 24 bits** con `RtSlerpSetupCacheCpp` (X87Math.cpp, `/O2 /arch:IA32 /fp:fast` con `float_control(precise, on)`): producto escalar en el orden del EXE, arcocoseno por polinomio (como fdlibm e_acosf) con constantes por bits, `RwSqrt` (0x7EDB30) por dirección fija en las ramas `|c| >= 0.5`, ramas por bits (`>= 1`: 0 o pi, `<= 2^-29`: pi/2), `nearlyZeroOm = (c >= 0x3F7FFF58)` y escalado por `1/(w + w^3*poly(w^2))`. El banco debe probar: dot negativo, `< -1`, `> 1`, NaN, 0, valores pequeños, 0.5..1, y cercanos a 1 (flag a 1: no escala). Leer el objeto compilado y comprobar que no cambia el orden ni sale como llamada a la CRT. El EXE usa los huecos de los argumentos de pila como locales (los llamadores 0x616ECA, 0x651E42, 0x6522BD no los releen); la versión C++ no los toca. Solo EAX/ECX/EDX se sobrescriben en el EXE: ningún llamador depende de ellos. |

Las siete funciones de RtAnim van en `RtAnim::InjectHooks` (ya existente); `RpUVAnimLinearKeyFrameDataFromMatrix` en `RpUVAnimInjectHooks` y la de RtSlerp en la función nueva `RtSlerpInjectHooks` (`rtslerp.cpp`, declarada en `RenderWare.h` y llamada desde `InjectHooksMain.cpp`). No es un `namespace RtSlerp` porque `rtslerp.h` ya define `struct RtSlerp`. Clases en `docs/hooks.csv`: `RtAnim`, `RpUVAnim` y `RtSlerp`.

## Saltadas

| Dirección | Motivo |
| --- | --- |
| `0x007CCF6C` | No es una función: es un **fragmento intermedio de `RtAnimAnimationRead` (0x7CCF30)**. `0x7CCF30` abre el fichero (`0x7ECEF0`, EDI = flujo), busca el chunk (`0x7ED2D0`) y cae directamente en `0x7CCF6C`, que empieza con `push ebx; push ebp; push esi` y usa **EDI sin definir** (el flujo) y la pila de `0x7CCF30` (`sub esp,20h`); termina en `0x7CD130..0x7CD144` con `add esp,20h; ret` que equilibra ese `sub`. No tiene prólogo propio ni se puede llamar. Es parte de `RtAnimAnimationRead` (envoltura 0x7CCF30, fuera de este lote). |

## Notas de revisión (sin banco)

- ABI: todas cdecl; las del EXE devuelven en EAX, `0x7EC220` es `void`. Ninguna usa SEH.
- `0x7CCCD0`/`0x7CCD10` (constructor/destructor del plugin) y `0x7CCC80` (parámetros de la lista libre) siguen sin reconstruir y no son de este lote.
- `RaiseRtAnimError` (en `rtanim.cpp`, espacio anónimo) usa `_rwerror` directamente como `rtdict.cpp`; el pluginID `0x1B7` es el literal del EXE.
- `RtAnimInterpolatorSetCurrentAnim` (0x7CD5A0) ya estaba en C++ y enganchada: `AddAnimTime` la llama por su nombre C++.

## Bancos viejos que citan las direcciones

Búsqueda con `git grep` de cada dirección sobre `reversing/` (sin `runs/`, ni `.csv/.txt/.json/.md`). No se repiten.

| Dirección | Bancos que la citan |
| --- | --- |
| `0x7CCCA0` | AppBatch |
| `0x7CCD40` | RpAnim43 |
| `0x7CCE40` | RpAnim43 |
| `0x7CCF10` | RwHelper24 |
| resto (0x7CCC50, 0x7CCF6C, 0x7CD520, 0x7CD8D0, 0x7EC220) | ninguno |

## Compilación

Compilación completa de `gta_reversed.asi` con éxito (build-project.ps1, nivel 1) tras el último cambio. El objeto de `X87Math.cpp` se leyó con `dumpbin /disasm`: la secuencia x87 de `RtSlerpSetupCacheCpp` conserva el orden del EXE (suma del producto escalar `((f1*t1 + f0*t0) + f2*t2) + f3*t3`, polinomios de arcocoseno y de 1/sen, `RwSqrt` por `mov eax,7EDB30h; call eax`) y no contiene llamadas a la CRT ni a `std::bit_cast`.
