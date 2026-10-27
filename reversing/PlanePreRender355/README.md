<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlanePreRender355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::PreRender`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-prerender355/20261008-025219-656200/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# PlanePreRender355

Estado: aprobado en 16384 comparaciones EXE/DLL, informe `20261008-025219-656200`. Debe repetirse tras la activación y congelación final del lote; esta aprobación corresponde al binario archivado.

`CPlane::PreRender` (`0x6C94A0..0x6CAADD`). Entrada virtual verificada slot17 de VMT `0x871948`; el banco fuerza y comprueba el destino EXE/DLL en cada pareja. La implementación de root reside en Plane.cpp, con todos los saltos internos/switch/SEH en DLL; sus colaboradores externos se enumeran en `../VehicleAudio355Evidence/Plane_PreRender_dependencies.json`. ASM completo y pseudo: `../Pending355/combo/CPlane/006C94A0.txt` y `../VehicleAudio355Evidence/Plane_PreRenderFull.txt`. EXE SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

16384 parejas numeradas 0..16383, 16 modelos: Skimmer460, RC464, Rustler476, Beagle511, Cropduster512, Stunt513, Shamal519, Hydra520, Vortex539, Nevada553, AT400577, Andromada592 y cuatro ramas default/límites vecinos. Variaciones de status, nodos presentes/ausentes, superficie de control, propulsor que cruza una/dos vueltas, tren negativo/cerrado/abierto/extremos, suspensión suavizada, velocidad positiva/negativa y esquema de geometría cero/no cero. Hydra tiene cuatro FX existentes/ausentes, creación fallida/exitosa, engine on/off, límite de tobera, dust kill/create, matrices/offsets/time. Incluye Vortex539 con hover/pitch y cálculos de ruedas/cuaternión, panels activos/inactivos, shadow types, streaks, smoke de512/513 y Skimmer460 con rama boat.

Comparación estricta de FNV1a64 de todos los bytes de vehículo, matriz, 25frames de256bytes, 8fixtures FX y atomic; salida explícita de prop y alturas de ruedas; traza exacta de argumentos de colaboradores. FNV usa semilla14695981039346656037, xor por byte y multiplicación1099511628211. Streak identifica ambos lados por offset0/1 respecto de vehículo. Se comparan valores XYZ definidos de matrices temporales; los flags/padding de esas matrices que el original deja sin escribir no se usan como contrato.

Los cálculos de matrices/cuaterniones/vectores/CRT son reales y compartidos, para conservar cambios completos de frames. Render/FX/basePreRender/wheel-model/virtualReset/Hover/alpha/componentrotation/panels/shadow/streaks/boat están aislados por colaboradores simétricos; todos los calls del cuerpo DLL pasan por los mismos entry points que EXE, por lo que una EntryPatch por dirección alcanza ambas rutas. La vtable falsa conserva71slots y sustituye D0/114. No hay render, emisiones, pools, eventos o archivos reales.

Se restauran timestep y las16 entradas de modelinfo sustituidas. Todo vehículo/frame/matriz/FX/model/collision/handling/clump/vtable vive en fixtures locales. Muestras de colisión tienen denominadores finitos no cero. Las llamadas a sombras/FX/boat nunca ejecutan sus subsistemas reales. El banco no modifica los globals de CRT/math.

Cobertura observada en `trace-coverage.json`: boat y hover en1024 casos cada uno; partículas1366, creaciónFX683, kill341, dust409, panels13107, streaks12287, suspensión8192. Los recuentos proceden de las trazas originales de las16384 parejas iguales. Acreditan los colaboradores observados; no acreditan cobertura exhaustiva de ramas ni excepciones SEH.
