<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEntryExitManager::Init como el EXE

Validadas el 04/10/2026 con **2.048 pares original/candidato dentro de GTA**, todos exactos: 3 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEntryExitManager` | `Init`, `Save` (vuelta a cubrir), `Shutdown` (vuelta a cubrir) |

**Diferencias reales corregidas:**
- `Init` (decisión del usuario: igual que el EXE):
  - pool de **400** entradas (antes 512, ampliación NOTSA);
  - el pool se construye como 0x43F720: `operator new` de 0x14 bytes, objetos y mapa con `new[]`, **sin relleno 0xFD** del almacenamiento, cada byte del mapa en 0x80, `m_LastFreeSlot` = -1; después el byte +0x11 a 1;
  - el nodo raíz del árbol solo se construye si el pool de nodos devuelve memoria (si está lleno el árbol queda nulo).
- `Save`:
  - recorre **400 slots fijos** (no la capacidad del pool);
  - índices crudos `(p - base) / 0x3C` con signo, sin los `assert` de `CPool::GetIndex` (pila con punteros nulos o ajenos al pool);
  - el enlace se guarda si su índice está en `[0, capacidad)` y el slot está ocupado (un puntero desalineado o justo antes del pool se trunca como en el EXE).
- `Shutdown` destruye el pool en línea como el EXE (sin `~CPool`): sin relleno, almacenamiento, mapa, capacidad y primer libre a cero, y solo si la capacidad es positiva.

**Método:**
- `Init`:
  - los globales del gestor (pila, árbol, pool, estados) se rellenan con basura antes de la llamada y se restauran;
  - `operator new`/`new[]` (0x82119A/0x821195) se parchean **solo durante la llamada** con una arena fija rellena con un patrón por caso; las trazas van a un array fijo (sin reservas); a veces el `new[]` del almacenamiento devuelve nulo;
  - el nodo raíz sale del pool real de nodos del árbol con `m_LastFreeSlot` elegido por caso (y a veces con el pool lleno); se guardan y restauran su almacenamiento, su mapa y `m_LastFreeSlot`;
  - se comparan las reservas, la cabecera del pool, la arena completa, el nodo raíz, el mapa de nodos y los globales.
- `Save` y `Shutdown` usan los fixtures de World18 (pool falso de 400 entradas, árbol con nodos reales, `SaveDataToWorkBuffer` y `operator delete/delete[]` simulados):
  - `Save` con pila de -1 a 6 entradas, punteros nulos, desalineados, antes o después del pool, y capacidad menor que 400;
  - `Shutdown` compara además la cabecera del pool tras la destrucción (no se libera de verdad) y a veces con capacidad 0.

Repetir: `python reversing/EnExInit/build_probe.py` y `python reversing/run.py --case enexinit`. Evidencia: [validation-summary.json](validation-summary.json).
