<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión independiente previa a ejecución

Se revisaron propuesta, fixture, gates, desensamblados y colaboradores. La revisión comenzó sólo lectura; la coordinación autorizó después corregir exclusivamente instrumentación, analizador y README. **No se cambió source-proposal.patch, producción, catálogos ni evidencia previa. No se compiló ni ejecutó el juego.**

## Fuente y ABI

Se cotejaron automáticamente 498 instrucciones etiquetadas de los cuatro cuerpos propuestos con `dis.sh` sobre el EXE original. Normalizando sólo espacios, destinos de etiquetas y nombres de puentes a la dirección externa equivalente, se obtuvieron **cero diferencias**. Esto verifica la transcripción estática; no sustituye la prueba real.

Los puentes `push dirección; ret` reenvían al colaborador sin alterar registros ni flags y dejan el retorno del CALL original del candidato en la pila. Los métodos usan cuerpos naked y los RET originales: 0x55A778, 0x55D02A, 0x55D087 y 0x55985F. AddToHealth modifica su propio argumento en 0x55D03B, y el llamador retira los cuatro bytes; Gate observa ese slot después de la llamada. Respawn preserva ECX mediante push/pop explícitos, como el EXE. Las lecturas x87, extensión unsigned mediante +2^32 y ramas unordered se conservaron.

## Defectos del banco encontrados y corregidos

1. **Estado flotante incompleto.** Restaurar `_controlfp` únicamente conserva el control, pero no status, TOP, tags, registros ni MXCSR. Los fixtures NaN y las instrucciones `fnstsw ax` pueden arrastrar flags a EAX y provocar diferencias de banco. Se añadió FNSAVE/FRSTOR de 108 bytes y STMXCSR/LDMXCSR completos. Cada lado arranca del estado guardado; al restaurar se compara la imagen x87 completa y MXCSR. Write restaura también antes y después de publicar, y el final exige ambos gates.

2. **Rutas de colaboradores sólo informativas.** Registrar entry_bytes/dll no demostraba el modo efectivo ni el destino de código. `HelperRoutes.hpp` ahora identifica RH Simple por entrada, conserva sin modificar sus modos iniciales, comprueba enabled/locked/reversed, bytes de entrada y biblioteca frente al contenido RH esperado, cuerpo E9 y módulo ASI cuando está ON. OFF debe resolver al EXE y su biblioteca regresar al mismo destino; un wrapper con reversed=false no puede estar ON. Entradas sin RH deben resolver a código del EXE, y sus bytes permanecen iguales. Se comprueba antes/después de cada llamada y al terminar. También se añadieron snapshots de 16 bytes de las cuatro entradas/símbolos/cuerpos de los objetivos para verificar la restauración final.

3. **Tiempo de vuelo insuficiente para la rama alta.** El fixture anterior usaba B790C4=100000 o 0. Las instrucciones 0x55A702..0x55A714 multiplican ese entero por 1/60000 (0x863FD0) y 1/60 (0x859044), después truncándolo mediante 0x821B40. 100000 produce unas 0.0278 horas y nunca satisface el `>10` de 0x55A719..0x55A721. Se utilizan ahora 0, 36000000 y 39600000 ms: inferior, diez horas y once horas. El analizador exige los tres dentro de ratings 375000..499999.

4. **Dinero correlacionado con la banda.** Con 66 bandas, la única banda ≥1000000 correspondía siempre a scenario%3=2 y dinero 10000001. Faltaban las rutas ≤10000000 de 0x55A74D..0x55A757. El dinero varía ahora con el grupo de repeticiones de bandas, independientemente del índice: 9999999, 10000000 y 10000001. El analizador exige los tres cuando rating≥1000000.

5. **Arrestos correlacionados con la banda.** Enumerar los 2048 escenarios originales mostró sólo B79034=1001 para ratings 1150..1299, aunque la documentación afirmaba ambas rutas. El nuevo selector independiente usa 0 y 1001 y conserva la compensación ×3 del helper real de rating. El analizador exige ambos en ese intervalo.

La enumeración estática del fixture corregido confirmó las tres combinaciones especiales completas. No se cambió ningún umbral ni instrucción de la propuesta para hacer pasar el banco.

## Puerta y alcance

El analizador específico exige 8192 pares únicos, 2048 casos exactos por función, original/candidate no nulos, igualdad y gates verdaderos para ABI, contratos de fixture, restauración global/flotante y rutas. Rechaza rutas incompletas, wrappers habilitados sin reconstruir y títulos GXT nulos/de fallback. Verifica además la cobertura de vuelo/dinero/arrestos descrita.

Los nueve colaboradores directos conservan el **modo inicial compartido**, no conmutan junto al objetivo. Esto puede incluir rutas originales OFF; se identifican expresamente en begin. IncrementStat puede llamar a CheckForStatsMessage: durante los objetivos de salud ese otro objetivo permanece OFF como las restantes rutas, mientras que su propia función se prueba OFF/ON por separado. Esta tanda no acredita independencia transitiva del EXE ni valida otra vez los colaboradores.

Las capturas de EAX/ECX/EDX para void se mantienen como igualdad observada de esta transcripción y sus colaboradores, no se convierten en contratos semánticos universales. Ningún campo de salida ni estado mutable se elimina de la comparación. La propuesta todavía requiere construcción y prueba completa en el juego; esta revisión no es aprobación.

## Nota posterior: compilación y validación

Los hechos anteriores describen la revisión inicial. Posteriormente, C4102/C2220 impidió compilar por 429 etiquetas asm sin referencias. La coordinación convirtió sólo esas etiquetas en comentarios de dirección en Stats.cpp y source-proposal.patch; conservó destinos referenciados e instrucciones y no desactivó advertencias. Los instrumentos permanecieron intactos. La corrida `20261010-203949-345184` aprobó las cuatro entradas con 8192 pares; véanse README y audit-runtime.md para la evidencia final y sus límites.
