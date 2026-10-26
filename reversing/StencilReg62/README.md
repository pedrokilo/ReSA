<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco 62: CStencilShadows::RegisterStencilShadows

Fecha: 06/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `build_probe.py` y `run_local.py`).

Compara dentro del motor el EXE original (hooks de `RegisterStencilShadows` y de `CStencilShadowObject::Destroy` desactivados)
contra el candidato (activos) para `RegisterStencilShadows` (0x711760), que el banco 61b dejó fuera por recorrer el pool de
vehículos y los sectores reales del mundo. Aquí se aíslan con fixtures sin dejar de ejecutar el cuerpo real.

## Revisión contra el EXE (corregido)

* Todo el cuerpo coincide con 0x711760..0x711D89: limpieza (dueño nulo; objetos con bit 7 de +0x1C clear o bits 6/10 de +0x140;
  tipo 1 = objeto: distancia 0x711160 y área 0xB72914/13; tipo 2 = vehículo: 0x7111F0), pool de vehículos (stride 0xA18, +0xB0,
  bit 2 de colData+7, búsqueda de dueño, Create(…, 2)), ventana `floor(v·0.02+60)` con recortes 0 / 0x77, scan code (0xB7CD78,
  ClearScanCodes a 0xFFFF), listas Buildings (0xB7D0B8) y Objects de sector repetido (0xB992C0, `& 0xF`) y Create(…, 1).
* **Diferencia corregida:** `StencilSignedDistSq` calculaba el centro de la esfera con `CEntity::TransformFromObjectSpace` (DLL:
  `CMatrix::TransformPoint`, suma `x·right + y·forward + z·up + pos`); el EXE (0x5334F0) usa 0x59C890 (orden `up, forward, right,
  posición`) con matriz y 0x54ECE0 (`fcos`/`fsin` x87 completos) sin ella. Ahora `StencilTransformFromObjectSpace` usa
  `StencilMulPoint` y llama a 0x54ECE0 del EXE. Las dos funciones de distancia del EXE (0x711160 y 0x7111F0) son idénticas.

## Fixtures

* **Pool de vehículos:** el puntero global 0xB74494 se cambia por una cabecera falsa de `CPool` (0x14 bytes) con 0..24 vehículos
  falsos de 0xA18 bytes y ranuras libres/ocupadas al azar; se restaura al acabar cada ejecución.
* **Sectores del mundo:** las cabezas `Buildings` (sector) y `Objects` (sector repetido) de las 5x5 celdas de la ventana de la
  cámara se sustituyen por nodos propios (el código los lee crudos, sin validar contra el pool de nodos) y se restauran; se guardan,
  comparan y restauran además los 115.200 bytes de `ms_aSectors` y los 3.072 de `ms_aRepeatSectors`.
* **Entidades falsas:** 40 generales (objeto, edificio, dummy, vehículo, peatón, otro) y 24 vehículos del pool; con matriz propia o
  con `CSimpleTransform`; visibilidad, `bIsProcObject`, explotado/roto, área, scan code (a veces igual al nuevo), índice de modelo.
  Un 30 % se coloca justo en el límite de 50 unidades a la superficie de la esfera (± ulps) para detectar diferencias de orden de
  sumas o de precisión.
* **Modelos:** índices 19990..19999 de `ms_modelInfoPtrs` sustituidos por `CBaseModelInfo` falsos (vtable con `GetModelType`
  trazado y trampa para cualquier otra ranura), modelos de colisión y `CCollisionData` falsos (esfera, `bHasShadowInfo`, datos
  nulos) y `CVehicle::m_aSpecialColModel` (0xC1CC78) rellenado.
* **Objetos de sombra:** el array real de 64 objetos (0xC6A198) y sus cabezas (0xC6A168/0xC6A16C) se reescriben en cada ejecución
  con 0..64 activos (muchos casos con la lista casi llena para ejercer el agotamiento y el retorno `false`), dueños nulos, tipos
  incoherentes, datos de sombra falsos y listas de referencias de los dueños (con `pEmptyList` 0xBA1768) para que `Destroy`
  ejecute `CleanUpOldReference` de verdad. Contadores 0xC6A898/0xC6A899, `currArea`, scan code, pool y modelos se guardan y restauran.

## Dobles trazados (sin reservar memoria; eventos en un array fijo)

Parchean la dirección del EXE y el cuerpo de la DLL (resuelto con `Body()` antes de parchear):
`CStencilShadowObject::Create` (0x711280: toma el primer objeto libre, fija dueño/tipo/caras/datos y lo enlaza el primero de los
activos; falla según una máscara por caso), `CWorld::ClearScanCodes` (0x563470) y `CMemoryMgr::Free` (0x72F430). `Destroy` (0x711310)
se ejecuta de verdad (EXE en el original, DLL en el candidato; su hook se conmuta junto con el de Register).

Se compara retorno (solo AL), traza completa, los 0x700 bytes del array de objetos con punteros traducidos, ambas cabezas, el
recorrido de la lista activa, scan code, `pEmptyList`, hash de entidades/vehículos/referencias, cabecera del pool y los sectores
reales. Antes de cada caso se escribe y vacía un evento `case_start`; cada `Write` llama a `PumpMessages`; el evento final lleva
`state_restored`. Antes de empezar se comprueban los desplazamientos de la DLL (+0xB0, +0x22, +0x48B, +0x140).

## Casos

`CStencilShadows::RegisterStencilShadows`: 1.024 (22 % con `doNotCreateNew`; cámaras en el interior, bordes del mundo, fuera del
mundo y en límites de celda).

## Fuera del banco

* `Create` real (0x711280) y su `malloc`/`RegisterReference`: se sustituye por el doble (ya validado con los demás de
  `CStencilShadowObject`); no se prueba que Register pase la entidad correcta más allá de la traza de argumentos.
* `ClearScanCodes` real (recorre las listas del mundo): doble trazado.
* Bordes con coordenadas de cámara no finitas (NaN/inf) o más allá de ±2^31 sectores: `_ftol2` del EXE y la conversión de la DLL no son
  comparables.

## Uso (no ejecutado)

`python reversing/StencilReg62/build_probe.py` y después `python reversing/StencilReg62/run_local.py`. Filtro opcional: fichero
`stencilreg62-only.txt` con `CStencilShadows::RegisterStencilShadows`.
