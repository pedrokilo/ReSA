<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Procesado de colisión de `CCollision` (lista de 59b)

Banco escrito el 05/10/2026 para **13 funciones de `CCollision`** (`.cpp` revisado contra el EXE 1.0 US, sin `FIX_BUGS`). **Pendiente de compilar y ejecutar**: se prepararon 8.192 pares original/candidato dentro de GTA, ninguno corrido todavía.

| Función | Dirección | Casos |
|---|---|---|
| `ProcessSphereBox` | 0x412130 | 640 |
| `ProcessLineSphere` | 0x412AA0 | 512 |
| `ProcessLineBox` | 0x413100 | 640 |
| `ProcessDiscCollision` | 0x413960 | 512 |
| `ProcessLineTriangle` | 0x4140F0 | 640 |
| `ProcessVerticalLineTriangle` | 0x4147E0 | 640 |
| `IsStoredPolyStillValidVerticalLine` | 0x414D70 | 512 |
| `CalculateTrianglePlanes-colData` | 0x416330 | 256 |
| `RemoveTrianglePlanes-colData` | 0x416400 | 256 |
| `ProcessSphereSphere` | 0x416450 | 512 |
| `ProcessSphereTriangle` | 0x416BA0 | 1024 |
| `ProcessVerticalLine` | 0x417BF0 | 512 |
| `ProcessColModels` | 0x4185C0 | 1536 |

**Diferencias reales corregidas (evidencia en el desensamblado):**
- Reglas comunes: el EXE suma en orden x87 distinto del de C++ (`z, x, y` en normas y productos), multiplica por el inverso en `operator/` (0x4119D0), no limita raíces con `max(0, ...)` y compara con `test ah,41h/5` (NaN pasa). Las transformaciones usan `MultiplyMatrixWithVector` (0x59C890) y `Multiply3x3` (0x59C790), con su propio orden de sumas (ayudantes en el espacio `ColProc59b` de `Collision.cpp`). `CColPoint::operator=` del EXE (0x40FC80) no copia `field_C`, `field_1C` ni el relleno 0x26.
- `ProcessSphereBox`: con la distancia igual al radio **sí** hay colisión (solo descarta `dist > radio`); la normal es `dir * (1/dist)`; copia también la pieza (3 bytes) de cada superficie.
- `ProcessLineSphere`: reescrita. No hay atajo "origen dentro", la discriminante usa `m = centro - origen` con sumas en orden z/y/x (`b = -(m·d)`), descarta `t < 0` y `t > 1`, no escribe la iluminación A ni comprueba ajustes de depuración; el punto es `start + d * t`.
- `ProcessLineBox`: reescrita. Origen estrictamente dentro => `CalculateColPointInsideBox`, distancia 0 y `+1` cruce si el final queda fuera; en el resto `t = A / (A - B)` (distancias firmadas, no `(e - s)`), contador de cruces `+1`/`+2` según el final y sin escribir la iluminación A.
- `ProcessDiscCollision`: productos escalares en orden y/z/x, raíz sin `max(0, ...)` y copias con el `operator=` del EXE.
- `ProcessLineTriangle` y `ProcessVerticalLineTriangle`: reescritas desde cero. El EXE no usa la ruta "vanilla" de la DLL: `t = ((D - n.x*s.x) - n.y*s.y) - n.z*s.z) / ((dz*n.z + dy*n.y) + dx*n.x)` (la vertical usa `n.z * dz`), prueba 2D con las tres aristas (`R1 >= 0`, `R2 <= 0`, `R3 >= 0`), `CStoredCollPoly` con solo vértices, `valid` y el dword de iluminación en `+0x28`, y la vertical tiene un rectángulo envolvente asimétrico respecto a `v0`.
- `IsStoredPolyStillValidVerticalLine`: el EXE normaliza `(v2 - v0) x (v1 - v0)` (no usa `CColTrianglePlane`), el segundo parámetro es la **z final** de la línea (no un factor) y invalida `valid` al fallar la prueba de lado o la 2D.
- `ProcessSphereSphere`: sin la salida temprana por `distSq`, `dist = sqrt((dz² + dx²) + dy²) - rB`, profundidad calculada **antes** de limitar a 0.
- `ProcessSphereTriangle`: reescrita desde cero (base ortogonal `vec2 = (B - A)/|B - A|`, `vec1 = vec2 x n`, tres semiplanos y regiones de vértice/arista/interior); no escribe la iluminación B.
- `ProcessVerticalLine`: el EXE pasa la línea al espacio del objeto como `(v - pos)·ejes` y **no** fuerza que sea vertical; **salta** las superficies que se ven a través (la DLL hacía lo contrario); copia 0x2C bytes al `CStoredCollPoly` de salida.
- `ProcessColModels`: reescrita siguiendo el flujo del EXE. Entre otras cosas:
  - los grupos de caras se recorren desde `tris - 0x20` hacia abajo (la DLL los recorría al revés);
  - el tope de triángulos es 599 (0x257), no 600;
  - los índices de discos de B se escriben en `collSphB` desde el principio y no suman a `numCollSphB`;
  - el bloque final B contra A solo arma la profundidad `-1` si hay triángulos de A que chocan, no limita antes de procesar una caja (solo después), y excluye de los bucles `numDiscosB`;
  - en la ruta de discos copia la superficie del disco al punto emitido, no limita `hitK` a 0 y las esferas de B salen siempre de `m_pSpheres`.

**Método:**
- Cada pareja usa la misma semilla; el original corre con los hooks de las 13 funciones desactivados y el candidato con ellos activados (el cambio se comprueba tras `State()`).
- Todo vive en una arena estática: esferas, cajas, líneas, discos, vértices comprimidos, triángulos con y sin grupos de caras, `CColModel`/`CCollisionData` en bruto y matrices de rotación aleatorias. Los planos de triángulo los calcula el propio EXE (`CColTrianglePlane::Set`, 0x411660) antes de cada llamada.
- La caché de planos (`CCollision::ms_colModelCache`, 0x96592C) se sustituye por una lista falsa de 3 a 8 enlaces (nunca se mezclan modelos reales); se guarda entera (0x34 bytes) y se restaura al final. Los planos reservados se liberan al terminar cada ejecución.
- Se comparan el valor devuelto, los `CColPoint` completos (también lo que el EXE deja sin escribir, sobre un relleno 0xCD), las distancias y banderas de entrada/salida, el `CStoredCollPoly`, `ms_iProcessLineNumCrossings` y una firma de cada modelo (arrays, cabecera y planos con el puntero enmascarado y el índice del enlace).
- `Calculate/RemoveTrianglePlanes` corren secuencias de 6 a 16 operaciones sobre 2 a 6 modelos y comparan cache, enlaces y planos tras cada una.
- No hay parches de entrada ni simulaciones: estas funciones solo llaman a cálculo, a `CMemoryMgr::Malloc/Free` y a `IsSeeThrough` (tabla real, solo lectura).

**Fuera de los fixtures** (comportamiento indefinido o basura del EXE):
- `ProcessLineBox` con `maxTouchDistance > 1` cuando el origen queda fuera y ningún plano acierta (el EXE devuelve verdadero con punto y normal sin inicializar);
- orientaciones de plano mayores que 5, triángulos degenerados, NaN en entradas;
- modelos con más de 128 esferas, 64 cajas o 16 líneas (desbordarían los arrays estáticos del EXE);
- los estáticos de trabajo del EXE (`0x965A30`...`0x968A00`), que no se restauran porque son zona de trabajo.

**Dependencias del banco:** `ProcessVerticalLine` y `ProcessColModels` llaman por la DLL a `TestLineBox`, `TestSphereBox`, `TestSphereTriangle` y `CalculateColPointInsideBox`; si alguna de ellas difiere del EXE el fallo aparecerá aquí.

Repetir: `python reversing/ColProc59b/build_probe.py` y `python reversing/ColProc59b/run_local.py` (solo desde la regresión; lanzan el juego).
