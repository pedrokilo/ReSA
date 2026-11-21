<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan de sustitución de las raíces de código (lote PLAN1, 18/11/2026)

[Informe IND0A](README.md) · [Tabla de raíces (CSV)](raices.csv) · [Lotes (CSV)](lotes-sustitucion.csv) · [Herramientas](../../reversing/FnPLAN1/README.md) · [Plan de validación de RenderWare](../RenderWareExe/plan-validacion.md)

> Solo planificación y análisis estático: no se ejecutó el juego, no se compiló y no se tocó ninguna fuente de producción. Todo se midió sobre el mismo ASI Debug de IND0A (HEAD `39276bea85`) y sobre el EXE original (SHA256 `72ae59e4…ecc9b`, solo lectura). Los números de línea de «quién la llama» salen de este árbol (`55597159b8`) y pueden moverse unas líneas respecto del Debug medido.

## Resumen

1. **788 raíces, no 779.** IND0A dejó 779 raíces seguras y probables; la revisión manual encontró **9 funciones reales que había clasificado como ruido** (se usan como puntero a función en el código propio) y las suma. Base del plan: **788 raíces abiertas → 0**.
2. **Reparto (788):** juego 94 + 2 excluidas = **96**; RenderWare interno **581**; «no reconstruibles» **31**; biblioteca **80** (CRT 57, D3DX9 19, libpng+zlib 2, QuickTime 2). Los 111 «ruido» de IND0A quedan en 102 (1 es un gancho propio de interior sin catalogar y 101 son constantes o interior de función).
3. **Corrección a IND0A:** las **31 «no reconstruibles» no son D3DX9/3DNow!: son los 31 puentes de QuickTime/QTML** (26 bytes cada uno, de `0x818430` a `0x81DAA0`). Las 19 de D3DX9 y las 2 de libpng/zlib son las «enlazadas». Casi todas las raíces **de RenderWare interno no son llamadas a la API**: ~210 son **callbacks registrados por dirección** (constructor/destructor/copia/lectura/escritura/tamaño de plugin, comparadores de `qsort`, funciones de memoria del motor).
4. **Las raíces son hojas en su mayoría:** 613 de 676 (91 %) no arrastran ninguna función más (cierre = ella misma). Solo **5 de 677** tienen alguna llamadora directa dentro del EXE (las demás solo las usa código propio): **casi ninguna necesita hook**; cerrarlas es «cuerpo propio + apuntar el código propio a él», validado contra el original que sigue cargado.
5. **Salen 33 lotes S1A–S33A (677 raíces) + SQA (31 puentes) + LIB (80, lote aparte).** Juego S1A–S4A (96), RenderWare S5A–S33A (581: oleada 1 puras 271 en 15 lotes, oleada 2 flujos y memoria 282 en 12 lotes, oleada 3 dispositivo D3D 28 en 2 lotes).
6. **Lo que se gana:** cerrar los 33 lotes baja el cierre de código de IND0A de **1.586 a 856 funciones, pero solo de 579 KB a 458 KB (−21 %)**: los lotes S quitan 730 funciones y ~121 KB; lo que queda (825 funciones, 457 KB, el 79 % de los bytes de hoy) son **bibliotecas** (CRT, D3DX9, libpng+zlib, QuickTime), que van en lotes aparte.
7. **Primer lote recomendado: S7A** (RpAnisot, 8 callbacks de plugin, 233 bytes, todos de arranque, sin x87, sin dependencias) y, en paralelo, **S1A** (13 funciones del juego sin infraestructura nueva).
8. **Esfuerzo relativo:** ~38 «lotes equivalentes» de sustitución más ~4,5 de infraestructura; sumados a la validación pendiente de RenderWare (663 entradas, ~27 lotes) son ~70 si se hacen por separado y ~52 si se **combinan por módulo** (estimación, ver sección 3.4).
9. **Decisiones del usuario (sección 4):** QuickTime/radio de usuario, D3DX9 (DLL del sistema o no), libjpeg (a mano o con las fuentes IJG), qué es «independiente» respecto a los datos, `qsort` exacto, cuándo dejar de validar contra el original, y confirmar la base de 788.

---

## 1. Tabla de raíces

Fichero: [`raices.csv`](raices.csv), **890 filas** (las 779 de IND0A + las 111 de ruido de IND0A; 9 de ellas promovidas) generadas por [`reversing/FnPLAN1/raices.py`](../../reversing/FnPLAN1/raices.py) y completadas con lote y tipo de trabajo por `lotes.py`.

| Columna | Qué es |
|---|---|
| `direccion`, `nombre` (+`fuente_nombre`) | Dirección en el EXE; nombre solo si hay (Ghidra/símbolos o una cadena ancla). Casi ninguna lo tiene: Ghidra no las tomó como función. |
| `modulo` (+`fuente_modulo`) | Clase/módulo: mapa de RenderWare o del juego, si no el archivo llamador, si no la vecindad en el mapa. Para bibliotecas: CRT, D3DX9, libpng+zlib, QuickTime/QTML. |
| `bytes` (+`fuente_bytes`) | Tamaño: Ghidra si lo tiene, si no recorrido de flujo con capstone. |
| `categoria` | juego · RenderWare interno · RenderWare no reconstruible · biblioteca: CRT/D3DX9/libpng+zlib/QuickTime · excluida · ruido |
| `confianza` | segura (A) · probable (B) · probable (promovida de ruido) · ruido (C) |
| `llamadas_desde`, `llamadoras_fuente`, `texto_fuente` | Objetos que la escriben a mano y archivo:línea de las apariciones en código (no comentarios). |
| `tipo_uso`, `forma_uso` | `envoltura plugin::Call` · `puntero por direccion fija` (llamada con cast o puntero guardado/pasado). `mov_call`=llamada, `push`/`imm`=puntero. |
| `funcion_propia_sin_hook` | Excluida (cerrada por inspección) · hook propio ya instalado fuera de `hooks.csv` · thunk a otra dirección (con o sin hook). |
| `cierre_total`, `cierre_bytes`, `cierre_exclusivo` | Funciones del EXE (y bytes) que arrastra por llamadas directas (parando en las que tienen hook) y las que **solo** arrastra esta raíz. |
| `raices_de_las_que_depende`, `llamadas_indirectas`, `flags_cuerpo`, `flags_cierre` | Dependencias entre raíces; llamadas indirectas sin seguir; `x87`, `seh`, `sse` en el cuerpo y en el cierre. |
| `llamadoras_en_exe(_sin_hook)` | Sitios de llamada directa desde el propio EXE (Ghidra) y cuántos de ellos están en funciones sin hook. |
| `lote`, `oleada_validacion`, `tipo_trabajo`, `arranque` | Lote asignado; oleada de `plan-validacion.md` (solo RenderWare); qué hay que hacer; si se registra al arrancar el motor (heurística por el código que la pasa). |
| `veredicto_auto`, `prev`, `prologo`, `es_thunk`, `primeras` | Comprobación estructural contra el EXE (ver 1.3). |

### 1.1 Reparto

| Categoría | Segura | Probable | Promovida | Total |
|---|---:|---:|---:|---:|
| juego | 26 | 65 | 3 | 94 |
| excluida (`0x4CCE60`, `0x4A2BB0`) | 2 | – | – | 2 |
| RenderWare interno | 385 | 192 | 4 | 581 |
| RenderWare no reconstruible (puentes QTML) | 31 | – | – | 31 |
| biblioteca: CRT y bibliotecas estáticas | 52 | 4 | 1 | 57 |
| biblioteca: D3DX9 | 19 | – | – | 19 |
| biblioteca: libpng + zlib | 2 | – | – | 2 |
| biblioteca: QuickTime (gancho `0x819700`, stub `0x8194D0`) | – | 1 | 1 | 2 |
| **Total** | **517** | **262** | **9** | **788** |
| ruido (IND0A C) | | | | 102 |

Cuadra con IND0A: 93 de juego (26+2 excluidas seguras, 65 probables), 578 RenderWare internas (577 + el gancho QuickTime `0x819700`, que el barrido por dirección ponía en RenderWare), 31+21, 56 CRT.

Hallazgos que cambian el plan:

- **Juego (96): 58 son callbacks de libjpeg** (`0x5C2FF0`–`0x5CF690`): los `jinit_*` reconstruidos (≈105 enganchados y validados) guardan por dirección las rutinas de método (`start_pass`, upsample, Huffman, memoria, errores) y esas rutinas siguen siendo del EXE. Las otras 38: envolturas (`Physical`, `Vehicle`, `Ped`, `Camera`, `Garages`, `Lines`, `Sprite2d`, `AudioZones`…), callbacks de animación de tareas, `RwHelper` y 4 hooks de entrada de `WinInput`/`CPad` **ya instalados con cuerpo propio pero fuera de `hooks.csv`** (`0x541DD0`, `0x7469A0`, `0x746D80`, `0x747020`: solo hay que catalogarlos y validarlos).
- **RenderWare interno (581): 206 son callbacks registrados por dirección** (plugin ctor/dtor/copy/stream, `qsort`, `memoryFuncs`, `fileFuncs`, convertidores de formato de píxel `0x7FFBE0`…); 398 son envolturas «llamada con cast» a funciones de la API (`RpClumpGetNumLights`, `RwCameraGetClump`…). Por módulo: RpWorld 210, RwCore 139, RwPlCore 71, RpMatFX 45, RpHAnim 26, RpUVAnim 26, RpSkin 25, RtAnim 20, RpAnisot 8, RtDict 6, RtQuat 2, RtBMP 1, RwTexDict 1, RwD3D9 1. En total solo **79 KB** para 581 funciones (mediana 56 bytes; 240 de ≤ 32 bytes; la mayor, 5.801 bytes).
- **Dependencias entre raíces: solo 62 aristas entre las 677 de los lotes S** (las otras 521 aristas del grafo son de bibliotecas, sobre todo hacia el CRT): RpWorld→RpWorld 29, RpWorld→RwCore 15, RwCore→RwCore 8, RpUVAnim/RpHAnim/RtAnim→RtAnim 6, RpWorld→RwPlCore 3, RtBMP→RwPlCore 1. Las raíces de RenderWare llaman al CRT (`memcpy`, `qsort` `0x8247E0`, `free`…): un cuerpo propio las sustituye por el CRT del exe nuevo; **ojo con `qsort`** (sección 4).
- **Riesgos medidos en cuerpo+cierre de las 677:** x87 en 76, SEH (`fs:[0]`) en 7, SSE en 1, con llamadas indirectas sin seguir en 156 (la mayoría vía puntero del motor `[0xC97B24+…]`: `rwmalloc`/`free`/archivos). Callbacks de arranque (heurística): 104.

### 1.2 Cobertura de «quién la llama»

Las 788 raíces tienen al menos un objeto que las escribe a mano y un archivo:línea de código en `llamadoras_fuente` (las 98 filas de ruido que no aparecen en la fuente son valores que el barrido de `.obj` ve y la fuente escribe de otra forma). `llamadoras_en_exe` solo cuenta llamadas directas del EXE; no cuenta punteros de tablas ni llamadas virtuales.

Equivalencia propia sin hook: tras mirar cada comentario de dirección y cada definición, **no hay ninguna raíz con cuerpo C++ propio completo sin hook** salvo (a) las 2 excluidas, (b) los 4 hooks ya instalados pero sin catalogar (+ el hook de interior `0x7487CF` de `WinInput::Initialise`, que IND0A contó como ruido) y (c) 3 thunks CRT de 5 bytes (`0x821195`→`0x82119A`, `0x8213AE`→`0x8214BD`→`0x82413F`). Es decir: **las 788 raíces siguen siendo trabajo real de reconstrucción**, no papeleo.

### 1.3 Verificación contra el EXE y falsos positivos

- **Prueba estructural sobre las 890 filas** (`raices.py`): ¿decodifica?, ¿empieza como función (relleno/`ret` antes y prólogo típico, o inicio de Ghidra)?, ¿alcanza un `ret`?, ¿es thunk? Resultado: 775 de las 788 pasan (seguras 511/517, probables y promovidas 264/271); las 13 marcadas se revisaron a mano: 12 son funciones sin relleno previo o con prólogo atípico y 1 es un falso positivo. De las 102 de ruido, 4 parecen función.
- **Revisión manual de 55 filas** (desensamblado de capstone + línea de fuente; [`verificacion_muestra.csv`](../../reversing/FnPLAN1/datos/verificacion_muestra.csv), reproducible con `muestra.py --fija`): 25 seguras, 18 probables, 12 ruido, con estratos por categoría (juego, RenderWare, QuickTime, bibliotecas, libjpeg…). Además se revisaron una a una las 13 filas marcadas por la prueba estructural y las 13 de ruido con uso en código fuente.

| Clase | Muestra | Reales | Estimación |
|---|---:|---:|---|
| Segura (A) | 25 | 25 | **~0,2 % falsos positivos** (1 confirmado en 517: `0x7F6786`, dirección de retorno interior de una función en el parche `_asm` de `WindowedMode.cpp:443`; otras 5 marcadas por la prueba estructural —`0x735140`, `0x756600`, `0x758910`, `0x761C50`, `0x812930`— son funciones sin relleno previo) |
| Probable (B) | 18 | 18 | **0 falsos positivos hallados** en 262 (los 192 de RenderWare son callbacks de plugin, `qsort` y memoria del motor; los 65 de juego, libjpeg y callbacks de tareas; todos con el uso visible en la fuente). Con 0/18 solo se puede acotar < 18 % (95 %); la prueba estructural y la lectura de los 257 usos lo acotan en torno a < 1 % |
| Ruido (C) | 12 | 2 | **8 % de ruido real** (falso negativo de IND0A): 9 de 111 son funciones usadas como puntero; 98 no aparecen en la fuente (10/10 de la muestra son interior de función) |

Las 9 promovidas: `0x5C2FF0`, `0x5C61E0` (libjpeg), `0x734970` (callback de `RwHelper`), `0x801C30` (`RwFreeListAlloc` por defecto), `0x807C60`, `0x807D80` (callbacks de plugin de motor), `0x810D20` (copia de RpMatFX, 1.043 bytes), `0x8194D0` (stub QTML `mov ax,0F7D1h; ret`), `0x856BC0` (stub de inicializador estático). **No** se promovieron: `0x475441` (`EventTimer`, constante), `0x7FFFFF`, `0x800000` (máscaras) y `0x7487CF` (hook en el interior de `0x748710`).

Muestra de lo verificado: `0x4637C0` es `CMissionCleanup::FindFree` (bucle de 75 entradas de 8 bytes); `0x4CF9B0` es `CQuaternion::Scale`; `0x7F2E10` devuelve `&engine->metrics`; `0x8020B0` es el `rwrealloc` del motor (reenvía a `0x824269`); `0x818970` es un puente QTML de 26 bytes (`mov eax,0x1D002E; jmp [0x8E2A0C]`); `0x5C9340` es un `start_pass` vacío de 1 byte.

---

## 2. Lotes

Reglas: hasta 32 funciones por lote (objetivo ~25), hojas primero, riesgo creciente dentro del grupo, funciones contiguas dentro del módulo. Cada lote sigue el flujo del proyecto: worktree propio, `compile_lock`/`gta_lock`, evidencia en `reversing/CppTwelveLevel6Final/runs` (banco ≥ 16.384 pares, 0 diferencias, prueba negativa de 2 rondas, `register_hook.py` cuando haya hook). La tabla completa (miembros por raíz en `raices.csv`, columna `lote`):

| Lote | Grupo | Fn | Bytes | x87 | SEH | EXE la llama | Arranque | Depende de | Banco estandar |
|---|---|---:|---:|---:|---:|---:|---:|---|---|
| S1A | juego: envolturas y callbacks sencillos (sin x87 ni SEH) | 13 | 1038 | 0 | 0 | 0 | 0 | - | si (metodo estandar) |
| S2A | juego: x87, SEH, COM de DirectInput, excluidas | 25 | 7724 | 14 | 3 | 5 | 1 | - | si (metodo estandar) |
| S3A | juego: libjpeg, compresion (0x5C2FF0-0x5C8xxx) | 29 | 7651 | 2 | 0 | 0 | 0 | - | si, con estructuras jpeg falsas |
| S4A | juego: libjpeg, descompresion, memoria y errores | 29 | 6062 | 2 | 1 | 0 | 0 | - | si, con estructuras jpeg falsas |
| S5A | RenderWare RtQuat+RtBMP+RtDict+RtAnim | 15 | 1925 | 2 | 0 | 0 | 0 | - (y posterior: S20A) | si, con memoria falsa |
| S6A | RenderWare RtAnim | 14 | 2442 | 3 | 0 | 0 | 0 | - | si, con memoria falsa |
| S7A | RenderWare RpAnisot | 8 | 233 | 0 | 0 | 0 | 8 | - | si, con memoria falsa |
| S8A | RenderWare RpUVAnim | 13 | 717 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S9A | RenderWare RpUVAnim | 13 | 1769 | 9 | 0 | 0 | 0 | S6A | si, con memoria falsa |
| S10A | RenderWare RpHAnim | 13 | 1173 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S11A | RenderWare RpHAnim | 13 | 4103 | 5 | 0 | 0 | 0 | S6A | si, con memoria falsa |
| S12A | RenderWare RpSkin | 25 | 2666 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S13A | RenderWare RpMatFX | 23 | 1510 | 0 | 0 | 0 | 9 | - | si, con memoria falsa |
| S14A | RenderWare RpMatFX | 22 | 5114 | 7 | 0 | 0 | 3 | - | si, con memoria falsa |
| S15A | RenderWare RwCore | 23 | 1379 | 0 | 0 | 0 | 1 | - | si, con memoria falsa |
| S16A | RenderWare RwCore | 23 | 1159 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S17A | RenderWare RwCore | 23 | 1302 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S18A | RenderWare RwCore | 23 | 2125 | 1 | 0 | 0 | 1 | - | si, con memoria falsa |
| S19A | RenderWare RwCore | 20 | 10177 | 3 | 0 | 0 | 0 | S15A | si, con memoria falsa |
| S20A | RenderWare RwPlCore | 24 | 2144 | 0 | 0 | 0 | 13 | - | si, con memoria y flujos falsos |
| S21A | RenderWare RwPlCore | 24 | 797 | 1 | 0 | 0 | 11 | - | si, con memoria y flujos falsos |
| S22A | RenderWare RwPlCore+RwTexDict | 24 | 4988 | 7 | 2 | 0 | 14 | - | si, con memoria y flujos falsos |
| S23A | RenderWare RpWorld | 24 | 992 | 0 | 0 | 0 | 7 | - | si, con memoria y flujos falsos |
| S24A | RenderWare RpWorld | 24 | 1139 | 0 | 0 | 0 | 5 | - | si, con memoria y flujos falsos |
| S25A | RenderWare RpWorld | 24 | 1128 | 0 | 0 | 0 | 17 | - | si, con memoria y flujos falsos |
| S26A | RenderWare RpWorld | 24 | 1242 | 0 | 0 | 0 | 6 | - | si, con memoria y flujos falsos |
| S27A | RenderWare RpWorld | 24 | 2275 | 0 | 0 | 0 | 0 | - | si, con memoria y flujos falsos |
| S28A | RenderWare RpWorld | 24 | 3325 | 0 | 0 | 0 | 6 | - | si, con memoria y flujos falsos |
| S29A | RenderWare RpWorld | 24 | 3170 | 1 | 0 | 0 | 2 | - | si, con memoria y flujos falsos |
| S30A | RenderWare RpWorld | 24 | 7638 | 10 | 0 | 0 | 0 | S24A,S29A | si, con memoria y flujos falsos |
| S31A | RenderWare RpWorld | 18 | 11211 | 9 | 1 | 0 | 0 | S15A,S17A,S18A,S19A,S20A,S22A,S24A,S26A,S28A,S29A,S30A | si, con memoria y flujos falsos |
| S32A | RenderWare RwCore | 14 | 441 | 0 | 0 | 0 | 0 | - | no hasta tener el dispositivo D3D falso |
| S33A | RenderWare RwCore+Controlador D3D9 (RwD3D9) | 14 | 871 | 0 | 0 | 0 | 0 | - | no hasta tener el dispositivo D3D falso |
| SQA | puentes QTML (no reconstruibles) | 31 | 806 |  |  |  |  | - | no: validacion por inspeccion |
| LIB | biblioteca (lote aparte): biblioteca: CRT 57; biblioteca: D3DX9 19; bi | 80 | 14418 |  |  |  |  | - | depende |


Columnas: «EXE la llama» = raíces con alguna llamadora directa en el EXE (las únicas que necesitarían hook); «Arranque» = registradas al arrancar el motor (heurística); «Depende de» = lotes que contienen raíces que estas llaman (si aparece «y posterior», la dependencia está en un lote posterior: el cuerpo propio llama al original por dirección hasta que el posterior cierre; no impide validar).

### 2.1 Juego (S1A–S4A, 96 raíces)

| Lote | Qué trabajo es |
|---|---|
| **S1A** (13) | Envolturas «llamada con cast» y callbacks de animación sin x87 ni SEH: `CMissionCleanup::FindFree`, `CQuaternion::Copy`, 6 callbacks de fin de animación de tareas (`TaskSimpleFight`, `BeKickedOnGround`, `BeHitWhileMoving`, `GangDriveBy`, `SitInChair`), `Vehicle` `0x6D2950`, 3 de `RwHelper`, el buffer de relleno de `CAEMFDecoder`. **Convertir en cuerpo propio** y apuntar el puntero propio; método estándar, sin infraestructura. |
| **S2A** (25) | Lo anterior con x87 (14: `Physical`×3, `Vehicle::DoSunGlare` 2.158 B, `Lines` 669 B, `Camera`, `Garages`, `Quaternion::Scale`, `Fx*`, `Sprite2d`, `Ped::SortPeds`…), SEH (`0x41B1D0` lectura de chunk de colisión), COM de DirectInput (`0x746D80`, `0x747020`), **4 hooks ya instalados sin catalogar** (`0x541DD0` `CPad::UpdatePads`, `0x7469A0` `diMouseInit`, `0x746D80`, `0x747020`: catalogar y validar) y las **2 excluidas** (`0x4CCE60` usercall de RenderWare, que ahora solo llaman 2 sitios propios: basta un C++ con ABI libre; `0x4A2BB0`, un `jmp` a `FxPrim_c::Destructor` `0x4A9F40`, que **ya tiene hook validado**: se cierra llamando a ese). x87 a 24 bits y plantilla del banco. |
| **S3A** (29) | libjpeg, compresión (`0x5C2FF0`–`0x5C8FB0`): callbacks `start_pass`, DCT hacia delante, Huffman, marcadores, reducción de muestreo. **Cuerpo propio** y escribir el puntero propio en el lugar de `At<uint32>(…)=0x5C…`; banco con estructuras `jpeg_*` falsas (los `jinit` validados ya conocen los desplazamientos). Alternativa en la sección 4 (usar las fuentes IJG 6b). |
| **S4A** (29) | libjpeg, descompresión, memoria y errores (`0x5C9150`–`0x5CF690`): Huffman, controlador de coeficientes, conversión de color, upsample (`0x5CCF60`–`0x5CD220`), gestor de memoria y `jpeg_std_error`. Incluye 1 con SEH en el cierre. |

### 2.2 RenderWare interno (S5A–S33A, 581 raíces) y solapamiento con la validación

Importante: **las raíces no están en las 724 entradas reconstruidas**. Son funciones internas que las envolturas y los hooks ya validados llaman en el EXE; hoy el mapa de RenderWare las da por «hechas» porque Ghidra no las tomó como función (`plan-lotes.md` quedó vacío: 0 funciones por reconstruir). Hay que **reconstruirlas y validarlas** como trabajo nuevo, **además de** las 663 entradas pendientes de validación de `plan-validacion.md`.

| Oleada (plan-validacion) | Módulos y raíces | Lotes | Pendientes de validación del módulo (plan) | ¿Mismo lote que la validación? |
|---|---|---|---|---|
| 1 · Puras (+ memoria falsa) | RtQuat 2, RtBMP 1, RtDict 6, RtAnim 20, RpAnisot 8, RpUVAnim 26, RpHAnim 26, RpSkin 25, RpMatFX 45, RwCore puro 112 = 271 | S5A–S19A (15) | RtDict 10, RtAnim 7, RpUVAnim 14, RpHAnim+RtQuat 18, RpSkin 33, RpMatFX 28, RtBMP+RtSlerp 3, RwCore (60 puras de 175) | **Sí, por módulo**: son hojas de los mismos archivos `.cpp`; el banco de cada entrada ya llama al resto con enganche. Una sola lectura del módulo y una sola infraestructura. |
| 2 · Flujos y memoria | RwPlCore 71, RwTexDict 1, RpWorld 210 = 282 | S20A–S31A (12; S22A mixto) | RwPlCore 118, RpWorld 94 | **Sí**, usa los falsos de 227A–229A. S20A–S22A (memoria, archivos y plugins del motor) son la parte de arranque más caliente (38 de 71). |
| 3 · Dispositivo D3D | RwCore cerca del controlador D3D9 27, RwD3D9 1 = 28 | S32A–S33A (2) | RwD3D9 115, RwCore D3D (~115 de 175) | **No** hasta tener el dispositivo falso; después, mismo lote que la entrada D3D que las llama. |
| 4 · Especiales | 31 puentes QTML | SQA | QTMLRuntime 6, ImportThunks 17, DelayLoadThunks 3, ZlibInflate 14 | Inspección, mismo formato de registro que los thunks de la oleada 4. |

Sobre el orden dentro de la oleada y si «puede hacerse en el mismo lote»: una raíz-hoja puede entrar en el lote de validación de la entrada que la llama cuando **todas** sus llamadoras están en ese lote o en lotes cerrados; es la regla que se aplicó a la columna «Depende de» (solo S5A tiene una dependencia hacia un lote posterior —`RtBMP` llama a una raíz de RwPlCore, S20A— y aun así el banco funciona porque el cuerpo propio llama al original por dirección).

### 2.3 Las 31 «no reconstruibles» (SQA) y las bibliotecas

**Qué las hace no reconstruibles.** No son 3DNow!: son **puentes de QuickTime/QTML de 26 bytes**: `mov eax,[0x8E2A08]; or eax,eax; je …; call [0x8E2A08]; mov eax,<ID>; jmp [0x8E2A0C]`. Reciben los argumentos en la pila, el **identificador de función va en EAX** y saltan al despachador de QuickTime sin tocar la pila: ni el identificador en EAX ni el `jmp` son expresables en C++ puro (sin `__asm`/`naked`, regla del proyecto). Los usa solo `CAEMFDecoder` (pistas de usuario, `AEMFDecoder.cpp:44…`) y `QTMLRuntime`.

**Tratamiento propuesto (SQA, 1 lote de inspección):** (1) mientras QuickTime exista, se **cierran por inspección**: se comprueba en el EXE que cada puente salta a `[0x8E2A0C]` con el identificador que dice la fuente y se archiva esa evidencia (mismo formato que `ImportThunks`); (2) si se decide quitar QuickTime (sección 4), **se eliminan los 29 puentes usados y `CAEMFDecoder`**: el gestor de pistas de usuario ya cae al decodificador WMA cuando `InitLibrary` devuelve `false`. Los 2 ganchos QTML (`0x819700` y `0x8194D0`) entran en LIB con QuickTime.

**LIB (80 raíces, 14.418 bytes de las propias raíces; arrastran 825 funciones y 457 KB), lote aparte:** CRT 57 (`operator new`, `_free`, `_rand`, `sscanf`, `qsort`, funciones de archivo con SEH, thunks…), D3DX9 19 (funciones matemáticas `0x76626B`…`0x772BB1` y `0x77EE77`), libpng+zlib 2 (`0x7CF600`, `0x7CF9B0`: de LIB1A) y QuickTime 2. Aquí solo se **enumeran**; las marca el CSV como «biblioteca: lote aparte».

---

## 3. Orden y métrica

### 3.1 La métrica N

Estado del cargador: al cierre de PLAN1 `docs/ExeIndependiente/README.md` aún no tiene la sección «Estado del cargador» (IND1A en curso), así que se define aquí:

> **N = número de funciones distintas del `.text` original (0x401000–0x857000) que se ejecutan desde que arranca el proceso hasta el primer fotograma del menú principal aceptando entrada.** Medida de IND1A (VEH con `.text` original sin permiso de ejecución, anotando cada entrada distinta). Se reportan tres cifras: **N** (funciones, agrupando por `binary_function_ranges.csv`), **N_bytes** y **N_raíces** (cuántas de las 788 aparecen en N).

Sin tener aún el fichero de IND1A, `lotes.py --ejecutadas <csv>` acepta cualquier lista de direcciones hexadecimales (una por línea o en cualquier columna) y rellena `ejecutadas_hasta_menu` por lote. **Cuando exista, hay que reordenar los lotes por esa columna** (el orden de más abajo usa la heurística `arranque`).

N es la meta final; el conteo de raíces es la medida de avance **necesaria pero no suficiente**: N también depende de funciones que ninguna raíz llama (inicializadores `_initterm`, ranuras de tablas sin hook, llamadas virtuales): ver sección 4.

### 3.2 Criterio de «raíz cerrada» y cómo se cuenta

Una raíz está **cerrada** cuando se cumple una de:

1. **Hook propio validado**: la dirección tiene entrada en `hooks.csv` con `validated=1` (banco par a par ≥ 16.384, 0 diferencias, prueba negativa de 2 rondas) y el código propio ya no la escribe a mano. Solo hace falta para las raíces con llamadora directa en el EXE (5 de las 677).
2. **Cuerpo propio validado sin hook**: la fuente tiene la función, el banco la compara contra el original **mientras este sigue cargado** (llamada por dirección), con la misma exigencia, y **la dirección deja de aparecer en `refs_codigo.csv`** (el código propio llama a la función propia o registra su puntero).
3. **Biblioteca sustituida**: la dirección y su cierre dejan de aparecer (se enlaza la biblioteca equivalente).
4. **Cerrada por inspección documentada** (excluidas, thunks, puentes QTML): evidencia archivada y la llamada propia retirada o redirigida.

Cómo se cuenta: [`reversing/FnPLAN1/avance.py`](../../reversing/FnPLAN1/avance.py) compara `raices.csv` con una medición nueva de IND0A (`scan_obj.py` + `analizar.py` sobre el ASI actual) y escribe las raíces **abiertas** por categoría y por lote (hoy 788/788). Una raíz está abierta si su dirección sigue en `dependencias_codigo.csv` y no tiene hook en `hooks.csv`. Avance = **788 → 0**; objetivo intermedio: **0 raíces abiertas en los lotes S** (677 → 0) y después las 80 de LIB y las 31 de SQA.

### 3.3 Orden recomendado para bajar N

Los números S1A… siguen riesgo y dependencia. Para **bajar N primero**, el orden de ejecución propuesto (se afina con la lista real de IND1A):

| Paso | Lotes | Por qué |
|---|---|---|
| 0 | Medida N0 (IND1A) y cabecera común de falsos de memoria/listas libres (la misma que R1A de la validación) | Línea base y base de los bancos de RenderWare |
| 1 (piloto) | **S7A** y, en paralelo, **S1A** | S7A: 8 callbacks de arranque, 233 bytes, sin x87, sin dependencias, prueba la categoría nueva «callback por dirección». S1A: método estándar sin infraestructura |
| 2 | S20A, S21A, S22A (RwPlCore: memoria, archivos y plugins del motor; 38 de arranque) | Las raíces que más se ejecutan al arrancar |
| 3 | S25A, S23A, S24A, S26A, S28A, S29A (RpWorld de arranque) y S13A, S14A (RpMatFX) | 55 raíces de arranque más; tras S20A–S22A porque comparten los falsos de flujos |
| 4 | S15A–S19A (RwCore puro), S5A, S6A, S8A–S12A | El resto de la oleada 1 |
| 5 | S27A, S30A, S31A (RpWorld con dependencias hacia RwCore/RwPlCore) | S31A depende de 10 lotes anteriores |
| 6 | S2A | Hooks sin catalogar, x87, SEH, COM y las 2 excluidas |
| 7 | S3A, S4A (libjpeg) | Baja contribución a N (solo captura de pantalla/guardado) |
| 8 | Infraestructura D3D y S32A, S33A | Necesitan el dispositivo falso |
| 9 | SQA tras la decisión de QuickTime; LIB en lotes aparte | |

Curva estática del cierre (`datos/curva_cierre.csv`; es una **cota**: solo llamadas directas):

| Tras | Raíces abiertas | Funciones del EXE aún necesarias | Bytes |
|---|---:|---:|---:|
| (hoy) | 788 | 1.586 | 578.904 |
| S1A–S4A (juego) | 692 | 1.462 | 550.219 |
| S5A–S19A (RW oleada 1) | 421 | 1.199 | 519.061 |
| S20A–S31A (RW oleada 2) | 139 | 883 | 458.628 |
| S32A–S33A (RW oleada 3) | 111 | 856 | 457.551 |
| SQA | 80 | 825 | 456.745 |

### 3.4 Esfuerzo relativo (en lotes, sin fechas)

Unidad: un lote estándar de ~25 funciones (1,0). Los pesos son estimaciones de PLAN1, no mediciones.

| Concepto | Lotes | Peso | Equivalente |
|---|---:|---:|---:|
| S1A (13 fn, sin infra) | 1 | 0,6 | 0,6 |
| S2A–S4A (x87/SEH/COM; libjpeg con estructuras falsas) | 3 | 1,2 | 3,6 |
| S5A–S19A (oleada 1; falsos de memoria) | 15 | 1,0 | 15 |
| S20A–S31A (oleada 2; flujos y memoria; 3 con x87/SEH) | 12 | 1,2 | 14,4 |
| S32A–S33A (oleada 3) | 2 | 1,8 | 3,6 |
| SQA (inspección) | 1 | 0,5 | 0,5 |
| **Sustitución de raíces** | **34** | | **≈ 38** |
| Infraestructura: cabecera común de falsos (1), flujos generalizados (1), dispositivo D3D falso (2) | 4 | | ≈ 4,5 |
| Validación pendiente de RenderWare (`plan-validacion.md`: 663 entradas) | ≈ 27 | | ≈ 27 |
| **Total si se hacen separados** | | | **≈ 70** |
| **Total si se combinan por módulo** (ahorra la infraestructura repetida y una lectura por módulo) | | | **≈ 52** (−25 %, estimación) |

Aparte (otros lotes, orden de magnitud): LIB1A libpng+zlib (en curso), D3DX9 1–2, CRT propio 2–4, 1.673 inicializadores 2–3 (casi todos son una constante: generables), 3.933 ranuras de tablas 2–3, instantánea de datos y prueba sin original 1–2.

---

## 4. Riesgos y decisiones pendientes

Decisiones que necesito del usuario:

1. **QuickTime y las pistas de usuario (radio).** Los 31 puentes QTML solo sirven para decodificar pistas de usuario MP3/AAC con QuickTime (`CAEMFDecoder`), que además **necesita QuickTime instalado** y ya tiene alternativa (decodificador WMA). Opciones: (a) quitar `CAEMFDecoder` y los 29 puentes en el exe independiente (recomendado: el número de raíces baja en 31 y se elimina una dependencia externa); (b) mantenerlos como están y cerrar por inspección (el exe necesita QuickTime); (c) sustituirlos por Media Foundation (lote nuevo).
2. **D3DX9.** Las 19 raíces son D3DX9 matemático (`D3DXFloat16To32Array`, etc.) y el cierre completo son 321 funciones/132 KB del compilador de shaders. ¿Se acepta importar `d3dx9_43.dll` del sistema (el EXE original ya usa d3d9 diferido) o hay que reimplementarlo? Cambia LIB de «1–2 lotes» a decenas.
3. **libjpeg.** Los 58 callbacks de S3A–S4A son IJG libjpeg 6b: ¿se reconstruyen a mano como el resto (2 lotes con estructuras falsas) o se **incorpora el código fuente IJG** (licencia permisiva) y se valida contra el EXE? Lo segundo suele ser más barato y exacto.
4. **Qué es «independiente» respecto a los datos.** Este plan solo cierra **código**. Los 4.949 datos por dirección fija, las 3.933 ranuras de tablas y los 1.673 inicializadores siguen en el EXE: no hay independencia sin esa parte (estrategia (c) de IND0A). ¿Se planifica ya el lote de instantánea de datos y los generadores?
5. **Cuándo cambiar de método de validación sin EXE.** Todo este plan **se valida con el original cargado** (ASI sobre el EXE o el exe-cáscara de IND0A). Cuando N llegue a 0 ya no habrá oráculo: propongo congelar la evidencia y seguir con (i) regresión por hashes de salida y (ii) la partida de prueba sin original. ¿Aceptas ese corte, o se mantiene un proceso original como oráculo?
6. **`qsort`.** Las raíces de RpWorld ordenan con el `qsort` del EXE (`0x8247E0`); un `std::sort` o un `qsort` distinto **cambia el orden de empates**. Propuesta: reproducir el algoritmo exacto del CRT en un helper propio. Confirmar.
7. **Confirmar la base de 788** (los 9 promovidos) y las 5 «promovibles» que se descartaron.
8. **Combinar la validación de RenderWare con la sustitución** (≈52 lotes) o separarlas (≈70). Con el límite de 2 validaciones a la vez y una sola sesión de juego, lo combinado es más rápido pero cada lote es más largo.

Riesgos técnicos que no se pueden cerrar con análisis estático:

- **1.003 llamadas indirectas sin seguir** (156 raíces tienen alguna): un cierre «a cero» por llamadas directas no prueba que no se ejecute código original; la comprobación final es **N = 0**, no el conteo de raíces. Los lotes deben **medir N antes y después** de cada tanda.
- **Ranuras de tablas sin hook (3.881 + 52 de vtables) y llamadas virtuales entre objetos creados por código original**: invisibles para este plan; pueden mantener funciones originales vivas aunque todas las raíces estén cerradas.
- **1.673 inicializadores estáticos**: no son raíces; hacen falta lotes de generación o el CRT propio ejecutando sus tablas.
- **Calling conventions**: `usercall` (las 2 excluidas y posiblemente alguna de las 581 que IND0A no marcó; el barrido solo ve el uso, no el ABI): el desensamblado de la muestra no encontró ninguna más, pero un lote debe revisarlo antes de escribir el cuerpo. Los puentes QTML (EAX) son inexpresables en C++.
- **x87 y SEH**: x87 en 76 raíces (bancos a 24 bits como en el resto); SEH en 7 (manejadores en el código original; `/SAFESEH:NO`) y un `_asm` real en `WindowedMode.cpp:443` (viola la política «todo en C++»; no es raíz, pero conviene un lote de saneamiento).
- **Heurística de arranque** (columna `arranque`) y **«quién la llama»** salen de la fuente: pueden fallar si la dirección se escribe con una constante intermedia.
- **Líneas de fuente** respecto del Debug medido: pueden variar unas líneas.

---

## 5. Cómo se reproduce

```
python -I reversing/FnPLAN1/raices.py --exe "<gta_sa.exe>" --ind0a <datos de IND0A> --repo <raiz>   # 890 filas; ~30 s
python -I reversing/FnPLAN1/lotes.py  --repo <raiz> [--ejecutadas <lista de IND1A>]                 # lotes, curva y columnas del CSV
python -I reversing/FnPLAN1/verificacion_muestra.py --repo <raiz>                                    # tabla de la muestra de 55
python -I reversing/FnPLAN1/muestra.py --exe "<gta_sa.exe>" --repo <raiz> --fija                     # desensamblado de esa muestra
python -I reversing/FnPLAN1/avance.py --repo <raiz> --datos <IND0A nueva>                            # raíces abiertas
```

`refs_codigo.csv` (7,5 MB) no se versiona: se regenera con `scan_obj.py` (IND0A). Todo el análisis es de solo lectura.
