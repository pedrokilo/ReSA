"""Spanish navigational descriptions; not a behavioral audit of the sources."""
import re


FOLDER_DESCRIPTIONS = {
    'animation': 'Carga, organización y mezcla de animaciones; jerarquías y movimiento de huesos.',
    'audio': 'Reproducción de sonidos, voces, música y radio; carga de pistas y acceso al dispositivo de audio.',
    'buildings': 'Entidades que representan edificios y elementos estáticos del escenario.',
    'collision': 'Geometría y pruebas para detectar contactos e intersecciones entre objetos del mundo.',
    'control': 'Lógica del mundo, misiones, scripts y comportamiento de personajes y tráfico.',
    'core': 'Sistemas centrales del juego y utilidades compartidas: ciclo de ejecución, cámara, mundo y estructuras básicas.',
    'entities': 'Bases de los objetos del mundo: posición, representación y propiedades físicas.',
    'extras': 'Extensiones del proyecto, herramientas y efectos gráficos adicionales; correspondencia parcial con reSA.',
    'fakerw': 'Capa de compatibilidad con la interfaz RenderWare; compartir interfaz no implica compartir implementación.',
    'math': 'Vectores, matrices, cuaterniones y operaciones geométricas usadas por el motor.',
    'modelinfo': 'Metadatos de modelos 3D, tipos de modelo y niveles de detalle.',
    'objects': 'Objetos del escenario, sus propiedades e interacción; incluye objetos asociados a efectos.',
    'peds': 'Peatones y jugador: movimiento, combate, percepción, decisiones e interacción.',
    'renderer': 'Dibujo de la escena, visibilidad, iluminación, sombras y efectos visuales.',
    'rw': 'Integración gráfica con RenderWare, texturas y extensiones del motor.',
    'save': 'Lectura, escritura y organización de partidas guardadas.',
    'skel': 'Arranque, ventanas y eventos de plataforma; su contraparte en reSA está fuera de game_sa, en source/app.',
    'text': 'Carga y presentación de textos, mensajes y fuentes tipográficas.',
    'vehicles': 'Vehículos, conducción, transmisión, daños y propiedades de manejo.',
    'weapons': 'Armas, disparos, proyectiles, explosiones y sus efectos.',
}

# Keys can list related filenames. Descriptions refer to purpose, not status.
SUBJECTS = {}


def add(names, description):
    for name in names.split():
        SUBJECTS[name.casefold()] = description


for names, description in [
    ('Entity', 'la entidad base del mundo y sus operaciones comunes'),
    ('Physical', 'propiedades físicas, movimiento y respuesta a colisiones de las entidades'),
    ('Placeable SimpleTransform', 'posición y orientación de elementos del mundo'),
    ('Building AnimatedBuilding Treadable Solid', 'representación de edificios y geometría del escenario'),
    ('Dummy DummyObject DummyPed', 'representaciones simplificadas de entidades'),
    ('Object', 'objetos del escenario y su comportamiento'),
    ('ObjectData', 'propiedades físicas y de comportamiento de los tipos de objeto'),
    ('Ped', 'el personaje base: estado, movimiento e interacción del peatón'),
    ('PlayerPed PlayerInfo PlayerPedData', 'el personaje del jugador y sus datos de estado'),
    ('CivilianPed', 'peatones civiles'), ('CopPed', 'personajes de policía'),
    ('EmergencyPed', 'personajes de servicios de emergencia'),
    ('Vehicle', 'el vehículo base y sus operaciones comunes'),
    ('Automobile', 'automóviles y su comportamiento físico'),
    ('Bike Bmx', 'motocicletas o bicicletas, según la clase'),
    ('Boat', 'embarcaciones y navegación'), ('Heli', 'helicópteros'),
    ('Plane', 'aviones'), ('Train TrainNode', 'trenes y sus recorridos'),
    ('Trailer', 'remolques'), ('QuadBike', 'cuatrimotos'), ('MonsterTruck', 'camionetas de ruedas grandes'),
    ('Door', 'puertas de vehículos y su movimiento'),
    ('DamageManager', 'estado de daños de los componentes del vehículo'),
    ('HandlingMgr cHandlingDataMgr tHandlingData', 'propiedades que determinan el manejo de vehículos'),
    ('Transmission cTransmission tTransmissionGear', 'transmisión, marchas y entrega de velocidad del vehículo'),
    ('CarGen CarGenerator TheCarGenerators', 'generación de vehículos estacionados en puntos definidos'),
    ('CarAI CarCtrl AutoPilot', 'conducción automática, tráfico y control de vehículos por la IA'),
    ('CarEnterExit', 'entrada y salida de personajes de los vehículos'),
    ('StuckCarCheck UpsideDownCarCheck', 'detección de vehículos atascados o volcados'),
    ('VehicleRecording', 'grabación y reproducción de recorridos de vehículos'),
    ('CustomCarPlateMgr SpecialPlateHandler', 'generación y gestión de matrículas'),
    ('Camera Cam IdleCam QueuedMode', 'cámaras del juego, modos de vista y seguimiento'),
    ('CamPathSplines', 'trayectorias interpoladas de cámara'),
    ('Game GameLogic', 'inicialización, actualización y reglas generales de ejecución del juego'),
    ('World World_Helper Scene', 'organización y consulta de entidades de la escena'),
    ('Sector RepeatSector', 'división espacial del mundo en sectores'),
    ('Clock Date', 'hora y calendario del juego'),
    ('Timer TimeStep', 'tiempo transcurrido y paso de simulación'),
    ('Weather WindModifiers', 'clima y variaciones del viento'),
    ('TimeCycle Timecycle', 'variación de iluminación y ambiente según hora y clima'),
    ('Clouds', 'nubes y su representación visual'),
    ('WaterLevel', 'superficie, altura y representación del agua'),
    ('Buoyancy Floater', 'flotación y fuerzas asociadas al agua'),
    ('WaterCreatures WaterCreatureManager_c WaterCreature_c', 'criaturas acuáticas ambientales'),
    ('Birds', 'aves ambientales'),
    ('Streaming StreamingInfo', 'carga y descarga de recursos del mundo según demanda'),
    ('CdStream CdStreamPosix CdStreamInfo', 'lectura de datos usada por el sistema de streaming'),
    ('Directory', 'índice de entradas de archivos de recursos'),
    ('FileMgr', 'operaciones de archivos y directorios'),
    ('FileLoader FileObjectInstance', 'carga de definiciones y colocación de elementos del mundo'),
    ('IplStore IplDef tBinaryIplFile', 'datos IPL de colocación de entidades del mapa'),
    ('TxdStore TxdDef TxdStore_Plugin', 'diccionarios de texturas y su gestión'),
    ('TexRead TexList TexturePools', 'lectura, listas o reservas de texturas'),
    ('MemoryMgr MemoryHeap MemoryCard', 'gestión de memoria o almacenamiento según la implementación'),
    ('Pools', 'reservas reutilizables de objetos del juego'),
    ('References Reference', 'seguimiento de referencias a entidades'),
    ('Pad Pad_SDL ControllerState KeyboardState MouseControllerState', 'lectura y estado de controles del jugador'),
    ('ControllerConfig ControllerConfigManager FrontEndControls', 'asignación y configuración de controles'),
    ('Frontend Frontend_PS2 MenuManager MenuScreens MenuScreensCustom MenuSystem', 'menús, opciones y navegación de la interfaz'),
    ('Hud HudColours UserDisplay', 'información mostrada al jugador durante la partida'),
    ('Radar', 'radar, mapa y marcadores de navegación'),
    ('Font', 'dibujo y formato de fuentes tipográficas'),
    ('Text GxtChar', 'textos localizados y su representación GXT'),
    ('Messages Pager', 'mensajes presentados al jugador'),
    ('Localisation', 'ajustes y diferencias de localización'),
    ('Credits', 'presentación de créditos'),
    ('LoadingScreen LoadMonitor', 'pantallas y seguimiento de carga'),
    ('OnscreenTimer OnscreenTimerEntry OnscreenCounterEntry', 'temporizadores y contadores visibles en pantalla'),
    ('Renderer Draw', 'preparación y dibujo de la escena'),
    ('RenderBuffer', 'almacenamiento temporal de geometría para dibujar'),
    ('VisibilityPlugins Occlusion Occluder ActiveOccluder', 'selección de elementos visibles y ocultación por geometría'),
    ('Shadows ShadowCamera RealTimeShadow RealTimeShadowManager CutsceneShadow', 'generación y dibujo de sombras'),
    ('StencilShadows StencilShadowObject', 'sombras mediante búfer stencil'),
    ('Coronas RegisteredCorona', 'halos luminosos alrededor de luces'),
    ('PointLights BrightLights SearchLight', 'luces puntuales, luces visibles o haces de búsqueda'),
    ('Sprite Sprite2d', 'dibujo de imágenes y elementos gráficos planos'),
    ('MBlur MotionBlurStreaks RegisteredMotionBlurStreak', 'desenfoque de movimiento y estelas visuales'),
    ('PostEffects postfx', 'efectos aplicados a la imagen renderizada'),
    ('Mirrors', 'reflejos en espejos'), ('Gamma', 'ajuste de gamma de la imagen'),
    ('Glass FallingGlassPane', 'cristales, roturas y fragmentos'),
    ('Skidmarks Skidmark', 'marcas de neumáticos en el suelo'),
    ('BulletTrace BulletTraces', 'estelas visibles de disparos'),
    ('PlaneTrail PlaneTrails PlaneBanners', 'estelas y elementos visuales asociados a aviones'),
    ('SpecialFX', 'efectos visuales especiales'),
    ('Particle ParticleMgr ParticleObject ParticleType', 'partículas y efectos; su organización difiere entre juegos'),
    ('Rubbish', 'basura ambiental del escenario'),
    ('MovingThings', 'elementos ambientales móviles'),
    ('Weapon WeaponInfo WeaponType', 'armas, sus propiedades y tipos'),
    ('WeaponEffects', 'efectos visuales asociados a armas y apuntado'),
    ('Projectile ProjectileInfo ShotInfo BulletInfo', 'proyectiles, balas y datos de disparos'),
    ('Explosion', 'explosiones, su evolución y efectos'),
    ('Fire FireManager CreepingFire', 'fuego, focos de incendio y propagación'),
    ('WaterCannon WaterCannons', 'chorros de cañones de agua'),
    ('Wanted Crime CrimeBeingQd', 'delitos y respuesta policial o nivel de búsqueda'),
    ('RoadBlocks', 'bloqueos de carretera'),
    ('Population PopCycle', 'creación y distribución de población y tráfico'),
    ('Gangs GangInfo GangWars GangWarsSaveStructure', 'bandas, relaciones y guerras territoriales'),
    ('PedAI PedFight PedChat', 'IA, combate o conversación de peatones'),
    ('PedIK', 'cinemática inversa para orientar huesos de personajes'),
    ('PedType PedStats Acquaintance', 'tipos, atributos y relaciones de peatones'),
    ('PedPlacement PedGeometryAnalyser', 'colocación de peatones y análisis de su entorno geométrico'),
    ('PedIntelligence', 'coordinación de percepción, eventos y tareas del peatón'),
    ('PedGroup PedGroups PedGroupMembership PedGroupIntelligence PedGroupPlacer', 'grupos de peatones, integrantes y coordinación'),
    ('PedDamageResponse PedDamageResponseCalculator', 'cálculo y representación del daño recibido por peatones'),
    ('PedStuckChecker', 'detección de peatones atascados'),
    ('Formation', 'formaciones de personajes'),
    ('PathFind NodeAddress NodeRoute PointList PedRoutes', 'rutas, nodos y búsqueda de caminos'),
    ('TrafficLights', 'semáforos y control de sus estados'),
    ('Pickups Pickup tPickupMessage', 'objetos recogibles y sus mensajes'),
    ('Clothes ClothesBuilder PedClothesDesc PlayerSkin', 'ropa, apariencia y construcción visual de personajes'),
    ('Stats', 'estadísticas y progreso del jugador'),
    ('Shopping', 'compras y artículos disponibles en tiendas'),
    ('Cheat', 'trucos y sus efectos'),
    ('Darkel', 'retos de matanza o rampages'),
    ('StuntJumpManager', 'saltos acrobáticos especiales'),
    ('Garages Garage StoredCar', 'garajes y vehículos almacenados'),
    ('Cranes', 'grúas'), ('Bridge', 'puentes y su lógica'),
    ('Escalator Escalators', 'escaleras mecánicas'),
    ('Rope Ropes', 'cuerdas y sus puntos de simulación'),
    ('CutsceneMgr CutsceneObject', 'cinemáticas y objetos que participan en ellas'),
    ('Replay Record', 'grabación o reproducción de secuencias del juego'),
    ('Restart', 'reinicio y puntos de reaparición'),
    ('Remote', 'vehículos controlados a distancia'),
    ('GenericGameStorage C_PcSave PCSave SaveBuf', 'serialización y almacenamiento de partidas'),
    ('MissionCleanup ScriptResourceManager', 'recursos creados por scripts y limpieza al terminar misiones'),
    ('StreamedScripts ScriptsForBrains', 'carga de scripts y comportamientos controlados por scripts'),
    ('EntryExit EntryExitManager', 'transiciones entre entradas, salidas e interiores'),
    ('Zones TheZones Zone ZoneInfo', 'zonas del mapa y sus propiedades'),
    ('ZoneCull CullZones', 'zonas que alteran visibilidad y propiedades del entorno'),
    ('PlaceName', 'nombre de la ubicación presentado al jugador'),
    ('SurfaceTable SurfaceInfo_c SurfaceInfos_c', 'propiedades de materiales y superficies'),
    ('Checkpoint Checkpoints 3dMarker 3dMarkers', 'puntos de control y marcadores del mundo'),
    ('SetPiece SetPieces', 'situaciones o escenas predefinidas activadas en el mundo'),
    ('Accident AccidentManager', 'registro y gestión de accidentes'),
    ('Cover CoverPoint', 'puntos de cobertura para personajes'),
    ('PlantMgr PlantSurfPropMgr GrassRenderer PPTriPlantBuffer', 'vegetación, hierba y superficies donde se generan'),
    ('TagManager', 'grafitis o marcas del escenario'),
    ('CustomRoadsignMgr ShinyTexts', 'carteles o textos dibujados en el escenario'),
    ('BreakManager_c BreakObject_c', 'objetos rompibles y fragmentación'),
    ('Vector Vector2D Matrix Quaternion Rect Box BoundingBox Range2D Range3D Curves', 'tipos y operaciones matemáticas o geométricas'),
    ('CompressedVector CompressedMatrixNotAligned CompressedBox', 'representaciones compactas de datos geométricos'),
    ('RGBA', 'componentes de color y transparencia'),
    ('General Maths math maths common constants templates', 'utilidades, definiciones u operaciones compartidas'),
    ('Debug PedDebug PedDebugVisualiser Console VarConsole', 'inspección, diagnóstico y herramientas de depuración'),
    ('IniFile ini', 'lectura o gestión de configuración INI'),
    ('Config config', 'opciones de compilación o configuración del proyecto'),
    ('Profile timebars', 'medición y visualización de rendimiento'),
    ('DMAudio AudioEngine', 'interfaz principal del sistema de sonido'),
    ('AudioManager AudioLogic', 'gestión general y lógica de reproducción de audio'),
    ('MusicManager', 'selección y reproducción de música y radio'),
    ('AudioCollision', 'sonidos generados por colisiones'),
    ('PolRadio', 'mensajes de radio policial'),
    ('AudioScriptObject', 'fuentes o eventos de audio controlados desde scripts'),
    ('AudioSamples soundlist audio_enums', 'identificadores y datos de sonidos'),
    ('sampman sampman_miles sampman_null sampman_oal', 'gestión de muestras de audio y sus backends'),
    ('AnimationId AnimationEnums', 'identificadores de animaciones'),
    ('Bones FrameUpdate', 'huesos y actualización de la jerarquía animada'),
    ('AnimViewer', 'visualización de animaciones para inspección'),
    ('Script Script2 Script3 Script4 Script5 Script6 Script7 Script8 TheScripts RunningScript', 'ejecución y administración de scripts de misiones'),
    ('ScriptCommands eScriptCommands', 'identificadores de instrucciones u opcodes de scripts'),
    ('ScriptDebug', 'depuración de scripts'),
    ('EventList', 'registro de eventos del mundo'),
    ('Phones', 'teléfonos y su interacción'),
    ('Stinger', 'barreras de pinchos para neumáticos'),
    ('Antennas', 'antenas y su comportamiento visual'),
    ('Fluff', 'elementos ambientales decorativos'),
    ('NodeName', 'nombres asociados a nodos de RenderWare'),
    ('RpAnimBlend RpHAnimBlendInterpFrame', 'mezcla de animaciones sobre jerarquías de RenderWare'),
    ('RwHelper RwObjectNameIdAssocation', 'utilidades de integración con RenderWare'),
    ('main re3 skeleton events crossplatform platform glfw win', 'arranque y conexión de la aplicación con eventos y servicios de plataforma'),
    ('debugmenu frontendoption', 'herramientas de depuración y opciones adicionales de interfaz'),
    ('custompipes custompipes_d3d9 custompipes_gl', 'rutas de renderizado personalizadas para distintos backends'),
    ('screendroplets', 'efecto de gotas sobre la imagen de pantalla'),
    ('GitSHA1', 'identificación de la revisión del código'),
    ('re3_inttypes', 'tipos enteros utilizados por el proyecto'),
    ('Lines', 'dibujo de líneas en el espacio del juego'),
    ('ModelIndices', 'identificadores de modelos conocidos por el juego'),
    ('BouncingPanel', 'oscilación de paneles de vehículos mediante límites y amortiguación'),
    ('CurrentVehicle', 'seguimiento y presentación de información del vehículo actual'),
    ('GridRef', 'referencias de cuadrícula del mapa y consulta de datos de área'),
    ('HandShaker', 'generación de oscilaciones angulares y matrices de movimiento'),
    ('InformFriendsEventQueue InformGroupEvent InformGroupEventQueue', 'propagación de eventos a compañeros o grupos de peatones'),
    ('InterestingEvents', 'registro de sucesos destacados y entidades relacionadas'),
    ('LoadedCarGroup', 'grupos de modelos de vehículos cargados'),
    ('MentalHealth', 'estado de enfado del personaje y seguimiento de cambios de salud'),
    ('ModelInfoAccelerator', 'almacenamiento y reutilización de identificadores para acelerar la carga de modelos'),
    ('PedList', 'listas de referencias a peatones'),
    ('PedTaskPair', 'asociación de un peatón con una tarea'),
    ('PlayerRelationshipRecorder', 'registro de relaciones entre peatones y el jugador'),
    ('PolyBunch', 'grupos de vértices de polígonos y coordenadas de textura'),
    ('Queue', 'una estructura de datos de cola'),
    ('ReplayBlockType', 'tipos de bloques usados por el sistema de repeticiones'),
    ('RideAnimData RideAnims VehicleAnimGroupData', 'selección y agrupación de animaciones de conducción y uso de vehículos'),
    ('Route', 'representación de recorridos mediante puntos'),
    ('StoredCollPoly', 'almacenamiento de polígonos de colisión'),
    ('tAudioExtensionType', 'asociación entre extensiones de archivo y tipos de audio'),
    ('tBikeHandlingData', 'propiedades de manejo específicas de motos o bicicletas'),
    ('tBoatHandlingData', 'propiedades de manejo específicas de embarcaciones'),
    ('tFlyingHandlingData', 'propiedades de manejo específicas de aeronaves'),
    ('tUserTracksInfo', 'información sobre pistas de música del usuario'),
    ('HandObject', 'representación de manos vinculadas a huesos de un peatón'),
    ('PedStdBonePositions', 'posiciones de referencia de los huesos de peatones'),
    ('HandlingDataType', 'identificación de categorías de datos de conducción'),
    ('OpcodeResult', 'resultados y control de flujo al ejecutar un opcode'),
    ('SCMChunks', 'estructuras de bloques del formato de scripts SCM'),
    ('Scripted2dEffects', 'efectos 2D y puntos de interacción controlados desde scripts'),
    ('ScriptParam', 'representación de los parámetros de instrucciones de scripts'),
]:
    add(names, description)


TASK_ACTIONS = [
    (r'Car.*Door|Open.*Door|GotoDoor', 'interactuar con puertas de vehículos o del entorno'),
    (r'Enter.*Car|CarGetIn|CreateCarAndGetIn|SetPedIn', 'entrar o colocar a un personaje en un vehículo'),
    (r'Leave.*Car|CarGetOut|CarJumpOut|CarFallOut|SetPedOut', 'salir o extraer a un personaje de un vehículo'),
    (r'Drag|Dragged', 'arrastrar a un personaje o sacarlo de un vehículo'),
    (r'Drive|CarDrive|AutoDriver', 'conducir o controlar desplazamientos en vehículo'),
    (r'Kill|Fight|DestroyCar|Stealth', 'combate y ataque a un objetivo'),
    (r'Gun|Shoot|ThrowProjectile|ThrowControl|GangDriveBy', 'apuntar, disparar o lanzar armas'),
    (r'Flee', 'huir de una amenaza o posición'),
    (r'Follow|TrackEntity', 'seguir a una entidad, líder o recorrido'),
    (r'SeekCover', 'buscar y usar cobertura'),
    (r'SeekEntity', 'acercarse a una entidad objetivo'),
    (r'GoTo|Goto|Walk|Wander|CrossRoad|ObserveTraffic', 'desplazarse, recorrer el entorno o cruzar vías'),
    (r'Attractor', 'acudir a un punto de interacción o utilizarlo'),
    (r'Anim|Facial|HandSignal', 'reproducir animaciones o gestos'),
    (r'IK|LookAt|LookAbout|Stare|FaceEntity|Heading|Turn180', 'orientar el cuerpo, la mirada o los huesos'),
    (r'Hit|Hurt|BeHit', 'reaccionar a golpes o colisiones'),
    (r'Die|Dead|Drown', 'gestionar muerte o ahogamiento del personaje'),
    (r'Fall|GetUp|Land|InAir|Jump|Climb|Dive', 'saltar, trepar, caer, aterrizar o levantarse'),
    (r'Swim|InWater', 'desplazarse o mantenerse en el agua'),
    (r'Fire|Extinguish', 'reaccionar al fuego o extinguirlo'),
    (r'Chat|Partner|Greet|Say|Signal|Couple', 'conversar o interactuar con otro personaje'),
    (r'Gang|Group|Leader', 'participar en un grupo y coordinarse con sus integrantes'),
    (r'Arrest|Cop|Police|Criminal', 'ejecutar comportamiento policial o de persecución'),
    (r'Medic|CPR|TreatAccident|Injured', 'atender personajes heridos o accidentes'),
    (r'PickUp|PutDown|HoldEntity|PassObject', 'recoger, sostener, soltar o entregar objetos'),
    (r'Wait|Pause', 'esperar una condición o un intervalo'),
    (r'Sit|Stand|Still|LieInBed|Sunbathe', 'mantener o cambiar una postura de reposo'),
    (r'Boat', 'interactuar con una embarcación'),
    (r'JetPack', 'usar el jetpack'),
    (r'UseGun', 'utilizar un arma'),
    (r'UseAtm', 'utilizar un cajero automático'),
    (r'UseMobilePhone', 'utilizar un teléfono móvil'),
    (r'Goggles', 'utilizar gafas de visión especial'),
    (r'SwatRope|Abseil', 'descender por una cuerda'),
    (r'Brain|Decision|Threat|Behaviour', 'controlar decisiones o comportamientos del personaje'),
    (r'Cower|Evasive|Duck|HandsUp', 'adoptar una postura defensiva o evasiva'),
    (r'Interior|House|Office|Shop', 'actuar en una casa, oficina o tienda'),
]


def subject(path):
    stem, parts = path.stem, path.parts
    key = stem.casefold()
    if key in SUBJECTS:
        return SUBJECTS[key]
    if stem.startswith(('TaskSimple', 'TaskComplex', 'TaskInterior', 'TaskGang')):
        kind = 'tarea compleja' if stem.startswith('TaskComplex') else 'tarea simple' if stem.startswith('TaskSimple') else 'tarea de IA'
        for pattern, action in TASK_ACTIONS:
            if re.search(pattern, stem):
                return f'la {kind} `{stem}`, relacionada con {action}'
        return f'la {kind} `{stem}`, una acción o secuencia ejecutada por peatones'
    if stem.startswith('Event') or 'Events' in parts:
        return f'el evento `{stem}` del sistema de IA, usado para comunicar situaciones y provocar respuestas de peatones'
    if stem.startswith('e') and len(stem) > 1 and stem[1].isupper():
        return f'la enumeración `{stem}`, que define identificadores o estados usados por el juego'
    if stem.startswith('Anim'):
        return f'el componente `{stem}` de organización, datos o mezcla de animaciones'
    if stem.startswith('FxInfo'):
        return f'el parámetro de efecto `{stem}`, utilizado para definir propiedades de partículas'
    if stem.startswith('FxInterp'):
        return f'la interpolación `{stem}` de valores de efectos a lo largo del tiempo'
    if 'Fx' in parts:
        return f'el componente `{stem}` del sistema de efectos visuales y partículas'
    if 'Pools' in parts or stem.endswith('Pool'):
        return f'la reserva `{stem}`, que administra instancias reutilizables de un tipo de objeto'
    if 'Ragdoll' in parts or stem.startswith(('Bone', 'IKChain')):
        return f'el componente `{stem}` de huesos o cadenas de cinemática inversa'
    if 'Models' in parts or 'modelinfo' in parts:
        return f'los metadatos de modelo `{stem}`, utilizados para cargar y representar tipos de objetos 3D'
    if 'Collision' in parts or 'collision' in parts:
        return f'el componente geométrico `{stem}` usado en detección y datos de colisiones'
    if 'Audio' in parts or 'audio' in parts:
        sub = next((s for p, s in [
            ('Entities', 'sonidos asociados a entidades o situaciones del juego'),
            ('Managers', 'coordinación de sonidos, música o pistas'),
            ('Hardware', 'canales y dispositivo de reproducción'),
            ('Loaders', 'carga o decodificación de datos de audio'),
            ('OpenAL', 'integración del audio con OpenAL'),
            ('Enums', 'identificadores de sonidos, voces o bancos'),
            ('Data', 'tablas de datos del motor de sonido'),
            ('Config', 'configuración de pistas de audio'),
        ] if p in parts), 'reproducción, configuración o soporte del sistema de audio')
        return f'`{stem}`: {sub}'
    if stem.endswith('Commands') or 'Commands' in parts:
        return f'los comandos de script `{stem}`, agrupados por su ámbito de actuación'
    if 'CommandParser' in parts:
        return f'`{stem}`: lectura de argumentos, despacho o escritura de resultados de comandos de scripts'
    if 'Tasks' in parts:
        return f'`{stem}`: infraestructura para asignar, organizar o ejecutar tareas de peatones'
    if 'Attractors' in parts or 'Attractor' in stem:
        return f'`{stem}`: puntos del entorno que atraen peatones y organizan su interacción'
    if 'Interior' in parts or 'Furniture' in stem:
        return f'`{stem}`: mobiliario, distribución o puntos de actividad de interiores'
    if 'DecisionMakers' in parts:
        return f'`{stem}`: reglas de selección de respuestas de la inteligencia artificial'
    if 'Conversations' in parts:
        return f'`{stem}`: participantes, nodos o gestión de conversaciones'
    if 'Frontend' in parts:
        return f'`{stem}`: parte del dibujo, entrada o procesamiento de los menús del juego'
    if 'Memory' in parts:
        return f'`{stem}`: administración de bloques de memoria y espacio libre'
    if 'Core' in parts:
        return f'`{stem}`: estructuras de datos o utilidades matemáticas compartidas por el motor'
    if 'Plant' in parts:
        return f'`{stem}`: vegetación y colocación procedural de objetos sobre superficies'
    if 'Pipelines' in parts:
        return f'`{stem}`: procesamiento gráfico especializado de geometría y materiales'
    if 'Plugins' in parts:
        return f'`{stem}`: extensión que asocia datos o comportamiento a objetos de RenderWare'
    if 'RenderWare' in parts or any(p in ('rw', 'fakerw') for p in parts):
        return f'`{stem}`: interfaz, recursos o utilidades del motor gráfico RenderWare'
    if stem.endswith('SaveStructure'):
        return f'los datos `{stem}` utilizados para guardar y restaurar estado de la partida'
    if stem.endswith('Scanner'):
        return f'`{stem}`: exploración de entidades cercanas para alimentar la percepción de la IA'
    if 'Text' in parts:
        return f'`{stem}`: datos, claves o búsqueda de textos del juego'
    folder = parts[0].casefold()
    if folder in FOLDER_DESCRIPTIONS:
        return f'el componente `{stem}` del área {folder}; {FOLDER_DESCRIPTIONS[folder][0].lower() + FOLDER_DESCRIPTIONS[folder][1:]}'
    return None


def describe_file(columns):
    # Prefer reSA's more detailed directory structure, retaining all links.
    path = next(col[0] for col in reversed(columns) if col)
    suffix = path.suffix.casefold()
    special = {
        '.hlsl': 'Programa de sombreado HLSL para procesar geometría o píxeles en la GPU.',
        '.vert': 'Shader de vértices: transforma geometría y prepara datos para dibujar.',
        '.frag': 'Shader de fragmentos: calcula el color final de los píxeles.',
        '.cso': 'Shader compilado utilizado por el backend gráfico.',
        '.inc': 'Fragmento incluido desde otro archivo; contiene código o datos auxiliares.',
        '.sh': 'Script de shell para operaciones auxiliares del proyecto.',
        '.cmd': 'Script de comandos de Windows para operaciones auxiliares del proyecto.',
        '.s': 'Rutinas escritas en lenguaje ensamblador.',
        '.dsm': 'Código o listado de ensamblador asociado al procesamiento gráfico.',
        '.ico': 'Recurso de icono de la aplicación.',
        '.rc': 'Definición de recursos de Windows, como iconos, diálogos y metadatos.',
        '.in': 'Plantilla utilizada para generar un archivo de configuración o código.',
        '.md': 'Documentación del subsistema donde se encuentra.',
        '.def': 'Lista de definiciones de datos incluida por el código del juego.',
    }
    if path.name == 'CMakeLists.txt':
        return 'Reglas de CMake para reunir fuentes y configurar la compilación.'
    if suffix in special:
        return f'{special[suffix]} Recurso: `{path.stem}`.'
    purpose = subject(path)
    if purpose is None:
        return f'Archivo `{path.name}` de la raíz del juego; propósito específico pendiente de revisión del contenido.'
    kind = 'Declaraciones y tipos de' if suffix in ('.h', '.hpp') else 'Implementación de'
    return f'{kind} {purpose.rstrip(".")}.'.replace(' de el ', ' del ')
