<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de Stats10

Corrida `20261010-210539-196204`. Lectura íntegra del gzip y comprobación nueva de resultados sin ejecutar el analizador del banco, compilar o iniciar GTA.

## Comparaciones y colaboración

6144 pares únicos exactamente 0..6143, tres funciones con 2048 casos únicos exactamente 0..2047. Cero resultados nulos, cero desigualdades de los objetos completos, cero equal/accepted falsos. Cada lado cumple stack_restored, nonvolatile_preserved, route_ok, helper_route_ok, nested_helper_route_ok, arguments_unchanged, globals_restored y floating_state_restored. Cada resultado contiene los 17 bloques crudos previstos. Los tres resúmenes indican 2048 casos y cero fallos; cierre único con todos los gates finales verdaderos, incluida nested_helper_restored.

Once colaboradores se identifican en el comienzo con route_ok=true. FindPlayerInfo53FB70 y ftol821B40 conservan el EXE sin RH; los otros nueve están reconstruidos, registrados y ON. Increment55C180, Display55B980, GetSkill743CD0 y Coop441390 ejecutan cuerpos DLL reales compartidos.

Un nested_helper_begin identifica FatMuscle55C470 durante natación, con entrada, símbolo, cuerpo y módulo ASI. Los 2048 pares de natación conservan esa metadata y helper_route_ok=true en ambos lados. La lectura del observador confirma ON+locked, Reversed, igualdad de bytes, resolución entrada/símbolo al cuerpo y pertenencia al mismo módulo; al acabar se restauran el modo/código inicial y el snapshot exterior del objetivo. FatMuscle se compara también por separado como objetivo OFF/ON, no sólo como colaboración compartida.

## Efectos independientes y cobertura

Se recalcularon los efectos definidos de contadores desde los inputs, respetando FILD signed/+2^32, redondeo PC24 por operación, truncación, producto bajo de dword y división unsigned por diez. Los resultados corresponden a los campos explícitos y a sus dwords dentro de state[0] para todos los pares. El byte B79534 del bloque crudo corresponde igualmente a flag_after.

| Dominio derivado de inputs/efectos | Casos |
| --- | ---: |
| Fat grant / acumulación, en ambos objetivos que lo ejecutan | 1056 / 992 |
| Salud máxima grant / acumulación | 864 / 1184 |
| Armas: co-op / habilidad omitida / cambio / mostrar / acumular | 1024 / 408 / 308 / 232 / 76 |
| Natación: ritmo normal / rápido | 512 / 1536 |
| Stamina grant / acumulación | 960 / 1088 |
| Lung omitido / grant / acumulación | 1024 / 464 / 560 |

Los inputs contienen todas las armas 22..32 y los cuatro pares de bool de natación; perfil/value corresponden a cada identificador. Las ramas de pérdida e indicador y sus límites también se enumeran en complete_paths. Esta cobertura deriva de inputs serializados y oráculos, no de tracing dinámico de cada instrucción ni de todo el producto cartesiano. Los bool contienen bits altos no canónicos como el llamador original; el gate compara los slots físicos completos.

## Restauración e integridad

Los 17 snapshots incluyen las estadísticas ampliadas hasta B79537 para cubrir B79534, ambos PlayerInfo, ayuda/búferes completos, flags y timestep. Cada lado restaura esos bloques y la imagen x87 de 108 bytes/MXCSR. El cierre incluye los tres hooks y la ruta anidada restaurados con flags/código originales. No acredita todos los globales del juego, una sesión real de co-op ni independencia transitiva de HUD/texto/CRT.

SHA-256 recalculado del JSONL descomprimido: `92b924ac1a782c07b1ff53ab839931f26c5df4262ab02b02e0243491877c9399`. ASI disponible: `7bfe88b1b786c4497d5e604dd288c85d9460e35cdf8d58795686e718dfc1b7be`. Coinciden con aprobación, resumen y manifest. Las 42 fuentes del manifest coinciden con aprobación y archivos actuales al revisar; los cuatro instrumentos coinciden también.

El resumen del runner declara partidas sin cambios y configuraciones restauradas y conserva tres huellas iniciales de partidas y dos de configuración, sin huellas posteriores individuales archivadas. Esas propiedades se apoyan en el runner; no se releyeron archivos vivos de otra corrida ni se recalculó el EXE vivo. La propuesta se aplicó con LF sin reparación de instrucciones; el tratamiento de F7 E2/`mul edx` está documentado en README. No se encontró un pase nulo ni discrepancia oculta en el registro completo.
