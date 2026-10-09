<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamControl355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::CamControl`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-control355/20261008-070335-007128/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamControl355

Estado: preparado; pendiente de compilación y comparación EXE/DLL por el agente principal. No se declara validación aprobada.

Reconstrucción C++ completa de `CCamera::CamControl` (`0x527FA0..0x52B725`), dividida por selección, geometría, arma, cooperación/arresto/muerte, cine y transición. Evidencia combinada pseudo REA + ASM completo en `../World355Evidence/CCamera_CamControl.txt`. EXE canonical SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. El hook permanece `.reversed=false` hasta aprobación runtime.

24576 parejas, numeradas 0..24575. Matriz de modos antiguos y solicitados, entidades ped/vehículo/objeto, zoom/cull/pausa/idle, cooperativo, estados de arresto/muerte, controles de primera persona, selección de armas, garajes, escaleras y política de salto/interpolación/script. Se comparan FNV1a64 de todos los bytes inicializados de cámara/objetos/matrices/pad/intelligence/playerdata y globals mutados, modo/fuente explícitos, argumentos y traza exactos. Semilla FNV `14695981039346656037`, xor por byte y multiplicación `1099511628211`.

Colaboradores simétricos para evitar audio, colisión, referencias y transiciones reales. Pureza geométrica de vectores/matrices permanece real. `sub_5404F0` y `CameraShouldBeOutside` son inline en la DLL: se ejercitan las implementaciones reales sobre estado falso consistente, sin stub asimétrico. Los cinco selectores de cine y los helpers de sticks/puertas/posición de zona/WellBufferMe quedan colaboradores externos del EXE con ABI exacta y stubs comunes; no se cuentan como reconstrucciones adicionales.

Limitaciones confirmadas por ASM en `../World355Evidence/CamControl.limitations.json`: EXE lee punteros de puertas indeterminados en BIKE+stairs+zone sin garage al calcular posición; con primera puerta nula+segunda válida usa un vector alternativo de pila no establecido. No se fabrican residuos de pila para aprobarlos. Casos cercanos cubiertos: vehículo con stairs+garage válido; ped con stairs sin garage; ambas puertas, primera sola y ninguna puerta. El C++ instalado inicializa puertas nulas en el camino vehicular sin garage y termina usando target más offsets de vehículo. Con primera puerta nula y segunda válida, parte del target en vehículo y de la fuente actual en ped para el cálculo inicial; el helper final toma posición y eje Right del dummy de la segunda puerta más offsets. Estas elecciones están definidas únicamente fuera del dominio comparable; no se certifican como equivalentes al EXE. La descripción se revisó contra los tres helpers instalados en Camera.cpp.

El banco restaura pointers de ambos jugadores, jugador activo, tiempo/step/pausas/replay/modelviewer, visibilidad, override zoom, flag de garage, controles 0xB70134..43 y flags de cine 0xB6EC2D..34. No pools/archivos/render/eventos reales.

## Evidencia y cierre del lote 355

Último informe real: `20261008-025315-386510`; aprobado, 24576 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-control355/20261008-025315-386510/summary.json>).

Última aprobación histórica: `20261008-025315-386510`; aprobado, 24576 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-control355/20261008-025315-386510/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_CamControl.txt](../World355Evidence/CCamera_CamControl.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
