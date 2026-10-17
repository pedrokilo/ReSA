<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CONTEXT del logger: revisión independiente

Diagnóstico leído sin modificar: 432bytes, tres records; hash y campos en diagnostic-readonly.json. VEH EIP0x824257/ESP15446780. Filtro EIP2006049153/ESP15466468 conserva ExceptionAddress y rejectedEIP/ESP originales. Mask79=0x4F pierde únicamente predicados bits4/5 de EIP/ESP; decision0 CONTINUE_SEARCH.

Logging::Logging registra WindowsExceptionHandler con AddVectoredExceptionHandler(1). HardwareTrace::Start registra después Handler con1; al retornar CONTINUE_SEARCH se ejecutan handlers previos antes del filtro SEH. La versión inspeccionada previamente tomaba CONTEXT& del ContextRecord y entregaba ese alias a StackWalk. Microsoft documenta que ContextRecord es in/out y puede modificarse. Esto explica coherentemente la corrupción observada hacia un frame superior sin cambiar ExceptionRecord.

Corrección mínima: CONTEXT context=*pExceptionInfo->ContextRecord por valor. StackWalk modifica sólo copia local, conservando el contexto de excepción para handlers posteriores. No toca filtro, condicionesEIP/ESP, CAS ni banco. Durante revisión la fuente ya estaba actualizada por coordinación a copia por valor, línea100: no se aplicó parche ni se modificó producción. Debe confirmarse solución en ejecución posterior; evidencia estática no descarta otros handlers mutadores.

Fuentes oficiales verificadas: [StackWalk64](https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-stackwalk64), [AddVectoredExceptionHandler](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler).

Sin compilación, GTA, Git, cambios de producción o instrumentos.
