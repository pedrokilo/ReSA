<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DatosExe, auditoría de «dato en dos sitios» en G2, G3 y G4 (24/11/2026, rama `datos-g2g4`, base `origin/main` 48e53f065c)

Alcance: lotes DE02 (constantes `.rdata` por otras vías, 185 objetos), DE03 (cadenas y GUID `.rdata`, 30), DE04A/B/C/D/R/XA/XB (`.data` inicializado, 636 objetos con tipo de tamaño fijo en la tabla de verificación) y los objetos de DE04 que quedaron fuera por criterio (`DE04R`, 36 filas). Clase de fallo: el C++ escribe un objeto `DatosExe::*` y algo lee la dirección del EXE (o al revés), de modo que cada lado ve su copia.

## 1. Método

1. **Estático ampliado** (scripts de trabajo en `D:\Original\sim_out\datos1\`, no versionados). Citas por literal 0x858000-0xCB0FFF fuera de `DatosExe/` (exactas, interiores, con índice y ventanas de ±0x200 bytes); objetos referidos por alias (`static inline auto& x = DatosExe::...`); conversiones `reinterpret_cast<T*>(&objeto)` a un ancho mayor que el del objeto (ninguna); solapes entre los 867 objetos de los lotes (ninguno; 1 sin tamaño calculable) y direcciones con dos objetos (28, todas constantes `.rdata` duplicadas con `uint32_t` en R5 o declaraciones repetidas del mismo nombre; sin lector que las distinga). Lectores del EXE que pueden ejecutarse:
   * operandos de 4 bytes (exactos y con índice `[reg*N+base]`) en **todo** el código vivo: las 5.279 funciones de `docs/CodigoVivoExe/funciones-vivas.csv` y los 128 huecos ALCANZABLES de `docs/DatosExe/huecos-alcanzables.csv`, escaneo de bytes sin alineación;
   * palabras de `.rdata`/`.data` del EXE que apunten al objeto (o a su interior si es un arreglo): ninguna para los objetos mutables de G4.
2. **Dinámico** (nuevo, versionado): `source/game_sa/DatosExe/Verif.cpp`/`Verif.h` y `VerifTabla.cpp` (generado por `contrib/datos_exe_verif_gen.py`: 2.120 objetos de tamaño fijo y 30 cadenas). Con `DATOSEXE_VERIF=<registro>` un hilo guarda el valor de partida del objeto y de su dirección a los 300 ms de cargar el módulo (`DATOSEXE_VERIF_RETARDO`) y cada `DATOSEXE_VERIF_MS` compara el objeto con la memoria de su dirección (confirmación a 1 ms para descartar escrituras a medias). Registra `DIV`, `CAMBIO` (hasta 6) y `CADENA_DIV`. Sin la variable no hace nada. Sirve igual en el ASI (memoria = `gta-sa.exe`) y en el independiente (memoria = imagen de datos); en este último las direcciones de las variables no se resuelven por el `.map`: la tabla toma `&objeto` en compilación.
   Una divergencia **no** es por sí sola un fallo: lo es si algo lee el otro lado. Por eso se cruza con el estático.

## 2. Ejecuciones

| Ejecución | Qué | Resultado |
| --- | --- | --- |
| ASI, `prueba_ropero.py` ×3 (`v1`/`v2`/`v4`) y `prueba_marcadores.py` (`v3`) | arranque + partida + casa + ropero, ~35 s de registro por prueba | BIEN/BIEN; 540-554 objetos divergentes de 2.120 |
| Independiente (`D:\Original\_datos1_run`, 120 s, `DATOSEXE_VERIF`) | partida nueva | estado de juego 9, 2.678 fotogramas, `excepciones=0`, `codigo-original-distinto=0` (el `exit=0xDEAD0002` es la guardia de tiempo); 585 divergentes |
| Cadenas DE03 | `strcmp` del texto propio con el de la dirección | 30 de 30 iguales |

Cobertura de los lotes: de 847 direcciones de DE02 + DE04 (sin DE03), 811 tienen objeto de tamaño fijo en la tabla; el resto son los 36 objetos ya devueltos a `StaticRef` por DE04R. **No se condujo un coche ni se ejecutó el tráfico durante el registro** (solo casa/ropero/marcadores y el arranque de la partida), de modo que una escritura que solo ocurra conduciendo no se habría visto en la parte dinámica.

## 3. Hallazgos

### 3.1 Reales (corregidos)

| Objeto | Dirección | Evidencia | Corrección |
| --- | --- | --- | --- |
| `v8CD76C` = `MI_HYDRAULICS` (`ModelIndex`, DE04D) | 0x8CD76C | Dinámica: `InitModelIndices` escribe el objeto (`0x043F`) y la memoria queda en `0xFFFF` durante toda la partida (`DIV t=6516 0x008CD76C ... obj=3F04 mem=FFFF`, ASI, `v2.log`). Estática: el despachador de comandos de script del EXE (hueco ALCANZABLE 0x470A8C) la lee en 0x4732EA (`movzx eax, word [0x8CD76C]; push eax; call 0x6DF930`), es decir, el comando de script que usa ese modelo recibiría -1 | `ModelIndices.cpp`: `MI_HYDRAULICS = StaticRef<ModelIndex>(0x8CD76C)`; objeto retirado de `DataInicializadaD.*` y anotado en `lotes/DE04R.csv` |
| `v8D37D5` = `CWaterLevel::m_bWaterFogScript` (bool, DE04B/XB) | 0x8D37D5 | Solo estática: el despachador de scripts del EXE lo **escribe** a 0 en 0x47309A (`mov byte [0x8D37D5], 0`, hueco ALCANZABLE) mientras el C++ lo lee por el objeto (valor 1); el comando de script que apaga la niebla de agua no tendría efecto. **No se observó divergencia en ejecución** porque ese comando de script no se ejecutó | `WaterLevel.h`: `StaticRef<bool>(0x8D37D5)`; objeto retirado de `DataInicializadaXB.*` (ya figuraba en `DE04R.csv`) |

La corrección de ambos no se ha visto «en juego» (ninguna de las dos órdenes de script se ejecuta en la partida nueva): la evidencia es la de la tabla.

### 3.2 Descartados (divergen o tienen lector en código reemplazado, sin lector vivo)

* 151-157 objetos G2/G4 de `.data` divergen en ejecución con el patrón «el objeto cambia, la memoria no» (MI_*, `desiredNumVehiclesLoaded` 22 frente a 12, `ms_lodDistScale` 1,8 frente a 1,2, globales de CdStream…): ninguno tiene cita por literal que lo cubra, ni operando en código vivo o huecos alcanzables, ni puntero en datos del EXE; los lectores del EXE que existen (p. ej. 0x40B9A0 `StreamVehiclesAndPeds`, 0x5B8E70 `Init2`) están dentro de funciones reemplazadas.
* En `.rdata` (366 constantes en la tabla) **ningún objeto difiere** de su dirección: los valores extraídos son exactos.
* Los 14 objetos de `DE04R` que siguen en `origin/main` (`v8CD800`, `v8CD804`, `v8D12BC/C0/C4/CC/D0`, `v8D2358`, `v8D2E30`, `v8D37D0`, `v8D5038`, `v8D60BC`, `v8E3F00`, `v8E3FE0`): la marca «viva» venía de referencias en cuerpos de funciones ya reemplazadas; no hay lector vivo.
* Tablas con base literal e índice (Cam 0x8CC41C/0x8CC430/0x8CC4C0…, GangWars 0x8D1344/0x8D1350/0x8D135C, ClothesBuilder 0x8D0A90, WaterLevel 0x8D390C, Train 0x8D44F8, Radio, PostEffects 0x8D5190…): se revisó el rango de índices a mano en los casos con objetos propios a menos de 0x60 bytes; son tablas solo de lectura o no alcanzan ningún objeto.

### 3.3 Fuera de mi alcance, avisos para quien lleve G5 (`.bss`)

La comparación dinámica los vio igual en el ASI y en el independiente (excepto donde se indica):

* `bC17738` (`float[2]`) y `bC17780` (`uint8[2]`), DE05E5: `TaskSimpleUseGun.cpp:620-622` los lee por el objeto (ceros); la memoria cambia a `0.2415f ×2` y `{1,1}` a los ~29 s, tanto en el ASI como en el independiente. Están justo detrás de `CTaskSimpleFight::m_aComboData` (0xC170D0, 12 × `CMeleeInfo` de 0x88 bytes → termina en 0xC17730): sospecha de arreglo con más elementos que el declarado.
* `bA9B76C`, `bB61364`, `bB62C1C`, `bC0B184`, `bC0F074`, `bC0F240`: sin uso por nombre (objetos huérfanos de la auditoría estática de 23/11); la memoria sí cambia.
* `bBAA410`, `bBAA430` (`CHud::m_LastWeapon`, `m_LastDisplayScore`): la memoria cambia solo en el ASI (código original de `CHud::DrawPlayerInfo`), no en el independiente.

## 4. Límites

* No se condujo ni se midió tráfico ni scripts concretos durante el registro dinámico (ver §2).
* Divergencia del objeto frente a una dirección con lector **por registro base** (`[esi+0x14]` sobre un puntero a la variable) no se ve: el análisis estático solo ve operandos absolutos o con índice.
* `StaticInit.cpp` y los inicializadores `_initterm` quedan fuera (G5).
* De DE03 se compararon por texto 30 cadenas; los 2 GUID entran en la tabla genérica de bytes (iguales).
