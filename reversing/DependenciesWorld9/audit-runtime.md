<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente del run aprobado — World9

Run **20261010-205616-536437**. Lectura directa del gzip completo sin build/juego ni cambios a instrumentación. SHA256 comprimido/descomprimido y cuatro hashes instrumentales coinciden con validation-summary.json.

**2 funciones, 4096 pares, cero diferencias/nulos**, original/candidate dict completos iguales y accepted/equal verdaderos. Todos los gates en ambos lados son verdaderos: rutas, CRT real/intacto, tabla real, ABI, FP108/MXCSR restaurados, globals, contrato y readonly/guardas. EAX/ECX/EDX y slots posteriores son observaciones por fixture, sin convertir void en contrato universal.

Cobertura exacta del gzip:

```json
{
  "coverage": {
    "elapsed": [
      0,
      2999,
      3000,
      3499,
      3500,
      4999,
      5000,
      123456
    ],
    "weather": [
      0,
      1,
      2,
      3
    ],
    "big": [
      0,
      1,
      2,
      3
    ],
    "small": [
      0,
      1,
      2,
      3
    ],
    "coordinate_signs": [
      -1,
      0,
      1
    ],
    "ticks": [
      0,
      1,
      2147483647,
      2147483648,
      4294967294,
      4294967295
    ],
    "changed": 1760,
    "unchanged": 288,
    "nan_cases": 480,
    "signed_zero_equal": 64
  },
  "summaries": [
    {
      "cases": 2048,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CWaterLevel::CalculateWavesOnlyForCoordinate2Native"
    },
    {
      "cases": 2048,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D"
    }
  ],
  "final": {
    "crt_unchanged": true,
    "event": "complete",
    "fp_state_restored": true,
    "pairs": 4096,
    "state_restored": true
  }
}
```

Begin acredita hooks registrados, CRT original módulo/entrada/cuerpo con64bytes por sitio, tabla real, cero colaboradores del juego y cero stubs. Bytes y rutas CRT permanecen inmutables; fuente usa tres llamadas reales de conversión. Objetos setterXY reales58bytes, timers y padding incluidos. Write preserva estado FP antes/después de PumpMessages/serialización. Final restituye imagenFP108, MXCSR, globals y rutas; admisión confirma partidas/configuraciones restauradas.

SHA256 log `99fcbcd38fec81e2b7617dbf8d28baa94e6a48b336b15405c73bd45c494892b2`; gzip `70574b98b7af015e0cfb152644b15902699c9f612b98dc6e7262e626774008bd`; ASI `a0c9e3e5241c17048a370403b42316649eca357f85f7d73330b1958d5d091ee6`.

Reparación previa al build: nombre small colisionaba con macro Windows; raíz cambió alias a bigWavesAmpl/smallWavesAmpl en wrapper/propuesta, sin cambiar109instrucciones naked. Especialización externa del setter compiló. Alcance limitado a fixtures dentro del juego cargado y colaboradores CRT originales; no integración universal ni independencia transitiva.
