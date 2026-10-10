<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
