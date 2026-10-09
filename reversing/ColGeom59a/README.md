<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Geometría pura de CCollision (lista 59a)

Banco preparado el 05/10/2026: **19.712 pares original/candidato dentro de GTA** para 25 funciones de `CCollision` (pendiente de compilar y ejecutar). Los 59 hooks de la clase seguían bloqueados en `InjectHooks` («bad performance in debug mode»); las 25 funciones de este banco pasan a hooks normales.

| Grupo | Funciones |
|---|---|
| Pruebas de solapamiento | `TestSphereSphere`, `TestSphereBox`, `TestSphereTriangle`, `TestLineSphere`, `TestLineBox`, `TestLineBox_DW`, `TestVerticalLineBox`, `TestLineTriangle`, `Test2DLineAgainst2DLine` |
| Distancias | `DistToLine`, `DistToLineSqr`, `DistToMathematicalLine`, `DistToMathematicalLine2D`, `DistAlongLine2D`, `ClosestSquaredDistanceBetweenFiniteLines` |
| Punto en figura / ejes | `PointInTriangle`, `PointInPoly`, `GetPrincipleAxis` |
| Puntos más cercanos | `Closest3`, `ClosestPointOnLine`, `ClosestPointsOnPoly`, `ClosestPointOnPoly`, `RayPolyPOP` |
| Cajas y puntos de colisión | `GetBoundingBoxFromTwoSpheres`, `CalculateColPointInsideBox` |

**Diferencias reales corregidas** (todas contra el desensamblado del EXE 1.0 US; x87 a 24 bits = `float`, mismo orden de sumas):
- `TestSphereSphere` (0x411E70): d² = `(dz² + dx²) + dy²` y cierto solo si `(r1 + r2)² > d²` (la DLL usaba `<=` y otro orden de suma).
- `CalculateColPointInsideBox` (0x411EC0): el EXE solo escribe punto, normal (3 floats) y profundidad; la DLL ponía a cero todo el `CColPoint` (`colPoint = {}`). `NaN` en el signo cuenta como `<= 0`.
- `PointInTriangle` (0x412700): se transcriben los productos escalares con el orden del EXE (`z, x, y` en `v1·v1`, `z, y, x` en `v2·v2`, etc.); `NaN` pasa las pruebas.
- `DistToLineSqr` (0x412850):
  - el resultado se limita a +0 cuando no es positivo (la DLL devolvía valores negativos por redondeo);
  - tras el final del segmento se resta `pt - ln1` directamente (la DLL hacía `(pt - ln0) - (ln1 - ln0)`);
  - orden de sumas del EXE. `DistToLine` (0x417610) queda igual de exacta porque es la raíz de esa función.
- `DistToMathematicalLine` (0x412970): el EXE calcula `|p|² - (p·l)² / |l|²` (divide por la longitud **de la recta**); la DLL dividía por `|p|²` y usaba `pow`.
- `DistToMathematicalLine2D` (0x412A30): orden de sumas `(py² + px²) - dot²` y `NaN` llega a la raíz.
- `TestLineBox_DW` / `TestLineBox` (0x412C70 / 0x413070 = `jmp 0x412C70`): algoritmo distinto. El EXE calcula un código de salida de 6 bits con el **bit de signo** de las 12 diferencias (el signo de `-0.0` cuenta), descarta si comparten bit, acepta si un extremo es cero y solo cruza los planos marcados con `1/dx`, `1/dy`, `1/dz` y comparaciones estrictas. La DLL usaba `CBoundingBox` y productos de signo.
- `TestVerticalLineBox` (0x413080): comparaciones estrictas con `NaN` fallando en X/Y y menor/mayor de Z con `start.z < end.z`.
- `Test2DLineAgainst2DLine` (0x4138D0): `NaN` pasa cada prueba (`test ah,41h`), no se compara con `double`.
- `TestLineTriangle` (0x413AC0): reescrita. El EXE corta la recta con el plano (`t = ((d - n·s...) / (n·d)`), toma el punto en 2D eliminando el eje dominante según la orientación del plano (tabla de 6 casos; los impares intercambian B y C) y hace tres pruebas de lado con `<0`, `>0`, `<0`. La DLL usaba la versión Möller-Trumbore compartida con `ProcessLineTriangle`.
- `TestLineSphere` (0x417470): reescrita. Solo mira la raíz cercana de la cuadrática (`t` en `[0, 1]`, un inicio dentro de la esfera da falso), con `len` = `sqrt(...)`, `h = (s·d) * -2`, `s²` como `|c|² + |start|² - 2 c·start` y rama de recta casi nula (`len < 1e-6`). La DLL llamaba a `ProcessLineSphere_Internal`.
- `TestSphereTriangle` (0x4165B0): reescrita. El EXE mide la distancia al plano, proyecta sobre la base (arista AB normalizada, perpendicular en el plano), cuenta cuántas de las tres aristas contienen la proyección y devuelve la distancia al plano, a una arista o a un vértice `< radio`. La DLL tenía el algoritmo de Ericson. Se quita la comprobación de depuración `s_DebugSettings`.
- `ClosestSquaredDistanceBetweenFiniteLines` (0x415A40): la DLL usaba `ClosestPtSegmentSegment` (Ericson); el EXE es el algoritmo de Dan Sunday (recortes de `sN`/`tN`, `SMALL_NUM = 1e-5f`, `a` es el argumento y no se recalcula).
- `ClosestPointOnLine` (0x417FD0): **el primer argumento es el punto** y los dos siguientes la recta (la DLL los tenía como `l0, l1, point`); normaliza con `1/len`, proyecta y limita con `proj < 0` / `proj > len`. Cambia el orden de parámetros en `Collision.h`.
- `Closest3`, `ClosestPointsOnPoly`, `ClosestPointOnPoly`, `RayPolyPOP`: la DLL tenía `NOTSA_UNREACHABLE` («no usadas»), pero el EXE tiene el código (0x415950, 0x418100, 0x418150, 0x415620). Se transcriben; `Closest3` sustituye el punto por el vértice más cercano (empates por `<` estricto) y `ClosestPointOnPoly` lo hace con los 3 puntos más cercanos de las aristas.
- `GetPrincipleAxis` (0x4156D0): con `NaN` en Y/Z el EXE devuelve 4 (`ny > nz ? 2 : 4`).
- `PointInPoly` (0x415730): se transcriben las tres pruebas de lado por eje principal: los ejes X y Z comparan con `>= 0` y el eje Y con `<= 0` (la DLL usaba una prueba de signo distinta).
- `GetBoundingBoxFromTwoSpheres` (0x415230): solo cambia el tratamiento de `NaN` (el EXE elige `B` como menor cuando no es `A < B`); el EXE no escribe los bytes de material 0x18..0x1B del `CColBox`.

**Sin cambios (coinciden con el EXE):** `TestSphereBox` (0x4120C0) y `DistAlongLine2D` (0x412A80).

**Método:**
- Son funciones puras: se llaman por la dirección del EXE (sin hooks) y con el hook activo (el EXE salta a la DLL) sobre buffers propios; se conmutan los 25 hooks una vez por pasada y se comprueba con `JumpsToDll` que el salto está donde debe.
- Se comparan el valor devuelto (float de ST0 como bits, bool de AL, int de EAX; `NaN` canónico) y los buffers de salida a nivel de bytes con bytes de patrón detrás: `CColPoint` entero de 0x30 bytes (lo que el EXE no escribe debe quedar igual), `closest`, los 3 puntos de `ClosestPointsOnPoly` y el punto modificado de `Closest3`, `ClosestPointOnPoly` y `RayPolyPOP`; además se comprueba que las entradas no cambian.
- Generador determinista con modos de números: aleatorios, enteros pequeños (muchos empates exactos: tangentes, aristas, caras), medios, grandes (±1000), solo −1/0/1 y mezcla con ceros y `-0.0`.
- Casos límite por función: recta nula o casi nula (< 1e-6), paralelas y colineales, extremos sobre caras/aristas de la caja, recta que atraviesa por el centro, esfera tangente, esfera empezando dentro, triángulos de los 6 tipos de orientación (más planos aleatorios), proyección dentro/fuera/en arista/en vértice, `a` (|u|²) nulo, segmentos casi paralelos y empates de distancia en `Closest3`.
- Triángulos y planos como los guarda el EXE (vértices `int16 / 128`, normal `int16 / 4096`, desplazamiento `int16 / 128`, orientación 0..5).
- Fuera de los fixtures: `NaN` e infinitos, rectas de longitud cero donde dan 0/0 (`DistToMathematicalLine`, `ClosestPointOnLine`, triángulos degenerados), rectas exactamente paralelas y dentro del plano en `TestLineTriangle` y orientaciones de plano > 5 (el EXE deja la pila sin inicializar; la DLL devuelve falso).
- No se compara el material/pieza/iluminación (0x18..0x1B) de `GetBoundingBoxFromTwoSpheres`: el EXE no los escribe y la DLL construye un `CColBox` por valor.
- No se toca ningún estado global: solo se guardan y restauran los estados de los hooks (`state_restored`).

Repetir: `python reversing/ColGeom59a/build_probe.py` y `python reversing/run.py --case colgeom59a`.
