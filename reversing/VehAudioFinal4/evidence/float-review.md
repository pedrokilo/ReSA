<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de retornos float

Corrida examinada: `20261010-143839-960710/comparisons.jsonl`.

| Función | Pares | Diferencias | Clasificación |
|---|---:|---:|---|
| GetFlyingMetalVolume | 128 | 14 | Fuente REAL: redondeo intermedio del logaritmo |
| GetVolumeForDummyIdle | 96 | 10 | Fuente REAL: redondeo intermedio del logaritmo |
| GetVolumeForDummyRev | 96 | 9 | Fuente REAL: redondeo intermedio del logaritmo |

`CAEAudioUtility::AudioLog10` del EXE ejecuta `0x4D9E61 fldlg2`, `0x4D9E63 fld [esp+4]`, `0x4D9E67 fyl2x`, `0x4D9E69 ret`: conserva en ST0 la precisión interna del logaritmo. La fuente de esa utilidad almacena `fstp result` de tipo float. Al multiplicar posteriormente, esa pérdida de precisión produce los retornos distintos observados.

- **FIXED GetFlyingMetalVolume:** `0x4F61CC call 0x4D9E50` conserva ST0 hasta la multiplicación por 10. Se sustituye la expresión por `AudioLog10ScaledExe(v, 10.0f)`.
- **FIXED GetVolumeForDummyIdle:** `0x4F528F call 0x4D9E50; 0x4F5294 fmul [0x858BA4]` (20), y `0x4F52BD call; 0x4F52C2 fmul [0x85862C]` (10). Ambas ramas usan el helper con multiplicación antes del almacenamiento float.
- **FIXED GetVolumeForDummyRev:** `0x4F547B call 0x4D9E50; 0x4F5480 fmul [0x858BA4]`, y `0x4F5499 call; 0x4F549E fmul [0x85862C]`. Misma corrección.

El helper compartido mantiene la rama del EXE para entradas inferiores a 1e-5 o NaN (`0x4D9E5C test ah,1; jne`) y aplica `fyl2x; fmul; fstp`. No se cambia la tolerancia ni se enmascara el retorno en el banco. Las tablas, orden de las sumas, ajustes por agua/remolque y límites ya coinciden con el EXE. Validación dentro del juego pendiente de la ejecución serial del coordinador.
