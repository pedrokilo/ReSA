<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn217A: 31 funciones del EXE (lote 217A: cuerpos de destructor y destructores con borrado de subclases de FxInfo_c)

Estado: **APROBADO dentro del juego** (17/11/2026, worktree `wv-5` desde 373a5aad6d, sin cherry-pick; plantilla `Fn215A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn217a/20261117-065741-627757` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn217A-20261117-065741-627757`, `comparisons.jsonl.gz`). 31 x 16384 = **507904 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia (el lote no usa x87), cuerpos de la DLL distintos para cada direccion y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `9de2e92fc3d9b79347fc487258135432b4c320e44815d3f725f8665c99083692`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

| Funciones | Entradas |
|---|---|
| 15 cuerpos de destructor (67 B, SEH) de FxInfoEmWeather_c, EmRotation, Noise, Force, Friction, AttractPt, AttractLine, GroundCollide, Wind, Jitter, RotSpeed, Float, Underwater, Colour, Size | 0x4A6DA0 ... 0x4A7650 |
| 16 destructores con borrado (27 B) de las mismas clases y FxInfoSpriteRect_c | 0x4A6D80 ... 0x4A76A0 |

**Excluida de este banco: 0x4A6D30 (cuerpo del destructor de FxInfoEmPos_c).** Figura en los informes del 217A y del 216A, la fuente la escribio el 216A (`Destructor()` con el destructor generado por el compilador) y la valida el banco Fn216A; aqui no se toca. Este banco valida por tanto 31 de las 32 direcciones del informe.

Sin defectos de fuente: el banco no encontro diferencias; no se cambio ninguna fuente de produccion.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D, guardas de 0x40); el objeto de 0x18 bytes se coloca alineado, desalineado, al borde (invade la guarda) y cerca del borde, con relleno aleatorio, ceros, unos y 0xCD, y con dwords previos en +0/+4/+8/+0xC/+0x10/+0x14 (incluidas las vtables ya puestas). Cuerpos: el destructor del miembro +8 (0x4A8460 / 0x4A89B0 / 0x4A87F0, con hook RH propio de otro lote) corre REAL con el hook apagado en el EXE; la DLL escribe las dos vtables del EXE (0x85A968 en +8, 0x85A7DC en +0) directamente; se comprueba que solo esos dos dwords cambian. Destructores con borrado: el cuerpo (entrada + 0x20) lleva el mismo falso en la direccion del EXE y en el cuerpo de la DLL (en SpriteRect es 0x4A76C0, de otro lote, que la DLL llama por la direccion del EXE); operator delete 0x8214BD lleva el falso solo en la direccion del EXE; banderas con ruido (solo cuenta el bit 0 del byte bajo); devuelve this. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/Fn217A/build_run.sh`.

## Prueba negativa
`mutate.py` (31 mutaciones por ronda, condicionadas a `this` desalineado a 4 bytes). **Las 31 funciones fallan en ambas rondas** (`negative-runs/`, 2048 casos por funcion): ronda 1 `20261117-072326-014101` (21693 de 63488 pares con diferencias) y ronda 2 `20261117-072658-338068` (27857 de 63488); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas (SHA256 igual) y sonda buena recompilada (`neg_all.sh`, una sola sesion del candado de juego).

## Bancos que citan las direcciones
Ninguno (grep de las 31 direcciones en `reversing/` sin este banco, `_handoff`, evidencia archivada ni informes). Los cuerpos llaman a los destructores de interp 0x4A8460/0x4A89B0/0x4A87F0 en el EXE; si el lote que los reconstruye cambia sus vtables, este banco los corre con el hook apagado y no se ve afectado.
