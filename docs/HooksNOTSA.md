<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Extensiones y adaptaciones NOTSA fuera del progreso

`hooks_notsa.csv` conserva cuatro registros separados del catálogo de reconstrucción: tres hooks de WindowedModeHooks y Win::NOTSA_WinMain. La auditoría de hooks contempla ambos CSV; la clasificación separada no retira sus hooks ni elimina sus direcciones del mapa.

| Grupo | Registro | Dirección | Clasificación |
| --- | --- | --- | --- |
| WindowedModeHooks | RenderWare::_rwD3D9CameraBeginUpdate | 0x007F8F20 | Adaptación de modo ventana |
| WindowedModeHooks | RenderWare::_rwD3D9CameraClear | 0x007F7730 | Adaptación de modo ventana |
| WindowedModeHooks | notsa::WindowedMode::HookDirect3DDeviceReplacerSA | 0x007F6781 | Extensión de modo ventana |
| Win | NOTSA_WinMain | 0x00748710 | Punto de entrada con cuerpo adaptado NOTSA |

Win::NOTSA_WinMain ocupa una dirección real del EXE, pero su cuerpo adaptado no se presenta como reconstrucción fiel validada. Por decisión explícita de alcance se excluye del denominador de progreso y conserva `validated=0`. Su hook permanece instalado según la fuente; no se elimina código ni se inventa una comparación aprobada.

El catálogo de progreso queda en **8.445/8.445 funciones y 771/771 grupos con evidencia histórica (100%)**, con cero pendientes y cero locks. El corte 453 y las corridas históricas conservan sus resultados y denominadores originales; este cambio de clasificación no añade funciones validadas.

El porcentaje del catálogo no acredita independencia del EXE ni compatibilidad universal. La **vigencia actual sigue en 0**: las evidencias históricas mantienen sus límites y sus revisiones de dependencias pendientes. Véanse el [estado actual](CurrentStatus.md), el [catálogo principal](hooks.csv) y el [catálogo NOTSA](hooks_notsa.csv).

El cierre de ese catálogo no abarca todas las clases ni todos los métodos del juego. La [auditoría binaria](BinaryCoverageAudit.md) identifica 924 destinos originales explícitos sin nombre catalogado, 499 con aristas desde funciones catalogadas y 9.328 entradas binarias sin nombre del catálogo. Los conjuntos se solapan e incluyen código de biblioteca y generado; no deben sumarse ni leerse como un total exacto de funciones pendientes.
