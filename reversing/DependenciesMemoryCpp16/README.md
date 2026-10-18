<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesMemoryCpp16 — observación real del CRT

Banco separado propuesto para sustituir exclusivamente la identidad de instrucciones de la candidata en DependenciesMemory9. **Compilado; la primera comprobación sobre fuente ASM se detuvo antes de comparar pares y no autoriza registrar funciones.** Memory9 y su evidencia permanecen intactos.

## Contrato

Cuatro rutas registradas, EXE original con hook apagado frente a fuente con hook activo: Malloc 0x72F420, Realloc 0x72F440, MallocAlign 0x72F4C0 y FreeAlign 0x72F4F0; 2048 fixtures por función, 8192 pares. Se conservan argumentos físicos después de retornar, pila, registros no volátiles, payload definido, preservación de prefijo, alineamiento/backpointer, canaries independientes, balance de recursos y estado x87 de 108 bytes/MXCSR. FreeAlign debe conservar el contrato físico original: el primer slot queda convertido en la base liberada. Este banco no relaja ese requisito.

DR0/1/2 vigilan entradas CRT 0x824257/0x824269/0x82413F con breakpoints de ejecución locales. VEH registra EIP, ESP, retorno y argumentos directamente de la pila del hilo propietario. No modifica bytes ni argumentos CRT, ni sustituye el asignador. Los cuerpos completos CRT y los cuatro cuerpos originales se contrastan contra bytes canónicos. Sólo se normalizan identidades de bloques después de verificar los argumentos crudos de cada lado; tamaños permanecen numéricos.

Un hilo cooperativo suspende al propietario cuando éste espera un evento, obtiene y configura su contexto, verifica lectura posterior y reanuda exactamente su incremento de suspensión. Nunca usa GetThreadContext sobre el hilo actual en ejecución. Rechaza slots/debugger/TF/GD preexistentes. El handler usa almacenamiento fijo, sin allocations ni locks; sólo modifica DR6 y RF. Comprueba que registros generales y estado FP permanecen idénticos. La restauración exige readback exacto de DR0..DR7 guardados, cierre del helper y eliminación del VEH. Una restauración fallida no puede admitir resultados.

## Calibración obligatoria

Antes de las parejas se realizan llamadas reales: malloc(33)/free; realloc(NULL,17) con su malloc anidado; realloc(bloque,0) con free anidado; realloc(bloque,65) preservando 23 bytes. Se comprueban los retornos anidados canónicos 0x824286 y 0x824299. El control negativo llama malloc(17) esperando 18: debe producir un evento físico de 17 y ser abortado mediante SEH **antes de la primera instrucción del asignador**. Debe demostrar que nunca se ejecutó la operación errónea. También se comprueba que llamadas fuera del scope no generan registros de pareja y que un segundo hilo no contamina el propietario.

Si aparece una petición inesperada, se rechaza antes de ejecutarla, se termina la campaña y jamás se toca un payload cuyo tamaño no esté demostrado. Un aborto posterior al inicio de realloc puede dejar ownership desconocido: se informa restauración incompleta y no se inventa un free seguro. El runner debe cerrar ese proceso fallido. OOM, memoria corrupta, CRT modificado y eventos debug ajenos son bloqueos, no aprobaciones.

## Límite de la prueba de procedencia

La candidata no debe delegar al cuerpo original. El banco verifica módulo ASI, estabilidad de 64 bytes y retorno dentro de una ventana de 1024 bytes de su entrada (o retorno directo al gate para tail calls). **Esa ventana no delimita toda la función ni demuestra por sí sola ausencia de delegación.** La revisión del fuente, desensamblado compilado y xrefs sigue siendo obligatoria antes de sustituir el gate anterior. Se guardan direcciones y retornos crudos para inspección; el analizador reproduce independientemente argumentos, secuencias, fixture, propietario y registros debug.

## Ejecución serial por el responsable

```bash
CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh DependenciesMemoryCpp16 3
```

Usa exclusivamente el clon E:/JUEGOS/GTA San Andreas - CVehicle Test. No modifica runtime_runner.py ni instala nada en E:/JUEGOS/GTA San Andreas. `run_local.py --analyze <comparisons.jsonl>` exige 8192 parejas completas, sin resultados nulos, cero diferencias y restauración real.

La comprobación 20261011-010415-108830 produjo cero pares: al armar el observador se solicitó DR6=0, pero el procesador devolvió 0xFFFF0FF0. La restauración del contexto guardado fue exacta. Se clasifica como defecto del instrumento, sin evidencia de diferencia del juego. La corrección inicializa únicamente el contexto armado a 0xFFFF0FF0: bits reservados uno y causas BLD/RTM inactivas, según [Intel SDM 18.2.3](https://cdrdv2-public.intel.com/825748/253669-sdm-vol-3b.pdf). El handler exige el valor completo con exactamente un breakpoint propio. El contexto guardado y la comparación de restauración permanecen crudos, sin normalización. Esta corrección necesita repetir la calibración antes de convertir la fuente.

La repetición 20261011-011149-240118 pasó las cinco calibraciones positivas. El negativo abortó malloc(17), pero añadió una asignación malloc(8) desde operator new (CALL 0x8211B2, retorno 0x8211B7) durante el manejo de la excepción; la procedencia exacta de esa asignación todavía es una inferencia. Se rechazó la campaña, con cero pares y restauración verificada. Congelar la captura al primer rechazo evita incluir operaciones posteriores; un filtro pendiente comprueba hilo, EIP, ESP y campaña. La siguiente repetición 20261011-011513-432630 terminó con SINGLE_STEP no manejado antes de comparar pares. Sigue bloqueado el instrumento: MemoryMgr.cpp conserva el ensamblador anterior y ninguna de estas corridas aprueba una conversión.

El banco publica también el primer slot físico del argumento después del retorno en la evidencia de cada lado. Su auditor debe verificar el puntero base en FreeAlign y el argumento entrante en las otras tres funciones; no basta el booleano emitido por el instrumento.

## Fundamento de las APIs

Microsoft exige suspender el hilo para obtener/modificar un contexto válido: [GetThreadContext](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadcontext), [SetThreadContext](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadcontext). El formato x86 está documentado en [CONTEXT](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-context-r2). El handler debe evitar allocations y sincronización: [PVECTORED_EXCEPTION_HANDLER](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nc-winnt-pvectored_exception_handler). Se exige éxito al [eliminar VEH](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-removevectoredexceptionhandler). Los breakpoints de instrucción son faults antes de ejecutar y RF evita repetirlos al continuar: [Intel SDM, volumen 3B, sección 19.3.1.1](https://cdrdv2-public.intel.com/858456/253669-088-sdm-vol-3b.pdf).
