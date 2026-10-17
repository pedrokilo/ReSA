<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diagnóstico fijo de rechazo VEH → filtro SEH

Propuesta separada, sin inclusión ni aplicación al banco. `FixedRejectionDiagnostic.hpp` escribe registros binarios POD de 144 bytes en un handle preabierto. No usa CRT, JSON, filesystem, strings, allocations propias ni lectura de una pila desconocida. Registra EIP/ESP del rechazo y del contexto recibido en cada filtro, identidad de hilo/campaña, pending/scope/count, causas DR, excepción/flags/address y condiciones de coincidencia. Un guard atómico evita recursión; límite de 256 registros y contadores de errores/pérdidas permiten detectar evidencia incompleta.

## Integración propuesta para una campaña sólo diagnóstica

1. Fuera de VEH/SEH, antes de armar DR: CreateFileW con GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, sin OVERLAPPED; usar una ruta del clon de pruebas. Verificar handle y entregarlo mediante SetPreopenedHandle. No abrir/crear/cerrar dentro del handler.
2. En el primer rechazo propio, después de fijar rejectedEip/rejectedEsp/rejectedSerial/pending y cerrar scope, antes de CONTINUE_SEARCH: escribir kind=1 con State tomada de los campos ya capturados. Usar returnAddress/args del evento guardado; jamás dereferenciar el ESP recibido en el filtro.
3. Al entrar en CADA Filter, construir mask con bits independientes: owner=0, rejected=1, serial=2, SINGLE_STEP=3, EIP=4, ESP=5 y pending==1=6. Obtener pending con lectura atómica, sin consumirlo. Escribir kind=2, filterResult=0xFFFFFFFF (decisión todavía pendiente). Mantener intacto el predicado y CAS existentes.
4. Después de evaluar el predicado y CAS exactamente una vez, escribir kind=3 con la decisión real (EXCEPTION_EXECUTE_HANDLER=1; CONTINUE_SEARCH=0). No repetir el CAS para diagnosticar. Actualizar State.pending a su lectura posterior, conservando mask previo. Devolver la decisión original.
5. Cerrar el handle sólo después de desarmar y retirar el observer, fuera de handler/filtro. Si termina con excepción, el archivo puede aportar registros anteriores; si hay cola incompleta o missed/error, no deducir ausencia de eventos.

`decode_rejection_diagnostic.py archivo.bin` muestra los registros y condiciones fallidas. Debe comprobarse que el contexto del Filter proviene de GetExceptionInformation pasado directamente desde la expresión __except. No usar ESP actual del propio Filter ni una captura nueva de contexto. También se compara ExceptionAddress con EIP y se inspeccionan flags de despacho/unwind.

## Interpretación y límites

No se concluye que todo despacho SEH preserve ESP. Si EIP coincide pero ESP cambia, el registro demuestra la diferencia y hay que estudiar qué contexto/VEH/intercepción la produjo antes de proponer otra identificación del rechazo. Si el filtro nunca aparece, el fallo podría ocurrir antes de alcanzarlo o interceptarse por otra ruta; no equivale automáticamente a fallo de comparación ESP.

WriteFile síncrono puede bloquear y tener efectos internos de Windows: no se promete ausencia absoluta de allocations ajenas ni transparencia temporal. Este logger es exclusivamente diagnóstico y **su campaña no se registra como validación**. No reemplaza gates ni prueba que el original no se ejecutó por sí solo. Los argumentos propios siguen congelados antes de dispatch; efectos de diagnóstico quedan fuera de scope y no se toleran como eventos de pareja. Tras localizar la causa, retirar el logger y repetir la calibración negativa y campaña completas.

Fuentes: [GetExceptionInformation](https://learn.microsoft.com/en-us/windows/win32/debug/getexceptioninformation) documenta el contexto asociado a la excepción y el paso de EXCEPTION_POINTERS desde la expresión a una función filtro. [WriteFile](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile) documenta duración del buffer, escritura síncrona, bytes escritos y errores. No se asumió equivalencia entre ese contexto y el ESP de entrada del handler después de otras intervenciones.

## Parche de integración preparado

`integrate_rejection_diagnostic.patch` modifica exclusivamente cinco archivos del banco separado: HardwareTrace.hpp, TraceSupport.hpp, LiveComparison.hpp, build_probe.py y run_local.py. Está SIN APLICAR. El header del diagnóstico permanece en esta carpeta de propuesta y se añade a las fuentes hasheadas del manifiesto. Los hashes de los cinco archivos base se guardan en `integrate_rejection_diagnostic_base_hashes.json`; si el banco cambió entretanto, regenerar/revisar el parche.

Start recibe una ruta construida fuera de VEH/SEH en el clon: `memorycpp16-debug.bin`. Abre el handle antes de instalar VEH/armar DR. Si CreateFile falla, se rechaza preflight. El primer rechazo se registra tras congelar scope; Filter registra condiciones previas y decisión posterior sin alterar el predicado ni repetir su CAS. El DR6 del contexto registrado puede reflejar la limpieza propia ya realizada; los argumentos/retorno provienen del evento guardado, no de otra lectura de la pila.

Stop cierra el handle únicamente tras restaurar DR, terminar helper y retirar VEH. Un cierre fallido no declara Stop exitoso. Begin/error/final incluyen `diagnostic_only=true`, nombre, registros intentados, write_error, missed y handle_closed. Los ocho controles de calibración y el requisito negativo Count==1 permanecen intactos. El analizador rechaza explícitamente cualquier campaña con diagnostic_only=true, aunque sus 8192 parejas pasaran: retirar/revertir la instrumentación diagnóstica antes de la campaña final de admisión.

No se cambió MemoryMgr.cpp ni se aplicó el parche, compiló o inició GTA. La revisión offline verificó que los cinco archivos compartidos conservaran su contenido durante la generación y que las transformaciones Python fueran sintácticamente válidas. El C++ todavía requiere compilación real de MSVC.

## Diagnóstico ejecutado y corrección del contexto

La campaña diagnóstica 20261011-014929-702149 conservó tres registros completos. El VEH recibió EIP 0x824257 y ESP 15446780; el filtro recibió EIP 2006049153 y ESP 15466468, mientras ExceptionAddress seguía en 0x824257. Fallaron exactamente los predicados EIP y ESP. No hubo errores ni pérdidas en esos registros. La campaña terminó con SINGLE_STEP y no se admitió.

WindowsExceptionHandler pasaba por referencia el contexto compartido a StackWalk. Esa operación puede modificarlo según [Microsoft](https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-stackwalk64). Se cambió únicamente a una copia por valor, sin relajar Filter. La campaña diagnóstica 20261011-015225-344561 pasó las ocho calibraciones, incluida la negativa con un solo evento y aborto anterior a la instrucción del asignador. Se detuvo deliberadamente después de obtener esa evidencia; tampoco se admite.

El archivo binario del logger se abre con CREATE_ALWAYS cada vez que el observador se reinicia: otro Start sobrescribe los registros de calibración. No se afirma conservar el contexto crudo del filtro corregido; la evidencia posterior es la calibración exitosa con condiciones estrictas. Se retiró el logger restaurando sus cinco archivos originales byte por byte; se añadió app_debug.cpp al manifiesto. Falta una campaña completa sin logger antes de aprobar el observador o convertir MemoryMgr.cpp. Evidencias en diagnostics/.
