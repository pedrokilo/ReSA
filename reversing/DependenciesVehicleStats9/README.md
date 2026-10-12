<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Habilidad de conducción y vuelo: validación aprobada

Dos dependencias reconstruidas, validadas y admitidas: CStats::UpdateStatsWhenDriving (0x55CAC0) y UpdateStatsWhenFlying (0x55CC00). Corrida **20261010-205837-320565: 4096 pares**, 2048 por función, **cero diferencias y cero resultados nulos**, rutas RH OFF/ON, colaboradores y restauración global/flotante correctos. La propuesta histórica ya está aplicada y no debe reaplicarse.

## Reconstrucción fiel y ABI

Driving-Flying.dis.txt contiene ambos cuerpos completos y margen posterior. La propuesta sustituye dos wrappers de Stats.cpp y añade sus RH dentro del InjectHooks existente. No cambia headers ni la instalación principal. Sólo conserva etiquetas asm que son destinos de salto; las restantes direcciones son comentarios para evitar C4102 sin desactivar advertencias.

| Función | Evidencia del EXE |
| --- | --- |
| Driving | 0x55CAC5 lee subtype +0x590; 0x55CADE..0x55CAE8 convierte el contador unsigned a float; 0x55CAFC..0x55CB01 usa test ah,5/jp para decidir el límite, incluida unordered. 0x55CB36..0x55CB55 calcula magnitud x87; 0x55CB68 lee +0x960 sólo para automóvil. Las rutas incrementan por 0.5 o 1.5 tras truncar el paso, o dejan el contador intacto. RET 0x55CB35/0x55CBBD/0x55CBFE. |
| Flying | 0x55CC05..0x55CC0D excluye subtype no automóvil; 0x55CC7C..0x55CC7E excluye contacto +0x960 distinto de cero en la ruta de movimiento. 0x55CCB2..0x55CCC3 lee matriz +0x14 y up.z +0x28 y conserva fcomp/test ah,5/jnp. RET 0x55CC75/0x55CD10/0x55CD51. |

Los tres colaboradores directos son IncrementStat 0x55C180, DisplayScriptStatUpdateMessage 0x55B980 y ftol CRT 0x821B40. Puentes naked push dirección/ret mantienen el CALL/RET original sin añadir estado observable a los registros. Increment/Display deben estar registrados, reconstruidos y ON en ambos lados; la prueba comprueba sus cuerpos y bytes mediante RH. ftol conserva su implementación CRT original. No se sustituye un colaborador con un mock.

Ambos cuerpos son static/cdecl, un argumento y RET sin limpieza. **El argumento sí se sobrescribe**: Driving 0x55CAE8 y Flying 0x55CC28 escriben el contador convertido a float en el slot del puntero. El gate observa ese slot antes de retirarlo. En salidas posteriores ftol puede sobrescribir además el ECX guardado en [esp]; por eso no se presupone ECX preservado: se comparan EAX/ECX/EDX crudos completos, ESP y EBX/ESI/EDI/EBP. Es igualdad observada de estos cuerpos, no un contrato genérico de funciones void.

Los cuatro llamadores directos registrados son 0x5713CB/0x5713DB para Flying y 0x571482/0x571492 para Driving. Los desensamblados callers-*.dis.txt muestran `add esp,4` tras cada llamada y salto a 0x571567; no consumen el argumento escrito ni el retorno. Las rutas de 0x5713DB/0x571492 pasan NULL, pero ambos cuerpos originales dereferencian el argumento antes de comprobarlo (0x55CC05/0x55CAC5): NULL queda explícitamente fuera del dominio seguro del banco. La propuesta conserva esa conducta original y no inventa una guarda.

## Fixture completo y restauración

16 perfiles × 128 variantes por función. Perfiles combinan subtype automóvil/avión, contacto cero/positivo y up.z -1/0/1/NaN; +0x960 es m_nNumContactWheels, los valores positivos recorren 1..4, no un flag airborne. Son **fixtures de memoria sintética**, sin constructor: 0xA00 bytes para el layout de vehículo/automóvil y 0x50 para matriz, ambos con márgenes de 16 bytes, contienen todas las lecturas reales. No hay virtuales invocadas, registro de referencias, pool, asignador ni integración en el mundo; ningún colaborador recibe el puntero al fixture. Las bases finitas de la matriz son rotaciones invertibles; NaN representa un borde numérico explícito. Todo vehículo/matriz, incluidos canarios y relleno, debe quedar intacto.

Las variantes incluyen ocho contadores (0,1,999,1000,INT_MAX,0x80000000,0xFFFFFFFE,0xFFFFFFFF), nueve límites (negativo, cero, float alrededor de 1, valores grandes, infinito/NaN), ocho pasos (incluido cero, fracciones y negativo), incrementos 0/25/50 y dieciséis velocidades con valores adyacentes a 0.2/0.5/0.8/1.3, negativo y NaN. No es todo el producto cartesiano. El JSON serializa cada input efectivo; el analizador exige exactamente el input derivado para todos los casos, sin depender de una cifra agregada.

Además de comparar los cuerpos, el analizador calcula independientemente el efecto definido sobre el contador con redondeo por operación a float, truncación y wrap de dword. Exige las rutas grant/fast/slow/unchanged, y en Flying subtype_skip/contact_skip. Verifica velocidades inferior/igual/superior a los dos umbrales de cada función dentro de rutas de movimiento activas y up.z inferior/igual/superior a cero en Flying activo: no basta tener esos inputs en casos que salieron temprano.

La enumeración estática independiente del fixture produce Driving grant=1024, fast=496, slow=288, unchanged=240; Flying grant=512, subtype_skip=1024, fast=76, slow=84, contact_skip=256, unchanged=96. Son rutas previstas, no contadores ejecutados. Las 184 instrucciones de la propuesta fueron cotejadas automáticamente contra los dos cuerpos completos, normalizando sólo etiquetas, nombres de puentes y ceros de literales: cero diferencias.

Se preparan y restauran snapshots completos de estadísticas B78200..B79517, ayuda BAA460..BAA92F, gString/gString2, PlayerInfo y los flags/búferes enumerados en LiveComparison.hpp. El paso global B7CB5C se restaura por lado. Snapshot x87 de 108 bytes y MXCSR completo: cada lado parte de la misma imagen, se restaura antes/después de publicar y se comprueba igualdad exacta al finalizar. Partidas/configuraciones se verifican con el runner; no se guarda sesión.

Las nueve rutas de colaboradores conservan su modo inicial compartido y comprueban flags, código, E9 y módulo. Se incluyen CheckForStatsMessage y los helpers de texto/HUD de su ruta anidada; esa ruta utiliza TotalNumStatMessages=0 y no configura entradas falsas de mensajes. No se certifica independencia transitiva ni se aprueban otra vez esos colaboradores. Los objetivos usan RH real OFF/ON sobre la misma entrada EXE y restauran flags/código iniciales, incluidos símbolos/cuerpos DLL.

La salida compara todos los bloques crudos observados, contador final, argumento final y registros; no omite efectos para pasar. Los gates requieren bloques locales intactos, ABI, ruta, colaboradores y restauración global/flotante. Ningún resultado nulo se acepta. No se prueba adquisición real de habilidad conduciendo un vehículo dentro del mundo: se prueba la función completa sobre estado preparado en el juego cargado.

## Construcción coordinada

El manifest enumera Stats, layouts de Vehicle/Automobile/Matrix, Timer y todos los bloques/colaboradores usados, además de HelperRoutes, HookRoute y RH Simple. Los scripts redirigen en memoria a la copia CVehicle Test sin editar runtime_runner.py. Sólo el coordinador aplica la propuesta y ejecuta en serie:

La propuesta histórica se escribió con CRLF mientras Stats.cpp tenía LF; por eso la comprobación inicial de formato rechazó el parche. La coordinación comprobó y aplicó con `--ignore-space-change`, conservando el contexto textual y las instrucciones. No fue una corrección del comportamiento reconstruido. El primer intento del banco encontró además una ruta inexistente del manifest (`source/game_sa/Matrix.h`); se corrigió a `source/game_sa/Core/Matrix.h` antes de reintentar, sin modificar la instrumentación. Las propuestas posteriores se escriben como bytes UTF-8 con LF y comprueban que todas las rutas del manifest existan.

`bash reversing/_handoff/bench.sh DependenciesVehicleStats9 3`

## Evidencia final

[Registro completo](runs/20261010-205837-320565/comparisons.jsonl.gz), [resumen](runs/20261010-205837-320565/summary.json), [aprobación](runs/20261010-205837-320565/approval-audit.json), [resultado](runs/20261010-205837-320565/RESULTADO.txt) y [auditoría independiente](audit-runtime.md).

SHA-256 del registro descomprimido: `d218e38f10bf24eba551286ac0f40a891d0706725a7b61c01ee80a8331b09725`.

SHA-256 del ASI ejecutado: `9650b4515c596a00f7aaaf51b4566232173d15a79de5ef947e62cb3236563f84`.

El runner informa partidas sin cambios y configuraciones restauradas. La revisión independiente del registro entero confirmó también el oráculo de contadores, sus bytes dentro del bloque crudo y la cobertura de los umbrales en rutas activas. El alcance continúa siendo estas dos funciones sobre memoria preparada dentro del juego, no una prueba de conducción/vuelo real ni de clases completas.

LoadStatUpdateConditions 0x559860 queda fuera por E/S, sscanf y cambios de directorio; WeaponHit 0x55CEB0 queda pendiente para un banco con clasificación de armas y estado de gang adecuados. No están reservadas ni reconstruidas aquí.
