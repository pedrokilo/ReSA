<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de Memory9

Se abrió y recorrió íntegramente el gzip de la corrida20261010-210141-343078, sin ejecutar juego ni build, y se contrastaron registros y hashes con approval-audit.json y summary.json. Resultado: **4 funciones, 8192 pares únicos, cero diferencias, cero lados nulos**, y2048 casos únicos0..2047 por función. Se verificaron directamente `equal=true`, `accepted=true` e igualdad de los dos objetos en todos los pares; no se tomó una marca global de aprobación como sustituto de esa revisión.

## Prueba de argumentos y colaboradores

Los cuatro body_proofs tienen entradas72F420/72F440/72F4C0/72F4F0, longitudes completas14/19/35/16 bytes y opcode offsets5/10/12/11. Se recalcularon destinos de desplazamientos con signo: original llega al CRT824257/824269/824257/82413F respectivamente y el candidato al puente que registra. Fuera de sus cuatro bytes de displacement, los cuerpos son idénticos. Cada puente es exactamente PUSH inmediato de ese CRT / RET, seis bytes, y pertenece al mismo módulo ASI que el cuerpo. No se normalizaron instrucciones, fallos ni opcodes para obtener igualdad.

Los tres cuerpos CRT completos en begin se compararon contra CrtProof.hpp:18/429/113 bytes, entradas resueltas a sí mismas y gate de ruta verdadero. No se modificó el CRT. El instrumento aprobado verifica los bytes del candidato mientras está activo antes/después de cada llamada, la entrada original mientras está apagada, y la restitución final de bytes y flags RH. El log incluye **212992 gates verdaderos**:13 campos×2 lados×8192 pares. Incluyen cand_body_contract, ESP/no volátiles, argumentos físicos, colaboradores, contratos, recursos y restitución de x87/MXCSR. No se interpreta EAX de FreeAlign como resultado útil ni se comparan literalmente punteros del heap de lados distintos.

## Payload y vidas propias

Se reconstruyó independientemente el patrón de cada caso: `(índice*37+case*13+0x5A)&255`. Todos los bytes registrados de payload definido coinciden, total **4735002 bytes entre ambos lados**. Malloc y MallocAlign observan exactamente size bytes que el banco inicializó; Realloc observa min(old,new) bytes conservados antes de escribir la zona nueva. FreeAlign no lee el bloque después de liberarlo.

Se verifican seis perfiles legales distintos de Realloc: NULL+0, NULL+positivo, owned+0, reducción, tamaño igual y crecimiento. Owned+0 devuelve NULL y se contabiliza su liberación una sola vez; NULL+0 sigue malloc real. Los2048 casos de cada función y todas las fronteras de tamaños/alineación se preservan. Hay **16384 vidas de recursos propios por lado**, incluidas las reservas guardianas, con allocations=frees en todos los registros. Es un balance lógico de llamadas reales y recursos del fixture; no certifica una disposición interna idéntica del heap ni cuenta reservas internas del CRT.

El guardian de64 bytes es otra reserva real cuyos canarios se verifican, no una redzone contigua ni una medición de capacidad del bloque objetivo. requested_size es un argumento del fixture y su transmisión queda demostrada por los cuerpos compilados descritos arriba; no es una lectura de `_msize`. No se fuerza OOM ni se afirma probar recuperación por fallo excepcional. Los controles son conformes a los límites declarados en README.

## Restitución y huellas

El único complete declara8192 pares y state_restored/helper_route_ok/floating_state_restored verdaderos. Summary confirma partidas intactas y configuraciones restituidas. Los nueve archivos del manifiesto y los cuatro instrumentos actuales coinciden físicamente con los hashes archivados; la ASI actual coincide con la ejecutada. El cambio previo de paths MemoryHeap al directorio Memory está reflejado correctamente en el manifiesto definitivo.

| Evidencia | SHA256 verificado |
|---|---|
| Log descomprimido | `1ee38e958e608ad4fb9419db8b8fcda35cfdfa8174112b5a4e926c8e93f7187d` |
| Gzip archivado | `bda3a375c5b086237ad156bf76b703f8a0f92956ffdceb2341692b51d5aba173` |
| ASI ejecutada | `06d3e6c0922f7613b66b9542159d37627dc8d680e5683db8bad2d6213507f38c` |
| LiveComparison.hpp | `8498942e8f3271f2dddb56a42243946472932427ba54c58ad51a3196bd68b11b` |
| CrtProof.hpp | `e6761c33e2386316cb17d4fd19a9ca9dea3ee0ec6e3a32aee3d40afc0c3c0bf9` |
| build_probe.py | `1e2d266a63167addccb43bfbbfc7c65272d5541fd3727785a680a2eb89847b5a` |
| run_local.py | `bd34a1d390c80bb62bc64344749f260c51b9c2e5c4471277860c2f792b449ad9` |

Alcance: las cuatro hojas con el CRT auténtico y recursos privados en el juego completo. No supone admisión de auxiliares de biblioteca ni prueba transitiva de todo el CRT, de sus handlers SEH excepcionales o de otras plataformas/heap alternativo.
