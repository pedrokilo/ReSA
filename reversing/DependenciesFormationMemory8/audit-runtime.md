<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente — FormationMemory8 aprobado

Run **20261010-203532-167959**. Gzip archivado leído directamente; SHA256 comprimido/descomprimido coincide con validation-summary.json. Cuatro archivos de instrumentación coinciden con manifest; se conserva la evidencia histórica sin exigir Main actual idéntico.

**2 funciones, 8256 pares, cero diferencias/nulos.** Original/candidate completos iguales, equal/accepted verdaderos. Todos los contratos de ruta EXE/DLL, ABI ESP/EBP/EBX/ESI/EDI, FP, dominio, destino libre/primer empate, tabla real/readonly y guardas verdaderos en ambos lados. Formation8192 y getter64. Sin colaboradores ni stubs.

Cobertura exacta observada:

```json
{
  "begin": [
    {
      "collaborators_stubbed": false,
      "event": "begin",
      "game_collaborators": 0,
      "registered_hook": true
    }
  ],
  "coverage": {
    "counts": [
      0,
      1,
      2,
      3,
      4,
      5,
      6,
      7,
      8,
      9,
      10,
      11,
      12,
      13,
      14,
      15,
      16,
      17,
      18,
      19,
      20,
      21,
      22,
      23,
      24
    ],
    "modes": [
      0,
      1,
      2,
      3,
      4,
      5,
      6,
      7
    ],
    "negative": 2288,
    "selected": 5904,
    "tie_first": 984
  },
  "summaries": [
    {
      "cases": 8192,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CFormation::FindNearestFreeDestination"
    },
    {
      "cases": 64,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "Rs::psGetMemoryFunctions"
    }
  ],
  "complete": {
    "event": "complete",
    "pairs": 8256,
    "state_restored": true
  }
}
```

Snapshot/restauración CPointList real, FNSAVE/FRSTOR y MXCSR y dos estados RH; final complete/state_restored verdadero. Admisión documenta partidas sin cambios y configuración restaurada. Getter retorna8D6228 y compara16bytes con callbacks ejecutables reales sin invocarlos: no valida esos callbacks. La corrección de compilación CVector{...} fue sólo tipado explícito del fixture, sin alterar fuente del juego.

SHA256 log `e6a5950555d2d02265bdf3f8baa357cb24b4c93a9ddfd75febc5bc37ee887db9`; gzip `a326eec641322910e8e728a3b482ba49d3391cd4991d87b94bf734bd344acbe9`; ASI `904f607590de333efdf4e09250b8b42d0ded0cb47e5f4385b1e64df5a190af8d`.

Alcance limitado a fixtures del banco dentro del juego cargado; no integración universal ni independencia transitiva. Auditoría estática en audit-static.md. Sin ejecutar ni compilar GTA durante esta revisión.
