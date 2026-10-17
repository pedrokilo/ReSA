<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> Actualización 11/10/2026: este banco se compiló y ejecutó dentro del juego; la corrida 20261011-135438-113628 aprobó 2048 pares con cero diferencias (auditoría en runs/20261011-135438-113628/approval-audit.json). Las frases siguientes que dicen «no compilado ni ejecutado» describen el estado de cuando se escribió. Resultado y límites en [CppConversationStudy](../CppConversationStudy/README.md).

# CppConversationClear

Banco por pares dentro del juego para `CConversationNode::ClearRecursively` (0x43A710): EXE (hook apagado) frente a
`source/game_sa/Conversations/ConversationNode.cpp` (archivo completo compilado con /O2 /Oi /arch:IA32 /fp:fast) enganchado directamente con
`RH_ScopedInstall`. **No compilado ni ejecutado.** Ningun resultado anterior (p. ej. Pools56, 512 pares sin registros)
valida este candidato.

## Que cubre (2048 pares, una funcion)

- Por caso: copia limpia de los 50 nodos de `CConversations::m_Nodes` (0x969570), grafo acíclico determinista, llamada cruda
  `thiscall` con ECX=&nodo raiz, EDX y EAX sembrados (EDX alterna entre constante distintiva y puntero a otro nodo),
  EBX/ESI/EDI sembrados, `cmp eax,0`+`pushfd` antes y `pushfd` inmediatamente tras `call`.
- Formas (16): hoja, cadenas yes/no/alternada, binario completo, peines yes/no, mixto, subárbol compartido (ambas ramas al
  mismo hijo), DAG de saltos, DAG aleatorio, árbol aleatorio, cadena duplicada, raiz->extremo, árbol aleatorio pequeño, cadena de 49.
  Raices 0 y 49 incluidas; hasta 49 nodos alcanzables; nodos señuelo fuera del arbol (pueden apuntar al arbol, nunca al reves).
- Puertas en AMBOS lados: ruta (RH) correcta, ESP igual, EBP/EBX/ESI/EDI preservados, **EDX == semilla**, DF conservado,
  nodos finales == oraculo (alcanzables: nombre[0]=0, ramas -1, voces 0; resto intacto), tocados == alcanzables,
  ventanas de guarda de 64 bytes a cada lado de m_Nodes, snapshot restaurado, FP/MXCSR canonicos. Ademas original == candidato
  en todo el JSON (EAX, ECX, hash de los 50 nodos, banderas definidas).
- Banderas: se registra `flags_defined` (mascara 0x8C5; AF excluida por indefinida tras OR). La hipotesis SF|PF (0x84) se
  registra como `flags_hypothesis_ok` y el auditor la EXIGE como valor unico observado, pero no condiciona la puerta en juego
  (la puerta es igualdad EXE/candidato).

## Que NO cubre

- **Ciclos**: el original recursa sin fin; los fixtures los excluyen (se verifica con DFS antes de llamar).
- Indices fuera de [-1,49] u otros negativos distintos de -1; nombres con ambos bytes finales distintos de lo sembrado.
- EDX preservado no prueba el cuerpo completo del EXE ni el resto de contratos de CConversations::RemoveConversationForPed
  (0x43A960) o CConversationForPed::Clear (0x43A770): solo que este candidato no escribe EDX/EBX/EDI/EBP y deja iguales EAX, ECX y banderas.
- No comprueba que el objeto se compile con /O2: lo demuestra solo el comportamiento de registros; `candidate_body_bytes` queda registrado para revision manual.
- Las ventanas de guarda podrian dar falsos fallos si otro hilo escribe datos vecinos de m_Nodes durante una llamada.
- Partida guardada del juego como fixture (sin NewGameConfiguration): los nodos reales se restauran, no se prueban.

## Uso

`python build_probe.py` -> `python run_local.py` -> `python audit/audit_run.py CppConversationClear <carpeta_de_ejecucion> [--archive]`.
