# CONTEXT del logger: revisión independiente

Diagnóstico leído sin modificar: 432bytes, tres records; hash y campos en diagnostic-readonly.json. VEH EIP0x824257/ESP15446780. Filtro EIP2006049153/ESP15466468 conserva ExceptionAddress y rejectedEIP/ESP originales. Mask79=0x4F pierde únicamente predicados bits4/5 de EIP/ESP; decision0 CONTINUE_SEARCH.

Logging::Logging registra WindowsExceptionHandler con AddVectoredExceptionHandler(1). HardwareTrace::Start registra después Handler con1; al retornar CONTINUE_SEARCH se ejecutan handlers previos antes del filtro SEH. La versión inspeccionada previamente tomaba CONTEXT& del ContextRecord y entregaba ese alias a StackWalk. Microsoft documenta que ContextRecord es in/out y puede modificarse. Esto explica coherentemente la corrupción observada hacia un frame superior sin cambiar ExceptionRecord.

Corrección mínima: CONTEXT context=*pExceptionInfo->ContextRecord por valor. StackWalk modifica sólo copia local, conservando el contexto de excepción para handlers posteriores. No toca filtro, condicionesEIP/ESP, CAS ni banco. Durante revisión la fuente ya estaba actualizada por coordinación a copia por valor, línea100: no se aplicó parche ni se modificó producción. Debe confirmarse solución en ejecución posterior; evidencia estática no descarta otros handlers mutadores.

Fuentes oficiales verificadas: [StackWalk64](https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-stackwalk64), [AddVectoredExceptionHandler](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler).

Sin compilación, GTA, Git, cambios de producción o instrumentos.
