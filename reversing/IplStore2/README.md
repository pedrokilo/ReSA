<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CIplStore: carga y borrado (8 funciones)

Validadas el 02/10/2026 con **3.008 pares original/candidato dentro de GTA**, todos exactos: `Initialise`, `Shutdown`, `LoadIpl`, `LoadIplBoundingBox`, `LoadIpls`, `RemoveIpl`, `RemoveIplSlot` y `SetupRelatedIpls`, con `validated=1`. Con [IplStore](../IplStore/README.md), `CIplStore` queda en 34/34.

**Diferencias reales corregidas:**
- `RemoveIpl`: recorre edificios y dummies de `first` a `last` **incluido** (la DLL excluía el último).
- `LoadIpls`:
  - de la limpieza de misión solo tiene en cuenta los vehículos SIMPLE, PHYSICS o ABANDONED (la DLL hacía justo lo contrario);
  - un handle inválido se salta (la DLL hacía `assert`);
  - el bucle final va en orden ascendente.
- `LoadIpl` y `LoadIplBoundingBox`:
  - los recuentos binarios son de 16 bits y los desplazamientos se reubican escribiendo en el propio buffer;
  - `LoadIpl` marca `loaded` al final, así que la copia que recibe `CColAccel::setIplDef` aún tiene el valor anterior.
- `Shutdown`:
  - libera los arrays de entidades con `delete[]`;
  - destruye el pool como el original, poniendo a cero almacenamiento, mapa, capacidad y primer libre, en lugar de con el destructor de `CPool`.

**Método:**
- Los pools de edificios, objetos y dummies se sustituyen por pools propios con entidades falsas (vtable propia). Las entidades devueltas por `LoadObjectInstance` viven en el pool falso de edificios, porque `IncludeEntity` exige que estén en él.
- Stubs trazados para:
  - el quadtree: `AddItem` y `DeleteItem`, en el EXE y en la instancia `CQuadTreeNode<IplDef*>` de la DLL;
  - `operator new/delete`, con un asignador lineal y una traza sin reservas;
  - `LoadObjectInstance`, `LoadCarGenerator`, `CWorld::Add/Remove`, `RemoveCarGenerators`, `CColAccel` y el streaming.
- Buffers IPL de texto y binarios generados por caso.
- `Shutdown` usa un nodo real del pool de nodos del quadtree y un pool de IPL propio. El pool de IPL real se restaura tras cada ejecución.

Repetir: `python reversing/IplStore2/build_probe.py` y `python reversing/run.py --case ipl2`. Evidencia: [validation-summary.json](validation-summary.json).
