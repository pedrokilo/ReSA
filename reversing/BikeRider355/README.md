<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeRider355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::ProcessRiderAnims`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-rider355/20261008-065536-474295/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# BikeRider355

Preparado: 1.024 pares completos CBike::ProcessRiderAnims (0x6B7280), hook false, sin aprobación de runtime todavía. Evidencia: ASM privada Bike_RiderFull.txt y pseudocódigo sano tasks355-bike-pseudo/CBike.ProcessRiderAnims_6B7280.c, EXE canónico 72ae59e4.

Cubre jugador y NPC, moto y automóvil, contacto y ruedas levantadas, velocidades positivas/negativas/casi cero, habilidad, fuerzas angulares, caída, pasajero, idle/retroceso y animaciones especiales, mezcla lateral/frontal, wheelie/stoppie y vibración de hueso. Se comparan todos los bytes de vehículo, dos peatones, inteligencias, diez asociaciones y jerarquías, RideAnimData, handling, matrices/hueso/frame y dos regiones globales contiguas, además de las trazas y los eventos semánticos completos. Restaura globals y hook.

Se aíslan igual en EXE/DLL IsPlayer, GetBikeRidingSkill, consultas de asociaciones, AddAnimation, BlendAnimation, SetCurrentTime, Add del grupo de eventos, registro/limpieza de referencias, rand/GetRandomNumber y RwMatrixRotate. Los eventos KnockOffBike conservan sus constructores/destructores reales; la traza compara sus campos semánticos y omite vtable/padding. No se acredita aquí la integración del animador y los eventos durante una partida.

Sólo root compila con MP6 y ejecuta el banco en GTA de manera secuencial. SOURCES conserva sus dependencias reales.

Coordinator run `20261008-024627-912355` approved 1024 pairs, all comparisons equal. Final source/manifest replay remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-024627-912355`, 1024 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-rider355/20261008-024627-912355/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-024627-912355`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-rider355/20261008-024627-912355/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
