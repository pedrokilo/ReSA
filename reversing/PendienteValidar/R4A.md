<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11695 con evidencia histórica y 746 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote R4A: reconstruccion sin validar

Rama `reconstruccion-sin-validar`, worktree `wv-7`. Veintitres direcciones del encargo, atribuidas por el listado a RpHAnim. **No hay banco**: no se ha ejecutado el juego ni se ha tocado `approvals.json`, `validation-registry` ni `gta_lock`. En `docs/hooks.csv` entran con `validated=0`. Cada cuerpo se transcribio del desensamblado (`dumpbin /disasm` del EXE) instruccion por instruccion.

**Hallazgo de clase.** Solo seis direcciones son de RpHAnim (`0x7C4600`, `0x7C4C30`, `0x7C5120`, `0x7C51A0`, `0x7C51D0`, `0x7C64C0`). Las otras diecisiete (`0x7C1D80`..`0x7C4220`) **no son de RpHAnim**: son las rutinas matematicas con variantes 3DNow!/MMX y SSE de **D3DX9** enlazado estaticamente (conversion de medios flotantes, `D3DXVec2Transform*`, acos, seno/coseno, atan2); sus llamadores estan en `0x7B4FF3..0x7BEE44`, que es el compilador de shaders de D3DX9, no codigo de RenderWare. La clase en `hooks.csv` es `D3DX9` (archivo nuevo `source/game_sa/RenderWare/rw/d3dx9math.cpp/.h`, `D3DX9InjectHooks` declarada en `RenderWare.h` y llamada desde `InjectHooksMain.cpp`).

Ninguna de las direcciones estaba enganchada en la rama (`git grep` y `docs/hooks.csv`). Las seis de RpHAnim eran envolturas por direccion fija en `rphanim.cpp` (`0x7C4600`, `0x7C4C30`, `0x7C5120`, `0x7C51A0`, `0x7C51D0`, `0x7C64C0`) y se sustituyeron por el cuerpo en C++. `0x7C5160` (`RpHAnimFrameGetHierarchy`) ya estaba enganchada y validada; no se toca.

Todas las direcciones del encargo son inicios de funcion completos (terminan en `ret`/`ret N`); las que no estan alineadas a 16 vienen tras el `ret` de la funcion anterior (sin prologo de pila, sin relleno de `int 3`). Ninguna es un fragmento. Las de D3DX9 son `__stdcall` (cierran con `ret 8`/`ret 0Ch`), como la API publica.

## Funciones

| Direccion | Nombre | Bytes | Tipo | Archivo | Riesgos que debe mirar el banco |
| --- | --- | ---: | :---: | --- | --- |
| `0x007C1D80` | `D3DXFloat16To32Array_3DNow` | 298 | S | `source/game_sa/RenderWare/rw/d3dx9math.cpp` | `__stdcall` (out, in, count), devuelve `out`. MMX/3DNow!: **no se puede ejecutar en CPU sin 3DNow!** (`femms` da #UD en Intel y en AMD desde Zen). Aritmetica entera por lane + una `pfsub` exacta (restas de dos normales cercanos), reproducida bit a bit. Con count < 4 (signo) llama al escalar `0x76626B` (stdcall; con 0 no llama). Con sobrante 1..3 reconvierte los ultimos 4 (solape). Infinitos/NaN salen como 65536.0f con mantisa desplazada (igual que el EXE). Comparar el buffer completo para count 0..9, medios flotantes normales, denormales, +-0 y exponente 31. |
| `0x007C203B` | `D3DXVec2Transform_3DNow` | 85 | S | `d3dx9math.cpp` | 3DNow! (pfmul/pfadd por pares). Orden por componente: `((x*M[c]) + M[12+c]) + (y*M[4+c])`. **Redondeo**: el 3DNow! no es IEEE (trunca / sin denormales segun el modelo de CPU); aqui son operaciones IEEE de 32 bits. No ejecutable en CPU modernas. Devuelve `out`; lee x,y antes de escribir (out puede solapar v). |
| `0x007C2090` | `D3DXVec2TransformNormal_3DNow` | 57 | S | `d3dx9math.cpp` | Igual que `0x7C203B`, solo dos componentes: `(x*M[c]) + (y*M[4+c])`, c = 0..1. Devuelve `out`. |
| `0x007C2AAE` | `D3DX9AcosPS` | 502 | S | `d3dx9math.cpp` | Acos de 4 floats alineados a 16 (movaps: un puntero desalineado falla igual que el EXE). Intrinsecos SSE sin `__asm`. Tres caminos segun `movmskps(|x| <= 0.5)`: 0xF (pi/2 - x*P), 0 (2 s P | signo + (x<0 ? pi : 0)) y mezcla (lane a lane, con sqrt de todas las lanes). Orden de las operaciones y operandos de cada `addps`/`mulps` igual que el EXE; el unico efecto no reproducible es que el compilador conmute operandos de una suma (afecta solo a la carga util de un NaN con dos operandos NaN). Probar x en [-1,1], +-0.5, +-1, fuera de rango, NaN, mezclas de lanes. |
| `0x007C2CA4` | `D3DX9TwoAcosScalar` | 241 | S | `d3dx9math.cpp` | `2*acos(value)` escalar (valor por pila, `movss`+`shufps 0`, escribe un solo float con `movss`). Mismo polinomio que `0x7C2AAE` con coeficientes en pares (`0x882940/50/60`) y `addps x,x` final. Conserva ebx. Probar |x| = 0.5 exacto (camino pequeno por `cmple`), 0, 1, -1, NaN. |
| `0x007C307E` | `D3DX9SinCosPS` | 489 | S | `d3dx9math.cpp` | Seno/coseno de 4 floats alineados (`in`, `outSin`, `outCos`), devuelve `outSin` en EAX. Reduccion a cuadrantes con el truco 2^23 (`0x4B000000`), pi/2 en cuatro piezas restadas en orden, q & 0x7FFFFF, bits de signo por (q -/+ (q&1)) & 2 << 30, polinomios degrado 7/8. Comparten cuerpo `0x7C307E` y `0x7C381F` (mismas constantes y operaciones; el EXE usa memoria y enteros escalares en una y SSE2 en la otra). Probar |x| grande (> 2^22), 0, +-pi/2 multiplos, NaN/Inf, x negativo. EDX sale con basura distinta de la del EXE (que dejaba `outCos`). |
| `0x007C381F` | `D3DX9SinCosPS_SSE2` | 326 | S | `d3dx9math.cpp` | Igual que `0x7C307E` (ver arriba). |
| `0x007C4600` | `RpHAnimPluginAttach` | 111 | S | `source/game_sa/RenderWare/rw/rphanim.cpp` | Sustituye la envoltura. `RwEngineRegisterPlugin(0, 0x11E, 0x7C4670, 0x7C4720)` (si < 0: FALSE sin seguir); `RwFrameRegisterPlugin(8, 0x11E, 0x7C4750, 0x7C4770, 0x7C4830)` guardado en `0xC9B8C8` aunque sea negativo; `RwFrameRegisterPluginStream(0x11E, 0x7C4A00, 0x7C48D0, 0x7C4BF0)` (si < 0: FALSE); despues `0xC9B8C8 < 0` -> FALSE; si no TRUE. Rutinas del plugin por direccion fija (no reconstruidas). Orden exacto de las tres llamadas y de los argumentos. |
| `0x007C4C30` | `RpHAnimHierarchyCreate` | 247 | S | `rphanim.cpp` | Sustituye la envoltura. Orden: `RwFreeListAlloc(0xC9B8CC, 0x3011E)` sin comprobar; `RtAnimInterpolatorCreate` (C++); campos; si no hay NOMATRICES `RwMalloc(n*64+15)` y alineado a 16 (`+15 & ~15`); `RwMalloc(n*16)` para nodeInfo; por nodo pFrame=0, nodeID solo si nodeIDs, nodeIndex=i, flags solo si nodeFlags (lo que no se escribe queda con lo que traiga el bloque: comparar solo lo escrito). parentHierarchy = si mismo. |
| `0x007C5120` | `RpHAnimHierarchyGetMatrixArray` | 8 | S | `rphanim.cpp` | Sustituye la envoltura. Lee hierarchy+8. 94 llamadores: no tocan ECX/EDX tras la llamada. |
| `0x007C51A0` | `RpHAnimIDGetIndex` | 45 | S | `rphanim.cpp` | Sustituye la envoltura. Devuelve el contador del bucle (no nodeIndex) del primer nodo con ese nodeID; -1 si no hay o numNodes <= 0. 101 llamadores. |
| `0x007C51D0` | `RpHAnimHierarchyUpdateMatrices` | 2467 | S | `rphanim.cpp` | Sustituye la envoltura. La funcion mas arriesgada del lote. Dos caminos: NOMATRICES (pila de copias de matrices de 64 B, base = copia) y con matrices (pila de punteros). Revisar: (a) calculo de la matriz del padre (identidad / `RwFrameDirty` + cadena de `RwMatrixMultiply` sobre modelling / `RwFrameGetLTM`), (b) alta en la lista de marcos sucios (`RwGlobals+0xBC`) y bit 2 del indicador privado del marco raiz, (c) flags de nodo & 3 (0 nada, 1 pop, 2 push, 3 sin cambio), (d) UPDATEMODELLINGMATRICES (copia a modelling, y `RwFrameUpdateObjects` si no hay UPDATELTMS), UPDATELTMS (copia a ltm o, con LOCALSPACEMATRICES, `RwMatrixMultiply(ltm, matriz[i], padre global)`; privateFlags = (pf & ~4) | 8), (e) sub-jerarquia (SUBHIERARCHY con rootParentOffset != -1: base = matriz del padre). El EXE incrusta `RpHAnimKeyFrameApply` (0x7C5B80) cuando el callback es esa direccion; aqui se llama siempre al callback (mismo codigo). Diferencias asumidas: los flags de las matrices identidad que el EXE monta con basura de pila (`|0x20003`) salen 0x20003, y los huecos de relleno de las matrices temporales (+0x1C, +0x2C, +0x3C), que el EXE copia sin inicializar, salen a 0: el banco no debe comparar esos huecos. Probar con 1, 2 y 31 nodos, todas las combinaciones de flags de jerarquia y de nodo (incluida la profundidad de pila), parentFrame sin padre / con padre sucio / con padre limpio, y numNodes = 0. |
| `0x007C64C0` | `RpHAnimKeyFrameStreamRead` | 116 | S | `rphanim.cpp` | Sustituye la envoltura. Por fotograma: `RwStreamReadReal(stream, &frame->time, 0x20)` y `RwStreamReadInt32(stream, &off, 4)`; `prevFrame = pFrames + (off / 36 sin signo) * 36`. Cualquier lectura que falle devuelve NULL sin deshacer; numFrames se relee del objeto en cada vuelta. Probar offsets no multiplos de 36 y >= 2^31. |

Las 13 son tipo S (`RH_ScopedGlobalInstall`); ninguna es V. Las funciones de RpHAnim y las de D3DX9 que usan coma flotante no necesitan los helpers x87 de 24 bits: las de RpHAnim no hacen aritmetica propia (la hace el callback `RpHAnimKeyFrameApply`, no reconstruido) y las de D3DX9 son SSE/3DNow!.

## Saltadas (10)

| Direccion | Bytes | Motivo (evidencia del desensamblado) |
| --- | ---: | --- |
| `0x007C20C9` | 100 | D3DXVec2TransformCoord 3DNow!: divide con `pfrcp` + `pfrcpit1` + `pfrcpit2` (reciproco aproximado por tabla, valores definidos por el hardware 3DNow!, no reproducibles bit a bit en C++ puro; la CPU de pruebas tampoco ejecuta 3DNow!). Un `1.0f / w` daria otro resultado. |
| `0x007C290B` | 100 | Normalizacion de un vector de 4 floats con `pfrsqrt` + `pfrsqit1` + `pfrcpit2` (raiz inversa aproximada del 3DNow!): igual que la anterior. Llamadores `0x7BD05E`, `0x7BD149`. |
| `0x007C2D95` | 344 | SSE con **ABI de registro**: el argumento entra en XMM0 y el resultado sale en XMM0; ninguna lectura de pila y cierra con `ret 10h` (el llamador `0x7BA772` no empuja nada: el `ret 10h` quita 16 bytes de la pila del llamador, que el compilador del EXE compensa). C++ sin `naked` no puede leer/escribir XMM0 con esa limpieza de pila. |
| `0x007C2EF0` | 398 | Igual: argumento y resultado en XMM0, `ret 10h`. Llamadores `0x7B6FAA`, `0x7B7037`. |
| `0x007C3267` | 1232 | atan2 de lanes SSE con un camino `fpatan`; entra con dos punteros por pila (`ret 8`) y **devuelve en XMM0** (`movss xmm0, ...`; el llamador `0x7BA73B` lee XMM0 al volver). Un `__stdcall` de MSVC no devuelve en XMM0; solo `__vectorcall`, que pasa los punteros en ECX/EDX. |
| `0x007C3737` | 232 | Argumento y resultado en XMM0, `ret 10h` (como `0x7C2D95`). Llamadores `0x7B97D0`, `0x7B985D`. |
| `0x007C3A40` | 245 | Funcion MMX/3DNow! con ABI de registros MMX: entra con MM0/MM1 y sale con MM0 (`pfrcp`, `pfmax`...), `ret` sin argumentos de pila. Llamador `0x7BBEBD`. |
| `0x007C3B40` | 247 | Igual (MM0 entrada y salida; `pfrsqrt`). Seis llamadores (`0x7BB89A`..`0x7BCAED`). |
| `0x007C4120` | 252 | MMX/3DNow! con MM0 de entrada/salida, usa EBX, ECX, EDX, ESI sin guardarlos y `pf2id`/`pi2fd`; 14 llamadores. |
| `0x007C4220` | 254 | Igual que `0x7C4120` (mismo cuerpo hasta el final, que normaliza con `pfrcp`); siete llamadores. |

Todas las saltadas son de D3DX9, no de RpHAnim; las seis de RpHAnim y siete de D3DX9 estan hechas.

## Notas de revision (sin banco)

- ABI: `D3DXFloat16To32Array_3DNow`, `D3DXVec2Transform*`, `D3DX9AcosPS`, `D3DX9TwoAcosScalar` y `D3DX9SinCosPS*` son `__stdcall` (el EXE limpia 4..12 bytes con `ret N`). Las de RpHAnim son `__cdecl`.
- Las funciones de D3DX9 se enganchan aunque la CPU no ejecute 3DNow!: si algun llamador las usa en una maquina con 3DNow!, el redondeo de `0x7C203B` y `0x7C2090` puede diferir en la ultima cifra (el EXE usa pfmul/pfadd del 3DNow!, no IEEE).
- `RpHAnimHierarchyUpdateMatrices` solo toca EAX, ECX y EDX como el EXE (el resto de registros los preserva el compilador por la convencion cdecl).
- Llamadas por direccion fija que NO son de este lote: `0x76626B` (escalar de medios flotantes), `0x7C5B80` (callback de aplicar fotograma), `0x7C4670`, `0x7C4720`, `0x7C4750`, `0x7C4770`, `0x7C4830`, `0x7C4A00`, `0x7C48D0`, `0x7C4BF0` (rutinas del plugin), y las ya reconstruidas `RwFrameDirty` (0x7F0340), `RwMatrixMultiply` (0x7F18B0), `RwFrameGetLTM` (0x7F0990), `RwFrameUpdateObjects` (0x7F0910), `RtAnimInterpolatorCreate` (0x7CD520).

## Bancos viejos que citan las direcciones

Busqueda con `git grep` de cada direccion sobre `reversing/` (sin `PendienteValidar` ni `runs/`). No se repiten. `BinaryCoverageAudit` es el inventario de cobertura (cita todas); los demas son bancos o estudios.

| Direccion | Carpetas de `reversing/` que la citan (ademas de BinaryCoverageAudit) |
| --- | --- |
| `0x007C1D80` | (solo BinaryCoverageAudit) |
| `0x007C203B` | (solo BinaryCoverageAudit) |
| `0x007C2090` | (solo BinaryCoverageAudit) |
| `0x007C20C9` | (solo BinaryCoverageAudit) |
| `0x007C290B` | (solo BinaryCoverageAudit) |
| `0x007C2AAE` | (solo BinaryCoverageAudit) |
| `0x007C2CA4` | (solo BinaryCoverageAudit) |
| `0x007C2D95` | (solo BinaryCoverageAudit) |
| `0x007C2EF0` | (solo BinaryCoverageAudit) |
| `0x007C307E` | (solo BinaryCoverageAudit) |
| `0x007C3267` | (solo BinaryCoverageAudit) |
| `0x007C3737` | (solo BinaryCoverageAudit) |
| `0x007C381F` | (solo BinaryCoverageAudit) |
| `0x007C3A40` | (solo BinaryCoverageAudit) |
| `0x007C3B40` | (solo BinaryCoverageAudit) |
| `0x007C4120` | (solo BinaryCoverageAudit) |
| `0x007C4220` | (solo BinaryCoverageAudit) |
| `0x007C4600` | AppBatch, OriginalDependencies499 |
| `0x007C4C30` | OriginalDependencies2026-10-17 |
| `0x007C5120` | AnimAudio6, BikeHands355, CPedA, CPedB, CPedC, CPedE, CTasks14b, CamAim355, CamFirstPerson355, CamFollowPed355, CamM16355, CppTrigCompound6Swim, CppTrigCompound6SwimCoverageProposal, CppTrigJetPack, CppTrigJetPackCoverageProposal, CppTrigSwim, Cutscene35, Dependencies21C, Fn86A, Fn89C, FxPost1, Mix11, OriginalDependencies499, PedWrappers60, STasks8b, VehicleAudio355Evidence, VisPluginsFinal4, WeaponCarShot355, WeaponInstant355, WeaponOccupant355, World355Evidence, validation-registry |
| `0x007C51A0` | BikeHands355, CPedA, CPedB, CPedC, CPedE, CTasks14b, CamAim355, CamFirstPerson355, CamFollowPed355, CamM16355, CppTrigCompound6Swim, CppTrigCompound6SwimCoverageProposal, CppTrigJetPack, CppTrigJetPackCoverageProposal, CppTrigSwim, Cutscene35, Dependencies21C, Fn180A, Fn86A, Fn89C, FxPost1, GfxWave2, Mix11, OriginalDependencies499, PedWrappers60, Pending355, STasks8b, ValidationRegistry, VehicleAudio355Evidence, VisPluginsFinal4, WeaponCarShot355, WeaponInstant355, WeaponOccupant355, World355Evidence, validation-registry |
| `0x007C51D0` | CEntA, OriginalDependencies499 |
| `0x007C64C0` | OriginalDependencies2026-10-17, RpAnim43 |
