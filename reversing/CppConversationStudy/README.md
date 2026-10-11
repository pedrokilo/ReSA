# Conversaciones: ensamblador a C++ (11/10/2026)

Conversión de las dos únicas fuentes con ensamblador en `source/game_sa/Conversations`.

## CConversationForPed::IsPlayerInPositionForConversation (0x43AC40)

Los tres bloques `__asm` x87 pasaron a `ConversationMathCpp.cpp` (`ConversationTooFarCpp`, `ConversationFacingAwayCpp`, `ConversationMovingTooFastCpp`), compilado /O2 /Oi /arch:IA32 /fp:fast /Zi sin RTC/PCH/unity sólo para ese archivo. Objeto real: mismas operaciones y mismo orden de sumas que el ensamblador ((dz²+dy²)+dx², (z·z+y·y)+x·x, x²+y²), FSQRT en el cálculo de distancia y velocidad, y la comparación FCOMP + FNSTSW + TEST AH,41h que da `false` con NaN. El compilador reparte los registros x87 de otra forma (FXCH). El cuerpo de la función sigue siendo código Debug: se probó el resultado y el estado, no igualdad byte a byte con el EXE.

Banco: CppConversationPosition, corrida 20261011-135326-813627, 2414 pares (los fixtures de posición del banco histórico ConversationForPed), cero diferencias. Un intento anterior (134333-986645) falló por disco lleno, sin resultado ni cambios en partidas o configuración.

## CConversationNode::ClearRecursively (0x43A710)

Sustituye el envoltorio `naked` que preservaba EDX por un hook directo (`RH_ScopedInstall`) a `CConversationNode::ClearRecursively` definido en `ConversationNodeCpp.cpp` (/O2, mismas opciones por archivo). El objeto real sólo escribe EAX, ECX y ESI (guardado), igual que el EXE (ver `exe-clear-recursively.json`), no contiene referencias a EDX y termina con OR EAX,-1. Se movieron a ese archivo las definiciones de `Clear` y `ClearRecursively`; la constante 0x969570 de `CConversations::m_Nodes` está duplicada allí para no incluir `Conversations.h`, y el banco comprueba que coincide.

Banco: CppConversationClear, corrida 20261011-135438-113628, 2048 pares con registros crudos: EDX preservado en todos los pares, EAX=0xFFFFFFFF, banderas definidas 0x84 (SF y PF) en todos, hash de los 50 nodos idéntico. Sólo grafos acíclicos (un ciclo recursaría sin fin también en el EXE).

## Límites

No cubre NaN ni infinito más allá de los 18 casos x87 históricos, ni el contrato de registros crudos del cuerpo de IsPlayerInPositionForConversation, ni otros llamadores de ClearRecursively, ni la rama `Update`. Los objetos `.obj` de antes y después no se publican (hashes abajo).

| Objeto | SHA256 |
| --- | --- |
| ConversationForPed.actual-before.obj | 0e2fcb794e46d7a0a896a9a3e17a489e5d2ad36229cf659ef6046540c7141f2b |
| ConversationForPed.actual-after.obj | 6cc3f4c84e5ff0bbe26e1e9a811042e8e191c32276afca9fd4c099a624ddec5f |
| ConversationNode.actual-before.obj | 0cde90cf13d65f9dbcc2cafccaf84754934bdef7479d16bce0678f9b2f5c98a0 |
| ConversationNode.actual-after.obj | fc02335eef6f0620bd4ad555b72f1517bd261f29e44eec8b4e556edb0249a772 |
| ConversationMathCpp.actual.obj | 5d9ee3b014dceff56d8a36674c2d5429443f8056834c9f5a35115e5dfb42e8a3 |
| ConversationNodeCpp.actual.obj | 0ccc505336018beb5d888cdce292a5be2815a01ce478b688983f788db5d7130a |
