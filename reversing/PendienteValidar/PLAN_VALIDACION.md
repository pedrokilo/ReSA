<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan de validación de lo reconstruido sin validar (lotes 189A–239A)

Generado el 15/11/2026 sobre la rama `reconstruccion-sin-validar`. Cada lote tiene su detalle en `reversing/PendienteValidar/<lote>.md` (tabla de funciones, riesgos y bancos viejos que las citan).

## Cifras

- **1,204 funciones sin validar** en 51 lotes (189A–239A); más unas 270 filas de los lotes 176A–188A que en la rama siguen con `validated=0` pero en `main` ya están validadas (se resuelven al fusionar `main` en la rama).

## Oleadas

| Oleada | Lotes | Funciones | Qué es |
| --- | --- | ---: | --- |
| V1 | 189A–206A | 544 | Independientes del código ya validado: ranuras de eventos y tareas, destructores, plantillas STL, colas y pools |
| V2 | 207A–215A | 201 | Tocan código ya validado (UseSequence, PathFind.h, EnvMap, Replay, CQuaternion::Normalise, PlayerInfo, interpoladores Fx): banco nuevo + repetir bancos viejos afectados |
| V3 | 216A–222A | 197 | Fx (FxInfo*, FxEmitter), atractores y arranque del EXE (protección/MD5): constructores, destructores con borrado y rutinas de arranque |
| V4 | 223A–229A | 121 | SDK de RenderWare: rasters D3D9, RpAtomic/RpClump/RpWorld, geometrías y materiales; exige dispositivo D3D, asignadores y listas libres falsos |
| V5 | 230A–236A | 136 | libjpeg 6b del EXE (compresión y descompresión): exige estructuras jpeg_*_struct con el layout del EXE y error_exit que vuelva |
| V6 | 237A–239A | 5 | Despachadores de comandos de script (800–899, 1200–1299, 1700–1799, 1900–1999): 5 funciones de 6–9 KB; un banco por función con un caso por comando |

## Lista completa por oleada

Columnas: lote, funciones, menciones de x87 en su informe (indicador de riesgo: más menciones = más aritmética de 24 bits que replicar).

### V1 · 189A–206A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 189A | 29 | 3 | [189A.md](189A.md) |
| 190A | 25 | 3 | [190A.md](190A.md) |
| 191A | 33 | 10 | [191A.md](191A.md) |
| 192A | 33 | 8 | [192A.md](192A.md) |
| 193A | 34 | 4 | [193A.md](193A.md) |
| 194A | 33 | 7 | [194A.md](194A.md) |
| 195A | 31 | 7 | [195A.md](195A.md) |
| 196A | 25 | 11 | [196A.md](196A.md) |
| 197A | 32 | 2 | [197A.md](197A.md) |
| 198A | 31 | 6 | [198A.md](198A.md) |
| 199A | 30 | 3 | [199A.md](199A.md) |
| 200A | 31 | 18 | [200A.md](200A.md) |
| 201A | 32 | 5 | [201A.md](201A.md) |
| 202A | 32 | 9 | [202A.md](202A.md) |
| 203A | 23 | 2 | [203A.md](203A.md) |
| 204A | 28 | 7 | [204A.md](204A.md) |
| 205A | 32 | 2 | [205A.md](205A.md) |
| 206A | 30 | 2 | [206A.md](206A.md) |

### V2 · 207A–215A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 207A | 22 | 5 | [207A.md](207A.md) |
| 208A | 15 | 8 | [208A.md](208A.md) |
| 209A | 29 | 2 | [209A.md](209A.md) |
| 210A | 17 | 21 | [210A.md](210A.md) |
| 211A | 25 | 5 | [211A.md](211A.md) |
| 212A | 32 | 31 | [212A.md](212A.md) |
| 213A | 32 | 8 | [213A.md](213A.md) |
| 214A | 7 | 5 | [214A.md](214A.md) |
| 215A | 22 | 8 | [215A.md](215A.md) |

### V3 · 216A–222A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 216A | 33 | 1 | [216A.md](216A.md) |
| 217A | 32 | 17 | [217A.md](217A.md) |
| 218A | 32 | 1 | [218A.md](218A.md) |
| 219A | 30 | 1 | [219A.md](219A.md) |
| 220A | 29 | 0 | [220A.md](220A.md) |
| 221A | 25 | 3 | [221A.md](221A.md) |
| 222A | 16 | 1 | [222A.md](222A.md) |

### V4 · 223A–229A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 223A | 22 | 4 | [223A.md](223A.md) |
| 224A | 14 | 1 | [224A.md](224A.md) |
| 225A | 8 | 1 | [225A.md](225A.md) |
| 226A | 17 | 3 | [226A.md](226A.md) |
| 227A | 15 | 2 | [227A.md](227A.md) |
| 228A | 22 | 4 | [228A.md](228A.md) |
| 229A | 23 | 2 | [229A.md](229A.md) |

### V5 · 230A–236A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 230A | 25 | 2 | [230A.md](230A.md) |
| 231A | 29 | 1 | [231A.md](231A.md) |
| 232A | 15 | 3 | [232A.md](232A.md) |
| 233A | 23 | 1 | [233A.md](233A.md) |
| 234A | 10 | 2 | [234A.md](234A.md) |
| 235A | 30 | 1 | [235A.md](235A.md) |
| 236A | 4 | 1 | [236A.md](236A.md) |

### V6 · 237A–239A

| Lote | Funciones | x87 | Informe |
| ---: | ---: | ---: | --- |
| 237A | 2 | 5 | [237A.md](237A.md) |
| 238A | 2 | 7 | [238A.md](238A.md) |
| 239A | 1 | 1 | [239A.md](239A.md) |

