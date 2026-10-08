<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reproducción y corrección Sunbathe; propuesta CTask no aplicada

El replay 20261008-115720-657111 produjo1536pares y restauró banco+harness. Boatctor256 yHitctor256 sin diferencias; Boat/Hit/Sundtor256 cadauno fallaronvptrterminal, Sunctor256 fallóordenqueries. Logs originales permanecen completos; reproduction-and-patch.json anclaEXE/ASI/loghash yfuente corregida.

Sun parcheado permanentemente conexactamente2reordenamientos: LoadAnim hace GetAnimationBlock antesGetAnimationBlockIndex, siguiendo EXE631FEF/631FFC y632009/632016; destructor hace SafeCleanUpRef antes objectType=OBJECT_TEMPORARY siguiendo632087/63208E. Conentitypresente seed1 la fuenteanterior emitía cleanupobservandotype3, original5. No compile ni re-ejecucióntraspatch ysinflagsvalidaciónnuevos.

## Causa común acreditada y propuesta

Task.h declara virtual~CTask()=default. El replay del buildDebug dejó vptrCTaskComplex en candidato, mientras EXE dejóCTask. No es una hipótesis de normalización: ambasdirecciones se reconocen separadamente y elanalizador exigeCTask final. Losbytes payload/trace restantes Boat/Hit coinciden.

EXE CTaskComplex destructor61A3D0..61A424 escribe86D4CC alentrar, borra subtaskdespuésdeletingvirtualslot0, escribe86D48C en61A413 alterminar. EXE61A660..61A697 es SCALAR-DELETING destructorCTask (ret4), no simplemente destructor sinargs: en61A668 escribe86D48C antes branchflag1 que devuelve slot al taskpoolB744A8. No enlazar61A660 como Fnvoid(this) ni ejecutar branchfree con memoria privada.

Propuesta SINAPLICAR: Task.h virtual~CTask(); y Task.cpp definición outofline CTask::~CTask(){}. Se espera que una definición propia fuerce código de destructorbase con storevfptrCTask enMSVC y permita llamada antesfinalizarCTaskComplex. Esto es hipótesis práctica y NO garantía del estándar: virtualdefaulted no equivale a destructortrivial y escribir vptrpostlifetime no es contratoportableC++.

El proyectoDebug tiene InlineFunctionExpansionDisabled yOptimizationDisabled en vcxprojactual. Aún así falta comprobarASM del candidato de nueva definición, retorno, callorder, stack yscalar-deletingfree. No tocarflags ni aceptarCTaskComplex comoCTask. Si cuerpo vacíooutofline no emite exactostore, estudiar soluciónMSVCABIacotada basadaen evidencia; no asumir que cambiósemántica correcta por compilar.

## Verificación necesaria y alcance

CambiarTask.h afecta todas tareas ymanifiestos de bancos que incluyaneseheader; congelarfuentes/instrumentos nuevos yretenerreproducciónfallida. CompilarMP6serial despuéscoordinación; inspeccionar destructorbase candidate antesreplay. Repetir896/192/1536/Additional con mismosgates de vptr yrestauración. Destructores consubtaskchild privado verificanbranchvirtualcleanup y deletepropioCRT; referencia/animación son contratosstub. Ningúnresultado aquí demuestra recuperacióndetaskpool en integración o standalone.
