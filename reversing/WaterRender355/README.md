<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WaterRender355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CWaterLevel::RenderWater`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-water-render355/20261008-012157-621813/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# WaterRender355

Varía rangos visibles, niveles y flujo de agua, cámara, RNG y temporizadores; compara buffers, estados gráficos y parámetros semánticos del renderizado. Los bytes de padding no definidos de CRenPar se excluyen expresamente; se comparan sus cinco campos semánticos.

Funciones y pares previstos:

- `CWaterLevel::RenderWater` — 0x6EF650; 1024 pares EXE/DLL.

La evidencia canónica se conserva en `../Pending355/combo/`. `LiveComparison.hpp` especifica los fixtures y colaboradores aislados mediante parches simétricos de las entradas EXE y DLL. Esos colaboradores quedan fuera de la acreditación del banco. El banco exige conmutación real del hook, compara resultados, estado y trazas, y restaura los globals y hooks modificados.

Estado al preparar esta documentación: `approved_historical`. Último informe: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-water-render355\20261008-012157-621813`. Un informe histórico no acredita fuentes modificadas posteriormente: la aprobación definitiva depende de los hashes actuales y del preflight de `Pending355/register.py`.

Compilar con `python reversing/WaterRender355/build_probe.py`; ejecutar con GTA cerrado mediante `python reversing/WaterRender355/run_local.py`. Root serializa las compilaciones y ejecuciones del lote para no mezclar sondas.
