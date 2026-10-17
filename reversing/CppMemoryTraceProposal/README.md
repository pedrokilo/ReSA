<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Observación real de argumentos al CRT para el banco de memoria C++

**Estado: diseño offline, no instrumentación aprobada.** No se modificó
DependenciesMemory9, su evidencia histórica, producción, el runner ni los
archivos de IntFloat24. No se compiló ni se ejecutó GTA.

La comprobación histórica `cand_body_contract` exige cuerpos de instrucciones
idénticos salvo desplazamientos de llamadas. Ese contrato demuestra la
transmisión del tamaño, pero rechaza una reconstrucción C++ con instrucciones
distintas. Su sustitución debe observar el argumento **real**, en el momento
en que el CRT original lo recibe. No basta eliminar el gate, comparar capacidad
del heap, ni repetir `requested_size` desde el fixture.

## Observador propuesto y viabilidad

En GTA x86, tres breakpoints hardware de ejecución del **hilo del banco**:

| Registro | Entrada auténtica | Observación antes de ejecutar la primera instrucción |
| --- | --- | --- |
| DR0 | malloc 0x824257 | retorno `[ESP]`, tamaño uint32 `[ESP+4]` |
| DR1 | realloc 0x824269 | retorno `[ESP]`, puntero `[ESP+4]`, tamaño `[ESP+8]` |
| DR2 | free 0x82413F | retorno `[ESP]`, puntero `[ESP+4]` |

Un VEH registra la excepción propia `EXCEPTION_SINGLE_STEP`, sin reemplazar
instrucciones ni argumentos del CRT. Para estos breakpoints el procesador
interrumpe antes de la instrucción. El handler conserva EIP y pone RF para
ejecutar esa instrucción una vez sin repetir el mismo breakpoint. Esto se
fundamenta en el [manual de Intel, volumen 3B, sección 19.3.1.1](https://cdrdv2-public.intel.com/858456/253669-088-sdm-vol-3b.pdf).

La estructura [CONTEXT x86 oficial](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-context-r2)
incluye EIP, ESP, EFlags y Dr0..3/Dr6/Dr7. Microsoft muestra modificar el
CONTEXT y devolver CONTINUE_EXECUTION en su [ejemplo VEH](https://learn.microsoft.com/en-us/windows/win32/debug/using-a-vectored-exception-handler).
Esto hace el mecanismo **plausible y comprobable**, pero esas páginas no
garantizan expresamente que una excepción software capture todos los Dr*
ni que una modificación de esos campos sea aplicada como se espera en todas
las versiones de Windows/WOW64. No se declara que ya funciona en el equipo.

Hay dos formas de preparar los registros:

1. Preferida y documentada: un hilo auxiliar del banco suspende brevemente
   al propietario que está en una espera cooperativa sin locks del juego,
   captura `CONTEXT_DEBUG_REGISTERS`, establece la configuración y verifica
   un segundo readback. Al final restaura el snapshot y verifica otra vez.
2. Experimental: `RaiseException` continuable con código privado,
   magic/nonce/fase; el VEH modifica `ContextRecord` para arm/disarm. La
   configuración se acepta sólo después del mismo readback independiente
   y calibración real. Si el sistema no la aplica, el banco termina sin
   ejecutar casos ni admitir evidencia. No hacer fallback que borre gates.

**No usar GetThreadContext(GetCurrentThread()) como prueba.** Microsoft
advierte que puede devolver éxito con contexto inválido, y exige suspensión
para capturar un hilo en ejecución. Asimismo SetThreadContext sobre un hilo
corriendo tiene resultado impredecible. El helper sólo suspende el hilo
del banco y siempre deshace exactamente su incremento del suspend count;
un count previo no cero se considera entorno incompatible.
[GetThreadContext](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadcontext),
[SetThreadContext](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadcontext).

## Configuración y conservación del contexto

- Compilación x86 nativa del proceso GTA, incluso sobre Windows64/WOW64.
  El auxiliar del mismo proceso usa CONTEXT x86; no confundir con un
  controlador externo x64, que requiere las APIs Wow64 específicas.
- Capturar DR0/1/2/3/6/7 mediante el helper antes de modificar. Rechazar
  breakpoints habilitados, TF, GD o depurador que consuma las excepciones.
  No apropiarse de slots ni eliminar la configuración de otro depurador.
- Configurar DR0/1/2 con las tres direcciones; L0/L1/L2=1, G0/G1/G2=0,
  RW0/1/2=00 y LEN0/1/2=00. Preservar los campos ajenos/reservados y DR3.
  Los bits locales de habilitación corresponden a máscara 0x15; no escribir
  simplemente DR7=0x15 ignorando el snapshot y normalización del sistema.
- El VEH acepta sólo el hilo propietario, los tres EIP concretos, Dr7
  comprobado y el único bit Bn esperado de Dr6. Un evento con BS/BD/BT/B3
  u otro EIP pasa a la cadena ajena y marca conflicto; no consumir
  indiscriminadamente cualquier SINGLE_STEP. Limpiar sólo el estado de
  breakpoint propio que se ha atendido y conservar lo demás. Registrar
  antes/después de Dr6/Dr7 para revisar la política exacta.
- En un hit propio sin scope activo se debe reanudar con RF y no registrarlo
  como llamada del target; devolver SEARCH causaría un breakpoint sin
  consumidor después de una llamada legítima del hilo. Mantener contador
  de hits fuera de scope y declararlo. Nunca contar housekeeping como target.
- Al finalizar, cerrar scope, desarmar/restaurar todos los Dr* vía helper,
  verificar readback completo y entonces retirar únicamente el token VEH
  propio. Comprobar [RemoveVectoredExceptionHandler](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-removevectoredexceptionhandler)
  no cero; handles/events/hilo auxiliar cerrados y suspend count restituido.
  No descargar una DLL mientras quede un callback registrado.

Un control software privado debe usar código distinto de las excepciones del
runner, bit28 reservado en cero, flags continuables y parámetros que incluyan
magic, nonce y fase válida. No se atiende una excepción de código parecido
sin esos campos. Microsoft documenta el código y argumentos privados en
[RaiseException](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-raiseexception).

## Handler mínimo y scope

Crear antes de armar los recursos y un buffer fijo POD de, por ejemplo,
16 eventos por invocación. Sin JSON, vectores, strings, malloc/new/free,
printf, locks, Pump, llamadas RH ni callbacks del juego dentro del handler.
Microsoft desaconseja explícitamente asignaciones y sincronización en
[PVECTORED_EXCEPTION_HANDLER](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nc-winnt-pvectored_exception_handler).

El estado lleva ownerThreadId, generación/serial del caso, función, lado,
scopeActive, contador limitado y latch de fallo. Cambiar scope inmediatamente
antes/después del **Gate** que invoca el target; ninguna preparación,
limpieza, lectura JSON ni cambio de RH queda dentro. El handler lee sólo
CONTEXT y palabras de stack validadas contra los límites comprometidos
previamente conocidos del stack del propietario. Verificar ESP y ESP+12
sin overflow. Stack inválido, reentrada o capacidad agotada no se normalizan.
Los registros conservan EIP/ESP/retorno/argumentos crudos, hilo, serial,
Dr6/Dr7 y número de secuencia.

El código entero del handler no debe alterar FPU/CONTEXT ajeno. Contrastar
registros generales, segmentos y campos FP del CONTEXT antes/después,
permitiendo sólo RF y los Dr* propios. Preservar LastError/errno donde sea
observable. Fuera del handler, el banco sigue capturando/restaurando FPU108,
MXCSR, ESP/no volátiles y los recursos como antes. Verificar CRT auténtico
completo al comienzo, antes/después de ambos lados y al final.

## Contrato dinámico exacto

Todos los eventos siguientes son obligatorios, ordenados y sin extras.
Son inferidos del CRT auténtico completo registrado en CrtProof.hpp,
especialmente realloc: NULL llama malloc y puntero no NULL con size0 llama
free. Deben volver a contrastarse con el EXE canónico antes de implementar.

| Target / caso | Secuencia esperada dentro del Gate |
| --- | --- |
| Malloc(size,hint) | malloc(size) |
| Realloc(NULL,size,hint), incluido size0 | realloc(NULL,size), malloc(size) |
| Realloc(p,0,hint), p no NULL | realloc(p,0), free(p) |
| Realloc(p,size,hint), p no NULL y size>0 | realloc(p,size) |
| MallocAlign(size,align,hint) | malloc(size+align), suma uint32 en dominio sin overflow |
| FreeAlign(aligned) | free(base), base leído de aligned-4 **antes** del target |

Las direcciones de recursos difieren entre lados. Registrar los argumentos
crudos y contrastarlos con la identidad **real de ese lado**: input_p y base
capturados antes de invocar. Sólo después producir roles normalizados
`null`/`input`/`base`. No excluir un puntero arbitrario por ser un puntero
del heap, ni comparar literalmente los bloques de lados diferentes.
Los tamaños jamás se normalizan ni se infieren de `_msize`.

Mantener código candidato completo estable, módulo ASI correcto, ruta RH
ON/OFF y CRT íntegro. Registrar direcciones de retorno y resolver su origen
contra las regiones declaradas en el build manifest. Realloc→malloc/free
debe devolver a su cuerpo CRT auténtico. Las llamadas directas deben venir
del cuerpo target o de un tail-call acreditado cuyo retorno sea el Gate.
La prueba de no delegar el target original se hace inspeccionando source,
xrefs y cuerpo compilado de la candidata; no exige igualdad de sus
instrucciones, y no desaparece por tener los tamaños correctos.

`trace_contract.py` contiene un modelo offline del contrato semántico y
`test_trace_contract.py` controles negativos. No es un observador hardware
ni un analizador de validación completo; no debe usarse para registrar un banco.

## Fallos y seguridad de la prueba

Un argumento incorrecto se rechaza **antes de Fill/payload/cleanup que
dependa del tamaño**. Existe un riesgo más temprano: MallocAlign escribe
su backpointer dentro del propio target, por lo que una petición demasiado
pequeña observada en el CRT no puede dejarse continuar alegremente para
validarla después. Esta condición requiere una salida de fallo probada.

No corregir el argumento ni sustituir el allocator, ni inyectar un éxito o
puntero NULL artificial para convertir una candidata errónea en pasante.
Opciones a estudiar: devolver SEARCH para que un filtro SEH externo
reconozca exactamente el breakpoint propio rechazado y aborte la llamada
mediante unwind, con limpieza fuera del VEH; o una terminación controlada
de la copia de pruebas después de registrar fallo fuera del handler. La
compatibilidad del unwind con la pila de los targets/CRT debe probarse
primero en un control negativo seguro. Si no se demuestra salida segura,
la instrumentación queda **bloqueada**, no se debilita el contrato.

El filtro SEH no debe aceptar SINGLE_STEP ajenos. Tiene que enlazar código,
ownerThreadId, latch/serial y EIP del rechazo del VEH. Esto sólo se admite
para un scope de target sin propiedad de recursos internos desconocidos;
en estos wrappers el primer allocator aún no ejecutó su primera instrucción.
Fixtures canary/input ya adquiridos se liberan fuera del scope, y el caso
queda fallido aunque el proceso logre restaurar todo. No llamar funciones
de logging/asignación dentro del VEH para manejar este error.

OOM, fallos excepcionales del CRT, tamaños enormes/overflow, alineaciones
inválidas, debugger invasivo y pila no válida quedan fuera de la campaña
inicial. Toda carencia de observer/configuración/missing-hit hace `error`
con cero aprobación, nunca cae al banco viejo sin gate.

## Calibración y evidencia obligatorias antes del banco C++

1. Prueba independiente de arm/readback/disarm/readback, en el proceso y
   arquitectura reales; negativos con slot ocupado/TF/fallo API y timeout.
2. Con las rutas RH del target fuera del scope, llamadas auténticas conocidas:
   malloc(33), realloc(NULL,17), realloc(p,0), realloc(p,65), free(p).
   Verificar exactamente los argumentos/eventos esperados, y liberar todas
   las reservas. OOM pequeño es fallo, no fixture aprobado.
3. Calibración con scope cerrado y llamada en otro hilo: ningún evento
   atribuible al target. Confirmar aislamiento del hilo y consumo correcto
   de hits propios fuera de scope, sin callbacks recursivos ni locks.
4. Control negativo seguro que pase petición incorrecta al allocator bajo
   scope: demostrar rechazo antes de ejecutar la reserva y unwind/limpieza.
   No introducir ese control en producción ni simular una función original.
5. Mantener las cuatro funciones ×2048 pares históricas, originales/candidatos
   no NULL, payload/canarios/alineación/backpointer/realloc-prefix/ABI,
   recursos balanceados, código CRT íntegro, rutas y estado FP restaurados.
   Añadir gates observer_ready/armed_readback/trace_complete/args_exact/
   observer_scope_ok/debug_regs_restored/veh_removed/worker_closed.
6. El analizador valida registros crudos y roles por lado, casos/seriales
   únicos, eventos completos y sin excedentes, cuerpo correcto y restauración
   independiente. Cerrar handles/VEH/debugregs también en preflight y errores.
   No marcar complete sin todos esos cierres ni con latch negativo.

La campaña genera evidencia **nueva** en un banco distinto; no reescribe
instrumentos ni manifiestos aprobados de DependenciesMemory9. Que una
implementación C++ pase este diseño futuro no está demostrado todavía.
