<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente del run aprobado — PadCamera7

Run **20261010-203312-870430**. Gzip leído y descomprimido directamente; huellas comprimida/descomprimida iguales al manifest. Las cuatro huellas de instrumentación coinciden con los archivos vigentes. No se exige que Main actual coincida con una ejecución histórica.

**3 funciones, 20480 pares, cero diferencias y cero nulos.** Original/candidate son objetos completos iguales con equal/accepted verdaderos. Cada lado de cada par confirma entrada RH correcta, helper real, CRT real, contrato de resultado, ABI ESP/EBP/EBX/ESI/EDI, top/control x87 y guardas/entradas readonly.

El helper50A850 se apaga para original y activa para candidato. El gate vigente exige que el puntero miembro resuelva al Body capturado y al mismo ASI, además de registro y ruta nativa. No se infiere un cuerpo reconstruido desde un ILThunk apagado. CRT821B40 permanece original por contrato. Se restauran cámara/mouse/flag, FNSAVE/FRSTOR, MXCSR y cuatro estados RH. Final complete/state_restored verdadero; summary de admisión confirma saves_unchanged y configs_restored verdaderos.

Cobertura exacta del log:

```json
{
  "begin": [
    {
      "collaborators_stubbed": false,
      "event": "begin",
      "helper_registered": true,
      "library_route_ok": true,
      "registered_hook": true
    }
  ],
  "summaries": [
    {
      "branch_counts": [
        1824,
        224,
        2048
      ],
      "cases": 4096,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CPad::GetCameraCycleUp"
    },
    {
      "branch_counts": [
        2628,
        5280,
        284
      ],
      "cases": 8192,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CPad::GetMouseCameraStickHorizontal"
    },
    {
      "branch_counts": [
        2628,
        5280,
        284
      ],
      "cases": 8192,
      "complete_paths": true,
      "event": "function_summary",
      "failures": 0,
      "function": "CPad::GetMouseCameraStickVertical"
    }
  ],
  "complete": {
    "event": "complete",
    "pairs": 20480,
    "state_restored": true
  }
}
```

SHA256 log `794ccaced29634e35862395dd61827b2e1dd212f1d1101f6c5f3cb6adc4d6f49`; gzip `ff88d01f53d3787be901c49deec29b55c5cf2f27ca1fe4134f0037b9cd89443f`; ASI ejecutado `f5c5ced31dd1584cff207272603aee52803731c7cbc7adbfc1b1956e8aa8ce3e`.

Alcance: estas tres funciones y fixtures dentro del juego cargado. No acredita independencia transitiva del CRT ni servicios del host, ni integración universal. Instrucciones y legalidad de fixtures revisadas en audit-static.md. No se ejecutó ni compiló GTA en esta auditoría.
