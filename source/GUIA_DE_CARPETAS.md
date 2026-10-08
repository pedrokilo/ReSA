<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Guía de las carpetas de `source`

Esta guía explica todas las carpetas y subcarpetas presentes en `source` al 26 de septiembre de 2026. Las rutas de las tablas son relativas a `source/` y enlazan a las carpetas correspondientes.

Aquí conviven el código reconstruido de GTA San Andreas, la integración con Windows, las herramientas de depuración y las utilidades del proyecto. La existencia de una carpeta o clase no significa que toda su implementación esté reconstruida, habilitada o validada. Para consultar el avance, véase [el informe de clases](../docs/ReversedClasses.md). Para comprobar que los hooks activos y el catálogo siguen sincronizados, usar `python contrib/audit_hook_catalog.py --strict` desde la raíz.

## Vista general

| Carpeta | Para qué sirve |
| --- | --- |
| [app](app/) | Capa de aplicación: inicialización, ciclo del juego, entrada, cámara y conexión con la plataforma. Ejemplos: `app.cpp`, `app_game.cpp` y `app_input.cpp`. |
| [extensions](extensions/) | Utilidades añadidas por el proyecto: configuración, conversiones, referencias a entidades, tipos auxiliares y funciones reutilizables. |
| [game_sa](game_sa/) | Núcleo del juego reconstruido en C++. Contiene los subsistemas organizados en las carpetas descritas más abajo y numerosas clases directamente en su raíz. |
| [libs](libs/) | Código de apoyo de bibliotecas externas incluido en este árbol. Actualmente contiene las integraciones locales de ImGui. |
| [oswrapper](oswrapper/) | Interfaz para servicios del sistema operativo: archivos, hilos, mutex y semáforos. `oswrapper.h` declara la interfaz y `oswrapper_win.cpp` contiene la implementación para Windows. |
| [reversiblebugfixes](reversiblebugfixes/) | Registro de correcciones de errores con estado activado/desactivado y metadatos. `Bugs.hpp` declara las correcciones; `ReversibleBugFix.hpp` define su estructura, incluidos bloqueos y requisitos de reinicio. |
| [reversiblehooks](reversiblehooks/) | Infraestructura para redirigir funciones originales a sus implementaciones reconstruidas y gestionar el estado de esas redirecciones, llamadas *hooks*. También organiza los hooks por categorías. |
| [toolsmenu](toolsmenu/) | Interfaz de herramientas y depuración (ImGui): renderizado de ventanas, entrada del menú y controles compartidos. **Desactivada desde el 04/10/2026:** el menú F7 no se crea, no dibuja ni recibe entrada, para que el juego se comporte como el EXE. |

## Aplicación y utilidades

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [app/platform](app/platform/) | Abstracción de plataforma y servicios usados por la aplicación. Incluye `platform.cpp`, vistas predefinidas y la integración `SDLWrapper`. |
| [app/platform/win](app/platform/win/) | Código específico de Windows: punto de entrada `WinMain`, mensajes de ventana, entrada, modos de vídeo y funcionamiento en ventana. |
| [app/platform/win/VideoPlayer](app/platform/win/VideoPlayer/) | Reproducción de vídeo en la plataforma Windows, encapsulada en `VideoPlayer.cpp` y su cabecera. |
| [app/securom](app/securom/) | Rutinas de visualización de estado relacionadas con SecuROM. El código presente inicializa y dibuja indicadores de tipo `Trigger` y `Trap`. |
| [extensions/Configs](extensions/Configs/) | Definiciones de opciones de configuración del proyecto, como `FastLoader.hpp` y `Miscellaneous.hpp`. |
| [extensions/Shapes](extensions/Shapes/) | Figuras geométricas auxiliares añadidas por el proyecto. Actualmente contiene `AngledRect`, un rectángulo orientado. |

## Subsistemas principales del juego

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [game_sa/Animation](game_sa/Animation/) | Animaciones: jerarquías, fotogramas, asociaciones, grupos y mezcla de movimientos. Ejemplos: `AnimBlendAssociation` y `AnimBlendHierarchy`. |
| [game_sa/Attractors](game_sa/Attractors/) | Puntos de interacción que atraen a peatones y organizan su uso: asientos, cajeros, refugios, paradas y puntos controlados por scripts. |
| [game_sa/Audio](game_sa/Audio/) | Motor de sonido, sonidos individuales, entorno acústico, zonas de audio e hilos de reproducción y transiciones. Sus subcarpetas separan datos, dispositivos y comportamiento. |
| [game_sa/Collision](game_sa/Collision/) | Detección de colisiones y sus datos geométricos: cajas, líneas, modelos y otras primitivas. Ejemplos: `Collision`, `CollisionData` y `BoundingBox`. |
| [game_sa/Conversations](game_sa/Conversations/) | Gestión de conversaciones entre peatones y con el jugador: participantes, nodos y estado de las conversaciones. |
| [game_sa/Core](game_sa/Core/) | Estructuras y operaciones básicas compartidas por el juego, como matrices, listas enlazadas y generación de claves. |
| [game_sa/Data](game_sa/Data/) | Definiciones de datos incluidas en el código. Actualmente contiene `Weather.def`, relacionado con el clima; no es la carpeta de recursos externos del juego. |
| [game_sa/DecisionMakers](game_sa/DecisionMakers/) | Reglas de decisión de la inteligencia artificial: qué respuesta debe elegir un peatón ante una situación o evento y cómo se cargan esas reglas. |
| [game_sa/Entity](game_sa/Entity/) | Jerarquía de entidades del mundo. Incluye bases como `Entity`, `Placeable` y `Physical`, además de edificios; las entidades especializadas tienen sus propias subcarpetas. |
| [game_sa/Enums](game_sa/Enums/) | Enumeraciones compartidas: identificadores de animaciones, cámaras, huesos, eventos de audio, misiones de vehículos y otras categorías. |
| [game_sa/Events](game_sa/Events/) | Eventos que alimentan las reacciones de la inteligencia artificial, como colisiones, relaciones entre peatones o interacción con atractores. |
| [game_sa/Frontend](game_sa/Frontend/) | Pantallas y lógica de los menús del juego: dibujo, entrada, páginas, galería y procesamiento de `MenuManager`. |
| [game_sa/Fx](game_sa/Fx/) | Efectos visuales y partículas: emisores, sistemas, primitivas y sus parámetros; incluye también `CarFXRenderer`. |
| [game_sa/Handling](game_sa/Handling/) | Tipos relacionados con las propiedades de conducción de vehículos. Actualmente contiene `HandlingDataType.h`; otros archivos de handling permanecen en la raíz de `game_sa`. |
| [game_sa/Interior](game_sa/Interior/) | Gestión de interiores y mobiliario: grupos, elementos, colocación y puntos de interacción usados por los personajes. |
| [game_sa/Memory](game_sa/Memory/) | Gestión de memoria mediante bloques y montículos (*heaps*). Ejemplos: `MemoryHeap`, `HeapBlockDesc` y `HeapFreeBlockDesc`. |
| [game_sa/Models](game_sa/Models/) | Información de los modelos del juego: propiedades comunes y especializaciones para peatones, vehículos, objetos y niveles de detalle. Aquí están las clases de gestión, no los archivos de modelos 3D. |
| [game_sa/Pipelines](game_sa/Pipelines/) | Rutas especializadas de renderizado y utilidades comunes para dibujar edificios y vehículos con sus materiales y efectos. |
| [game_sa/Plant](game_sa/Plant/) | Soporte para vegetación y colocación procedural de objetos sobre superficies: triángulos, referencias de colisión y administración de objetos generados. |
| [game_sa/Plugins](game_sa/Plugins/) | Extensiones integradas con RenderWare para asociar datos y comportamientos a sus objetos. No es una carpeta de instalación de mods ASI o CLEO. |
| [game_sa/Pools](game_sa/Pools/) | Definiciones de reservas de objetos reutilizables (*pools*) para peatones, vehículos, edificios, eventos, rutas y otros elementos. |
| [game_sa/Ragdoll](game_sa/Ragdoll/) | Nodos de huesos y cadenas de cinemática inversa (IK), con sus gestores. El contenido actual permite trabajar con objetivos, límites y movimientos de huesos; el nombre no implica por sí solo un sistema completo de física ragdoll. |
| [game_sa/RenderWare](game_sa/RenderWare/) | Integración con RenderWare y recursos gráficos Direct3D, incluidos búferes de índices, texturas y gestión de recursos. |
| [game_sa/Scripts](game_sa/Scripts/) | Sistema de scripts del juego: ejecución de instrucciones, variables y parámetros, estructuras SCM y efectos controlados por scripts. Ejemplos: `RunningScript` y `TheScripts`. |
| [game_sa/Tasks](game_sa/Tasks/) | Sistema de tareas de los peatones: tareas simples y complejas, secuencias, administración y relación con órdenes de scripts. |
| [game_sa/Text](game_sa/Text/) | Carga y búsqueda de textos del juego mediante claves, datos de texto y desplazamientos de textos de misión. Ejemplos: `Text`, `KeyArray` y `MissionTextOffsets`. |

Muchos sistemas todavía están directamente en `game_sa/`, sin una carpeta propia. Para localizar una función concreta conviene buscar también los archivos de esa raíz, no únicamente las subcarpetas de esta tabla.

## Audio: subcarpetas

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [game_sa/Audio/Config](game_sa/Audio/Config/) | Configuración de pistas de radio para PC en `RadioStreamsPC.h`. |
| [game_sa/Audio/Data](game_sa/Audio/Data/) | Tablas usadas por el motor de audio, como la de atenuación del sonido en `SoundAttenuationTable.h`. |
| [game_sa/Audio/Entities](game_sa/Audio/Entities/) | Comportamiento sonoro asociado a entidades y situaciones: vehículos, peatones, puertas, armas, colisiones, fuego y explosiones. |
| [game_sa/Audio/Enums](game_sa/Audio/Enums/) | Identificadores de bancos y sonidos, tipos de audio de vehículos, voces y contextos de diálogo. |
| [game_sa/Audio/Hardware](game_sa/Audio/Hardware/) | Capa de canales y dispositivo de audio: canales estáticos, canales de streaming y administración del hardware. |
| [game_sa/Audio/Loaders](game_sa/Audio/Loaders/) | Lectura de bancos y pistas, flujos de datos y decodificación de audio. Incluye cargadores MP3 y el decodificador basado en Media Foundation. |
| [game_sa/Audio/Managers](game_sa/Audio/Managers/) | Coordinación de sonidos y pistas por función: ambiente, radio, cinemáticas, radio del usuario y gestión general de sonidos. |
| [game_sa/Audio/OpenAL](game_sa/Audio/OpenAL/) | Soporte de OpenAL mediante objetos de fuente y búfer. Su inclusión depende de la opción de compilación `WITH_OPENAL`. |

## Entidades: subcarpetas

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [game_sa/Entity/Dummy](game_sa/Entity/Dummy/) | Representaciones simplificadas de entidades, con clases como `Dummy`, `DummyObject` y `DummyPed`. |
| [game_sa/Entity/Object](game_sa/Entity/Object/) | Objetos del mundo y especializaciones, como proyectiles, objetos de cinemáticas y objetos de mano. |
| [game_sa/Entity/Ped](game_sa/Entity/Ped/) | Personajes: peatón base, civiles, policías, personal de emergencias y jugador, además de datos de guardado, huesos y apariencia. |
| [game_sa/Entity/Vehicle](game_sa/Entity/Vehicle/) | Vehículos y sus variantes: automóviles, motos, bicicletas, barcos, helicópteros, aviones, quads y otros tipos. |

## Efectos, renderizado y extensiones de RenderWare

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [game_sa/Fx/Blueprints](game_sa/Fx/Blueprints/) | Definiciones de propiedades de efectos, como color, dirección, textura animada y fuerzas de atracción. Se representan mediante las clases `FxInfo`. |
| [game_sa/Fx/Blueprints/Interp](game_sa/Fx/Blueprints/Interp/) | Interpolación de valores de los efectos, para calcular cómo cambian sus propiedades a lo largo del tiempo. Incluye distintas representaciones numéricas. |
| [game_sa/Pipelines/CustomBuilding](game_sa/Pipelines/CustomBuilding/) | Renderizado especializado de edificios, incluida la variante de iluminación diurna/nocturna `CustomBuildingDNPipeline`. |
| [game_sa/Pipelines/CustomCarEnvMap](game_sa/Pipelines/CustomCarEnvMap/) | Renderizado de vehículos con mapas de entorno y componentes especulares: reflejos y brillo de materiales. |
| [game_sa/Plugins/BreakablePlugin](game_sa/Plugins/BreakablePlugin/) | Datos y operaciones de la extensión para geometría rompible. |
| [game_sa/Plugins/CollisionPlugin](game_sa/Plugins/CollisionPlugin/) | Registro de la extensión de colisión de RenderWare y asociación con la información del modelo correspondiente. |
| [game_sa/Plugins/JPegCompressPlugin](game_sa/Plugins/JPegCompressPlugin/) | Compresión de imágenes de cámara a JPEG y descompresión para su uso gráfico; incluye escritura a archivo o búfer. |
| [game_sa/Plugins/NodeNamePlugin](game_sa/Plugins/NodeNamePlugin/) | Asociación y consulta de nombres de nodos en la jerarquía de RenderWare. |
| [game_sa/Plugins/PipelinePlugin](game_sa/Plugins/PipelinePlugin/) | Datos de identificación de pipelines asociados a objetos de RenderWare. |
| [game_sa/Plugins/RpAnimBlendPlugin](game_sa/Plugins/RpAnimBlendPlugin/) | Integración de la mezcla de animaciones con objetos y jerarquías de RenderWare. Complementa las clases de `Animation`. |
| [game_sa/Plugins/TwoDEffectPlugin](game_sa/Plugins/TwoDEffectPlugin/) | Definición y serialización de efectos 2D asociados a modelos, junto con sus estructuras de datos. |
| [game_sa/RenderWare/rw](game_sa/RenderWare/rw/) | Cabeceras y código de interfaces de RenderWare y sus extensiones: animación jerárquica, materiales, colisión, filtrado anisotrópico y otras funciones gráficas. |

## Scripts y tareas de inteligencia artificial

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [game_sa/Scripts/CommandParser](game_sa/Scripts/CommandParser/) | Infraestructura para interpretar argumentos de instrucciones, convertirlos a tipos C++ y guardar resultados. Incluye `Parser.hpp`, `ReadArg.hpp`, `StoreArg.hpp` y generación de tablas de búsqueda. |
| [game_sa/Scripts/Commands](game_sa/Scripts/Commands/) | Implementaciones de comandos u opcodes, organizadas por tema: cámara, personajes, reloj, matemáticas, misiones, objetos y otros sistemas. |
| [game_sa/Scripts/Commands/CLEO](game_sa/Scripts/Commands/CLEO/) | Implementaciones y registro de comandos de extensión CLEO, como acceso a archivos, memoria, bibliotecas dinámicas y audio. Es código C++ del proyecto; su habilitación está asociada a `WITH_CLEO_COMMANDS`. |
| [game_sa/Tasks/Allocators](game_sa/Tasks/Allocators/) | Asignación de tareas a personajes, especialmente en comportamientos de grupo, combate y órdenes del jugador. |
| [game_sa/Tasks/Allocators/PedGroup](game_sa/Tasks/Allocators/PedGroup/) | Asignadores predeterminados para grupos de peatones: seguir, conversar, permanecer quietos, actuar aleatoriamente o sentarse en el coche del líder. |
| [game_sa/Tasks/TaskTypes](game_sa/Tasks/TaskTypes/) | Implementaciones concretas de comportamiento: caminar, conducir, combatir, detener a alguien, usar objetos y muchas otras acciones simples o compuestas. |
| [game_sa/Tasks/TaskTypes/Interior](game_sa/Tasks/TaskTypes/Interior/) | Tareas propias de interiores: estar en una casa, oficina o tienda; acostarse, sentarse en un escritorio y atender como dependiente. |
| [game_sa/Tasks/TaskTypes/SeekEntity](game_sa/Tasks/TaskTypes/SeekEntity/) | Tareas para acercarse a una entidad objetivo mediante distintas estrategias, distancias y desplazamientos. |
| [game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators](game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/) | Cálculo del punto de destino de las tareas anteriores: posición fija, posición de la entidad o desplazamientos definidos por coordenadas, radio y ángulo. |

Como orientación para seguir el comportamiento de un peatón: `Events` representa lo que sucede, `DecisionMakers` ayuda a seleccionar una respuesta y `Tasks` contiene las acciones que se ejecutan. `Entity/Ped` contiene al personaje sobre el que actúan esos sistemas.

## Bibliotecas, hooks y herramientas de depuración

| Carpeta | Para qué sirve y qué contiene |
| --- | --- |
| [libs/imgui](libs/imgui/) | Agrupa las integraciones y utilidades locales de Dear ImGui usadas por la interfaz de depuración. |
| [libs/imgui/bindings](libs/imgui/bindings/) | Adaptadores de ImGui para DirectX 9 y SDL3: conectan el dibujo y los eventos de plataforma con la interfaz. |
| [libs/imgui/misc](libs/imgui/misc/) | Agrupación de utilidades complementarias de ImGui; actualmente contiene la integración con tipos de C++. |
| [libs/imgui/misc/cpp](libs/imgui/misc/cpp/) | Adaptación de controles de texto de ImGui a `std::string`, mediante `imgui_stdlib.cpp` y su cabecera. |
| [reversiblehooks/ReversibleHook](reversiblehooks/ReversibleHook/) | Tipos concretos de hooks: base común, funciones normales, funciones virtuales y comandos de scripts. |
| [toolsmenu/DebugModules](toolsmenu/DebugModules/) | Paneles de inspección y control de subsistemas del juego, como colisiones, trucos, nubes, puntos de control y correcciones. Su `!README.md` explica cómo añadir un módulo. |
| [toolsmenu/DebugModules/Audio](toolsmenu/DebugModules/Audio/) | Paneles de depuración de audio, sonidos y gestores de pistas de ambiente, cinemáticas y radio del usuario. |
| [toolsmenu/DebugModules/Audio/Entities](toolsmenu/DebugModules/Audio/Entities/) | Inspección de entidades de audio específicas, actualmente vehículos y escáner policial. |
| [toolsmenu/DebugModules/Script](toolsmenu/DebugModules/Script/) | Herramientas de depuración relacionadas con scripts; actualmente incluye el módulo de misiones `MissionDebugModule`. |

## Archivos de orientación en la raíz de `source`

Estos archivos no son carpetas, pero ayudan a entender cómo se conectan las anteriores:

| Archivo | Función |
| --- | --- |
| [CMakeLists.txt](CMakeLists.txt) | Recopila las fuentes y configura la compilación y sus opciones, incluidas SDL3, OpenAL y comandos CLEO. |
| [dllmain.cpp](dllmain.cpp) | Punto de entrada e integración del módulo cargado en el proceso. |
| [InjectHooksMain.cpp](InjectHooksMain.cpp) | Centraliza el registro e instalación de hooks de los distintos sistemas. |
| [HookSystem.h](HookSystem.h) | Interfaz de apoyo para el sistema de hooks. |
| [Base.h](Base.h) y [StdInc.h](StdInc.h) | Definiciones e inclusiones compartidas por gran parte del código. |
| [PluginBase.h](PluginBase.h) y [PluginBase.cpp](PluginBase.cpp) | Utilidades de integración con el ejecutable, sus direcciones y llamadas. |
| [config.h](config.h) | Configuración común del código. |
| [notsa.natvis](notsa.natvis) | Visualizadores de tipos para facilitar su inspección en el depurador de Visual Studio. |

## Dónde empezar según lo que buscas

| Objetivo | Carpetas principales |
| --- | --- |
| Entender al jugador o a los NPC | `game_sa/Entity/Ped`, `game_sa/Tasks`, `game_sa/Events` y `game_sa/DecisionMakers`. |
| Revisar vehículos | `game_sa/Entity/Vehicle`, `game_sa/Handling` y archivos relacionados de la raíz de `game_sa`. |
| Seguir un opcode | `game_sa/Scripts/Commands`, `game_sa/Scripts/CommandParser` y `game_sa/Scripts`. |
| Investigar sonidos o radio | `game_sa/Audio`, especialmente `Entities`, `Managers`, `Loaders` y `Hardware`. |
| Investigar animaciones o movimiento de huesos | `game_sa/Animation`, `game_sa/Ragdoll` y `game_sa/Plugins/RpAnimBlendPlugin`. |
| Revisar gráficos o efectos | `game_sa/Fx`, `game_sa/Pipelines`, `game_sa/RenderWare` y `game_sa/Plugins`. |
| Revisar ventana, controles o inicio | `app`, `app/platform` y `app/platform/win`. |
| Añadir una herramienta de inspección | `toolsmenu/DebugModules`. |
| Entender cómo se sustituye una función original | `reversiblehooks` y `InjectHooksMain.cpp`. |
