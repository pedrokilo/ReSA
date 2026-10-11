<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente del run aprobado

Run **20261010-202741-158472**. Lectura independiente del gzip sin ejecutar ni compilar el juego. SHA-256 comprimido y descomprimido coinciden con validation-summary.json; los cuatro archivos de instrumentación coinciden con su manifest. No se exige que InjectHooksMain actual permanezca idéntico a una ejecución histórica.

**4 funciones, 24576 pares, 0 diferencias, 0 registros nulos.** Todos los objetos original/candidate son iguales, equal y accepted verdaderos. En ambos lados de cada par se comprobaron fixture legal, rango de lectura, canarios, outputs opcionales, ruta EXE/DLL y CRT, ABI, x87, entradas de sólo lectura y guardas.

Evento begin: registro RH verdadero, atlas real cargado y ambas orientaciones, CRT real. Todos los resúmenes complete_paths son verdaderos. Camino vector y escalar: tres clases de duración presentes y avance observado; cada comparación conserva read_range_ok y path_canaries_ok. Agua: ambas rutas geométricas y retornos true/false; igualdad inferior, rechazo inferior limitado, igualdad superior, rechazo superior y rama de salidas nulas verdaderos.

Datos exactos extraídos del log:

```json
{
  "begin": [
    {
      "actual_crt_library": true,
      "both_triangle_orientations": true,
      "event": "begin",
      "loaded_quads": 301,
      "loaded_triangles": 6,
      "loaded_vertices": 1021,
      "registered_hook": true,
      "zero_height_quads": 256,
      "zero_height_triangles": 6
    }
  ],
  "summaries": [
    {
      "accepted_fixture_kinds": [
        512,
        512,
        512,
        512,
        512,
        512,
        512,
        512
      ],
      "cases": 4096,
      "complete_paths": true,
      "depth_high_equal": false,
      "depth_high_reject": false,
      "depth_low_equal": false,
      "depth_low_reject": false,
      "duration_classes": [
        520,
        216,
        3360
      ],
      "event": "function_summary",
      "failures": 0,
      "function": "CCam::CameraPathInterpolateVector",
      "null_depth_branch": false,
      "saw_false": true,
      "saw_index_advance": true,
      "saw_true": false
    },
    {
      "accepted_fixture_kinds": [
        2048,
        2048,
        0,
        0,
        0,
        0,
        0,
        0
      ],
      "cases": 4096,
      "complete_paths": true,
      "depth_high_equal": false,
      "depth_high_reject": false,
      "depth_low_equal": false,
      "depth_low_reject": false,
      "duration_classes": [
        520,
        216,
        3360
      ],
      "event": "function_summary",
      "failures": 0,
      "function": "CCam::CameraPathInterpolateScalar",
      "null_depth_branch": false,
      "saw_false": true,
      "saw_index_advance": true,
      "saw_true": false
    },
    {
      "accepted_fixture_kinds": [
        2436,
        1740,
        0,
        0,
        0,
        0,
        0,
        0
      ],
      "cases": 8192,
      "complete_paths": true,
      "depth_high_equal": true,
      "depth_high_reject": true,
      "depth_low_equal": true,
      "depth_low_reject": true,
      "duration_classes": [
        0,
        0,
        0
      ],
      "event": "function_summary",
      "failures": 0,
      "function": "CWaterLevel::GetWaterLevelNoWavesForQuad",
      "null_depth_branch": true,
      "saw_false": true,
      "saw_index_advance": false,
      "saw_true": true
    },
    {
      "accepted_fixture_kinds": [
        1269,
        729,
        0,
        0,
        0,
        0,
        0,
        0
      ],
      "cases": 8192,
      "complete_paths": true,
      "depth_high_equal": true,
      "depth_high_reject": true,
      "depth_low_equal": true,
      "depth_low_reject": true,
      "duration_classes": [
        0,
        0,
        0
      ],
      "event": "function_summary",
      "failures": 0,
      "function": "CWaterLevel::GetWaterLevelNoWavesForTriangle",
      "null_depth_branch": true,
      "saw_false": true,
      "saw_index_advance": false,
      "saw_true": true
    }
  ],
  "complete": {
    "event": "complete",
    "pairs": 24576,
    "state_restored": true
  }
}
```

Huellas: log `ff7a5756fef912bff337370db1b4585e3d997981b28e7c43439383984f325373`; gzip `d12af45ae7ed5abf88b63cf427f2627748c6c06ddb21c4ac7208d3d35290ecd4`; ASI ejecutado `e88cc70c414f1e2278023180f041c62bd303c491babb5e77da513195e7ee6936`; EXE `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El evento final exige state_restored. El resultado de admisión documenta además saves_unchanged/configs_restored verdaderos. Alcance: fixtures completos descritos por el banco en el juego cargado; no acredita integración universal ni independencia transitiva del EXE. El CRT permanece original por contrato. La revisión estática de instrucciones y formato real está en audit-static.md.
