<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorSpatial — tiles y geometría del interior

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093610-928011`, 7168 pares. El alcance se limita a los fixtures y colaboradores documentados.

Evidencia: pseudocódigo REA y ensamblador completo en `../InteriorEvidence/Interior_<función>.txt`, extraídos del EXE 1.0 US `D:/Grand Theft Auto San Andreas/gta-sa.exe`, SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

| Función | Dirección | Pares previstos |
| --- | --- | ---: |
| `GetTileCentre` | `0x591BD0` | 1024 |
| `GetRandomTile` | `0x591B20` | 1024 |
| `FindEmptyTiles` | `0x591C50` | 1024 |
| `FindBoundingBox` | `0x5922C0` | 1024 |
| `ResetTiles` | `0x593910` | 1024 |
| `IsPtInside` | `0x5913E0` | 1024 |
| `CalcMatrix` | `0x5914D0` | 1024 |

Total: 7168 pares contiguos. Cada caso compara original EXE y candidato y confirma selección del hook. Se exportan hashes del Interior completo 0x794, caja, grupo, entidad, RwObject, frame, mapa visited, salidas enteras, bits de punto/bias/traslación, retorno y traza completa. Fixtures exteriores conservan direcciones entre ambos lados, se reinicializan y no necesitan normalizar punteros de estado. Las matrices se identifican como Interior o frame sólo cuando coinciden exactamente; punteros desconocidos se conservan crudos.

Escenarios: dimensiones 5–9 con altura 2–8; traslaciones fraccionarias; bases unidad y bases con todos los componentes distintos de cero; interiores positivos/negativos, igualdad al límite, NaN e infinitos en IsPtInside; cuatro patrones de ResetTiles con spans ausentes/presentes y rectángulos; bounding boxes vacías y regiones conectadas con huecos; RNG con rechazo del primer tile y acierto posterior; FindEmptyTiles éxito inmediato/tardío y fracaso de las cien tentativas; CalcMatrix entidad con/sin RwObject y flags/padding no nulos.

Cobertura exigida: true/false en ambos métodos booleanos, retry real en GetRandomTile, expansión visited en FindBoundingBox, escritura de salida en GetTileCentre, mutación completa en ResetTiles/CalcMatrix y matrices nulas/no nulas en CalcMatrix. Los contadores y requisitos se escriben en function_summary y afectan al resultado final.

Aislamiento simétrico según objetivo: FindEmptyTiles sustituye CheckTilesEmpty 591680 y su símbolo DLL; GetTileCentre sustituye RwV3dTransformPoints 7EDD90 y DLL; CalcMatrix sustituye GetEntity 594BD0 y DLL, Rotate 7F1FD0, Translate 7F2450 y Multiply 7F18B0 y sus símbolos DLL. RNG 821B1E y las entradas DLL de std::rand/CGeneral::GetRandomNumber se sustituyen simétricamente en RandomTile/Empty, evitando parches duplicados. `_ftol2` 821B40 se ejecuta realmente.

GetRandomTile y FindBoundingBox leen los tiles directamente en el EXE; ResetTiles integra las escrituras de SetTilesStatus dentro de su cuerpo. Estos colaboradores no se instrumentan con trazas: el candidato ejecuta sus implementaciones reales para evitar trazas unilaterales. Las regiones de BoundingBox se sitúan dentro de habitaciones de 20×20. FindBoundingBox conserva su recursión real y compara el mapa de visitados completo. GetTileCentre devuelve el puntero de salida conservado en EAX tras TransformPoints; GetRandomTile devuelve el entero Y conservado en EAX. Se comparan ambos retornos completos y se exige que coincidan con sus salidas. IsPtInside pasa bias por valor (RET 10h).

Límites: los callbacks validan argumentos/orden/estado antes y después de colaboradores deterministas. No validan el algoritmo RenderWare de transformación/rotación/multiplicación ; los colaboradores de tiles se ejecutan realmente cuando el candidato los usa y también tienen bancos separados. Los fixtures no crean entidades ni recursos RenderWare en el juego. Hooks y parches se restauran después de cada identidad; no hay activación permanente.

El coordinador compila y ejecuta: `python reversing/InteriorSpatial/build_probe.py`, después `python reversing/InteriorSpatial/run_local.py`. Este agente verificó codificación UTF8/LF, sintaxis Python, siete identidades EXPECTED/7168 pares y existencia de las fuentes declaradas; ninguna comprobación sustituye la validación runtime.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093610-928011`: 7,168 pairs across 7 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
