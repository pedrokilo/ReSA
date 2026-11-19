<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ruta hacia un EXE independiente: medición real (lote IND0A, 18/11/2026)

Primera medición de **cuánto del `gta_sa.exe` original sigue haciendo falta** para correr el juego con el código propio. Todo se midió sobre los `.obj` ya compilados del ASI Debug (HEAD `39276bea85`) y sobre el EXE original (SHA256 `72ae59e4…ecc9b`, solo lectura). **No se ejecutó el juego.**

Datos y herramientas: [`reversing/FnIND0A/`](../../reversing/FnIND0A/README.md) (CSV en `datos/`). Contexto previo: [RutaExeIndependiente.md](../RutaExeIndependiente.md), [JuegoExe](../JuegoExe/README.md), [RenderWareExe](../RenderWareExe/README.md).

## Resumen en diez líneas

1. **Con 12.4 mil funciones reconstruidas, el código propio sigue necesitando 890 direcciones de código del EXE sin cuerpo propio** (779 seguras, 111 probable ruido). Cerradas por llamadas, equivalen a **≥ 1.572 funciones y ≥ 577 KB de código original**, más 1.003 llamadas indirectas que no se pudieron seguir. Es una **cota inferior**.
2. **El «100 %» de los mapas no es independencia.** Hay **93 direcciones de código del juego** (28 seguras, 65 probables, muchas son callbacks de libjpeg) y **630 de RenderWare** (578 internas sin hook —485 en 0x750000+ y 93 por debajo de esa dirección, llamadas desde `rw*/rp*/rt*`—, 31 no reconstruibles, 21 de bibliotecas) que el código propio todavía llama en el EXE; casi ninguna figura en los mapas porque Ghidra no las tomó como función (`JuegoExe/funciones.csv` solo tiene entradas de Ghidra).
3. **Los datos son lo grande: 100 % siguen en el EXE.** El código propio toca **4.949 direcciones de datos distintas (54.225 referencias)**, todas por dirección fija (`StaticRef`, casts, inmediatos). Ninguna vive en el binario propio.
4. **Las bibliotecas** que hay que sustituir o enlazar: D3DX9 (≥ 321 funciones alcanzadas, 132 KB), libpng+zlib (176, 81 KB), CRT (314, 230 KB), QuickTime (32), más **1.673 inicializadores estáticos** del CRT (`_initterm`, 34,7 KB) que hoy ejecuta el original.
5. **El PE original es amable:** base 0x400000 fija, **sin relocaciones, sin TLS, sin ASLR, sin DEP**, 7 secciones, 214 imports (la mayoría del sistema).
6. **El enlace no es el obstáculo:** el código propio enlaza como EXE de base fija **con 0 símbolos sin resolver**, porque todo lo del EXE se alcanza por dirección fija. El obstáculo es el **mapa de memoria**: el exe tiene que reservar `0x401000–0xCB1000` y rellenarlo.
7. **Prototipo (estrategia a, «caparazón») construido y comprobado estáticamente:** `gta_reversed_exe.exe` (base 0x400000, `/FIXED`, reserva de 0x8B0000 bytes en 0x401000, código propio desde 0xCB1000, reserva a cero en el archivo, **no importa `gta_sa.exe`**). Lleva un cargador de la imagen original **escrito pero NO ejecutado**.
8. **Meta «EXE independiente ya»: no es alcanzable todavía.** Hoy el exe todavía necesita el EXE del usuario como **imagen de datos y código**. Sí es alcanzable un hito intermedio: «cáscara» que lo carga como archivo de datos, y después medir **cuántas funciones originales se ejecutan hasta el menú** (objetivo 0).
9. **Riesgo grande:** los **~20.265 datos** que usa todo el código original (4.917 en común con el propio) y las **3.784 entradas de tablas de punteros a código sin hook** en `.rdata`: cualquier camino no cubierto salta a código original.
10. **Recomendación:** (1) que el usuario pruebe el cargador en modo solo-cargar y luego `--ejecutar`; (2) ola de sustitución de las ~780 raíces (93 de juego, 578 de RenderWare interno, 56 de CRT/bibliotecas); (3) contador de funciones originales ejecutadas (VEH con `.text` original no ejecutable). Detalle en la sección 6.

---

## 0. Cómo se midió (y límites)

| Paso | Método |
|---|---|
| Referencias a direcciones del EXE | Barrido con capstone de **todas las secciones de código de los 1.101 `.obj`** de `build/source/gta_reversed.dir/Debug`: toda constante en 0x400000–0xCFFFFF **no cubierta por una relocalización** (dirección absoluta escrita a mano: `plugin::Call*`, `StaticRef`, `reinterpret_cast<…>(0x…)`, `RH_Scoped*Install`). 70.825 referencias, 18.285 destinos distintos. |
| «Tiene cuerpo propio» | La dirección está en `docs/direcciones.csv` ∪ `hooks.csv` ∪ `hooks_notsa.csv` (12.445 direcciones con hook). |
| Confianza de un destino de código sin hook | **A** (517): es entrada de Ghidra / del mapa RW / destino de una llamada del EXE / se usa como `mov reg,imm; call reg`. **B** (262): se guarda como puntero (`mov [m],imm` / `push`) y empieza como una función (bytes anteriores `ret`/relleno y primer opcode típico). **C** (111): resto, **ruido probable** (constantes enteras, p. ej. `imul … ,0x696d76`). |
| Cierre de código | Recorrido de flujo del EXE desde las raíces A+B (seguir `call` directos, saltos, tablas `jmp [r*4+tabla]`), parando en funciones con hook. Sin seguir llamadas indirectas. |
| Datos | Mismas referencias pero a `.rdata/.data/.bss/_TEXT_HA`; tipos y tamaños por el código fuente (`StaticRef<T>`, `RH_ScopedVirtualClass`, casts) y por los bytes del EXE (cadenas, vtables). |

Límites: (1) solo se leen instrucciones de los `.obj`; direcciones guardadas en tablas de datos propias (`static const uintptr_t[]`) no se ven. (2) El análisis es del Debug; los números de destinos son estables, no así el de llamadas. (3) 715 de las 4.949 direcciones de datos solo aparecen en binario y no en el código fuente: algunas son ruido (p. ej. `0x989680` = 10 000 000). (4) El cierre es cota inferior: 1.003 llamadas indirectas y las llamadas virtuales entre objetos creados por código original no se siguen. (5) La clasificación de módulo de RenderWare usa `RenderWareExe/funciones.csv`, que no contiene las 515 funciones internas de RW alcanzadas.

## 1. Mapa de dependencias de código

Fuente: [`datos/dependencias_codigo.csv`](../../reversing/FnIND0A/datos/dependencias_codigo.csv) (una fila por destino sin hook, con quién lo llama), [`cierre_codigo.csv`](../../reversing/FnIND0A/datos/cierre_codigo.csv).

| | Destinos distintos |
|---|---:|
| Direcciones de código (0x401000–0x857FFF) que usa el código propio | 13.334 |
| … con cuerpo propio (hook) | 12.444 (93,3 %) |
| … **sin cuerpo propio** | **890** |
| de ellas, A (seguras) / B (probables) / C (ruido) | 517 / 262 / 111 |
| A+B dentro de 0x401000–0x81FFFF / fuera (CRT, 0x820000+) | 723 / 56 |

### Sin hook, por categoría (A+B)

| Categoría | Destinos | Qué son |
|---|---:|---|
| Juego (llamado desde código del juego) sin hook | 93 (28 A + 65 B) | Funciones que **siguen llamándose en el EXE** y no están en ningún mapa: envolturas reales (`Garages.cpp:685 plugin::Call<0x448890>`, `MissionCleanup.cpp:398`, `Physical`, `Vehicle`, `Ped`, `Camera`…) y callbacks de libjpeg (`0x5CD220`…). |
| RenderWare interno sin hook | 485 + 93 (llamadas desde `rw*/rp*/rt*` a direcciones < 0x750000, p. ej. `0x74AA10`) | `rwcore`, `rpworld`, `rwplcore`, `rpmatfx`, `rphanim`, `rpuvanim`, `rpskin`, `rtanim`… Los 724 «reconstruidos» son hooks; las envolturas siguen llamando a su interior (p. ej. `rpworld.cpp:2322 → 0x756260`). |
| RenderWare no reconstruible | 31 | D3DX9 (3DNow!, …) |
| RenderWare enlazada (biblioteca) | 21 | libpng/zlib/D3DX9/QuickTime que se enlazan |
| CRT y bibliotecas (0x820000+) | 55 (+1) | `operator new` (0x82119A, 0x821195), `_free`, `_rand`, …; el puente QuickTime 0x818000–0x81B000 |

### Cierre transitivo (lo que habría que sustituir o enlazar)

| Región / módulo | Funciones | Bytes |
|---|---:|---:|
| RenderWare interno (sin mapa) | 515 | 95.746 |
| D3DX9 (compilador de shaders) | 321 | 131.792 |
| libpng + zlib | 176 | 81.146 |
| QuickTime/QTML (puentes) | 32 | 832 |
| CRT y bibliotecas estáticas (0x820000+) | 314 | 230.089 |
| Juego y RenderWare bajo 0x750000 sin sustituir | 213 | 37.152 |
| **Total (cota inferior)** | **1.572** | **576.757** |

Además: 58 funciones de la IAT (API del sistema) usadas, 1.003 llamadas indirectas sin resolver y ranuras de vtable declaradas sin hook (3 según `binary_slots.csv`, 52 según el barrido de `.rdata`; ver sección 2).

### Las 118 excluidas y las ~800 de BinaryCoverageAudit

- **Excluidas** ([`excluidas_estado.csv`](../../reversing/FnIND0A/datos/excluidas_estado.csv)): 0 enganchadas (por definición); **2 las referencia el código propio hoy** (`0x4CCE60` usercall de RenderWare desde `D3D9RasterFormat`, `0x4A2BB0` salto puro desde `FxEmitter`); 83 tienen alguna llamadora directa en el EXE. Las 18 de la protección no se necesitan en un exe propio; las 51 `usercall-cubierta` dependen de que sus llamadoras validadas sigan siendo propias.
- **BinaryCoverageAudit (819 destinos sin nombre, 10/10):** hoy **796 están enganchadas** y **23 no**: 6 del CRT (`_rand`, `_free`×2, `operator new`×2, `0x821B11`), 15 del puente QuickTime (0x818xxx–0x81Axxx) y 2 de RenderWare (`0x7CF600`, `0x7CF9B0`) ([`audit819_estado.csv`](../../reversing/FnIND0A/datos/audit819_estado.csv)). Las 819 siguen referenciadas por el código propio (por el hook o por la llamada).

## 2. Mapa de dependencias de datos

Fuente: [`datos/dependencias_datos.csv`](../../reversing/FnIND0A/datos/dependencias_datos.csv) (tipo, tamaño conocido, formas, quién), [`tablas_de_punteros.csv`](../../reversing/FnIND0A/datos/tablas_de_punteros.csv).

| Sección | Tipo | Direcciones distintas | Referencias |
|---|---|---:|---:|
| `.data` (bss, por encima de 0x8E4000) | variable sin inicial | 2.773 | 44.001 |
| `.data` inicializada | variable con valor inicial | 1.169 | 7.156 |
| `.rdata` | vtable (443 declaradas con `RH_ScopedVirtualClass`) | 475 | 1.189 |
| `.rdata` | constante / tabla | 458 | 1.753 |
| `.rdata` | cadena | 36 | 49 |
| `_TEXT_HA` | variable | 38 | 77 |
| **Total** | | **4.949** | **54.225** |

- **Nombradas en C++** (`static inline auto& x = StaticRef<T>(0x…)`): 2.756; **con tipo en el código fuente**: 4.234 (86 %); el resto (715) solo por inmediatos, p. ej. base de array + índice. **Tamaño conocido** (solo tipos escalares y arrays declarados): 2.892 direcciones, 39.915 bytes: infravalora.
- **Declaradas como variable propia con almacenamiento propio: ninguna medible.** Todo acceso a un global del EXE es una referencia por dirección fija. El código propio sí tiene sus propias variables (miles), pero ninguna sustituye a un global del EXE.
- **Lo que usa todo el código original** (barrido de todo `.text`): **20.265 direcciones de datos distintas**; **4.917** también las usa el código propio; **32** solo el propio. Un exe totalmente propio necesitaría reproducir los datos de las 4.917 (los que toca) y, mientras exista código original, los 20.265.
- **Imagen de datos a reservar:** `.rdata` 311.296 B, `.data` 4.169.728 B (262.144 con valor inicial, 3,9 MB bss), `_TEXT_HA` 69.632 B, `_rwdseg` 4.096 B, `.rsrc` 4.096 B (~4,56 MB).
- **Constantes que ya no se leen del EXE:** muchas (p. ej. `0x859878` = 57,29578f) están literales en el código propio, no son dependencia.
- **Tablas de punteros a código dentro de `.rdata/.data` del EXE:** vtables: 5.434 ranuras enganchadas, **52 sin hook**; otras tablas: 724 enganchadas, **3.881 sin hook** (D3DX9, CRT, tablas de callbacks de RenderWare, la tabla de comandos del script `0x8A6168` está entre las enganchadas). Son las que hay que reubicar si el código original se mueve o desaparece.
- **Inicializadores estáticos (`_initterm`, `_cinit` en 0x823B76):** 2 tablas, **1.673 entradas** (C: 0x8A5A14–0x8A5A30, 6; C++: 0x8A4000–0x8A5A10, 1.667), todas en 0x821CBE–0x855FD0, **0 enganchadas**. 1.291 miden ≤ 20 B, 371 ≤ 40 B, 11 ≤ 87 B; 153 llaman a un constructor. En total **34.699 B**. Las ejecuta el CRT original hoy ([`initterm.csv`](../../reversing/FnIND0A/datos/initterm.csv)); casi todas calculan una constante en un global (`fld/fmul/fdiv/fstp [0x…]`).

## 3. PE, imports, recursos

Fuente: `datos/pe_*.csv` y `pe_resumen.json`.

| Dato | Valor |
|---|---|
| Tamaño archivo / imagen | 5.189.632 B / 9.113.600 B (cabeceras 1.024 B) |
| Base / entrada | 0x00400000 / **0x00824570** (`start` del CRT de MSVC 7.1) |
| Subsistema | Windows GUI (2) |
| Flags | 0x10F: **relocaciones quitadas**, ejecutable, 32 bits; DllCharacteristics **0** (sin ASLR, sin DEP) |
| Reubicaciones / TLS / Load Config / SafeSEH / overlay | 0 / ninguna / ninguna / ninguna (SEH por cadena `fs:[0]`) / 0 |
| Pila | reserva 2 MB, confirmada 4 KB; montón 2 MB |

| Sección | VA | Virtual | Archivo | Perm. |
|---|---|---:|---:|---|
| `.text` | 0x401000 | 4.546.560 | 4.546.048 | R-X |
| `_rwcseg` | 0x857000 | 4.096 | 1.536 | R-X |
| `.rdata` | 0x858000 | 311.296 | 308.224 | R-- |
| `.data` | 0x8A4000 | 4.169.728 | 262.144 | RW- |
| `_TEXT_HA` | 0xC9E000 | 69.632 | 68.608 | RW- |
| `_rwdseg` | 0xCAF000 | 4.096 | 512 | RW- |
| `.rsrc` | 0xCB0000 | 4.096 | 1.536 | R-- |

**Imports (medidos del PE, 214):** estáticos: KERNEL32 138, USER32 44, WS2_32 10, vorbisfile.dll 7, ADVAPI32 6, WINMM 4, ole32 3, GDI32 1, `EAX.DLL` 1; **diferidos (delay-load):** `d3d9`, `DINPUT8`, `DSOUND`, `DDRAW` (1 cada una). Las cifras por DLL de `RutaExeIndependiente.md` (p. ej. 142/48/… , d3d9 7) no coinciden con el PE; esta tabla sale del directorio de imports real. `EAX.DLL` y `vorbisfile.dll` son los únicos no del sistema.

**Recursos (3):** icono (744 B), grupo de iconos (20 B), un diálogo (302 B, id 104, 0x809). **Sin manifiesto, sin versión.**

**Protector/anti-manipulación:** 18 bloques (`ExeProtectionStub`) ya descartados; un exe propio no los ejecuta. **SEH:** cadena `fs:[0]` con manejadores dentro del código original (por ejemplo `0x83723B`): un exe que los ejecute debe enlazarse con **`/SAFESEH:NO`** (si no, Windows rechaza manejadores que no estén en la tabla de su imagen); el prototipo ya lo hace.

## 4. Estrategias

Escala de esfuerzo en **piezas de trabajo medidas**, no en fechas.

### (a) EXE «cáscara» (imagen original como archivo de datos)

El exe propio se enlaza con base 0x400000 fija y **reserva `0x401000–0xCB1000`**; al arrancar, un cargador lee el `gta_sa.exe` del usuario y copia sus secciones a sus direcciones, resuelve sus imports y salta al CRT original (o, en la variante a1, deja el `.text` original sin permiso de ejecución).

- **Qué exige:** `/BASE:0x400000 /FIXED`, `/DYNAMICBASE:NO /SAFESEH:NO /NXCOMPAT:NO`, reserva del rango como primera cosa de `.text` (**hay que hacerlo con ensamblador**: una sección propia `.orig` y una `.text$a` de datos salen en otro sitio; ver `prototipo/reserva_imagen.asm`), cargador, resolución de imports (214) y verificar el hash del archivo.
- **Pros:** **prototipable ya** (hecho); no toca ningún puntero (todo está donde estaba); da una medida de avance exacta con (a1); reutiliza todos los hooks sin cambios.
- **Contras:** **no es independencia de código**: el original sigue aportando el código no reconstruido (≥ 1.572 funciones, ≥ 577 KB, cota inferior) y el CRT. El exe necesita el `gta_sa.exe` del usuario siempre. Todo el código propio vive en 0xCB1000+, así que `GetModuleHandle(NULL)` apunta a nuestro exe (el hook `InjectHooksMain(hModule)` y las exportaciones de vtables pasan a un exe, no a una DLL).
- **Esfuerzo:** a0 (cargador + exe) hecho; a1 (VEH que registra cada función original ejecutada): pequeño (1 archivo + pruebas del usuario).

### (b) EXE completo con globales como variables C++ reubicables

- **Qué exige:** declarar y dimensionar **todos** los globales: 4.949 (propio) y hasta 20.265 (original); valores iniciales de 262 KB de `.data` y 311 KB de `.rdata`; redirigir 5.4 mil ranuras de vtable y 3,9 mil entradas de tablas; sustituir los 1.673 inicializadores estáticos y el CRT; cambiar el 100 % de `StaticRef` a variables y todos los accesos por base+índice a arrays con tamaño. Sin ningún código original: es el trabajo de las otras dos más la de **todo** el código no reconstruido (hoy solo se conoce la parte alcanzada desde el código propio; las 9.223 entradas de Ghidra sin nombre no están clasificadas) y el ABI de los `usercall`.
- **Pros:** exe de verdad (reubicable, ASLR, portable; librw abierta como opción).
- **Contras:** el más caro; el estado de los datos del juego (pools, arrays con tamaños desconocidos) no está catalogado; los 715 accesos solo por inmediatos exigen análisis; la validación par a par desaparece.
- **Esfuerzo:** muy alto; no prototipable hoy salvo una clase piloto.

### (c) Híbrida: datos del original en bloque fijo, código solo propio

El exe reserva `0x858000–0xCB1000` (datos) y rellena esa zona **desde el `gta_sa.exe` del usuario** (o desde una instantánea generada de él en la compilación); el `.text` original **no se mapea**. Los punteros de esas tablas a código original (5.434 ranuras enganchadas, 52 + 3.881 sin hook) se reescriben a código propio o a un trampolín que cuenta y falla.

- **Qué exige:** a1 hecho (medición), sustituir la cola de código (≥ 1.572 funciones: 780 raíces propias + D3DX9 como DLL del sistema, libpng/zlib enlazadas, CRT propio, QuickTime → Media Foundation), los 1.673 inicializadores (generables: casi todos son una constante) y una tabla de reubicación de punteros-a-código.
- **Pros:** es el camino corto a «sin código original»; deja los datos como están (el mapa de memoria que el código reconstruido ya da por supuesto) sin tener que dimensionar nada.
- **Contras:** necesita el archivo `gta_sa.exe` (o una extracción) **como datos** (aspecto de distribución: la extracción de `.rdata/.data` del original no debe subirse); depende de que el cierre de código llegue a cero, incluida la parte indirecta (virtual) no medida.
- **Esfuerzo:** alto pero acotado y medible por el contador de la variante a1.

### Cuál se puede prototipar ya y cómo se mide el avance

- **Prototipar ya: (a).** Medida del avance: **«el juego arranca hasta el menú con N funciones del original ejecutadas»**, N = número de entradas distintas del `.text` original ejecutadas (a1: `.text` original `PAGE_READONLY` tras instalar los hooks, VEH que registra EIP, lo marca ejecutable página a página y sigue). El objetivo es N = 0 (entonces (a) se convierte en (c)). Cifras de partida de esta medición: 779 raíces, ≥ 1.572 funciones, 1.673 inicializadores, 3.933 ranuras de tablas sin hook.
- (c) es la meta realista; (b) solo si hace falta portabilidad.

## 5. Prototipo

Todo en [`reversing/FnIND0A/prototipo/`](../../reversing/FnIND0A/prototipo/):

| Archivo | Para qué |
|---|---|
| `reserva_imagen.asm` | Reserva `0x8B0000` bytes como primera pieza de `.text` → VA `0x401000`–`0xCB1000`. |
| `cargador_imagen.cpp` | `WinMain` del prototipo: lee el `gta_sa.exe`, comprueba tamaño/PE, copia las 7 secciones, resuelve los imports y (opcional) quita el permiso de ejecución. Con `--ejecutar` llama a `DllMain(PROCESS_ATTACH)` del proyecto (instala los hooks) y entra por `0x824570`. |
| `enlazar_exe_prueba.cmd` | Enlaza los 1.101 `.obj` ya compilados + libs de Conan como exe. No recompila el proyecto. |
| `CMakeLists.txt` | Objetivo `gta_reversed_exe` (se activa con `-DGTASA_EXE_PROTOTIPO=ON`; desactivado por defecto; enlaza y comprueba). |
| `../verificar_exe.py` | Comprobación estática (sin ejecutar nada). |
| `salida/verificacion.json`, `link.log` | Resultado del enlace de este lote. |

**Resultado del enlace (HEAD 39276bea85, Debug):** código de salida 0, **0 símbolos sin resolver**, avisos LNK4075 (E&C) y LNK4098 (LIBCMT). Comprobación estática: base 0x400000 sí; sin relocaciones sí; sin ASLR sí; reserva en 0x401000 sí; **primer código propio en 0xCB1000**; reserva a cero en el archivo (el exe no lleva bytes del original); **no importa `gta_sa.exe`**; imports propios: kernel32 219, user32 159, gdi32 24, ole32 12, imm32 11, dbghelp 8, setupapi 8, shell32 9… `todo_ok = true`. Exe de 32,3 MB.

**Qué NO prueba:** que el exe arranque. El enlace sale limpio porque todo lo del EXE se resuelve por dirección fija; lo que falla, si falla, será en ejecución. Además `operator new` y similares siguen apuntando al EXE (el Debug no usa `NOTSA_STANDALONE`).

### Cómo lo probaría el usuario (sin que nadie más lance el juego a la vez)

1. Compilar o usar `…/prototipo/salida/gta_reversed_exe.exe` (ya enlazado; si hace falta repetirlo: `enlazar_exe_prueba.cmd D:\Original\wv-6 <salida>`, y `python -I reversing\FnIND0A\verificar_exe.py <salida>`).
2. Copiar el exe a la carpeta del juego (o dejarlo donde esté) y ejecutar **solo cargar**: `gta_reversed_exe.exe --ruta-original "…\gta_sa.exe"`. Debe terminar con código 0 y escribir `ind0a_informe.txt` con las 7 secciones copiadas, 9 DLL estáticas y «imports sin resolver: 0» (puede avisar de `EAX.DLL` si no está junto al exe original).
3. Si el paso 2 es limpio: `--ejecutar` (equivale a hoy: ASI sobre el original, pero con un exe nuestro). Si arranca hasta el menú, la estrategia (a0) vale.
4. Con `--sin-ejecutar-texto` el original se queda sin ejecución: se espera un fallo de acceso en la primera función original; es la base del contador N de la sección 4.

## 6. Conclusión honesta

**Porcentaje de dependencias que queda del EXE original (medido hoy):**

| Dependencia | Estado |
|---|---|
| Código (direcciones de código que el código propio usa) | 93,3 % con cuerpo propio; **6,7 % (890) sigue en el EXE**; cierre ≥ 1.572 funciones / 577 KB. |
| Funciones de RenderWare | las 724 de los mapas hechas, pero **630 destinos internos** (578 sin hook + 31 no reconstruibles + 21 de bibliotecas) siguen en el EXE. |
| CRT y bibliotecas | **100 % en el EXE** (CRT 314 fn / 230 KB; libpng+zlib 176 / 81 KB; D3DX9 321 / 132 KB; QuickTime 32). |
| Inicializadores estáticos | **100 %** (1.673). |
| Datos | **100 %**: 4.949 direcciones, 54.225 referencias; ni una variable propia sustituye un global del EXE. |
| Imports | 214: sistema + `EAX.DLL` + `vorbisfile.dll`. |

**Qué es lo siguiente:** (1) probar el prototipo; (2) cerrar las ~780 raíces reales (envolturas que los mapas no cuentan: 93 de juego y 578 de RenderWare, más 31+21+56 de bibliotecas y CRT) con el patrón de siempre (reconstrucción + validación), empezando por las 82 de `rpworld.cpp`; (3) implementar el contador N (a1); (4) decidir cómo sustituir D3DX9, libpng/zlib, CRT, QuickTime y los 1.673 inicializadores; (5) generar la instantánea de datos si se quiere (c).

**Riesgo grande:** el código reconstruido da por supuesto un mapa de memoria de **4,5 MB de datos con 4.949 direcciones tocadas por dirección fija**; mientras existan, **el exe propio tiene que reproducir ese mapa exactamente** (por eso (c) y no (b) es el camino). Además la medición es una cota inferior: no vemos las llamadas virtuales entre objetos creados por código original ni los 1.003 saltos indirectos, y el EXE entero tiene 17.765 entradas de función frente a las 12.4 mil reconstruidas.

**La meta «EXE independiente ya» no es alcanzable aún.** Lo que sí hay hoy es el **hito intermedio**: un exe de base fija con la imagen original cargada como archivo de datos (a0) y el método de medida para llevar a cero el código original ejecutado (a1).

---

## 7. zlib/libpng (lote LIB1A, 19/11/2026)

Primer paso para sustituir las **176 funciones de libpng + zlib** (81 KB) que el código propio todavía toma del EXE. Informe completo, mapa y banco: [`reversing/FnLIB1A/README.md`](../../reversing/FnLIB1A/README.md).

- **Bibliotecas compiladas desde fuente oficial, sin parches:** zlib 1.1.3 y libpng 1.0.12 (las versiones que lleva el EXE: cadenas `inflate 1.1.3 Copyright 1995-1998 Mark Adler` en `0x8841B1`, `deflate 1.1.3 …` en `0x883271`, `libpng version 1.0.12 - June 8, 2001` en `0x8DF70A`) en `libs/zlib-1.1.3/` y `libs/libpng-1.0.12/` (CMake propio, bibliotecas estáticas `zlib113` y `libpng1012`, mismo runtime que el proyecto, `/O2 /arch:IA32 /Gd`). Los 42 `.c`/`.h` copiados son idénticos byte a byte a los descargados. Único ajuste de compilación: `inftrees.c` con `-Dlocal=` para poder sustituir `huft_build` (que es `static`).
- **Opción CMake `GTASA_WITH_ZLIB_LIBPNG` (por defecto APAGADA):** enlaza las dos bibliotecas y hace que los 14 hooks de `ZlibInflate` (`0x7E9430`–`0x7EB260`) llamen a la biblioteca en lugar de a la reconstrucción a mano. El proyecto completo compila (Debug) con ella encendida y apagada.
- **Mapa EXE ↔ fuente** (`reversing/FnLIB1A/mapa_funciones.csv`): de las 343 funciones que se descubren en `0x7CF600–0x7EB5C0`, 41 con equivalencia de confianza alta, 104 media, 67 baja y 131 sin equivalente claro; de las 176 del cierre: 15 / 46 / 52 y 63 sin equivalente (incluidas las dos raíces `0x7CF600` y `0x7CF9B0`, que son `rtpng` de RenderWare, no libpng). Se corrige el dato «zlib 1.1.4» del proyecto: es 1.1.3.
- **Banco par a par EXE original vs biblioteca** (fuera del juego: el EXE se mapea en un proceso de prueba y se llaman sus funciones por dirección): las 14 funciones de zlib, **14 de 14 sin diferencias** (296.269 casos, ≥ 18.680 pares por función, oráculos independientes de Python sin fallos; las 14 mutaciones de cada una de las 2 rondas negativas se detectan). Prueba negativa de dos rondas. **`validated` sigue en 0:** falta la corrida en juego con registro (`register_hook.py`).
- **Siguientes lotes:** libpng lectura (`pngread/pngrutil/pngrtran/pngset/pngget`, ~90 funciones, con x87 y `png_struct` real), libpng escritura + `deflate`/`trees` (~70 funciones; `deflate 1.1.3` está en el EXE), subir la confianza de las 63 sin equivalencia, reconstruir `rtpng` y probar `ZlibInflate` con la opción encendida dentro del juego.

## 8. Estado del cargador (lote IND1A, 19/11/2026)

**El cargador funciona**: `gta_reversed_exe.exe` carga la imagen del `gta_sa.exe` del usuario, instala los hooks una sola vez y llega al **menú principal**, estable 170-200 s (límite de la guardia), 0 excepciones, ~145 fotogramas/s. Informe completo, tabla de iteraciones, comandos y guion de prueba a mano: [`reversing/FnIND1A/README.md`](../../reversing/FnIND1A/README.md).

| Modo | Resultado |
|---|---|
| Solo cargar (`--ruta-original`) | Limpio: 7 secciones, hash por sección = archivo (salvo la IAT), 214/214 imports, 9 DLL. Comprobación automática `reversing/FnIND1A/verificar_carga.py` (`todo_ok: true`) |
| `--ejecutar --sin-hooks` | El original corre solo sobre la cáscara hasta el menú |
| `--ejecutar` | Hooks (12.445) + WinMain original hasta el menú, estable |
| FastLoader (partida 2) | Falla en el **código original** `0x004CA5B5` (lectura de 0) tras cargar la partida; pendiente |

**Cambios de diseño respecto a IND0A** (el cargador original no podía arrancar): (1) cargar la imagen desde un callback TLS (`.CRT$XLB`) del exe, antes del punto de entrada y sin CRT; (2) anular, hasta que arranque el CRT propio, la ranura del callback TLS dinámico (`thread_local` como `CGeneral::randomEngine` revientan sin los cerrojos del CRT); (3) **entrar por el CRT original** `0x824570` y redirigir su llamada a WinMain (`0x8246EC`) hacia una función propia que inicia el CRT propio, instala los hooks y llama al WinMain original: el `operator new` del proyecto usa el montón del juego (`0x82119A`) y los inicializadores estáticos propios solo pueden correr después de `_heap_init` del original.

**Contador de funciones originales ejecutadas hasta el menú** (INT3 en 20.093 inicios de función; el VEH cuenta cada primera entrada): **2.832 sin hooks, 148 con hooks** (78 del CRT/bibliotecas, 53 de RenderWare/D3D, 17 del código del juego). Lista con llamadores: `reversing/FnIND1A/datos/funciones_ejecutadas_hasta_menu_hooks.md`. Es una cota inferior (solo cuenta funciones de la lista), contrastada con un contador por páginas.

**Para llegar a 0 en este camino**: las 17 del juego y las 53 de RenderWare (olas pequeñas), y sustituir el CRT original (`operator new/delete`, `malloc`…, 78 funciones) por el propio, lo que exige decidir qué montón comparten el código propio y las estructuras que el original sigue liberando. Siguiente medida: repetir el contador con una partida cargada (tras resolver el fallo de FastLoader).

**Prueba a mano**: `pwsh -File D:\Original\wv-6\reversing\FnIND1A\probar_a_mano.ps1` (crea `E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A`, sin `dinput8.dll` ni `scripts\` para no cargar el ASI otra vez, y lanza el exe sin límite); mirar el menú principal y que en la consola salga una sola vez «Funciones del juego reemplazadas por código C++: 12445».
