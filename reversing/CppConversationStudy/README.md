<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Conversaciones: ensamblador a C++ (11/10/2026)

Conversión de las dos únicas fuentes con ensamblador en `source/game_sa/Conversations`.

## CConversationForPed::IsPlayerInPositionForConversation (0x43AC40)

Los tres bloques `__asm` x87 pasaron a funciones `static __declspec(noinline)` dentro de `ConversationForPed.cpp` (`ConversationTooFar`, `ConversationFacingAway`, `ConversationMovingTooFast`), de modo que la clase queda en sus dos archivos, `.cpp` y `.h`. El archivo completo se compila con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity: `#pragma optimize` dentro de un archivo Debug no sirve con `/ZI` y `/RTC1` (el compilador lo ignora y deja código Debug con cada `float` guardado en memoria y una llamada a `__CIsqrt`).

Con las funciones insertadas en línea, `/fp:fast` sustituyó `sqrt(s) > 4` por `s > 16` y eliminó el FSQRT, lo que cambia resultados en los bordes de redondeo. Por eso siguen `noinline`: el objeto real conserva FSQRT, FCOMP + FNSTSW + TEST AH,41h, el orden de las sumas ((dz²+dy²)+dx², (z·z+y·y)+x·x, x²+y²) y el resultado `false` con NaN. El objeto es igual al de los helpers del estudio anterior.

Al ser /O2 para todo el archivo, el banco valida la clase completa: CppConversationForPed, corrida 20261011-143612-800417, 4702 pares (2414 de posición y 2288 de `Update`), cero diferencias. `Update` nunca tuvo ensamblador; se revalida porque cambian sus opciones de compilación. Reemplaza al banco CppConversationPosition (corrida 20261011-135326-813627, ya sin vigencia: su fuente `ConversationMathCpp` ya no existe).

Para compilar sin PCH hizo falta corregir dos errores de cabecera que el PCH ocultaba: las especializaciones `notsa::PoolOf<T>()` de `Pools.h` y ocho funciones de `CompressedVector.h` estaban definidas sin `inline`, y duplicaban símbolos al incluir `StdInc.h` fuera del PCH.

## CConversationNode::ClearRecursively (0x43A710)

Sustituye el envoltorio `naked` que preservaba EDX por un hook directo (`RH_ScopedInstall`) a `CConversationNode::ClearRecursively`, que queda en `ConversationNode.cpp` junto con `Clear` e `InjectHooks`: el archivo completo se compila con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (la clase queda en sus dos archivos, `.cpp` y `.h`). El objeto real de `ClearRecursively` mide 92 bytes como el del EXE, sólo escribe EAX, ECX y ESI (guardado), no contiene referencias a EDX y termina con OR EAX,-1 (ver `exe-clear-recursively.json`). Accede a los nodos con `m_Nodes.data()[i]`: `m_Nodes[i]` añadía el assert de límites de `std::array`, con una ruta que llama a `__CrtDbgReport` y clobberaría EDX, y el EXE no comprueba límites.

Banco: CppConversationClear, corrida 20261011-141656-479908 (repetida tras unificar el archivo; la anterior 20261011-135438-113628 era del archivo ConversationNodeCpp.cpp, ya eliminado y sólo se conserva como historia), 2048 pares con registros crudos: EDX preservado en todos los pares, EAX=0xFFFFFFFF, banderas definidas 0x84 (SF y PF) en todos, hash de los 50 nodos idéntico. Sólo grafos acíclicos (un ciclo recursaría sin fin también en el EXE).

## Límites

No cubre NaN ni infinito más allá de los 18 casos x87 históricos, ni el contrato de registros crudos del cuerpo de IsPlayerInPositionForConversation, ni otros llamadores de ClearRecursively, ni la rama `Update`. Los objetos `.obj` de antes y después no se publican (hashes abajo).

| Objeto | SHA256 |
| --- | --- |
| ConversationForPed.actual-before.obj | 0e2fcb794e46d7a0a896a9a3e17a489e5d2ad36229cf659ef6046540c7141f2b |
| ConversationNode.actual-before.obj | 0cde90cf13d65f9dbcc2cafccaf84754934bdef7479d16bce0678f9b2f5c98a0 |
| ConversationNode.actual-final.obj | 297eff01f1cb05918fc8dac542f6c46521e432e10d376113b87916f6991abe56 |
| ConversationForPed.actual-final.obj | 5a870c5a8fe80482740caa51e61f8c060c95a09b3e5311946cb14c10baf39b1f |
