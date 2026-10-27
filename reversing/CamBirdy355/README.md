<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamBirdy355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_DW_BirdyCam`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-birdy355/20261008-070246-909117/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamBirdy355

Process_DW_BirdyCam (`0x51B850`), 16384 pares EXE/DLL pendientes. C++ completo y bool(bool) corregido, reconstruido desde combo pseudo+ASM `../World355Evidence/CCam_Process_DW_BirdyCam.txt`.

Cubre target gate, inicialización flags/cache/counter, consultas adelante/detrás, filtrado flags/up/lamp, selección doble con índice inicial compartido, posiciones/model bbox/random height, doble LOS, persistencia cache/duración/ease sin clamp, failed/exit/LOS/counter/final. Fixture cuatro objetos con matrices y modelos falsos; consultas/clear/lamp/core/random/LOS/exit/final/handshake stubs simétricos. FNV1a64 de todos los bytes de cámara y globals/times/parameters/cache, trace/salida exacta. Model table/globals/time/flags/params restaurados, sin eventos/render. Pares0..16383.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-040031-694941`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-birdy355/20261008-040031-694941/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
