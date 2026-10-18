<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Estadísticas y mensajes: validación aprobada

Las cuatro entradas están reconstruidas, validadas y admitidas. La corrida `20261010-203949-345184` aprobó **8.192 pares**, 2.048 por función, con **cero diferencias y cero resultados nulos**, rutas RH OFF/ON y restauración global/flotante correctas. La propuesta histórica ya está aplicada a Stats.cpp; no debe volver a aplicarse.

| Función | Entrada | Último RET comprobado |
| --- | --- | --- |
| FindCriminalRatingString | 55A210 | 55A778, ret |
| UpdateStatsOnRespawn | 55CFC0 | 55D02A, ret |
| UpdateStatsAddToHealth | 55D030 | 55D087, ret |
| CheckForStatsMessage | 559760 | 55985F, ret |

Los cuatro archivos `.dis.txt` conservan el desensamblado del EXE original con margen después del último RET. La propuesta transcribe íntegramente cada cuerpo, incluidas todas las salidas intermedias; usa puentes naked con `push dirección; ret` en instrucciones separadas para los colaboradores reales, sin sustituirlos. El llamador limpia el argumento de AddToHealth: el propio original modifica ese slot, y el banco observa también su valor final. No se sustituye la truncación x87 de 821B40 por una conversión C++.

Rating: todos los umbrales, negativos incluidos, y ambos lados de cada límite positivo; selección especial de 1150–1299 mediante B79034, de 375000–499999 mediante B790C4 y de ≥1000000 mediante dinero mostrado. Se construye el rating a través de estadísticas reales, el dinero real de PlayerInfo y FindCriminalRatingNumber 559080. Se compensa la penalización por arrestos cuando se cambia B79034 y se exige que el número observado corresponda al esperado. No hay sustituto de ese clasificador. El resultado debe apuntar a texto GXT cargado, no ser nulo ni el búfer GxtErrorString. Se comparan dirección y contenido.

Salud: contadores 0/1/2/3/9/10/11, límites signed/unsigned y FFFFFFFF; suma con overflow unsigned, umbrales negativos/cero/igual/menor/mayor/infinito/NaN, salud inferior/igual/superior a1000 y NaN, incrementos0/25/50. Respawn usa fild signed y +2^32 antes de comparar, restablece el contador al alcanzar umbral incluso cuando no aumenta salud; AddToHealth escribe primero la suma, y sólo restablece el contador si alcanza el umbral. Se usan IncrementStat55C180 y DisplayScriptStatUpdateMessage55B980 reales con estadísticas y mensajes restaurados entre lados.

Mensajes: seis condiciones de salida iniciales por separado, casos normales, 0/1/2/5/127/128/129 entradas, marcas displayed, ambos tipos de condición y comparación menor/igual/mayor/unordered. El límite de128 se ejecuta aunque el contador indique129. No se agrega un break después de una coincidencia: el original puede mostrar varias entradas. La clave se obtiene de un registro configurado realmente por el juego y se comprueba contra el GXT cargado; falta de clave válida impide aprobar. TheText/GetStatValue/HelpMessageDisplayed/SetHelpMessage permanecen reales.

HookRoute exige las cuatro RH registradas, captura el cuerpo DLL antes de apagar cada hook, hace OFF/ON para cada pareja y deja OFF entre llamadas. El gate llama siempre a la entrada EXE, captura EAX/ECX/EDX inmediatamente, comprueba ESP y EBX/ESI/EDI/EBP, y usa semillas idénticas. No se interpretan los valores volátiles como contratos semánticos de una función void, pero se exige su igualdad observada.

Se guarda y compara/restaura el bloque completo de estadísticas B78200..B79517, todos los estados y los tres búferes de ayuda BAA460..BAA92F, gString/gString2, PlayerInfo completo, JustOutOfFrontEnd, foco, cheat, gates de pantalla/cutscene/menús, replay, primer carácter de mensajes grandes/garaje, GxtErrorString, x87 completo y MXCSR. Cada lado restaura antes de bombear mensajes; se exige igualdad de cada huella, bytes de entradas OFF y restauración independiente de las cuatro rutas iniciales. Partidas y configuraciones siguen bajo la comprobación del runner. No se guarda la sesión.

Las rutas y bytes de nueve colaboradores se registran y **validan** en begin y antes/después de cada llamada. HelperRoutes identifica RH por entrada, comprueba flags y código frente al contenido esperado de RH, resolución E9 y módulo del cuerpo. Los modos iniciales se conservan compartidos; los wrappers sin reconstruir deben permanecer OFF. Las entradas sin RH deben resolver al EXE. La prueba cubre estas cuatro funciones con colaboradores reales, sin certificar independencia transitiva de FindCriminalRatingNumber, texto, HUD, CRT o las funciones ya reconstruidas. CheckForStatsMessage permanece OFF como colaborador anidado de las funciones de salud, y se prueba como objetivo OFF/ON por separado. No se fuerzan errores de GXT ni estados inválidos de PlayerInfo.

Preparación revisada: scripts Python con AST válido y propuesta aplicada virtualmente a la fuente actual sin escribirla. El manifiesto añade HelperRoutes y RH Simple a los colaboradores enumerados. El hunk de RH se coloca antes de Init para convivir con StatsRadio7, que añade sus dos RH después de Init; aplicar y revisar ambos secuencialmente.

La [revisión independiente](review.md) cotejó 498 instrucciones sin diferencias y corrigió exclusivamente el banco: snapshot x87 completo de 108 bytes con FNSAVE/FRSTOR, MXCSR íntegro y gates antes de publicar/finales; prueba efectiva de colaboradores; selección independiente de arrestos y dinero; tiempos de vuelo 0/36000000/39600000 ms para alcanzar ambas rutas alrededor de diez horas. El analizador exige las tres variantes de vuelo dentro de 375000..499999, dinero 9999999/10000000/10000001 dentro de rating≥1000000 y arrestos 0/1001 dentro de 1150..1299, además de 8192 pares únicos completos, ningún nulo y todas las restauraciones. La propuesta de fuente no cambió.

Después de la revisión inicial, la compilación detectó C4102/C2220 por 429 etiquetas asm sin referencias. La coordinación convirtió únicamente esas etiquetas en comentarios `// 0xADDR` en Stats.cpp y source-proposal.patch, conservando las etiquetas usadas como destinos y las instrucciones. No se desactivaron advertencias. Por tanto, la afirmación anterior de propuesta intacta describe la revisión inicial, no esta reparación posterior.

## Evidencia final

[Registro completo](runs/20261010-203949-345184/comparisons.jsonl.gz), [resumen](runs/20261010-203949-345184/summary.json), [aprobación](runs/20261010-203949-345184/approval-audit.json), [resultado](runs/20261010-203949-345184/RESULTADO.txt) y [auditoría independiente](audit-runtime.md).

SHA-256 del registro descomprimido: `36580e799fa2346a4a9a8186e6f85a0266eb50fa4cd1f1f453a0df1aa4e792c8`.

SHA-256 del ASI ejecutado: `acfc66e52cb4a0e0d1d18b0fad5f7a5a71da34bed33e37d5d6178900c6f55ca1`.

Partidas sin cambios y configuraciones restauradas según el runner. La aprobación cubre estas cuatro entradas con sus colaboradores reales en el modo registrado, sin certificar independencia transitiva ni validar las clases completas de texto/HUD/estadísticas.
