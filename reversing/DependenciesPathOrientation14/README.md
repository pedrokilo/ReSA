<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesPathOrientation14 — selección y orientación de nodo

Preparado, propuesta sin aplicar, sin compilación ni prueba dentro del juego. Una función GAME pendiente: `CPathFind::FindNodeOrientationForCarPlacement(CNodeAddress)` en `0x450320`. Banco de 2.048 pares principales y 2.048 pares auxiliares del colaborador Normalise; sólo la principal se propone para admisión.

## Fuente y evidencia del EXE

`FindNodeOrientation.dis.txt` conserva el cuerpo completo de 126 instrucciones `0x450320..0x4504DE`, incluidos los tres retornos `ret 4`. La propuesta activa su registro existente en CPathFind::InjectHooks y reemplaza el wrapper por ensamblador completo. Se preservan sólo las etiquetas referenciadas; las restantes direcciones son comentarios, sin desactivar /WX.

La función obtiene área/node del argumento de cuatro bytes, exige área cargada y número de enlaces no cero. Para los primeros links-1 enlaces comprueba área de navegación cargada; si la dirección del CCarPathLink coincide con el nodo consultado usa carriles bits3..5, en otro caso bits0..2. Selecciona el primer enlace con carriles o el último por defecto. `0x4503E3: mov [esp+0x38],eax` sobrescribe el argumento físico con el nodeLink elegido antes de exigir que el destino esté cargado. El banco exige ese argumento mutado exacto, incluso en retorno temprano por destino ausente; sólo los retornos anteriores conservan el argumento inicial.

El cuerpo convierte coordenadas x/y/z comprimidas con 0.125, construye la diferencia XY y fuerza Z=0. `0x4504C3: call 0x59C910` normaliza esa diferencia. Después se conservan literalmente los dos `fld [esp+8]`, uno con `fchs`, `fpatan` y multiplicación por la constante de `0x859878`, antes de `0x4504DE: ret 4`. No se sustituye por una interpretación geométrica presumida distinta de las instrucciones del EXE.

## Colaborador propio y prueba auxiliar

La clase CVector sigue inactiva en Main. Para no delegar al EXE la propuesta incluye un helper cpp `PathOrientation14Normalise` naked, las 32 instrucciones completas de `0x59C910..0x59C960` preservadas en `Normalise.dis.txt`. El call de la función reconstruida enlaza a este helper DLL. No se instala RH nuevo en CVector ni se acredita esa función fuera de la cohorte.

El helper tiene símbolo enlazable desde el banco y no requiere header nuevo. Los 2.048 auxiliares llaman a `0x59C910` original sin E9 y al helper DLL directo, exigiendo módulo EXE/ASI correcto y cuerpo resuelto. Se comprueban bytes originales de entrada, ILT y cuerpo antes/después, EAX/ECX/EDX, ESP y no volátiles, vector de doce bytes y canarios completos. Hay vectores cero, ejes, signos, componentes 3/4, octavos, 1e10 y 1e-10 finitos. El oráculo independiente PC24 reconstruye todos los bytes del vector.

El gate auxiliar registra FNSAVE completo de 108 bytes con una sola normalización documentada: la dirección de instrucción x87 de cuatro bytes en offset12 cambia entre EXE y DLL. Se conserva cruda en original_fp_ip_raw/candidate_fp_ip_raw del registro, se exige que caiga dentro del cuerpo correcto y se compara el resto íntegro (control/status/tag/opcode/selectores/data-pointer/80 bytes de registros), más MXCSR. Una sentinel ST0=3.25 comprueba la conservación de la pila por un helper que no retorna float. La propia función usa el mismo estado legal inicial y el gate extrae float32 y diez bytes crudos del resultado ST0.

## Fixtures y cobertura

Objeto sintético completo CPathFind 0x3C80, tres fuentes CPathNode 0x1C, dieciséis destinos, marcador de área, quince CCarPathLink 0xE, 45 nodeLinks y 45 naviLinks: siete bloques con canarios y padding serializados íntegros. No son nodos cargados de un archivo de la partida. No hay mocks, entidades virtuales ni pools; ThePaths real sólo se observa completo y permanece intacto.

64 perfiles×32 valores: 64 áreas de origen, destinos ordinarios y áreas64–71, los 16 valores del nibble de enlaces, navegación/destino ausentes, owner coincidente/diferente, carriles0–7, selección temprana/última, destino de la misma área, XY cero/no cero y signos del X normalizado. Incluye z fuente/destino no cero aunque el cuerpo la fuerza a cero antes de Normalise, int16 extremos y wrap. No es producto cartesiano ni tracing de instrucciones.

Comprobación offline del generador: 64 fuente ausente, 64 sin enlaces, 152 casos con navegación ausente, 408 selección del último enlace, 1.512 selección temprana, 128 destino ausente, 1.792 retornos de ángulo y 128 XY cero. Todos los dominios requeridos aparecen. El analizador reconstruye independientemente los siete bloques y el argumento elegido; compara íntegramente ambos retornos flotantes, sin imponer un oráculo de atan diferente del EXE. La rama temprana sí exige float +0.

## Rutas, protocolo y restauración

Original y candidate de la principal llaman la misma entrada EXE mediante RH producción OFF/ON. Candidate debe resolver al Body de la misma ASI. HookRoute conserva/restaura Enabled/Locked y bytes de entrada/cuerpo/ILT; también se comprueban bytes de ambos helpers. Cada lado restaura los siete bloques, vector auxiliar y FP108/MXCSR. Ningún lado JSON puede ser nulo; los contadores principales/auxiliares y casos únicos se exigen completos.

ThePaths real tiene eventos inicial/final completos separados y complete compacto. Inicio con marker dependenciespathorientation14-runtime.start, log runtime.jsonl, captura de fallo con prefijo string y espera de jugador/cutscene. Nunca se modifica runtime_runner.py. AST, once paths de manifiesto, generador/oráculos de 2.048 inputs y git apply --check comprobados offline. La repetición final siguiente acredita ejecución, restauración, ABI y rutas dentro del dominio declarado.

## Revisión del primer banco en juego

La ejecución 20261010-221706-623876 obtuvo 2.048 pares principales y 2.048 auxiliares iguales, con restitución completa, pero fue rechazada por una expectativa incompleta del oráculo del argumento físico. El EXE escribe primero el enlace seleccionado en 0x4503E3; en la ruta con destino cargado reutiliza la misma palabra de pila y la última escritura, 0x45046D: mov [esp+30h],edx, guarda Z del destino extendida con signo. Las 1.792 discrepancias del oráculo afectan únicamente esta expectativa; original y candidato coinciden. Se corrige el oráculo, conservando íntegramente la fuente y las observaciones, y se exige repetir el banco antes de cualquier admisión.

## Validación final admitida

Corrida [20261010-222007-638464](runs/20261010-222007-638464/summary.json) aprobada: 2.048 pares principales y 2.048 auxiliares, cero diferencias, cero lados nulos, rutas RH OFF/ON, ThePaths y FP108/MXCSR restaurados; partidas y configuración conservadas. El auxiliar Normalise se demuestra por separado y no se admite como función adicional de la cohorte. La corrección del argumento físico del primer banco afecta al oráculo; no cambia el cuerpo reconstruido.

SHA256 del JSONL descomprimido: `f56aca0c5fd38d8d16b014fe91ce149d8d0be7f9b9e9a7e197053610bd09c16a`. ASI ejecutada: `ee6c3f0183e04f286dec949bc3177a0aa483c6053c86ca32ef252f85091a9396`. [Auditoría independiente](audit-runtime.md). Los tres instrumentos admitidos quedan congelados; los hashes históricos de las fuentes probadas no se sustituyen por sus versiones actuales.
