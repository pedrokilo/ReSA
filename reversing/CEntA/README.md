<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEntity, grupo A: matrices, transformaciones, limites y visibilidad (26 funciones)

Banco por pares dentro del motor (original = EXE con los hooks del grupo desactivados; candidato = DLL), con comparacion bit a bit
de todo el estado tocado y de la traza de las dependencias simuladas. Escrito sin compilar ni ejecutar: lo compila y lo itera quien lo lance.

| Funcion | Direccion | Casos |
|---|---|---:|
| `AttachToRwObject` | 0x533ED0 | 768 |
| `DetachFromRwObject` | 0x533FB0 | 512 |
| `BuildWindSockMatrix` | 0x532DB0 | 512 |
| `CalculateBBProjection` | 0x535340 | 1024 |
| `ModifyMatrixForBannerInWind` | 0x535040 | 768 |
| `ModifyMatrixForCrane` | 0x533170 | 512 |
| `ModifyMatrixForTreeInWind` | 0x534E90 | 1024 |
| `GetBoundCentre-ref` / `GetBoundCentre-vec` | 0x534290 / 0x534250 | 512 / 512 |
| `GetBoundRect` (virtual, vtable 0x863928 ranura 9) | 0x534120 | 512 |
| `GetColModel` | 0x535300 | 256 |
| `GetDistanceFromCentreOfMassToBaseOfModel` | 0x536BE0 | 256 |
| `GetModellingMatrix` | 0x46A2D0 | 128 |
| `TransformFromObjectSpace-ptr` / `-ref` | 0x533560 / 0x5334F0 | 768 / 768 |
| `UpdateRwFrame` | 0x532B00 | 128 |
| `UpdateRwMatrix` | 0x446F90 | 512 |
| `UpdateRpHAnim` | 0x532B20 | 256 |
| `UpdateAnim` | 0x535F00 | 768 |
| `Add-rect` (virtual, ranura 1 = 0x5347D0) | 0x5347D0 | 1024 |
| `Remove` (virtual, ranura 3 = 0x534AE0) | 0x534AE0 | 1024 |
| `LivesInThisNonOverlapSector` | 0x533050 | 1024 |
| `GetIsBoundingBoxOnScreen` | 0x5345D0 | 1024 |
| `GetIsOnScreen` | 0x534540 | 512 |
| `IsEntityOccluded` | 0x71FAE0 | 1024 |
| `IsVisible` | 0x536BC0 | 256 |

Total: 16.384 pares. Repetir: `python reversing/CEntA/build_probe.py` y `python reversing/run.py --case centa`
(o `python reversing/CEntA/run_local.py`; filtro por funcion con `centa-only.txt`, p. ej. `CEntity::Remove`).

**Fixtures**
- Entidades falsas en un bloque de 4 x 0xA20 bytes (relleno 0xCD y solo los campos que el EXE lee: vtable falsa, posicion compacta o matriz falsa,
  banderas, semilla, indice de modelo, objeto RenderWare, enlace de streaming, tipo en +0x36, lista de movimiento +0xB4, tipo de objeto +0x13C
  e indice de colision especial +0x48B). Nunca entidades ni matrices del pool real: `CPlaceable::AllocateMatrix`/`GetMatrix` estan simuladas y
  devuelven una matriz propia del banco (el EXE las llama en linea; la DLL por su simbolo).
- vtable falsa de entidad: `SetIsStatic` (ranura 4) y `GetBoundRect` (ranura 9, rectangulo inyectado por caso) trazados; el resto aborta con evento `error`.
- Model info falso en el indice 19990 (banderas +0x12, modelo de colision +0x14, tipo de modelo por la ranura 4 de su vtable), modelo de colision falso
  (caja, esfera) y las 4 primeras entradas de `CVehicle::m_aSpecialColModel` (se guardan y restauran).
- RenderWare falso: clump (tipo 2) y atomico (tipo 1) con su marco (matriz de modelado en +0x10), geometria, piel, jerarquia y asociacion de animacion.
- Mundo falso para `Add`/`Remove`: sectores (115.200 bytes), repeticion (3.072), LOD (3.600) y cabeza de la lista de movimiento se vacian en cada
  ejecucion y se rellenan con pools de nodos LOCALES (8192 nodos simples y dobles) y pools de entidades FALSOS que cubren el bloque de entidades
  (como Physical56: la DLL de depuracion valida nodos e Items contra esos pools). Los rectangulos usan bordes de sector exactos (50*k-3000, +-1 ulp), +-3000,
  2999,99 y 3000,0 (estos dos para el recorte `top > 3000`), con un maximo de ~700 celdas por caso. Los casos LOD evitan `top == 3000.0` y `left > 2990`
  (el EXE escribiria fuera del array de 30x30 listas).
- Estado global guardado y restaurado en cada ejecucion y al final: punteros de pools, sectores, repeticion, LOD, lista de movimiento, entrada 19990 de
  `ms_modelInfoPtrs`, `m_bMirrorActive` y frustum de la camara (normales, espejo, desplazamientos), `numBBFailed`, viento y direccion, pausa, pasos de tiempo,
  reloj, contador y 4 primeros oclusores activos, modelos de colision especiales.
- Se comprueba en tiempo de ejecucion que los desplazamientos de la DLL coinciden con los del EXE (si no, evento `error` con los campos).

**Dependencias simuladas y trazadas (EXE y DLL)**: CPlaceable::AllocateMatrix/GetMatrix, RwFrameUpdateObjects, GetFirstAtomic, RpSkinGeometryGetSkin,
GetAnimHierarchyFromSkinClump, RpHAnimHierarchyUpdateMatrices, CBaseModelInfo::AddRef/RemoveRef, CStreaming::AddEntity/RemoveEntity,
CEntity::CreateEffects/DestroyEffects, CPhysical::AddToMovingList (la funcion real sale si ya hay enlace o espera colision; el stub lo imita porque el EXE
solo la llama sin enlace), CCamera::IsSphereVisible (distingue camara principal y espejo por el puntero de la matriz), RpAnimBlendClumpGetFirstAssociation,
RpAnimBlendClumpUpdateAnimations (el booleano se compara enmascarado a 8 bits: el EXE empuja un dword con basura en los bytes altos), CWindModifiers::FindWindModifier.
`CalcScreenCoors` (las dos sobrecargas) y `CActiveOccluder::IsPointWithinOcclusionArea/IsPointBehindOccluder` son funciones PURAS por hash de sus argumentos
(sin traza): asi el calculo perezoso del EXE y el de la DLL son comparables. CVector, CMatrix, CSimpleTransform, TransformPoint/TransformVectors y
RwMatrixUpdate corren de verdad (el relleno +0x0C/+0x1C/+0x2C/+0x3C de matrices y del marco se enmascara a 0 antes de comparar).

**Correcciones de la DLL (comparadas con el desensamblado)**
- `Add-rect` / `Remove`: reescritas. Recorte del rectangulo con las comparaciones del x87 (el campo +4 se recorta con `> 3000` estricto, no `>= 3000`);
  sector = floor(v * 0.02 + 60) en float (no `v / 50 + 60`), LOD = trunc(v * 0.005 + 15) sin floor y sin comprobar rangos; las celdas de repeticion se indexan
  con `(y & 15)` y `(x & 15)` (no `abs % 16`); sin los asserts de `IterateSectors`.
- `BuildWindSockMatrix`: el vector de viento se invierte (`fchs`): `(-(x + 0,01), -(y + 0,01), 0,1)`; la DLL no lo negaba.
- `ModifyMatrixForTreeInWind`: con viento >= 0,5 el orden es `((1 - c) * t1 + c * t2) + 1` (la DLL hacia `a + (1 + b)`), c = fase * 1/4096; con viento < 0,5
  `fsin` de precision completa y el factor es 0,008 (0x863984) con viento >= 0,2 o 0,005 (no `0,005 * 1,6`), angulo = fase * 0,0015332031762227416 (0x863988), arrays con `.data()[]`.
- `ModifyMatrixForBannerInWind`: se multiplica por la constante elegida (0,2 / 0,43 / 0,66), no por `CWeather::Wind`; fase = `((trunc(y + x) << 6) + tiempo) << 4` en 32 bits
  (sin el `uint16` intermedio); `sqrt(1 - w * w)` en float en vez de `pow` en doble.
- `ModifyMatrixForCrane`: constante exacta del EXE 0x863980 (0,0061328127...) en vez de `PI / 512,26F` (PI de common.h es impreciso).
- `CalculateBBProjection`: las longitudes de los ejes son 2D (`sqrt(y*y + x*x)`, `sqrt(x*x + y*y)`), no `Magnitude()` 3D; el valor absoluto es `(v < 0) ? -v : v` (como `fcomp` + `fchs`).
- `GetIsBoundingBoxOnScreen`: la esquina se elige por el bit de signo de la normal (no `< 0`), el producto escalar sigue el orden z, y, x del EXE y la rama del espejo
  usa `!(dot > offset)` (con NaN el EXE continua).
- `LivesInThisNonOverlapSector`: centro `(derecha + izquierda) * 0,5` y sector `floor(c * 0,02 + 60)` (no `GetSectorX` con division).
- `IsEntityOccluded`: transcripcion fiel. Calcula el centro antes de mirar si hay oclusores, transforma los puntos con la matriz (`GetMatrix()` + 0x59C890 con su orden
  de sumas, no `TransformFromObjectSpace`) y los comprueba perezosamente (los seis puntos eran precalculados), la distancia al oclusor es `int16` con signo (`movsx`), el
  maximo de escalas es `x > y ? x : y`, y los umbrales de tamano son `!(v > 60)`/`!(v > 30)`.
- `UpdateAnim`: el paso de tiempo es `ms_fTimeStep * 0,02` y `ms_fTimeStepNonClipped * 0,02` (la DLL dividia entre 50).
- `TransformFromObjectSpace-ref`: producto matriz x vector con el orden de sumas de 0x59C890 (no `TransformPoint`); `GetColModel` accede a `m_aSpecialColModel` sin comprobar rango.
- Dependencias de ruta compacta (sin matriz), no estan en el grupo pero cambian el resultado: `TransformPoint` (common.cpp, fcos/fsin a precision completa y productos del x87),
  `TransformVectors(CSimpleTransform)` (solo rota, sin sumar la posicion, z copiada, coseno y seno redondeados a float) y `CSimpleTransform::Invert` / `UpdateRwMatrix`
  (SimpleTransform.cpp, fcos/fsin a precision completa).

**Sin diferencia (MATCH)**: `AttachToRwObject`, `DetachFromRwObject`, `GetBoundCentre-ref/-vec`, `GetBoundRect`, `GetModellingMatrix`, `UpdateRwFrame`, `UpdateRwMatrix`,
`UpdateRpHAnim`, `GetIsOnScreen`, `IsVisible`, `GetDistanceFromCentreOfMassToBaseOfModel`, `TransformFromObjectSpace-ptr` (salvo lo indicado arriba).

**No cubierto**: pool de nodos lleno (el EXE escribe en la direccion 4); tipos de entidad 0 y >= 6 en `Add`/`Remove` (el EXE salta a un puntero basura); rectangulos LOD fuera de 0..29
(el EXE escribe fuera del array); indices de modelo >= 0x8000 (el EXE usa `movsx`); NaN en rectangulos, matrices y viento; `Add()` sin argumentos (ranura 2, no hookeada).
