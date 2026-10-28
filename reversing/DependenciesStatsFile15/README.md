<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesStatsFile15 — carga de condiciones estadísticas

Validado dentro del juego y registrado tras la corrida 20261010-224119-554752. Una entrada GAME: CStats::LoadStatUpdateConditions `0x559860`, 2.048 pares originales/RH OFF contra reconstrucción/RH ON.

## Reconstrucción e identidad

`LoadStatUpdateConditions.dis.txt` conserva las 101 instrucciones completas `559860..5599A5`. La propuesta registra el método existente y reemplaza su wrapper por ensamblador naked fiel, incluido frame EBP/alineación de ocho bytes, locales inicializados, cinco argumentos de sscanf, comparaciones de nueve bytes y copia con terminador. Hay seis etiquetas referenciadas; las demás direcciones son comentarios para /WX. `repe cmpsb` expresa exactamente el opcode F3 A6 de las dos comparaciones del EXE.

El EXE usa SetDir("") en `559880`, OpenFile("DATA\\STATDISP.DAT","rb") en `55988F`, LoadLine en `5598A5/55997B` y CloseFile en `559996`. Filas vacías o que comienzan con # se omiten. La llamada `559902: call 8220AD` aplica `%d %s %s %f %s`; guarda el int truncado a int16 en cada registro de 16 bytes de B78200, displayed=0, condition=0/1 sólo para lessthan/morethan, float y texto con terminador. La condición desconocida conserva el byte anterior. Al final guarda el número de registros en B794D0.

No se recrea un parser distinto ni se delega la función principal a su entrada original. Los cinco destinos de CALL usan puentes cpp naked privados push-dirección/ret: SetDir, OpenFile, LoadLine, CloseFile y sscanf. Sustituyen operandos CALL absolutos ambiguos para MSVC y conservan registros, flags, argumentos físicos y retorno; las entradas reales de los helpers siguen siendo las probadas por el banco. ReadLine es una sexta dependencia anidada, sin nuevo CALL directo. Los cinco colaboradores de juego son SetDir5387D0, OpenFile538900, LoadLine536F80, CloseFile5389D0 y ReadLine5389B0 anidado. El banco exige RH registrado, reconstruido y ON para los cinco, Body propio, E9, módulo ASI y bytes/flags iniciales intactos antes y después; así ambas rutas usan las mismas reconstrucciones reales. sscanf8220AD es biblioteca original compartida, se exige ruta EXE sin E9. No se acredita independencia de CRT ni validación nueva de estos colaboradores.

## Archivos reales aislados

El banco verifica que el EXE corre en `E:/JUEGOS/GTA San Andreas - CVehicle Test` antes de crear nada. Sólo crea una subcarpeta nueva `dependenciesstatsfile15-fixture-PID`, su DATA y STATDISP.DAT propio. Si esa carpeta ya existe, aborta. Redirige temporalmente ms_rootDirName a esa raíz; nunca escribe STATDISP.DAT del clon principal ni la instalación personal. Escribe una fixture completa y cierra el writer antes de ambos lados. Ambos usan apertura/lectura/cierre reales; el contenido se compara íntegro tras cada llamada.

Después de cada lado se reabre el mismo archivo con CreateFileW/GENERIC_READ y share=0. El éxito prueba que no queda un handle lector abierto de ese archivo; ese handle de comprobación también se cierra. No se infiere que el proceso entero conserve su número de handles ni el estado interno del asignador CRT. Al final se elimina únicamente el archivo creado y sus dos directorios vacíos, con rutas hijas fijas verificadas, sin borrado recursivo.

## Fixtures y oráculo

32 perfiles×64 valores = 2.048 casos. Cantidades de filas 0/1/2/7/8/16/31/64/127/128, comentarios iniciales/intercalados/finales, líneas vacías, LF/CRLF y separadores espacio/coma/tab. Los bytes exactos de859C6C son72 62 00 (rb), exigidos también en begin/analyzer: apertura binaria sin traducción CRLF a LF, tanto fopen como _wfopen reciben ese mode sin cambios. El oráculo conserva ambos bytes antes de sanitizarlos a dos espacios. Las líneas pasan por el LoadLine real que convierte comas y bytes de control en espacios y devuelve el primer byte no blanco. Se usan cinco tokens completos en cada fila de datos: no se introducen lecturas scanf incompletas de locales indefinidos. Textos de exactamente siete caracteres más NUL caben en los ocho bytes del registro; ninguna línea llega al límite512.

IDs int32 incluyen -32768/-1/0/32767/65535/INT_MAX/INT_MIN para demostrar truncado, y condiciones lessthan/morethan/equals para los dos writes y el byte conservado. Floats finitos incluyen +0/-0, 1.25/-2.5, 1e-10/1e10 y máximo float32. No hay string overflow, más de128 filas, sscanf malformado o archivo inaccesible en el dominio declarado.

El oráculo Python reconstruye todos los bytes de los mensajes y sus dos guardas globales, contador, buffer512 de LoadLine y dos buffers128 de CFileMgr, incluidos padding y colas no limpiadas. Emula el buffer de la última línea leída, EOF que conserva buffer y sanitización completa; una fila desconocida conserva condition. No reemplaza las llamadas reales del banco. Generador offline: todos los dominios exigidos presentes, 256 casos sin registros, 256 con uno y 192 con128; 1.024 LF, 1.024 CRLF, 1.024 separadores coma/tab. No es producto cartesiano ni tracing de instrucciones.

## ABI y restauración

Gate void sin argumentos, ESP y EBX/ESI/EDI/EBP, EAX/ECX/EDX crudos, errno DLL y LastError Win32 observados. Después de la llamada se comparan FNSAVE de 108 bytes y MXCSR íntegros, sin normalización; no hay retorno flotante del método. Cada lado restaura FP108/MXCSR, cwd Win32 y CRT, los cinco bloques globales y errores DLL/Win32 antes de procesar mensajes. No se afirma conservar todos los internos o caches de CRT original; sscanf recibe entradas válidas del dominio declarado.

HookRoute restaura Enabled/Locked y código de entrada/cuerpo/ILT. Se conserva también código inicial de sscanf y cinco colaboradores. Ningún registro pasante puede tener original/candidate nulos. `global_state_final` contiene los cinco snapshots completos, cwd, errores y FP reales finales: el analizador exige igualdad íntegra con begin. Se mantiene `complete` compacto para el detector de4096 bytes del runner. El protocolo usa marker dependenciesstatsfile15-runtime.start, log runtime.jsonl, espera jugador/cutscene y captura de fallos con prefijo string; no se modifica runtime_runner.py.

## Preparación anterior a la validación

AST de ambos scripts, existencia de13 paths de manifiesto, oráculos de2.048 fixtures/layout y git apply --check comprobados offline. Estas comprobaciones preparatorias precedieron a la aplicación, compilación y validación dentro del juego documentadas abajo.

## Validación aprobada

Corrida 20261010-224119-554752: 2.048 pares, cero diferencias, cero registros nulos y estado, partidas, configuraciones y binarios originales restaurados. Verificación independiente archivada en runs/20261010-224119-554752/approval-audit.json.

La corrida anterior 20261010-223046-041730 produjo 2.048 pares iguales, pero se rechazó por WinError 1450 al restaurar el PDB. Se conservó su resultado fallido, se restauró el PDB por bloques verificando tamaño y SHA256 y se repitió el banco completo; runtime_runner.py permanece intacto.
