<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión estática previa — World11

Estado: propuesta sin aplicar, pendiente de compilación y prueba dentro del juego.

## Entrada reconstruida

0x734E50–0x734E5F guarda EBX/EBP/ESI/EDI y obtiene la cabeza del listado en dict+8. 0x734E65 resta 8 al enlace para recuperar RwTexture; 0x734E68 calcula name+0x10. 0x734E70 llama a GetUppercaseKey 0x53CF30 y 0x734E78 compara el hash. 0x734E7C avanza el enlace, 0x734E80 repite hasta cabeza. 0x734E82–0x734E88 restaura registros y devuelve NULL; 0x734E89–0x734E8F devuelve la textura y restaura los mismos registros. Los parámetros son cdecl dict/hash, y ambos RET sin limpieza dejan al caller limpiar 8 bytes.

La propuesta conserva todos los accesos y saltos. Sustituye únicamente la dirección directa del helper por una variable de dirección de la función reconstruida; el banco demuestra que esta dirección resuelve al cuerpo DLL cuando el helper está ON y exige original OFF para la otra mitad. No se usa una dirección DLL cuyo thunk vuelva al EXE durante la llamada candidata.

## Datos y vida

Las aserciones de layout exigen texturesInDict+8, RwTexture::lInDictionary+8 y name+16. Se comprueba pertenencia t->dict, terminación de nombre y lista cerrada. La selección necesita al menos dos texturas y último hash único, así que found-last atraviesa un rechazo real. El hash ausente se elige fuera de todos los hashes del diccionario. El diccionario vacío se construye con el allocator y registro de plugins originales.

0x7F3644–0x7F3681 inserta lInInstance+0x10 en la lista global situada en (*0xC97B24)+(*0xC97B4C). 0x7F3684–0x7F368A inicializa la lista de texturas vacía. 0x7F36F1–0x7F3700 restaura enlaces al destruir; 0x7F3703–0x7F3713 libera mediante la tabla RW original. Se capturan módulo de 20 bytes, enlace inicial y diccionarios/texturas prestados antes de crear; se captura nuevamente el estado después de crear para verificar sólo lectura de la función bajo prueba. La destrucción final debe recuperar los estados anteriores a la creación. No se examina metadata privada del heap.

## Criterios pendientes

El banco todavía debe compilar, acreditar la entrada real de hook y los bytes/module de las bibliotecas dentro del juego, producir 2.048 pares únicos con objetos no nulos de comparación, cuatro grupos de 512 casos, cero diferencias, ABI preservada y restauración completa de los estados declarados. La igualdad aislada no basta para admisión. La rama imposible name==NULL se conserva y se documenta como inalcanzable en el dominio de objetos vivos, sin fixture inválido.
