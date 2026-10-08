<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Scripts en streaming, Game y árbol cuaternario (lista de 12)

Validadas el 02/10/2026 con **12.320 pares original/candidato dentro de GTA**, todos exactos: 34 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CStreamedScripts` | `Initialise`, `ReInitialise`, `FindStreamedScript`, `FindStreamedScriptQuiet`, `GetProperIndexFromIndexUsedByScript`, `GetStreamedScriptFilename`, `GetStreamedScriptWithThisStartAddress`, `RegisterScript`, `RemoveStreamedScriptFromMemory`, `LoadStreamedScript`, `StartNewStreamedScript`, `ReadStreamedScriptData` |
| `CQuadTreeNode<T>` | `InitPool`, `FindSector` (rectángulo y punto), `InSector`, `AddItem`, `DeleteItem` (simple y con rectángulo), `GetAll`, `GetAllMatching` (rectángulo y punto), `ForAllMatching` (rectángulo y punto) |
| `Game` | `GameInit`, `RwInitialize`, `RwTerminate`, `InitialiseGame`, `TheGame`, `RenderMenus`, `RenderDebugShit`, `RenderEffects`, `RenderScene`, `Render2dStuff` |

**Diferencias reales corregidas:**
- `CStreamedScripts`:
  - `Initialise` solo escribe puntero, usuarios, índice, el primer byte del nombre y el tamaño de cada entrada, y además pone a cero el tamaño máximo y la cuenta;
  - `FindStreamedScriptQuiet` y `RegisterScript` leen la cuenta sin signo; `RegisterScript` usa `strcpy` y devuelve la cuenta nueva menos uno;
  - `GetProperIndexFromIndexUsedByScript` recorre las 82 entradas, no solo las registradas;
  - `GetStreamedScriptWithThisStartAddress` devuelve 82 si no lo encuentra (la DLL lo daba por inalcanzable);
  - `LoadStreamedScript` devuelve `bool` (falso si falla la versión);
  - `ReadStreamedScriptData` llega al bloque con tres saltos fijos, recorre tantas entradas como scripts registrados y un nombre no encontrado escribe en la entrada -1, como el EXE;
  - `RemoveStreamedScriptFromMemory` llama siempre a `operator delete[]`, también con puntero nulo.
- `CQuadTreeNode<T>`:
  - `FindSector` (las dos) e `InSector` siguen las comparaciones del EXE con NaN;
  - `DeleteItem` con rectángulo baja con el mismo rectángulo, sin comprobar que el hijo exista;
  - `InitPool` crea el pool en línea con `operator new`, sin rellenar el almacenamiento; los nodos usan la política original del pool (busca desde el hueco siguiente, sin rellenos);
  - `CPtrNodeSingleLinkPool::Delete` (compartido) ya no rellena el nodo liberado con 0xDD.
- `Game`:
  - `GameInit` lleva `RwInitialize` en línea y no comprueba el resultado (sin `VERIFY`);
  - `RenderScene`: con `UnderWaterness` NaN el agua se dibuja pronto; con la Z frontal NaN se usa -Z;
  - `Render2dStuff`: el marco del zoom usa mitades enteras de la resolución, los factores 1/448 y 1/640 del EXE en su orden y sin corrección de aspecto; ya no llama a `RenderDebugShit`;
  - `RenderDebugShit` está vacía en el EXE;
  - `Idle` (sin banco) dibuja los créditos dos veces, como el EXE; `Idle` y `FrontendIdle` (sin banco) calculan la ventana de vista con `fptan`.

**Método:**
- `CStreamedScripts` trabaja sobre un objeto falso con 0x20 bytes delante para ver las escrituras en la entrada -1; `ReadStreamedScriptData` lee la cabecera real del SCM cargado (solo lectura).
- Se simulan `CheckStreamedScriptVersion`, `RwStreamRead`, `StartNewScript`, `SetMissionDoesntRequireModel` y `operator new[]`/`delete[]` (solo durante la llamada).
- `CQuadTreeNode` trabaja sobre pools falsos de nodos del árbol y de lista; los punteros globales de los pools se cambian solo durante la llamada.
- Las funciones de `Game` son orquestación: se interceptan todas sus dependencias y se compara la secuencia de llamadas, argumentos y estados de render. Escena, cámara, espejos, agua, pads, réplica, menú, resolución y gamma se fijan y se restauran.
- `Render2dStuff` tiene además un `HookInstall` permanente (ImGui) al mismo destino: desactivar su hook reversible crea un bucle EXE↔DLL. Su hook se deja activo y el original se ejecuta reponiendo durante la llamada los 5 bytes originales de la entrada del EXE.
- Fuera de los fixtures:
  - `Idle` y `FrontendIdle` (alojan el propio banco y llevan añadidos NOTSA de ImGui, ratón y carga rápida; `Idle` tampoco limita los FPS);
  - el arma sin jugador en `Render2dStuff` (el EXE lee la pila sin inicializar);
  - punteros de script no encontrados con nombres sin terminar, índices fuera de rango y sectores mayores que 3;
  - `printf` del EXE en `TheGame` (la DLL usa su log) y `DebugModules::Render3D` (lo único que hace `UIRenderer::Render3D`, NOTSA), sin traza.

Repetir: `python reversing/Sys12/build_probe.py` y `python reversing/run.py --case sys12`. Evidencia: [validation-summary.json](validation-summary.json).
