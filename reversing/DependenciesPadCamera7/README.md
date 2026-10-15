<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesPadCamera7 — aprobada y registrada

Tres dependencias aprobadas y registradas con run **20261010-203312-870430**: **20480 pares, cero diferencias y cero nulos**, restauración completa, partidas/configuraciones conservadas. No reaplicar la propuesta histórica ni ejecutar prepare.py.

| EXE | Alias descriptivo | Pares |
|---|---|---:|
| 5404A0 | CPad::GetCameraCycleUp | 4096 |
| 540E80 | CPad::GetMouseCameraStickHorizontal | 8192 |
| 540F80 | CPad::GetMouseCameraStickVertical | 8192 |

**20.480 pares aprobados**. Los alias describen el contrato y no afirman nombres binarios originales. No incluye AutoPilot41B980 ni Garage449FF0, liberadas para otros lotes.

Se reemplazan las llamadas delegadas en Camera.cpp/Cam.cpp y MouseCameraStick se convierte en un selector de las dos implementaciones completas; se añaden sólo tres hooks CPad. CPad y CCam ya se inyectan: InjectHooksMain no necesita cambio.

Evidencia dirección+instrucción en disassembly.txt. 5404A0 usa tabla binaria5404DC, verificada desde el EXE original: modos0/2/3 van a5404B3 y modo1 a5404C1. El candidato usa dispatch local y nunca salta al cuerpo del EXE. El caso de modo fuera0..3 devuelve sólo AL; el banco compara bool AL. Ejes comparan int16 AX porque otros bits de EAX son indefinidos en algunas salidas. Horizontal conserva x87, condiciones estrictas y tail-jumps reales a CRT821B40; vertical conserva NEG en int16 para -32768 y los epílogos enteros/x87. No se usa std::abs/float C++ para sustituir la implementación candidata.

Único colaborador del juego: CCam::Using3rdPersonMouseCam50A850, ya reconstruido y catalogado. Se apaga para original y se activa para candidata. El banco comprueba la ruta y demuestra que el símbolo miembro mutable resuelve al cuerpo DLL capturado al activar RH, perteneciente al mismo módulo ASI. La biblioteca CRT821B40 permanece real en EXE y su módulo se verifica; no hay mocks, stubs ni sustitución del teclado/dispositivos del sistema. No se atribuye independencia transitiva de los servicios reales del host.

Fixtures CPad de tamaño completo, alineados, con guardas y campos nativos (mode10A, disable10E, botones26/1A/4A/10/40). No se ejecuta ciclo de vida ni se presenta la arena privada como pad del jugador. Ciclo cubre modos0/1/2/3, modos fuera de rango y old/new cero/no cero positivos/negativos. Ejes usan128 valores incluidos extremos int16, ambos lados y puntos exactos50/100, casos impares de truncado y muestras repartidas; cruzados con inversión, modo de cámara, bandera mouse y las cuatro combinaciones disable/botón. Se exige ejecutar retorno0, ruta de magnitud>100 y ruta de mouse>50; el ciclo exige true/false/modo inválido.

El banco escribe solamente campos reales de cámara/mouse que usan las funciones y los restaura antes de cada Write: CCam real completoB6F19C, mouseB73400..B7345F y flagB6EC2E. La arena CPad permanece privada; todas las entradas y guardas deben quedar idénticas. Compara outputs/contrato, ESP, EBX/ESI/EDI/EBP, top/control x87; guarda y restaura FNSAVE/FRSTOR y MXCSR. Restaura cuatro rutas RH y verifica bytes/flags iniciales. Original/candidate siempre son objetos completos, incluso al devolver cero.

No altera runtime_runner.py, documentos congelados ni instalación personal de GTA. Todos los scripts leen UTF-8 explícitamente y el manifiesto cubre fuentes y rutas que realmente se ejecutan. Validación admitida con cero diferencias y NULL y estado restaurado; auditoría independiente en [audit-runtime.md](audit-runtime.md).

SHA256 log `794ccaced29634e35862395dd61827b2e1dd212f1d1101f6c5f3cb6adc4d6f49`; ASI `f5c5ced31dd1584cff207272603aee52803731c7cbc7adbfc1b1956e8aa8ce3e`. La evidencia se limita a los fixtures descritos; no acredita integración universal ni independencia transitiva del EXE.
