<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Correlación de fuentes: re3 · reVC · reSA

Inventario local generado el 2026-09-23.

| Proyecto | Rama | Raíz examinada | Archivos |
| --- | --- | --- | ---: |
| re3 | `Re3` | `E:/Original/gta3/src` | 551 |
| reVC | `ReVC` | `E:/Original/gtavc/src` | 575 |
| reSA | `ReSA` | `D:/Original/gta-reversed-master/source/game_sa` | 2,119 |

reSA corresponde a `source/game_sa` del proyecto gta-reversed-master. Las otras columnas corresponden a las copias locales indicadas; la columna Rama es la rama de git de cada copia (comprobada el 05/10/2026: `gta3` está en `Re3` y `gtavc` en `ReVC`).

**Navegación:** [Carpetas](#carpetas) · [Nombres distintos](#funcionales) · [Coincidencias con reSA](#coincidencias) · [Otros archivos re3/reVC](#otros) · [Archivos solo por nombre en reSA](#resto-sa)

## Cómo leer la correlación

- Cada ruta es un enlace al archivo o carpeta. Los enlaces relativos solo funcionan si los tres repositorios son carpetas hermanas; hoy re3/reVC están en `E:/Original` y reSA en `D:/Original`, así que esos enlaces no resuelven y al regenerar hay que pasar `--re3`/`--revc`.
- **Mapa de carpetas y relaciones funcionales:** orientación manual por responsabilidad. Puede ser uno a varios; no certifica equivalencia de clases, algoritmos, formatos ni comportamiento.
- **Inventario por nombre:** agrupa nombres de archivo iguales, sin distinguir mayúsculas/minúsculas y conservando la extensión. No compara el contenido ni demuestra que sean equivalentes.
- **Descripción:** explica el propósito del archivo o familia, distinguiendo declaraciones, implementaciones y recursos. Es orientación basada en nombre y ubicación, no una auditoría de cada función. En coincidencias nominales se toma reSA como referencia cuando está presente; las responsabilidades pueden diferir entre juegos.
- **—** significa que no hay una ruta para esa relación o un archivo con ese nombre en el árbol examinado. No significa que el juego carezca de esa función.
- Si hay varios archivos con el mismo nombre en un proyecto, se muestran todos en la misma celda. Se incluyen cabeceras, fuentes, shaders, recursos y documentación.
- Las relaciones funcionales pueden reaparecer en el inventario: este último cuenta cada archivo una sola vez y mantiene separadas las coincidencias nominales de las aproximaciones.

<a id="carpetas"></a>
## Mapa de carpetas por responsabilidad

En reSA muchos sistemas que re3/reVC agrupan en `core`, `control`, `renderer` o `vehicles` siguen en la raíz de `game_sa`. Las rutas siguientes son puntos de entrada representativos.

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [animation/](../../gta3/src/animation) | [animation/](../../gtavc/src/animation) | [Animation/](../source/game_sa/Animation)<br>[Plugins/RpAnimBlendPlugin/](../source/game_sa/Plugins/RpAnimBlendPlugin)<br>[Ragdoll/](../source/game_sa/Ragdoll) | Carga, organización y mezcla de animaciones; jerarquías y movimiento de huesos. |
| [audio/](../../gta3/src/audio) | [audio/](../../gtavc/src/audio) | [Audio/](../source/game_sa/Audio) | Reproducción de sonidos, voces, música y radio; carga de pistas y acceso al dispositivo de audio. |
| [buildings/](../../gta3/src/buildings) | [buildings/](../../gtavc/src/buildings) | [Entity/Building.cpp](../source/game_sa/Entity/Building.cpp)<br>[Entity/AnimatedBuilding.cpp](../source/game_sa/Entity/AnimatedBuilding.cpp) | Entidades que representan edificios y elementos estáticos del escenario. |
| [collision/](../../gta3/src/collision) | [collision/](../../gtavc/src/collision) | [Collision/](../source/game_sa/Collision) | Geometría y pruebas para detectar contactos e intersecciones entre objetos del mundo. |
| [control/](../../gta3/src/control) | [control/](../../gtavc/src/control) | [Scripts/](../source/game_sa/Scripts)<br>[Tasks/](../source/game_sa/Tasks)<br>[Events/](../source/game_sa/Events)<br>[DecisionMakers/](../source/game_sa/DecisionMakers)<br>[CarCtrl.cpp](../source/game_sa/CarCtrl.cpp)<br>[Population.cpp](../source/game_sa/Population.cpp) | Lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [core/](../../gta3/src/core) | [core/](../../gtavc/src/core) | [Core/](../source/game_sa/Core)<br>[Game.cpp](../source/game_sa/Game.cpp)<br>[Camera.cpp](../source/game_sa/Camera.cpp)<br>[World.cpp](../source/game_sa/World.cpp)<br>[Frontend/](../source/game_sa/Frontend) | Sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [entities/](../../gta3/src/entities) | [entities/](../../gtavc/src/entities) | [Entity/](../source/game_sa/Entity) | Bases de los objetos del mundo: posición, representación y propiedades físicas. |
| [extras/](../../gta3/src/extras) | [extras/](../../gtavc/src/extras) | [Pipelines/](../source/game_sa/Pipelines)<br>[PostEffects.cpp](../source/game_sa/PostEffects.cpp) | Extensiones del proyecto, herramientas y efectos gráficos adicionales; correspondencia parcial con reSA. |
| [fakerw/](../../gta3/src/fakerw) | [fakerw/](../../gtavc/src/fakerw) | [RenderWare/rw/](../source/game_sa/RenderWare/rw) | Capa de compatibilidad con la interfaz RenderWare; compartir interfaz no implica compartir implementación. |
| [math/](../../gta3/src/math) | [math/](../../gtavc/src/math) | [Core/](../source/game_sa/Core) | Vectores, matrices, cuaterniones y operaciones geométricas usadas por el motor. |
| [modelinfo/](../../gta3/src/modelinfo) | [modelinfo/](../../gtavc/src/modelinfo) | [Models/](../source/game_sa/Models) | Metadatos de modelos 3D, tipos de modelo y niveles de detalle. |
| [objects/](../../gta3/src/objects) | [objects/](../../gtavc/src/objects) | [Entity/Object/](../source/game_sa/Entity/Object)<br>[ObjectData.cpp](../source/game_sa/ObjectData.cpp)<br>[Fx/](../source/game_sa/Fx) | Objetos del escenario, sus propiedades e interacción; incluye objetos asociados a efectos. |
| [peds/](../../gta3/src/peds) | [peds/](../../gtavc/src/peds) | [Entity/Ped/](../source/game_sa/Entity/Ped)<br>[Tasks/](../source/game_sa/Tasks)<br>[Events/](../source/game_sa/Events)<br>[DecisionMakers/](../source/game_sa/DecisionMakers)<br>[Attractors/](../source/game_sa/Attractors) | Peatones y jugador: movimiento, combate, percepción, decisiones e interacción. |
| [renderer/](../../gta3/src/renderer) | [renderer/](../../gtavc/src/renderer) | [Renderer.cpp](../source/game_sa/Renderer.cpp)<br>[VisibilityPlugins.cpp](../source/game_sa/VisibilityPlugins.cpp)<br>[Fx/](../source/game_sa/Fx)<br>[Pipelines/](../source/game_sa/Pipelines)<br>[RenderWare/](../source/game_sa/RenderWare) | Dibujo de la escena, visibilidad, iluminación, sombras y efectos visuales. |
| [rw/](../../gta3/src/rw) | [rw/](../../gtavc/src/rw) | [RenderWare/](../source/game_sa/RenderWare)<br>[Plugins/](../source/game_sa/Plugins)<br>[TxdStore.cpp](../source/game_sa/TxdStore.cpp) | Integración gráfica con RenderWare, texturas y extensiones del motor. |
| [save/](../../gta3/src/save) | [save/](../../gtavc/src/save) | [GenericGameStorage.cpp](../source/game_sa/GenericGameStorage.cpp)<br>[C_PcSave.cpp](../source/game_sa/C_PcSave.cpp) | Lectura, escritura y organización de partidas guardadas. |
| [skel/](../../gta3/src/skel) | [skel/](../../gtavc/src/skel) | — | Arranque, ventanas y eventos de plataforma; su contraparte en reSA está fuera de game_sa, en source/app. |
| [text/](../../gta3/src/text) | [text/](../../gtavc/src/text) | [Text/](../source/game_sa/Text)<br>[Font.cpp](../source/game_sa/Font.cpp)<br>[Messages.cpp](../source/game_sa/Messages.cpp) | Carga y presentación de textos, mensajes y fuentes tipográficas. |
| [vehicles/](../../gta3/src/vehicles) | [vehicles/](../../gtavc/src/vehicles) | [Entity/Vehicle/](../source/game_sa/Entity/Vehicle)<br>[cHandlingDataMgr.cpp](../source/game_sa/cHandlingDataMgr.cpp)<br>[cTransmission.cpp](../source/game_sa/cTransmission.cpp) | Vehículos, conducción, transmisión, daños y propiedades de manejo. |
| [weapons/](../../gta3/src/weapons) | [weapons/](../../gtavc/src/weapons) | [Weapon.cpp](../source/game_sa/Weapon.cpp)<br>[WeaponInfo.cpp](../source/game_sa/WeaponInfo.cpp)<br>[WeaponEffects.cpp](../source/game_sa/WeaponEffects.cpp)<br>[Explosion.cpp](../source/game_sa/Explosion.cpp) | Armas, disparos, proyectiles, explosiones y sus efectos. |

`skel/` contiene integración de aplicación/plataforma. Su zona comparable en este proyecto es `source/app`, fuera del árbol reSA solicitado. Lo mismo ocurre con parte de las herramientas de `extras/`, cuya zona comparable es `source/toolsmenu`. `fakerw/` y `RenderWare/rw/` se relacionan con la interfaz de RenderWare, pero no implican el mismo backend ni la misma implementación.

<a id="funcionales"></a>
## Correspondencias funcionales con nombres distintos

### Generadores de vehículos

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [vehicles/CarGen.cpp](../../gta3/src/vehicles/CarGen.cpp)<br>[vehicles/CarGen.h](../../gta3/src/vehicles/CarGen.h) | [vehicles/CarGen.cpp](../../gtavc/src/vehicles/CarGen.cpp)<br>[vehicles/CarGen.h](../../gtavc/src/vehicles/CarGen.h) | [CarGenerator.cpp](../source/game_sa/CarGenerator.cpp)<br>[CarGenerator.h](../source/game_sa/CarGenerator.h)<br>[TheCarGenerators.cpp](../source/game_sa/TheCarGenerators.cpp)<br>[TheCarGenerators.h](../source/game_sa/TheCarGenerators.h) | Generadores de vehículos. El generador individual y el administrador están separados en reSA. |

### Propiedades de conducción

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [vehicles/HandlingMgr.cpp](../../gta3/src/vehicles/HandlingMgr.cpp)<br>[vehicles/HandlingMgr.h](../../gta3/src/vehicles/HandlingMgr.h) | [vehicles/HandlingMgr.cpp](../../gtavc/src/vehicles/HandlingMgr.cpp)<br>[vehicles/HandlingMgr.h](../../gtavc/src/vehicles/HandlingMgr.h) | [cHandlingDataMgr.cpp](../source/game_sa/cHandlingDataMgr.cpp)<br>[cHandlingDataMgr.h](../source/game_sa/cHandlingDataMgr.h)<br>[Handling/HandlingDataType.h](../source/game_sa/Handling/HandlingDataType.h) | Propiedades de conducción. Mismo ámbito de configuración de conducción; los tipos y formatos pueden cambiar. |

### Transmisión

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [vehicles/Transmission.cpp](../../gta3/src/vehicles/Transmission.cpp)<br>[vehicles/Transmission.h](../../gta3/src/vehicles/Transmission.h) | [vehicles/Transmission.cpp](../../gtavc/src/vehicles/Transmission.cpp)<br>[vehicles/Transmission.h](../../gtavc/src/vehicles/Transmission.h) | [cTransmission.cpp](../source/game_sa/cTransmission.cpp)<br>[cTransmission.h](../source/game_sa/cTransmission.h) | Transmisión. Correspondencia por la clase y el subsistema de transmisión. |

### Configuración de controles

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/ControllerConfig.cpp](../../gta3/src/core/ControllerConfig.cpp)<br>[core/ControllerConfig.h](../../gta3/src/core/ControllerConfig.h) | [core/ControllerConfig.cpp](../../gtavc/src/core/ControllerConfig.cpp)<br>[core/ControllerConfig.h](../../gtavc/src/core/ControllerConfig.h) | [ControllerConfigManager.cpp](../source/game_sa/ControllerConfigManager.cpp)<br>[ControllerConfigManager.h](../source/game_sa/ControllerConfigManager.h) | Configuración de controles. Administración de la configuración de controles. |

### Zonas del mundo

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/Zones.cpp](../../gta3/src/core/Zones.cpp)<br>[core/Zones.h](../../gta3/src/core/Zones.h) | [core/Zones.cpp](../../gtavc/src/core/Zones.cpp)<br>[core/Zones.h](../../gtavc/src/core/Zones.h) | [TheZones.cpp](../source/game_sa/TheZones.cpp)<br>[TheZones.h](../source/game_sa/TheZones.h)<br>[Zone.cpp](../source/game_sa/Zone.cpp)<br>[Zone.h](../source/game_sa/Zone.h) | Zonas del mundo. En reSA se distinguen el administrador y los datos de una zona. |

### Zonas de culling

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/ZoneCull.cpp](../../gta3/src/core/ZoneCull.cpp)<br>[core/ZoneCull.h](../../gta3/src/core/ZoneCull.h) | [core/ZoneCull.cpp](../../gtavc/src/core/ZoneCull.cpp)<br>[core/ZoneCull.h](../../gtavc/src/core/ZoneCull.h) | [CullZones.cpp](../source/game_sa/CullZones.cpp)<br>[CullZones.h](../source/game_sa/CullZones.h) | Zonas de culling. Zonas que modifican visibilidad y otras propiedades del entorno. |

### Superficies

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/SurfaceTable.cpp](../../gta3/src/core/SurfaceTable.cpp)<br>[core/SurfaceTable.h](../../gta3/src/core/SurfaceTable.h) | [core/SurfaceTable.cpp](../../gtavc/src/core/SurfaceTable.cpp)<br>[core/SurfaceTable.h](../../gtavc/src/core/SurfaceTable.h) | [SurfaceInfos_c.cpp](../source/game_sa/SurfaceInfos_c.cpp)<br>[SurfaceInfos_c.h](../source/game_sa/SurfaceInfos_c.h)<br>[SurfaceInfo_c.h](../source/game_sa/SurfaceInfo_c.h) | Superficies. Propiedades de superficies; es una relación funcional, no identidad de tablas. |

### Lectura de datos del disco

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/CdStream.cpp](../../gta3/src/core/CdStream.cpp)<br>[core/CdStream.h](../../gta3/src/core/CdStream.h)<br>[core/CdStreamPosix.cpp](../../gta3/src/core/CdStreamPosix.cpp) | [core/CdStream.cpp](../../gtavc/src/core/CdStream.cpp)<br>[core/CdStream.h](../../gtavc/src/core/CdStream.h)<br>[core/CdStreamPosix.cpp](../../gtavc/src/core/CdStreamPosix.cpp) | [CdStreamInfo.cpp](../source/game_sa/CdStreamInfo.cpp)<br>[CdStreamInfo.h](../source/game_sa/CdStreamInfo.h) | Lectura de datos del disco. Área de streaming de datos. Las implementaciones de plataforma no son intercambiables. |

### Menús

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [core/Frontend.cpp](../../gta3/src/core/Frontend.cpp)<br>[core/Frontend.h](../../gta3/src/core/Frontend.h)<br>[core/MenuScreens.cpp](../../gta3/src/core/MenuScreens.cpp) | [core/Frontend.cpp](../../gtavc/src/core/Frontend.cpp)<br>[core/Frontend.h](../../gtavc/src/core/Frontend.h)<br>[core/MenuScreens.cpp](../../gtavc/src/core/MenuScreens.cpp) | [MenuManager.cpp](../source/game_sa/MenuManager.cpp)<br>[MenuManager.h](../source/game_sa/MenuManager.h)<br>[Frontend/](../source/game_sa/Frontend) | Menús. reSA distribuye dibujo, entrada, páginas y procesamiento entre varios archivos. |

### Scripts y opcodes

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [control/Script.cpp](../../gta3/src/control/Script.cpp)<br>[control/Script.h](../../gta3/src/control/Script.h)<br>[control/Script2.cpp](../../gta3/src/control/Script2.cpp)<br>[control/Script3.cpp](../../gta3/src/control/Script3.cpp)<br>[control/Script4.cpp](../../gta3/src/control/Script4.cpp)<br>[control/Script5.cpp](../../gta3/src/control/Script5.cpp)<br>[control/Script6.cpp](../../gta3/src/control/Script6.cpp) | [control/Script.cpp](../../gtavc/src/control/Script.cpp)<br>[control/Script.h](../../gtavc/src/control/Script.h)<br>[control/Script2.cpp](../../gtavc/src/control/Script2.cpp)<br>[control/Script3.cpp](../../gtavc/src/control/Script3.cpp)<br>[control/Script4.cpp](../../gtavc/src/control/Script4.cpp)<br>[control/Script5.cpp](../../gtavc/src/control/Script5.cpp)<br>[control/Script6.cpp](../../gtavc/src/control/Script6.cpp)<br>[control/Script7.cpp](../../gtavc/src/control/Script7.cpp)<br>[control/Script8.cpp](../../gtavc/src/control/Script8.cpp) | [Scripts/TheScripts.cpp](../source/game_sa/Scripts/TheScripts.cpp)<br>[Scripts/TheScripts.h](../source/game_sa/Scripts/TheScripts.h)<br>[Scripts/RunningScript.cpp](../source/game_sa/Scripts/RunningScript.cpp)<br>[Scripts/RunningScript.h](../source/game_sa/Scripts/RunningScript.h)<br>[Scripts/Commands/](../source/game_sa/Scripts/Commands)<br>[Scripts/CommandParser/](../source/game_sa/Scripts/CommandParser) | Scripts y opcodes. El sistema de scripts se divide en ejecución, administración y comandos temáticos en reSA. |

### Identificadores de opcodes

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [control/ScriptCommands.h](../../gta3/src/control/ScriptCommands.h) | [control/ScriptCommands.h](../../gtavc/src/control/ScriptCommands.h) | [Enums/eScriptCommands.h](../source/game_sa/Enums/eScriptCommands.h) | Identificadores de opcodes. Listas de identificadores. No presupone igualdad de números ni argumentos entre juegos. |

### Identificadores de animaciones

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [animation/AnimationId.h](../../gta3/src/animation/AnimationId.h) | [animation/AnimationId.h](../../gtavc/src/animation/AnimationId.h) | [Enums/AnimationEnums.h](../source/game_sa/Enums/AnimationEnums.h) | Identificadores de animaciones. Correspondencia de propósito; cada juego tiene su propio catálogo. |

### Guardado en PC

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [save/PCSave.cpp](../../gta3/src/save/PCSave.cpp)<br>[save/PCSave.h](../../gta3/src/save/PCSave.h) | [save/PCSave.cpp](../../gtavc/src/save/PCSave.cpp)<br>[save/PCSave.h](../../gtavc/src/save/PCSave.h) | [C_PcSave.cpp](../source/game_sa/C_PcSave.cpp)<br>[C_PcSave.h](../source/game_sa/C_PcSave.h) | Guardado en PC. Mismo ámbito de guardado en PC; no implica compatibilidad de partidas. |

### Modelos simples y LOD

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [modelinfo/SimpleModelInfo.cpp](../../gta3/src/modelinfo/SimpleModelInfo.cpp)<br>[modelinfo/SimpleModelInfo.h](../../gta3/src/modelinfo/SimpleModelInfo.h) | [modelinfo/SimpleModelInfo.cpp](../../gtavc/src/modelinfo/SimpleModelInfo.cpp)<br>[modelinfo/SimpleModelInfo.h](../../gtavc/src/modelinfo/SimpleModelInfo.h) | [Models/AtomicModelInfo.cpp](../source/game_sa/Models/AtomicModelInfo.cpp)<br>[Models/AtomicModelInfo.h](../source/game_sa/Models/AtomicModelInfo.h)<br>[Models/LodAtomicModelInfo.cpp](../source/game_sa/Models/LodAtomicModelInfo.cpp) | Modelos simples y LOD. Relación aproximada: reSA reorganiza los modelos atómicos y los niveles de detalle. |

### Interfaz de audio

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/DMAudio.cpp](../../gta3/src/audio/DMAudio.cpp)<br>[audio/DMAudio.h](../../gta3/src/audio/DMAudio.h) | [audio/DMAudio.cpp](../../gtavc/src/audio/DMAudio.cpp)<br>[audio/DMAudio.h](../../gtavc/src/audio/DMAudio.h) | [Audio/AudioEngine.cpp](../source/game_sa/Audio/AudioEngine.cpp)<br>[Audio/AudioEngine.h](../source/game_sa/Audio/AudioEngine.h) | Interfaz de audio. Punto de entrada del sistema de sonido; cDMAudio y CAudioEngine son interfaces distintas. |

### Gestión y lógica de audio

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/AudioManager.cpp](../../gta3/src/audio/AudioManager.cpp)<br>[audio/AudioManager.h](../../gta3/src/audio/AudioManager.h)<br>[audio/AudioLogic.cpp](../../gta3/src/audio/AudioLogic.cpp) | [audio/AudioManager.cpp](../../gtavc/src/audio/AudioManager.cpp)<br>[audio/AudioManager.h](../../gtavc/src/audio/AudioManager.h)<br>[audio/AudioLogic.cpp](../../gtavc/src/audio/AudioLogic.cpp) | [Audio/Managers/](../source/game_sa/Audio/Managers)<br>[Audio/Entities/](../source/game_sa/Audio/Entities)<br>[Audio/Hardware/](../source/game_sa/Audio/Hardware) | Gestión y lógica de audio. Correspondencia de subsistema: reSA reparte responsabilidades entre gestores, entidades y hardware. |

### Música y radio

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/MusicManager.cpp](../../gta3/src/audio/MusicManager.cpp)<br>[audio/MusicManager.h](../../gta3/src/audio/MusicManager.h) | [audio/MusicManager.cpp](../../gtavc/src/audio/MusicManager.cpp)<br>[audio/MusicManager.h](../../gtavc/src/audio/MusicManager.h) | [Audio/Managers/AERadioTrackManager.cpp](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp)<br>[Audio/Managers/AEAmbienceTrackManager.cpp](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp)<br>[Audio/Managers/AECutsceneTrackManager.cpp](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) | Música y radio. reSA tiene gestores especializados; no existe aquí una equivalencia uno a uno. |

### Radio policial

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/PolRadio.cpp](../../gta3/src/audio/PolRadio.cpp)<br>[audio/PolRadio.h](../../gta3/src/audio/PolRadio.h) | [audio/PolRadio.cpp](../../gtavc/src/audio/PolRadio.cpp)<br>[audio/PolRadio.h](../../gtavc/src/audio/PolRadio.h) | [Audio/Entities/AEPoliceScannerAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp)<br>[Audio/Entities/AEPoliceScannerAudioEntity.h](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h) | Radio policial. Mismo ámbito funcional de mensajes de radio policial. |

### Sonido de colisiones

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/AudioCollision.cpp](../../gta3/src/audio/AudioCollision.cpp)<br>[audio/AudioCollision.h](../../gta3/src/audio/AudioCollision.h) | [audio/AudioCollision.cpp](../../gtavc/src/audio/AudioCollision.cpp)<br>[audio/AudioCollision.h](../../gtavc/src/audio/AudioCollision.h) | [Audio/Entities/AECollisionAudioEntity.cpp](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp)<br>[Audio/Entities/AECollisionAudioEntity.h](../source/game_sa/Audio/Entities/AECollisionAudioEntity.h) | Sonido de colisiones. Mismo ámbito funcional, integrado como entidad de audio en reSA. |

### Comportamiento de peatones

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [peds/PedAI.cpp](../../gta3/src/peds/PedAI.cpp)<br>[peds/PedFight.cpp](../../gta3/src/peds/PedFight.cpp)<br>[peds/PedChat.cpp](../../gta3/src/peds/PedChat.cpp) | [peds/PedAI.cpp](../../gtavc/src/peds/PedAI.cpp)<br>[peds/PedFight.cpp](../../gtavc/src/peds/PedFight.cpp)<br>[peds/PedChat.cpp](../../gtavc/src/peds/PedChat.cpp) | [Entity/Ped/Ped.cpp](../source/game_sa/Entity/Ped/Ped.cpp)<br>[Tasks/](../source/game_sa/Tasks)<br>[Events/](../source/game_sa/Events)<br>[DecisionMakers/](../source/game_sa/DecisionMakers)<br>[Conversations/](../source/game_sa/Conversations) | Comportamiento de peatones. Relación amplia: la IA de reSA está organizada en tareas, eventos y decisiones. |

### Partículas

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [renderer/Particle.cpp](../../gta3/src/renderer/Particle.cpp)<br>[renderer/Particle.h](../../gta3/src/renderer/Particle.h)<br>[renderer/ParticleMgr.cpp](../../gta3/src/renderer/ParticleMgr.cpp)<br>[renderer/ParticleMgr.h](../../gta3/src/renderer/ParticleMgr.h)<br>[objects/ParticleObject.cpp](../../gta3/src/objects/ParticleObject.cpp) | [renderer/Particle.cpp](../../gtavc/src/renderer/Particle.cpp)<br>[renderer/Particle.h](../../gtavc/src/renderer/Particle.h)<br>[renderer/ParticleMgr.cpp](../../gtavc/src/renderer/ParticleMgr.cpp)<br>[renderer/ParticleMgr.h](../../gtavc/src/renderer/ParticleMgr.h)<br>[objects/ParticleObject.cpp](../../gtavc/src/objects/ParticleObject.cpp) | [Fx/](../source/game_sa/Fx) | Partículas. Sistemas de efectos distintos. Compartir propósito o el nombre Particle.h no demuestra equivalencia. |

### Tipos de arma

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [weapons/WeaponType.h](../../gta3/src/weapons/WeaponType.h) | [weapons/WeaponType.h](../../gtavc/src/weapons/WeaponType.h) | [Enums/eWeaponType.h](../source/game_sa/Enums/eWeaponType.h) | Tipos de arma. Enumeraciones de armas de cada juego; comprobar valores antes de comparar lógica. |

## Resumen del inventario nominal

| Grupo | Nombres de archivo |
| --- | ---: |
| Coinciden reSA y al menos uno de re3/reVC | 309 |
| Presentes en re3/reVC sin el mismo nombre en reSA | 280 |
| Presentes en reSA sin el mismo nombre en re3/reVC | 1809 |

Una fila puede contener varias rutas. Los totales de archivos de las tres raíces aparecen al principio.

<a id="coincidencias"></a>
## Archivos con el mismo nombre en reSA y re3/reVC

Orden alfabético por nombre de archivo. Son candidatos para comparar; validar clases y contenido antes de trasladar código.

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [renderer/2dEffect.h](../../gta3/src/renderer/2dEffect.h) | [renderer/2dEffect.h](../../gtavc/src/renderer/2dEffect.h) | [Plugins/TwoDEffectPlugin/2dEffect.h](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.h) | Declaraciones y tipos de `2dEffect`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| [core/Accident.cpp](../../gta3/src/core/Accident.cpp) | [core/Accident.cpp](../../gtavc/src/core/Accident.cpp) | [Accident.cpp](../source/game_sa/Accident.cpp) | Implementación de registro y gestión de accidentes. |
| [core/Accident.h](../../gta3/src/core/Accident.h) | [core/Accident.h](../../gtavc/src/core/Accident.h) | [Accident.h](../source/game_sa/Accident.h) | Declaraciones y tipos de registro y gestión de accidentes. |
| [animation/AnimBlendAssocGroup.cpp](../../gta3/src/animation/AnimBlendAssocGroup.cpp) | [animation/AnimBlendAssocGroup.cpp](../../gtavc/src/animation/AnimBlendAssocGroup.cpp) | [Animation/AnimBlendAssocGroup.cpp](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) | Implementación del componente `AnimBlendAssocGroup` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendAssocGroup.h](../../gta3/src/animation/AnimBlendAssocGroup.h) | [animation/AnimBlendAssocGroup.h](../../gtavc/src/animation/AnimBlendAssocGroup.h) | [Animation/AnimBlendAssocGroup.h](../source/game_sa/Animation/AnimBlendAssocGroup.h) | Declaraciones y tipos del componente `AnimBlendAssocGroup` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendAssociation.cpp](../../gta3/src/animation/AnimBlendAssociation.cpp) | [animation/AnimBlendAssociation.cpp](../../gtavc/src/animation/AnimBlendAssociation.cpp) | [Animation/AnimBlendAssociation.cpp](../source/game_sa/Animation/AnimBlendAssociation.cpp) | Implementación del componente `AnimBlendAssociation` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendAssociation.h](../../gta3/src/animation/AnimBlendAssociation.h) | [animation/AnimBlendAssociation.h](../../gtavc/src/animation/AnimBlendAssociation.h) | [Animation/AnimBlendAssociation.h](../source/game_sa/Animation/AnimBlendAssociation.h) | Declaraciones y tipos del componente `AnimBlendAssociation` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendClumpData.cpp](../../gta3/src/animation/AnimBlendClumpData.cpp) | [animation/AnimBlendClumpData.cpp](../../gtavc/src/animation/AnimBlendClumpData.cpp) | [Animation/AnimBlendClumpData.cpp](../source/game_sa/Animation/AnimBlendClumpData.cpp) | Implementación del componente `AnimBlendClumpData` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendClumpData.h](../../gta3/src/animation/AnimBlendClumpData.h) | [animation/AnimBlendClumpData.h](../../gtavc/src/animation/AnimBlendClumpData.h) | [Animation/AnimBlendClumpData.h](../source/game_sa/Animation/AnimBlendClumpData.h) | Declaraciones y tipos del componente `AnimBlendClumpData` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendHierarchy.cpp](../../gta3/src/animation/AnimBlendHierarchy.cpp) | [animation/AnimBlendHierarchy.cpp](../../gtavc/src/animation/AnimBlendHierarchy.cpp) | [Animation/AnimBlendHierarchy.cpp](../source/game_sa/Animation/AnimBlendHierarchy.cpp) | Implementación del componente `AnimBlendHierarchy` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendHierarchy.h](../../gta3/src/animation/AnimBlendHierarchy.h) | [animation/AnimBlendHierarchy.h](../../gtavc/src/animation/AnimBlendHierarchy.h) | [Animation/AnimBlendHierarchy.h](../source/game_sa/Animation/AnimBlendHierarchy.h) | Declaraciones y tipos del componente `AnimBlendHierarchy` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendNode.cpp](../../gta3/src/animation/AnimBlendNode.cpp) | [animation/AnimBlendNode.cpp](../../gtavc/src/animation/AnimBlendNode.cpp) | [Animation/AnimBlendNode.cpp](../source/game_sa/Animation/AnimBlendNode.cpp) | Implementación del componente `AnimBlendNode` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendNode.h](../../gta3/src/animation/AnimBlendNode.h) | [animation/AnimBlendNode.h](../../gtavc/src/animation/AnimBlendNode.h) | [Animation/AnimBlendNode.h](../source/game_sa/Animation/AnimBlendNode.h) | Declaraciones y tipos del componente `AnimBlendNode` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendSequence.cpp](../../gta3/src/animation/AnimBlendSequence.cpp) | [animation/AnimBlendSequence.cpp](../../gtavc/src/animation/AnimBlendSequence.cpp) | [Animation/AnimBlendSequence.cpp](../source/game_sa/Animation/AnimBlendSequence.cpp) | Implementación del componente `AnimBlendSequence` de organización, datos o mezcla de animaciones. |
| [animation/AnimBlendSequence.h](../../gta3/src/animation/AnimBlendSequence.h) | [animation/AnimBlendSequence.h](../../gtavc/src/animation/AnimBlendSequence.h) | [Animation/AnimBlendSequence.h](../source/game_sa/Animation/AnimBlendSequence.h) | Declaraciones y tipos del componente `AnimBlendSequence` de organización, datos o mezcla de animaciones. |
| [animation/AnimManager.cpp](../../gta3/src/animation/AnimManager.cpp) | [animation/AnimManager.cpp](../../gtavc/src/animation/AnimManager.cpp) | [Animation/AnimManager.cpp](../source/game_sa/Animation/AnimManager.cpp) | Implementación del componente `AnimManager` de organización, datos o mezcla de animaciones. |
| [animation/AnimManager.h](../../gta3/src/animation/AnimManager.h) | [animation/AnimManager.h](../../gtavc/src/animation/AnimManager.h) | [Animation/AnimManager.h](../source/game_sa/Animation/AnimManager.h) | Declaraciones y tipos del componente `AnimManager` de organización, datos o mezcla de animaciones. |
| [vehicles/Automobile.cpp](../../gta3/src/vehicles/Automobile.cpp) | [vehicles/Automobile.cpp](../../gtavc/src/vehicles/Automobile.cpp) | [Entity/Vehicle/Automobile.cpp](../source/game_sa/Entity/Vehicle/Automobile.cpp) | Implementación de automóviles y su comportamiento físico. |
| [vehicles/Automobile.h](../../gta3/src/vehicles/Automobile.h) | [vehicles/Automobile.h](../../gtavc/src/vehicles/Automobile.h) | [Entity/Vehicle/Automobile.h](../source/game_sa/Entity/Vehicle/Automobile.h) | Declaraciones y tipos de automóviles y su comportamiento físico. |
| [control/AutoPilot.cpp](../../gta3/src/control/AutoPilot.cpp) | [control/AutoPilot.cpp](../../gtavc/src/control/AutoPilot.cpp) | [AutoPilot.cpp](../source/game_sa/AutoPilot.cpp) | Implementación de conducción automática, tráfico y control de vehículos por la IA. |
| [control/AutoPilot.h](../../gta3/src/control/AutoPilot.h) | [control/AutoPilot.h](../../gtavc/src/control/AutoPilot.h) | [AutoPilot.h](../source/game_sa/AutoPilot.h) | Declaraciones y tipos de conducción automática, tráfico y control de vehículos por la IA. |
| [modelinfo/BaseModelInfo.cpp](../../gta3/src/modelinfo/BaseModelInfo.cpp) | [modelinfo/BaseModelInfo.cpp](../../gtavc/src/modelinfo/BaseModelInfo.cpp) | [Models/BaseModelInfo.cpp](../source/game_sa/Models/BaseModelInfo.cpp) | Implementación de los metadatos de modelo `BaseModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/BaseModelInfo.h](../../gta3/src/modelinfo/BaseModelInfo.h) | [modelinfo/BaseModelInfo.h](../../gtavc/src/modelinfo/BaseModelInfo.h) | [Models/BaseModelInfo.h](../source/game_sa/Models/BaseModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `BaseModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | [vehicles/Bike.cpp](../../gtavc/src/vehicles/Bike.cpp) | [Entity/Vehicle/Bike.cpp](../source/game_sa/Entity/Vehicle/Bike.cpp) | Implementación de motocicletas o bicicletas, según la clase. |
| [vehicles/Bike.h](../../gta3/src/vehicles/Bike.h) | [vehicles/Bike.h](../../gtavc/src/vehicles/Bike.h) | [Entity/Vehicle/Bike.h](../source/game_sa/Entity/Vehicle/Bike.h) | Declaraciones y tipos de motocicletas o bicicletas, según la clase. |
| [vehicles/Boat.cpp](../../gta3/src/vehicles/Boat.cpp) | [vehicles/Boat.cpp](../../gtavc/src/vehicles/Boat.cpp) | [Entity/Vehicle/Boat.cpp](../source/game_sa/Entity/Vehicle/Boat.cpp) | Implementación de embarcaciones y navegación. |
| [vehicles/Boat.h](../../gta3/src/vehicles/Boat.h) | [vehicles/Boat.h](../../gtavc/src/vehicles/Boat.h) | [Entity/Vehicle/Boat.h](../source/game_sa/Entity/Vehicle/Boat.h) | Declaraciones y tipos de embarcaciones y navegación. |
| [control/Bridge.cpp](../../gta3/src/control/Bridge.cpp) | [control/Bridge.cpp](../../gtavc/src/control/Bridge.cpp) | [Bridge.cpp](../source/game_sa/Bridge.cpp) | Implementación de puentes y su lógica. |
| [control/Bridge.h](../../gta3/src/control/Bridge.h) | [control/Bridge.h](../../gtavc/src/control/Bridge.h) | [Bridge.h](../source/game_sa/Bridge.h) | Declaraciones y tipos de puentes y su lógica. |
| [buildings/Building.cpp](../../gta3/src/buildings/Building.cpp) | [buildings/Building.cpp](../../gtavc/src/buildings/Building.cpp) | [Entity/Building.cpp](../source/game_sa/Entity/Building.cpp) | Implementación de representación de edificios y geometría del escenario. |
| [buildings/Building.h](../../gta3/src/buildings/Building.h) | [buildings/Building.h](../../gtavc/src/buildings/Building.h) | [Entity/Building.h](../source/game_sa/Entity/Building.h) | Declaraciones y tipos de representación de edificios y geometría del escenario. |
| [weapons/BulletInfo.cpp](../../gta3/src/weapons/BulletInfo.cpp) | [weapons/BulletInfo.cpp](../../gtavc/src/weapons/BulletInfo.cpp) | [BulletInfo.cpp](../source/game_sa/BulletInfo.cpp) | Implementación de proyectiles, balas y datos de disparos. |
| [weapons/BulletInfo.h](../../gta3/src/weapons/BulletInfo.h) | [weapons/BulletInfo.h](../../gtavc/src/weapons/BulletInfo.h) | [BulletInfo.h](../source/game_sa/BulletInfo.h) | Declaraciones y tipos de proyectiles, balas y datos de disparos. |
| [core/Cam.cpp](../../gta3/src/core/Cam.cpp) | [core/Cam.cpp](../../gtavc/src/core/Cam.cpp) | [Cam.cpp](../source/game_sa/Cam.cpp) | Implementación de cámaras del juego, modos de vista y seguimiento. |
| [core/Camera.cpp](../../gta3/src/core/Camera.cpp) | [core/Camera.cpp](../../gtavc/src/core/Camera.cpp) | [Camera.cpp](../source/game_sa/Camera.cpp) | Implementación de cámaras del juego, modos de vista y seguimiento. |
| [core/Camera.h](../../gta3/src/core/Camera.h) | [core/Camera.h](../../gtavc/src/core/Camera.h) | [Camera.h](../source/game_sa/Camera.h) | Declaraciones y tipos de cámaras del juego, modos de vista y seguimiento. |
| [control/CarAI.cpp](../../gta3/src/control/CarAI.cpp) | [control/CarAI.cpp](../../gtavc/src/control/CarAI.cpp) | [CarAI.cpp](../source/game_sa/CarAI.cpp) | Implementación de conducción automática, tráfico y control de vehículos por la IA. |
| [control/CarAI.h](../../gta3/src/control/CarAI.h) | [control/CarAI.h](../../gtavc/src/control/CarAI.h) | [CarAI.h](../source/game_sa/CarAI.h) | Declaraciones y tipos de conducción automática, tráfico y control de vehículos por la IA. |
| [control/CarCtrl.cpp](../../gta3/src/control/CarCtrl.cpp) | [control/CarCtrl.cpp](../../gtavc/src/control/CarCtrl.cpp) | [CarCtrl.cpp](../source/game_sa/CarCtrl.cpp) | Implementación de conducción automática, tráfico y control de vehículos por la IA. |
| [control/CarCtrl.h](../../gta3/src/control/CarCtrl.h) | [control/CarCtrl.h](../../gtavc/src/control/CarCtrl.h) | [CarCtrl.h](../source/game_sa/CarCtrl.h) | Declaraciones y tipos de conducción automática, tráfico y control de vehículos por la IA. |
| [peds/CivilianPed.cpp](../../gta3/src/peds/CivilianPed.cpp) | [peds/CivilianPed.cpp](../../gtavc/src/peds/CivilianPed.cpp) | [Entity/Ped/CivilianPed.cpp](../source/game_sa/Entity/Ped/CivilianPed.cpp) | Implementación de peatones civiles. |
| [peds/CivilianPed.h](../../gta3/src/peds/CivilianPed.h) | [peds/CivilianPed.h](../../gtavc/src/peds/CivilianPed.h) | [Entity/Ped/CivilianPed.h](../source/game_sa/Entity/Ped/CivilianPed.h) | Declaraciones y tipos de peatones civiles. |
| [core/Clock.cpp](../../gta3/src/core/Clock.cpp) | [core/Clock.cpp](../../gtavc/src/core/Clock.cpp) | [Clock.cpp](../source/game_sa/Clock.cpp) | Implementación de hora y calendario del juego. |
| [core/Clock.h](../../gta3/src/core/Clock.h) | [core/Clock.h](../../gtavc/src/core/Clock.h) | [Clock.h](../source/game_sa/Clock.h) | Declaraciones y tipos de hora y calendario del juego. |
| [renderer/Clouds.cpp](../../gta3/src/renderer/Clouds.cpp) | [renderer/Clouds.cpp](../../gtavc/src/renderer/Clouds.cpp) | [Clouds.cpp](../source/game_sa/Clouds.cpp) | Implementación de nubes y su representación visual. |
| [renderer/Clouds.h](../../gta3/src/renderer/Clouds.h) | [renderer/Clouds.h](../../gtavc/src/renderer/Clouds.h) | [Clouds.h](../source/game_sa/Clouds.h) | Declaraciones y tipos de nubes y su representación visual. |
| [modelinfo/ClumpModelInfo.cpp](../../gta3/src/modelinfo/ClumpModelInfo.cpp) | [modelinfo/ClumpModelInfo.cpp](../../gtavc/src/modelinfo/ClumpModelInfo.cpp) | [Models/ClumpModelInfo.cpp](../source/game_sa/Models/ClumpModelInfo.cpp) | Implementación de los metadatos de modelo `ClumpModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/ClumpModelInfo.h](../../gta3/src/modelinfo/ClumpModelInfo.h) | [modelinfo/ClumpModelInfo.h](../../gtavc/src/modelinfo/ClumpModelInfo.h) | [Models/ClumpModelInfo.h](../source/game_sa/Models/ClumpModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `ClumpModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [collision/ColBox.cpp](../../gta3/src/collision/ColBox.cpp) | [collision/ColBox.cpp](../../gtavc/src/collision/ColBox.cpp) | [Collision/ColBox.cpp](../source/game_sa/Collision/ColBox.cpp) | Implementación del componente geométrico `ColBox` usado en detección y datos de colisiones. |
| [collision/ColBox.h](../../gta3/src/collision/ColBox.h) | [collision/ColBox.h](../../gtavc/src/collision/ColBox.h) | [Collision/ColBox.h](../source/game_sa/Collision/ColBox.h) | Declaraciones y tipos del componente geométrico `ColBox` usado en detección y datos de colisiones. |
| [collision/ColLine.cpp](../../gta3/src/collision/ColLine.cpp) | [collision/ColLine.cpp](../../gtavc/src/collision/ColLine.cpp) | [Collision/ColLine.cpp](../source/game_sa/Collision/ColLine.cpp) | Implementación del componente geométrico `ColLine` usado en detección y datos de colisiones. |
| [collision/ColLine.h](../../gta3/src/collision/ColLine.h) | [collision/ColLine.h](../../gtavc/src/collision/ColLine.h) | [Collision/ColLine.h](../source/game_sa/Collision/ColLine.h) | Declaraciones y tipos del componente geométrico `ColLine` usado en detección y datos de colisiones. |
| [collision/Collision.cpp](../../gta3/src/collision/Collision.cpp) | [collision/Collision.cpp](../../gtavc/src/collision/Collision.cpp) | [Collision/Collision.cpp](../source/game_sa/Collision/Collision.cpp) | Implementación del componente geométrico `Collision` usado en detección y datos de colisiones. |
| [collision/Collision.h](../../gta3/src/collision/Collision.h) | [collision/Collision.h](../../gtavc/src/collision/Collision.h) | [Collision/Collision.h](../source/game_sa/Collision/Collision.h) | Declaraciones y tipos del componente geométrico `Collision` usado en detección y datos de colisiones. |
| [collision/ColModel.cpp](../../gta3/src/collision/ColModel.cpp) | [collision/ColModel.cpp](../../gtavc/src/collision/ColModel.cpp) | [Collision/ColModel.cpp](../source/game_sa/Collision/ColModel.cpp) | Implementación del componente geométrico `ColModel` usado en detección y datos de colisiones. |
| [collision/ColModel.h](../../gta3/src/collision/ColModel.h) | [collision/ColModel.h](../../gtavc/src/collision/ColModel.h) | [Collision/ColModel.h](../source/game_sa/Collision/ColModel.h) | Declaraciones y tipos del componente geométrico `ColModel` usado en detección y datos de colisiones. |
| [collision/ColPoint.cpp](../../gta3/src/collision/ColPoint.cpp) | [collision/ColPoint.cpp](../../gtavc/src/collision/ColPoint.cpp) | [Collision/ColPoint.cpp](../source/game_sa/Collision/ColPoint.cpp) | Implementación del componente geométrico `ColPoint` usado en detección y datos de colisiones. |
| [collision/ColPoint.h](../../gta3/src/collision/ColPoint.h) | [collision/ColPoint.h](../../gtavc/src/collision/ColPoint.h) | [Collision/ColPoint.h](../source/game_sa/Collision/ColPoint.h) | Declaraciones y tipos del componente geométrico `ColPoint` usado en detección y datos de colisiones. |
| [collision/ColSphere.cpp](../../gta3/src/collision/ColSphere.cpp) | [collision/ColSphere.cpp](../../gtavc/src/collision/ColSphere.cpp) | [Collision/ColSphere.cpp](../source/game_sa/Collision/ColSphere.cpp) | Implementación del componente geométrico `ColSphere` usado en detección y datos de colisiones. |
| [collision/ColSphere.h](../../gta3/src/collision/ColSphere.h) | [collision/ColSphere.h](../../gtavc/src/collision/ColSphere.h) | [Collision/ColSphere.h](../source/game_sa/Collision/ColSphere.h) | Declaraciones y tipos del componente geométrico `ColSphere` usado en detección y datos de colisiones. |
| — | [collision/ColStore.cpp](../../gtavc/src/collision/ColStore.cpp) | [Collision/ColStore.cpp](../source/game_sa/Collision/ColStore.cpp) | Implementación del componente geométrico `ColStore` usado en detección y datos de colisiones. |
| — | [collision/ColStore.h](../../gtavc/src/collision/ColStore.h) | [Collision/ColStore.h](../source/game_sa/Collision/ColStore.h) | Declaraciones y tipos del componente geométrico `ColStore` usado en detección y datos de colisiones. |
| [collision/ColTriangle.cpp](../../gta3/src/collision/ColTriangle.cpp) | [collision/ColTriangle.cpp](../../gtavc/src/collision/ColTriangle.cpp) | [Collision/ColTriangle.cpp](../source/game_sa/Collision/ColTriangle.cpp) | Implementación del componente geométrico `ColTriangle` usado en detección y datos de colisiones. |
| [collision/ColTriangle.h](../../gta3/src/collision/ColTriangle.h) | [collision/ColTriangle.h](../../gtavc/src/collision/ColTriangle.h) | [Collision/ColTriangle.h](../source/game_sa/Collision/ColTriangle.h) | Declaraciones y tipos del componente geométrico `ColTriangle` usado en detección y datos de colisiones. |
| [core/common.h](../../gta3/src/core/common.h) | [core/common.h](../../gtavc/src/core/common.h) | [common.h](../source/game_sa/common.h) | Declaraciones y tipos de utilidades, definiciones u operaciones compartidas. |
| [collision/CompressedVector.h](../../gta3/src/collision/CompressedVector.h) | [collision/CompressedVector.h](../../gtavc/src/collision/CompressedVector.h) | [CompressedVector.h](../source/game_sa/CompressedVector.h) | Declaraciones y tipos de representaciones compactas de datos geométricos. |
| [peds/CopPed.cpp](../../gta3/src/peds/CopPed.cpp) | [peds/CopPed.cpp](../../gtavc/src/peds/CopPed.cpp) | [Entity/Ped/CopPed.cpp](../source/game_sa/Entity/Ped/CopPed.cpp) | Implementación de personajes de policía. |
| [peds/CopPed.h](../../gta3/src/peds/CopPed.h) | [peds/CopPed.h](../../gtavc/src/peds/CopPed.h) | [Entity/Ped/CopPed.h](../source/game_sa/Entity/Ped/CopPed.h) | Declaraciones y tipos de personajes de policía. |
| [renderer/Coronas.cpp](../../gta3/src/renderer/Coronas.cpp) | [renderer/Coronas.cpp](../../gtavc/src/renderer/Coronas.cpp) | [Coronas.cpp](../source/game_sa/Coronas.cpp) | Implementación de halos luminosos alrededor de luces. |
| [renderer/Coronas.h](../../gta3/src/renderer/Coronas.h) | [renderer/Coronas.h](../../gtavc/src/renderer/Coronas.h) | [Coronas.h](../source/game_sa/Coronas.h) | Declaraciones y tipos de halos luminosos alrededor de luces. |
| [vehicles/Cranes.cpp](../../gta3/src/vehicles/Cranes.cpp) | [vehicles/Cranes.cpp](../../gtavc/src/vehicles/Cranes.cpp) | [Cranes.cpp](../source/game_sa/Cranes.cpp) | Implementación de grúas. |
| [vehicles/Cranes.h](../../gta3/src/vehicles/Cranes.h) | [vehicles/Cranes.h](../../gtavc/src/vehicles/Cranes.h) | [Cranes.h](../source/game_sa/Cranes.h) | Declaraciones y tipos de grúas. |
| [renderer/Credits.cpp](../../gta3/src/renderer/Credits.cpp) | [renderer/Credits.cpp](../../gtavc/src/renderer/Credits.cpp) | [Credits.cpp](../source/game_sa/Credits.cpp) | Implementación de presentación de créditos. |
| [renderer/Credits.h](../../gta3/src/renderer/Credits.h) | [renderer/Credits.h](../../gtavc/src/renderer/Credits.h) | [Credits.h](../source/game_sa/Credits.h) | Declaraciones y tipos de presentación de créditos. |
| [core/Crime.h](../../gta3/src/core/Crime.h) | [core/Crime.h](../../gtavc/src/core/Crime.h) | [Crime.h](../source/game_sa/Crime.h) | Declaraciones y tipos de delitos y respuesta policial o nivel de búsqueda. |
| [control/Curves.cpp](../../gta3/src/control/Curves.cpp) | [control/Curves.cpp](../../gtavc/src/control/Curves.cpp) | [Curves.cpp](../source/game_sa/Curves.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [control/Curves.h](../../gta3/src/control/Curves.h) | [control/Curves.h](../../gtavc/src/control/Curves.h) | [Curves.h](../source/game_sa/Curves.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [animation/CutsceneMgr.cpp](../../gta3/src/animation/CutsceneMgr.cpp) | [animation/CutsceneMgr.cpp](../../gtavc/src/animation/CutsceneMgr.cpp) | [CutsceneMgr.cpp](../source/game_sa/CutsceneMgr.cpp) | Implementación de cinemáticas y objetos que participan en ellas. |
| [animation/CutsceneMgr.h](../../gta3/src/animation/CutsceneMgr.h) | [animation/CutsceneMgr.h](../../gtavc/src/animation/CutsceneMgr.h) | [CutsceneMgr.h](../source/game_sa/CutsceneMgr.h) | Declaraciones y tipos de cinemáticas y objetos que participan en ellas. |
| [objects/CutsceneObject.cpp](../../gta3/src/objects/CutsceneObject.cpp) | [objects/CutsceneObject.cpp](../../gtavc/src/objects/CutsceneObject.cpp) | [Entity/Object/CutsceneObject.cpp](../source/game_sa/Entity/Object/CutsceneObject.cpp) | Implementación de cinemáticas y objetos que participan en ellas. |
| [objects/CutsceneObject.h](../../gta3/src/objects/CutsceneObject.h) | [objects/CutsceneObject.h](../../gtavc/src/objects/CutsceneObject.h) | [Entity/Object/CutsceneObject.h](../source/game_sa/Entity/Object/CutsceneObject.h) | Declaraciones y tipos de cinemáticas y objetos que participan en ellas. |
| [vehicles/DamageManager.cpp](../../gta3/src/vehicles/DamageManager.cpp) | [vehicles/DamageManager.cpp](../../gtavc/src/vehicles/DamageManager.cpp) | [DamageManager.cpp](../source/game_sa/DamageManager.cpp) | Implementación de estado de daños de los componentes del vehículo. |
| [vehicles/DamageManager.h](../../gta3/src/vehicles/DamageManager.h) | [vehicles/DamageManager.h](../../gtavc/src/vehicles/DamageManager.h) | [DamageManager.h](../source/game_sa/DamageManager.h) | Declaraciones y tipos de estado de daños de los componentes del vehículo. |
| [control/Darkel.cpp](../../gta3/src/control/Darkel.cpp) | [control/Darkel.cpp](../../gtavc/src/control/Darkel.cpp) | [Darkel.cpp](../source/game_sa/Darkel.cpp) | Implementación de retos de matanza o rampages. |
| [control/Darkel.h](../../gta3/src/control/Darkel.h) | [control/Darkel.h](../../gtavc/src/control/Darkel.h) | [Darkel.h](../source/game_sa/Darkel.h) | Declaraciones y tipos de retos de matanza o rampages. |
| [save/Date.cpp](../../gta3/src/save/Date.cpp) | [save/Date.cpp](../../gtavc/src/save/Date.cpp) | [Date.cpp](../source/game_sa/Date.cpp) | Implementación de hora y calendario del juego. |
| [save/Date.h](../../gta3/src/save/Date.h) | [save/Date.h](../../gtavc/src/save/Date.h) | [Date.h](../source/game_sa/Date.h) | Declaraciones y tipos de hora y calendario del juego. |
| [core/Debug.cpp](../../gta3/src/core/Debug.cpp) | [core/Debug.cpp](../../gtavc/src/core/Debug.cpp) | [Debug.cpp](../source/game_sa/Debug.cpp) | Implementación de inspección, diagnóstico y herramientas de depuración. |
| [core/Debug.h](../../gta3/src/core/Debug.h) | [core/Debug.h](../../gtavc/src/core/Debug.h) | [Debug.h](../source/game_sa/Debug.h) | Declaraciones y tipos de inspección, diagnóstico y herramientas de depuración. |
| [core/Directory.cpp](../../gta3/src/core/Directory.cpp) | [core/Directory.cpp](../../gtavc/src/core/Directory.cpp) | [Directory.cpp](../source/game_sa/Directory.cpp) | Implementación de índice de entradas de archivos de recursos. |
| [core/Directory.h](../../gta3/src/core/Directory.h) | [core/Directory.h](../../gtavc/src/core/Directory.h) | [Directory.h](../source/game_sa/Directory.h) | Declaraciones y tipos de índice de entradas de archivos de recursos. |
| [vehicles/Door.cpp](../../gta3/src/vehicles/Door.cpp) | [vehicles/Door.cpp](../../gtavc/src/vehicles/Door.cpp) | [Door.cpp](../source/game_sa/Door.cpp) | Implementación de puertas de vehículos y su movimiento. |
| [vehicles/Door.h](../../gta3/src/vehicles/Door.h) | [vehicles/Door.h](../../gtavc/src/vehicles/Door.h) | [Door.h](../source/game_sa/Door.h) | Declaraciones y tipos de puertas de vehículos y su movimiento. |
| [renderer/Draw.cpp](../../gta3/src/renderer/Draw.cpp) | [renderer/Draw.cpp](../../gtavc/src/renderer/Draw.cpp) | [Draw.cpp](../source/game_sa/Draw.cpp) | Implementación de preparación y dibujo de la escena. |
| [renderer/Draw.h](../../gta3/src/renderer/Draw.h) | [renderer/Draw.h](../../gtavc/src/renderer/Draw.h) | [Draw.h](../source/game_sa/Draw.h) | Declaraciones y tipos de preparación y dibujo de la escena. |
| [entities/Dummy.cpp](../../gta3/src/entities/Dummy.cpp) | [entities/Dummy.cpp](../../gtavc/src/entities/Dummy.cpp) | [Entity/Dummy/Dummy.cpp](../source/game_sa/Entity/Dummy/Dummy.cpp) | Implementación de representaciones simplificadas de entidades. |
| [entities/Dummy.h](../../gta3/src/entities/Dummy.h) | [entities/Dummy.h](../../gtavc/src/entities/Dummy.h) | [Entity/Dummy/Dummy.h](../source/game_sa/Entity/Dummy/Dummy.h) | Declaraciones y tipos de representaciones simplificadas de entidades. |
| [objects/DummyObject.cpp](../../gta3/src/objects/DummyObject.cpp) | [objects/DummyObject.cpp](../../gtavc/src/objects/DummyObject.cpp) | [Entity/Dummy/DummyObject.cpp](../source/game_sa/Entity/Dummy/DummyObject.cpp) | Implementación de representaciones simplificadas de entidades. |
| [objects/DummyObject.h](../../gta3/src/objects/DummyObject.h) | [objects/DummyObject.h](../../gtavc/src/objects/DummyObject.h) | [Entity/Dummy/DummyObject.h](../source/game_sa/Entity/Dummy/DummyObject.h) | Declaraciones y tipos de representaciones simplificadas de entidades. |
| [peds/DummyPed.h](../../gta3/src/peds/DummyPed.h) | [peds/DummyPed.h](../../gtavc/src/peds/DummyPed.h) | [Entity/Dummy/DummyPed.h](../source/game_sa/Entity/Dummy/DummyPed.h) | Declaraciones y tipos de representaciones simplificadas de entidades. |
| [peds/EmergencyPed.cpp](../../gta3/src/peds/EmergencyPed.cpp) | [peds/EmergencyPed.cpp](../../gtavc/src/peds/EmergencyPed.cpp) | [Entity/Ped/EmergencyPed.cpp](../source/game_sa/Entity/Ped/EmergencyPed.cpp) | Implementación de personajes de servicios de emergencia. |
| [peds/EmergencyPed.h](../../gta3/src/peds/EmergencyPed.h) | [peds/EmergencyPed.h](../../gtavc/src/peds/EmergencyPed.h) | [Entity/Ped/EmergencyPed.h](../source/game_sa/Entity/Ped/EmergencyPed.h) | Declaraciones y tipos de personajes de servicios de emergencia. |
| [entities/Entity.cpp](../../gta3/src/entities/Entity.cpp) | [entities/Entity.cpp](../../gtavc/src/entities/Entity.cpp) | [Entity/Entity.cpp](../source/game_sa/Entity/Entity.cpp) | Implementación de la entidad base del mundo y sus operaciones comunes. |
| [entities/Entity.h](../../gta3/src/entities/Entity.h) | [entities/Entity.h](../../gtavc/src/entities/Entity.h) | [Entity/Entity.h](../source/game_sa/Entity/Entity.h) | Declaraciones y tipos de la entidad base del mundo y sus operaciones comunes. |
| [weapons/Explosion.cpp](../../gta3/src/weapons/Explosion.cpp) | [weapons/Explosion.cpp](../../gtavc/src/weapons/Explosion.cpp) | [Explosion.cpp](../source/game_sa/Explosion.cpp) | Implementación de explosiones, su evolución y efectos. |
| [weapons/Explosion.h](../../gta3/src/weapons/Explosion.h) | [weapons/Explosion.h](../../gtavc/src/weapons/Explosion.h) | [Explosion.h](../source/game_sa/Explosion.h) | Declaraciones y tipos de explosiones, su evolución y efectos. |
| [core/FileLoader.cpp](../../gta3/src/core/FileLoader.cpp) | [core/FileLoader.cpp](../../gtavc/src/core/FileLoader.cpp) | [FileLoader.cpp](../source/game_sa/FileLoader.cpp) | Implementación de carga de definiciones y colocación de elementos del mundo. |
| [core/FileLoader.h](../../gta3/src/core/FileLoader.h) | [core/FileLoader.h](../../gtavc/src/core/FileLoader.h) | [FileLoader.h](../source/game_sa/FileLoader.h) | Declaraciones y tipos de carga de definiciones y colocación de elementos del mundo. |
| [core/FileMgr.cpp](../../gta3/src/core/FileMgr.cpp) | [core/FileMgr.cpp](../../gtavc/src/core/FileMgr.cpp) | [FileMgr.cpp](../source/game_sa/FileMgr.cpp) | Implementación de operaciones de archivos y directorios. |
| [core/FileMgr.h](../../gta3/src/core/FileMgr.h) | [core/FileMgr.h](../../gtavc/src/core/FileMgr.h) | [FileMgr.h](../source/game_sa/FileMgr.h) | Declaraciones y tipos de operaciones de archivos y directorios. |
| [core/Fire.cpp](../../gta3/src/core/Fire.cpp) | [core/Fire.cpp](../../gtavc/src/core/Fire.cpp) | [Fire.cpp](../source/game_sa/Fire.cpp) | Implementación de fuego, focos de incendio y propagación. |
| [core/Fire.h](../../gta3/src/core/Fire.h) | [core/Fire.h](../../gtavc/src/core/Fire.h) | [Fire.h](../source/game_sa/Fire.h) | Declaraciones y tipos de fuego, focos de incendio y propagación. |
| [renderer/Font.cpp](../../gta3/src/renderer/Font.cpp) | [renderer/Font.cpp](../../gtavc/src/renderer/Font.cpp) | [Font.cpp](../source/game_sa/Font.cpp) | Implementación de dibujo y formato de fuentes tipográficas. |
| [renderer/Font.h](../../gta3/src/renderer/Font.h) | [renderer/Font.h](../../gtavc/src/renderer/Font.h) | [Font.h](../source/game_sa/Font.h) | Declaraciones y tipos de dibujo y formato de fuentes tipográficas. |
| [core/Game.cpp](../../gta3/src/core/Game.cpp) | [core/Game.cpp](../../gtavc/src/core/Game.cpp) | [Game.cpp](../source/game_sa/Game.cpp) | Implementación de inicialización, actualización y reglas generales de ejecución del juego. |
| [core/Game.h](../../gta3/src/core/Game.h) | [core/Game.h](../../gtavc/src/core/Game.h) | [Game.h](../source/game_sa/Game.h) | Declaraciones y tipos de inicialización, actualización y reglas generales de ejecución del juego. |
| [control/GameLogic.cpp](../../gta3/src/control/GameLogic.cpp) | [control/GameLogic.cpp](../../gtavc/src/control/GameLogic.cpp) | [GameLogic.cpp](../source/game_sa/GameLogic.cpp) | Implementación de inicialización, actualización y reglas generales de ejecución del juego. |
| [control/GameLogic.h](../../gta3/src/control/GameLogic.h) | [control/GameLogic.h](../../gtavc/src/control/GameLogic.h) | [GameLogic.h](../source/game_sa/GameLogic.h) | Declaraciones y tipos de inicialización, actualización y reglas generales de ejecución del juego. |
| [peds/Gangs.cpp](../../gta3/src/peds/Gangs.cpp) | [peds/Gangs.cpp](../../gtavc/src/peds/Gangs.cpp) | [Gangs.cpp](../source/game_sa/Gangs.cpp) | Implementación de bandas, relaciones y guerras territoriales. |
| [peds/Gangs.h](../../gta3/src/peds/Gangs.h) | [peds/Gangs.h](../../gtavc/src/peds/Gangs.h) | [Gangs.h](../source/game_sa/Gangs.h) | Declaraciones y tipos de bandas, relaciones y guerras territoriales. |
| [control/Garages.cpp](../../gta3/src/control/Garages.cpp) | [control/Garages.cpp](../../gtavc/src/control/Garages.cpp) | [Garages.cpp](../source/game_sa/Garages.cpp) | Implementación de garajes y vehículos almacenados. |
| [control/Garages.h](../../gta3/src/control/Garages.h) | [control/Garages.h](../../gtavc/src/control/Garages.h) | [Garages.h](../source/game_sa/Garages.h) | Declaraciones y tipos de garajes y vehículos almacenados. |
| [core/General.h](../../gta3/src/core/General.h) | [core/General.h](../../gtavc/src/core/General.h) | [General.h](../source/game_sa/General.h) | Declaraciones y tipos de utilidades, definiciones u operaciones compartidas. |
| [save/GenericGameStorage.cpp](../../gta3/src/save/GenericGameStorage.cpp) | [save/GenericGameStorage.cpp](../../gtavc/src/save/GenericGameStorage.cpp) | [GenericGameStorage.cpp](../source/game_sa/GenericGameStorage.cpp) | Implementación de serialización y almacenamiento de partidas. |
| [save/GenericGameStorage.h](../../gta3/src/save/GenericGameStorage.h) | [save/GenericGameStorage.h](../../gtavc/src/save/GenericGameStorage.h) | [GenericGameStorage.h](../source/game_sa/GenericGameStorage.h) | Declaraciones y tipos de serialización y almacenamiento de partidas. |
| [renderer/Glass.cpp](../../gta3/src/renderer/Glass.cpp) | [renderer/Glass.cpp](../../gtavc/src/renderer/Glass.cpp) | [Glass.cpp](../source/game_sa/Glass.cpp) | Implementación de cristales, roturas y fragmentos. |
| [renderer/Glass.h](../../gta3/src/renderer/Glass.h) | [renderer/Glass.h](../../gtavc/src/renderer/Glass.h) | [Glass.h](../source/game_sa/Glass.h) | Declaraciones y tipos de cristales, roturas y fragmentos. |
| [vehicles/Heli.cpp](../../gta3/src/vehicles/Heli.cpp) | [vehicles/Heli.cpp](../../gtavc/src/vehicles/Heli.cpp) | [Entity/Vehicle/Heli.cpp](../source/game_sa/Entity/Vehicle/Heli.cpp) | Implementación de helicópteros. |
| [vehicles/Heli.h](../../gta3/src/vehicles/Heli.h) | [vehicles/Heli.h](../../gtavc/src/vehicles/Heli.h) | [Entity/Vehicle/Heli.h](../source/game_sa/Entity/Vehicle/Heli.h) | Declaraciones y tipos de helicópteros. |
| [renderer/Hud.cpp](../../gta3/src/renderer/Hud.cpp) | [renderer/Hud.cpp](../../gtavc/src/renderer/Hud.cpp) | [Hud.cpp](../source/game_sa/Hud.cpp) | Implementación de información mostrada al jugador durante la partida. |
| [renderer/Hud.h](../../gta3/src/renderer/Hud.h) | [renderer/Hud.h](../../gtavc/src/renderer/Hud.h) | [Hud.h](../source/game_sa/Hud.h) | Declaraciones y tipos de información mostrada al jugador durante la partida. |
| [core/IniFile.cpp](../../gta3/src/core/IniFile.cpp) | [core/IniFile.cpp](../../gtavc/src/core/IniFile.cpp) | [IniFile.cpp](../source/game_sa/IniFile.cpp) | Implementación de lectura o gestión de configuración INI. |
| [core/IniFile.h](../../gta3/src/core/IniFile.h) | [core/IniFile.h](../../gtavc/src/core/IniFile.h) | [IniFile.h](../source/game_sa/IniFile.h) | Declaraciones y tipos de lectura o gestión de configuración INI. |
| [renderer/Lines.cpp](../../gta3/src/renderer/Lines.cpp) | [renderer/Lines.cpp](../../gtavc/src/renderer/Lines.cpp) | [Lines.cpp](../source/game_sa/Lines.cpp) | Implementación de dibujo de líneas en el espacio del juego. |
| [renderer/Lines.h](../../gta3/src/renderer/Lines.h) | [renderer/Lines.h](../../gtavc/src/renderer/Lines.h) | [Lines.h](../source/game_sa/Lines.h) | Declaraciones y tipos de dibujo de líneas en el espacio del juego. |
| [math/maths.h](../../gta3/src/math/maths.h) | [math/maths.h](../../gtavc/src/math/maths.h) | [Maths.h](../source/game_sa/Maths.h) | Declaraciones y tipos de utilidades, definiciones u operaciones compartidas. |
| [math/Matrix.cpp](../../gta3/src/math/Matrix.cpp) | [math/Matrix.cpp](../../gtavc/src/math/Matrix.cpp) | [Core/Matrix.cpp](../source/game_sa/Core/Matrix.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [math/Matrix.h](../../gta3/src/math/Matrix.h) | [math/Matrix.h](../../gtavc/src/math/Matrix.h) | [Core/Matrix.h](../source/game_sa/Core/Matrix.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [renderer/MBlur.cpp](../../gta3/src/renderer/MBlur.cpp) | [renderer/MBlur.cpp](../../gtavc/src/renderer/MBlur.cpp) | [MBlur.cpp](../source/game_sa/MBlur.cpp) | Implementación de desenfoque de movimiento y estelas visuales. |
| [renderer/MBlur.h](../../gta3/src/renderer/MBlur.h) | [renderer/MBlur.h](../../gtavc/src/renderer/MBlur.h) | [MBlur.h](../source/game_sa/MBlur.h) | Declaraciones y tipos de desenfoque de movimiento y estelas visuales. |
| [rw/MemoryHeap.cpp](../../gta3/src/rw/MemoryHeap.cpp) | [rw/MemoryHeap.cpp](../../gtavc/src/rw/MemoryHeap.cpp) | [Memory/MemoryHeap.cpp](../source/game_sa/Memory/MemoryHeap.cpp) | Implementación de gestión de memoria o almacenamiento según la implementación. |
| [rw/MemoryHeap.h](../../gta3/src/rw/MemoryHeap.h) | [rw/MemoryHeap.h](../../gtavc/src/rw/MemoryHeap.h) | [Memory/MemoryHeap.h](../source/game_sa/Memory/MemoryHeap.h) | Declaraciones y tipos de gestión de memoria o almacenamiento según la implementación. |
| [rw/MemoryMgr.cpp](../../gta3/src/rw/MemoryMgr.cpp) | [rw/MemoryMgr.cpp](../../gtavc/src/rw/MemoryMgr.cpp) | [MemoryMgr.cpp](../source/game_sa/MemoryMgr.cpp) | Implementación de gestión de memoria o almacenamiento según la implementación. |
| [rw/MemoryMgr.h](../../gta3/src/rw/MemoryMgr.h) | [rw/MemoryMgr.h](../../gtavc/src/rw/MemoryMgr.h) | [MemoryMgr.h](../source/game_sa/MemoryMgr.h) | Declaraciones y tipos de gestión de memoria o almacenamiento según la implementación. |
| [text/Messages.cpp](../../gta3/src/text/Messages.cpp) | [text/Messages.cpp](../../gtavc/src/text/Messages.cpp) | [Messages.cpp](../source/game_sa/Messages.cpp) | Implementación de mensajes presentados al jugador. |
| [text/Messages.h](../../gta3/src/text/Messages.h) | [text/Messages.h](../../gtavc/src/text/Messages.h) | [Messages.h](../source/game_sa/Messages.h) | Declaraciones y tipos de mensajes presentados al jugador. |
| [modelinfo/ModelIndices.cpp](../../gta3/src/modelinfo/ModelIndices.cpp) | [modelinfo/ModelIndices.cpp](../../gtavc/src/modelinfo/ModelIndices.cpp) | [ModelIndices.cpp](../source/game_sa/ModelIndices.cpp) | Implementación de identificadores de modelos conocidos por el juego. |
| [modelinfo/ModelIndices.h](../../gta3/src/modelinfo/ModelIndices.h) | [modelinfo/ModelIndices.h](../../gtavc/src/modelinfo/ModelIndices.h) | [ModelIndices.h](../source/game_sa/ModelIndices.h) | Declaraciones y tipos de identificadores de modelos conocidos por el juego. |
| [modelinfo/ModelInfo.cpp](../../gta3/src/modelinfo/ModelInfo.cpp) | [modelinfo/ModelInfo.cpp](../../gtavc/src/modelinfo/ModelInfo.cpp) | [Models/ModelInfo.cpp](../source/game_sa/Models/ModelInfo.cpp) | Implementación de los metadatos de modelo `ModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/ModelInfo.h](../../gta3/src/modelinfo/ModelInfo.h) | [modelinfo/ModelInfo.h](../../gtavc/src/modelinfo/ModelInfo.h) | [Models/ModelInfo.h](../source/game_sa/Models/ModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `ModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [rw/NodeName.cpp](../../gta3/src/rw/NodeName.cpp) | [rw/NodeName.cpp](../../gtavc/src/rw/NodeName.cpp) | [Plugins/NodeNamePlugin/NodeName.cpp](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) | Implementación de nombres asociados a nodos de RenderWare. |
| [rw/NodeName.h](../../gta3/src/rw/NodeName.h) | [rw/NodeName.h](../../gtavc/src/rw/NodeName.h) | [Plugins/NodeNamePlugin/NodeName.h](../source/game_sa/Plugins/NodeNamePlugin/NodeName.h) | Declaraciones y tipos de nombres asociados a nodos de RenderWare. |
| [objects/Object.cpp](../../gta3/src/objects/Object.cpp) | [objects/Object.cpp](../../gtavc/src/objects/Object.cpp) | [Entity/Object/Object.cpp](../source/game_sa/Entity/Object/Object.cpp) | Implementación de objetos del escenario y su comportamiento. |
| [objects/Object.h](../../gta3/src/objects/Object.h) | [objects/Object.h](../../gtavc/src/objects/Object.h) | [Entity/Object/Object.h](../source/game_sa/Entity/Object/Object.h) | Declaraciones y tipos de objetos del escenario y su comportamiento. |
| [objects/ObjectData.cpp](../../gta3/src/objects/ObjectData.cpp) | [objects/ObjectData.cpp](../../gtavc/src/objects/ObjectData.cpp) | [ObjectData.cpp](../source/game_sa/ObjectData.cpp) | Implementación de propiedades físicas y de comportamiento de los tipos de objeto. |
| [objects/ObjectData.h](../../gta3/src/objects/ObjectData.h) | [objects/ObjectData.h](../../gtavc/src/objects/ObjectData.h) | [ObjectData.h](../source/game_sa/ObjectData.h) | Declaraciones y tipos de propiedades físicas y de comportamiento de los tipos de objeto. |
| — | [renderer/Occlusion.cpp](../../gtavc/src/renderer/Occlusion.cpp) | [Occlusion.cpp](../source/game_sa/Occlusion.cpp) | Implementación de selección de elementos visibles y ocultación por geometría. |
| — | [renderer/Occlusion.h](../../gtavc/src/renderer/Occlusion.h) | [Occlusion.h](../source/game_sa/Occlusion.h) | Declaraciones y tipos de selección de elementos visibles y ocultación por geometría. |
| [control/OnscreenTimer.cpp](../../gta3/src/control/OnscreenTimer.cpp) | [control/OnscreenTimer.cpp](../../gtavc/src/control/OnscreenTimer.cpp) | [OnscreenTimer.cpp](../source/game_sa/OnscreenTimer.cpp) | Implementación de temporizadores y contadores visibles en pantalla. |
| [control/OnscreenTimer.h](../../gta3/src/control/OnscreenTimer.h) | [control/OnscreenTimer.h](../../gtavc/src/control/OnscreenTimer.h) | [OnscreenTimer.h](../source/game_sa/OnscreenTimer.h) | Declaraciones y tipos de temporizadores y contadores visibles en pantalla. |
| [core/Pad.cpp](../../gta3/src/core/Pad.cpp) | [core/Pad.cpp](../../gtavc/src/core/Pad.cpp) | [Pad.cpp](../source/game_sa/Pad.cpp) | Implementación de lectura y estado de controles del jugador. |
| [core/Pad.h](../../gta3/src/core/Pad.h) | [core/Pad.h](../../gtavc/src/core/Pad.h) | [Pad.h](../source/game_sa/Pad.h) | Declaraciones y tipos de lectura y estado de controles del jugador. |
| [renderer/Particle.h](../../gta3/src/renderer/Particle.h) | [renderer/Particle.h](../../gtavc/src/renderer/Particle.h) | [Fx/Particle.h](../source/game_sa/Fx/Particle.h) | Declaraciones y tipos de partículas y efectos; su organización difiere entre juegos. |
| [control/PathFind.cpp](../../gta3/src/control/PathFind.cpp) | [control/PathFind.cpp](../../gtavc/src/control/PathFind.cpp) | [PathFind.cpp](../source/game_sa/PathFind.cpp) | Implementación de rutas, nodos y búsqueda de caminos. |
| [control/PathFind.h](../../gta3/src/control/PathFind.h) | [control/PathFind.h](../../gtavc/src/control/PathFind.h) | [PathFind.h](../source/game_sa/PathFind.h) | Declaraciones y tipos de rutas, nodos y búsqueda de caminos. |
| [peds/Ped.cpp](../../gta3/src/peds/Ped.cpp) | [peds/Ped.cpp](../../gtavc/src/peds/Ped.cpp) | [Entity/Ped/Ped.cpp](../source/game_sa/Entity/Ped/Ped.cpp) | Implementación del personaje base: estado, movimiento e interacción del peatón. |
| [peds/Ped.h](../../gta3/src/peds/Ped.h) | [peds/Ped.h](../../gtavc/src/peds/Ped.h) | [Entity/Ped/Ped.h](../source/game_sa/Entity/Ped/Ped.h) | Declaraciones y tipos del personaje base: estado, movimiento e interacción del peatón. |
| — | [peds/PedAttractor.cpp](../../gtavc/src/peds/PedAttractor.cpp) | [Attractors/PedAttractor.cpp](../source/game_sa/Attractors/PedAttractor.cpp) | Implementación de `PedAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | [peds/PedAttractor.h](../../gtavc/src/peds/PedAttractor.h) | [Attractors/PedAttractor.h](../source/game_sa/Attractors/PedAttractor.h) | Declaraciones y tipos de `PedAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| [peds/PedIK.cpp](../../gta3/src/peds/PedIK.cpp) | [peds/PedIK.cpp](../../gtavc/src/peds/PedIK.cpp) | [PedIK.cpp](../source/game_sa/PedIK.cpp) | Implementación de cinemática inversa para orientar huesos de personajes. |
| [peds/PedIK.h](../../gta3/src/peds/PedIK.h) | [peds/PedIK.h](../../gtavc/src/peds/PedIK.h) | [PedIK.h](../source/game_sa/PedIK.h) | Declaraciones y tipos de cinemática inversa para orientar huesos de personajes. |
| [modelinfo/PedModelInfo.cpp](../../gta3/src/modelinfo/PedModelInfo.cpp) | [modelinfo/PedModelInfo.cpp](../../gtavc/src/modelinfo/PedModelInfo.cpp) | [Models/PedModelInfo.cpp](../source/game_sa/Models/PedModelInfo.cpp) | Implementación de los metadatos de modelo `PedModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/PedModelInfo.h](../../gta3/src/modelinfo/PedModelInfo.h) | [modelinfo/PedModelInfo.h](../../gtavc/src/modelinfo/PedModelInfo.h) | [Models/PedModelInfo.h](../source/game_sa/Models/PedModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `PedModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [peds/PedPlacement.cpp](../../gta3/src/peds/PedPlacement.cpp) | [peds/PedPlacement.cpp](../../gtavc/src/peds/PedPlacement.cpp) | [PedPlacement.cpp](../source/game_sa/PedPlacement.cpp) | Implementación de colocación de peatones y análisis de su entorno geométrico. |
| [peds/PedPlacement.h](../../gta3/src/peds/PedPlacement.h) | [peds/PedPlacement.h](../../gtavc/src/peds/PedPlacement.h) | [PedPlacement.h](../source/game_sa/PedPlacement.h) | Declaraciones y tipos de colocación de peatones y análisis de su entorno geométrico. |
| [peds/PedType.cpp](../../gta3/src/peds/PedType.cpp) | [peds/PedType.cpp](../../gtavc/src/peds/PedType.cpp) | [PedType.cpp](../source/game_sa/PedType.cpp) | Implementación de tipos, atributos y relaciones de peatones. |
| [peds/PedType.h](../../gta3/src/peds/PedType.h) | [peds/PedType.h](../../gtavc/src/peds/PedType.h) | [PedType.h](../source/game_sa/PedType.h) | Declaraciones y tipos de tipos, atributos y relaciones de peatones. |
| [entities/Physical.cpp](../../gta3/src/entities/Physical.cpp) | [entities/Physical.cpp](../../gtavc/src/entities/Physical.cpp) | [Entity/Physical.cpp](../source/game_sa/Entity/Physical.cpp) | Implementación de propiedades físicas, movimiento y respuesta a colisiones de las entidades. |
| [entities/Physical.h](../../gta3/src/entities/Physical.h) | [entities/Physical.h](../../gtavc/src/entities/Physical.h) | [Entity/Physical.h](../source/game_sa/Entity/Physical.h) | Declaraciones y tipos de propiedades físicas, movimiento y respuesta a colisiones de las entidades. |
| [control/Pickups.cpp](../../gta3/src/control/Pickups.cpp) | [control/Pickups.cpp](../../gtavc/src/control/Pickups.cpp) | [Pickups.cpp](../source/game_sa/Pickups.cpp) | Implementación de objetos recogibles y sus mensajes. |
| [control/Pickups.h](../../gta3/src/control/Pickups.h) | [control/Pickups.h](../../gtavc/src/control/Pickups.h) | [Pickups.h](../source/game_sa/Pickups.h) | Declaraciones y tipos de objetos recogibles y sus mensajes. |
| [core/Placeable.cpp](../../gta3/src/core/Placeable.cpp) | [core/Placeable.cpp](../../gtavc/src/core/Placeable.cpp) | [Entity/Placeable.cpp](../source/game_sa/Entity/Placeable.cpp) | Implementación de posición y orientación de elementos del mundo. |
| [core/Placeable.h](../../gta3/src/core/Placeable.h) | [core/Placeable.h](../../gtavc/src/core/Placeable.h) | [Entity/Placeable.h](../source/game_sa/Entity/Placeable.h) | Declaraciones y tipos de posición y orientación de elementos del mundo. |
| [vehicles/Plane.cpp](../../gta3/src/vehicles/Plane.cpp) | [vehicles/Plane.cpp](../../gtavc/src/vehicles/Plane.cpp) | [Entity/Vehicle/Plane.cpp](../source/game_sa/Entity/Vehicle/Plane.cpp) | Implementación de aviones. |
| [vehicles/Plane.h](../../gta3/src/vehicles/Plane.h) | [vehicles/Plane.h](../../gtavc/src/vehicles/Plane.h) | [Entity/Vehicle/Plane.h](../source/game_sa/Entity/Vehicle/Plane.h) | Declaraciones y tipos de aviones. |
| [core/PlayerInfo.cpp](../../gta3/src/core/PlayerInfo.cpp) | [core/PlayerInfo.cpp](../../gtavc/src/core/PlayerInfo.cpp) | [PlayerInfo.cpp](../source/game_sa/PlayerInfo.cpp) | Implementación del personaje del jugador y sus datos de estado. |
| [core/PlayerInfo.h](../../gta3/src/core/PlayerInfo.h) | [core/PlayerInfo.h](../../gtavc/src/core/PlayerInfo.h) | [PlayerInfo.h](../source/game_sa/PlayerInfo.h) | Declaraciones y tipos del personaje del jugador y sus datos de estado. |
| [peds/PlayerPed.cpp](../../gta3/src/peds/PlayerPed.cpp) | [peds/PlayerPed.cpp](../../gtavc/src/peds/PlayerPed.cpp) | [Entity/Ped/PlayerPed.cpp](../source/game_sa/Entity/Ped/PlayerPed.cpp) | Implementación del personaje del jugador y sus datos de estado. |
| [peds/PlayerPed.h](../../gta3/src/peds/PlayerPed.h) | [peds/PlayerPed.h](../../gtavc/src/peds/PlayerPed.h) | [Entity/Ped/PlayerPed.h](../source/game_sa/Entity/Ped/PlayerPed.h) | Declaraciones y tipos del personaje del jugador y sus datos de estado. |
| [renderer/PlayerSkin.cpp](../../gta3/src/renderer/PlayerSkin.cpp) | [renderer/PlayerSkin.cpp](../../gtavc/src/renderer/PlayerSkin.cpp) | [Entity/Ped/PlayerSkin.cpp](../source/game_sa/Entity/Ped/PlayerSkin.cpp) | Implementación de ropa, apariencia y construcción visual de personajes. |
| [renderer/PlayerSkin.h](../../gta3/src/renderer/PlayerSkin.h) | [renderer/PlayerSkin.h](../../gtavc/src/renderer/PlayerSkin.h) | [Entity/Ped/PlayerSkin.h](../source/game_sa/Entity/Ped/PlayerSkin.h) | Declaraciones y tipos de ropa, apariencia y construcción visual de personajes. |
| [renderer/PointLights.cpp](../../gta3/src/renderer/PointLights.cpp) | [renderer/PointLights.cpp](../../gtavc/src/renderer/PointLights.cpp) | [PointLights.cpp](../source/game_sa/PointLights.cpp) | Implementación de luces puntuales, luces visibles o haces de búsqueda. |
| [renderer/PointLights.h](../../gta3/src/renderer/PointLights.h) | [renderer/PointLights.h](../../gtavc/src/renderer/PointLights.h) | [PointLights.h](../source/game_sa/PointLights.h) | Declaraciones y tipos de luces puntuales, luces visibles o haces de búsqueda. |
| [core/Pools.cpp](../../gta3/src/core/Pools.cpp) | [core/Pools.cpp](../../gtavc/src/core/Pools.cpp) | [Pools.cpp](../source/game_sa/Pools.cpp) | Implementación de reservas reutilizables de objetos del juego. |
| [core/Pools.h](../../gta3/src/core/Pools.h) | [core/Pools.h](../../gtavc/src/core/Pools.h) | [Pools/Pools.h](../source/game_sa/Pools/Pools.h) | Declaraciones y tipos de reservas reutilizables de objetos del juego. |
| [peds/Population.cpp](../../gta3/src/peds/Population.cpp) | [peds/Population.cpp](../../gtavc/src/peds/Population.cpp) | [Population.cpp](../source/game_sa/Population.cpp) | Implementación de creación y distribución de población y tráfico. |
| [peds/Population.h](../../gta3/src/peds/Population.h) | [peds/Population.h](../../gtavc/src/peds/Population.h) | [Population.h](../source/game_sa/Population.h) | Declaraciones y tipos de creación y distribución de población y tráfico. |
| [objects/Projectile.cpp](../../gta3/src/objects/Projectile.cpp) | [objects/Projectile.cpp](../../gtavc/src/objects/Projectile.cpp) | [Entity/Object/Projectile.cpp](../source/game_sa/Entity/Object/Projectile.cpp) | Implementación de proyectiles, balas y datos de disparos. |
| [objects/Projectile.h](../../gta3/src/objects/Projectile.h) | [objects/Projectile.h](../../gtavc/src/objects/Projectile.h) | [Entity/Object/Projectile.h](../source/game_sa/Entity/Object/Projectile.h) | Declaraciones y tipos de proyectiles, balas y datos de disparos. |
| [weapons/ProjectileInfo.cpp](../../gta3/src/weapons/ProjectileInfo.cpp) | [weapons/ProjectileInfo.cpp](../../gtavc/src/weapons/ProjectileInfo.cpp) | [ProjectileInfo.cpp](../source/game_sa/ProjectileInfo.cpp) | Implementación de proyectiles, balas y datos de disparos. |
| [weapons/ProjectileInfo.h](../../gta3/src/weapons/ProjectileInfo.h) | [weapons/ProjectileInfo.h](../../gtavc/src/weapons/ProjectileInfo.h) | [ProjectileInfo.h](../source/game_sa/ProjectileInfo.h) | Declaraciones y tipos de proyectiles, balas y datos de disparos. |
| [math/Quaternion.cpp](../../gta3/src/math/Quaternion.cpp) | [math/Quaternion.cpp](../../gtavc/src/math/Quaternion.cpp) | [Core/Quaternion.cpp](../source/game_sa/Core/Quaternion.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [math/Quaternion.h](../../gta3/src/math/Quaternion.h) | [math/Quaternion.h](../../gtavc/src/math/Quaternion.h) | [Core/Quaternion.h](../source/game_sa/Core/Quaternion.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [core/Radar.cpp](../../gta3/src/core/Radar.cpp) | [core/Radar.cpp](../../gtavc/src/core/Radar.cpp) | [Radar.cpp](../source/game_sa/Radar.cpp) | Implementación de radar, mapa y marcadores de navegación. |
| [core/Radar.h](../../gta3/src/core/Radar.h) | [core/Radar.h](../../gtavc/src/core/Radar.h) | [Radar.h](../source/game_sa/Radar.h) | Declaraciones y tipos de radar, mapa y marcadores de navegación. |
| [core/Range2D.cpp](../../gta3/src/core/Range2D.cpp) | [core/Range2D.cpp](../../gtavc/src/core/Range2D.cpp) | [Range2D.cpp](../source/game_sa/Range2D.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [core/Range2D.h](../../gta3/src/core/Range2D.h) | [core/Range2D.h](../../gtavc/src/core/Range2D.h) | [Range2D.h](../source/game_sa/Range2D.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [core/Range3D.cpp](../../gta3/src/core/Range3D.cpp) | [core/Range3D.cpp](../../gtavc/src/core/Range3D.cpp) | [Range3D.cpp](../source/game_sa/Range3D.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [core/Range3D.h](../../gta3/src/core/Range3D.h) | [core/Range3D.h](../../gtavc/src/core/Range3D.h) | [Range3D.h](../source/game_sa/Range3D.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [math/Rect.cpp](../../gta3/src/math/Rect.cpp) | [math/Rect.cpp](../../gtavc/src/math/Rect.cpp) | [Core/Rect.cpp](../source/game_sa/Core/Rect.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [math/Rect.h](../../gta3/src/math/Rect.h) | [math/Rect.h](../../gtavc/src/math/Rect.h) | [Core/Rect.h](../source/game_sa/Core/Rect.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [core/References.cpp](../../gta3/src/core/References.cpp) | [core/References.cpp](../../gtavc/src/core/References.cpp) | [References.cpp](../source/game_sa/References.cpp) | Implementación de seguimiento de referencias a entidades. |
| [core/References.h](../../gta3/src/core/References.h) | [core/References.h](../../gtavc/src/core/References.h) | [References.h](../source/game_sa/References.h) | Declaraciones y tipos de seguimiento de referencias a entidades. |
| [control/Remote.cpp](../../gta3/src/control/Remote.cpp) | [control/Remote.cpp](../../gtavc/src/control/Remote.cpp) | [Remote.cpp](../source/game_sa/Remote.cpp) | Implementación de vehículos controlados a distancia. |
| [control/Remote.h](../../gta3/src/control/Remote.h) | [control/Remote.h](../../gtavc/src/control/Remote.h) | [Remote.h](../source/game_sa/Remote.h) | Declaraciones y tipos de vehículos controlados a distancia. |
| [renderer/RenderBuffer.cpp](../../gta3/src/renderer/RenderBuffer.cpp) | [renderer/RenderBuffer.cpp](../../gtavc/src/renderer/RenderBuffer.cpp) | [RenderBuffer.cpp](../source/game_sa/RenderBuffer.cpp) | Implementación de almacenamiento temporal de geometría para dibujar. |
| [renderer/Renderer.cpp](../../gta3/src/renderer/Renderer.cpp) | [renderer/Renderer.cpp](../../gtavc/src/renderer/Renderer.cpp) | [Renderer.cpp](../source/game_sa/Renderer.cpp) | Implementación de preparación y dibujo de la escena. |
| [renderer/Renderer.h](../../gta3/src/renderer/Renderer.h) | [renderer/Renderer.h](../../gtavc/src/renderer/Renderer.h) | [Renderer.h](../source/game_sa/Renderer.h) | Declaraciones y tipos de preparación y dibujo de la escena. |
| [control/Replay.cpp](../../gta3/src/control/Replay.cpp) | [control/Replay.cpp](../../gtavc/src/control/Replay.cpp) | [Replay.cpp](../source/game_sa/Replay.cpp) | Implementación de grabación o reproducción de secuencias del juego. |
| [control/Replay.h](../../gta3/src/control/Replay.h) | [control/Replay.h](../../gtavc/src/control/Replay.h) | [Replay.h](../source/game_sa/Replay.h) | Declaraciones y tipos de grabación o reproducción de secuencias del juego. |
| [control/Restart.cpp](../../gta3/src/control/Restart.cpp) | [control/Restart.cpp](../../gtavc/src/control/Restart.cpp) | [Restart.cpp](../source/game_sa/Restart.cpp) | Implementación de reinicio y puntos de reaparición. |
| [control/Restart.h](../../gta3/src/control/Restart.h) | [control/Restart.h](../../gtavc/src/control/Restart.h) | [Restart.h](../source/game_sa/Restart.h) | Declaraciones y tipos de reinicio y puntos de reaparición. |
| [control/RoadBlocks.cpp](../../gta3/src/control/RoadBlocks.cpp) | [control/RoadBlocks.cpp](../../gtavc/src/control/RoadBlocks.cpp) | [RoadBlocks.cpp](../source/game_sa/RoadBlocks.cpp) | Implementación de bloqueos de carretera. |
| [control/RoadBlocks.h](../../gta3/src/control/RoadBlocks.h) | [control/RoadBlocks.h](../../gtavc/src/control/RoadBlocks.h) | [RoadBlocks.h](../source/game_sa/RoadBlocks.h) | Declaraciones y tipos de bloqueos de carretera. |
| — | [core/Ropes.cpp](../../gtavc/src/core/Ropes.cpp) | [Ropes.cpp](../source/game_sa/Ropes.cpp) | Implementación de cuerdas y sus puntos de simulación. |
| — | [core/Ropes.h](../../gtavc/src/core/Ropes.h) | [Ropes.h](../source/game_sa/Ropes.h) | Declaraciones y tipos de cuerdas y sus puntos de simulación. |
| [animation/RpAnimBlend.cpp](../../gta3/src/animation/RpAnimBlend.cpp) | [animation/RpAnimBlend.cpp](../../gtavc/src/animation/RpAnimBlend.cpp) | [Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) | Implementación de mezcla de animaciones sobre jerarquías de RenderWare. |
| [animation/RpAnimBlend.h](../../gta3/src/animation/RpAnimBlend.h) | [animation/RpAnimBlend.h](../../gtavc/src/animation/RpAnimBlend.h) | [Plugins/RpAnimBlendPlugin/RpAnimBlend.h](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h) | Declaraciones y tipos de mezcla de animaciones sobre jerarquías de RenderWare. |
| [fakerw/rpanisot.h](../../gta3/src/fakerw/rpanisot.h) | [fakerw/rpanisot.h](../../gtavc/src/fakerw/rpanisot.h) | [RenderWare/rw/rpanisot.h](../source/game_sa/RenderWare/rw/rpanisot.h) | Declaraciones y tipos de `rpanisot`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rphanim.h](../../gta3/src/fakerw/rphanim.h) | [fakerw/rphanim.h](../../gtavc/src/fakerw/rphanim.h) | [RenderWare/rw/rphanim.h](../source/game_sa/RenderWare/rw/rphanim.h) | Declaraciones y tipos de `rphanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rpmatfx.h](../../gta3/src/fakerw/rpmatfx.h) | [fakerw/rpmatfx.h](../../gtavc/src/fakerw/rpmatfx.h) | [RenderWare/rw/rpmatfx.h](../source/game_sa/RenderWare/rw/rpmatfx.h) | Declaraciones y tipos de `rpmatfx`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rpskin.h](../../gta3/src/fakerw/rpskin.h) | [fakerw/rpskin.h](../../gtavc/src/fakerw/rpskin.h) | [RenderWare/rw/rpskin.h](../source/game_sa/RenderWare/rw/rpskin.h) | Declaraciones y tipos de `rpskin`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rpworld.h](../../gta3/src/fakerw/rpworld.h) | [fakerw/rpworld.h](../../gtavc/src/fakerw/rpworld.h) | [RenderWare/rw/rpworld.h](../source/game_sa/RenderWare/rw/rpworld.h) | Declaraciones y tipos de `rpworld`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rtbmp.h](../../gta3/src/fakerw/rtbmp.h) | [fakerw/rtbmp.h](../../gtavc/src/fakerw/rtbmp.h) | [RenderWare/rw/rtbmp.h](../source/game_sa/RenderWare/rw/rtbmp.h) | Declaraciones y tipos de `rtbmp`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rtcharse.h](../../gta3/src/fakerw/rtcharse.h) | [fakerw/rtcharse.h](../../gtavc/src/fakerw/rtcharse.h) | [RenderWare/rw/rtcharse.h](../source/game_sa/RenderWare/rw/rtcharse.h) | Declaraciones y tipos de `rtcharse`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rtpng.h](../../gta3/src/fakerw/rtpng.h) | [fakerw/rtpng.h](../../gtavc/src/fakerw/rtpng.h) | [RenderWare/rw/rtpng.h](../source/game_sa/RenderWare/rw/rtpng.h) | Declaraciones y tipos de `rtpng`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [fakerw/rtquat.h](../../gta3/src/fakerw/rtquat.h) | [fakerw/rtquat.h](../../gtavc/src/fakerw/rtquat.h) | [RenderWare/rw/rtquat.h](../source/game_sa/RenderWare/rw/rtquat.h) | Declaraciones y tipos de `rtquat`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [renderer/Rubbish.cpp](../../gta3/src/renderer/Rubbish.cpp) | [renderer/Rubbish.cpp](../../gtavc/src/renderer/Rubbish.cpp) | [Rubbish.cpp](../source/game_sa/Rubbish.cpp) | Implementación de basura ambiental del escenario. |
| [renderer/Rubbish.h](../../gta3/src/renderer/Rubbish.h) | [renderer/Rubbish.h](../../gtavc/src/renderer/Rubbish.h) | [Rubbish.h](../source/game_sa/Rubbish.h) | Declaraciones y tipos de basura ambiental del escenario. |
| [fakerw/rwcore.h](../../gta3/src/fakerw/rwcore.h) | [fakerw/rwcore.h](../../gtavc/src/fakerw/rwcore.h) | [RenderWare/rw/rwcore.h](../source/game_sa/RenderWare/rw/rwcore.h) | Declaraciones y tipos de `rwcore`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [rw/RwHelper.cpp](../../gta3/src/rw/RwHelper.cpp) | [rw/RwHelper.cpp](../../gtavc/src/rw/RwHelper.cpp) | [RwHelper.cpp](../source/game_sa/RwHelper.cpp) | Implementación de utilidades de integración con RenderWare. |
| [rw/RwHelper.h](../../gta3/src/rw/RwHelper.h) | [rw/RwHelper.h](../../gtavc/src/rw/RwHelper.h) | [RwHelper.h](../source/game_sa/RwHelper.h) | Declaraciones y tipos de utilidades de integración con RenderWare. |
| [fakerw/rwplcore.h](../../gta3/src/fakerw/rwplcore.h) | [fakerw/rwplcore.h](../../gtavc/src/fakerw/rwplcore.h) | [RenderWare/rw/rwplcore.h](../source/game_sa/RenderWare/rw/rwplcore.h) | Declaraciones y tipos de `rwplcore`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | [control/SetPieces.cpp](../../gtavc/src/control/SetPieces.cpp) | [SetPieces.cpp](../source/game_sa/SetPieces.cpp) | Implementación de situaciones o escenas predefinidas activadas en el mundo. |
| — | [control/SetPieces.h](../../gtavc/src/control/SetPieces.h) | [SetPieces.h](../source/game_sa/SetPieces.h) | Declaraciones y tipos de situaciones o escenas predefinidas activadas en el mundo. |
| — | [renderer/ShadowCamera.cpp](../../gtavc/src/renderer/ShadowCamera.cpp) | [ShadowCamera.cpp](../source/game_sa/ShadowCamera.cpp) | Implementación de generación y dibujo de sombras. |
| — | [renderer/ShadowCamera.h](../../gtavc/src/renderer/ShadowCamera.h) | [ShadowCamera.h](../source/game_sa/ShadowCamera.h) | Declaraciones y tipos de generación y dibujo de sombras. |
| [renderer/Shadows.cpp](../../gta3/src/renderer/Shadows.cpp) | [renderer/Shadows.cpp](../../gtavc/src/renderer/Shadows.cpp) | [Shadows.cpp](../source/game_sa/Shadows.cpp) | Implementación de generación y dibujo de sombras. |
| [renderer/Shadows.h](../../gta3/src/renderer/Shadows.h) | [renderer/Shadows.h](../../gtavc/src/renderer/Shadows.h) | [Shadows.h](../source/game_sa/Shadows.h) | Declaraciones y tipos de generación y dibujo de sombras. |
| [weapons/ShotInfo.cpp](../../gta3/src/weapons/ShotInfo.cpp) | [weapons/ShotInfo.cpp](../../gtavc/src/weapons/ShotInfo.cpp) | [ShotInfo.cpp](../source/game_sa/ShotInfo.cpp) | Implementación de proyectiles, balas y datos de disparos. |
| [weapons/ShotInfo.h](../../gta3/src/weapons/ShotInfo.h) | [weapons/ShotInfo.h](../../gtavc/src/weapons/ShotInfo.h) | [ShotInfo.h](../source/game_sa/ShotInfo.h) | Declaraciones y tipos de proyectiles, balas y datos de disparos. |
| [skel/skeleton.h](../../gta3/src/skel/skeleton.h) | [skel/skeleton.h](../../gtavc/src/skel/skeleton.h) | [RenderWare/rw/skeleton.h](../source/game_sa/RenderWare/rw/skeleton.h) | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [renderer/Skidmarks.cpp](../../gta3/src/renderer/Skidmarks.cpp) | [renderer/Skidmarks.cpp](../../gtavc/src/renderer/Skidmarks.cpp) | [Skidmarks.cpp](../source/game_sa/Skidmarks.cpp) | Implementación de marcas de neumáticos en el suelo. |
| [renderer/Skidmarks.h](../../gta3/src/renderer/Skidmarks.h) | [renderer/Skidmarks.h](../../gtavc/src/renderer/Skidmarks.h) | [Skidmarks.h](../source/game_sa/Skidmarks.h) | Declaraciones y tipos de marcas de neumáticos en el suelo. |
| [renderer/SpecialFX.cpp](../../gta3/src/renderer/SpecialFX.cpp) | [renderer/SpecialFX.cpp](../../gtavc/src/renderer/SpecialFX.cpp) | [SpecialFX.cpp](../source/game_sa/SpecialFX.cpp) | Implementación de efectos visuales especiales. |
| [renderer/SpecialFX.h](../../gta3/src/renderer/SpecialFX.h) | [renderer/SpecialFX.h](../../gtavc/src/renderer/SpecialFX.h) | [SpecialFX.h](../source/game_sa/SpecialFX.h) | Declaraciones y tipos de efectos visuales especiales. |
| [renderer/Sprite.cpp](../../gta3/src/renderer/Sprite.cpp) | [renderer/Sprite.cpp](../../gtavc/src/renderer/Sprite.cpp) | [Sprite.cpp](../source/game_sa/Sprite.cpp) | Implementación de dibujo de imágenes y elementos gráficos planos. |
| [renderer/Sprite.h](../../gta3/src/renderer/Sprite.h) | [renderer/Sprite.h](../../gtavc/src/renderer/Sprite.h) | [Sprite.h](../source/game_sa/Sprite.h) | Declaraciones y tipos de dibujo de imágenes y elementos gráficos planos. |
| [renderer/Sprite2d.cpp](../../gta3/src/renderer/Sprite2d.cpp) | [renderer/Sprite2d.cpp](../../gtavc/src/renderer/Sprite2d.cpp) | [Sprite2d.cpp](../source/game_sa/Sprite2d.cpp) | Implementación de dibujo de imágenes y elementos gráficos planos. |
| [renderer/Sprite2d.h](../../gta3/src/renderer/Sprite2d.h) | [renderer/Sprite2d.h](../../gtavc/src/renderer/Sprite2d.h) | [Sprite2d.h](../source/game_sa/Sprite2d.h) | Declaraciones y tipos de dibujo de imágenes y elementos gráficos planos. |
| [core/Stats.cpp](../../gta3/src/core/Stats.cpp) | [core/Stats.cpp](../../gtavc/src/core/Stats.cpp) | [Stats.cpp](../source/game_sa/Stats.cpp) | Implementación de estadísticas y progreso del jugador. |
| [core/Stats.h](../../gta3/src/core/Stats.h) | [core/Stats.h](../../gtavc/src/core/Stats.h) | [Stats.h](../source/game_sa/Stats.h) | Declaraciones y tipos de estadísticas y progreso del jugador. |
| [core/Streaming.cpp](../../gta3/src/core/Streaming.cpp) | [core/Streaming.cpp](../../gtavc/src/core/Streaming.cpp) | [Streaming.cpp](../source/game_sa/Streaming.cpp) | Implementación de carga y descarga de recursos del mundo según demanda. |
| [core/Streaming.h](../../gta3/src/core/Streaming.h) | [core/Streaming.h](../../gtavc/src/core/Streaming.h) | [Streaming.h](../source/game_sa/Streaming.h) | Declaraciones y tipos de carga y descarga de recursos del mundo según demanda. |
| [collision/TempColModels.cpp](../../gta3/src/collision/TempColModels.cpp) | [collision/TempColModels.cpp](../../gtavc/src/collision/TempColModels.cpp) | [Collision/TempColModels.cpp](../source/game_sa/Collision/TempColModels.cpp) | Implementación del componente geométrico `TempColModels` usado en detección y datos de colisiones. |
| [collision/TempColModels.h](../../gta3/src/collision/TempColModels.h) | [collision/TempColModels.h](../../gtavc/src/collision/TempColModels.h) | [Collision/TempColModels.h](../source/game_sa/Collision/TempColModels.h) | Declaraciones y tipos del componente geométrico `TempColModels` usado en detección y datos de colisiones. |
| [text/Text.cpp](../../gta3/src/text/Text.cpp) | [text/Text.cpp](../../gtavc/src/text/Text.cpp) | [Text/Text.cpp](../source/game_sa/Text/Text.cpp) | Implementación de textos localizados y su representación GXT. |
| [text/Text.h](../../gta3/src/text/Text.h) | [text/Text.h](../../gtavc/src/text/Text.h) | [Text/Text.h](../source/game_sa/Text/Text.h) | Declaraciones y tipos de textos localizados y su representación GXT. |
| [renderer/Timecycle.cpp](../../gta3/src/renderer/Timecycle.cpp) | [renderer/Timecycle.cpp](../../gtavc/src/renderer/Timecycle.cpp) | [TimeCycle.cpp](../source/game_sa/TimeCycle.cpp) | Implementación de variación de iluminación y ambiente según hora y clima. |
| [renderer/Timecycle.h](../../gta3/src/renderer/Timecycle.h) | [renderer/Timecycle.h](../../gtavc/src/renderer/Timecycle.h) | [TimeCycle.h](../source/game_sa/TimeCycle.h) | Declaraciones y tipos de variación de iluminación y ambiente según hora y clima. |
| [modelinfo/TimeModelInfo.cpp](../../gta3/src/modelinfo/TimeModelInfo.cpp) | [modelinfo/TimeModelInfo.cpp](../../gtavc/src/modelinfo/TimeModelInfo.cpp) | [Models/TimeModelInfo.cpp](../source/game_sa/Models/TimeModelInfo.cpp) | Implementación de los metadatos de modelo `TimeModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/TimeModelInfo.h](../../gta3/src/modelinfo/TimeModelInfo.h) | [modelinfo/TimeModelInfo.h](../../gtavc/src/modelinfo/TimeModelInfo.h) | [Models/TimeModelInfo.h](../source/game_sa/Models/TimeModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `TimeModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [core/Timer.cpp](../../gta3/src/core/Timer.cpp) | [core/Timer.cpp](../../gtavc/src/core/Timer.cpp) | [Timer.cpp](../source/game_sa/Timer.cpp) | Implementación de tiempo transcurrido y paso de simulación. |
| [core/Timer.h](../../gta3/src/core/Timer.h) | [core/Timer.h](../../gtavc/src/core/Timer.h) | [Timer.h](../source/game_sa/Timer.h) | Declaraciones y tipos de tiempo transcurrido y paso de simulación. |
| [control/TrafficLights.cpp](../../gta3/src/control/TrafficLights.cpp) | [control/TrafficLights.cpp](../../gtavc/src/control/TrafficLights.cpp) | [TrafficLights.cpp](../source/game_sa/TrafficLights.cpp) | Implementación de semáforos y control de sus estados. |
| [control/TrafficLights.h](../../gta3/src/control/TrafficLights.h) | [control/TrafficLights.h](../../gtavc/src/control/TrafficLights.h) | [TrafficLights.h](../source/game_sa/TrafficLights.h) | Declaraciones y tipos de semáforos y control de sus estados. |
| [vehicles/Train.cpp](../../gta3/src/vehicles/Train.cpp) | [vehicles/Train.cpp](../../gtavc/src/vehicles/Train.cpp) | [Entity/Vehicle/Train.cpp](../source/game_sa/Entity/Vehicle/Train.cpp) | Implementación de trenes y sus recorridos. |
| [vehicles/Train.h](../../gta3/src/vehicles/Train.h) | [vehicles/Train.h](../../gtavc/src/vehicles/Train.h) | [Entity/Vehicle/Train.h](../source/game_sa/Entity/Vehicle/Train.h) | Declaraciones y tipos de trenes y sus recorridos. |
| [rw/TxdStore.cpp](../../gta3/src/rw/TxdStore.cpp) | [rw/TxdStore.cpp](../../gtavc/src/rw/TxdStore.cpp) | [TxdStore.cpp](../source/game_sa/TxdStore.cpp) | Implementación de diccionarios de texturas y su gestión. |
| [rw/TxdStore.h](../../gta3/src/rw/TxdStore.h) | [rw/TxdStore.h](../../gtavc/src/rw/TxdStore.h) | [TxdStore.h](../source/game_sa/TxdStore.h) | Declaraciones y tipos de diccionarios de texturas y su gestión. |
| [math/Vector.cpp](../../gta3/src/math/Vector.cpp) | [math/Vector.cpp](../../gtavc/src/math/Vector.cpp) | [Core/Vector.cpp](../source/game_sa/Core/Vector.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| [math/Vector.h](../../gta3/src/math/Vector.h) | [math/Vector.h](../../gtavc/src/math/Vector.h) | [Core/Vector.h](../source/game_sa/Core/Vector.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [math/Vector2D.h](../../gta3/src/math/Vector2D.h) | [math/Vector2D.h](../../gtavc/src/math/Vector2D.h) | [Core/Vector2D.h](../source/game_sa/Core/Vector2D.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| [vehicles/Vehicle.cpp](../../gta3/src/vehicles/Vehicle.cpp) | [vehicles/Vehicle.cpp](../../gtavc/src/vehicles/Vehicle.cpp) | [Entity/Vehicle/Vehicle.cpp](../source/game_sa/Entity/Vehicle/Vehicle.cpp) | Implementación del vehículo base y sus operaciones comunes. |
| [vehicles/Vehicle.h](../../gta3/src/vehicles/Vehicle.h) | [vehicles/Vehicle.h](../../gtavc/src/vehicles/Vehicle.h) | [Entity/Vehicle/Vehicle.h](../source/game_sa/Entity/Vehicle/Vehicle.h) | Declaraciones y tipos del vehículo base y sus operaciones comunes. |
| [modelinfo/VehicleModelInfo.cpp](../../gta3/src/modelinfo/VehicleModelInfo.cpp) | [modelinfo/VehicleModelInfo.cpp](../../gtavc/src/modelinfo/VehicleModelInfo.cpp) | [Models/VehicleModelInfo.cpp](../source/game_sa/Models/VehicleModelInfo.cpp) | Implementación de los metadatos de modelo `VehicleModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/VehicleModelInfo.h](../../gta3/src/modelinfo/VehicleModelInfo.h) | [modelinfo/VehicleModelInfo.h](../../gtavc/src/modelinfo/VehicleModelInfo.h) | [Models/VehicleModelInfo.h](../source/game_sa/Models/VehicleModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `VehicleModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [rw/VisibilityPlugins.cpp](../../gta3/src/rw/VisibilityPlugins.cpp) | [rw/VisibilityPlugins.cpp](../../gtavc/src/rw/VisibilityPlugins.cpp) | [VisibilityPlugins.cpp](../source/game_sa/VisibilityPlugins.cpp) | Implementación de selección de elementos visibles y ocultación por geometría. |
| [rw/VisibilityPlugins.h](../../gta3/src/rw/VisibilityPlugins.h) | [rw/VisibilityPlugins.h](../../gtavc/src/rw/VisibilityPlugins.h) | [VisibilityPlugins.h](../source/game_sa/VisibilityPlugins.h) | Declaraciones y tipos de selección de elementos visibles y ocultación por geometría. |
| [core/Wanted.cpp](../../gta3/src/core/Wanted.cpp) | [core/Wanted.cpp](../../gtavc/src/core/Wanted.cpp) | [Wanted.cpp](../source/game_sa/Wanted.cpp) | Implementación de delitos y respuesta policial o nivel de búsqueda. |
| [core/Wanted.h](../../gta3/src/core/Wanted.h) | [core/Wanted.h](../../gtavc/src/core/Wanted.h) | [Wanted.h](../source/game_sa/Wanted.h) | Declaraciones y tipos de delitos y respuesta policial o nivel de búsqueda. |
| [renderer/WaterCannon.cpp](../../gta3/src/renderer/WaterCannon.cpp) | [renderer/WaterCannon.cpp](../../gtavc/src/renderer/WaterCannon.cpp) | [WaterCannon.cpp](../source/game_sa/WaterCannon.cpp) | Implementación de chorros de cañones de agua. |
| [renderer/WaterCannon.h](../../gta3/src/renderer/WaterCannon.h) | [renderer/WaterCannon.h](../../gtavc/src/renderer/WaterCannon.h) | [WaterCannon.h](../source/game_sa/WaterCannon.h) | Declaraciones y tipos de chorros de cañones de agua. |
| [renderer/WaterLevel.cpp](../../gta3/src/renderer/WaterLevel.cpp) | [renderer/WaterLevel.cpp](../../gtavc/src/renderer/WaterLevel.cpp) | [WaterLevel.cpp](../source/game_sa/WaterLevel.cpp) | Implementación de superficie, altura y representación del agua. |
| [renderer/WaterLevel.h](../../gta3/src/renderer/WaterLevel.h) | [renderer/WaterLevel.h](../../gtavc/src/renderer/WaterLevel.h) | [WaterLevel.h](../source/game_sa/WaterLevel.h) | Declaraciones y tipos de superficie, altura y representación del agua. |
| [weapons/Weapon.cpp](../../gta3/src/weapons/Weapon.cpp) | [weapons/Weapon.cpp](../../gtavc/src/weapons/Weapon.cpp) | [Weapon.cpp](../source/game_sa/Weapon.cpp) | Implementación de armas, sus propiedades y tipos. |
| [weapons/Weapon.h](../../gta3/src/weapons/Weapon.h) | [weapons/Weapon.h](../../gtavc/src/weapons/Weapon.h) | [Weapon.h](../source/game_sa/Weapon.h) | Declaraciones y tipos de armas, sus propiedades y tipos. |
| [weapons/WeaponEffects.cpp](../../gta3/src/weapons/WeaponEffects.cpp) | [weapons/WeaponEffects.cpp](../../gtavc/src/weapons/WeaponEffects.cpp) | [WeaponEffects.cpp](../source/game_sa/WeaponEffects.cpp) | Implementación de efectos visuales asociados a armas y apuntado. |
| [weapons/WeaponEffects.h](../../gta3/src/weapons/WeaponEffects.h) | [weapons/WeaponEffects.h](../../gtavc/src/weapons/WeaponEffects.h) | [WeaponEffects.h](../source/game_sa/WeaponEffects.h) | Declaraciones y tipos de efectos visuales asociados a armas y apuntado. |
| [weapons/WeaponInfo.cpp](../../gta3/src/weapons/WeaponInfo.cpp) | [weapons/WeaponInfo.cpp](../../gtavc/src/weapons/WeaponInfo.cpp) | [WeaponInfo.cpp](../source/game_sa/WeaponInfo.cpp) | Implementación de armas, sus propiedades y tipos. |
| [weapons/WeaponInfo.h](../../gta3/src/weapons/WeaponInfo.h) | [weapons/WeaponInfo.h](../../gtavc/src/weapons/WeaponInfo.h) | [WeaponInfo.h](../source/game_sa/WeaponInfo.h) | Declaraciones y tipos de armas, sus propiedades y tipos. |
| — | [modelinfo/WeaponModelInfo.cpp](../../gtavc/src/modelinfo/WeaponModelInfo.cpp) | [Models/WeaponModelInfo.cpp](../source/game_sa/Models/WeaponModelInfo.cpp) | Implementación de los metadatos de modelo `WeaponModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | [modelinfo/WeaponModelInfo.h](../../gtavc/src/modelinfo/WeaponModelInfo.h) | [Models/WeaponModelInfo.h](../source/game_sa/Models/WeaponModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `WeaponModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [renderer/Weather.cpp](../../gta3/src/renderer/Weather.cpp) | [renderer/Weather.cpp](../../gtavc/src/renderer/Weather.cpp) | [Weather.cpp](../source/game_sa/Weather.cpp) | Implementación de clima y variaciones del viento. |
| [renderer/Weather.h](../../gta3/src/renderer/Weather.h) | [renderer/Weather.h](../../gtavc/src/renderer/Weather.h) | [Weather.h](../source/game_sa/Weather.h) | Declaraciones y tipos de clima y variaciones del viento. |
| — | [renderer/WindModifiers.cpp](../../gtavc/src/renderer/WindModifiers.cpp) | [WindModifiers.cpp](../source/game_sa/WindModifiers.cpp) | Implementación de clima y variaciones del viento. |
| — | [renderer/WindModifiers.h](../../gtavc/src/renderer/WindModifiers.h) | [WindModifiers.h](../source/game_sa/WindModifiers.h) | Declaraciones y tipos de clima y variaciones del viento. |
| [core/World.cpp](../../gta3/src/core/World.cpp) | [core/World.cpp](../../gtavc/src/core/World.cpp) | [World.cpp](../source/game_sa/World.cpp) | Implementación de organización y consulta de entidades de la escena. |
| [core/World.h](../../gta3/src/core/World.h) | [core/World.h](../../gtavc/src/core/World.h) | [World.h](../source/game_sa/World.h) | Declaraciones y tipos de organización y consulta de entidades de la escena. |

<a id="otros"></a>
## Archivos de re3/reVC sin el mismo nombre en reSA

Algunos tienen una correspondencia con otro nombre en la sección funcional anterior.

<details>
<summary>Mostrar 280 nombres de archivo</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| [audio/oal/aldlist.cpp](../../gta3/src/audio/oal/aldlist.cpp) | [audio/oal/aldlist.cpp](../../gtavc/src/audio/oal/aldlist.cpp) | — | Implementación de `aldlist`: reproducción, configuración o soporte del sistema de audio. |
| [audio/oal/aldlist.h](../../gta3/src/audio/oal/aldlist.h) | [audio/oal/aldlist.h](../../gtavc/src/audio/oal/aldlist.h) | — | Declaraciones y tipos de `aldlist`: reproducción, configuración o soporte del sistema de audio. |
| [animation/AnimationId.h](../../gta3/src/animation/AnimationId.h) | [animation/AnimationId.h](../../gtavc/src/animation/AnimationId.h) | — | Declaraciones y tipos de identificadores de animaciones. |
| [animation/AnimBlendList.h](../../gta3/src/animation/AnimBlendList.h) | [animation/AnimBlendList.h](../../gtavc/src/animation/AnimBlendList.h) | — | Declaraciones y tipos del componente `AnimBlendList` de organización, datos o mezcla de animaciones. |
| [core/AnimViewer.cpp](../../gta3/src/core/AnimViewer.cpp) | [core/AnimViewer.cpp](../../gtavc/src/core/AnimViewer.cpp) | — | Implementación de visualización de animaciones para inspección. |
| [core/AnimViewer.h](../../gta3/src/core/AnimViewer.h) | [core/AnimViewer.h](../../gtavc/src/core/AnimViewer.h) | — | Declaraciones y tipos de visualización de animaciones para inspección. |
| [renderer/Antennas.cpp](../../gta3/src/renderer/Antennas.cpp) | [renderer/Antennas.cpp](../../gtavc/src/renderer/Antennas.cpp) | — | Implementación de antenas y su comportamiento visual. |
| [renderer/Antennas.h](../../gta3/src/renderer/Antennas.h) | [renderer/Antennas.h](../../gtavc/src/renderer/Antennas.h) | — | Declaraciones y tipos de antenas y su comportamiento visual. |
| [extras/arrow.inc](../../gta3/src/extras/arrow.inc) | [extras/arrow.inc](../../gtavc/src/extras/arrow.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `arrow`. |
| [audio/audio_enums.h](../../gta3/src/audio/audio_enums.h) | [audio/audio_enums.h](../../gtavc/src/audio/audio_enums.h) | — | Declaraciones y tipos de identificadores y datos de sonidos. |
| [audio/AudioCollision.cpp](../../gta3/src/audio/AudioCollision.cpp) | [audio/AudioCollision.cpp](../../gtavc/src/audio/AudioCollision.cpp) | — | Implementación de sonidos generados por colisiones. |
| [audio/AudioCollision.h](../../gta3/src/audio/AudioCollision.h) | [audio/AudioCollision.h](../../gtavc/src/audio/AudioCollision.h) | — | Declaraciones y tipos de sonidos generados por colisiones. |
| [audio/AudioLogic.cpp](../../gta3/src/audio/AudioLogic.cpp) | [audio/AudioLogic.cpp](../../gtavc/src/audio/AudioLogic.cpp) | — | Implementación de gestión general y lógica de reproducción de audio. |
| [audio/AudioManager.cpp](../../gta3/src/audio/AudioManager.cpp) | [audio/AudioManager.cpp](../../gtavc/src/audio/AudioManager.cpp) | — | Implementación de gestión general y lógica de reproducción de audio. |
| [audio/AudioManager.h](../../gta3/src/audio/AudioManager.h) | [audio/AudioManager.h](../../gtavc/src/audio/AudioManager.h) | — | Declaraciones y tipos de gestión general y lógica de reproducción de audio. |
| [audio/AudioSamples.h](../../gta3/src/audio/AudioSamples.h) | [audio/AudioSamples.h](../../gtavc/src/audio/AudioSamples.h) | — | Declaraciones y tipos de identificadores y datos de sonidos. |
| [audio/AudioScriptObject.cpp](../../gta3/src/audio/AudioScriptObject.cpp) | [audio/AudioScriptObject.cpp](../../gtavc/src/audio/AudioScriptObject.cpp) | — | Implementación de fuentes o eventos de audio controlados desde scripts. |
| [audio/AudioScriptObject.h](../../gta3/src/audio/AudioScriptObject.h) | [audio/AudioScriptObject.h](../../gtavc/src/audio/AudioScriptObject.h) | — | Declaraciones y tipos de fuentes o eventos de audio controlados desde scripts. |
| [animation/Bones.cpp](../../gta3/src/animation/Bones.cpp) | [animation/Bones.cpp](../../gtavc/src/animation/Bones.cpp) | — | Implementación de huesos y actualización de la jerarquía animada. |
| [animation/Bones.h](../../gta3/src/animation/Bones.h) | [animation/Bones.h](../../gtavc/src/animation/Bones.h) | — | Declaraciones y tipos de huesos y actualización de la jerarquía animada. |
| [vehicles/CarGen.cpp](../../gta3/src/vehicles/CarGen.cpp) | [vehicles/CarGen.cpp](../../gtavc/src/vehicles/CarGen.cpp) | — | Implementación de generación de vehículos estacionados en puntos definidos. |
| [vehicles/CarGen.h](../../gta3/src/vehicles/CarGen.h) | [vehicles/CarGen.h](../../gtavc/src/vehicles/CarGen.h) | — | Declaraciones y tipos de generación de vehículos estacionados en puntos definidos. |
| [core/CdStream.cpp](../../gta3/src/core/CdStream.cpp) | [core/CdStream.cpp](../../gtavc/src/core/CdStream.cpp) | — | Implementación de lectura de datos usada por el sistema de streaming. |
| [core/CdStream.h](../../gta3/src/core/CdStream.h) | [core/CdStream.h](../../gtavc/src/core/CdStream.h) | — | Declaraciones y tipos de lectura de datos usada por el sistema de streaming. |
| [core/CdStreamPosix.cpp](../../gta3/src/core/CdStreamPosix.cpp) | [core/CdStreamPosix.cpp](../../gtavc/src/core/CdStreamPosix.cpp) | — | Implementación de lectura de datos usada por el sistema de streaming. |
| [audio/oal/channel.cpp](../../gta3/src/audio/oal/channel.cpp) | [audio/oal/channel.cpp](../../gtavc/src/audio/oal/channel.cpp) | — | Implementación de `channel`: reproducción, configuración o soporte del sistema de audio. |
| [audio/oal/channel.h](../../gta3/src/audio/oal/channel.h) | [audio/oal/channel.h](../../gtavc/src/audio/oal/channel.h) | — | Declaraciones y tipos de `channel`: reproducción, configuración o soporte del sistema de audio. |
| [rw/ClumpRead.cpp](../../gta3/src/rw/ClumpRead.cpp) | [rw/ClumpRead.cpp](../../gtavc/src/rw/ClumpRead.cpp) | — | Implementación de `ClumpRead`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [CMakeLists.txt](../../gta3/src/CMakeLists.txt) | [CMakeLists.txt](../../gtavc/src/CMakeLists.txt) | — | Reglas de CMake para reunir fuentes y configurar la compilación. |
| [extras/shaders/colourfilterIII.frag](../../gta3/src/extras/shaders/colourfilterIII.frag) | — | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `colourfilterIII`. |
| [extras/shaders/obj/colourfilterIII_frag.inc](../../gta3/src/extras/shaders/obj/colourfilterIII_frag.inc) | — | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `colourfilterIII_frag`. |
| [extras/shaders/obj/colourfilterIII_PS.cso](../../gta3/src/extras/shaders/obj/colourfilterIII_PS.cso) | — | — | Shader compilado utilizado por el backend gráfico. Recurso: `colourfilterIII_PS`. |
| [extras/shaders/colourfilterIII_PS.hlsl](../../gta3/src/extras/shaders/colourfilterIII_PS.hlsl) | — | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `colourfilterIII_PS`. |
| [extras/shaders/obj/colourfilterIII_PS.inc](../../gta3/src/extras/shaders/obj/colourfilterIII_PS.inc) | — | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `colourfilterIII_PS`. |
| — | [extras/shaders/colourfilterVC.frag](../../gtavc/src/extras/shaders/colourfilterVC.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `colourfilterVC`. |
| — | [extras/shaders/obj/colourfilterVC_frag.inc](../../gtavc/src/extras/shaders/obj/colourfilterVC_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `colourfilterVC_frag`. |
| — | [extras/shaders/obj/colourfilterVC_PS.cso](../../gtavc/src/extras/shaders/obj/colourfilterVC_PS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `colourfilterVC_PS`. |
| — | [extras/shaders/colourfilterVC_PS.hlsl](../../gtavc/src/extras/shaders/colourfilterVC_PS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `colourfilterVC_PS`. |
| — | [extras/shaders/obj/colourfilterVC_PS.inc](../../gtavc/src/extras/shaders/obj/colourfilterVC_PS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `colourfilterVC_PS`. |
| [core/config.h](../../gta3/src/core/config.h) | [core/config.h](../../gtavc/src/core/config.h) | — | Declaraciones y tipos de opciones de compilación o configuración del proyecto. |
| [renderer/Console.cpp](../../gta3/src/renderer/Console.cpp) | [renderer/Console.cpp](../../gtavc/src/renderer/Console.cpp) | — | Implementación de inspección, diagnóstico y herramientas de depuración. |
| [renderer/Console.h](../../gta3/src/renderer/Console.h) | [renderer/Console.h](../../gtavc/src/renderer/Console.h) | — | Declaraciones y tipos de inspección, diagnóstico y herramientas de depuración. |
| [extras/shaders/contrast.frag](../../gta3/src/extras/shaders/contrast.frag) | [extras/shaders/contrast.frag](../../gtavc/src/extras/shaders/contrast.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `contrast`. |
| [extras/shaders/obj/contrast_frag.inc](../../gta3/src/extras/shaders/obj/contrast_frag.inc) | [extras/shaders/obj/contrast_frag.inc](../../gtavc/src/extras/shaders/obj/contrast_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `contrast_frag`. |
| [extras/shaders/obj/contrastPS.cso](../../gta3/src/extras/shaders/obj/contrastPS.cso) | [extras/shaders/obj/contrastPS.cso](../../gtavc/src/extras/shaders/obj/contrastPS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `contrastPS`. |
| [extras/shaders/contrastPS.hlsl](../../gta3/src/extras/shaders/contrastPS.hlsl) | [extras/shaders/contrastPS.hlsl](../../gtavc/src/extras/shaders/contrastPS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `contrastPS`. |
| [extras/shaders/obj/contrastPS.inc](../../gta3/src/extras/shaders/obj/contrastPS.inc) | [extras/shaders/obj/contrastPS.inc](../../gtavc/src/extras/shaders/obj/contrastPS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `contrastPS`. |
| [core/ControllerConfig.cpp](../../gta3/src/core/ControllerConfig.cpp) | [core/ControllerConfig.cpp](../../gtavc/src/core/ControllerConfig.cpp) | — | Implementación de asignación y configuración de controles. |
| [core/ControllerConfig.h](../../gta3/src/core/ControllerConfig.h) | [core/ControllerConfig.h](../../gtavc/src/core/ControllerConfig.h) | — | Declaraciones y tipos de asignación y configuración de controles. |
| [skel/crossplatform.cpp](../../gta3/src/skel/crossplatform.cpp) | [skel/crossplatform.cpp](../../gtavc/src/skel/crossplatform.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [skel/crossplatform.h](../../gta3/src/skel/crossplatform.h) | [skel/crossplatform.h](../../gtavc/src/skel/crossplatform.h) | — | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [extras/cursor.inc](../../gta3/src/extras/cursor.inc) | [extras/cursor.inc](../../gtavc/src/extras/cursor.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `cursor`. |
| [extras/custompipes.cpp](../../gta3/src/extras/custompipes.cpp) | [extras/custompipes.cpp](../../gtavc/src/extras/custompipes.cpp) | — | Implementación de rutas de renderizado personalizadas para distintos backends. |
| [extras/custompipes.h](../../gta3/src/extras/custompipes.h) | [extras/custompipes.h](../../gtavc/src/extras/custompipes.h) | — | Declaraciones y tipos de rutas de renderizado personalizadas para distintos backends. |
| [extras/custompipes_d3d9.cpp](../../gta3/src/extras/custompipes_d3d9.cpp) | [extras/custompipes_d3d9.cpp](../../gtavc/src/extras/custompipes_d3d9.cpp) | — | Implementación de rutas de renderizado personalizadas para distintos backends. |
| [extras/custompipes_gl.cpp](../../gta3/src/extras/custompipes_gl.cpp) | [extras/custompipes_gl.cpp](../../gtavc/src/extras/custompipes_gl.cpp) | — | Implementación de rutas de renderizado personalizadas para distintos backends. |
| [objects/CutsceneHead.cpp](../../gta3/src/objects/CutsceneHead.cpp) | — | — | Implementación del componente `CutsceneHead` del área objects; objetos del escenario, sus propiedades e interacción; incluye objetos asociados a efectos. |
| [objects/CutsceneHead.h](../../gta3/src/objects/CutsceneHead.h) | — | — | Declaraciones y tipos del componente `CutsceneHead` del área objects; objetos del escenario, sus propiedades e interacción; incluye objetos asociados a efectos. |
| — | [renderer/CutsceneShadow.cpp](../../gtavc/src/renderer/CutsceneShadow.cpp) | — | Implementación de generación y dibujo de sombras. |
| — | [renderer/CutsceneShadow.h](../../gtavc/src/renderer/CutsceneShadow.h) | — | Declaraciones y tipos de generación y dibujo de sombras. |
| [extras/debugmenu.cpp](../../gta3/src/extras/debugmenu.cpp) | [extras/debugmenu.cpp](../../gtavc/src/extras/debugmenu.cpp) | — | Implementación de herramientas de depuración y opciones adicionales de interfaz. |
| [extras/debugmenu.h](../../gta3/src/extras/debugmenu.h) | [extras/debugmenu.h](../../gtavc/src/extras/debugmenu.h) | — | Declaraciones y tipos de herramientas de depuración y opciones adicionales de interfaz. |
| [extras/shaders/default_UV2.vert](../../gta3/src/extras/shaders/default_UV2.vert) | [extras/shaders/default_UV2.vert](../../gtavc/src/extras/shaders/default_UV2.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `default_UV2`. |
| [extras/shaders/obj/default_UV2_vert.inc](../../gta3/src/extras/shaders/obj/default_UV2_vert.inc) | [extras/shaders/obj/default_UV2_vert.inc](../../gtavc/src/extras/shaders/obj/default_UV2_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `default_UV2_vert`. |
| [extras/shaders/obj/default_UV2_VS.cso](../../gta3/src/extras/shaders/obj/default_UV2_VS.cso) | [extras/shaders/obj/default_UV2_VS.cso](../../gtavc/src/extras/shaders/obj/default_UV2_VS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `default_UV2_VS`. |
| [extras/shaders/default_UV2_VS.hlsl](../../gta3/src/extras/shaders/default_UV2_VS.hlsl) | [extras/shaders/default_UV2_VS.hlsl](../../gtavc/src/extras/shaders/default_UV2_VS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `default_UV2_VS`. |
| [extras/shaders/obj/default_UV2_VS.inc](../../gta3/src/extras/shaders/obj/default_UV2_VS.inc) | [extras/shaders/obj/default_UV2_VS.inc](../../gtavc/src/extras/shaders/obj/default_UV2_VS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `default_UV2_VS`. |
| [audio/DMAudio.cpp](../../gta3/src/audio/DMAudio.cpp) | [audio/DMAudio.cpp](../../gtavc/src/audio/DMAudio.cpp) | — | Implementación de interfaz principal del sistema de sonido. |
| [audio/DMAudio.h](../../gta3/src/audio/DMAudio.h) | [audio/DMAudio.h](../../gtavc/src/audio/DMAudio.h) | — | Declaraciones y tipos de interfaz principal del sistema de sonido. |
| [audio/eax/eax-util.cpp](../../gta3/src/audio/eax/eax-util.cpp) | [audio/eax/eax-util.cpp](../../gtavc/src/audio/eax/eax-util.cpp) | — | Implementación de `eax-util`: reproducción, configuración o soporte del sistema de audio. |
| [audio/eax/eax-util.h](../../gta3/src/audio/eax/eax-util.h) | [audio/eax/eax-util.h](../../gtavc/src/audio/eax/eax-util.h) | — | Declaraciones y tipos de `eax-util`: reproducción, configuración o soporte del sistema de audio. |
| [audio/eax/eax.h](../../gta3/src/audio/eax/eax.h) | [audio/eax/eax.h](../../gtavc/src/audio/eax/eax.h) | — | Declaraciones y tipos de `eax`: reproducción, configuración o soporte del sistema de audio. |
| [core/EventList.cpp](../../gta3/src/core/EventList.cpp) | [core/EventList.cpp](../../gtavc/src/core/EventList.cpp) | — | Implementación de registro de eventos del mundo. |
| [core/EventList.h](../../gta3/src/core/EventList.h) | [core/EventList.h](../../gtavc/src/core/EventList.h) | — | Declaraciones y tipos de registro de eventos del mundo. |
| [skel/events.cpp](../../gta3/src/skel/events.cpp) | [skel/events.cpp](../../gtavc/src/skel/events.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [skel/events.h](../../gta3/src/skel/events.h) | [skel/events.h](../../gtavc/src/skel/events.h) | — | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [fakerw/fake.cpp](../../gta3/src/fakerw/fake.cpp) | [fakerw/fake.cpp](../../gtavc/src/fakerw/fake.cpp) | — | Implementación de `fake`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [vehicles/Floater.cpp](../../gta3/src/vehicles/Floater.cpp) | [vehicles/Floater.cpp](../../gtavc/src/vehicles/Floater.cpp) | — | Implementación de flotación y fuerzas asociadas al agua. |
| [vehicles/Floater.h](../../gta3/src/vehicles/Floater.h) | [vehicles/Floater.h](../../gtavc/src/vehicles/Floater.h) | — | Declaraciones y tipos de flotación y fuerzas asociadas al agua. |
| [renderer/Fluff.cpp](../../gta3/src/renderer/Fluff.cpp) | [renderer/Fluff.cpp](../../gtavc/src/renderer/Fluff.cpp) | — | Implementación de elementos ambientales decorativos. |
| [renderer/Fluff.h](../../gta3/src/renderer/Fluff.h) | [renderer/Fluff.h](../../gtavc/src/renderer/Fluff.h) | — | Declaraciones y tipos de elementos ambientales decorativos. |
| [animation/FrameUpdate.cpp](../../gta3/src/animation/FrameUpdate.cpp) | [animation/FrameUpdate.cpp](../../gtavc/src/animation/FrameUpdate.cpp) | — | Implementación de huesos y actualización de la jerarquía animada. |
| [core/Frontend.cpp](../../gta3/src/core/Frontend.cpp) | [core/Frontend.cpp](../../gtavc/src/core/Frontend.cpp) | — | Implementación de menús, opciones y navegación de la interfaz. |
| [core/Frontend.h](../../gta3/src/core/Frontend.h) | [core/Frontend.h](../../gtavc/src/core/Frontend.h) | — | Declaraciones y tipos de menús, opciones y navegación de la interfaz. |
| [core/Frontend_PS2.cpp](../../gta3/src/core/Frontend_PS2.cpp) | [core/Frontend_PS2.cpp](../../gtavc/src/core/Frontend_PS2.cpp) | — | Implementación de menús, opciones y navegación de la interfaz. |
| [core/Frontend_PS2.h](../../gta3/src/core/Frontend_PS2.h) | [core/Frontend_PS2.h](../../gtavc/src/core/Frontend_PS2.h) | — | Declaraciones y tipos de menús, opciones y navegación de la interfaz. |
| [core/FrontEndControls.cpp](../../gta3/src/core/FrontEndControls.cpp) | [core/FrontEndControls.cpp](../../gtavc/src/core/FrontEndControls.cpp) | — | Implementación de asignación y configuración de controles. |
| [core/FrontEndControls.h](../../gta3/src/core/FrontEndControls.h) | [core/FrontEndControls.h](../../gtavc/src/core/FrontEndControls.h) | — | Declaraciones y tipos de asignación y configuración de controles. |
| [extras/frontendoption.cpp](../../gta3/src/extras/frontendoption.cpp) | [extras/frontendoption.cpp](../../gtavc/src/extras/frontendoption.cpp) | — | Implementación de herramientas de depuración y opciones adicionales de interfaz. |
| [extras/frontendoption.h](../../gta3/src/extras/frontendoption.h) | [extras/frontendoption.h](../../gtavc/src/extras/frontendoption.h) | — | Declaraciones y tipos de herramientas de depuración y opciones adicionales de interfaz. |
| [core/FrontendTriggers.h](../../gta3/src/core/FrontendTriggers.h) | [core/FrontendTriggers.h](../../gtavc/src/core/FrontendTriggers.h) | — | Declaraciones y tipos del componente `FrontendTriggers` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [extras/GitSHA1.cpp.in](../../gta3/src/extras/GitSHA1.cpp.in) | [extras/GitSHA1.cpp.in](../../gtavc/src/extras/GitSHA1.cpp.in) | — | Plantilla utilizada para generar un archivo de configuración o código. Recurso: `GitSHA1.cpp`. |
| [extras/GitSHA1.h](../../gta3/src/extras/GitSHA1.h) | [extras/GitSHA1.h](../../gtavc/src/extras/GitSHA1.h) | — | Declaraciones y tipos de identificación de la revisión del código. |
| [skel/glfw/glfw.cpp](../../gta3/src/skel/glfw/glfw.cpp) | [skel/glfw/glfw.cpp](../../gtavc/src/skel/glfw/glfw.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [skel/win/gta3.ico](../../gta3/src/skel/win/gta3.ico) | — | — | Recurso de icono de la aplicación. Recurso: `gta3`. |
| — | [skel/win/gtavc.ico](../../gtavc/src/skel/win/gtavc.ico) | — | Recurso de icono de la aplicación. Recurso: `gtavc`. |
| [vehicles/HandlingMgr.cpp](../../gta3/src/vehicles/HandlingMgr.cpp) | [vehicles/HandlingMgr.cpp](../../gtavc/src/vehicles/HandlingMgr.cpp) | — | Implementación de propiedades que determinan el manejo de vehículos. |
| [vehicles/HandlingMgr.h](../../gta3/src/vehicles/HandlingMgr.h) | [vehicles/HandlingMgr.h](../../gtavc/src/vehicles/HandlingMgr.h) | — | Declaraciones y tipos de propiedades que determinan el manejo de vehículos. |
| [extras/shaders/im2d.vert](../../gta3/src/extras/shaders/im2d.vert) | [extras/shaders/im2d.vert](../../gtavc/src/extras/shaders/im2d.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `im2d`. |
| [extras/shaders/im2d_UV2.vert](../../gta3/src/extras/shaders/im2d_UV2.vert) | [extras/shaders/im2d_UV2.vert](../../gtavc/src/extras/shaders/im2d_UV2.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `im2d_UV2`. |
| [extras/shaders/obj/im2d_UV2_vert.inc](../../gta3/src/extras/shaders/obj/im2d_UV2_vert.inc) | [extras/shaders/obj/im2d_UV2_vert.inc](../../gtavc/src/extras/shaders/obj/im2d_UV2_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `im2d_UV2_vert`. |
| [extras/shaders/obj/im2d_vert.inc](../../gta3/src/extras/shaders/obj/im2d_vert.inc) | [extras/shaders/obj/im2d_vert.inc](../../gtavc/src/extras/shaders/obj/im2d_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `im2d_vert`. |
| [extras/ini.h](../../gta3/src/extras/ini.h) | [extras/ini.h](../../gtavc/src/extras/ini.h) | — | Declaraciones y tipos de lectura o gestión de configuración INI. |
| [renderer/Instance.cpp](../../gta3/src/renderer/Instance.cpp) | [renderer/Instance.cpp](../../gtavc/src/renderer/Instance.cpp) | — | Implementación del componente `Instance` del área renderer; dibujo de la escena, visibilidad, iluminación, sombras y efectos visuales. |
| [renderer/Instance.h](../../gta3/src/renderer/Instance.h) | [renderer/Instance.h](../../gtavc/src/renderer/Instance.h) | — | Declaraciones y tipos del componente `Instance` del área renderer; dibujo de la escena, visibilidad, iluminación, sombras y efectos visuales. |
| [extras/shaders/lighting.h](../../gta3/src/extras/shaders/lighting.h) | [extras/shaders/lighting.h](../../gtavc/src/extras/shaders/lighting.h) | — | Declaraciones y tipos del componente `lighting` del área extras; extensiones del proyecto, herramientas y efectos gráficos adicionales; correspondencia parcial con reSA. |
| [rw/Lights.cpp](../../gta3/src/rw/Lights.cpp) | [rw/Lights.cpp](../../gtavc/src/rw/Lights.cpp) | — | Implementación de `Lights`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [rw/Lights.h](../../gta3/src/rw/Lights.h) | [rw/Lights.h](../../gtavc/src/rw/Lights.h) | — | Declaraciones y tipos de `Lights`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [core/Lists.cpp](../../gta3/src/core/Lists.cpp) | [core/Lists.cpp](../../gtavc/src/core/Lists.cpp) | — | Implementación del componente `Lists` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [core/Lists.h](../../gta3/src/core/Lists.h) | [core/Lists.h](../../gtavc/src/core/Lists.h) | — | Declaraciones y tipos del componente `Lists` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [core/main.cpp](../../gta3/src/core/main.cpp) | [core/main.cpp](../../gtavc/src/core/main.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [core/main.h](../../gta3/src/core/main.h) | [core/main.h](../../gtavc/src/core/main.h) | — | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [extras/shaders/make_glsl.sh](../../gta3/src/extras/shaders/make_glsl.sh) | [extras/shaders/make_glsl.sh](../../gtavc/src/extras/shaders/make_glsl.sh) | — | Script de shell para operaciones auxiliares del proyecto. Recurso: `make_glsl`. |
| [extras/shaders/make_hlsl.cmd](../../gta3/src/extras/shaders/make_hlsl.cmd) | [extras/shaders/make_hlsl.cmd](../../gtavc/src/extras/shaders/make_hlsl.cmd) | — | Script de comandos de Windows para operaciones auxiliares del proyecto. Recurso: `make_hlsl`. |
| [extras/shaders/makeinc_glsl.sh](../../gta3/src/extras/shaders/makeinc_glsl.sh) | [extras/shaders/makeinc_glsl.sh](../../gtavc/src/extras/shaders/makeinc_glsl.sh) | — | Script de shell para operaciones auxiliares del proyecto. Recurso: `makeinc_glsl`. |
| [extras/shaders/makeinc_hlsl.sh](../../gta3/src/extras/shaders/makeinc_hlsl.sh) | [extras/shaders/makeinc_hlsl.sh](../../gtavc/src/extras/shaders/makeinc_hlsl.sh) | — | Script de shell para operaciones auxiliares del proyecto. Recurso: `makeinc_hlsl`. |
| [math/math.cpp](../../gta3/src/math/math.cpp) | [math/math.cpp](../../gtavc/src/math/math.cpp) | — | Implementación de utilidades, definiciones u operaciones compartidas. |
| [save/MemoryCard.cpp](../../gta3/src/save/MemoryCard.cpp) | [save/MemoryCard.cpp](../../gtavc/src/save/MemoryCard.cpp) | — | Implementación de gestión de memoria o almacenamiento según la implementación. |
| [save/MemoryCard.h](../../gta3/src/save/MemoryCard.h) | [save/MemoryCard.h](../../gtavc/src/save/MemoryCard.h) | — | Declaraciones y tipos de gestión de memoria o almacenamiento según la implementación. |
| [core/MenuScreens.cpp](../../gta3/src/core/MenuScreens.cpp) | [core/MenuScreens.cpp](../../gtavc/src/core/MenuScreens.cpp) | — | Implementación de menús, opciones y navegación de la interfaz. |
| [core/MenuScreensCustom.cpp](../../gta3/src/core/MenuScreensCustom.cpp) | [core/MenuScreensCustom.cpp](../../gtavc/src/core/MenuScreensCustom.cpp) | — | Implementación de menús, opciones y navegación de la interfaz. |
| [modelinfo/MloModelInfo.cpp](../../gta3/src/modelinfo/MloModelInfo.cpp) | [modelinfo/MloModelInfo.cpp](../../gtavc/src/modelinfo/MloModelInfo.cpp) | — | Implementación de los metadatos de modelo `MloModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/MloModelInfo.h](../../gta3/src/modelinfo/MloModelInfo.h) | [modelinfo/MloModelInfo.h](../../gtavc/src/modelinfo/MloModelInfo.h) | — | Declaraciones y tipos de los metadatos de modelo `MloModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [audio/MusicManager.cpp](../../gta3/src/audio/MusicManager.cpp) | [audio/MusicManager.cpp](../../gtavc/src/audio/MusicManager.cpp) | — | Implementación de selección y reproducción de música y radio. |
| [audio/MusicManager.h](../../gta3/src/audio/MusicManager.h) | [audio/MusicManager.h](../../gtavc/src/audio/MusicManager.h) | — | Declaraciones y tipos de selección y reproducción de música y radio. |
| [control/NameGrid.cpp](../../gta3/src/control/NameGrid.cpp) | [control/NameGrid.cpp](../../gtavc/src/control/NameGrid.cpp) | — | Implementación del componente `NameGrid` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [control/NameGrid.h](../../gta3/src/control/NameGrid.h) | [control/NameGrid.h](../../gtavc/src/control/NameGrid.h) | — | Declaraciones y tipos del componente `NameGrid` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [extras/shaders/neoGloss.frag](../../gta3/src/extras/shaders/neoGloss.frag) | [extras/shaders/neoGloss.frag](../../gtavc/src/extras/shaders/neoGloss.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `neoGloss`. |
| [extras/shaders/neoGloss.vert](../../gta3/src/extras/shaders/neoGloss.vert) | [extras/shaders/neoGloss.vert](../../gtavc/src/extras/shaders/neoGloss.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `neoGloss`. |
| [extras/shaders/obj/neoGloss_frag.inc](../../gta3/src/extras/shaders/obj/neoGloss_frag.inc) | [extras/shaders/obj/neoGloss_frag.inc](../../gtavc/src/extras/shaders/obj/neoGloss_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoGloss_frag`. |
| [extras/shaders/obj/neoGloss_PS.cso](../../gta3/src/extras/shaders/obj/neoGloss_PS.cso) | [extras/shaders/obj/neoGloss_PS.cso](../../gtavc/src/extras/shaders/obj/neoGloss_PS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoGloss_PS`. |
| [extras/shaders/neoGloss_PS.hlsl](../../gta3/src/extras/shaders/neoGloss_PS.hlsl) | [extras/shaders/neoGloss_PS.hlsl](../../gtavc/src/extras/shaders/neoGloss_PS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoGloss_PS`. |
| [extras/shaders/obj/neoGloss_PS.inc](../../gta3/src/extras/shaders/obj/neoGloss_PS.inc) | [extras/shaders/obj/neoGloss_PS.inc](../../gtavc/src/extras/shaders/obj/neoGloss_PS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoGloss_PS`. |
| [extras/shaders/obj/neoGloss_vert.inc](../../gta3/src/extras/shaders/obj/neoGloss_vert.inc) | [extras/shaders/obj/neoGloss_vert.inc](../../gtavc/src/extras/shaders/obj/neoGloss_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoGloss_vert`. |
| [extras/shaders/obj/neoGloss_VS.cso](../../gta3/src/extras/shaders/obj/neoGloss_VS.cso) | [extras/shaders/obj/neoGloss_VS.cso](../../gtavc/src/extras/shaders/obj/neoGloss_VS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoGloss_VS`. |
| [extras/shaders/neoGloss_VS.hlsl](../../gta3/src/extras/shaders/neoGloss_VS.hlsl) | [extras/shaders/neoGloss_VS.hlsl](../../gtavc/src/extras/shaders/neoGloss_VS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoGloss_VS`. |
| [extras/shaders/obj/neoGloss_VS.inc](../../gta3/src/extras/shaders/obj/neoGloss_VS.inc) | [extras/shaders/obj/neoGloss_VS.inc](../../gtavc/src/extras/shaders/obj/neoGloss_VS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoGloss_VS`. |
| [extras/shaders/neoRim.vert](../../gta3/src/extras/shaders/neoRim.vert) | [extras/shaders/neoRim.vert](../../gtavc/src/extras/shaders/neoRim.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `neoRim`. |
| [extras/shaders/obj/neoRim_vert.inc](../../gta3/src/extras/shaders/obj/neoRim_vert.inc) | [extras/shaders/obj/neoRim_vert.inc](../../gtavc/src/extras/shaders/obj/neoRim_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoRim_vert`. |
| [extras/shaders/obj/neoRim_VS.cso](../../gta3/src/extras/shaders/obj/neoRim_VS.cso) | [extras/shaders/obj/neoRim_VS.cso](../../gtavc/src/extras/shaders/obj/neoRim_VS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoRim_VS`. |
| [extras/shaders/neoRim_VS.hlsl](../../gta3/src/extras/shaders/neoRim_VS.hlsl) | [extras/shaders/neoRim_VS.hlsl](../../gtavc/src/extras/shaders/neoRim_VS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoRim_VS`. |
| [extras/shaders/obj/neoRim_VS.inc](../../gta3/src/extras/shaders/obj/neoRim_VS.inc) | [extras/shaders/obj/neoRim_VS.inc](../../gtavc/src/extras/shaders/obj/neoRim_VS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoRim_VS`. |
| [extras/shaders/neoRimSkin.vert](../../gta3/src/extras/shaders/neoRimSkin.vert) | [extras/shaders/neoRimSkin.vert](../../gtavc/src/extras/shaders/neoRimSkin.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `neoRimSkin`. |
| [extras/shaders/obj/neoRimSkin_vert.inc](../../gta3/src/extras/shaders/obj/neoRimSkin_vert.inc) | [extras/shaders/obj/neoRimSkin_vert.inc](../../gtavc/src/extras/shaders/obj/neoRimSkin_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoRimSkin_vert`. |
| [extras/shaders/obj/neoRimSkin_VS.cso](../../gta3/src/extras/shaders/obj/neoRimSkin_VS.cso) | [extras/shaders/obj/neoRimSkin_VS.cso](../../gtavc/src/extras/shaders/obj/neoRimSkin_VS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoRimSkin_VS`. |
| [extras/shaders/neoRimSkin_VS.hlsl](../../gta3/src/extras/shaders/neoRimSkin_VS.hlsl) | [extras/shaders/neoRimSkin_VS.hlsl](../../gtavc/src/extras/shaders/neoRimSkin_VS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoRimSkin_VS`. |
| [extras/shaders/obj/neoRimSkin_VS.inc](../../gta3/src/extras/shaders/obj/neoRimSkin_VS.inc) | [extras/shaders/obj/neoRimSkin_VS.inc](../../gtavc/src/extras/shaders/obj/neoRimSkin_VS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoRimSkin_VS`. |
| [extras/shaders/neoVehicle.frag](../../gta3/src/extras/shaders/neoVehicle.frag) | [extras/shaders/neoVehicle.frag](../../gtavc/src/extras/shaders/neoVehicle.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `neoVehicle`. |
| [extras/shaders/neoVehicle.vert](../../gta3/src/extras/shaders/neoVehicle.vert) | [extras/shaders/neoVehicle.vert](../../gtavc/src/extras/shaders/neoVehicle.vert) | — | Shader de vértices: transforma geometría y prepara datos para dibujar. Recurso: `neoVehicle`. |
| [extras/shaders/obj/neoVehicle_frag.inc](../../gta3/src/extras/shaders/obj/neoVehicle_frag.inc) | [extras/shaders/obj/neoVehicle_frag.inc](../../gtavc/src/extras/shaders/obj/neoVehicle_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoVehicle_frag`. |
| [extras/shaders/obj/neoVehicle_PS.cso](../../gta3/src/extras/shaders/obj/neoVehicle_PS.cso) | [extras/shaders/obj/neoVehicle_PS.cso](../../gtavc/src/extras/shaders/obj/neoVehicle_PS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoVehicle_PS`. |
| [extras/shaders/neoVehicle_PS.hlsl](../../gta3/src/extras/shaders/neoVehicle_PS.hlsl) | [extras/shaders/neoVehicle_PS.hlsl](../../gtavc/src/extras/shaders/neoVehicle_PS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoVehicle_PS`. |
| [extras/shaders/obj/neoVehicle_PS.inc](../../gta3/src/extras/shaders/obj/neoVehicle_PS.inc) | [extras/shaders/obj/neoVehicle_PS.inc](../../gtavc/src/extras/shaders/obj/neoVehicle_PS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoVehicle_PS`. |
| [extras/shaders/obj/neoVehicle_vert.inc](../../gta3/src/extras/shaders/obj/neoVehicle_vert.inc) | [extras/shaders/obj/neoVehicle_vert.inc](../../gtavc/src/extras/shaders/obj/neoVehicle_vert.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoVehicle_vert`. |
| [extras/shaders/obj/neoVehicle_VS.cso](../../gta3/src/extras/shaders/obj/neoVehicle_VS.cso) | [extras/shaders/obj/neoVehicle_VS.cso](../../gtavc/src/extras/shaders/obj/neoVehicle_VS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoVehicle_VS`. |
| [extras/shaders/neoVehicle_VS.hlsl](../../gta3/src/extras/shaders/neoVehicle_VS.hlsl) | [extras/shaders/neoVehicle_VS.hlsl](../../gtavc/src/extras/shaders/neoVehicle_VS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoVehicle_VS`. |
| [extras/shaders/obj/neoVehicle_VS.inc](../../gta3/src/extras/shaders/obj/neoVehicle_VS.inc) | [extras/shaders/obj/neoVehicle_VS.inc](../../gtavc/src/extras/shaders/obj/neoVehicle_VS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoVehicle_VS`. |
| [extras/shaders/neoWorldIII.frag](../../gta3/src/extras/shaders/neoWorldIII.frag) | — | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `neoWorldIII`. |
| [extras/shaders/obj/neoWorldIII_frag.inc](../../gta3/src/extras/shaders/obj/neoWorldIII_frag.inc) | — | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoWorldIII_frag`. |
| [extras/shaders/obj/neoWorldIII_PS.cso](../../gta3/src/extras/shaders/obj/neoWorldIII_PS.cso) | — | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoWorldIII_PS`. |
| [extras/shaders/neoWorldIII_PS.hlsl](../../gta3/src/extras/shaders/neoWorldIII_PS.hlsl) | — | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoWorldIII_PS`. |
| [extras/shaders/obj/neoWorldIII_PS.inc](../../gta3/src/extras/shaders/obj/neoWorldIII_PS.inc) | — | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoWorldIII_PS`. |
| — | [extras/shaders/neoWorldVC.frag](../../gtavc/src/extras/shaders/neoWorldVC.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `neoWorldVC`. |
| — | [extras/shaders/obj/neoWorldVC_frag.inc](../../gtavc/src/extras/shaders/obj/neoWorldVC_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoWorldVC_frag`. |
| — | [extras/shaders/obj/neoWorldVC_PS.cso](../../gtavc/src/extras/shaders/obj/neoWorldVC_PS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `neoWorldVC_PS`. |
| — | [extras/shaders/neoWorldVC_PS.hlsl](../../gtavc/src/extras/shaders/neoWorldVC_PS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `neoWorldVC_PS`. |
| — | [extras/shaders/obj/neoWorldVC_PS.inc](../../gtavc/src/extras/shaders/obj/neoWorldVC_PS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `neoWorldVC_PS`. |
| [audio/oal/oal_utils.cpp](../../gta3/src/audio/oal/oal_utils.cpp) | [audio/oal/oal_utils.cpp](../../gtavc/src/audio/oal/oal_utils.cpp) | — | Implementación de `oal_utils`: reproducción, configuración o soporte del sistema de audio. |
| [audio/oal/oal_utils.h](../../gta3/src/audio/oal/oal_utils.h) | [audio/oal/oal_utils.h](../../gtavc/src/audio/oal/oal_utils.h) | — | Declaraciones y tipos de `oal_utils`: reproducción, configuración o soporte del sistema de audio. |
| [core/obrstr.cpp](../../gta3/src/core/obrstr.cpp) | [core/obrstr.cpp](../../gtavc/src/core/obrstr.cpp) | — | Implementación del componente `obrstr` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [core/obrstr.h](../../gta3/src/core/obrstr.h) | [core/obrstr.h](../../gtavc/src/core/obrstr.h) | — | Declaraciones y tipos del componente `obrstr` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [text/Pager.cpp](../../gta3/src/text/Pager.cpp) | [text/Pager.cpp](../../gtavc/src/text/Pager.cpp) | — | Implementación de mensajes presentados al jugador. |
| [text/Pager.h](../../gta3/src/text/Pager.h) | [text/Pager.h](../../gtavc/src/text/Pager.h) | — | Declaraciones y tipos de mensajes presentados al jugador. |
| [renderer/Particle.cpp](../../gta3/src/renderer/Particle.cpp) | [renderer/Particle.cpp](../../gtavc/src/renderer/Particle.cpp) | — | Implementación de partículas y efectos; su organización difiere entre juegos. |
| [renderer/ParticleMgr.cpp](../../gta3/src/renderer/ParticleMgr.cpp) | [renderer/ParticleMgr.cpp](../../gtavc/src/renderer/ParticleMgr.cpp) | — | Implementación de partículas y efectos; su organización difiere entre juegos. |
| [renderer/ParticleMgr.h](../../gta3/src/renderer/ParticleMgr.h) | [renderer/ParticleMgr.h](../../gtavc/src/renderer/ParticleMgr.h) | — | Declaraciones y tipos de partículas y efectos; su organización difiere entre juegos. |
| [objects/ParticleObject.cpp](../../gta3/src/objects/ParticleObject.cpp) | [objects/ParticleObject.cpp](../../gtavc/src/objects/ParticleObject.cpp) | — | Implementación de partículas y efectos; su organización difiere entre juegos. |
| [objects/ParticleObject.h](../../gta3/src/objects/ParticleObject.h) | [objects/ParticleObject.h](../../gtavc/src/objects/ParticleObject.h) | — | Declaraciones y tipos de partículas y efectos; su organización difiere entre juegos. |
| [renderer/ParticleType.h](../../gta3/src/renderer/ParticleType.h) | [renderer/ParticleType.h](../../gtavc/src/renderer/ParticleType.h) | — | Declaraciones y tipos de partículas y efectos; su organización difiere entre juegos. |
| [save/PCSave.cpp](../../gta3/src/save/PCSave.cpp) | [save/PCSave.cpp](../../gtavc/src/save/PCSave.cpp) | — | Implementación de serialización y almacenamiento de partidas. |
| [save/PCSave.h](../../gta3/src/save/PCSave.h) | [save/PCSave.h](../../gtavc/src/save/PCSave.h) | — | Declaraciones y tipos de serialización y almacenamiento de partidas. |
| [peds/PedAI.cpp](../../gta3/src/peds/PedAI.cpp) | [peds/PedAI.cpp](../../gtavc/src/peds/PedAI.cpp) | — | Implementación de IA, combate o conversación de peatones. |
| [peds/PedChat.cpp](../../gta3/src/peds/PedChat.cpp) | [peds/PedChat.cpp](../../gtavc/src/peds/PedChat.cpp) | — | Implementación de IA, combate o conversación de peatones. |
| [peds/PedDebug.cpp](../../gta3/src/peds/PedDebug.cpp) | [peds/PedDebug.cpp](../../gtavc/src/peds/PedDebug.cpp) | — | Implementación de inspección, diagnóstico y herramientas de depuración. |
| [peds/PedFight.cpp](../../gta3/src/peds/PedFight.cpp) | [peds/PedFight.cpp](../../gtavc/src/peds/PedFight.cpp) | — | Implementación de IA, combate o conversación de peatones. |
| [peds/PedRoutes.cpp](../../gta3/src/peds/PedRoutes.cpp) | [peds/PedRoutes.cpp](../../gtavc/src/peds/PedRoutes.cpp) | — | Implementación de rutas, nodos y búsqueda de caminos. |
| [peds/PedRoutes.h](../../gta3/src/peds/PedRoutes.h) | [peds/PedRoutes.h](../../gtavc/src/peds/PedRoutes.h) | — | Declaraciones y tipos de rutas, nodos y búsqueda de caminos. |
| [control/Phones.cpp](../../gta3/src/control/Phones.cpp) | [control/Phones.cpp](../../gtavc/src/control/Phones.cpp) | — | Implementación de teléfonos y su interacción. |
| [control/Phones.h](../../gta3/src/control/Phones.h) | [control/Phones.h](../../gtavc/src/control/Phones.h) | — | Declaraciones y tipos de teléfonos y su interacción. |
| [skel/platform.h](../../gta3/src/skel/platform.h) | [skel/platform.h](../../gtavc/src/skel/platform.h) | — | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [audio/PolRadio.cpp](../../gta3/src/audio/PolRadio.cpp) | [audio/PolRadio.cpp](../../gtavc/src/audio/PolRadio.cpp) | — | Implementación de mensajes de radio policial. |
| [audio/PolRadio.h](../../gta3/src/audio/PolRadio.h) | [audio/PolRadio.h](../../gtavc/src/audio/PolRadio.h) | — | Declaraciones y tipos de mensajes de radio policial. |
| [extras/postfx.cpp](../../gta3/src/extras/postfx.cpp) | [extras/postfx.cpp](../../gtavc/src/extras/postfx.cpp) | — | Implementación de efectos aplicados a la imagen renderizada. |
| [extras/postfx.h](../../gta3/src/extras/postfx.h) | [extras/postfx.h](../../gtavc/src/extras/postfx.h) | — | Declaraciones y tipos de efectos aplicados a la imagen renderizada. |
| [control/PowerPoints.cpp](../../gta3/src/control/PowerPoints.cpp) | [control/PowerPoints.cpp](../../gtavc/src/control/PowerPoints.cpp) | — | Implementación del componente `PowerPoints` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [control/PowerPoints.h](../../gta3/src/control/PowerPoints.h) | [control/PowerPoints.h](../../gtavc/src/control/PowerPoints.h) | — | Declaraciones y tipos del componente `PowerPoints` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [core/Profile.cpp](../../gta3/src/core/Profile.cpp) | [core/Profile.cpp](../../gtavc/src/core/Profile.cpp) | — | Implementación de medición y visualización de rendimiento. |
| [core/Profile.h](../../gta3/src/core/Profile.h) | [core/Profile.h](../../gtavc/src/core/Profile.h) | — | Declaraciones y tipos de medición y visualización de rendimiento. |
| [core/re3.cpp](../../gta3/src/core/re3.cpp) | [core/re3.cpp](../../gtavc/src/core/re3.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [extras/re3_inttypes.h](../../gta3/src/extras/re3_inttypes.h) | [extras/re3_inttypes.h](../../gtavc/src/extras/re3_inttypes.h) | — | Declaraciones y tipos de tipos enteros utilizados por el proyecto. |
| [control/Record.cpp](../../gta3/src/control/Record.cpp) | [control/Record.cpp](../../gtavc/src/control/Record.cpp) | — | Implementación de grabación o reproducción de secuencias del juego. |
| [control/Record.h](../../gta3/src/control/Record.h) | [control/Record.h](../../gtavc/src/control/Record.h) | — | Declaraciones y tipos de grabación o reproducción de secuencias del juego. |
| [renderer/RenderBuffer.h](../../gta3/src/renderer/RenderBuffer.h) | [renderer/RenderBuffer.h](../../gtavc/src/renderer/RenderBuffer.h) | — | Declaraciones y tipos de almacenamiento temporal de geometría para dibujar. |
| [skel/win/resource.h](../../gta3/src/skel/win/resource.h) | [skel/win/resource.h](../../gtavc/src/skel/win/resource.h) | — | Declaraciones y tipos del componente `resource` del área skel; arranque, ventanas y eventos de plataforma; su contraparte en reSA está fuera de game_sa, en source/app. |
| [rw/RwMatFX.cpp](../../gta3/src/rw/RwMatFX.cpp) | [rw/RwMatFX.cpp](../../gtavc/src/rw/RwMatFX.cpp) | — | Implementación de `RwMatFX`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [rw/RwPS2AlphaTest.cpp](../../gta3/src/rw/RwPS2AlphaTest.cpp) | [rw/RwPS2AlphaTest.cpp](../../gtavc/src/rw/RwPS2AlphaTest.cpp) | — | Implementación de `RwPS2AlphaTest`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| [audio/sampman.h](../../gta3/src/audio/sampman.h) | [audio/sampman.h](../../gtavc/src/audio/sampman.h) | — | Declaraciones y tipos de gestión de muestras de audio y sus backends. |
| [audio/sampman_miles.cpp](../../gta3/src/audio/sampman_miles.cpp) | [audio/sampman_miles.cpp](../../gtavc/src/audio/sampman_miles.cpp) | — | Implementación de gestión de muestras de audio y sus backends. |
| [audio/sampman_null.cpp](../../gta3/src/audio/sampman_null.cpp) | [audio/sampman_null.cpp](../../gtavc/src/audio/sampman_null.cpp) | — | Implementación de gestión de muestras de audio y sus backends. |
| [audio/sampman_oal.cpp](../../gta3/src/audio/sampman_oal.cpp) | [audio/sampman_oal.cpp](../../gtavc/src/audio/sampman_oal.cpp) | — | Implementación de gestión de muestras de audio y sus backends. |
| [save/SaveBuf.h](../../gta3/src/save/SaveBuf.h) | [save/SaveBuf.h](../../gtavc/src/save/SaveBuf.h) | — | Declaraciones y tipos de serialización y almacenamiento de partidas. |
| [control/SceneEdit.cpp](../../gta3/src/control/SceneEdit.cpp) | [control/SceneEdit.cpp](../../gtavc/src/control/SceneEdit.cpp) | — | Implementación del componente `SceneEdit` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [control/SceneEdit.h](../../gta3/src/control/SceneEdit.h) | [control/SceneEdit.h](../../gtavc/src/control/SceneEdit.h) | — | Declaraciones y tipos del componente `SceneEdit` del área control; lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico. |
| [extras/shaders/screenDroplet.frag](../../gta3/src/extras/shaders/screenDroplet.frag) | [extras/shaders/screenDroplet.frag](../../gtavc/src/extras/shaders/screenDroplet.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `screenDroplet`. |
| [extras/shaders/obj/screenDroplet_frag.inc](../../gta3/src/extras/shaders/obj/screenDroplet_frag.inc) | [extras/shaders/obj/screenDroplet_frag.inc](../../gtavc/src/extras/shaders/obj/screenDroplet_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `screenDroplet_frag`. |
| [extras/shaders/obj/screenDroplet_PS.cso](../../gta3/src/extras/shaders/obj/screenDroplet_PS.cso) | [extras/shaders/obj/screenDroplet_PS.cso](../../gtavc/src/extras/shaders/obj/screenDroplet_PS.cso) | — | Shader compilado utilizado por el backend gráfico. Recurso: `screenDroplet_PS`. |
| [extras/shaders/screenDroplet_PS.hlsl](../../gta3/src/extras/shaders/screenDroplet_PS.hlsl) | [extras/shaders/screenDroplet_PS.hlsl](../../gtavc/src/extras/shaders/screenDroplet_PS.hlsl) | — | Programa de sombreado HLSL para procesar geometría o píxeles en la GPU. Recurso: `screenDroplet_PS`. |
| [extras/shaders/obj/screenDroplet_PS.inc](../../gta3/src/extras/shaders/obj/screenDroplet_PS.inc) | [extras/shaders/obj/screenDroplet_PS.inc](../../gtavc/src/extras/shaders/obj/screenDroplet_PS.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `screenDroplet_PS`. |
| [extras/screendroplets.cpp](../../gta3/src/extras/screendroplets.cpp) | [extras/screendroplets.cpp](../../gtavc/src/extras/screendroplets.cpp) | — | Implementación de efecto de gotas sobre la imagen de pantalla. |
| [extras/screendroplets.h](../../gta3/src/extras/screendroplets.h) | [extras/screendroplets.h](../../gtavc/src/extras/screendroplets.h) | — | Declaraciones y tipos de efecto de gotas sobre la imagen de pantalla. |
| [control/Script.cpp](../../gta3/src/control/Script.cpp) | [control/Script.cpp](../../gtavc/src/control/Script.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/Script.h](../../gta3/src/control/Script.h) | [control/Script.h](../../gtavc/src/control/Script.h) | — | Declaraciones y tipos de ejecución y administración de scripts de misiones. |
| [control/Script2.cpp](../../gta3/src/control/Script2.cpp) | [control/Script2.cpp](../../gtavc/src/control/Script2.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/Script3.cpp](../../gta3/src/control/Script3.cpp) | [control/Script3.cpp](../../gtavc/src/control/Script3.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/Script4.cpp](../../gta3/src/control/Script4.cpp) | [control/Script4.cpp](../../gtavc/src/control/Script4.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/Script5.cpp](../../gta3/src/control/Script5.cpp) | [control/Script5.cpp](../../gtavc/src/control/Script5.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/Script6.cpp](../../gta3/src/control/Script6.cpp) | [control/Script6.cpp](../../gtavc/src/control/Script6.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| — | [control/Script7.cpp](../../gtavc/src/control/Script7.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| — | [control/Script8.cpp](../../gtavc/src/control/Script8.cpp) | — | Implementación de ejecución y administración de scripts de misiones. |
| [control/ScriptCommands.h](../../gta3/src/control/ScriptCommands.h) | [control/ScriptCommands.h](../../gtavc/src/control/ScriptCommands.h) | — | Declaraciones y tipos de identificadores de instrucciones u opcodes de scripts. |
| [control/ScriptDebug.cpp](../../gta3/src/control/ScriptDebug.cpp) | [control/ScriptDebug.cpp](../../gtavc/src/control/ScriptDebug.cpp) | — | Implementación de depuración de scripts. |
| [extras/shaders/simple.frag](../../gta3/src/extras/shaders/simple.frag) | [extras/shaders/simple.frag](../../gtavc/src/extras/shaders/simple.frag) | — | Shader de fragmentos: calcula el color final de los píxeles. Recurso: `simple`. |
| [extras/shaders/obj/simple_frag.inc](../../gta3/src/extras/shaders/obj/simple_frag.inc) | [extras/shaders/obj/simple_frag.inc](../../gtavc/src/extras/shaders/obj/simple_frag.inc) | — | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `simple_frag`. |
| [modelinfo/SimpleModelInfo.cpp](../../gta3/src/modelinfo/SimpleModelInfo.cpp) | [modelinfo/SimpleModelInfo.cpp](../../gtavc/src/modelinfo/SimpleModelInfo.cpp) | — | Implementación de los metadatos de modelo `SimpleModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [modelinfo/SimpleModelInfo.h](../../gta3/src/modelinfo/SimpleModelInfo.h) | [modelinfo/SimpleModelInfo.h](../../gtavc/src/modelinfo/SimpleModelInfo.h) | — | Declaraciones y tipos de los metadatos de modelo `SimpleModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [skel/skeleton.cpp](../../gta3/src/skel/skeleton.cpp) | [skel/skeleton.cpp](../../gtavc/src/skel/skeleton.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [buildings/Solid.h](../../gta3/src/buildings/Solid.h) | [buildings/Solid.h](../../gtavc/src/buildings/Solid.h) | — | Declaraciones y tipos de representación de edificios y geometría del escenario. |
| [audio/soundlist.h](../../gta3/src/audio/soundlist.h) | [audio/soundlist.h](../../gtavc/src/audio/soundlist.h) | — | Declaraciones y tipos de identificadores y datos de sonidos. |
| [extras/shaders/standardConstants.h](../../gta3/src/extras/shaders/standardConstants.h) | [extras/shaders/standardConstants.h](../../gtavc/src/extras/shaders/standardConstants.h) | — | Declaraciones y tipos del componente `standardConstants` del área extras; extensiones del proyecto, herramientas y efectos gráficos adicionales; correspondencia parcial con reSA. |
| — | [objects/Stinger.cpp](../../gtavc/src/objects/Stinger.cpp) | — | Implementación de barreras de pinchos para neumáticos. |
| — | [objects/Stinger.h](../../gtavc/src/objects/Stinger.h) | — | Declaraciones y tipos de barreras de pinchos para neumáticos. |
| [audio/oal/stream.cpp](../../gta3/src/audio/oal/stream.cpp) | [audio/oal/stream.cpp](../../gtavc/src/audio/oal/stream.cpp) | — | Implementación de `stream`: reproducción, configuración o soporte del sistema de audio. |
| [audio/oal/stream.h](../../gta3/src/audio/oal/stream.h) | [audio/oal/stream.h](../../gtavc/src/audio/oal/stream.h) | — | Declaraciones y tipos de `stream`: reproducción, configuración o soporte del sistema de audio. |
| [core/SurfaceTable.cpp](../../gta3/src/core/SurfaceTable.cpp) | [core/SurfaceTable.cpp](../../gtavc/src/core/SurfaceTable.cpp) | — | Implementación de propiedades de materiales y superficies. |
| [core/SurfaceTable.h](../../gta3/src/core/SurfaceTable.h) | [core/SurfaceTable.h](../../gtavc/src/core/SurfaceTable.h) | — | Declaraciones y tipos de propiedades de materiales y superficies. |
| [core/templates.h](../../gta3/src/core/templates.h) | [core/templates.h](../../gtavc/src/core/templates.h) | — | Declaraciones y tipos de utilidades, definiciones u operaciones compartidas. |
| [renderer/TexList.cpp](../../gta3/src/renderer/TexList.cpp) | [renderer/TexList.cpp](../../gtavc/src/renderer/TexList.cpp) | — | Implementación de lectura, listas o reservas de texturas. |
| [renderer/TexList.h](../../gta3/src/renderer/TexList.h) | [renderer/TexList.h](../../gtavc/src/renderer/TexList.h) | — | Declaraciones y tipos de lectura, listas o reservas de texturas. |
| [rw/TexRead.cpp](../../gta3/src/rw/TexRead.cpp) | [rw/TexRead.cpp](../../gtavc/src/rw/TexRead.cpp) | — | Implementación de lectura, listas o reservas de texturas. |
| [rw/TexturePools.cpp](../../gta3/src/rw/TexturePools.cpp) | [rw/TexturePools.cpp](../../gtavc/src/rw/TexturePools.cpp) | — | Implementación de lectura, listas o reservas de texturas. |
| [rw/TexturePools.h](../../gta3/src/rw/TexturePools.h) | [rw/TexturePools.h](../../gtavc/src/rw/TexturePools.h) | — | Declaraciones y tipos de lectura, listas o reservas de texturas. |
| [core/timebars.cpp](../../gta3/src/core/timebars.cpp) | [core/timebars.cpp](../../gtavc/src/core/timebars.cpp) | — | Implementación de medición y visualización de rendimiento. |
| [core/timebars.h](../../gta3/src/core/timebars.h) | [core/timebars.h](../../gtavc/src/core/timebars.h) | — | Declaraciones y tipos de medición y visualización de rendimiento. |
| [core/TimeStep.cpp](../../gta3/src/core/TimeStep.cpp) | [core/TimeStep.cpp](../../gtavc/src/core/TimeStep.cpp) | — | Implementación de tiempo transcurrido y paso de simulación. |
| [core/TimeStep.h](../../gta3/src/core/TimeStep.h) | [core/TimeStep.h](../../gtavc/src/core/TimeStep.h) | — | Declaraciones y tipos de tiempo transcurrido y paso de simulación. |
| [vehicles/Transmission.cpp](../../gta3/src/vehicles/Transmission.cpp) | [vehicles/Transmission.cpp](../../gtavc/src/vehicles/Transmission.cpp) | — | Implementación de transmisión, marchas y entrega de velocidad del vehículo. |
| [vehicles/Transmission.h](../../gta3/src/vehicles/Transmission.h) | [vehicles/Transmission.h](../../gtavc/src/vehicles/Transmission.h) | — | Declaraciones y tipos de transmisión, marchas y entrega de velocidad del vehículo. |
| [buildings/Treadable.cpp](../../gta3/src/buildings/Treadable.cpp) | [buildings/Treadable.cpp](../../gtavc/src/buildings/Treadable.cpp) | — | Implementación de representación de edificios y geometría del escenario. |
| [buildings/Treadable.h](../../gta3/src/buildings/Treadable.h) | [buildings/Treadable.h](../../gtavc/src/buildings/Treadable.h) | — | Declaraciones y tipos de representación de edificios y geometría del escenario. |
| [core/User.cpp](../../gta3/src/core/User.cpp) | [core/User.cpp](../../gtavc/src/core/User.cpp) | — | Implementación del componente `User` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| [core/User.h](../../gta3/src/core/User.h) | [core/User.h](../../gtavc/src/core/User.h) | — | Declaraciones y tipos del componente `User` del área core; sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas. |
| — | [renderer/VarConsole.cpp](../../gtavc/src/renderer/VarConsole.cpp) | — | Implementación de inspección, diagnóstico y herramientas de depuración. |
| — | [renderer/VarConsole.h](../../gtavc/src/renderer/VarConsole.h) | — | Declaraciones y tipos de inspección, diagnóstico y herramientas de depuración. |
| [collision/vu0Collision.dsm](../../gta3/src/collision/vu0Collision.dsm) | [collision/vu0Collision.dsm](../../gtavc/src/collision/vu0Collision.dsm) | — | Código o listado de ensamblador asociado al procesamiento gráfico. Recurso: `vu0Collision`. |
| [collision/vu0Collision_1.s](../../gta3/src/collision/vu0Collision_1.s) | [collision/vu0Collision_1.s](../../gtavc/src/collision/vu0Collision_1.s) | — | Rutinas escritas en lenguaje ensamblador. Recurso: `vu0Collision_1`. |
| [collision/vu0Collision_2.s](../../gta3/src/collision/vu0Collision_2.s) | [collision/vu0Collision_2.s](../../gtavc/src/collision/vu0Collision_2.s) | — | Rutinas escritas en lenguaje ensamblador. Recurso: `vu0Collision_2`. |
| [collision/VuCollision.cpp](../../gta3/src/collision/VuCollision.cpp) | [collision/VuCollision.cpp](../../gtavc/src/collision/VuCollision.cpp) | — | Implementación del componente geométrico `VuCollision` usado en detección y datos de colisiones. |
| [collision/VuCollision.h](../../gta3/src/collision/VuCollision.h) | [collision/VuCollision.h](../../gtavc/src/collision/VuCollision.h) | — | Declaraciones y tipos del componente geométrico `VuCollision` usado en detección y datos de colisiones. |
| [math/VuVector.h](../../gta3/src/math/VuVector.h) | [math/VuVector.h](../../gtavc/src/math/VuVector.h) | — | Declaraciones y tipos del componente `VuVector` del área math; vectores, matrices, cuaterniones y operaciones geométricas usadas por el motor. |
| — | [renderer/WaterCreatures.cpp](../../gtavc/src/renderer/WaterCreatures.cpp) | — | Implementación de criaturas acuáticas ambientales. |
| — | [renderer/WaterCreatures.h](../../gtavc/src/renderer/WaterCreatures.h) | — | Declaraciones y tipos de criaturas acuáticas ambientales. |
| [weapons/WeaponType.h](../../gta3/src/weapons/WeaponType.h) | [weapons/WeaponType.h](../../gtavc/src/weapons/WeaponType.h) | — | Declaraciones y tipos de armas, sus propiedades y tipos. |
| [skel/win/win.cpp](../../gta3/src/skel/win/win.cpp) | [skel/win/win.cpp](../../gtavc/src/skel/win/win.cpp) | — | Implementación de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [skel/win/win.h](../../gta3/src/skel/win/win.h) | [skel/win/win.h](../../gtavc/src/skel/win/win.h) | — | Declaraciones y tipos de arranque y conexión de la aplicación con eventos y servicios de plataforma. |
| [skel/win/win.rc](../../gta3/src/skel/win/win.rc) | [skel/win/win.rc](../../gtavc/src/skel/win/win.rc) | — | Definición de recursos de Windows, como iconos, diálogos y metadatos. Recurso: `win`. |
| [modelinfo/XtraCompsModelInfo.h](../../gta3/src/modelinfo/XtraCompsModelInfo.h) | — | — | Declaraciones y tipos de los metadatos de modelo `XtraCompsModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| [core/ZoneCull.cpp](../../gta3/src/core/ZoneCull.cpp) | [core/ZoneCull.cpp](../../gtavc/src/core/ZoneCull.cpp) | — | Implementación de zonas que alteran visibilidad y propiedades del entorno. |
| [core/ZoneCull.h](../../gta3/src/core/ZoneCull.h) | [core/ZoneCull.h](../../gtavc/src/core/ZoneCull.h) | — | Declaraciones y tipos de zonas que alteran visibilidad y propiedades del entorno. |
| [core/Zones.cpp](../../gta3/src/core/Zones.cpp) | [core/Zones.cpp](../../gtavc/src/core/Zones.cpp) | — | Implementación de zonas del mapa y sus propiedades. |
| [core/Zones.h](../../gta3/src/core/Zones.h) | [core/Zones.h](../../gtavc/src/core/Zones.h) | — | Declaraciones y tipos de zonas del mapa y sus propiedades. |

</details>

<a id="resto-sa"></a>
## Archivos de reSA sin el mismo nombre en re3/reVC

Agrupados por carpeta de reSA. Incluye reorganizaciones y sistemas con nombres distintos; no es una lista de características exclusivas de San Andreas.

<details>
<summary>./ — 341 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [3dMarker.cpp](../source/game_sa/3dMarker.cpp) | Implementación de puntos de control y marcadores del mundo. |
| — | — | [3dMarker.h](../source/game_sa/3dMarker.h) | Declaraciones y tipos de puntos de control y marcadores del mundo. |
| — | — | [3dMarkers.cpp](../source/game_sa/3dMarkers.cpp) | Implementación de puntos de control y marcadores del mundo. |
| — | — | [3dMarkers.h](../source/game_sa/3dMarkers.h) | Declaraciones y tipos de puntos de control y marcadores del mundo. |
| — | — | [AccidentManager.cpp](../source/game_sa/AccidentManager.cpp) | Implementación de registro y gestión de accidentes. |
| — | — | [AccidentManager.h](../source/game_sa/AccidentManager.h) | Declaraciones y tipos de registro y gestión de accidentes. |
| — | — | [Acquaintance.cpp](../source/game_sa/Acquaintance.cpp) | Implementación de tipos, atributos y relaciones de peatones. |
| — | — | [Acquaintance.h](../source/game_sa/Acquaintance.h) | Declaraciones y tipos de tipos, atributos y relaciones de peatones. |
| — | — | [ActiveOccluder.cpp](../source/game_sa/ActiveOccluder.cpp) | Implementación de selección de elementos visibles y ocultación por geometría. |
| — | — | [ActiveOccluder.h](../source/game_sa/ActiveOccluder.h) | Declaraciones y tipos de selección de elementos visibles y ocultación por geometría. |
| — | — | [AttractorScanner.cpp](../source/game_sa/AttractorScanner.cpp) | Implementación de `AttractorScanner`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [AttractorScanner.h](../source/game_sa/AttractorScanner.h) | Declaraciones y tipos de `AttractorScanner`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Birds.cpp](../source/game_sa/Birds.cpp) | Implementación de aves ambientales. |
| — | — | [Birds.h](../source/game_sa/Birds.h) | Declaraciones y tipos de aves ambientales. |
| — | — | [BouncingPanel.cpp](../source/game_sa/BouncingPanel.cpp) | Implementación de oscilación de paneles de vehículos mediante límites y amortiguación. |
| — | — | [BouncingPanel.h](../source/game_sa/BouncingPanel.h) | Declaraciones y tipos de oscilación de paneles de vehículos mediante límites y amortiguación. |
| — | — | [BreakManager_c.cpp](../source/game_sa/BreakManager_c.cpp) | Implementación de objetos rompibles y fragmentación. |
| — | — | [BreakManager_c.h](../source/game_sa/BreakManager_c.h) | Declaraciones y tipos de objetos rompibles y fragmentación. |
| — | — | [BreakObject_c.cpp](../source/game_sa/BreakObject_c.cpp) | Implementación de objetos rompibles y fragmentación. |
| — | — | [BreakObject_c.h](../source/game_sa/BreakObject_c.h) | Declaraciones y tipos de objetos rompibles y fragmentación. |
| — | — | [BrightLights.cpp](../source/game_sa/BrightLights.cpp) | Implementación de luces puntuales, luces visibles o haces de búsqueda. |
| — | — | [BrightLights.h](../source/game_sa/BrightLights.h) | Declaraciones y tipos de luces puntuales, luces visibles o haces de búsqueda. |
| — | — | [BulletTrace.cpp](../source/game_sa/BulletTrace.cpp) | Implementación de estelas visibles de disparos. |
| — | — | [BulletTrace.h](../source/game_sa/BulletTrace.h) | Declaraciones y tipos de estelas visibles de disparos. |
| — | — | [BulletTraces.cpp](../source/game_sa/BulletTraces.cpp) | Implementación de estelas visibles de disparos. |
| — | — | [BulletTraces.h](../source/game_sa/BulletTraces.h) | Declaraciones y tipos de estelas visibles de disparos. |
| — | — | [Buoyancy.cpp](../source/game_sa/Buoyancy.cpp) | Implementación de flotación y fuerzas asociadas al agua. |
| — | — | [Buoyancy.h](../source/game_sa/Buoyancy.h) | Declaraciones y tipos de flotación y fuerzas asociadas al agua. |
| — | — | [C_PcSave.cpp](../source/game_sa/C_PcSave.cpp) | Implementación de serialización y almacenamiento de partidas. |
| — | — | [C_PcSave.h](../source/game_sa/C_PcSave.h) | Declaraciones y tipos de serialización y almacenamiento de partidas. |
| — | — | [Cam.h](../source/game_sa/Cam.h) | Declaraciones y tipos de cámaras del juego, modos de vista y seguimiento. |
| — | — | [CamPathSplines.h](../source/game_sa/CamPathSplines.h) | Declaraciones y tipos de trayectorias interpoladas de cámara. |
| — | — | [CarEnterExit.cpp](../source/game_sa/CarEnterExit.cpp) | Implementación de entrada y salida de personajes de los vehículos. |
| — | — | [CarEnterExit.h](../source/game_sa/CarEnterExit.h) | Declaraciones y tipos de entrada y salida de personajes de los vehículos. |
| — | — | [CarGenerator.cpp](../source/game_sa/CarGenerator.cpp) | Implementación de generación de vehículos estacionados en puntos definidos. |
| — | — | [CarGenerator.h](../source/game_sa/CarGenerator.h) | Declaraciones y tipos de generación de vehículos estacionados en puntos definidos. |
| — | — | [CdStreamInfo.cpp](../source/game_sa/CdStreamInfo.cpp) | Implementación de lectura de datos usada por el sistema de streaming. |
| — | — | [CdStreamInfo.h](../source/game_sa/CdStreamInfo.h) | Declaraciones y tipos de lectura de datos usada por el sistema de streaming. |
| — | — | [cHandlingDataMgr.cpp](../source/game_sa/cHandlingDataMgr.cpp) | Implementación de propiedades que determinan el manejo de vehículos. |
| — | — | [cHandlingDataMgr.h](../source/game_sa/cHandlingDataMgr.h) | Declaraciones y tipos de propiedades que determinan el manejo de vehículos. |
| — | — | [Cheat.cpp](../source/game_sa/Cheat.cpp) | Implementación de trucos y sus efectos. |
| — | — | [Cheat.h](../source/game_sa/Cheat.h) | Declaraciones y tipos de trucos y sus efectos. |
| — | — | [Checkpoint.cpp](../source/game_sa/Checkpoint.cpp) | Implementación de puntos de control y marcadores del mundo. |
| — | — | [Checkpoint.h](../source/game_sa/Checkpoint.h) | Declaraciones y tipos de puntos de control y marcadores del mundo. |
| — | — | [Checkpoints.cpp](../source/game_sa/Checkpoints.cpp) | Implementación de puntos de control y marcadores del mundo. |
| — | — | [Checkpoints.h](../source/game_sa/Checkpoints.h) | Declaraciones y tipos de puntos de control y marcadores del mundo. |
| — | — | [Clothes.cpp](../source/game_sa/Clothes.cpp) | Implementación de ropa, apariencia y construcción visual de personajes. |
| — | — | [Clothes.h](../source/game_sa/Clothes.h) | Declaraciones y tipos de ropa, apariencia y construcción visual de personajes. |
| — | — | [ClothesBuilder.cpp](../source/game_sa/ClothesBuilder.cpp) | Implementación de ropa, apariencia y construcción visual de personajes. |
| — | — | [ClothesBuilder.h](../source/game_sa/ClothesBuilder.h) | Declaraciones y tipos de ropa, apariencia y construcción visual de personajes. |
| — | — | [common.cpp](../source/game_sa/common.cpp) | Implementación de utilidades, definiciones u operaciones compartidas. |
| — | — | [CompressedBox.cpp](../source/game_sa/CompressedBox.cpp) | Implementación de representaciones compactas de datos geométricos. |
| — | — | [CompressedBox.h](../source/game_sa/CompressedBox.h) | Declaraciones y tipos de representaciones compactas de datos geométricos. |
| — | — | [CompressedMatrixNotAligned.cpp](../source/game_sa/CompressedMatrixNotAligned.cpp) | Implementación de representaciones compactas de datos geométricos. |
| — | — | [CompressedMatrixNotAligned.h](../source/game_sa/CompressedMatrixNotAligned.h) | Declaraciones y tipos de representaciones compactas de datos geométricos. |
| — | — | [constants.h](../source/game_sa/constants.h) | Declaraciones y tipos de utilidades, definiciones u operaciones compartidas. |
| — | — | [ControllerConfigManager.cpp](../source/game_sa/ControllerConfigManager.cpp) | Implementación de asignación y configuración de controles. |
| — | — | [ControllerConfigManager.h](../source/game_sa/ControllerConfigManager.h) | Declaraciones y tipos de asignación y configuración de controles. |
| — | — | [ControllerState.cpp](../source/game_sa/ControllerState.cpp) | Implementación de lectura y estado de controles del jugador. |
| — | — | [ControllerState.h](../source/game_sa/ControllerState.h) | Declaraciones y tipos de lectura y estado de controles del jugador. |
| — | — | [Cover.cpp](../source/game_sa/Cover.cpp) | Implementación de puntos de cobertura para personajes. |
| — | — | [Cover.h](../source/game_sa/Cover.h) | Declaraciones y tipos de puntos de cobertura para personajes. |
| — | — | [CoverPoint.cpp](../source/game_sa/CoverPoint.cpp) | Implementación de puntos de cobertura para personajes. |
| — | — | [CoverPoint.h](../source/game_sa/CoverPoint.h) | Declaraciones y tipos de puntos de cobertura para personajes. |
| — | — | [CreepingFire.cpp](../source/game_sa/CreepingFire.cpp) | Implementación de fuego, focos de incendio y propagación. |
| — | — | [CreepingFire.h](../source/game_sa/CreepingFire.h) | Declaraciones y tipos de fuego, focos de incendio y propagación. |
| — | — | [Crime.cpp](../source/game_sa/Crime.cpp) | Implementación de delitos y respuesta policial o nivel de búsqueda. |
| — | — | [CrimeBeingQd.h](../source/game_sa/CrimeBeingQd.h) | Declaraciones y tipos de delitos y respuesta policial o nivel de búsqueda. |
| — | — | [cTransmission.cpp](../source/game_sa/cTransmission.cpp) | Implementación de transmisión, marchas y entrega de velocidad del vehículo. |
| — | — | [cTransmission.h](../source/game_sa/cTransmission.h) | Declaraciones y tipos de transmisión, marchas y entrega de velocidad del vehículo. |
| — | — | [CullZones.cpp](../source/game_sa/CullZones.cpp) | Implementación de zonas que alteran visibilidad y propiedades del entorno. |
| — | — | [CullZones.h](../source/game_sa/CullZones.h) | Declaraciones y tipos de zonas que alteran visibilidad y propiedades del entorno. |
| — | — | [CurrentVehicle.cpp](../source/game_sa/CurrentVehicle.cpp) | Implementación de seguimiento y presentación de información del vehículo actual. |
| — | — | [CurrentVehicle.h](../source/game_sa/CurrentVehicle.h) | Declaraciones y tipos de seguimiento y presentación de información del vehículo actual. |
| — | — | [CustomCarPlateMgr.cpp](../source/game_sa/CustomCarPlateMgr.cpp) | Implementación de generación y gestión de matrículas. |
| — | — | [CustomCarPlateMgr.h](../source/game_sa/CustomCarPlateMgr.h) | Declaraciones y tipos de generación y gestión de matrículas. |
| — | — | [CustomRoadsignMgr.cpp](../source/game_sa/CustomRoadsignMgr.cpp) | Implementación de carteles o textos dibujados en el escenario. |
| — | — | [CustomRoadsignMgr.h](../source/game_sa/CustomRoadsignMgr.h) | Declaraciones y tipos de carteles o textos dibujados en el escenario. |
| — | — | [EntityScanner.cpp](../source/game_sa/EntityScanner.cpp) | Implementación de `EntityScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [EntityScanner.h](../source/game_sa/EntityScanner.h) | Declaraciones y tipos de `EntityScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [EntryExit.cpp](../source/game_sa/EntryExit.cpp) | Implementación de transiciones entre entradas, salidas e interiores. |
| — | — | [EntryExit.h](../source/game_sa/EntryExit.h) | Declaraciones y tipos de transiciones entre entradas, salidas e interiores. |
| — | — | [EntryExitManager.cpp](../source/game_sa/EntryExitManager.cpp) | Implementación de transiciones entre entradas, salidas e interiores. |
| — | — | [EntryExitManager.h](../source/game_sa/EntryExitManager.h) | Declaraciones y tipos de transiciones entre entradas, salidas e interiores. |
| — | — | [Escalator.cpp](../source/game_sa/Escalator.cpp) | Implementación de escaleras mecánicas. |
| — | — | [Escalator.h](../source/game_sa/Escalator.h) | Declaraciones y tipos de escaleras mecánicas. |
| — | — | [Escalators.cpp](../source/game_sa/Escalators.cpp) | Implementación de escaleras mecánicas. |
| — | — | [Escalators.h](../source/game_sa/Escalators.h) | Declaraciones y tipos de escaleras mecánicas. |
| — | — | [FallingGlassPane.cpp](../source/game_sa/FallingGlassPane.cpp) | Implementación de cristales, roturas y fragmentos. |
| — | — | [FallingGlassPane.h](../source/game_sa/FallingGlassPane.h) | Declaraciones y tipos de cristales, roturas y fragmentos. |
| — | — | [FileObjectInstance.h](../source/game_sa/FileObjectInstance.h) | Declaraciones y tipos de carga de definiciones y colocación de elementos del mundo. |
| — | — | [FireManager.cpp](../source/game_sa/FireManager.cpp) | Implementación de fuego, focos de incendio y propagación. |
| — | — | [FireManager.h](../source/game_sa/FireManager.h) | Declaraciones y tipos de fuego, focos de incendio y propagación. |
| — | — | [Formation.cpp](../source/game_sa/Formation.cpp) | Implementación de formaciones de personajes. |
| — | — | [Formation.h](../source/game_sa/Formation.h) | Declaraciones y tipos de formaciones de personajes. |
| — | — | [Gamma.cpp](../source/game_sa/Gamma.cpp) | Implementación de ajuste de gamma de la imagen. |
| — | — | [Gamma.h](../source/game_sa/Gamma.h) | Declaraciones y tipos de ajuste de gamma de la imagen. |
| — | — | [GangInfo.h](../source/game_sa/GangInfo.h) | Declaraciones y tipos de bandas, relaciones y guerras territoriales. |
| — | — | [GangWars.cpp](../source/game_sa/GangWars.cpp) | Implementación de bandas, relaciones y guerras territoriales. |
| — | — | [GangWars.h](../source/game_sa/GangWars.h) | Declaraciones y tipos de bandas, relaciones y guerras territoriales. |
| — | — | [GangWarsSaveStructure.cpp](../source/game_sa/GangWarsSaveStructure.cpp) | Implementación de bandas, relaciones y guerras territoriales. |
| — | — | [GangWarsSaveStructure.h](../source/game_sa/GangWarsSaveStructure.h) | Declaraciones y tipos de bandas, relaciones y guerras territoriales. |
| — | — | [Garage.cpp](../source/game_sa/Garage.cpp) | Implementación de garajes y vehículos almacenados. |
| — | — | [Garage.h](../source/game_sa/Garage.h) | Declaraciones y tipos de garajes y vehículos almacenados. |
| — | — | [General.cpp](../source/game_sa/General.cpp) | Implementación de utilidades, definiciones u operaciones compartidas. |
| — | — | [GrassRenderer.cpp](../source/game_sa/GrassRenderer.cpp) | Implementación de vegetación, hierba y superficies donde se generan. |
| — | — | [GrassRenderer.h](../source/game_sa/GrassRenderer.h) | Declaraciones y tipos de vegetación, hierba y superficies donde se generan. |
| — | — | [GridRef.cpp](../source/game_sa/GridRef.cpp) | Implementación de referencias de cuadrícula del mapa y consulta de datos de área. |
| — | — | [GridRef.h](../source/game_sa/GridRef.h) | Declaraciones y tipos de referencias de cuadrícula del mapa y consulta de datos de área. |
| — | — | [GxtChar.cpp](../source/game_sa/GxtChar.cpp) | Implementación de textos localizados y su representación GXT. |
| — | — | [GxtChar.h](../source/game_sa/GxtChar.h) | Declaraciones y tipos de textos localizados y su representación GXT. |
| — | — | [HandShaker.cpp](../source/game_sa/HandShaker.cpp) | Implementación de generación de oscilaciones angulares y matrices de movimiento. |
| — | — | [HandShaker.h](../source/game_sa/HandShaker.h) | Declaraciones y tipos de generación de oscilaciones angulares y matrices de movimiento. |
| — | — | [HudColours.cpp](../source/game_sa/HudColours.cpp) | Implementación de información mostrada al jugador durante la partida. |
| — | — | [HudColours.h](../source/game_sa/HudColours.h) | Declaraciones y tipos de información mostrada al jugador durante la partida. |
| — | — | [IdleCam.cpp](../source/game_sa/IdleCam.cpp) | Implementación de cámaras del juego, modos de vista y seguimiento. |
| — | — | [IdleCam.h](../source/game_sa/IdleCam.h) | Declaraciones y tipos de cámaras del juego, modos de vista y seguimiento. |
| — | — | [InformFriendsEventQueue.cpp](../source/game_sa/InformFriendsEventQueue.cpp) | Implementación de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InformFriendsEventQueue.h](../source/game_sa/InformFriendsEventQueue.h) | Declaraciones y tipos de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InformGroupEvent.cpp](../source/game_sa/InformGroupEvent.cpp) | Implementación de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InformGroupEvent.h](../source/game_sa/InformGroupEvent.h) | Declaraciones y tipos de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InformGroupEventQueue.cpp](../source/game_sa/InformGroupEventQueue.cpp) | Implementación de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InformGroupEventQueue.h](../source/game_sa/InformGroupEventQueue.h) | Declaraciones y tipos de propagación de eventos a compañeros o grupos de peatones. |
| — | — | [InterestingEvents.cpp](../source/game_sa/InterestingEvents.cpp) | Implementación de registro de sucesos destacados y entidades relacionadas. |
| — | — | [InterestingEvents.h](../source/game_sa/InterestingEvents.h) | Declaraciones y tipos de registro de sucesos destacados y entidades relacionadas. |
| — | — | [IplDef.h](../source/game_sa/IplDef.h) | Declaraciones y tipos de datos IPL de colocación de entidades del mapa. |
| — | — | [IplStore.cpp](../source/game_sa/IplStore.cpp) | Implementación de datos IPL de colocación de entidades del mapa. |
| — | — | [IplStore.h](../source/game_sa/IplStore.h) | Declaraciones y tipos de datos IPL de colocación de entidades del mapa. |
| — | — | [KeyboardState.cpp](../source/game_sa/KeyboardState.cpp) | Implementación de lectura y estado de controles del jugador. |
| — | — | [KeyboardState.h](../source/game_sa/KeyboardState.h) | Declaraciones y tipos de lectura y estado de controles del jugador. |
| — | — | [LoadedCarGroup.cpp](../source/game_sa/LoadedCarGroup.cpp) | Implementación de grupos de modelos de vehículos cargados. |
| — | — | [LoadedCarGroup.h](../source/game_sa/LoadedCarGroup.h) | Declaraciones y tipos de grupos de modelos de vehículos cargados. |
| — | — | [LoadingScreen.cpp](../source/game_sa/LoadingScreen.cpp) | Implementación de pantallas y seguimiento de carga. |
| — | — | [LoadingScreen.h](../source/game_sa/LoadingScreen.h) | Declaraciones y tipos de pantallas y seguimiento de carga. |
| — | — | [LoadMonitor.cpp](../source/game_sa/LoadMonitor.cpp) | Implementación de pantallas y seguimiento de carga. |
| — | — | [LoadMonitor.h](../source/game_sa/LoadMonitor.h) | Declaraciones y tipos de pantallas y seguimiento de carga. |
| — | — | [Localisation.cpp](../source/game_sa/Localisation.cpp) | Implementación de ajustes y diferencias de localización. |
| — | — | [Localisation.h](../source/game_sa/Localisation.h) | Declaraciones y tipos de ajustes y diferencias de localización. |
| — | — | [Maths.cpp](../source/game_sa/Maths.cpp) | Implementación de utilidades, definiciones u operaciones compartidas. |
| — | — | [MentalHealth.cpp](../source/game_sa/MentalHealth.cpp) | Implementación de estado de enfado del personaje y seguimiento de cambios de salud. |
| — | — | [MentalHealth.h](../source/game_sa/MentalHealth.h) | Declaraciones y tipos de estado de enfado del personaje y seguimiento de cambios de salud. |
| — | — | [MenuManager.cpp](../source/game_sa/MenuManager.cpp) | Implementación de menús, opciones y navegación de la interfaz. |
| — | — | [MenuManager.h](../source/game_sa/MenuManager.h) | Declaraciones y tipos de menús, opciones y navegación de la interfaz. |
| — | — | [MenuSystem.cpp](../source/game_sa/MenuSystem.cpp) | Implementación de menús, opciones y navegación de la interfaz. |
| — | — | [MenuSystem.h](../source/game_sa/MenuSystem.h) | Declaraciones y tipos de menús, opciones y navegación de la interfaz. |
| — | — | [Mirrors.cpp](../source/game_sa/Mirrors.cpp) | Implementación de reflejos en espejos. |
| — | — | [Mirrors.h](../source/game_sa/Mirrors.h) | Declaraciones y tipos de reflejos en espejos. |
| — | — | [MissionCleanup.cpp](../source/game_sa/MissionCleanup.cpp) | Implementación de recursos creados por scripts y limpieza al terminar misiones. |
| — | — | [MissionCleanup.h](../source/game_sa/MissionCleanup.h) | Declaraciones y tipos de recursos creados por scripts y limpieza al terminar misiones. |
| — | — | [ModelInfoAccelerator.cpp](../source/game_sa/ModelInfoAccelerator.cpp) | Implementación de almacenamiento y reutilización de identificadores para acelerar la carga de modelos. |
| — | — | [ModelInfoAccelerator.h](../source/game_sa/ModelInfoAccelerator.h) | Declaraciones y tipos de almacenamiento y reutilización de identificadores para acelerar la carga de modelos. |
| — | — | [MotionBlurStreaks.cpp](../source/game_sa/MotionBlurStreaks.cpp) | Implementación de desenfoque de movimiento y estelas visuales. |
| — | — | [MotionBlurStreaks.h](../source/game_sa/MotionBlurStreaks.h) | Declaraciones y tipos de desenfoque de movimiento y estelas visuales. |
| — | — | [MouseControllerState.cpp](../source/game_sa/MouseControllerState.cpp) | Implementación de lectura y estado de controles del jugador. |
| — | — | [MouseControllerState.h](../source/game_sa/MouseControllerState.h) | Declaraciones y tipos de lectura y estado de controles del jugador. |
| — | — | [MovingThings.cpp](../source/game_sa/MovingThings.cpp) | Implementación de elementos ambientales móviles. |
| — | — | [MovingThings.h](../source/game_sa/MovingThings.h) | Declaraciones y tipos de elementos ambientales móviles. |
| — | — | [NodeAddress.cpp](../source/game_sa/NodeAddress.cpp) | Implementación de rutas, nodos y búsqueda de caminos. |
| — | — | [NodeAddress.h](../source/game_sa/NodeAddress.h) | Declaraciones y tipos de rutas, nodos y búsqueda de caminos. |
| — | — | [NodeRoute.cpp](../source/game_sa/NodeRoute.cpp) | Implementación de rutas, nodos y búsqueda de caminos. |
| — | — | [NodeRoute.h](../source/game_sa/NodeRoute.h) | Declaraciones y tipos de rutas, nodos y búsqueda de caminos. |
| — | — | [ObjectSaveStructure.cpp](../source/game_sa/ObjectSaveStructure.cpp) | Implementación de los datos `ObjectSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [ObjectSaveStructure.h](../source/game_sa/ObjectSaveStructure.h) | Declaraciones y tipos de los datos `ObjectSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [ObjectScanner.cpp](../source/game_sa/ObjectScanner.cpp) | Implementación de `ObjectScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [ObjectScanner.h](../source/game_sa/ObjectScanner.h) | Declaraciones y tipos de `ObjectScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [Occluder.cpp](../source/game_sa/Occluder.cpp) | Implementación de selección de elementos visibles y ocultación por geometría. |
| — | — | [Occluder.h](../source/game_sa/Occluder.h) | Declaraciones y tipos de selección de elementos visibles y ocultación por geometría. |
| — | — | [OnscreenCounterEntry.cpp](../source/game_sa/OnscreenCounterEntry.cpp) | Implementación de temporizadores y contadores visibles en pantalla. |
| — | — | [OnscreenCounterEntry.h](../source/game_sa/OnscreenCounterEntry.h) | Declaraciones y tipos de temporizadores y contadores visibles en pantalla. |
| — | — | [OnscreenTimerEntry.cpp](../source/game_sa/OnscreenTimerEntry.cpp) | Implementación de temporizadores y contadores visibles en pantalla. |
| — | — | [OnscreenTimerEntry.h](../source/game_sa/OnscreenTimerEntry.h) | Declaraciones y tipos de temporizadores y contadores visibles en pantalla. |
| — | — | [Pad_SDL.cpp](../source/game_sa/Pad_SDL.cpp) | Implementación de lectura y estado de controles del jugador. |
| — | — | [PedAttractorManager.cpp](../source/game_sa/PedAttractorManager.cpp) | Implementación de `PedAttractorManager`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [PedAttractorManager.h](../source/game_sa/PedAttractorManager.h) | Declaraciones y tipos de `PedAttractorManager`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [PedClothesDesc.cpp](../source/game_sa/PedClothesDesc.cpp) | Implementación de ropa, apariencia y construcción visual de personajes. |
| — | — | [PedClothesDesc.h](../source/game_sa/PedClothesDesc.h) | Declaraciones y tipos de ropa, apariencia y construcción visual de personajes. |
| — | — | [PedDamageResponse.cpp](../source/game_sa/PedDamageResponse.cpp) | Implementación de cálculo y representación del daño recibido por peatones. |
| — | — | [PedDamageResponse.h](../source/game_sa/PedDamageResponse.h) | Declaraciones y tipos de cálculo y representación del daño recibido por peatones. |
| — | — | [PedDamageResponseCalculator.cpp](../source/game_sa/PedDamageResponseCalculator.cpp) | Implementación de cálculo y representación del daño recibido por peatones. |
| — | — | [PedDamageResponseCalculator.h](../source/game_sa/PedDamageResponseCalculator.h) | Declaraciones y tipos de cálculo y representación del daño recibido por peatones. |
| — | — | [PedDebugVisualiser.cpp](../source/game_sa/PedDebugVisualiser.cpp) | Implementación de inspección, diagnóstico y herramientas de depuración. |
| — | — | [PedDebugVisualiser.h](../source/game_sa/PedDebugVisualiser.h) | Declaraciones y tipos de inspección, diagnóstico y herramientas de depuración. |
| — | — | [PedGeometryAnalyser.cpp](../source/game_sa/PedGeometryAnalyser.cpp) | Implementación de colocación de peatones y análisis de su entorno geométrico. |
| — | — | [PedGeometryAnalyser.h](../source/game_sa/PedGeometryAnalyser.h) | Declaraciones y tipos de colocación de peatones y análisis de su entorno geométrico. |
| — | — | [PedGroup.cpp](../source/game_sa/PedGroup.cpp) | Implementación de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroup.h](../source/game_sa/PedGroup.h) | Declaraciones y tipos de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupIntelligence.cpp](../source/game_sa/PedGroupIntelligence.cpp) | Implementación de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupIntelligence.h](../source/game_sa/PedGroupIntelligence.h) | Declaraciones y tipos de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupMembership.cpp](../source/game_sa/PedGroupMembership.cpp) | Implementación de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupMembership.h](../source/game_sa/PedGroupMembership.h) | Declaraciones y tipos de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupPlacer.cpp](../source/game_sa/PedGroupPlacer.cpp) | Implementación de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroupPlacer.h](../source/game_sa/PedGroupPlacer.h) | Declaraciones y tipos de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroups.cpp](../source/game_sa/PedGroups.cpp) | Implementación de grupos de peatones, integrantes y coordinación. |
| — | — | [PedGroups.h](../source/game_sa/PedGroups.h) | Declaraciones y tipos de grupos de peatones, integrantes y coordinación. |
| — | — | [PedIntelligence.cpp](../source/game_sa/PedIntelligence.cpp) | Implementación de coordinación de percepción, eventos y tareas del peatón. |
| — | — | [PedIntelligence.h](../source/game_sa/PedIntelligence.h) | Declaraciones y tipos de coordinación de percepción, eventos y tareas del peatón. |
| — | — | [PedList.cpp](../source/game_sa/PedList.cpp) | Implementación de listas de referencias a peatones. |
| — | — | [PedList.h](../source/game_sa/PedList.h) | Declaraciones y tipos de listas de referencias a peatones. |
| — | — | [PedScanner.cpp](../source/game_sa/PedScanner.cpp) | Implementación de `PedScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [PedScanner.h](../source/game_sa/PedScanner.h) | Declaraciones y tipos de `PedScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [PedStats.cpp](../source/game_sa/PedStats.cpp) | Implementación de tipos, atributos y relaciones de peatones. |
| — | — | [PedStats.h](../source/game_sa/PedStats.h) | Declaraciones y tipos de tipos, atributos y relaciones de peatones. |
| — | — | [PedStuckChecker.cpp](../source/game_sa/PedStuckChecker.cpp) | Implementación de detección de peatones atascados. |
| — | — | [PedStuckChecker.h](../source/game_sa/PedStuckChecker.h) | Declaraciones y tipos de detección de peatones atascados. |
| — | — | [PedTaskPair.cpp](../source/game_sa/PedTaskPair.cpp) | Implementación de asociación de un peatón con una tarea. |
| — | — | [PedTaskPair.h](../source/game_sa/PedTaskPair.h) | Declaraciones y tipos de asociación de un peatón con una tarea. |
| — | — | [Pickup.cpp](../source/game_sa/Pickup.cpp) | Implementación de objetos recogibles y sus mensajes. |
| — | — | [Pickup.h](../source/game_sa/Pickup.h) | Declaraciones y tipos de objetos recogibles y sus mensajes. |
| — | — | [PlaceName.cpp](../source/game_sa/PlaceName.cpp) | Implementación de nombre de la ubicación presentado al jugador. |
| — | — | [PlaceName.h](../source/game_sa/PlaceName.h) | Declaraciones y tipos de nombre de la ubicación presentado al jugador. |
| — | — | [PlaneBanners.cpp](../source/game_sa/PlaneBanners.cpp) | Implementación de estelas y elementos visuales asociados a aviones. |
| — | — | [PlaneBanners.h](../source/game_sa/PlaneBanners.h) | Declaraciones y tipos de estelas y elementos visuales asociados a aviones. |
| — | — | [PlaneTrail.cpp](../source/game_sa/PlaneTrail.cpp) | Implementación de estelas y elementos visuales asociados a aviones. |
| — | — | [PlaneTrail.h](../source/game_sa/PlaneTrail.h) | Declaraciones y tipos de estelas y elementos visuales asociados a aviones. |
| — | — | [PlaneTrails.cpp](../source/game_sa/PlaneTrails.cpp) | Implementación de estelas y elementos visuales asociados a aviones. |
| — | — | [PlaneTrails.h](../source/game_sa/PlaneTrails.h) | Declaraciones y tipos de estelas y elementos visuales asociados a aviones. |
| — | — | [PlantMgr.cpp](../source/game_sa/PlantMgr.cpp) | Implementación de vegetación, hierba y superficies donde se generan. |
| — | — | [PlantMgr.h](../source/game_sa/PlantMgr.h) | Declaraciones y tipos de vegetación, hierba y superficies donde se generan. |
| — | — | [PlantSurfPropMgr.cpp](../source/game_sa/PlantSurfPropMgr.cpp) | Implementación de vegetación, hierba y superficies donde se generan. |
| — | — | [PlantSurfPropMgr.h](../source/game_sa/PlantSurfPropMgr.h) | Declaraciones y tipos de vegetación, hierba y superficies donde se generan. |
| — | — | [PlayerInfoProcess.cpp.inc](../source/game_sa/PlayerInfoProcess.cpp.inc) | Fragmento incluido desde otro archivo; contiene código o datos auxiliares. Recurso: `PlayerInfoProcess.cpp`. |
| — | — | [PlayerPedData.cpp](../source/game_sa/PlayerPedData.cpp) | Implementación del personaje del jugador y sus datos de estado. |
| — | — | [PlayerPedData.h](../source/game_sa/PlayerPedData.h) | Declaraciones y tipos del personaje del jugador y sus datos de estado. |
| — | — | [PlayerRelationshipRecorder.cpp](../source/game_sa/PlayerRelationshipRecorder.cpp) | Implementación de registro de relaciones entre peatones y el jugador. |
| — | — | [PlayerRelationshipRecorder.h](../source/game_sa/PlayerRelationshipRecorder.h) | Declaraciones y tipos de registro de relaciones entre peatones y el jugador. |
| — | — | [PointList.cpp](../source/game_sa/PointList.cpp) | Implementación de rutas, nodos y búsqueda de caminos. |
| — | — | [PointList.h](../source/game_sa/PointList.h) | Declaraciones y tipos de rutas, nodos y búsqueda de caminos. |
| — | — | [PolyBunch.cpp](../source/game_sa/PolyBunch.cpp) | Implementación de grupos de vértices de polígonos y coordenadas de textura. |
| — | — | [PolyBunch.h](../source/game_sa/PolyBunch.h) | Declaraciones y tipos de grupos de vértices de polígonos y coordenadas de textura. |
| — | — | [PopCycle.cpp](../source/game_sa/PopCycle.cpp) | Implementación de creación y distribución de población y tráfico. |
| — | — | [PopCycle.h](../source/game_sa/PopCycle.h) | Declaraciones y tipos de creación y distribución de población y tráfico. |
| — | — | [PostEffects.cpp](../source/game_sa/PostEffects.cpp) | Implementación de efectos aplicados a la imagen renderizada. |
| — | — | [PostEffects.h](../source/game_sa/PostEffects.h) | Declaraciones y tipos de efectos aplicados a la imagen renderizada. |
| — | — | [PPTriPlantBuffer.cpp](../source/game_sa/PPTriPlantBuffer.cpp) | Implementación de vegetación, hierba y superficies donde se generan. |
| — | — | [PPTriPlantBuffer.h](../source/game_sa/PPTriPlantBuffer.h) | Declaraciones y tipos de vegetación, hierba y superficies donde se generan. |
| — | — | [Queue.h](../source/game_sa/Queue.h) | Declaraciones y tipos de una estructura de datos de cola. |
| — | — | [QueuedMode.h](../source/game_sa/QueuedMode.h) | Declaraciones y tipos de cámaras del juego, modos de vista y seguimiento. |
| — | — | [RealTimeShadow.cpp](../source/game_sa/RealTimeShadow.cpp) | Implementación de generación y dibujo de sombras. |
| — | — | [RealTimeShadow.h](../source/game_sa/RealTimeShadow.h) | Declaraciones y tipos de generación y dibujo de sombras. |
| — | — | [RealTimeShadowManager.cpp](../source/game_sa/RealTimeShadowManager.cpp) | Implementación de generación y dibujo de sombras. |
| — | — | [RealTimeShadowManager.h](../source/game_sa/RealTimeShadowManager.h) | Declaraciones y tipos de generación y dibujo de sombras. |
| — | — | [Reference.h](../source/game_sa/Reference.h) | Declaraciones y tipos de seguimiento de referencias a entidades. |
| — | — | [RegisteredCorona.cpp](../source/game_sa/RegisteredCorona.cpp) | Implementación de halos luminosos alrededor de luces. |
| — | — | [RegisteredCorona.h](../source/game_sa/RegisteredCorona.h) | Declaraciones y tipos de halos luminosos alrededor de luces. |
| — | — | [RegisteredMotionBlurStreak.cpp](../source/game_sa/RegisteredMotionBlurStreak.cpp) | Implementación de desenfoque de movimiento y estelas visuales. |
| — | — | [RegisteredMotionBlurStreak.h](../source/game_sa/RegisteredMotionBlurStreak.h) | Declaraciones y tipos de desenfoque de movimiento y estelas visuales. |
| — | — | [RenderBuffer.hpp](../source/game_sa/RenderBuffer.hpp) | Declaraciones y tipos de almacenamiento temporal de geometría para dibujar. |
| — | — | [RepeatSector.h](../source/game_sa/RepeatSector.h) | Declaraciones y tipos de división espacial del mundo en sectores. |
| — | — | [ReplayBlockType.cpp](../source/game_sa/ReplayBlockType.cpp) | Implementación de tipos de bloques usados por el sistema de repeticiones. |
| — | — | [ReplayBlockType.h](../source/game_sa/ReplayBlockType.h) | Declaraciones y tipos de tipos de bloques usados por el sistema de repeticiones. |
| — | — | [RGBA.h](../source/game_sa/RGBA.h) | Declaraciones y tipos de componentes de color y transparencia. |
| — | — | [RideAnimData.h](../source/game_sa/RideAnimData.h) | Declaraciones y tipos de selección y agrupación de animaciones de conducción y uso de vehículos. |
| — | — | [RideAnims.cpp](../source/game_sa/RideAnims.cpp) | Implementación de selección y agrupación de animaciones de conducción y uso de vehículos. |
| — | — | [RideAnims.h](../source/game_sa/RideAnims.h) | Declaraciones y tipos de selección y agrupación de animaciones de conducción y uso de vehículos. |
| — | — | [Rope.cpp](../source/game_sa/Rope.cpp) | Implementación de cuerdas y sus puntos de simulación. |
| — | — | [Rope.h](../source/game_sa/Rope.h) | Declaraciones y tipos de cuerdas y sus puntos de simulación. |
| — | — | [Route.hpp](../source/game_sa/Route.hpp) | Declaraciones y tipos de representación de recorridos mediante puntos. |
| — | — | [RpHAnimBlendInterpFrame.h](../source/game_sa/RpHAnimBlendInterpFrame.h) | Declaraciones y tipos de mezcla de animaciones sobre jerarquías de RenderWare. |
| — | — | [RwObjectNameIdAssocation.h](../source/game_sa/RwObjectNameIdAssocation.h) | Declaraciones y tipos de utilidades de integración con RenderWare. |
| — | — | [Scene.cpp](../source/game_sa/Scene.cpp) | Implementación de organización y consulta de entidades de la escena. |
| — | — | [Scene.h](../source/game_sa/Scene.h) | Declaraciones y tipos de organización y consulta de entidades de la escena. |
| — | — | [ScriptResourceManager.cpp](../source/game_sa/ScriptResourceManager.cpp) | Implementación de recursos creados por scripts y limpieza al terminar misiones. |
| — | — | [ScriptResourceManager.h](../source/game_sa/ScriptResourceManager.h) | Declaraciones y tipos de recursos creados por scripts y limpieza al terminar misiones. |
| — | — | [ScriptsForBrains.cpp](../source/game_sa/ScriptsForBrains.cpp) | Implementación de carga de scripts y comportamientos controlados por scripts. |
| — | — | [ScriptsForBrains.h](../source/game_sa/ScriptsForBrains.h) | Declaraciones y tipos de carga de scripts y comportamientos controlados por scripts. |
| — | — | [SearchLight.cpp](../source/game_sa/SearchLight.cpp) | Implementación de luces puntuales, luces visibles o haces de búsqueda. |
| — | — | [SearchLight.h](../source/game_sa/SearchLight.h) | Declaraciones y tipos de luces puntuales, luces visibles o haces de búsqueda. |
| — | — | [Sector.h](../source/game_sa/Sector.h) | Declaraciones y tipos de división espacial del mundo en sectores. |
| — | — | [SetPiece.cpp](../source/game_sa/SetPiece.cpp) | Implementación de situaciones o escenas predefinidas activadas en el mundo. |
| — | — | [SetPiece.h](../source/game_sa/SetPiece.h) | Declaraciones y tipos de situaciones o escenas predefinidas activadas en el mundo. |
| — | — | [ShinyTexts.cpp](../source/game_sa/ShinyTexts.cpp) | Implementación de carteles o textos dibujados en el escenario. |
| — | — | [ShinyTexts.h](../source/game_sa/ShinyTexts.h) | Declaraciones y tipos de carteles o textos dibujados en el escenario. |
| — | — | [Shopping.cpp](../source/game_sa/Shopping.cpp) | Implementación de compras y artículos disponibles en tiendas. |
| — | — | [Shopping.h](../source/game_sa/Shopping.h) | Declaraciones y tipos de compras y artículos disponibles en tiendas. |
| — | — | [SimpleTransform.cpp](../source/game_sa/SimpleTransform.cpp) | Implementación de posición y orientación de elementos del mundo. |
| — | — | [SimpleTransform.h](../source/game_sa/SimpleTransform.h) | Declaraciones y tipos de posición y orientación de elementos del mundo. |
| — | — | [SimpleVariablesSaveStructure.cpp](../source/game_sa/SimpleVariablesSaveStructure.cpp) | Implementación de los datos `SimpleVariablesSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [SimpleVariablesSaveStructure.h](../source/game_sa/SimpleVariablesSaveStructure.h) | Declaraciones y tipos de los datos `SimpleVariablesSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [Skidmark.cpp](../source/game_sa/Skidmark.cpp) | Implementación de marcas de neumáticos en el suelo. |
| — | — | [Skidmark.h](../source/game_sa/Skidmark.h) | Declaraciones y tipos de marcas de neumáticos en el suelo. |
| — | — | [SpecialPlateHandler.cpp](../source/game_sa/SpecialPlateHandler.cpp) | Implementación de generación y gestión de matrículas. |
| — | — | [SpecialPlateHandler.h](../source/game_sa/SpecialPlateHandler.h) | Declaraciones y tipos de generación y gestión de matrículas. |
| — | — | [StencilShadowObject.cpp](../source/game_sa/StencilShadowObject.cpp) | Implementación de sombras mediante búfer stencil. |
| — | — | [StencilShadowObject.h](../source/game_sa/StencilShadowObject.h) | Declaraciones y tipos de sombras mediante búfer stencil. |
| — | — | [StencilShadows.cpp](../source/game_sa/StencilShadows.cpp) | Implementación de sombras mediante búfer stencil. |
| — | — | [StencilShadows.h](../source/game_sa/StencilShadows.h) | Declaraciones y tipos de sombras mediante búfer stencil. |
| — | — | [StoredCar.h](../source/game_sa/StoredCar.h) | Declaraciones y tipos de garajes y vehículos almacenados. |
| — | — | [StoredCollPoly.h](../source/game_sa/StoredCollPoly.h) | Declaraciones y tipos de almacenamiento de polígonos de colisión. |
| — | — | [StreamedScripts.cpp](../source/game_sa/StreamedScripts.cpp) | Implementación de carga de scripts y comportamientos controlados por scripts. |
| — | — | [StreamedScripts.h](../source/game_sa/StreamedScripts.h) | Declaraciones y tipos de carga de scripts y comportamientos controlados por scripts. |
| — | — | [StreamingInfo.cpp](../source/game_sa/StreamingInfo.cpp) | Implementación de carga y descarga de recursos del mundo según demanda. |
| — | — | [StreamingInfo.h](../source/game_sa/StreamingInfo.h) | Declaraciones y tipos de carga y descarga de recursos del mundo según demanda. |
| — | — | [StuckCarCheck.cpp](../source/game_sa/StuckCarCheck.cpp) | Implementación de detección de vehículos atascados o volcados. |
| — | — | [StuckCarCheck.h](../source/game_sa/StuckCarCheck.h) | Declaraciones y tipos de detección de vehículos atascados o volcados. |
| — | — | [StuntJumpManager.cpp](../source/game_sa/StuntJumpManager.cpp) | Implementación de saltos acrobáticos especiales. |
| — | — | [StuntJumpManager.h](../source/game_sa/StuntJumpManager.h) | Declaraciones y tipos de saltos acrobáticos especiales. |
| — | — | [SurfaceInfo_c.h](../source/game_sa/SurfaceInfo_c.h) | Declaraciones y tipos de propiedades de materiales y superficies. |
| — | — | [SurfaceInfos_c.cpp](../source/game_sa/SurfaceInfos_c.cpp) | Implementación de propiedades de materiales y superficies. |
| — | — | [SurfaceInfos_c.h](../source/game_sa/SurfaceInfos_c.h) | Declaraciones y tipos de propiedades de materiales y superficies. |
| — | — | [TagManager.cpp](../source/game_sa/TagManager.cpp) | Implementación de grafitis o marcas del escenario. |
| — | — | [TagManager.h](../source/game_sa/TagManager.h) | Declaraciones y tipos de grafitis o marcas del escenario. |
| — | — | [tAudioExtensionType.h](../source/game_sa/tAudioExtensionType.h) | Declaraciones y tipos de asociación entre extensiones de archivo y tipos de audio. |
| — | — | [tBikeHandlingData.h](../source/game_sa/tBikeHandlingData.h) | Declaraciones y tipos de propiedades de manejo específicas de motos o bicicletas. |
| — | — | [tBinaryIplFile.h](../source/game_sa/tBinaryIplFile.h) | Declaraciones y tipos de datos IPL de colocación de entidades del mapa. |
| — | — | [tBoatHandlingData.h](../source/game_sa/tBoatHandlingData.h) | Declaraciones y tipos de propiedades de manejo específicas de embarcaciones. |
| — | — | [tFlyingHandlingData.h](../source/game_sa/tFlyingHandlingData.h) | Declaraciones y tipos de propiedades de manejo específicas de aeronaves. |
| — | — | [tHandlingData.h](../source/game_sa/tHandlingData.h) | Declaraciones y tipos de propiedades que determinan el manejo de vehículos. |
| — | — | [TheCarGenerators.cpp](../source/game_sa/TheCarGenerators.cpp) | Implementación de generación de vehículos estacionados en puntos definidos. |
| — | — | [TheCarGenerators.h](../source/game_sa/TheCarGenerators.h) | Declaraciones y tipos de generación de vehículos estacionados en puntos definidos. |
| — | — | [TheZones.cpp](../source/game_sa/TheZones.cpp) | Implementación de zonas del mapa y sus propiedades. |
| — | — | [TheZones.h](../source/game_sa/TheZones.h) | Declaraciones y tipos de zonas del mapa y sus propiedades. |
| — | — | [tPickupMessage.h](../source/game_sa/tPickupMessage.h) | Declaraciones y tipos de objetos recogibles y sus mensajes. |
| — | — | [TrainNode.cpp](../source/game_sa/TrainNode.cpp) | Implementación de trenes y sus recorridos. |
| — | — | [TrainNode.h](../source/game_sa/TrainNode.h) | Declaraciones y tipos de trenes y sus recorridos. |
| — | — | [tTransmissionGear.h](../source/game_sa/tTransmissionGear.h) | Declaraciones y tipos de transmisión, marchas y entrega de velocidad del vehículo. |
| — | — | [tUserTracksInfo.h](../source/game_sa/tUserTracksInfo.h) | Declaraciones y tipos de información sobre pistas de música del usuario. |
| — | — | [TxdDef.cpp](../source/game_sa/TxdDef.cpp) | Implementación de diccionarios de texturas y su gestión. |
| — | — | [TxdDef.h](../source/game_sa/TxdDef.h) | Declaraciones y tipos de diccionarios de texturas y su gestión. |
| — | — | [TxdStore_Plugin.cpp](../source/game_sa/TxdStore_Plugin.cpp) | Implementación de diccionarios de texturas y su gestión. |
| — | — | [UpsideDownCarCheck.cpp](../source/game_sa/UpsideDownCarCheck.cpp) | Implementación de detección de vehículos atascados o volcados. |
| — | — | [UpsideDownCarCheck.h](../source/game_sa/UpsideDownCarCheck.h) | Declaraciones y tipos de detección de vehículos atascados o volcados. |
| — | — | [UserDisplay.cpp](../source/game_sa/UserDisplay.cpp) | Implementación de información mostrada al jugador durante la partida. |
| — | — | [UserDisplay.h](../source/game_sa/UserDisplay.h) | Declaraciones y tipos de información mostrada al jugador durante la partida. |
| — | — | [VehicleAnimGroupData.cpp](../source/game_sa/VehicleAnimGroupData.cpp) | Implementación de selección y agrupación de animaciones de conducción y uso de vehículos. |
| — | — | [VehicleAnimGroupData.h](../source/game_sa/VehicleAnimGroupData.h) | Declaraciones y tipos de selección y agrupación de animaciones de conducción y uso de vehículos. |
| — | — | [VehicleRecording.cpp](../source/game_sa/VehicleRecording.cpp) | Implementación de grabación y reproducción de recorridos de vehículos. |
| — | — | [VehicleRecording.h](../source/game_sa/VehicleRecording.h) | Declaraciones y tipos de grabación y reproducción de recorridos de vehículos. |
| — | — | [VehicleSaveStructure.cpp](../source/game_sa/VehicleSaveStructure.cpp) | Implementación de los datos `VehicleSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [VehicleSaveStructure.h](../source/game_sa/VehicleSaveStructure.h) | Declaraciones y tipos de los datos `VehicleSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [VehicleScanner.cpp](../source/game_sa/VehicleScanner.cpp) | Implementación de `VehicleScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [VehicleScanner.h](../source/game_sa/VehicleScanner.h) | Declaraciones y tipos de `VehicleScanner`: exploración de entidades cercanas para alimentar la percepción de la IA. |
| — | — | [WaterCannons.cpp](../source/game_sa/WaterCannons.cpp) | Implementación de chorros de cañones de agua. |
| — | — | [WaterCannons.h](../source/game_sa/WaterCannons.h) | Declaraciones y tipos de chorros de cañones de agua. |
| — | — | [WaterCreature_c.cpp](../source/game_sa/WaterCreature_c.cpp) | Implementación de criaturas acuáticas ambientales. |
| — | — | [WaterCreature_c.h](../source/game_sa/WaterCreature_c.h) | Declaraciones y tipos de criaturas acuáticas ambientales. |
| — | — | [WaterCreatureManager_c.cpp](../source/game_sa/WaterCreatureManager_c.cpp) | Implementación de criaturas acuáticas ambientales. |
| — | — | [WaterCreatureManager_c.h](../source/game_sa/WaterCreatureManager_c.h) | Declaraciones y tipos de criaturas acuáticas ambientales. |
| — | — | [World_Helper.cpp](../source/game_sa/World_Helper.cpp) | Implementación de organización y consulta de entidades de la escena. |
| — | — | [Zone.cpp](../source/game_sa/Zone.cpp) | Implementación de zonas del mapa y sus propiedades. |
| — | — | [Zone.h](../source/game_sa/Zone.h) | Declaraciones y tipos de zonas del mapa y sus propiedades. |
| — | — | [ZoneInfo.h](../source/game_sa/ZoneInfo.h) | Declaraciones y tipos de zonas del mapa y sus propiedades. |

</details>

<details>
<summary>Animation/ — 11 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Animation/AnimAssocAnimations.h](../source/game_sa/Animation/AnimAssocAnimations.h) | Declaraciones y tipos del componente `AnimAssocAnimations` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimAssocDefinitions.cpp](../source/game_sa/Animation/AnimAssocDefinitions.cpp) | Implementación del componente `AnimAssocDefinitions` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimAssocDescriptions.h](../source/game_sa/Animation/AnimAssocDescriptions.h) | Declaraciones y tipos del componente `AnimAssocDescriptions` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimAssociationData.h](../source/game_sa/Animation/AnimAssociationData.h) | Declaraciones y tipos del componente `AnimAssociationData` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimationStyleDescriptor.h](../source/game_sa/Animation/AnimationStyleDescriptor.h) | Declaraciones y tipos del componente `AnimationStyleDescriptor` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimBlendFrameData.h](../source/game_sa/Animation/AnimBlendFrameData.h) | Declaraciones y tipos del componente `AnimBlendFrameData` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimBlendStaticAssociation.cpp](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) | Implementación del componente `AnimBlendStaticAssociation` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimBlendStaticAssociation.h](../source/game_sa/Animation/AnimBlendStaticAssociation.h) | Declaraciones y tipos del componente `AnimBlendStaticAssociation` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimBlock.h](../source/game_sa/Animation/AnimBlock.h) | Declaraciones y tipos del componente `AnimBlock` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/AnimSequenceFrames.h](../source/game_sa/Animation/AnimSequenceFrames.h) | Declaraciones y tipos del componente `AnimSequenceFrames` de organización, datos o mezcla de animaciones. |
| — | — | [Animation/README.md](../source/game_sa/Animation/README.md) | Documentación del subsistema donde se encuentra. Recurso: `README`. |

</details>

<details>
<summary>Attractors/ — 13 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Attractors/PedAtmAttractor.h](../source/game_sa/Attractors/PedAtmAttractor.h) | Declaraciones y tipos de `PedAtmAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedAttractorPedPlacer.cpp](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) | Implementación de `PedAttractorPedPlacer`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedAttractorPedPlacer.h](../source/game_sa/Attractors/PedAttractorPedPlacer.h) | Declaraciones y tipos de `PedAttractorPedPlacer`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedLookAtAttractor.h](../source/game_sa/Attractors/PedLookAtAttractor.h) | Declaraciones y tipos de `PedLookAtAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedParkAttractor.h](../source/game_sa/Attractors/PedParkAttractor.h) | Declaraciones y tipos de `PedParkAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedPizzaAttractor.h](../source/game_sa/Attractors/PedPizzaAttractor.h) | Declaraciones y tipos de `PedPizzaAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedScriptedAttractor.h](../source/game_sa/Attractors/PedScriptedAttractor.h) | Declaraciones y tipos de `PedScriptedAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedSeatAttractor.h](../source/game_sa/Attractors/PedSeatAttractor.h) | Declaraciones y tipos de `PedSeatAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedShelterAttractor.cpp](../source/game_sa/Attractors/PedShelterAttractor.cpp) | Implementación de `PedShelterAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedShelterAttractor.h](../source/game_sa/Attractors/PedShelterAttractor.h) | Declaraciones y tipos de `PedShelterAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedStepAttractor.h](../source/game_sa/Attractors/PedStepAttractor.h) | Declaraciones y tipos de `PedStepAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedStopAttractor.h](../source/game_sa/Attractors/PedStopAttractor.h) | Declaraciones y tipos de `PedStopAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |
| — | — | [Attractors/PedTriggerScriptAttractor.h](../source/game_sa/Attractors/PedTriggerScriptAttractor.h) | Declaraciones y tipos de `PedTriggerScriptAttractor`: puntos del entorno que atraen peatones y organizan su interacción. |

</details>

<details>
<summary>Audio/ — 14 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/AEAudioEnvironment.cpp](../source/game_sa/Audio/AEAudioEnvironment.cpp) | Implementación de `AEAudioEnvironment`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AEAudioEnvironment.h](../source/game_sa/Audio/AEAudioEnvironment.h) | Declaraciones y tipos de `AEAudioEnvironment`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AEAudioUtility.cpp](../source/game_sa/Audio/AEAudioUtility.cpp) | Implementación de `AEAudioUtility`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AEAudioUtility.h](../source/game_sa/Audio/AEAudioUtility.h) | Declaraciones y tipos de `AEAudioUtility`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AESmoothFadeThread.cpp](../source/game_sa/Audio/AESmoothFadeThread.cpp) | Implementación de `AESmoothFadeThread`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AESmoothFadeThread.h](../source/game_sa/Audio/AESmoothFadeThread.h) | Declaraciones y tipos de `AESmoothFadeThread`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AESound.cpp](../source/game_sa/Audio/AESound.cpp) | Implementación de `AESound`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AESound.h](../source/game_sa/Audio/AESound.h) | Declaraciones y tipos de `AESound`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AEStreamThread.cpp](../source/game_sa/Audio/AEStreamThread.cpp) | Implementación de `AEStreamThread`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AEStreamThread.h](../source/game_sa/Audio/AEStreamThread.h) | Declaraciones y tipos de `AEStreamThread`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AudioEngine.cpp](../source/game_sa/Audio/AudioEngine.cpp) | Implementación de interfaz principal del sistema de sonido. |
| — | — | [Audio/AudioEngine.h](../source/game_sa/Audio/AudioEngine.h) | Declaraciones y tipos de interfaz principal del sistema de sonido. |
| — | — | [Audio/AudioZones.cpp](../source/game_sa/Audio/AudioZones.cpp) | Implementación de `AudioZones`: reproducción, configuración o soporte del sistema de audio. |
| — | — | [Audio/AudioZones.h](../source/game_sa/Audio/AudioZones.h) | Declaraciones y tipos de `AudioZones`: reproducción, configuración o soporte del sistema de audio. |

</details>

<details>
<summary>Audio/Config/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Config/RadioStreamsPC.h](../source/game_sa/Audio/Config/RadioStreamsPC.h) | Declaraciones y tipos de `RadioStreamsPC`: configuración de pistas de audio. |

</details>

<details>
<summary>Audio/Data/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Data/SoundAttenuationTable.h](../source/game_sa/Audio/Data/SoundAttenuationTable.h) | Declaraciones y tipos de `SoundAttenuationTable`: tablas de datos del motor de sonido. |

</details>

<details>
<summary>Audio/Entities/ — 39 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Entities/AEAudioEntity.cpp](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) | Implementación de `AEAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEAudioEntity.h](../source/game_sa/Audio/Entities/AEAudioEntity.h) | Declaraciones y tipos de `AEAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AECollisionAudioEntity.CollisionLookup.h](../source/game_sa/Audio/Entities/AECollisionAudioEntity.CollisionLookup.h) | Declaraciones y tipos de `AECollisionAudioEntity.CollisionLookup`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AECollisionAudioEntity.cpp](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) | Implementación de `AECollisionAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AECollisionAudioEntity.h](../source/game_sa/Audio/Entities/AECollisionAudioEntity.h) | Declaraciones y tipos de `AECollisionAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEDoorAudioEntity.cpp](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) | Implementación de `AEDoorAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEDoorAudioEntity.h](../source/game_sa/Audio/Entities/AEDoorAudioEntity.h) | Declaraciones y tipos de `AEDoorAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEExplosionAudioEntity.cpp](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) | Implementación de `AEExplosionAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEExplosionAudioEntity.h](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.h) | Declaraciones y tipos de `AEExplosionAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEFireAudioEntity.cpp](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) | Implementación de `AEFireAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEFireAudioEntity.h](../source/game_sa/Audio/Entities/AEFireAudioEntity.h) | Declaraciones y tipos de `AEFireAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEFrontendAudioEntity.cpp](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) | Implementación de `AEFrontendAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEFrontendAudioEntity.h](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.h) | Declaraciones y tipos de `AEFrontendAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEGlobalWeaponAudioEntity.cpp](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) | Implementación de `AEGlobalWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEGlobalWeaponAudioEntity.h](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.h) | Declaraciones y tipos de `AEGlobalWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) | Implementación de `AEPedAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedAudioEntity.h](../source/game_sa/Audio/Entities/AEPedAudioEntity.h) | Declaraciones y tipos de `AEPedAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedlessSpeechAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) | Implementación de `AEPedlessSpeechAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedlessSpeechAudioEntity.h](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.h) | Declaraciones y tipos de `AEPedlessSpeechAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedSpeechAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) | Implementación de `AEPedSpeechAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedSpeechAudioEntity.h](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h) | Declaraciones y tipos de `AEPedSpeechAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedWeaponAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) | Implementación de `AEPedWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPedWeaponAudioEntity.h](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.h) | Declaraciones y tipos de `AEPedWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPoliceScannerAudioEntity.cpp](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) | Implementación de `AEPoliceScannerAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEPoliceScannerAudioEntity.h](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h) | Declaraciones y tipos de `AEPoliceScannerAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEScriptAudioEntity.cpp](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) | Implementación de `AEScriptAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEScriptAudioEntity.h](../source/game_sa/Audio/Entities/AEScriptAudioEntity.h) | Declaraciones y tipos de `AEScriptAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AETwinLoopSoundEntity.cpp](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) | Implementación de `AETwinLoopSoundEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AETwinLoopSoundEntity.h](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.h) | Declaraciones y tipos de `AETwinLoopSoundEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEVehicleAudioEntity.cpp](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) | Implementación de `AEVehicleAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEVehicleAudioEntity.h](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h) | Declaraciones y tipos de `AEVehicleAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h) | Declaraciones y tipos de `AEVehicleAudioEntity.VehicleAudioSettings`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWaterCannonAudioEntity.cpp](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) | Implementación de `AEWaterCannonAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWaterCannonAudioEntity.h](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.h) | Declaraciones y tipos de `AEWaterCannonAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWeaponAudioEntity.cpp](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) | Implementación de `AEWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWeaponAudioEntity.h](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.h) | Declaraciones y tipos de `AEWeaponAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWeatherAudioEntity.cpp](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) | Implementación de `AEWeatherAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/AEWeatherAudioEntity.h](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.h) | Declaraciones y tipos de `AEWeatherAudioEntity`: sonidos asociados a entidades o situaciones del juego. |
| — | — | [Audio/Entities/PedSpeechAudioEntityLUTs.inc.h](../source/game_sa/Audio/Entities/PedSpeechAudioEntityLUTs.inc.h) | Declaraciones y tipos de `PedSpeechAudioEntityLUTs.inc`: sonidos asociados a entidades o situaciones del juego. |

</details>

<details>
<summary>Audio/Enums/ — 14 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Enums/BankSlotSoundIDs.h](../source/game_sa/Audio/Enums/BankSlotSoundIDs.h) | Declaraciones y tipos de `BankSlotSoundIDs`: identificadores de sonidos, voces o bancos. |
| — | — | [Audio/Enums/eAERadioType.h](../source/game_sa/Audio/Enums/eAERadioType.h) | Declaraciones y tipos de la enumeración `eAERadioType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eAEVehicleAudioType.h](../source/game_sa/Audio/Enums/eAEVehicleAudioType.h) | Declaraciones y tipos de la enumeración `eAEVehicleAudioType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eAEVehicleAudioTypeForName.h](../source/game_sa/Audio/Enums/eAEVehicleAudioTypeForName.h) | Declaraciones y tipos de la enumeración `eAEVehicleAudioTypeForName`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eAEVehicleDoorType.h](../source/game_sa/Audio/Enums/eAEVehicleDoorType.h) | Declaraciones y tipos de la enumeración `eAEVehicleDoorType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eAEVehicleHornType.h](../source/game_sa/Audio/Enums/eAEVehicleHornType.h) | Declaraciones y tipos de la enumeración `eAEVehicleHornType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eAEVehicleSoundType.h](../source/game_sa/Audio/Enums/eAEVehicleSoundType.h) | Declaraciones y tipos de la enumeración `eAEVehicleSoundType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eBassSetting.h](../source/game_sa/Audio/Enums/eBassSetting.h) | Declaraciones y tipos de la enumeración `eBassSetting`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/ePedAudioType.h](../source/game_sa/Audio/Enums/ePedAudioType.h) | Declaraciones y tipos de la enumeración `ePedAudioType`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eSoundBank.h](../source/game_sa/Audio/Enums/eSoundBank.h) | Declaraciones y tipos de la enumeración `eSoundBank`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/eSoundBankSlot.h](../source/game_sa/Audio/Enums/eSoundBankSlot.h) | Declaraciones y tipos de la enumeración `eSoundBankSlot`, que define identificadores o estados usados por el juego. |
| — | — | [Audio/Enums/PedSpeechContexts.h](../source/game_sa/Audio/Enums/PedSpeechContexts.h) | Declaraciones y tipos de `PedSpeechContexts`: identificadores de sonidos, voces o bancos. |
| — | — | [Audio/Enums/PedSpeechVoices.h](../source/game_sa/Audio/Enums/PedSpeechVoices.h) | Declaraciones y tipos de `PedSpeechVoices`: identificadores de sonidos, voces o bancos. |
| — | — | [Audio/Enums/SoundIDs.h](../source/game_sa/Audio/Enums/SoundIDs.h) | Declaraciones y tipos de `SoundIDs`: identificadores de sonidos, voces o bancos. |

</details>

<details>
<summary>Audio/Hardware/ — 8 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Hardware/AEAudioChannel.cpp](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) | Implementación de `AEAudioChannel`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEAudioChannel.h](../source/game_sa/Audio/Hardware/AEAudioChannel.h) | Declaraciones y tipos de `AEAudioChannel`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEAudioHardware.cpp](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) | Implementación de `AEAudioHardware`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEAudioHardware.h](../source/game_sa/Audio/Hardware/AEAudioHardware.h) | Declaraciones y tipos de `AEAudioHardware`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEStaticChannel.cpp](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) | Implementación de `AEStaticChannel`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEStaticChannel.h](../source/game_sa/Audio/Hardware/AEStaticChannel.h) | Declaraciones y tipos de `AEStaticChannel`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEStreamingChannel.cpp](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) | Implementación de `AEStreamingChannel`: canales y dispositivo de reproducción. |
| — | — | [Audio/Hardware/AEStreamingChannel.h](../source/game_sa/Audio/Hardware/AEStreamingChannel.h) | Declaraciones y tipos de `AEStreamingChannel`: canales y dispositivo de reproducción. |

</details>

<details>
<summary>Audio/Loaders/ — 22 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Loaders/AEBankLoader.cpp](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) | Implementación de `AEBankLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEBankLoader.h](../source/game_sa/Audio/Loaders/AEBankLoader.h) | Declaraciones y tipos de `AEBankLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEDataStream.cpp](../source/game_sa/Audio/Loaders/AEDataStream.cpp) | Implementación de `AEDataStream`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEDataStream.h](../source/game_sa/Audio/Loaders/AEDataStream.h) | Declaraciones y tipos de `AEDataStream`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMFDecoder.cpp](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) | Implementación de `AEMFDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMFDecoder.h](../source/game_sa/Audio/Loaders/AEMFDecoder.h) | Declaraciones y tipos de `AEMFDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMP3BankLoader.cpp](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) | Implementación de `AEMP3BankLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMP3BankLoader.h](../source/game_sa/Audio/Loaders/AEMP3BankLoader.h) | Declaraciones y tipos de `AEMP3BankLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMP3TrackLoader.cpp](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) | Implementación de `AEMP3TrackLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEMP3TrackLoader.h](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.h) | Declaraciones y tipos de `AEMP3TrackLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEStreamingDecoder.cpp](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) | Implementación de `AEStreamingDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEStreamingDecoder.h](../source/game_sa/Audio/Loaders/AEStreamingDecoder.h) | Declaraciones y tipos de `AEStreamingDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEStreamTransformer.cpp](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) | Implementación de `AEStreamTransformer`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEStreamTransformer.h](../source/game_sa/Audio/Loaders/AEStreamTransformer.h) | Declaraciones y tipos de `AEStreamTransformer`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AETrackLoader.cpp](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) | Implementación de `AETrackLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AETrackLoader.h](../source/game_sa/Audio/Loaders/AETrackLoader.h) | Declaraciones y tipos de `AETrackLoader`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEVorbisDecoder.cpp](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) | Implementación de `AEVorbisDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEVorbisDecoder.h](../source/game_sa/Audio/Loaders/AEVorbisDecoder.h) | Declaraciones y tipos de `AEVorbisDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEWaveDecoder.cpp](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) | Implementación de `AEWaveDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEWaveDecoder.h](../source/game_sa/Audio/Loaders/AEWaveDecoder.h) | Declaraciones y tipos de `AEWaveDecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEWMADecoder.cpp](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) | Implementación de `AEWMADecoder`: carga o decodificación de datos de audio. |
| — | — | [Audio/Loaders/AEWMADecoder.h](../source/game_sa/Audio/Loaders/AEWMADecoder.h) | Declaraciones y tipos de `AEWMADecoder`: carga o decodificación de datos de audio. |

</details>

<details>
<summary>Audio/Managers/ — 11 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/Managers/AEAmbienceTrackManager.cpp](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) | Implementación de `AEAmbienceTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AEAmbienceTrackManager.h](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.h) | Declaraciones y tipos de `AEAmbienceTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AECutsceneTrackManager.cpp](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) | Implementación de `AECutsceneTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AECutsceneTrackManager.h](../source/game_sa/Audio/Managers/AECutsceneTrackManager.h) | Declaraciones y tipos de `AECutsceneTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AERadioTrackManager.cpp](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) | Implementación de `AERadioTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AERadioTrackManager.h](../source/game_sa/Audio/Managers/AERadioTrackManager.h) | Declaraciones y tipos de `AERadioTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AESoundManager.cpp](../source/game_sa/Audio/Managers/AESoundManager.cpp) | Implementación de `AESoundManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AESoundManager.h](../source/game_sa/Audio/Managers/AESoundManager.h) | Declaraciones y tipos de `AESoundManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AEUserRadioTrackManager.cpp](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) | Implementación de `AEUserRadioTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/AEUserRadioTrackManager.h](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.h) | Declaraciones y tipos de `AEUserRadioTrackManager`: coordinación de sonidos, música o pistas. |
| — | — | [Audio/Managers/CutSceneStreamsPC.h](../source/game_sa/Audio/Managers/CutSceneStreamsPC.h) | Declaraciones y tipos de `CutSceneStreamsPC`: coordinación de sonidos, música o pistas. |

</details>

<details>
<summary>Audio/OpenAL/ — 6 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Audio/OpenAL/OALBase.cpp](../source/game_sa/Audio/OpenAL/OALBase.cpp) | Implementación de `OALBase`: integración del audio con OpenAL. |
| — | — | [Audio/OpenAL/OALBase.h](../source/game_sa/Audio/OpenAL/OALBase.h) | Declaraciones y tipos de `OALBase`: integración del audio con OpenAL. |
| — | — | [Audio/OpenAL/OALBuffer.cpp](../source/game_sa/Audio/OpenAL/OALBuffer.cpp) | Implementación de `OALBuffer`: integración del audio con OpenAL. |
| — | — | [Audio/OpenAL/OALBuffer.h](../source/game_sa/Audio/OpenAL/OALBuffer.h) | Declaraciones y tipos de `OALBuffer`: integración del audio con OpenAL. |
| — | — | [Audio/OpenAL/OALSource.cpp](../source/game_sa/Audio/OpenAL/OALSource.cpp) | Implementación de `OALSource`: integración del audio con OpenAL. |
| — | — | [Audio/OpenAL/OALSource.h](../source/game_sa/Audio/OpenAL/OALSource.h) | Declaraciones y tipos de `OALSource`: integración del audio con OpenAL. |

</details>

<details>
<summary>Collision/ — 20 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Collision/BoundingBox.cpp](../source/game_sa/Collision/BoundingBox.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| — | — | [Collision/BoundingBox.h](../source/game_sa/Collision/BoundingBox.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| — | — | [Collision/Box.cpp](../source/game_sa/Collision/Box.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |
| — | — | [Collision/Box.h](../source/game_sa/Collision/Box.h) | Declaraciones y tipos de tipos y operaciones matemáticas o geométricas. |
| — | — | [Collision/ColAccel.cpp](../source/game_sa/Collision/ColAccel.cpp) | Implementación del componente geométrico `ColAccel` usado en detección y datos de colisiones. |
| — | — | [Collision/ColAccel.h](../source/game_sa/Collision/ColAccel.h) | Declaraciones y tipos del componente geométrico `ColAccel` usado en detección y datos de colisiones. |
| — | — | [Collision/ColDisk.cpp](../source/game_sa/Collision/ColDisk.cpp) | Implementación del componente geométrico `ColDisk` usado en detección y datos de colisiones. |
| — | — | [Collision/ColDisk.h](../source/game_sa/Collision/ColDisk.h) | Declaraciones y tipos del componente geométrico `ColDisk` usado en detección y datos de colisiones. |
| — | — | [Collision/ColHelpers.h](../source/game_sa/Collision/ColHelpers.h) | Declaraciones y tipos del componente geométrico `ColHelpers` usado en detección y datos de colisiones. |
| — | — | [Collision/CollisionData.cpp](../source/game_sa/Collision/CollisionData.cpp) | Implementación del componente geométrico `CollisionData` usado en detección y datos de colisiones. |
| — | — | [Collision/CollisionData.h](../source/game_sa/Collision/CollisionData.h) | Declaraciones y tipos del componente geométrico `CollisionData` usado en detección y datos de colisiones. |
| — | — | [Collision/CollisionEventScanner.cpp](../source/game_sa/Collision/CollisionEventScanner.cpp) | Implementación del componente geométrico `CollisionEventScanner` usado en detección y datos de colisiones. |
| — | — | [Collision/CollisionEventScanner.h](../source/game_sa/Collision/CollisionEventScanner.h) | Declaraciones y tipos del componente geométrico `CollisionEventScanner` usado en detección y datos de colisiones. |
| — | — | [Collision/ColourSet.cpp](../source/game_sa/Collision/ColourSet.cpp) | Implementación del componente geométrico `ColourSet` usado en detección y datos de colisiones. |
| — | — | [Collision/ColourSet.h](../source/game_sa/Collision/ColourSet.h) | Declaraciones y tipos del componente geométrico `ColourSet` usado en detección y datos de colisiones. |
| — | — | [Collision/ColSurface.h](../source/game_sa/Collision/ColSurface.h) | Declaraciones y tipos del componente geométrico `ColSurface` usado en detección y datos de colisiones. |
| — | — | [Collision/ColTrianglePlane.cpp](../source/game_sa/Collision/ColTrianglePlane.cpp) | Implementación del componente geométrico `ColTrianglePlane` usado en detección y datos de colisiones. |
| — | — | [Collision/ColTrianglePlane.h](../source/game_sa/Collision/ColTrianglePlane.h) | Declaraciones y tipos del componente geométrico `ColTrianglePlane` usado en detección y datos de colisiones. |
| — | — | [Collision/Sphere.cpp](../source/game_sa/Collision/Sphere.cpp) | Implementación del componente geométrico `Sphere` usado en detección y datos de colisiones. |
| — | — | [Collision/Sphere.h](../source/game_sa/Collision/Sphere.h) | Declaraciones y tipos del componente geométrico `Sphere` usado en detección y datos de colisiones. |

</details>

<details>
<summary>Conversations/ — 8 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Conversations/ConversationForPed.cpp](../source/game_sa/Conversations/ConversationForPed.cpp) | Implementación de `ConversationForPed`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/ConversationForPed.h](../source/game_sa/Conversations/ConversationForPed.h) | Declaraciones y tipos de `ConversationForPed`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/ConversationNode.cpp](../source/game_sa/Conversations/ConversationNode.cpp) | Implementación de `ConversationNode`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/ConversationNode.h](../source/game_sa/Conversations/ConversationNode.h) | Declaraciones y tipos de `ConversationNode`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/Conversations.cpp](../source/game_sa/Conversations/Conversations.cpp) | Implementación de `Conversations`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/Conversations.h](../source/game_sa/Conversations/Conversations.h) | Declaraciones y tipos de `Conversations`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/PedToPlayerConversations.cpp](../source/game_sa/Conversations/PedToPlayerConversations.cpp) | Implementación de `PedToPlayerConversations`: participantes, nodos o gestión de conversaciones. |
| — | — | [Conversations/PedToPlayerConversations.h](../source/game_sa/Conversations/PedToPlayerConversations.h) | Declaraciones y tipos de `PedToPlayerConversations`: participantes, nodos o gestión de conversaciones. |

</details>

<details>
<summary>Core/ — 32 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Core/EntryInfoList.cpp](../source/game_sa/Core/EntryInfoList.cpp) | Implementación de `EntryInfoList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/EntryInfoList.h](../source/game_sa/Core/EntryInfoList.h) | Declaraciones y tipos de `EntryInfoList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/EntryInfoNode.cpp](../source/game_sa/Core/EntryInfoNode.cpp) | Implementación de `EntryInfoNode`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/EntryInfoNode.h](../source/game_sa/Core/EntryInfoNode.h) | Declaraciones y tipos de `EntryInfoNode`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/KeyGen.cpp](../source/game_sa/Core/KeyGen.cpp) | Implementación de `KeyGen`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/KeyGen.h](../source/game_sa/Core/KeyGen.h) | Declaraciones y tipos de `KeyGen`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/Link.h](../source/game_sa/Core/Link.h) | Declaraciones y tipos de `Link`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/LinkList.h](../source/game_sa/Core/LinkList.h) | Declaraciones y tipos de `LinkList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/List_c.h](../source/game_sa/Core/List_c.h) | Declaraciones y tipos de `List_c`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/ListItem_c.cpp](../source/game_sa/Core/ListItem_c.cpp) | Implementación de `ListItem_c`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/ListItem_c.h](../source/game_sa/Core/ListItem_c.h) | Declaraciones y tipos de `ListItem_c`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/MatrixLink.cpp](../source/game_sa/Core/MatrixLink.cpp) | Implementación de `MatrixLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/MatrixLink.h](../source/game_sa/Core/MatrixLink.h) | Declaraciones y tipos de `MatrixLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/MatrixLinkList.cpp](../source/game_sa/Core/MatrixLinkList.cpp) | Implementación de `MatrixLinkList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/MatrixLinkList.h](../source/game_sa/Core/MatrixLinkList.h) | Declaraciones y tipos de `MatrixLinkList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/OctTree.cpp](../source/game_sa/Core/OctTree.cpp) | Implementación de `OctTree`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/OctTree.h](../source/game_sa/Core/OctTree.h) | Declaraciones y tipos de `OctTree`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/OctTreeBase.cpp](../source/game_sa/Core/OctTreeBase.cpp) | Implementación de `OctTreeBase`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/OctTreeBase.h](../source/game_sa/Core/OctTreeBase.h) | Declaraciones y tipos de `OctTreeBase`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/Pool.h](../source/game_sa/Core/Pool.h) | Declaraciones y tipos de la reserva `Pool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Core/PtrList.h](../source/game_sa/Core/PtrList.h) | Declaraciones y tipos de `PtrList`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrListDoubleLink.h](../source/game_sa/Core/PtrListDoubleLink.h) | Declaraciones y tipos de `PtrListDoubleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrListSingleLink.h](../source/game_sa/Core/PtrListSingleLink.h) | Declaraciones y tipos de `PtrListSingleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrNode.h](../source/game_sa/Core/PtrNode.h) | Declaraciones y tipos de `PtrNode`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrNodeDoubleLink.cpp](../source/game_sa/Core/PtrNodeDoubleLink.cpp) | Implementación de `PtrNodeDoubleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrNodeDoubleLink.h](../source/game_sa/Core/PtrNodeDoubleLink.h) | Declaraciones y tipos de `PtrNodeDoubleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrNodeSingleLink.cpp](../source/game_sa/Core/PtrNodeSingleLink.cpp) | Implementación de `PtrNodeSingleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/PtrNodeSingleLink.h](../source/game_sa/Core/PtrNodeSingleLink.h) | Declaraciones y tipos de `PtrNodeSingleLink`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/QuadTreeNode.h](../source/game_sa/Core/QuadTreeNode.h) | Declaraciones y tipos de `QuadTreeNode`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/SArray.h](../source/game_sa/Core/SArray.h) | Declaraciones y tipos de `SArray`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/Store.h](../source/game_sa/Core/Store.h) | Declaraciones y tipos de `Store`: estructuras de datos o utilidades matemáticas compartidas por el motor. |
| — | — | [Core/Vector2D.cpp](../source/game_sa/Core/Vector2D.cpp) | Implementación de tipos y operaciones matemáticas o geométricas. |

</details>

<details>
<summary>Data/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Data/Weather.def](../source/game_sa/Data/Weather.def) | Lista de definiciones de datos incluida por el código del juego. Recurso: `Weather`. |

</details>

<details>
<summary>DecisionMakers/ — 10 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [DecisionMakers/Decision.cpp](../source/game_sa/DecisionMakers/Decision.cpp) | Implementación de `Decision`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/Decision.h](../source/game_sa/DecisionMakers/Decision.h) | Declaraciones y tipos de `Decision`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMaker.cpp](../source/game_sa/DecisionMakers/DecisionMaker.cpp) | Implementación de `DecisionMaker`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMaker.h](../source/game_sa/DecisionMakers/DecisionMaker.h) | Declaraciones y tipos de `DecisionMaker`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMakerTypes.cpp](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) | Implementación de `DecisionMakerTypes`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMakerTypes.h](../source/game_sa/DecisionMakers/DecisionMakerTypes.h) | Declaraciones y tipos de `DecisionMakerTypes`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMakerTypesFileLoader.cpp](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) | Implementación de `DecisionMakerTypesFileLoader`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionMakerTypesFileLoader.h](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.h) | Declaraciones y tipos de `DecisionMakerTypesFileLoader`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionSimple.cpp](../source/game_sa/DecisionMakers/DecisionSimple.cpp) | Implementación de `DecisionSimple`: reglas de selección de respuestas de la inteligencia artificial. |
| — | — | [DecisionMakers/DecisionSimple.h](../source/game_sa/DecisionMakers/DecisionSimple.h) | Declaraciones y tipos de `DecisionSimple`: reglas de selección de respuestas de la inteligencia artificial. |

</details>

<details>
<summary>Entity/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Entity/AnimatedBuilding.cpp](../source/game_sa/Entity/AnimatedBuilding.cpp) | Implementación de representación de edificios y geometría del escenario. |
| — | — | [Entity/AnimatedBuilding.h](../source/game_sa/Entity/AnimatedBuilding.h) | Declaraciones y tipos de representación de edificios y geometría del escenario. |

</details>

<details>
<summary>Entity/Dummy/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Entity/Dummy/DummyPed.cpp](../source/game_sa/Entity/Dummy/DummyPed.cpp) | Implementación de representaciones simplificadas de entidades. |

</details>

<details>
<summary>Entity/Object/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Entity/Object/HandObject.cpp](../source/game_sa/Entity/Object/HandObject.cpp) | Implementación de representación de manos vinculadas a huesos de un peatón. |
| — | — | [Entity/Object/HandObject.h](../source/game_sa/Entity/Object/HandObject.h) | Declaraciones y tipos de representación de manos vinculadas a huesos de un peatón. |

</details>

<details>
<summary>Entity/Ped/ — 3 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Entity/Ped/PedSaveStructure.cpp](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) | Implementación de los datos `PedSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [Entity/Ped/PedSaveStructure.h](../source/game_sa/Entity/Ped/PedSaveStructure.h) | Declaraciones y tipos de los datos `PedSaveStructure` utilizados para guardar y restaurar estado de la partida. |
| — | — | [Entity/Ped/PedStdBonePositions.h](../source/game_sa/Entity/Ped/PedStdBonePositions.h) | Declaraciones y tipos de posiciones de referencia de los huesos de peatones. |

</details>

<details>
<summary>Entity/Vehicle/ — 8 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Entity/Vehicle/Bmx.cpp](../source/game_sa/Entity/Vehicle/Bmx.cpp) | Implementación de motocicletas o bicicletas, según la clase. |
| — | — | [Entity/Vehicle/Bmx.h](../source/game_sa/Entity/Vehicle/Bmx.h) | Declaraciones y tipos de motocicletas o bicicletas, según la clase. |
| — | — | [Entity/Vehicle/MonsterTruck.cpp](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) | Implementación de camionetas de ruedas grandes. |
| — | — | [Entity/Vehicle/MonsterTruck.h](../source/game_sa/Entity/Vehicle/MonsterTruck.h) | Declaraciones y tipos de camionetas de ruedas grandes. |
| — | — | [Entity/Vehicle/QuadBike.cpp](../source/game_sa/Entity/Vehicle/QuadBike.cpp) | Implementación de cuatrimotos. |
| — | — | [Entity/Vehicle/QuadBike.h](../source/game_sa/Entity/Vehicle/QuadBike.h) | Declaraciones y tipos de cuatrimotos. |
| — | — | [Entity/Vehicle/Trailer.cpp](../source/game_sa/Entity/Vehicle/Trailer.cpp) | Implementación de remolques. |
| — | — | [Entity/Vehicle/Trailer.h](../source/game_sa/Entity/Vehicle/Trailer.h) | Declaraciones y tipos de remolques. |

</details>

<details>
<summary>Enums/ — 66 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Enums/AnimationEnums.h](../source/game_sa/Enums/AnimationEnums.h) | Declaraciones y tipos de identificadores de animaciones. |
| — | — | [Enums/eAnimBlendCallbackType.h](../source/game_sa/Enums/eAnimBlendCallbackType.h) | Declaraciones y tipos de la enumeración `eAnimBlendCallbackType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eAreaCodes.h](../source/game_sa/Enums/eAreaCodes.h) | Declaraciones y tipos de la enumeración `eAreaCodes`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eAudioEvents.h](../source/game_sa/Enums/eAudioEvents.h) | Declaraciones y tipos de la enumeración `eAudioEvents`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eAudioFileType.h](../source/game_sa/Enums/eAudioFileType.h) | Declaraciones y tipos de la enumeración `eAudioFileType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eBirdsBiome.h](../source/game_sa/Enums/eBirdsBiome.h) | Declaraciones y tipos de la enumeración `eBirdsBiome`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eBoneTag.h](../source/game_sa/Enums/eBoneTag.h) | Declaraciones y tipos de la enumeración `eBoneTag`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCamMode.h](../source/game_sa/Enums/eCamMode.h) | Declaraciones y tipos de la enumeración `eCamMode`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCarDrivingStyle.h](../source/game_sa/Enums/eCarDrivingStyle.h) | Declaraciones y tipos de la enumeración `eCarDrivingStyle`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCarMission.h](../source/game_sa/Enums/eCarMission.h) | Declaraciones y tipos de la enumeración `eCarMission`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCarNodes.h](../source/game_sa/Enums/eCarNodes.h) | Declaraciones y tipos de la enumeración `eCarNodes`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCarWheel.cpp](../source/game_sa/Enums/eCarWheel.cpp) | Implementación de la enumeración `eCarWheel`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCarWheel.h](../source/game_sa/Enums/eCarWheel.h) | Declaraciones y tipos de la enumeración `eCarWheel`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCheats.h](../source/game_sa/Enums/eCheats.h) | Declaraciones y tipos de la enumeración `eCheats`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eClothesModelPart.h](../source/game_sa/Enums/eClothesModelPart.h) | Declaraciones y tipos de la enumeración `eClothesModelPart`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eClothesRules.h](../source/game_sa/Enums/eClothesRules.h) | Declaraciones y tipos de la enumeración `eClothesRules`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eClothesTexturePart.h](../source/game_sa/Enums/eClothesTexturePart.h) | Declaraciones y tipos de la enumeración `eClothesTexturePart`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eControllerType.h](../source/game_sa/Enums/eControllerType.h) | Declaraciones y tipos de la enumeración `eControllerType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eCrimeType.h](../source/game_sa/Enums/eCrimeType.h) | Declaraciones y tipos de la enumeración `eCrimeType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eDecisionMakerEvents.h](../source/game_sa/Enums/eDecisionMakerEvents.h) | Declaraciones y tipos de la enumeración `eDecisionMakerEvents`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eDirection.h](../source/game_sa/Enums/eDirection.h) | Declaraciones y tipos de la enumeración `eDirection`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eEntityStatus.h](../source/game_sa/Enums/eEntityStatus.h) | Declaraciones y tipos de la enumeración `eEntityStatus`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eEntityType.h](../source/game_sa/Enums/eEntityType.h) | Declaraciones y tipos de la enumeración `eEntityType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eEventType.h](../source/game_sa/Enums/eEventType.h) | Declaraciones y tipos de la enumeración `eEventType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eFontAlignment.h](../source/game_sa/Enums/eFontAlignment.h) | Declaraciones y tipos de la enumeración `eFontAlignment`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eGameState.h](../source/game_sa/Enums/eGameState.h) | Declaraciones y tipos de la enumeración `eGameState`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eHud.h](../source/game_sa/Enums/eHud.h) | Declaraciones y tipos de la enumeración `eHud`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eIKArm.h](../source/game_sa/Enums/eIKArm.h) | Declaraciones y tipos de la enumeración `eIKArm`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eIKChainSlot.h](../source/game_sa/Enums/eIKChainSlot.h) | Declaraciones y tipos de la enumeración `eIKChainSlot`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eItemDefinitionFlags.h](../source/game_sa/Enums/eItemDefinitionFlags.h) | Declaraciones y tipos de la enumeración `eItemDefinitionFlags`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eLanguage.h](../source/game_sa/Enums/eLanguage.h) | Declaraciones y tipos de la enumeración `eLanguage`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eLevelName.h](../source/game_sa/Enums/eLevelName.h) | Declaraciones y tipos de la enumeración `eLevelName`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eMeleeCombo.h](../source/game_sa/Enums/eMeleeCombo.h) | Declaraciones y tipos de la enumeración `eMeleeCombo`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eModelID.h](../source/game_sa/Enums/eModelID.h) | Declaraciones y tipos de la enumeración `eModelID`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eMoveState.h](../source/game_sa/Enums/eMoveState.h) | Declaraciones y tipos de la enumeración `eMoveState`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eOnscreenCounter.h](../source/game_sa/Enums/eOnscreenCounter.h) | Declaraciones y tipos de la enumeración `eOnscreenCounter`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/ePedModel.h](../source/game_sa/Enums/ePedModel.h) | Declaraciones y tipos de la enumeración `ePedModel`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/ePedRace.h](../source/game_sa/Enums/ePedRace.h) | Declaraciones y tipos de la enumeración `ePedRace`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/ePedState.h](../source/game_sa/Enums/ePedState.h) | Declaraciones y tipos de la enumeración `ePedState`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/ePedStats.h](../source/game_sa/Enums/ePedStats.h) | Declaraciones y tipos de la enumeración `ePedStats`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/ePedType.h](../source/game_sa/Enums/ePedType.h) | Declaraciones y tipos de la enumeración `ePedType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eRadioID.h](../source/game_sa/Enums/eRadioID.h) | Declaraciones y tipos de la enumeración `eRadioID`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eReplay.h](../source/game_sa/Enums/eReplay.h) | Declaraciones y tipos de la enumeración `eReplay`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eScriptCommands.h](../source/game_sa/Enums/eScriptCommands.h) | Declaraciones y tipos de identificadores de instrucciones u opcodes de scripts. |
| — | — | [Enums/eSoundID.h](../source/game_sa/Enums/eSoundID.h) | Declaraciones y tipos de la enumeración `eSoundID`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eSprintType.h](../source/game_sa/Enums/eSprintType.h) | Declaraciones y tipos de la enumeración `eSprintType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eStatModAbilities.h](../source/game_sa/Enums/eStatModAbilities.h) | Declaraciones y tipos de la enumeración `eStatModAbilities`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eStats.h](../source/game_sa/Enums/eStats.h) | Declaraciones y tipos de la enumeración `eStats`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eStatsReactions.h](../source/game_sa/Enums/eStatsReactions.h) | Declaraciones y tipos de la enumeración `eStatsReactions`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eSurfaceType.h](../source/game_sa/Enums/eSurfaceType.h) | Declaraciones y tipos de la enumeración `eSurfaceType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eTargetDoor.h](../source/game_sa/Enums/eTargetDoor.h) | Declaraciones y tipos de la enumeración `eTargetDoor`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eTaskType.h](../source/game_sa/Enums/eTaskType.h) | Declaraciones y tipos de la enumeración `eTaskType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eUserTrackPlayMode.h](../source/game_sa/Enums/eUserTrackPlayMode.h) | Declaraciones y tipos de la enumeración `eUserTrackPlayMode`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eUserTrackScanState.h](../source/game_sa/Enums/eUserTrackScanState.h) | Declaraciones y tipos de la enumeración `eUserTrackScanState`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eVehicleClass.h](../source/game_sa/Enums/eVehicleClass.h) | Declaraciones y tipos de la enumeración `eVehicleClass`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eVehicleHandlingFlags.h](../source/game_sa/Enums/eVehicleHandlingFlags.h) | Declaraciones y tipos de la enumeración `eVehicleHandlingFlags`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eVehicleHandlingModelFlags.h](../source/game_sa/Enums/eVehicleHandlingModelFlags.h) | Declaraciones y tipos de la enumeración `eVehicleHandlingModelFlags`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eVehicleType.h](../source/game_sa/Enums/eVehicleType.h) | Declaraciones y tipos de la enumeración `eVehicleType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWantedLevel.h](../source/game_sa/Enums/eWantedLevel.h) | Declaraciones y tipos de la enumeración `eWantedLevel`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeaponFire.h](../source/game_sa/Enums/eWeaponFire.h) | Declaraciones y tipos de la enumeración `eWeaponFire`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeaponFlags.h](../source/game_sa/Enums/eWeaponFlags.h) | Declaraciones y tipos de la enumeración `eWeaponFlags`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeaponModel.h](../source/game_sa/Enums/eWeaponModel.h) | Declaraciones y tipos de la enumeración `eWeaponModel`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeaponSkill.h](../source/game_sa/Enums/eWeaponSkill.h) | Declaraciones y tipos de la enumeración `eWeaponSkill`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeaponType.h](../source/game_sa/Enums/eWeaponType.h) | Declaraciones y tipos de la enumeración `eWeaponType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWeatherType.h](../source/game_sa/Enums/eWeatherType.h) | Declaraciones y tipos de la enumeración `eWeatherType`, que define identificadores o estados usados por el juego. |
| — | — | [Enums/eWinchType.h](../source/game_sa/Enums/eWinchType.h) | Declaraciones y tipos de la enumeración `eWinchType`, que define identificadores o estados usados por el juego. |

</details>

<details>
<summary>Events/ — 177 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Events/Event.cpp](../source/game_sa/Events/Event.cpp) | Implementación del evento `Event` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/Event.h](../source/game_sa/Events/Event.h) | Declaraciones y tipos del evento `Event` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePed.cpp](../source/game_sa/Events/EventAcquaintancePed.cpp) | Implementación del evento `EventAcquaintancePed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePed.h](../source/game_sa/Events/EventAcquaintancePed.h) | Declaraciones y tipos del evento `EventAcquaintancePed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedDislike.h](../source/game_sa/Events/EventAcquaintancePedDislike.h) | Declaraciones y tipos del evento `EventAcquaintancePedDislike` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedHate.cpp](../source/game_sa/Events/EventAcquaintancePedHate.cpp) | Implementación del evento `EventAcquaintancePedHate` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedHate.h](../source/game_sa/Events/EventAcquaintancePedHate.h) | Declaraciones y tipos del evento `EventAcquaintancePedHate` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedHateBadlyLit.cpp](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) | Implementación del evento `EventAcquaintancePedHateBadlyLit` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedHateBadlyLit.h](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h) | Declaraciones y tipos del evento `EventAcquaintancePedHateBadlyLit` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedLike.h](../source/game_sa/Events/EventAcquaintancePedLike.h) | Declaraciones y tipos del evento `EventAcquaintancePedLike` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAcquaintancePedRespect.h](../source/game_sa/Events/EventAcquaintancePedRespect.h) | Declaraciones y tipos del evento `EventAcquaintancePedRespect` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAreaCodes.cpp](../source/game_sa/Events/EventAreaCodes.cpp) | Implementación del evento `EventAreaCodes` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAreaCodes.h](../source/game_sa/Events/EventAreaCodes.h) | Declaraciones y tipos del evento `EventAreaCodes` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAttractor.cpp](../source/game_sa/Events/EventAttractor.cpp) | Implementación del evento `EventAttractor` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventAttractor.h](../source/game_sa/Events/EventAttractor.h) | Declaraciones y tipos del evento `EventAttractor` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventBuildingCollision.cpp](../source/game_sa/Events/EventBuildingCollision.cpp) | Implementación del evento `EventBuildingCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventBuildingCollision.h](../source/game_sa/Events/EventBuildingCollision.h) | Declaraciones y tipos del evento `EventBuildingCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCarUpsideDown.cpp](../source/game_sa/Events/EventCarUpsideDown.cpp) | Implementación del evento `EventCarUpsideDown` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCarUpsideDown.h](../source/game_sa/Events/EventCarUpsideDown.h) | Declaraciones y tipos del evento `EventCarUpsideDown` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventChatPartner.cpp](../source/game_sa/Events/EventChatPartner.cpp) | Implementación del evento `EventChatPartner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventChatPartner.h](../source/game_sa/Events/EventChatPartner.h) | Declaraciones y tipos del evento `EventChatPartner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCopCarBeingStolen.cpp](../source/game_sa/Events/EventCopCarBeingStolen.cpp) | Implementación del evento `EventCopCarBeingStolen` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCopCarBeingStolen.h](../source/game_sa/Events/EventCopCarBeingStolen.h) | Declaraciones y tipos del evento `EventCopCarBeingStolen` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCreatePartnerTask.cpp](../source/game_sa/Events/EventCreatePartnerTask.cpp) | Implementación del evento `EventCreatePartnerTask` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventCreatePartnerTask.h](../source/game_sa/Events/EventCreatePartnerTask.h) | Declaraciones y tipos del evento `EventCreatePartnerTask` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDamage.cpp](../source/game_sa/Events/EventDamage.cpp) | Implementación del evento `EventDamage` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDamage.h](../source/game_sa/Events/EventDamage.h) | Declaraciones y tipos del evento `EventDamage` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDanger.cpp](../source/game_sa/Events/EventDanger.cpp) | Implementación del evento `EventDanger` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDanger.h](../source/game_sa/Events/EventDanger.h) | Declaraciones y tipos del evento `EventDanger` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDeadPed.cpp](../source/game_sa/Events/EventDeadPed.cpp) | Implementación del evento `EventDeadPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDeadPed.h](../source/game_sa/Events/EventDeadPed.h) | Declaraciones y tipos del evento `EventDeadPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDeath.cpp](../source/game_sa/Events/EventDeath.cpp) | Implementación del evento `EventDeath` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDeath.h](../source/game_sa/Events/EventDeath.h) | Declaraciones y tipos del evento `EventDeath` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDontJoinPlayerGroup.cpp](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) | Implementación del evento `EventDontJoinPlayerGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDontJoinPlayerGroup.h](../source/game_sa/Events/EventDontJoinPlayerGroup.h) | Declaraciones y tipos del evento `EventDontJoinPlayerGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDraggedOutCar.cpp](../source/game_sa/Events/EventDraggedOutCar.cpp) | Implementación del evento `EventDraggedOutCar` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventDraggedOutCar.h](../source/game_sa/Events/EventDraggedOutCar.h) | Declaraciones y tipos del evento `EventDraggedOutCar` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventEditableResponse.cpp](../source/game_sa/Events/EventEditableResponse.cpp) | Implementación del evento `EventEditableResponse` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventEditableResponse.h](../source/game_sa/Events/EventEditableResponse.h) | Declaraciones y tipos del evento `EventEditableResponse` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventEscalator.cpp](../source/game_sa/Events/EventEscalator.cpp) | Implementación del evento `EventEscalator` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventEscalator.h](../source/game_sa/Events/EventEscalator.h) | Declaraciones y tipos del evento `EventEscalator` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventFireNearby.cpp](../source/game_sa/Events/EventFireNearby.cpp) | Implementación del evento `EventFireNearby` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventFireNearby.h](../source/game_sa/Events/EventFireNearby.h) | Declaraciones y tipos del evento `EventFireNearby` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGlobalGroup.cpp](../source/game_sa/Events/EventGlobalGroup.cpp) | Implementación del evento `EventGlobalGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGlobalGroup.h](../source/game_sa/Events/EventGlobalGroup.h) | Declaraciones y tipos del evento `EventGlobalGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGotKnockedOverByCar.cpp](../source/game_sa/Events/EventGotKnockedOverByCar.cpp) | Implementación del evento `EventGotKnockedOverByCar` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGotKnockedOverByCar.h](../source/game_sa/Events/EventGotKnockedOverByCar.h) | Declaraciones y tipos del evento `EventGotKnockedOverByCar` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGroup.cpp](../source/game_sa/Events/EventGroup.cpp) | Implementación del evento `EventGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGroup.h](../source/game_sa/Events/EventGroup.h) | Declaraciones y tipos del evento `EventGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGroupEvent.cpp](../source/game_sa/Events/EventGroupEvent.cpp) | Implementación del evento `EventGroupEvent` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGroupEvent.h](../source/game_sa/Events/EventGroupEvent.h) | Declaraciones y tipos del evento `EventGroupEvent` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunAimedAt.cpp](../source/game_sa/Events/EventGunAimedAt.cpp) | Implementación del evento `EventGunAimedAt` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunAimedAt.h](../source/game_sa/Events/EventGunAimedAt.h) | Declaraciones y tipos del evento `EventGunAimedAt` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunShot.cpp](../source/game_sa/Events/EventGunShot.cpp) | Implementación del evento `EventGunShot` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunShot.h](../source/game_sa/Events/EventGunShot.h) | Declaraciones y tipos del evento `EventGunShot` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunShotWhizzedBy.cpp](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) | Implementación del evento `EventGunShotWhizzedBy` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventGunShotWhizzedBy.h](../source/game_sa/Events/EventGunShotWhizzedBy.h) | Declaraciones y tipos del evento `EventGunShotWhizzedBy` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHandler.cpp](../source/game_sa/Events/EventHandler.cpp) | Implementación del evento `EventHandler` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHandler.h](../source/game_sa/Events/EventHandler.h) | Declaraciones y tipos del evento `EventHandler` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHandlerHistory.cpp](../source/game_sa/Events/EventHandlerHistory.cpp) | Implementación del evento `EventHandlerHistory` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHandlerHistory.h](../source/game_sa/Events/EventHandlerHistory.h) | Declaraciones y tipos del evento `EventHandlerHistory` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHealthLow.h](../source/game_sa/Events/EventHealthLow.h) | Declaraciones y tipos del evento `EventHealthLow` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHealthReallyLow.h](../source/game_sa/Events/EventHealthReallyLow.h) | Declaraciones y tipos del evento `EventHealthReallyLow` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHighAngerAtPlayer.h](../source/game_sa/Events/EventHighAngerAtPlayer.h) | Declaraciones y tipos del evento `EventHighAngerAtPlayer` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHitByWaterCannon.cpp](../source/game_sa/Events/EventHitByWaterCannon.cpp) | Implementación del evento `EventHitByWaterCannon` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventHitByWaterCannon.h](../source/game_sa/Events/EventHitByWaterCannon.h) | Declaraciones y tipos del evento `EventHitByWaterCannon` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInAir.cpp](../source/game_sa/Events/EventInAir.cpp) | Implementación del evento `EventInAir` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInAir.h](../source/game_sa/Events/EventInAir.h) | Declaraciones y tipos del evento `EventInAir` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInteriorUseInfo.cpp](../source/game_sa/Events/EventInteriorUseInfo.cpp) | Implementación del evento `EventInteriorUseInfo` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInteriorUseInfo.h](../source/game_sa/Events/EventInteriorUseInfo.h) | Declaraciones y tipos del evento `EventInteriorUseInfo` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInWater.cpp](../source/game_sa/Events/EventInWater.cpp) | Implementación del evento `EventInWater` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventInWater.h](../source/game_sa/Events/EventInWater.h) | Declaraciones y tipos del evento `EventInWater` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventKnockOffBike.cpp](../source/game_sa/Events/EventKnockOffBike.cpp) | Implementación del evento `EventKnockOffBike` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventKnockOffBike.h](../source/game_sa/Events/EventKnockOffBike.h) | Declaraciones y tipos del evento `EventKnockOffBike` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderEnteredCarAsDriver.cpp](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) | Implementación del evento `EventLeaderEnteredCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderEnteredCarAsDriver.h](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h) | Declaraciones y tipos del evento `EventLeaderEnteredCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderEntryExit.cpp](../source/game_sa/Events/EventLeaderEntryExit.cpp) | Implementación del evento `EventLeaderEntryExit` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderEntryExit.h](../source/game_sa/Events/EventLeaderEntryExit.h) | Declaraciones y tipos del evento `EventLeaderEntryExit` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderExitedCarAsDriver.cpp](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) | Implementación del evento `EventLeaderExitedCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderExitedCarAsDriver.h](../source/game_sa/Events/EventLeaderExitedCarAsDriver.h) | Declaraciones y tipos del evento `EventLeaderExitedCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderQuitEnteringCarAsDriver.cpp](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) | Implementación del evento `EventLeaderQuitEnteringCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeaderQuitEnteringCarAsDriver.h](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h) | Declaraciones y tipos del evento `EventLeaderQuitEnteringCarAsDriver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeanOnVehicle.cpp](../source/game_sa/Events/EventLeanOnVehicle.cpp) | Implementación del evento `EventLeanOnVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLeanOnVehicle.h](../source/game_sa/Events/EventLeanOnVehicle.h) | Declaraciones y tipos del evento `EventLeanOnVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLowAngerAtPlayer.cpp](../source/game_sa/Events/EventLowAngerAtPlayer.cpp) | Implementación del evento `EventLowAngerAtPlayer` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventLowAngerAtPlayer.h](../source/game_sa/Events/EventLowAngerAtPlayer.h) | Declaraciones y tipos del evento `EventLowAngerAtPlayer` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventNewGangMember.cpp](../source/game_sa/Events/EventNewGangMember.cpp) | Implementación del evento `EventNewGangMember` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventNewGangMember.h](../source/game_sa/Events/EventNewGangMember.h) | Declaraciones y tipos del evento `EventNewGangMember` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventObjectCollision.cpp](../source/game_sa/Events/EventObjectCollision.cpp) | Implementación del evento `EventObjectCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventObjectCollision.h](../source/game_sa/Events/EventObjectCollision.h) | Declaraciones y tipos del evento `EventObjectCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventOnFire.cpp](../source/game_sa/Events/EventOnFire.cpp) | Implementación del evento `EventOnFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventOnFire.h](../source/game_sa/Events/EventOnFire.h) | Declaraciones y tipos del evento `EventOnFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPassObject.cpp](../source/game_sa/Events/EventPassObject.cpp) | Implementación del evento `EventPassObject` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPassObject.h](../source/game_sa/Events/EventPassObject.h) | Declaraciones y tipos del evento `EventPassObject` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedCollisionWithPed.cpp](../source/game_sa/Events/EventPedCollisionWithPed.cpp) | Implementación del evento `EventPedCollisionWithPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedCollisionWithPed.h](../source/game_sa/Events/EventPedCollisionWithPed.h) | Declaraciones y tipos del evento `EventPedCollisionWithPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedCollisionWithPlayer.cpp](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) | Implementación del evento `EventPedCollisionWithPlayer` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedCollisionWithPlayer.h](../source/game_sa/Events/EventPedCollisionWithPlayer.h) | Declaraciones y tipos del evento `EventPedCollisionWithPlayer` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedEnteredMyVehicle.cpp](../source/game_sa/Events/EventPedEnteredMyVehicle.cpp) | Implementación del evento `EventPedEnteredMyVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedEnteredMyVehicle.h](../source/game_sa/Events/EventPedEnteredMyVehicle.h) | Declaraciones y tipos del evento `EventPedEnteredMyVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedToChase.cpp](../source/game_sa/Events/EventPedToChase.cpp) | Implementación del evento `EventPedToChase` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedToChase.h](../source/game_sa/Events/EventPedToChase.h) | Declaraciones y tipos del evento `EventPedToChase` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedToFlee.cpp](../source/game_sa/Events/EventPedToFlee.cpp) | Implementación del evento `EventPedToFlee` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPedToFlee.h](../source/game_sa/Events/EventPedToFlee.h) | Declaraciones y tipos del evento `EventPedToFlee` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCollisionWithPed.cpp](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) | Implementación del evento `EventPlayerCollisionWithPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCollisionWithPed.h](../source/game_sa/Events/EventPlayerCollisionWithPed.h) | Declaraciones y tipos del evento `EventPlayerCollisionWithPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroup.cpp](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) | Implementación del evento `EventPlayerCommandToGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroup.h](../source/game_sa/Events/EventPlayerCommandToGroup.h) | Declaraciones y tipos del evento `EventPlayerCommandToGroup` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroupAttack.cpp](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) | Implementación del evento `EventPlayerCommandToGroupAttack` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroupAttack.h](../source/game_sa/Events/EventPlayerCommandToGroupAttack.h) | Declaraciones y tipos del evento `EventPlayerCommandToGroupAttack` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroupGather.cpp](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) | Implementación del evento `EventPlayerCommandToGroupGather` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPlayerCommandToGroupGather.h](../source/game_sa/Events/EventPlayerCommandToGroupGather.h) | Declaraciones y tipos del evento `EventPlayerCommandToGroupGather` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialGetRunOver.cpp](../source/game_sa/Events/EventPotentialGetRunOver.cpp) | Implementación del evento `EventPotentialGetRunOver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialGetRunOver.h](../source/game_sa/Events/EventPotentialGetRunOver.h) | Declaraciones y tipos del evento `EventPotentialGetRunOver` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoBuilding.cpp](../source/game_sa/Events/EventPotentialWalkIntoBuilding.cpp) | Implementación del evento `EventPotentialWalkIntoBuilding` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoBuilding.h](../source/game_sa/Events/EventPotentialWalkIntoBuilding.h) | Declaraciones y tipos del evento `EventPotentialWalkIntoBuilding` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoFire.cpp](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) | Implementación del evento `EventPotentialWalkIntoFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoFire.h](../source/game_sa/Events/EventPotentialWalkIntoFire.h) | Declaraciones y tipos del evento `EventPotentialWalkIntoFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoObject.cpp](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) | Implementación del evento `EventPotentialWalkIntoObject` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoObject.h](../source/game_sa/Events/EventPotentialWalkIntoObject.h) | Declaraciones y tipos del evento `EventPotentialWalkIntoObject` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoPed.cpp](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) | Implementación del evento `EventPotentialWalkIntoPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoPed.h](../source/game_sa/Events/EventPotentialWalkIntoPed.h) | Declaraciones y tipos del evento `EventPotentialWalkIntoPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoVehicle.cpp](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) | Implementación del evento `EventPotentialWalkIntoVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventPotentialWalkIntoVehicle.h](../source/game_sa/Events/EventPotentialWalkIntoVehicle.h) | Declaraciones y tipos del evento `EventPotentialWalkIntoVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventRevived.cpp](../source/game_sa/Events/EventRevived.cpp) | Implementación del evento `EventRevived` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventRevived.h](../source/game_sa/Events/EventRevived.h) | Declaraciones y tipos del evento `EventRevived` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScanner.cpp](../source/game_sa/Events/EventScanner.cpp) | Implementación del evento `EventScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScanner.h](../source/game_sa/Events/EventScanner.h) | Declaraciones y tipos del evento `EventScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScriptCommand.cpp](../source/game_sa/Events/EventScriptCommand.cpp) | Implementación del evento `EventScriptCommand` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScriptCommand.h](../source/game_sa/Events/EventScriptCommand.h) | Declaraciones y tipos del evento `EventScriptCommand` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScriptedAttractor.cpp](../source/game_sa/Events/EventScriptedAttractor.cpp) | Implementación del evento `EventScriptedAttractor` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventScriptedAttractor.h](../source/game_sa/Events/EventScriptedAttractor.h) | Declaraciones y tipos del evento `EventScriptedAttractor` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSeenCop.cpp](../source/game_sa/Events/EventSeenCop.cpp) | Implementación del evento `EventSeenCop` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSeenCop.h](../source/game_sa/Events/EventSeenCop.h) | Declaraciones y tipos del evento `EventSeenCop` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSeenPanickedPed.cpp](../source/game_sa/Events/EventSeenPanickedPed.cpp) | Implementación del evento `EventSeenPanickedPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSeenPanickedPed.h](../source/game_sa/Events/EventSeenPanickedPed.h) | Declaraciones y tipos del evento `EventSeenPanickedPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSexyPed.cpp](../source/game_sa/Events/EventSexyPed.cpp) | Implementación del evento `EventSexyPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSexyPed.h](../source/game_sa/Events/EventSexyPed.h) | Declaraciones y tipos del evento `EventSexyPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSexyVehicle.cpp](../source/game_sa/Events/EventSexyVehicle.cpp) | Implementación del evento `EventSexyVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSexyVehicle.h](../source/game_sa/Events/EventSexyVehicle.h) | Declaraciones y tipos del evento `EventSexyVehicle` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSignalAtPed.cpp](../source/game_sa/Events/EventSignalAtPed.cpp) | Implementación del evento `EventSignalAtPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSignalAtPed.h](../source/game_sa/Events/EventSignalAtPed.h) | Declaraciones y tipos del evento `EventSignalAtPed` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSoundLoud.cpp](../source/game_sa/Events/EventSoundLoud.cpp) | Implementación del evento `EventSoundLoud` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSoundLoud.h](../source/game_sa/Events/EventSoundLoud.h) | Declaraciones y tipos del evento `EventSoundLoud` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSoundQuiet.cpp](../source/game_sa/Events/EventSoundQuiet.cpp) | Implementación del evento `EventSoundQuiet` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSoundQuiet.h](../source/game_sa/Events/EventSoundQuiet.h) | Declaraciones y tipos del evento `EventSoundQuiet` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSource.cpp](../source/game_sa/Events/EventSource.cpp) | Implementación del evento `EventSource` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSource.h](../source/game_sa/Events/EventSource.h) | Declaraciones y tipos del evento `EventSource` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSpecial.cpp](../source/game_sa/Events/EventSpecial.cpp) | Implementación del evento `EventSpecial` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventSpecial.h](../source/game_sa/Events/EventSpecial.h) | Declaraciones y tipos del evento `EventSpecial` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventStuckInAir.cpp](../source/game_sa/Events/EventStuckInAir.cpp) | Implementación del evento `EventStuckInAir` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventStuckInAir.h](../source/game_sa/Events/EventStuckInAir.h) | Declaraciones y tipos del evento `EventStuckInAir` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleCollision.cpp](../source/game_sa/Events/EventVehicleCollision.cpp) | Implementación del evento `EventVehicleCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleCollision.h](../source/game_sa/Events/EventVehicleCollision.h) | Declaraciones y tipos del evento `EventVehicleCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamage.cpp](../source/game_sa/Events/EventVehicleDamage.cpp) | Implementación del evento `EventVehicleDamage` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamage.h](../source/game_sa/Events/EventVehicleDamage.h) | Declaraciones y tipos del evento `EventVehicleDamage` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamageCollision.cpp](../source/game_sa/Events/EventVehicleDamageCollision.cpp) | Implementación del evento `EventVehicleDamageCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamageCollision.h](../source/game_sa/Events/EventVehicleDamageCollision.h) | Declaraciones y tipos del evento `EventVehicleDamageCollision` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamageWeapon.cpp](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) | Implementación del evento `EventVehicleDamageWeapon` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDamageWeapon.h](../source/game_sa/Events/EventVehicleDamageWeapon.h) | Declaraciones y tipos del evento `EventVehicleDamageWeapon` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDied.cpp](../source/game_sa/Events/EventVehicleDied.cpp) | Implementación del evento `EventVehicleDied` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleDied.h](../source/game_sa/Events/EventVehicleDied.h) | Declaraciones y tipos del evento `EventVehicleDied` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleHitAndRun.cpp](../source/game_sa/Events/EventVehicleHitAndRun.cpp) | Implementación del evento `EventVehicleHitAndRun` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleHitAndRun.h](../source/game_sa/Events/EventVehicleHitAndRun.h) | Declaraciones y tipos del evento `EventVehicleHitAndRun` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleOnFire.cpp](../source/game_sa/Events/EventVehicleOnFire.cpp) | Implementación del evento `EventVehicleOnFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleOnFire.h](../source/game_sa/Events/EventVehicleOnFire.h) | Declaraciones y tipos del evento `EventVehicleOnFire` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleThreat.cpp](../source/game_sa/Events/EventVehicleThreat.cpp) | Implementación del evento `EventVehicleThreat` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleThreat.h](../source/game_sa/Events/EventVehicleThreat.h) | Declaraciones y tipos del evento `EventVehicleThreat` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleToSteal.cpp](../source/game_sa/Events/EventVehicleToSteal.cpp) | Implementación del evento `EventVehicleToSteal` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/EventVehicleToSteal.h](../source/game_sa/Events/EventVehicleToSteal.h) | Declaraciones y tipos del evento `EventVehicleToSteal` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/GroupEventHandler.cpp](../source/game_sa/Events/GroupEventHandler.cpp) | Implementación del evento `GroupEventHandler` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/GroupEventHandler.h](../source/game_sa/Events/GroupEventHandler.h) | Declaraciones y tipos del evento `GroupEventHandler` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/NearbyFireScanner.h](../source/game_sa/Events/NearbyFireScanner.h) | Declaraciones y tipos del evento `NearbyFireScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/ObjectPotentialCollisionScanner.h](../source/game_sa/Events/ObjectPotentialCollisionScanner.h) | Declaraciones y tipos del evento `ObjectPotentialCollisionScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/PedAcquaintanceScanner.h](../source/game_sa/Events/PedAcquaintanceScanner.h) | Declaraciones y tipos del evento `PedAcquaintanceScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/SexyPedScanner.h](../source/game_sa/Events/SexyPedScanner.h) | Declaraciones y tipos del evento `SexyPedScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Events/VehiclePotentialCollisionScanner.h](../source/game_sa/Events/VehiclePotentialCollisionScanner.h) | Declaraciones y tipos del evento `VehiclePotentialCollisionScanner` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |

</details>

<details>
<summary>Frontend/ — 8 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Frontend/FrontendScreensPC.h](../source/game_sa/Frontend/FrontendScreensPC.h) | Declaraciones y tipos de `FrontendScreensPC`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Draw.cpp](../source/game_sa/Frontend/MenuManager_Draw.cpp) | Implementación de `MenuManager_Draw`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Gallery.cpp](../source/game_sa/Frontend/MenuManager_Gallery.cpp) | Implementación de `MenuManager_Gallery`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Input.cpp](../source/game_sa/Frontend/MenuManager_Input.cpp) | Implementación de `MenuManager_Input`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Internal.cpp](../source/game_sa/Frontend/MenuManager_Internal.cpp) | Implementación de `MenuManager_Internal`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Internal.h](../source/game_sa/Frontend/MenuManager_Internal.h) | Declaraciones y tipos de `MenuManager_Internal`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Pages.cpp](../source/game_sa/Frontend/MenuManager_Pages.cpp) | Implementación de `MenuManager_Pages`: parte del dibujo, entrada o procesamiento de los menús del juego. |
| — | — | [Frontend/MenuManager_Process.cpp](../source/game_sa/Frontend/MenuManager_Process.cpp) | Implementación de `MenuManager_Process`: parte del dibujo, entrada o procesamiento de los menús del juego. |

</details>

<details>
<summary>Fx/ — 40 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Fx/CarFXRenderer.cpp](../source/game_sa/Fx/CarFXRenderer.cpp) | Implementación del componente `CarFXRenderer` del sistema de efectos visuales y partículas. |
| — | — | [Fx/CarFXRenderer.h](../source/game_sa/Fx/CarFXRenderer.h) | Declaraciones y tipos del componente `CarFXRenderer` del sistema de efectos visuales y partículas. |
| — | — | [Fx/eFxInfoType.h](../source/game_sa/Fx/eFxInfoType.h) | Declaraciones y tipos de la enumeración `eFxInfoType`, que define identificadores o estados usados por el juego. |
| — | — | [Fx/EmissionInfo.h](../source/game_sa/Fx/EmissionInfo.h) | Declaraciones y tipos del componente `EmissionInfo` del sistema de efectos visuales y partículas. |
| — | — | [Fx/Fx.cpp](../source/game_sa/Fx/Fx.cpp) | Implementación del componente `Fx` del sistema de efectos visuales y partículas. |
| — | — | [Fx/Fx.h](../source/game_sa/Fx/Fx.h) | Declaraciones y tipos del componente `Fx` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxBox.cpp](../source/game_sa/Fx/FxBox.cpp) | Implementación del componente `FxBox` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxBox.h](../source/game_sa/Fx/FxBox.h) | Declaraciones y tipos del componente `FxBox` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitter.cpp](../source/game_sa/Fx/FxEmitter.cpp) | Implementación del componente `FxEmitter` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitter.h](../source/game_sa/Fx/FxEmitter.h) | Declaraciones y tipos del componente `FxEmitter` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitterBP.cpp](../source/game_sa/Fx/FxEmitterBP.cpp) | Implementación del componente `FxEmitterBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitterBP.h](../source/game_sa/Fx/FxEmitterBP.h) | Declaraciones y tipos del componente `FxEmitterBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitterPrt.cpp](../source/game_sa/Fx/FxEmitterPrt.cpp) | Implementación del componente `FxEmitterPrt` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxEmitterPrt.h](../source/game_sa/Fx/FxEmitterPrt.h) | Declaraciones y tipos del componente `FxEmitterPrt` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxFrustumInfo.cpp](../source/game_sa/Fx/FxFrustumInfo.cpp) | Implementación del componente `FxFrustumInfo` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxFrustumInfo.h](../source/game_sa/Fx/FxFrustumInfo.h) | Declaraciones y tipos del componente `FxFrustumInfo` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxInfoManager.cpp](../source/game_sa/Fx/FxInfoManager.cpp) | Implementación del parámetro de efecto `FxInfoManager`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/FxInfoManager.h](../source/game_sa/Fx/FxInfoManager.h) | Declaraciones y tipos del parámetro de efecto `FxInfoManager`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/FxManager.cpp](../source/game_sa/Fx/FxManager.cpp) | Implementación del componente `FxManager` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxManager.h](../source/game_sa/Fx/FxManager.h) | Declaraciones y tipos del componente `FxManager` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxMemoryPool.cpp](../source/game_sa/Fx/FxMemoryPool.cpp) | Implementación del componente `FxMemoryPool` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxMemoryPool.h](../source/game_sa/Fx/FxMemoryPool.h) | Declaraciones y tipos del componente `FxMemoryPool` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPlane.h](../source/game_sa/Fx/FxPlane.h) | Declaraciones y tipos del componente `FxPlane` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPlane_c.cpp](../source/game_sa/Fx/FxPlane_c.cpp) | Implementación del componente `FxPlane_c` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrim.cpp](../source/game_sa/Fx/FxPrim.cpp) | Implementación del componente `FxPrim` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrim.h](../source/game_sa/Fx/FxPrim.h) | Declaraciones y tipos del componente `FxPrim` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrimBP.cpp](../source/game_sa/Fx/FxPrimBP.cpp) | Implementación del componente `FxPrimBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrimBP.h](../source/game_sa/Fx/FxPrimBP.h) | Declaraciones y tipos del componente `FxPrimBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrtMult.cpp](../source/game_sa/Fx/FxPrtMult.cpp) | Implementación del componente `FxPrtMult` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxPrtMult.h](../source/game_sa/Fx/FxPrtMult.h) | Declaraciones y tipos del componente `FxPrtMult` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSphere.cpp](../source/game_sa/Fx/FxSphere.cpp) | Implementación del componente `FxSphere` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSphere.h](../source/game_sa/Fx/FxSphere.h) | Declaraciones y tipos del componente `FxSphere` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSystem.cpp](../source/game_sa/Fx/FxSystem.cpp) | Implementación del componente `FxSystem` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSystem.h](../source/game_sa/Fx/FxSystem.h) | Declaraciones y tipos del componente `FxSystem` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSystemBP.cpp](../source/game_sa/Fx/FxSystemBP.cpp) | Implementación del componente `FxSystemBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxSystemBP.h](../source/game_sa/Fx/FxSystemBP.h) | Declaraciones y tipos del componente `FxSystemBP` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxTools.cpp](../source/game_sa/Fx/FxTools.cpp) | Implementación del componente `FxTools` del sistema de efectos visuales y partículas. |
| — | — | [Fx/FxTools.h](../source/game_sa/Fx/FxTools.h) | Declaraciones y tipos del componente `FxTools` del sistema de efectos visuales y partículas. |
| — | — | [Fx/MovementInfo.h](../source/game_sa/Fx/MovementInfo.h) | Declaraciones y tipos del componente `MovementInfo` del sistema de efectos visuales y partículas. |
| — | — | [Fx/RenderInfo.h](../source/game_sa/Fx/RenderInfo.h) | Declaraciones y tipos del componente `RenderInfo` del sistema de efectos visuales y partículas. |

</details>

<details>
<summary>Fx/Blueprints/ — 69 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Fx/Blueprints/FxInfo.cpp](../source/game_sa/Fx/Blueprints/FxInfo.cpp) | Implementación del parámetro de efecto `FxInfo`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfo.h](../source/game_sa/Fx/Blueprints/FxInfo.h) | Declaraciones y tipos del parámetro de efecto `FxInfo`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAnimTexture.cpp](../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp) | Implementación del parámetro de efecto `FxInfoAnimTexture`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAnimTexture.h](../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.h) | Declaraciones y tipos del parámetro de efecto `FxInfoAnimTexture`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAttractLine.cpp](../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp) | Implementación del parámetro de efecto `FxInfoAttractLine`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAttractLine.h](../source/game_sa/Fx/Blueprints/FxInfoAttractLine.h) | Declaraciones y tipos del parámetro de efecto `FxInfoAttractLine`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAttractPt.cpp](../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp) | Implementación del parámetro de efecto `FxInfoAttractPt`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoAttractPt.h](../source/game_sa/Fx/Blueprints/FxInfoAttractPt.h) | Declaraciones y tipos del parámetro de efecto `FxInfoAttractPt`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColour.cpp](../source/game_sa/Fx/Blueprints/FxInfoColour.cpp) | Implementación del parámetro de efecto `FxInfoColour`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColour.h](../source/game_sa/Fx/Blueprints/FxInfoColour.h) | Declaraciones y tipos del parámetro de efecto `FxInfoColour`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColourBright.cpp](../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp) | Implementación del parámetro de efecto `FxInfoColourBright`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColourBright.h](../source/game_sa/Fx/Blueprints/FxInfoColourBright.h) | Declaraciones y tipos del parámetro de efecto `FxInfoColourBright`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColourRange.cpp](../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp) | Implementación del parámetro de efecto `FxInfoColourRange`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoColourRange.h](../source/game_sa/Fx/Blueprints/FxInfoColourRange.h) | Declaraciones y tipos del parámetro de efecto `FxInfoColourRange`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoDir.cpp](../source/game_sa/Fx/Blueprints/FxInfoDir.cpp) | Implementación del parámetro de efecto `FxInfoDir`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoDir.h](../source/game_sa/Fx/Blueprints/FxInfoDir.h) | Declaraciones y tipos del parámetro de efecto `FxInfoDir`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmAngle.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp) | Implementación del parámetro de efecto `FxInfoEmAngle`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmAngle.h](../source/game_sa/Fx/Blueprints/FxInfoEmAngle.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmAngle`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmDir.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp) | Implementación del parámetro de efecto `FxInfoEmDir`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmDir.h](../source/game_sa/Fx/Blueprints/FxInfoEmDir.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmDir`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmLife.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp) | Implementación del parámetro de efecto `FxInfoEmLife`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmLife.h](../source/game_sa/Fx/Blueprints/FxInfoEmLife.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmLife`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmPos.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp) | Implementación del parámetro de efecto `FxInfoEmPos`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmPos.h](../source/game_sa/Fx/Blueprints/FxInfoEmPos.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmPos`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmRate.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp) | Implementación del parámetro de efecto `FxInfoEmRate`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmRate.h](../source/game_sa/Fx/Blueprints/FxInfoEmRate.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmRate`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmRotation.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp) | Implementación del parámetro de efecto `FxInfoEmRotation`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmRotation.h](../source/game_sa/Fx/Blueprints/FxInfoEmRotation.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmRotation`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmSize.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp) | Implementación del parámetro de efecto `FxInfoEmSize`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmSize.h](../source/game_sa/Fx/Blueprints/FxInfoEmSize.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmSize`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmSpeed.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp) | Implementación del parámetro de efecto `FxInfoEmSpeed`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmSpeed.h](../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmSpeed`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmWeather.cpp](../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp) | Implementación del parámetro de efecto `FxInfoEmWeather`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoEmWeather.h](../source/game_sa/Fx/Blueprints/FxInfoEmWeather.h) | Declaraciones y tipos del parámetro de efecto `FxInfoEmWeather`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFlat.cpp](../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp) | Implementación del parámetro de efecto `FxInfoFlat`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFlat.h](../source/game_sa/Fx/Blueprints/FxInfoFlat.h) | Declaraciones y tipos del parámetro de efecto `FxInfoFlat`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFloat.cpp](../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp) | Implementación del parámetro de efecto `FxInfoFloat`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFloat.h](../source/game_sa/Fx/Blueprints/FxInfoFloat.h) | Declaraciones y tipos del parámetro de efecto `FxInfoFloat`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoForce.cpp](../source/game_sa/Fx/Blueprints/FxInfoForce.cpp) | Implementación del parámetro de efecto `FxInfoForce`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoForce.h](../source/game_sa/Fx/Blueprints/FxInfoForce.h) | Declaraciones y tipos del parámetro de efecto `FxInfoForce`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFriction.cpp](../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp) | Implementación del parámetro de efecto `FxInfoFriction`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoFriction.h](../source/game_sa/Fx/Blueprints/FxInfoFriction.h) | Declaraciones y tipos del parámetro de efecto `FxInfoFriction`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoGroundCollide.cpp](../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp) | Implementación del parámetro de efecto `FxInfoGroundCollide`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoGroundCollide.h](../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.h) | Declaraciones y tipos del parámetro de efecto `FxInfoGroundCollide`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoHeatHaze.cpp](../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp) | Implementación del parámetro de efecto `FxInfoHeatHaze`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoHeatHaze.h](../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.h) | Declaraciones y tipos del parámetro de efecto `FxInfoHeatHaze`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoJitter.cpp](../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp) | Implementación del parámetro de efecto `FxInfoJitter`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoJitter.h](../source/game_sa/Fx/Blueprints/FxInfoJitter.h) | Declaraciones y tipos del parámetro de efecto `FxInfoJitter`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoNoise.cpp](../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp) | Implementación del parámetro de efecto `FxInfoNoise`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoNoise.h](../source/game_sa/Fx/Blueprints/FxInfoNoise.h) | Declaraciones y tipos del parámetro de efecto `FxInfoNoise`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoRotate.h](../source/game_sa/Fx/Blueprints/FxInfoRotate.h) | Declaraciones y tipos del parámetro de efecto `FxInfoRotate`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoRotateOffset.h](../source/game_sa/Fx/Blueprints/FxInfoRotateOffset.h) | Declaraciones y tipos del parámetro de efecto `FxInfoRotateOffset`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoRotSpeed.cpp](../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp) | Implementación del parámetro de efecto `FxInfoRotSpeed`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoRotSpeed.h](../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.h) | Declaraciones y tipos del parámetro de efecto `FxInfoRotSpeed`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSelfLit.cpp](../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp) | Implementación del parámetro de efecto `FxInfoSelfLit`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSelfLit.h](../source/game_sa/Fx/Blueprints/FxInfoSelfLit.h) | Declaraciones y tipos del parámetro de efecto `FxInfoSelfLit`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSize.cpp](../source/game_sa/Fx/Blueprints/FxInfoSize.cpp) | Implementación del parámetro de efecto `FxInfoSize`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSize.h](../source/game_sa/Fx/Blueprints/FxInfoSize.h) | Declaraciones y tipos del parámetro de efecto `FxInfoSize`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSmoke.cpp](../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp) | Implementación del parámetro de efecto `FxInfoSmoke`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSmoke.h](../source/game_sa/Fx/Blueprints/FxInfoSmoke.h) | Declaraciones y tipos del parámetro de efecto `FxInfoSmoke`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSpriteRect.cpp](../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp) | Implementación del parámetro de efecto `FxInfoSpriteRect`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoSpriteRect.h](../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.h) | Declaraciones y tipos del parámetro de efecto `FxInfoSpriteRect`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoTexCoords.h](../source/game_sa/Fx/Blueprints/FxInfoTexCoords.h) | Declaraciones y tipos del parámetro de efecto `FxInfoTexCoords`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoTrail.cpp](../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp) | Implementación del parámetro de efecto `FxInfoTrail`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoTrail.h](../source/game_sa/Fx/Blueprints/FxInfoTrail.h) | Declaraciones y tipos del parámetro de efecto `FxInfoTrail`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoUnderwater.cpp](../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp) | Implementación del parámetro de efecto `FxInfoUnderwater`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoUnderwater.h](../source/game_sa/Fx/Blueprints/FxInfoUnderwater.h) | Declaraciones y tipos del parámetro de efecto `FxInfoUnderwater`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoWind.cpp](../source/game_sa/Fx/Blueprints/FxInfoWind.cpp) | Implementación del parámetro de efecto `FxInfoWind`, utilizado para definir propiedades de partículas. |
| — | — | [Fx/Blueprints/FxInfoWind.h](../source/game_sa/Fx/Blueprints/FxInfoWind.h) | Declaraciones y tipos del parámetro de efecto `FxInfoWind`, utilizado para definir propiedades de partículas. |

</details>

<details>
<summary>Fx/Blueprints/Interp/ — 10 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo.cpp](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.cpp) | Implementación de la interpolación `FxInterpInfo` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo.h](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h) | Declaraciones y tipos de la interpolación `FxInterpInfo` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo255.cpp](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) | Implementación de la interpolación `FxInterpInfo255` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo255.h](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.h) | Declaraciones y tipos de la interpolación `FxInterpInfo255` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo32.cpp](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) | Implementación de la interpolación `FxInterpInfo32` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfo32.h](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.h) | Declaraciones y tipos de la interpolación `FxInterpInfo32` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfoFloat.cpp](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) | Implementación de la interpolación `FxInterpInfoFloat` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfoFloat.h](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.h) | Declaraciones y tipos de la interpolación `FxInterpInfoFloat` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfoU255.cpp](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) | Implementación de la interpolación `FxInterpInfoU255` de valores de efectos a lo largo del tiempo. |
| — | — | [Fx/Blueprints/Interp/FxInterpInfoU255.h](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.h) | Declaraciones y tipos de la interpolación `FxInterpInfoU255` de valores de efectos a lo largo del tiempo. |

</details>

<details>
<summary>Handling/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Handling/HandlingDataType.h](../source/game_sa/Handling/HandlingDataType.h) | Declaraciones y tipos de identificación de categorías de datos de conducción. |

</details>

<details>
<summary>Interior/ — 19 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Interior/Furniture_c.cpp](../source/game_sa/Interior/Furniture_c.cpp) | Implementación de `Furniture_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/Furniture_c.h](../source/game_sa/Interior/Furniture_c.h) | Declaraciones y tipos de `Furniture_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureEntity_c.h](../source/game_sa/Interior/FurnitureEntity_c.h) | Declaraciones y tipos de `FurnitureEntity_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureGroup_c.cpp](../source/game_sa/Interior/FurnitureGroup_c.cpp) | Implementación de `FurnitureGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureGroup_c.h](../source/game_sa/Interior/FurnitureGroup_c.h) | Declaraciones y tipos de `FurnitureGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureItem.h](../source/game_sa/Interior/FurnitureItem.h) | Declaraciones y tipos de `FurnitureItem`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureManager_c.cpp](../source/game_sa/Interior/FurnitureManager_c.cpp) | Implementación de `FurnitureManager_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureManager_c.h](../source/game_sa/Interior/FurnitureManager_c.h) | Declaraciones y tipos de `FurnitureManager_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureSubGroup_c.cpp](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) | Implementación de `FurnitureSubGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/FurnitureSubGroup_c.h](../source/game_sa/Interior/FurnitureSubGroup_c.h) | Declaraciones y tipos de `FurnitureSubGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/Interior_c.cpp](../source/game_sa/Interior/Interior_c.cpp) | Implementación de `Interior_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/Interior_c.h](../source/game_sa/Interior/Interior_c.h) | Declaraciones y tipos de `Interior_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorEffectInfo_t.h](../source/game_sa/Interior/InteriorEffectInfo_t.h) | Declaraciones y tipos de `InteriorEffectInfo_t`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorGroup_c.cpp](../source/game_sa/Interior/InteriorGroup_c.cpp) | Implementación de `InteriorGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorGroup_c.h](../source/game_sa/Interior/InteriorGroup_c.h) | Declaraciones y tipos de `InteriorGroup_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorInfo_t.h](../source/game_sa/Interior/InteriorInfo_t.h) | Declaraciones y tipos de `InteriorInfo_t`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorManager_c.cpp](../source/game_sa/Interior/InteriorManager_c.cpp) | Implementación de `InteriorManager_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorManager_c.h](../source/game_sa/Interior/InteriorManager_c.h) | Declaraciones y tipos de `InteriorManager_c`: mobiliario, distribución o puntos de actividad de interiores. |
| — | — | [Interior/InteriorObject.h](../source/game_sa/Interior/InteriorObject.h) | Declaraciones y tipos de `InteriorObject`: mobiliario, distribución o puntos de actividad de interiores. |

</details>

<details>
<summary>Memory/ — 6 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Memory/CommonSize.cpp](../source/game_sa/Memory/CommonSize.cpp) | Implementación de `CommonSize`: administración de bloques de memoria y espacio libre. |
| — | — | [Memory/CommonSize.h](../source/game_sa/Memory/CommonSize.h) | Declaraciones y tipos de `CommonSize`: administración de bloques de memoria y espacio libre. |
| — | — | [Memory/HeapBlockDesc.cpp](../source/game_sa/Memory/HeapBlockDesc.cpp) | Implementación de `HeapBlockDesc`: administración de bloques de memoria y espacio libre. |
| — | — | [Memory/HeapBlockDesc.h](../source/game_sa/Memory/HeapBlockDesc.h) | Declaraciones y tipos de `HeapBlockDesc`: administración de bloques de memoria y espacio libre. |
| — | — | [Memory/HeapFreeBlockDesc.cpp](../source/game_sa/Memory/HeapFreeBlockDesc.cpp) | Implementación de `HeapFreeBlockDesc`: administración de bloques de memoria y espacio libre. |
| — | — | [Memory/HeapFreeBlockDesc.h](../source/game_sa/Memory/HeapFreeBlockDesc.h) | Declaraciones y tipos de `HeapFreeBlockDesc`: administración de bloques de memoria y espacio libre. |

</details>

<details>
<summary>Models/ — 10 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Models/AtomicModelInfo.cpp](../source/game_sa/Models/AtomicModelInfo.cpp) | Implementación de los metadatos de modelo `AtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/AtomicModelInfo.h](../source/game_sa/Models/AtomicModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `AtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/DamageAtomicModelInfo.cpp](../source/game_sa/Models/DamageAtomicModelInfo.cpp) | Implementación de los metadatos de modelo `DamageAtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/DamageAtomicModelInfo.h](../source/game_sa/Models/DamageAtomicModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `DamageAtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/LodAtomicModelInfo.cpp](../source/game_sa/Models/LodAtomicModelInfo.cpp) | Implementación de los metadatos de modelo `LodAtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/LodAtomicModelInfo.h](../source/game_sa/Models/LodAtomicModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `LodAtomicModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/LodTimeModelInfo.cpp](../source/game_sa/Models/LodTimeModelInfo.cpp) | Implementación de los metadatos de modelo `LodTimeModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/LodTimeModelInfo.h](../source/game_sa/Models/LodTimeModelInfo.h) | Declaraciones y tipos de los metadatos de modelo `LodTimeModelInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/TimeInfo.cpp](../source/game_sa/Models/TimeInfo.cpp) | Implementación de los metadatos de modelo `TimeInfo`, utilizados para cargar y representar tipos de objetos 3D. |
| — | — | [Models/TimeInfo.h](../source/game_sa/Models/TimeInfo.h) | Declaraciones y tipos de los metadatos de modelo `TimeInfo`, utilizados para cargar y representar tipos de objetos 3D. |

</details>

<details>
<summary>Pipelines/ — 1 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Pipelines/PipelinesCommon.hpp](../source/game_sa/Pipelines/PipelinesCommon.hpp) | Declaraciones y tipos de `PipelinesCommon`: procesamiento gráfico especializado de geometría y materiales. |

</details>

<details>
<summary>Pipelines/CustomBuilding/ — 6 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) | Implementación de `CustomBuildingDNPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomBuilding/CustomBuildingDNPipeline.h](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h) | Declaraciones y tipos de `CustomBuildingDNPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomBuilding/CustomBuildingPipeline.cpp](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) | Implementación de `CustomBuildingPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomBuilding/CustomBuildingPipeline.h](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.h) | Declaraciones y tipos de `CustomBuildingPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomBuilding/CustomBuildingRenderer.cpp](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) | Implementación de `CustomBuildingRenderer`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomBuilding/CustomBuildingRenderer.h](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.h) | Declaraciones y tipos de `CustomBuildingRenderer`: procesamiento gráfico especializado de geometría y materiales. |

</details>

<details>
<summary>Pipelines/CustomCarEnvMap/ — 5 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) | Implementación de `CustomCarEnvMapPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h) | Declaraciones y tipos de `CustomCarEnvMapPipeline`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvAtm.cpp](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvAtm.cpp) | Implementación de `CustomCarEnvMapPipeline_EnvAtm`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp) | Implementación de `CustomCarEnvMapPipeline_EnvMat`: procesamiento gráfico especializado de geometría y materiales. |
| — | — | [Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_SpecMat.cpp](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_SpecMat.cpp) | Implementación de `CustomCarEnvMapPipeline_SpecMat`: procesamiento gráfico especializado de geometría y materiales. |

</details>

<details>
<summary>Plant/ — 8 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plant/PlantColEntEntry.cpp](../source/game_sa/Plant/PlantColEntEntry.cpp) | Implementación de `PlantColEntEntry`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/PlantColEntEntry.h](../source/game_sa/Plant/PlantColEntEntry.h) | Declaraciones y tipos de `PlantColEntEntry`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/PlantLocTri.cpp](../source/game_sa/Plant/PlantLocTri.cpp) | Implementación de `PlantLocTri`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/PlantLocTri.h](../source/game_sa/Plant/PlantLocTri.h) | Declaraciones y tipos de `PlantLocTri`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/ProcObjectMan.cpp](../source/game_sa/Plant/ProcObjectMan.cpp) | Implementación de `ProcObjectMan`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/ProcObjectMan.h](../source/game_sa/Plant/ProcObjectMan.h) | Declaraciones y tipos de `ProcObjectMan`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/ProcSurfaceInfo.cpp](../source/game_sa/Plant/ProcSurfaceInfo.cpp) | Implementación de `ProcSurfaceInfo`: vegetación y colocación procedural de objetos sobre superficies. |
| — | — | [Plant/ProcSurfaceInfo.h](../source/game_sa/Plant/ProcSurfaceInfo.h) | Declaraciones y tipos de `ProcSurfaceInfo`: vegetación y colocación procedural de objetos sobre superficies. |

</details>

<details>
<summary>Plugins/BreakablePlugin/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plugins/BreakablePlugin/BreakablePlugin.cpp](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) | Implementación de `BreakablePlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| — | — | [Plugins/BreakablePlugin/BreakablePlugin.h](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h) | Declaraciones y tipos de `BreakablePlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |

</details>

<details>
<summary>Plugins/CollisionPlugin/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plugins/CollisionPlugin/CollisionPlugin.cpp](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) | Implementación de `CollisionPlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| — | — | [Plugins/CollisionPlugin/CollisionPlugin.h](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.h) | Declaraciones y tipos de `CollisionPlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |

</details>

<details>
<summary>Plugins/JPegCompressPlugin/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plugins/JPegCompressPlugin/JPegCompress.cpp](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) | Implementación de `JPegCompress`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| — | — | [Plugins/JPegCompressPlugin/JPegCompress.h](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.h) | Declaraciones y tipos de `JPegCompress`: extensión que asocia datos o comportamiento a objetos de RenderWare. |

</details>

<details>
<summary>Plugins/PipelinePlugin/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plugins/PipelinePlugin/PipelinePlugin.cpp](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) | Implementación de `PipelinePlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| — | — | [Plugins/PipelinePlugin/PipelinePlugin.h](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.h) | Declaraciones y tipos de `PipelinePlugin`: extensión que asocia datos o comportamiento a objetos de RenderWare. |

</details>

<details>
<summary>Plugins/TwoDEffectPlugin/ — 2 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Plugins/TwoDEffectPlugin/2dEffect.cpp](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) | Implementación de `2dEffect`: extensión que asocia datos o comportamiento a objetos de RenderWare. |
| — | — | [Plugins/TwoDEffectPlugin/2dEffectStream.h](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffectStream.h) | Declaraciones y tipos de `2dEffectStream`: extensión que asocia datos o comportamiento a objetos de RenderWare. |

</details>

<details>
<summary>Pools/ — 18 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Pools/BuildingPool.h](../source/game_sa/Pools/BuildingPool.h) | Declaraciones y tipos de la reserva `BuildingPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/ColModelPool.h](../source/game_sa/Pools/ColModelPool.h) | Declaraciones y tipos de la reserva `ColModelPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/DummyPool.h](../source/game_sa/Pools/DummyPool.h) | Declaraciones y tipos de la reserva `DummyPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/EntryInfoNodePool.h](../source/game_sa/Pools/EntryInfoNodePool.h) | Declaraciones y tipos de la reserva `EntryInfoNodePool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/EventPool.h](../source/game_sa/Pools/EventPool.h) | Declaraciones y tipos del evento `EventPool` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones. |
| — | — | [Pools/IplDefPool.h](../source/game_sa/Pools/IplDefPool.h) | Declaraciones y tipos de la reserva `IplDefPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/NodeRoutePool.h](../source/game_sa/Pools/NodeRoutePool.h) | Declaraciones y tipos de la reserva `NodeRoutePool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/ObjectPool.h](../source/game_sa/Pools/ObjectPool.h) | Declaraciones y tipos de la reserva `ObjectPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PatrolRoutePool.h](../source/game_sa/Pools/PatrolRoutePool.h) | Declaraciones y tipos de la reserva `PatrolRoutePool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PedAttractorPool.h](../source/game_sa/Pools/PedAttractorPool.h) | Declaraciones y tipos de la reserva `PedAttractorPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PedIntelligencePool.h](../source/game_sa/Pools/PedIntelligencePool.h) | Declaraciones y tipos de la reserva `PedIntelligencePool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PedPool.h](../source/game_sa/Pools/PedPool.h) | Declaraciones y tipos de la reserva `PedPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PointRoutePool.h](../source/game_sa/Pools/PointRoutePool.h) | Declaraciones y tipos de la reserva `PointRoutePool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PtrNodeDoubleLinkPool.h](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) | Declaraciones y tipos de la reserva `PtrNodeDoubleLinkPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/PtrNodeSingleLinkPool.h](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) | Declaraciones y tipos de la reserva `PtrNodeSingleLinkPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/TaskAllocatorPool.h](../source/game_sa/Pools/TaskAllocatorPool.h) | Declaraciones y tipos de la reserva `TaskAllocatorPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/TaskPool.h](../source/game_sa/Pools/TaskPool.h) | Declaraciones y tipos de la reserva `TaskPool`, que administra instancias reutilizables de un tipo de objeto. |
| — | — | [Pools/VehiclePool.h](../source/game_sa/Pools/VehiclePool.h) | Declaraciones y tipos de la reserva `VehiclePool`, que administra instancias reutilizables de un tipo de objeto. |

</details>

<details>
<summary>Ragdoll/ — 9 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Ragdoll/BoneInfo.h](../source/game_sa/Ragdoll/BoneInfo.h) | Declaraciones y tipos del componente `BoneInfo` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/BoneNode.cpp](../source/game_sa/Ragdoll/BoneNode.cpp) | Implementación del componente `BoneNode` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/BoneNode.h](../source/game_sa/Ragdoll/BoneNode.h) | Declaraciones y tipos del componente `BoneNode` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/BoneNodeManager.cpp](../source/game_sa/Ragdoll/BoneNodeManager.cpp) | Implementación del componente `BoneNodeManager` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/BoneNodeManager.h](../source/game_sa/Ragdoll/BoneNodeManager.h) | Declaraciones y tipos del componente `BoneNodeManager` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/IKChain.cpp](../source/game_sa/Ragdoll/IKChain.cpp) | Implementación del componente `IKChain` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/IKChain.h](../source/game_sa/Ragdoll/IKChain.h) | Declaraciones y tipos del componente `IKChain` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/IKChainManager.cpp](../source/game_sa/Ragdoll/IKChainManager.cpp) | Implementación del componente `IKChainManager` de huesos o cadenas de cinemática inversa. |
| — | — | [Ragdoll/IKChainManager.h](../source/game_sa/Ragdoll/IKChainManager.h) | Declaraciones y tipos del componente `IKChainManager` de huesos o cadenas de cinemática inversa. |

</details>

<details>
<summary>RenderWare/ — 7 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [RenderWare/D3DIndexDataBuffer.cpp](../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp) | Implementación de `D3DIndexDataBuffer`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/D3DIndexDataBuffer.h](../source/game_sa/RenderWare/D3DIndexDataBuffer.h) | Declaraciones y tipos de `D3DIndexDataBuffer`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/D3DResourceSystem.cpp](../source/game_sa/RenderWare/D3DResourceSystem.cpp) | Implementación de `D3DResourceSystem`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/D3DResourceSystem.h](../source/game_sa/RenderWare/D3DResourceSystem.h) | Declaraciones y tipos de `D3DResourceSystem`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/D3DTextureBuffer.cpp](../source/game_sa/RenderWare/D3DTextureBuffer.cpp) | Implementación de `D3DTextureBuffer`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/D3DTextureBuffer.h](../source/game_sa/RenderWare/D3DTextureBuffer.h) | Declaraciones y tipos de `D3DTextureBuffer`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/RenderWare.h](../source/game_sa/RenderWare/RenderWare.h) | Declaraciones y tipos de `RenderWare`: interfaz, recursos o utilidades del motor gráfico RenderWare. |

</details>

<details>
<summary>RenderWare/rw/ — 69 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [RenderWare/rw/rpadc.h](../source/game_sa/RenderWare/rw/rpadc.h) | Declaraciones y tipos de `rpadc`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpanisot.cpp](../source/game_sa/RenderWare/rw/rpanisot.cpp) | Implementación de `rpanisot`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpcollis.h](../source/game_sa/RenderWare/rw/rpcollis.h) | Declaraciones y tipos de `rpcollis`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpcriter.h](../source/game_sa/RenderWare/rw/rpcriter.h) | Declaraciones y tipos de `rpcriter`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpdbgerr.cpp](../source/game_sa/RenderWare/rw/rpdbgerr.cpp) | Implementación de `rpdbgerr`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpdbgerr.h](../source/game_sa/RenderWare/rw/rpdbgerr.h) | Declaraciones y tipos de `rpdbgerr`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpdmorph.h](../source/game_sa/RenderWare/rw/rpdmorph.h) | Declaraciones y tipos de `rpdmorph`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpenv.h](../source/game_sa/RenderWare/rw/rpenv.h) | Declaraciones y tipos de `rpenv`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rperror.h](../source/game_sa/RenderWare/rw/rperror.h) | Declaraciones y tipos de `rperror`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rphanim.cpp](../source/game_sa/RenderWare/rw/rphanim.cpp) | Implementación de `rphanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rplodatm.h](../source/game_sa/RenderWare/rw/rplodatm.h) | Declaraciones y tipos de `rplodatm`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rplogo.h](../source/game_sa/RenderWare/rw/rplogo.h) | Declaraciones y tipos de `rplogo`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpltmap.h](../source/game_sa/RenderWare/rw/rpltmap.h) | Declaraciones y tipos de `rpltmap`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpmatfx.cpp](../source/game_sa/RenderWare/rw/rpmatfx.cpp) | Implementación de `rpmatfx`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpmipkl.h](../source/game_sa/RenderWare/rw/rpmipkl.h) | Declaraciones y tipos de `rpmipkl`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpmorph.h](../source/game_sa/RenderWare/rw/rpmorph.h) | Declaraciones y tipos de `rpmorph`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpnormmap.h](../source/game_sa/RenderWare/rw/rpnormmap.h) | Declaraciones y tipos de `rpnormmap`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rppatch.h](../source/game_sa/RenderWare/rw/rppatch.h) | Declaraciones y tipos de `rppatch`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpplugin.h](../source/game_sa/RenderWare/rw/rpplugin.h) | Declaraciones y tipos de `rpplugin`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpprtstd.h](../source/game_sa/RenderWare/rw/rpprtstd.h) | Declaraciones y tipos de `rpprtstd`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpptank.h](../source/game_sa/RenderWare/rw/rpptank.h) | Declaraciones y tipos de `rpptank`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rppvs.h](../source/game_sa/RenderWare/rw/rppvs.h) | Declaraciones y tipos de `rppvs`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rprandom.h](../source/game_sa/RenderWare/rw/rprandom.h) | Declaraciones y tipos de `rprandom`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpskin.cpp](../source/game_sa/RenderWare/rw/rpskin.cpp) | Implementación de `rpskin`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpspline.h](../source/game_sa/RenderWare/rw/rpspline.h) | Declaraciones y tipos de `rpspline`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpusrdat.h](../source/game_sa/RenderWare/rw/rpusrdat.h) | Declaraciones y tipos de `rpusrdat`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpuvanim.cpp](../source/game_sa/RenderWare/rw/rpuvanim.cpp) | Implementación de `rpuvanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpuvanim.h](../source/game_sa/RenderWare/rw/rpuvanim.h) | Declaraciones y tipos de `rpuvanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rpworld.cpp](../source/game_sa/RenderWare/rw/rpworld.cpp) | Implementación de `rpworld`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rt2d.h](../source/game_sa/RenderWare/rw/rt2d.h) | Declaraciones y tipos de `rt2d`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rt2danim.h](../source/game_sa/RenderWare/rw/rt2danim.h) | Declaraciones y tipos de `rt2danim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtanim.cpp](../source/game_sa/RenderWare/rw/rtanim.cpp) | Implementación de `rtanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtanim.h](../source/game_sa/RenderWare/rw/rtanim.h) | Declaraciones y tipos de `rtanim`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtbary.h](../source/game_sa/RenderWare/rw/rtbary.h) | Declaraciones y tipos de `rtbary`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtbezpat.h](../source/game_sa/RenderWare/rw/rtbezpat.h) | Declaraciones y tipos de `rtbezpat`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtbmp.cpp](../source/game_sa/RenderWare/rw/rtbmp.cpp) | Implementación de `rtbmp`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtcmpkey.h](../source/game_sa/RenderWare/rw/rtcmpkey.h) | Declaraciones y tipos de `rtcmpkey`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtdict.cpp](../source/game_sa/RenderWare/rw/rtdict.cpp) | Implementación de `rtdict`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtdict.h](../source/game_sa/RenderWare/rw/rtdict.h) | Declaraciones y tipos de `rtdict`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtfsyst.h](../source/game_sa/RenderWare/rw/rtfsyst.h) | Declaraciones y tipos de `rtfsyst`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtgcond.h](../source/game_sa/RenderWare/rw/rtgcond.h) | Declaraciones y tipos de `rtgcond`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtgncpip.h](../source/game_sa/RenderWare/rw/rtgncpip.h) | Declaraciones y tipos de `rtgncpip`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtimport.h](../source/game_sa/RenderWare/rw/rtimport.h) | Declaraciones y tipos de `rtimport`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtintsec.h](../source/game_sa/RenderWare/rw/rtintsec.h) | Declaraciones y tipos de `rtintsec`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtltmap.h](../source/game_sa/RenderWare/rw/rtltmap.h) | Declaraciones y tipos de `rtltmap`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtltmapcnv.h](../source/game_sa/RenderWare/rw/rtltmapcnv.h) | Declaraciones y tipos de `rtltmapcnv`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtmipk.h](../source/game_sa/RenderWare/rw/rtmipk.h) | Declaraciones y tipos de `rtmipk`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtnormmap.h](../source/game_sa/RenderWare/rw/rtnormmap.h) | Declaraciones y tipos de `rtnormmap`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtpick.h](../source/game_sa/RenderWare/rw/rtpick.h) | Declaraciones y tipos de `rtpick`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtpitexd.h](../source/game_sa/RenderWare/rw/rtpitexd.h) | Declaraciones y tipos de `rtpitexd`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtpng.cpp](../source/game_sa/RenderWare/rw/rtpng.cpp) | Implementación de `rtpng`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtquat.cpp](../source/game_sa/RenderWare/rw/rtquat.cpp) | Implementación de `rtquat`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtras.h](../source/game_sa/RenderWare/rw/rtras.h) | Declaraciones y tipos de `rtras`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtray.h](../source/game_sa/RenderWare/rw/rtray.h) | Declaraciones y tipos de `rtray`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtskinsp.h](../source/game_sa/RenderWare/rw/rtskinsp.h) | Declaraciones y tipos de `rtskinsp`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtslerp.cpp](../source/game_sa/RenderWare/rw/rtslerp.cpp) | Implementación de `rtslerp`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtslerp.h](../source/game_sa/RenderWare/rw/rtslerp.h) | Declaraciones y tipos de `rtslerp`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtsplpvs.h](../source/game_sa/RenderWare/rw/rtsplpvs.h) | Declaraciones y tipos de `rtsplpvs`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rttiff.h](../source/game_sa/RenderWare/rw/rttiff.h) | Declaraciones y tipos de `rttiff`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rttilerd.h](../source/game_sa/RenderWare/rw/rttilerd.h) | Declaraciones y tipos de `rttilerd`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rttoc.h](../source/game_sa/RenderWare/rw/rttoc.h) | Declaraciones y tipos de `rttoc`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtvcat.h](../source/game_sa/RenderWare/rw/rtvcat.h) | Declaraciones y tipos de `rtvcat`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtwing.h](../source/game_sa/RenderWare/rw/rtwing.h) | Declaraciones y tipos de `rtwing`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rtworld.h](../source/game_sa/RenderWare/rw/rtworld.h) | Declaraciones y tipos de `rtworld`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rwcore.cpp](../source/game_sa/RenderWare/rw/rwcore.cpp) | Implementación de `rwcore`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rwplcore.cpp](../source/game_sa/RenderWare/rw/rwplcore.cpp) | Implementación de `rwplcore`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rwtexdict.cpp](../source/game_sa/RenderWare/rw/rwtexdict.cpp) | Implementación de `rwtexdict`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rwtexdict.h](../source/game_sa/RenderWare/rw/rwtexdict.h) | Declaraciones y tipos de `rwtexdict`: interfaz, recursos o utilidades del motor gráfico RenderWare. |
| — | — | [RenderWare/rw/rwversion.h](../source/game_sa/RenderWare/rw/rwversion.h) | Declaraciones y tipos de `rwversion`: interfaz, recursos o utilidades del motor gráfico RenderWare. |

</details>

<details>
<summary>Scripts/ — 9 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Scripts/OpcodeResult.h](../source/game_sa/Scripts/OpcodeResult.h) | Declaraciones y tipos de resultados y control de flujo al ejecutar un opcode. |
| — | — | [Scripts/RunningScript.cpp](../source/game_sa/Scripts/RunningScript.cpp) | Implementación de ejecución y administración de scripts de misiones. |
| — | — | [Scripts/RunningScript.h](../source/game_sa/Scripts/RunningScript.h) | Declaraciones y tipos de ejecución y administración de scripts de misiones. |
| — | — | [Scripts/SCMChunks.hpp](../source/game_sa/Scripts/SCMChunks.hpp) | Declaraciones y tipos de estructuras de bloques del formato de scripts SCM. |
| — | — | [Scripts/Scripted2dEffects.cpp](../source/game_sa/Scripts/Scripted2dEffects.cpp) | Implementación de efectos 2D y puntos de interacción controlados desde scripts. |
| — | — | [Scripts/Scripted2dEffects.h](../source/game_sa/Scripts/Scripted2dEffects.h) | Declaraciones y tipos de efectos 2D y puntos de interacción controlados desde scripts. |
| — | — | [Scripts/ScriptParam.h](../source/game_sa/Scripts/ScriptParam.h) | Declaraciones y tipos de representación de los parámetros de instrucciones de scripts. |
| — | — | [Scripts/TheScripts.cpp](../source/game_sa/Scripts/TheScripts.cpp) | Implementación de ejecución y administración de scripts de misiones. |
| — | — | [Scripts/TheScripts.h](../source/game_sa/Scripts/TheScripts.h) | Declaraciones y tipos de ejecución y administración de scripts de misiones. |

</details>

<details>
<summary>Scripts/CommandParser/ — 5 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Scripts/CommandParser/LUTGenerator.hpp](../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp) | Declaraciones y tipos de `LUTGenerator`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts. |
| — | — | [Scripts/CommandParser/Parser.hpp](../source/game_sa/Scripts/CommandParser/Parser.hpp) | Declaraciones y tipos de `Parser`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts. |
| — | — | [Scripts/CommandParser/ReadArg.hpp](../source/game_sa/Scripts/CommandParser/ReadArg.hpp) | Declaraciones y tipos de `ReadArg`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts. |
| — | — | [Scripts/CommandParser/StoreArg.hpp](../source/game_sa/Scripts/CommandParser/StoreArg.hpp) | Declaraciones y tipos de `StoreArg`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts. |
| — | — | [Scripts/CommandParser/Utility.hpp](../source/game_sa/Scripts/CommandParser/Utility.hpp) | Declaraciones y tipos de `Utility`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts. |

</details>

<details>
<summary>Scripts/Commands/ — 25 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Scripts/Commands/BasicCommands.cpp](../source/game_sa/Scripts/Commands/BasicCommands.cpp) | Implementación de los comandos de script `BasicCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CameraCommands.cpp](../source/game_sa/Scripts/Commands/CameraCommands.cpp) | Implementación de los comandos de script `CameraCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CharacterCommands.cpp](../source/game_sa/Scripts/Commands/CharacterCommands.cpp) | Implementación de los comandos de script `CharacterCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ClockCommands.cpp](../source/game_sa/Scripts/Commands/ClockCommands.cpp) | Implementación de los comandos de script `ClockCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/Commands.hpp](../source/game_sa/Scripts/Commands/Commands.hpp) | Declaraciones y tipos de los comandos de script `Commands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ComparasionCommands.cpp](../source/game_sa/Scripts/Commands/ComparasionCommands.cpp) | Implementación de los comandos de script `ComparasionCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ConversationCommands.cpp](../source/game_sa/Scripts/Commands/ConversationCommands.cpp) | Implementación de los comandos de script `ConversationCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/DrawCommands.cpp](../source/game_sa/Scripts/Commands/DrawCommands.cpp) | Implementación de los comandos de script `DrawCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/GameCommands.cpp](../source/game_sa/Scripts/Commands/GameCommands.cpp) | Implementación de los comandos de script `GameCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/GenericCommands.cpp](../source/game_sa/Scripts/Commands/GenericCommands.cpp) | Implementación de los comandos de script `GenericCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/MathCommands.cpp](../source/game_sa/Scripts/Commands/MathCommands.cpp) | Implementación de los comandos de script `MathCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/MissionCommands.cpp](../source/game_sa/Scripts/Commands/MissionCommands.cpp) | Implementación de los comandos de script `MissionCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ObjectCommands.cpp](../source/game_sa/Scripts/Commands/ObjectCommands.cpp) | Implementación de los comandos de script `ObjectCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/PadCommands.cpp](../source/game_sa/Scripts/Commands/PadCommands.cpp) | Implementación de los comandos de script `PadCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/PathCommands.cpp](../source/game_sa/Scripts/Commands/PathCommands.cpp) | Implementación de los comandos de script `PathCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/PedCommands.cpp](../source/game_sa/Scripts/Commands/PedCommands.cpp) | Implementación de los comandos de script `PedCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/PlayerCommands.cpp](../source/game_sa/Scripts/Commands/PlayerCommands.cpp) | Implementación de los comandos de script `PlayerCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ScriptCommands.cpp](../source/game_sa/Scripts/Commands/ScriptCommands.cpp) | Implementación de identificadores de instrucciones u opcodes de scripts. |
| — | — | [Scripts/Commands/SequenceCommands.cpp](../source/game_sa/Scripts/Commands/SequenceCommands.cpp) | Implementación de los comandos de script `SequenceCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/StatCommands.cpp](../source/game_sa/Scripts/Commands/StatCommands.cpp) | Implementación de los comandos de script `StatCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/TextCommands.cpp](../source/game_sa/Scripts/Commands/TextCommands.cpp) | Implementación de los comandos de script `TextCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/UnusedCommands.cpp](../source/game_sa/Scripts/Commands/UnusedCommands.cpp) | Implementación de los comandos de script `UnusedCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/UtilityCommands.cpp](../source/game_sa/Scripts/Commands/UtilityCommands.cpp) | Implementación de los comandos de script `UtilityCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/VehicleCommands.cpp](../source/game_sa/Scripts/Commands/VehicleCommands.cpp) | Implementación de los comandos de script `VehicleCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/ZoneCommands.cpp](../source/game_sa/Scripts/Commands/ZoneCommands.cpp) | Implementación de los comandos de script `ZoneCommands`, agrupados por su ámbito de actuación. |

</details>

<details>
<summary>Scripts/Commands/CLEO/ — 12 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp) | Implementación de los comandos de script `CLEOAudioStreamCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOCharacterCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOCharacterCommands.cpp) | Implementación de los comandos de script `CLEOCharacterCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOCommands.hpp](../source/game_sa/Scripts/Commands/CLEO/CLEOCommands.hpp) | Declaraciones y tipos de los comandos de script `CLEOCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp) | Implementación de los comandos de script `CLEODynamicLibraryCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOFsCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOFsCommands.cpp) | Implementación de los comandos de script `CLEOFsCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOGameCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOGameCommands.cpp) | Implementación de los comandos de script `CLEOGameCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOGenericCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOGenericCommands.cpp) | Implementación de los comandos de script `CLEOGenericCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOMemoryCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOMemoryCommands.cpp) | Implementación de los comandos de script `CLEOMemoryCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOPadCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOPadCommands.cpp) | Implementación de los comandos de script `CLEOPadCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOScriptCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOScriptCommands.cpp) | Implementación de los comandos de script `CLEOScriptCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOVehicleCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOVehicleCommands.cpp) | Implementación de los comandos de script `CLEOVehicleCommands`, agrupados por su ámbito de actuación. |
| — | — | [Scripts/Commands/CLEO/CLEOWorldCommands.cpp](../source/game_sa/Scripts/Commands/CLEO/CLEOWorldCommands.cpp) | Implementación de los comandos de script `CLEOWorldCommands`, agrupados por su ámbito de actuación. |

</details>

<details>
<summary>Tasks/ — 21 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/PedScriptedTaskRecord.cpp](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) | Implementación de `PedScriptedTaskRecord`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/PedScriptedTaskRecord.h](../source/game_sa/Tasks/PedScriptedTaskRecord.h) | Declaraciones y tipos de `PedScriptedTaskRecord`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/ScriptedBrainTaskStore.cpp](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) | Implementación de `ScriptedBrainTaskStore`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/ScriptedBrainTaskStore.h](../source/game_sa/Tasks/ScriptedBrainTaskStore.h) | Declaraciones y tipos de `ScriptedBrainTaskStore`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Task.cpp](../source/game_sa/Tasks/Task.cpp) | Implementación de `Task`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Task.h](../source/game_sa/Tasks/Task.h) | Declaraciones y tipos de `Task`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskCategories.cpp](../source/game_sa/Tasks/TaskCategories.cpp) | Implementación de `TaskCategories`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskCategories.h](../source/game_sa/Tasks/TaskCategories.h) | Declaraciones y tipos de `TaskCategories`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskComplex.h](../source/game_sa/Tasks/TaskComplex.h) | Declaraciones y tipos de la tarea compleja `TaskComplex`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskComplexSequence.cpp](../source/game_sa/Tasks/TaskComplexSequence.cpp) | Implementación de la tarea compleja `TaskComplexSequence`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskComplexSequence.h](../source/game_sa/Tasks/TaskComplexSequence.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSequence`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskComplexUseSequence.cpp](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) | Implementación de la tarea compleja `TaskComplexUseSequence`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskComplexUseSequence.h](../source/game_sa/Tasks/TaskComplexUseSequence.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseSequence`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskManager.cpp](../source/game_sa/Tasks/TaskManager.cpp) | Implementación de `TaskManager`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskManager.h](../source/game_sa/Tasks/TaskManager.h) | Declaraciones y tipos de `TaskManager`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskSequences.cpp](../source/game_sa/Tasks/TaskSequences.cpp) | Implementación de `TaskSequences`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskSequences.h](../source/game_sa/Tasks/TaskSequences.h) | Declaraciones y tipos de `TaskSequences`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskSimple.h](../source/game_sa/Tasks/TaskSimple.h) | Declaraciones y tipos de la tarea simple `TaskSimple`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTimer.cpp](../source/game_sa/Tasks/TaskTimer.cpp) | Implementación de `TaskTimer`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTimer.h](../source/game_sa/Tasks/TaskTimer.h) | Declaraciones y tipos de `TaskTimer`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TickCounter.h](../source/game_sa/Tasks/TickCounter.h) | Declaraciones y tipos de `TickCounter`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |

</details>

<details>
<summary>Tasks/Allocators/ — 18 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/Allocators/TaskAllocator.cpp](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) | Implementación de `TaskAllocator`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocator.h](../source/game_sa/Tasks/Allocators/TaskAllocator.h) | Declaraciones y tipos de `TaskAllocator`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorAttack.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) | Implementación de `TaskAllocatorAttack`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorAttack.h](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h) | Declaraciones y tipos de `TaskAllocatorAttack`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillOnFoot.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) | Implementación de `TaskAllocatorKillOnFoot`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillOnFoot.h](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h) | Declaraciones y tipos de `TaskAllocatorKillOnFoot`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) | Implementación de `TaskAllocatorKillThreatsBasic`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsBasic.h](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h) | Declaraciones y tipos de `TaskAllocatorKillThreatsBasic`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) | Implementación de `TaskAllocatorKillThreatsBasicRandomGroup`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h) | Declaraciones y tipos de `TaskAllocatorKillThreatsBasicRandomGroup`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) | Implementación de `TaskAllocatorKillThreatsDriveby`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h) | Declaraciones y tipos de `TaskAllocatorKillThreatsDriveby`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) | Implementación de `TaskAllocatorPlayerCommandAttack`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h) | Declaraciones y tipos de `TaskAllocatorPlayerCommandAttack`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorPlayerCommandRetreat.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandRetreat.cpp) | Implementación de `TaskAllocatorPlayerCommandRetreat`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorPlayerCommandRetreat.h](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandRetreat.h) | Declaraciones y tipos de `TaskAllocatorPlayerCommandRetreat`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorRetreat.cpp](../source/game_sa/Tasks/Allocators/TaskAllocatorRetreat.cpp) | Implementación de `TaskAllocatorRetreat`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/TaskAllocatorRetreat.h](../source/game_sa/Tasks/Allocators/TaskAllocatorRetreat.h) | Declaraciones y tipos de `TaskAllocatorRetreat`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |

</details>

<details>
<summary>Tasks/Allocators/PedGroup/ — 9 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocator`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorChat`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorFollowAnyMeans`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorFollowLimited`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorRandom`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h)<br>[Tasks/Allocators/PedGroupDefaultTaskAllocators.h](../source/game_sa/Tasks/Allocators/PedGroupDefaultTaskAllocators.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocators`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorSitInLeaderCar`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorStandStill`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorType.h](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorType.h) | Declaraciones y tipos de `PedGroupDefaultTaskAllocatorType`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |

</details>

<details>
<summary>Tasks/TaskTypes/ — 561 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/TaskTypes/TaskComplexArrestPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) | Implementación de la tarea compleja `TaskComplexArrestPed`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexArrestPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexArrestPed`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) | Implementación de la tarea compleja `TaskComplexAttractorPartnerWait`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.h) | Declaraciones y tipos de la tarea compleja `TaskComplexAttractorPartnerWait`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) | Implementación de la tarea compleja `TaskComplexAvoidOtherPedWhileWandering`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h) | Declaraciones y tipos de la tarea compleja `TaskComplexAvoidOtherPedWhileWandering`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexBeCop.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) | Implementación de la tarea compleja `TaskComplexBeCop`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexBeCop.h](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h) | Declaraciones y tipos de la tarea compleja `TaskComplexBeCop`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexBeInCouple.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) | Implementación de la tarea compleja `TaskComplexBeInCouple`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexBeInCouple.h](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h) | Declaraciones y tipos de la tarea compleja `TaskComplexBeInCouple`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexBeInGroup.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) | Implementación de la tarea compleja `TaskComplexBeInGroup`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexBeInGroup.h](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.h) | Declaraciones y tipos de la tarea compleja `TaskComplexBeInGroup`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexBeStill.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) | Implementación de la tarea compleja `TaskComplexBeStill`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexBeStill.h](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h) | Declaraciones y tipos de la tarea compleja `TaskComplexBeStill`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarDrive.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) | Implementación de la tarea compleja `TaskComplexCarDrive`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarDrive.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCarDrive`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarDriveMission.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) | Implementación de la tarea compleja `TaskComplexCarDriveMission`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarDriveMission.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCarDriveMission`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) | Implementación de la tarea compleja `TaskComplexCarSlowBeDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCarSlowBeDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) | Implementación de la tarea compleja `TaskComplexCarSlowBeDraggedOutAndStandUp`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCarSlowBeDraggedOutAndStandUp`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexChat.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) | Implementación de la tarea compleja `TaskComplexChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexChat.h](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.h) | Declaraciones y tipos de la tarea compleja `TaskComplexChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexClimb.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) | Implementación de la tarea compleja `TaskComplexClimb`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexClimb.h](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.h) | Declaraciones y tipos de la tarea compleja `TaskComplexClimb`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexCopInCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) | Implementación de la tarea compleja `TaskComplexCopInCar`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexCopInCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCopInCar`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) | Implementación de la tarea compleja `TaskComplexCrossRoadLookAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.h](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.h) | Declaraciones y tipos de la tarea compleja `TaskComplexCrossRoadLookAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) | Implementación de la tarea compleja `TaskComplexDestroyCar`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDestroyCar`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) | Implementación de la tarea compleja `TaskComplexDestroyCarArmed`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCarArmed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDestroyCarArmed`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) | Implementación de la tarea compleja `TaskComplexDestroyCarMelee`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDestroyCarMelee.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDestroyCarMelee`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexDie.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) | Implementación de la tarea compleja `TaskComplexDie`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexDie.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDie`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexDieInCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) | Implementación de la tarea compleja `TaskComplexDieInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexDieInCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDieInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) | Implementación de la tarea compleja `TaskComplexDiveFromAttachedEntityAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDiveFromAttachedEntityAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) | Implementación de la tarea compleja `TaskComplexDragPedFromCar`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDragPedFromCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDragPedFromCar`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) | Implementación de la tarea compleja `TaskComplexDriveFireTruck`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveFireTruck.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDriveFireTruck`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) | Implementación de la tarea compleja `TaskComplexDrivePointRoute`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDrivePointRoute.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDrivePointRoute`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveToPoint.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) | Implementación de la tarea compleja `TaskComplexDriveToPoint`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveToPoint.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDriveToPoint`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveWander.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) | Implementación de la tarea compleja `TaskComplexDriveWander`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexDriveWander.h](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h) | Declaraciones y tipos de la tarea compleja `TaskComplexDriveWander`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) | Implementación de la tarea compleja `TaskComplexEnterAnyCarAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterAnyCarAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) | Implementación de la tarea compleja `TaskComplexEnterBoatAsDriver`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterBoatAsDriver`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) | Implementación de la tarea compleja `TaskComplexEnterCar`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCar`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) | Implementación de la tarea compleja `TaskComplexEnterCarAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCarAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) | Implementación de la tarea compleja `TaskComplexEnterCarAsDriverTimed`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCarAsDriverTimed`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) | Implementación de la tarea compleja `TaskComplexEnterCarAsPassenger`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCarAsPassenger`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) | Implementación de la tarea compleja `TaskComplexEnterCarAsPassengerTimed`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCarAsPassengerTimed`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) | Implementación de la tarea compleja `TaskComplexEnterCarAsPassengerWait`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEnterCarAsPassengerWait`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveCower.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) | Implementación de la tarea compleja `TaskComplexEvasiveCower`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveCower.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEvasiveCower`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) | Implementación de la tarea compleja `TaskComplexEvasiveDiveAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEvasiveDiveAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveStep.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) | Implementación de la tarea compleja `TaskComplexEvasiveStep`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskComplexEvasiveStep.h](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h) | Declaraciones y tipos de la tarea compleja `TaskComplexEvasiveStep`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskComplexExtinguishFires.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) | Implementación de la tarea compleja `TaskComplexExtinguishFires`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskComplexExtinguishFires.h](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.h) | Declaraciones y tipos de la tarea compleja `TaskComplexExtinguishFires`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskComplexFacial.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) | Implementación de la tarea compleja `TaskComplexFacial`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskComplexFacial.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFacial`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) | Implementación de la tarea compleja `TaskComplexFallAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexFallAndGetUp.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFallAndGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexFallToDeath.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) | Implementación de la tarea compleja `TaskComplexFallToDeath`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexFallToDeath.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFallToDeath`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) | Implementación de la tarea compleja `TaskComplexFleeAnyMeans`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeAnyMeans.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFleeAnyMeans`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) | Implementación de la tarea compleja `TaskComplexFleeEntity`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeEntity.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFleeEntity`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleePoint.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) | Implementación de la tarea compleja `TaskComplexFleePoint`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleePoint.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFleePoint`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeShooting.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) | Implementación de la tarea compleja `TaskComplexFleeShooting`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexFleeShooting.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFleeShooting`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) | Implementación de la tarea compleja `TaskComplexFollowLeaderAnyMeans`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFollowLeaderAnyMeans`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) | Implementación de la tarea compleja `TaskComplexFollowLeaderInFormation`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFollowLeaderInFormation`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) | Implementación de la tarea compleja `TaskComplexFollowNodeRoute`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowNodeRoute.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFollowNodeRoute`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) | Implementación de la tarea compleja `TaskComplexFollowPedFootsteps`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFollowPedFootsteps`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) | Implementación de la tarea compleja `TaskComplexFollowPointRoute`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexFollowPointRoute.h](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.h) | Declaraciones y tipos de la tarea compleja `TaskComplexFollowPointRoute`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexGangFollower.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) | Implementación de la tarea compleja `TaskComplexGangFollower`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexGangFollower.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGangFollower`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) | Implementación de la tarea compleja `TaskComplexGangJoinRespond`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexGangJoinRespond.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGangJoinRespond`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexGangLeader.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) | Implementación de la tarea compleja `TaskComplexGangLeader`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexGangLeader.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGangLeader`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) | Implementación de la tarea compleja `TaskComplexGetOnBoatSeat`, relacionada con interactuar con una embarcación. |
| — | — | [Tasks/TaskTypes/TaskComplexGetOnBoatSeat.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGetOnBoatSeat`, relacionada con interactuar con una embarcación. |
| — | — | [Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) | Implementación de la tarea compleja `TaskComplexGetUpAndStandStill`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGetUpAndStandStill`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexGoPickUpEntity.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoPickUpEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) | Implementación de la tarea compleja `TaskComplexGoToAttractor`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToAttractor`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) | Implementación de la tarea compleja `TaskComplexGoToBoatSteeringWheel`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToBoatSteeringWheel`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) | Implementación de la tarea compleja `TaskComplexGoToCarDoorAndStandStill`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToCarDoorAndStandStill`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) | Implementación de la tarea compleja `TaskComplexGotoDoorAndOpen`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGotoDoorAndOpen`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointAiming`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAiming.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointAiming`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointAndStandStill`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointAndStandStill`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointAndStandStillAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointAndStandStillAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointAndStandStillTimed`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointAndStandStillTimed`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointAnyMeans`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointAnyMeans`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) | Implementación de la tarea compleja `TaskComplexGoToPointShooting`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexGoToPointShooting.h](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.h) | Declaraciones y tipos de la tarea compleja `TaskComplexGoToPointShooting`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) | Implementación de la tarea compleja `TaskComplexHitByGunResponse`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexHitByGunResponse.h](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.h) | Declaraciones y tipos de la tarea compleja `TaskComplexHitByGunResponse`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) | Implementación de la tarea compleja `TaskComplexHitPedWithCar`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskComplexHitPedWithCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexHitPedWithCar`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskComplexHitResponse.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) | Implementación de la tarea compleja `TaskComplexHitResponse`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskComplexHitResponse.h](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.h) | Declaraciones y tipos de la tarea compleja `TaskComplexHitResponse`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskComplexInAirAndLand.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) | Implementación de la tarea compleja `TaskComplexInAirAndLand`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexInAirAndLand.h](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.h) | Declaraciones y tipos de la tarea compleja `TaskComplexInAirAndLand`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) | Implementación de la tarea compleja `TaskComplexInvestigateDeadPed`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexInvestigateDeadPed`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) | Implementación de la tarea compleja `TaskComplexInvestigateDisturbance`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h) | Declaraciones y tipos de la tarea compleja `TaskComplexInvestigateDisturbance`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexInWater.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) | Implementación de la tarea compleja `TaskComplexInWater`, relacionada con desplazarse o mantenerse en el agua. |
| — | — | [Tasks/TaskTypes/TaskComplexInWater.h](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.h) | Declaraciones y tipos de la tarea compleja `TaskComplexInWater`, relacionada con desplazarse o mantenerse en el agua. |
| — | — | [Tasks/TaskTypes/TaskComplexJump.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) | Implementación de la tarea compleja `TaskComplexJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexJump.h](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.h) | Declaraciones y tipos de la tarea compleja `TaskComplexJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexKillCriminal.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) | Implementación de la tarea compleja `TaskComplexKillCriminal`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillCriminal.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillCriminal`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) | Implementación de la tarea compleja `TaskComplexKillPedFromBoat`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedFromBoat.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedFromBoat`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) | Implementación de la tarea compleja `TaskComplexKillPedGroupOnFoot`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedGroupOnFoot`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) | Implementación de la tarea compleja `TaskComplexKillPedOnFoot`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFoot.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedOnFoot`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) | Implementación de la tarea compleja `TaskComplexKillPedOnFootArmed`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedOnFootArmed`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) | Implementación de la tarea compleja `TaskComplexKillPedOnFootMelee`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedOnFootMelee`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) | Implementación de la tarea compleja `TaskComplexKillPedOnFootStealth`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h) | Declaraciones y tipos de la tarea compleja `TaskComplexKillPedOnFootStealth`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) | Implementación de la tarea compleja `TaskComplexLeaveAnyCar`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveAnyCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveAnyCar`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveBoat.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) | Implementación de la tarea compleja `TaskComplexLeaveBoat`, relacionada con interactuar con una embarcación. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveBoat.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveBoat`, relacionada con interactuar con una embarcación. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp) | Implementación de la tarea compleja `TaskComplexLeaveCar`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveCar`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) | Implementación de la tarea compleja `TaskComplexLeaveCarAndDie`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveCarAndDie`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) | Implementación de la tarea compleja `TaskComplexLeaveCarAndFlee`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveCarAndFlee`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) | Implementación de la tarea compleja `TaskComplexLeaveCarAndWander`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveCarAndWander`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) | Implementación de la tarea compleja `TaskComplexLeaveCarAsPassengerWait`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h) | Declaraciones y tipos de la tarea compleja `TaskComplexLeaveCarAsPassengerWait`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) | Implementación de la tarea compleja `TaskComplexMedicTreatInjuredPed`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexMedicTreatInjuredPed`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) | Implementación de la tarea compleja `TaskComplexMoveBackAndJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexMoveBackAndJump.h](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.h) | Declaraciones y tipos de la tarea compleja `TaskComplexMoveBackAndJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) | Implementación de la tarea compleja `TaskComplexObserveTrafficLights`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexObserveTrafficLights.h](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.h) | Declaraciones y tipos de la tarea compleja `TaskComplexObserveTrafficLights`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp) | Implementación de la tarea compleja `TaskComplexObserveTrafficLightsAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) | Declaraciones y tipos de la tarea compleja `TaskComplexObserveTrafficLightsAndAchieveHeading`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexOnFire.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) | Implementación de la tarea compleja `TaskComplexOnFire`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskComplexOnFire.h](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.h) | Declaraciones y tipos de la tarea compleja `TaskComplexOnFire`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) | Implementación de la tarea compleja `TaskComplexOpenDriverDoor`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexOpenDriverDoor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexOpenDriverDoor`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) | Implementación de la tarea compleja `TaskComplexOpenPassengerDoor`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexOpenPassengerDoor`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskComplexPartner.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) | Implementación de la tarea compleja `TaskComplexPartner`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartner.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPartner`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerChat.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) | Implementación de la tarea compleja `TaskComplexPartnerChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerChat.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPartnerChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerDeal.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) | Implementación de la tarea compleja `TaskComplexPartnerDeal`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerDeal.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPartnerDeal`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerGreet.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) | Implementación de la tarea compleja `TaskComplexPartnerGreet`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerGreet.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPartnerGreet`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerShove.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) | Implementación de la tarea compleja `TaskComplexPartnerShove`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPartnerShove.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPartnerShove`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexPassObject.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) | Implementación de la tarea compleja `TaskComplexPassObject`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskComplexPassObject.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPassObject`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) | Implementación de la tarea compleja `TaskComplexPlayHandSignalAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPlayHandSignalAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskComplexPolicePursuit.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) | Implementación de la tarea compleja `TaskComplexPolicePursuit`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexPolicePursuit.h](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h) | Declaraciones y tipos de la tarea compleja `TaskComplexPolicePursuit`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) | Implementación de la tarea compleja `TaskComplexProstituteSolicit`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexProstituteSolicit.h](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h) | Declaraciones y tipos de la tarea compleja `TaskComplexProstituteSolicit`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) | Implementación de la tarea compleja `TaskComplexReactToGunAimedAt`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h) | Declaraciones y tipos de la tarea compleja `TaskComplexReactToGunAimedAt`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskComplexRoadRage.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) | Implementación de la tarea compleja `TaskComplexRoadRage`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexRoadRage.h](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h) | Declaraciones y tipos de la tarea compleja `TaskComplexRoadRage`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) | Implementación de la tarea compleja `TaskComplexScreamInCarThenLeave`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h) | Declaraciones y tipos de la tarea compleja `TaskComplexScreamInCarThenLeave`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) | Implementación de la tarea compleja `TaskComplexSeekCoverUntilTargetDead`, relacionada con buscar y usar cobertura. |
| — | — | [Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekCoverUntilTargetDead`, relacionada con buscar y usar cobertura. |
| — | — | [Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) | Implementación de la tarea compleja `TaskComplexSeekEntityAiming`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexSeekEntityAiming.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityAiming`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/TaskComplexShuffleSeats.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) | Implementación de la tarea compleja `TaskComplexShuffleSeats`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexShuffleSeats.h](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.h) | Declaraciones y tipos de la tarea compleja `TaskComplexShuffleSeats`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexSignalAtPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) | Implementación de la tarea compleja `TaskComplexSignalAtPed`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexSignalAtPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSignalAtPed`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) | Implementación de la tarea compleja `TaskComplexSitDownThenIdleThenStandUp`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSitDownThenIdleThenStandUp`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) | Implementación de la tarea compleja `TaskComplexSmartFleeEntity`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexSmartFleeEntity.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSmartFleeEntity`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) | Implementación de la tarea compleja `TaskComplexSmartFleePoint`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexSmartFleePoint.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSmartFleePoint`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexStareAtPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) | Implementación de la tarea compleja `TaskComplexStareAtPed`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskComplexStareAtPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexStareAtPed`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskComplexStealCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) | Implementación de la tarea compleja `TaskComplexStealCar`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexStealCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexStealCar`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexStuckInAir.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) | Implementación de la tarea compleja `TaskComplexStuckInAir`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexStuckInAir.h](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.h) | Declaraciones y tipos de la tarea compleja `TaskComplexStuckInAir`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskComplexSunbathe.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) | Implementación de la tarea compleja `TaskComplexSunbathe`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexSunbathe.h](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSunbathe`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskComplexTrackEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) | Implementación de la tarea compleja `TaskComplexTrackEntity`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexTrackEntity.h](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.h) | Declaraciones y tipos de la tarea compleja `TaskComplexTrackEntity`, relacionada con seguir a una entidad, líder o recorrido. |
| — | — | [Tasks/TaskTypes/TaskComplexTreatAccident.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) | Implementación de la tarea compleja `TaskComplexTreatAccident`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskComplexTreatAccident.h](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.h) | Declaraciones y tipos de la tarea compleja `TaskComplexTreatAccident`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) | Implementación de la tarea compleja `TaskComplexTurnToFaceEntityOrCoord`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h) | Declaraciones y tipos de la tarea compleja `TaskComplexTurnToFaceEntityOrCoord`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskComplexUseAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) | Implementación de la tarea compleja `TaskComplexUseAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) | Implementación de la tarea compleja `TaskComplexUseAttractorPartner`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseAttractorPartner.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseAttractorPartner`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) | Implementación de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) | Implementación de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractorRun`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractorRun`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) | Implementación de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractorSprint`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseClosestFreeScriptedAttractorSprint`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffect.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) | Implementación de la tarea compleja `TaskComplexUseEffect`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffect.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseEffect`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) | Implementación de la tarea compleja `TaskComplexUseEffectRunning`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffectRunning.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseEffectRunning`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) | Implementación de la tarea compleja `TaskComplexUseEffectSprinting`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseEffectSprinting.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseEffectSprinting`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseGoggles.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) | Implementación de la tarea compleja `TaskComplexUseGoggles`, relacionada con utilizar gafas de visión especial. |
| — | — | [Tasks/TaskTypes/TaskComplexUseGoggles.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseGoggles`, relacionada con utilizar gafas de visión especial. |
| — | — | [Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp) | Implementación de la tarea compleja `TaskComplexUseMobilePhone`, relacionada con utilizar un teléfono móvil. |
| — | — | [Tasks/TaskTypes/TaskComplexUseMobilePhone.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseMobilePhone`, relacionada con utilizar un teléfono móvil. |
| — | — | [Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) | Implementación de la tarea compleja `TaskComplexUsePairedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUsePairedAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUsePairedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) | Implementación de la tarea compleja `TaskComplexUseScriptedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseScriptedAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseScriptedAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) | Implementación de la tarea compleja `TaskComplexUseScriptedBrain`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexUseScriptedBrain.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseScriptedBrain`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskComplexUseSwatRope.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) | Implementación de la tarea compleja `TaskComplexUseSwatRope`, relacionada con descender por una cuerda. |
| — | — | [Tasks/TaskTypes/TaskComplexUseSwatRope.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseSwatRope`, relacionada con descender por una cuerda. |
| — | — | [Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) | Implementación de la tarea compleja `TaskComplexUseWaterCannon`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexUseWaterCannon.h](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.h) | Declaraciones y tipos de la tarea compleja `TaskComplexUseWaterCannon`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) | Implementación de la tarea compleja `TaskComplexWaitAtAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitAtAttractor.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWaitAtAttractor`, relacionada con acudir a un punto de interacción o utilizarlo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForBackup.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) | Implementación de la tarea compleja `TaskComplexWaitForBackup`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForBackup.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWaitForBackup`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForBus.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) | Implementación de la tarea compleja `TaskComplexWaitForBus`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForBus.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWaitForBus`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) | Implementación de la tarea compleja `TaskComplexWaitForDryWeather`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForDryWeather.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWaitForDryWeather`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) | Implementación de la tarea compleja `TaskComplexWaitForPed`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWaitForPed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWaitForPed`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) | Implementación de la tarea compleja `TaskComplexWalkAlongsidePed`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWalkAlongsidePed`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) | Implementación de la tarea compleja `TaskComplexWalkRoundBuildingAttempt`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWalkRoundBuildingAttempt`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) | Implementación de la tarea compleja `TaskComplexWalkRoundCar`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundCar.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWalkRoundCar`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) | Implementación de la tarea compleja `TaskComplexWalkRoundFire`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundFire.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWalkRoundFire`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) | Implementación de la tarea compleja `TaskComplexWalkRoundObject`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWalkRoundObject.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWalkRoundObject`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWander.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) | Implementación de la tarea compleja `TaskComplexWander`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWander.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWander`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderCop.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) | Implementación de la tarea compleja `TaskComplexWanderCop`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderCop.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderCop`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderCriminal.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) | Implementación de la tarea compleja `TaskComplexWanderCriminal`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderCriminal.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderCriminal`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderFlee.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) | Implementación de la tarea compleja `TaskComplexWanderFlee`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderFlee.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderFlee`, relacionada con huir de una amenaza o posición. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderGang.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) | Implementación de la tarea compleja `TaskComplexWanderGang`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderGang.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderGang`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderMedic.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) | Implementación de la tarea compleja `TaskComplexWanderMedic`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderMedic.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderMedic`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderProstitute.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) | Implementación de la tarea compleja `TaskComplexWanderProstitute`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderProstitute.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderProstitute`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderStandard.cpp](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) | Implementación de la tarea compleja `TaskComplexWanderStandard`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskComplexWanderStandard.h](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h) | Declaraciones y tipos de la tarea compleja `TaskComplexWanderStandard`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskGangHasslePed.cpp](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) | Implementación de la tarea de IA `TaskGangHasslePed`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskGangHasslePed.h](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h) | Declaraciones y tipos de la tarea de IA `TaskGangHasslePed`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskGangHassleVehicle.cpp](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) | Implementación de la tarea de IA `TaskGangHassleVehicle`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskGangHassleVehicle.h](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h) | Declaraciones y tipos de la tarea de IA `TaskGangHassleVehicle`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) | Implementación de `TaskGoToVehicleAndLean`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/TaskGoToVehicleAndLean.h](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h) | Declaraciones y tipos de `TaskGoToVehicleAndLean`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/TaskLeanOnVehicle.cpp](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) | Implementación de `TaskLeanOnVehicle`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/TaskLeanOnVehicle.h](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h) | Declaraciones y tipos de `TaskLeanOnVehicle`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleAbseil.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.h) | Declaraciones y tipos de la tarea simple `TaskSimpleAbseil`, relacionada con descender por una cuerda. |
| — | — | [Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) | Implementación de la tarea simple `TaskSimpleAchieveHeading`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleAchieveHeading.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h) | Declaraciones y tipos de la tarea simple `TaskSimpleAchieveHeading`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) | Implementación de la tarea simple `TaskSimpleAffectSecondaryBehaviour`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h) | Declaraciones y tipos de la tarea simple `TaskSimpleAffectSecondaryBehaviour`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) | Implementación de la tarea simple `TaskSimpleAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleAnim.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.h) | Declaraciones y tipos de la tarea simple `TaskSimpleAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleArrestPed.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) | Implementación de la tarea simple `TaskSimpleArrestPed`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskSimpleArrestPed.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h) | Declaraciones y tipos de la tarea simple `TaskSimpleArrestPed`, relacionada con ejecutar comportamiento policial o de persecución. |
| — | — | [Tasks/TaskTypes/TaskSimpleBeHit.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) | Implementación de la tarea simple `TaskSimpleBeHit`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleBeHit.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.h) | Declaraciones y tipos de la tarea simple `TaskSimpleBeHit`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleBikeJacked.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) | Implementación de la tarea simple `TaskSimpleBikeJacked`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleBikeJacked.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.h) | Declaraciones y tipos de la tarea simple `TaskSimpleBikeJacked`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarAlign.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) | Implementación de la tarea simple `TaskSimpleCarAlign`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarAlign.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarAlign`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) | Implementación de la tarea simple `TaskSimpleCarCloseDoorFromInside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarCloseDoorFromInside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) | Implementación de la tarea simple `TaskSimpleCarCloseDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarCloseDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarDrive.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) | Implementación de la tarea simple `TaskSimpleCarDrive`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarDrive.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarDrive`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) | Implementación de la tarea simple `TaskSimpleCarDriveTimed`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarDriveTimed.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarDriveTimed`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarFallOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) | Implementación de la tarea simple `TaskSimpleCarFallOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarFallOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarFallOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGetIn.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) | Implementación de la tarea simple `TaskSimpleCarGetIn`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGetIn.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarGetIn`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGetOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) | Implementación de la tarea simple `TaskSimpleCarGetOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGetOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarGetOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) | Implementación de la tarea simple `TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) | Implementación de la tarea simple `TaskSimpleCarJumpOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarJumpOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarJumpOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) | Implementación de la tarea simple `TaskSimpleCarOpenDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarOpenDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) | Implementación de la tarea simple `TaskSimpleCarOpenLockedDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarOpenLockedDoorFromOutside`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) | Implementación de la tarea simple `TaskSimpleCarSetPedInAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSetPedInAsDriver`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) | Implementación de la tarea simple `TaskSimpleCarSetPedInAsPassenger`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSetPedInAsPassenger`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) | Implementación de la tarea simple `TaskSimpleCarSetPedOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSetPedOut`, relacionada con salir o extraer a un personaje de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) | Implementación de la tarea simple `TaskSimpleCarSetPedSlowDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSetPedSlowDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) | Implementación de la tarea simple `TaskSimpleCarSetTempAction`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSetTempAction.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSetTempAction`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarShuffle.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) | Implementación de la tarea simple `TaskSimpleCarShuffle`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarShuffle.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarShuffle`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) | Implementación de la tarea simple `TaskSimpleCarSlowBeDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSlowBeDraggedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) | Implementación de la tarea simple `TaskSimpleCarSlowDragPedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarSlowDragPedOut`, relacionada con arrastrar a un personaje o sacarlo de un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) | Implementación de la tarea simple `TaskSimpleCarWaitForDoorNotToBeInUse`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarWaitForDoorNotToBeInUse`, relacionada con interactuar con puertas de vehículos o del entorno. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) | Implementación de la tarea simple `TaskSimpleCarWaitToSlowDown`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCarWaitToSlowDown`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleChat.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) | Implementación de la tarea simple `TaskSimpleChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleChat.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h) | Declaraciones y tipos de la tarea simple `TaskSimpleChat`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleChoking.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) | Implementación de la tarea simple `TaskSimpleChoking`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleChoking.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h) | Declaraciones y tipos de la tarea simple `TaskSimpleChoking`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleClearLookAt.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) | Implementación de la tarea simple `TaskSimpleClearLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleClearLookAt.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.h) | Declaraciones y tipos de la tarea simple `TaskSimpleClearLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleClimb.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) | Implementación de la tarea simple `TaskSimpleClimb`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleClimb.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h) | Declaraciones y tipos de la tarea simple `TaskSimpleClimb`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleClimb_models.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb_models.h) | Declaraciones y tipos de la tarea simple `TaskSimpleClimb_models`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleCower.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) | Implementación de la tarea simple `TaskSimpleCower`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleCower.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCower`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) | Implementación de la tarea simple `TaskSimpleCreateCarAndGetIn`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h) | Declaraciones y tipos de la tarea simple `TaskSimpleCreateCarAndGetIn`, relacionada con entrar o colocar a un personaje en un vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleDead.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) | Implementación de la tarea simple `TaskSimpleDead`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDead.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDead`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDie.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) | Implementación de la tarea simple `TaskSimpleDie`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDie.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDie`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDieInCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) | Implementación de la tarea simple `TaskSimpleDieInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDieInCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDieInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) | Implementación de la tarea simple `TaskSimpleDoHandSignal`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleDoHandSignal.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDoHandSignal`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleDrown.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) | Implementación de la tarea simple `TaskSimpleDrown`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDrown.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDrown`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDrownInCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) | Implementación de la tarea simple `TaskSimpleDrownInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDrownInCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDrownInCar`, relacionada con gestionar muerte o ahogamiento del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleDuck.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) | Implementación de la tarea simple `TaskSimpleDuck`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleDuck.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDuck`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleDuckToggle.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) | Implementación de la tarea simple `TaskSimpleDuckToggle`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleDuckToggle.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.h) | Declaraciones y tipos de la tarea simple `TaskSimpleDuckToggle`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) | Implementación de la tarea simple `TaskSimpleEvasiveDive`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleEvasiveDive.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.h) | Declaraciones y tipos de la tarea simple `TaskSimpleEvasiveDive`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) | Implementación de la tarea simple `TaskSimpleEvasiveStep`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleEvasiveStep.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h) | Declaraciones y tipos de la tarea simple `TaskSimpleEvasiveStep`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleFacial.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) | Implementación de la tarea simple `TaskSimpleFacial`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleFacial.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.h) | Declaraciones y tipos de la tarea simple `TaskSimpleFacial`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleFall.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) | Implementación de la tarea simple `TaskSimpleFall`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleFall.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h) | Declaraciones y tipos de la tarea simple `TaskSimpleFall`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleFight.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) | Implementación de la tarea simple `TaskSimpleFight`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleFight.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h) | Declaraciones y tipos de la tarea simple `TaskSimpleFight`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleFightingControl.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) | Implementación de la tarea simple `TaskSimpleFightingControl`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleFightingControl.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h) | Declaraciones y tipos de la tarea simple `TaskSimpleFightingControl`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleFinishBrain.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) | Implementación de la tarea simple `TaskSimpleFinishBrain`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleFinishBrain.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h) | Declaraciones y tipos de la tarea simple `TaskSimpleFinishBrain`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) | Implementación de la tarea simple `TaskSimpleGangDriveBy`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleGangDriveBy.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGangDriveBy`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleGetUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) | Implementación de la tarea simple `TaskSimpleGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleGetUp.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGetUp`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleGiveCPR.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) | Implementación de la tarea simple `TaskSimpleGiveCPR`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskSimpleGiveCPR.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGiveCPR`, relacionada con atender personajes heridos o accidentes. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoTo.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) | Implementación de la tarea simple `TaskSimpleGoTo`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoTo.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGoTo`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoToPoint.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) | Implementación de la tarea simple `TaskSimpleGoToPoint`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoToPoint.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGoToPoint`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) | Implementación de la tarea simple `TaskSimpleGoToPointFine`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGoToPointFine.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGoToPointFine`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/TaskSimpleGunControl.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) | Implementación de la tarea simple `TaskSimpleGunControl`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleGunControl.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h) | Declaraciones y tipos de la tarea simple `TaskSimpleGunControl`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHandsUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) | Implementación de la tarea simple `TaskSimpleHandsUp`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleHandsUp.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHandsUp`, relacionada con adoptar una postura defensiva o evasiva. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) | Implementación de la tarea simple `TaskSimpleHitByGunFromFront`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitByGunFromFront`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) | Implementación de la tarea simple `TaskSimpleHitByGunFromLeft`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitByGunFromLeft`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) | Implementación de la tarea simple `TaskSimpleHitByGunFromRear`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitByGunFromRear`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) | Implementación de la tarea simple `TaskSimpleHitByGunFromRight`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitByGunFromRight`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromBack.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) | Implementación de la tarea simple `TaskSimpleHitFromBack`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromBack.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitFromBack`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) | Implementación de la tarea simple `TaskSimpleHitFromBehind`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromBehind.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitFromBehind`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromFront.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) | Implementación de la tarea simple `TaskSimpleHitFromFront`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromFront.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitFromFront`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) | Implementación de la tarea simple `TaskSimpleHitFromLeft`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromLeft.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitFromLeft`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromRight.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) | Implementación de la tarea simple `TaskSimpleHitFromRight`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitFromRight.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitFromRight`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitHead.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) | Implementación de la tarea simple `TaskSimpleHitHead`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitHead.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitHead`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitWall.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) | Implementación de la tarea simple `TaskSimpleHitWall`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHitWall.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHitWall`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHoldEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) | Implementación de la tarea simple `TaskSimpleHoldEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimpleHoldEntity.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHoldEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) | Implementación de la tarea simple `TaskSimpleHurtPedWithCar`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleHurtPedWithCar`, relacionada con reaccionar a golpes o colisiones. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKChain.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) | Implementación de la tarea simple `TaskSimpleIKChain`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKChain.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h) | Declaraciones y tipos de la tarea simple `TaskSimpleIKChain`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKLookAt.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) | Implementación de la tarea simple `TaskSimpleIKLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKLookAt.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h) | Declaraciones y tipos de la tarea simple `TaskSimpleIKLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKManager.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) | Implementación de la tarea simple `TaskSimpleIKManager`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKManager.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h) | Declaraciones y tipos de la tarea simple `TaskSimpleIKManager`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKPointArm.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) | Implementación de la tarea simple `TaskSimpleIKPointArm`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleIKPointArm.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h) | Declaraciones y tipos de la tarea simple `TaskSimpleIKPointArm`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleInAir.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) | Implementación de la tarea simple `TaskSimpleInAir`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleInAir.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h) | Declaraciones y tipos de la tarea simple `TaskSimpleInAir`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleJetPack.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) | Implementación de la tarea simple `TaskSimpleJetPack`, relacionada con usar el jetpack. |
| — | — | [Tasks/TaskTypes/TaskSimpleJetPack.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h) | Declaraciones y tipos de la tarea simple `TaskSimpleJetPack`, relacionada con usar el jetpack. |
| — | — | [Tasks/TaskTypes/TaskSimpleJump.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) | Implementación de la tarea simple `TaskSimpleJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleJump.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h) | Declaraciones y tipos de la tarea simple `TaskSimpleJump`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) | Implementación de la tarea simple `TaskSimpleKillPedWithCar`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleKillPedWithCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleKillPedWithCar`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleLand.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) | Implementación de la tarea simple `TaskSimpleLand`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleLand.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h) | Declaraciones y tipos de la tarea simple `TaskSimpleLand`, relacionada con saltar, trepar, caer, aterrizar o levantarse. |
| — | — | [Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) | Implementación de la tarea simple `TaskSimpleLeaveGroup`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskSimpleLeaveGroup.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h) | Declaraciones y tipos de la tarea simple `TaskSimpleLeaveGroup`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskSimpleLookAbout.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) | Implementación de la tarea simple `TaskSimpleLookAbout`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleLookAbout.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h) | Declaraciones y tipos de la tarea simple `TaskSimpleLookAbout`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleNone.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) | Implementación de la tarea simple `TaskSimpleNone`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleNone.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h) | Declaraciones y tipos de la tarea simple `TaskSimpleNone`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimplePause.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) | Implementación de la tarea simple `TaskSimplePause`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimplePause.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h) | Declaraciones y tipos de la tarea simple `TaskSimplePause`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimplePickUpBike.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) | Implementación de la tarea simple `TaskSimplePickUpBike`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimplePickUpBike.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h) | Declaraciones y tipos de la tarea simple `TaskSimplePickUpBike`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimplePickUpEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) | Implementación de la tarea simple `TaskSimplePickUpEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimplePickUpEntity.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h) | Declaraciones y tipos de la tarea simple `TaskSimplePickUpEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) | Implementación de la tarea simple `TaskSimplePlayerOnFire`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayerOnFire.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h) | Declaraciones y tipos de la tarea simple `TaskSimplePlayerOnFire`, relacionada con reaccionar al fuego o extinguirlo. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) | Implementación de la tarea simple `TaskSimplePlayerOnFoot`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayerOnFoot.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h) | Declaraciones y tipos de la tarea simple `TaskSimplePlayerOnFoot`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) | Implementación de la tarea simple `TaskSimplePlayHandSignalAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h) | Declaraciones y tipos de la tarea simple `TaskSimplePlayHandSignalAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimplePutDownEntity.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) | Implementación de la tarea simple `TaskSimplePutDownEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimplePutDownEntity.h](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h) | Declaraciones y tipos de la tarea simple `TaskSimplePutDownEntity`, relacionada con recoger, sostener, soltar o entregar objetos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) | Implementación de la tarea simple `TaskSimpleRunAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunAnim.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h) | Declaraciones y tipos de la tarea simple `TaskSimpleRunAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) | Implementación de la tarea simple `TaskSimpleRunNamedAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunNamedAnim.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.h) | Declaraciones y tipos de la tarea simple `TaskSimpleRunNamedAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) | Implementación de la tarea simple `TaskSimpleRunTimedAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleRunTimedAnim.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.h) | Declaraciones y tipos de la tarea simple `TaskSimpleRunTimedAnim`, relacionada con reproducir animaciones o gestos. |
| — | — | [Tasks/TaskTypes/TaskSimpleSay.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) | Implementación de la tarea simple `TaskSimpleSay`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleSay.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSay`, relacionada con conversar o interactuar con otro personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleScratchHead.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) | Implementación de la tarea simple `TaskSimpleScratchHead`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleScratchHead.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.h) | Declaraciones y tipos de la tarea simple `TaskSimpleScratchHead`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) | Implementación de la tarea simple `TaskSimpleSetCharDecisionMaker`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSetCharDecisionMaker`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) | Implementación de la tarea simple `TaskSimpleSetCharIgnoreWeaponRangeFlag`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSetCharIgnoreWeaponRangeFlag`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) | Implementación de la tarea simple `TaskSimpleSetKindaStayInSamePlace`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSetKindaStayInSamePlace`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) | Implementación de la tarea simple `TaskSimpleSetPedAsAutoDriver`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSetPedAsAutoDriver`, relacionada con conducir o controlar desplazamientos en vehículo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) | Implementación de la tarea simple `TaskSimpleSetStayInSamePlace`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSetStayInSamePlace`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleShakeFist.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) | Implementación de la tarea simple `TaskSimpleShakeFist`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleShakeFist.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h) | Declaraciones y tipos de la tarea simple `TaskSimpleShakeFist`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSitDown.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) | Implementación de la tarea simple `TaskSimpleSitDown`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSitDown.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSitDown`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSitIdle.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) | Implementación de la tarea simple `TaskSimpleSitIdle`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSitIdle.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSitIdle`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) | Implementación de la tarea simple `TaskSimpleSlideToCoord`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleSlideToCoord.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSlideToCoord`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleStandStill.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) | Implementación de la tarea simple `TaskSimpleStandStill`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleStandStill.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h) | Declaraciones y tipos de la tarea simple `TaskSimpleStandStill`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleStandUp.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) | Implementación de la tarea simple `TaskSimpleStandUp`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleStandUp.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.h) | Declaraciones y tipos de la tarea simple `TaskSimpleStandUp`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/TaskSimpleStealthKill.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) | Implementación de la tarea simple `TaskSimpleStealthKill`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleStealthKill.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h) | Declaraciones y tipos de la tarea simple `TaskSimpleStealthKill`, relacionada con combate y ataque a un objetivo. |
| — | — | [Tasks/TaskTypes/TaskSimpleSwim.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) | Implementación de la tarea simple `TaskSimpleSwim`, relacionada con desplazarse o mantenerse en el agua. |
| — | — | [Tasks/TaskTypes/TaskSimpleSwim.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h) | Declaraciones y tipos de la tarea simple `TaskSimpleSwim`, relacionada con desplazarse o mantenerse en el agua. |
| — | — | [Tasks/TaskTypes/TaskSimpleThrowControl.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) | Implementación de la tarea simple `TaskSimpleThrowControl`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleThrowControl.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h) | Declaraciones y tipos de la tarea simple `TaskSimpleThrowControl`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) | Implementación de la tarea simple `TaskSimpleThrowProjectile`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleThrowProjectile.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.h) | Declaraciones y tipos de la tarea simple `TaskSimpleThrowProjectile`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleTired.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) | Implementación de la tarea simple `TaskSimpleTired`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleTired.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h) | Declaraciones y tipos de la tarea simple `TaskSimpleTired`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) | Implementación de la tarea simple `TaskSimpleTogglePedThreatScanner`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h) | Declaraciones y tipos de la tarea simple `TaskSimpleTogglePedThreatScanner`, relacionada con controlar decisiones o comportamientos del personaje. |
| — | — | [Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp) | Implementación de la tarea simple `TaskSimpleTriggerLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleTriggerLookAt.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.h) | Declaraciones y tipos de la tarea simple `TaskSimpleTriggerLookAt`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleTurn180.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) | Implementación de la tarea simple `TaskSimpleTurn180`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleTurn180.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.h) | Declaraciones y tipos de la tarea simple `TaskSimpleTurn180`, relacionada con orientar el cuerpo, la mirada o los huesos. |
| — | — | [Tasks/TaskTypes/TaskSimpleUninterruptable.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) | Implementación de la tarea simple `TaskSimpleUninterruptable`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleUninterruptable.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.h) | Declaraciones y tipos de la tarea simple `TaskSimpleUninterruptable`, una acción o secuencia ejecutada por peatones. |
| — | — | [Tasks/TaskTypes/TaskSimpleUseAtm.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) | Implementación de la tarea simple `TaskSimpleUseAtm`, relacionada con utilizar un cajero automático. |
| — | — | [Tasks/TaskTypes/TaskSimpleUseAtm.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.h) | Declaraciones y tipos de la tarea simple `TaskSimpleUseAtm`, relacionada con utilizar un cajero automático. |
| — | — | [Tasks/TaskTypes/TaskSimpleUseGun.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) | Implementación de la tarea simple `TaskSimpleUseGun`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleUseGun.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h) | Declaraciones y tipos de la tarea simple `TaskSimpleUseGun`, relacionada con apuntar, disparar o lanzar armas. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitForBus.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) | Implementación de la tarea simple `TaskSimpleWaitForBus`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitForBus.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.h) | Declaraciones y tipos de la tarea simple `TaskSimpleWaitForBus`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) | Implementación de la tarea simple `TaskSimpleWaitUntilAreaCodesMatch`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h) | Declaraciones y tipos de la tarea simple `TaskSimpleWaitUntilAreaCodesMatch`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) | Implementación de la tarea simple `TaskSimpleWaitUntilLeaderAreaCodesMatch`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h) | Declaraciones y tipos de la tarea simple `TaskSimpleWaitUntilLeaderAreaCodesMatch`, relacionada con participar en un grupo y coordinarse con sus integrantes. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) | Implementación de la tarea simple `TaskSimpleWaitUntilPedIsInCar`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleWaitUntilPedIsInCar`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) | Implementación de la tarea simple `TaskSimpleWaitUntilPedIsOutCar`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h) | Declaraciones y tipos de la tarea simple `TaskSimpleWaitUntilPedIsOutCar`, relacionada con esperar una condición o un intervalo. |
| — | — | [Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) | Implementación de `TaskUtilityLineUpPedWithCar`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h) | Declaraciones y tipos de `TaskUtilityLineUpPedWithCar`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |

</details>

<details>
<summary>Tasks/TaskTypes/Interior/ — 16 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) | Implementación de la tarea de IA `TaskInteriorBeInHouse`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorBeInHouse`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) | Implementación de la tarea de IA `TaskInteriorBeInOffice`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorBeInOffice`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) | Implementación de la tarea de IA `TaskInteriorBeInShop`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorBeInShop`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) | Implementación de la tarea de IA `TaskInteriorGoToInfo`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorGoToInfo`, relacionada con desplazarse, recorrer el entorno o cruzar vías. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) | Implementación de la tarea de IA `TaskInteriorLieInBed`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorLieInBed`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) | Implementación de la tarea de IA `TaskInteriorShopKeeper`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorShopKeeper`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) | Implementación de la tarea de IA `TaskInteriorSitAtDesk`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorSitAtDesk`, relacionada con mantener o cambiar una postura de reposo. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) | Implementación de la tarea de IA `TaskInteriorUseInfo`, relacionada con actuar en una casa, oficina o tienda. |
| — | — | [Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h) | Declaraciones y tipos de la tarea de IA `TaskInteriorUseInfo`, relacionada con actuar en una casa, oficina o tienda. |

</details>

<details>
<summary>Tasks/TaskTypes/SeekEntity/ — 10 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntity`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeans.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityAnyMeans`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) | Implementación de la tarea compleja `TaskComplexSeekEntityAnyMeansXYOffset`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityAnyMeansXYOffset`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) | Implementación de la tarea compleja `TaskComplexSeekEntityRadiusAngleOffset`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityRadiusAngleOffset`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) | Implementación de la tarea compleja `TaskComplexSeekEntityStandard`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityStandard`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp) | Implementación de la tarea compleja `TaskComplexSeekEntityXYOffset`, relacionada con acercarse a una entidad objetivo. |
| — | — | [Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h) | Declaraciones y tipos de la tarea compleja `TaskComplexSeekEntityXYOffset`, relacionada con acercarse a una entidad objetivo. |

</details>

<details>
<summary>Tasks/TaskTypes/SeekEntity/PosCalculators/ — 10 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp) | Implementación de `EntitySeekPosCalculator`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h) | Declaraciones y tipos de `EntitySeekPosCalculator`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) | Implementación de `EntitySeekPosCalculatorFixedPos`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h) | Declaraciones y tipos de `EntitySeekPosCalculatorFixedPos`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) | Implementación de `EntitySeekPosCalculatorRadiusAngleOffset`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h) | Declaraciones y tipos de `EntitySeekPosCalculatorRadiusAngleOffset`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) | Implementación de `EntitySeekPosCalculatorStandard`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h) | Declaraciones y tipos de `EntitySeekPosCalculatorStandard`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) | Implementación de `EntitySeekPosCalculatorXYOffset`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |
| — | — | [Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h) | Declaraciones y tipos de `EntitySeekPosCalculatorXYOffset`: infraestructura para asignar, organizar o ejecutar tareas de peatones. |

</details>

<details>
<summary>Text/ — 6 nombres</summary>

| re3 | reVC | reSA | Descripción |
| --- | --- | --- | --- |
| — | — | [Text/Data.cpp](../source/game_sa/Text/Data.cpp) | Implementación de `Data`: datos, claves o búsqueda de textos del juego. |
| — | — | [Text/Data.h](../source/game_sa/Text/Data.h) | Declaraciones y tipos de `Data`: datos, claves o búsqueda de textos del juego. |
| — | — | [Text/KeyArray.cpp](../source/game_sa/Text/KeyArray.cpp) | Implementación de `KeyArray`: datos, claves o búsqueda de textos del juego. |
| — | — | [Text/KeyArray.h](../source/game_sa/Text/KeyArray.h) | Declaraciones y tipos de `KeyArray`: datos, claves o búsqueda de textos del juego. |
| — | — | [Text/MissionTextOffsets.cpp](../source/game_sa/Text/MissionTextOffsets.cpp) | Implementación de `MissionTextOffsets`: datos, claves o búsqueda de textos del juego. |
| — | — | [Text/MissionTextOffsets.h](../source/game_sa/Text/MissionTextOffsets.h) | Declaraciones y tipos de `MissionTextOffsets`: datos, claves o búsqueda de textos del juego. |

</details>

## Actualizar

Desde la raíz de gta-reversed-master:

```powershell
python contrib/correlate_sources.py
```

El generador solo lee los tres árboles y escribe este documento. Las relaciones funcionales se mantienen explícitamente en `contrib/correlate_sources.py`; las descripciones, en `contrib/source_descriptions.py`. El inventario se obtiene de los archivos existentes.
