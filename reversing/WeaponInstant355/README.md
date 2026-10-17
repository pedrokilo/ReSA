<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WeaponInstant355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CWeapon::FireInstantHit`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-instant355/20261008-062300-248587/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# WeaponInstant355

Reconstrucción de `CWeapon::FireInstantHit` (0x73FB10) contrastada con el pseudocódigo REA y ensamblador completo del EXE canónico en `World355Evidence/CWeapon_FireInstantHit.txt`.

Banco preparado para 20480 pares EXE/DLL, pendiente de compilación y ejecución por el coordinador. No se declara validado antes de obtener el informe de ejecución.

Diez armas y variantes deterministas cubren tirador vehículo/ped/objeto, estados jugador y remoto, disparo desde tarea UseGun, objetivo explícito/posición/cámara/avance, datos de jugador y hueso transformado, vehículo/entidad adjunta ignorada, dispersión y sway, escopetas y piezas de neumáticos excluidas, uno o dos impactos de perdigones, FX con/sin flash, reloj/contador de armas automáticas y splash.

Los colaboradores de trayectoria, colisión, eventos, referencias, efectos, audio, luces, shell, fuego, aleatoriedad y reloj se sustituyen simétricamente. No se generan eventos ni efectos reales. Se comparan argumentos y resultados float por bits, orden exacto de trazas y hashes FNV1a64 de todos los bytes definidos de objetos, matrices, datos de jugador y globales. Las direcciones de referencias de eventos en la pila se omiten porque son ubicaciones temporales diferentes; los registros y limpiezas se comparan por cantidad y orden. Las ramas que dejarían CColPoint sin inicializar se evitan mediante objetivos con autorización de aim.

El helper de agua 0x6E61B0 es un colaborador externo fuera del catálogo de esta reconstrucción. No se cuenta como cuerpo reconstruido. El x87 se conserva mediante helper nombrado para seno/coseno del tiempo; el resto del cuerpo está escrito en C++.

Los globales de opciones de línea, contador, reloj, tabla de modelos y las dos cámaras modificadas se restauran. El hook nuevo sigue desactivado hasta aprobación runtime.

## Evidencia y cierre del lote 355

Último informe real: `20261008-050146-214458`; no aprobado, pares no informados en el cierre. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-instant355/20261008-050146-214458/summary.json>).

No hay aprobación histórica en los informes disponibles.

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CWeapon_FireInstantHit.txt](../World355Evidence/CWeapon_FireInstantHit.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
