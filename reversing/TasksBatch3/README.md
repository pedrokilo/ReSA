<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas 3: UseGoggles, UseAttractor, UseAttractorPartner, UseEffect y UsePairedAttractor

Validadas el 02/10/2026 con **19.168 pares original/candidato dentro de GTA**, todos exactos: 38 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CPedAttractor` (afecta a todo el juego): en la vtable del EXE (0x86C538), la ranura 0 es `GetType` y la 1 el destructor. La DLL declaraba antes el destructor, así que `GetType` llamado desde la DLL sobre atractores del EXE caía en otra ranura.
- Hook de `CGeneral::GetRandomNumberInRange_int32` (0x407180): usaba la plantilla entera (puede salir desfasada en uno). Ahora usa `GetRandomIntInRange`, que es la fórmula del EXE.
- `UseAttractor::CreateFirstSubTask`: faltaban el evento interesante 3 al empezar y el borrado de `bUseAttractorInstantly` al final. El tiempo de sentarse es `GetRandomIntInRange(0,100) > 40 ? 9999999 : GetRandomIntInRange(15000,60000)`, y se sienta según `bUseAttractorInstantly` (no `bStayInSamePlace`). En el script de atractor, un objeto sin atractores habilitados no arranca el script, pero la tarea se crea igual. Un tipo desconocido no aborta.
- `UseAttractor::CreateNextSubTask`: el dinero de la pizza se compara como sin signo y el del cajero usa `GetRandomIntInRange`. `ControlSubTask` aborta solo si la entidad es un objeto **y** está rota (la fuente tenía `||`).
- `UseEffect::MakeAbortable`: da de baja al peatón en el gestor de atractores. Las distancias se comparan con `<` 2,25, y se mira el estado de movimiento de la víctima, no el de la tarea. `CreateFirstSubTask` registra si hay entidad **o** el efecto es guionizado. `ControlSubTask` da de baja al `ped` recibido y llama dos veces a `FindPlayerWanted`.
- `UsePairedAttractor`: `m_Partner` es un puntero crudo con la referencia gestionada a mano, porque el destructor original no la limpia. `SeekPartnership` recorre siempre los 4 pares. `ControlSubTask` aborta salvo que el compañero tenga el atractor emparejado y además use o espere al compañero, y aborta también tras limpiar la pareja. `CreateSubTask` conserva la búsqueda sin usar del original. Los tipos desconocidos devuelven la subtarea o `nullptr` en vez de `NOTSA_UNREACHABLE`, y `ms_useAgainFlags` se indexa sin comprobación (índice -1 como el original).

**Método:** peatones con inteligencia y datos de jugador falsos; atractor, evento y subtarea con vtables propias; pares de efectos falsos. El gestor de atractores, `CScripted2dEffects`, `CScriptsForBrains`, `CScriptedBrainTaskStore`, los buscadores de tareas, las gafas, el audio, el buscado, los eventos interesantes y el RNG se simulan y se trazan. Los constructores de las subtareas creadas son los reales. Durante el banco, los 64 efectos guionizados se marcan como atractores (la DLL lo comprueba con `cast<>` en depuración).

Repetir: `python reversing/TasksBatch3/build_probe.py` y `python reversing/run.py --case tasks3`. Evidencia: [validation-summary.json](validation-summary.json).
