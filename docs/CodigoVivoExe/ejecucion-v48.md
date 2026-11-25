<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cierre de los descartes V4–V8: búsqueda por registro de las llamadas indirectas

Rama `vivo-v48` (desde `origin/main` 6fc1d58ba9, 21/11/2026). Solo análisis estático (capstone sobre el EXE canónico `72ae59e4…ecc9b`, el código fuente y los CSV); no se tocó C++ y no hay compilación ni humo que hacer.

Herramienta: `contrib/codigo_vivo_cierre_v48.py` (extiende `codigo_vivo_cierre.py`). Resultado: [cierre-p1.csv](cierre-p1.csv) (columna `motivo_v48`), [cierre.csv](cierre.csv), [funciones-vivas.csv](funciones-vivas.csv), [plan.csv](plan.csv), [plan-lotes.md](plan-lotes.md) y [busqueda-registro-v48.csv](busqueda-registro-v48.csv) (las 114 llamadas indirectas, una fila cada una).

## 1. Cifras antes y después

Antes = `cierre-p1.csv` publicado (1.197 funciones de V4–V8: 845 «verificadas sin cerrar», 340 «condicionadas a hueco o tabla» y 12 abiertas). Después = este cierre.

| Lote | Funciones | Antes (verificada / condicionada / abierta) | Después (cerrada / abierta) | Estado del lote |
| --- | ---: | --- | --- | --- |
| V4-1 D3DX9 | 250 | 137 / 113 / 0 | 250 / 0 | cerrado |
| V4-2 D3DX9 | 250 | 149 / 101 / 0 | 250 / 0 | cerrado |
| V4-3 D3DX9 | 250 | 177 / 73 / 0 | 250 / 0 | cerrado |
| V4-4 D3DX9 | 182 | 162 / 20 / 0 | 182 / 0 | cerrado |
| V5-1 CRT | 70 | 51 / 12 / 7 | 59 / 11 | parcial |
| V5-2 CRT | 70 | 59 / 9 / 2 | 66 / 4 | parcial |
| V5-3 CRT | 63 | 52 / 8 / 3 | 57 / 6 | parcial |
| V6-1 QuickTime | 31 | 30 / 1 / 0 | 31 / 0 | cerrado |
| V7-1 RenderWare | 17 | 14 / 3 / 0 | 13 / 4 | parcial |
| V8-1 libpng | 14 | 14 / 0 / 0 | 14 / 0 | cerrado |
| **Total** | **1.197** | **845 / 340 / 12** | **1.172 / 25** | |

De las 845 «verificadas» del encargo (625 D3DX9, 162 CRT, 30 QuickTime, 14 RenderWare, 14 libpng) se cierran **835**: las 625 de D3DX9, 154 del CRT, las 30 de QuickTime, 12 de RenderWare y las 14 de libpng. Se **reabren 10** (8 del CRT y 2 de RenderWare, ver 3). Además se cierran 335 de las 340 «condicionadas» (V4: 307; V5: 26; V6: 1; V7: 1) y 2 de las 12 abiertas (`0x82FE09` y `0x830068`, que la rama del locale ya enganchó: su cuerpo no se ejecuta); quedan abiertas 25 funciones en total (21 de V5 y 4 de V7).

## 2. Método

El cierre anterior probaba que ninguna **arista directa, dword de la imagen ni literal del C++** alcanza la función, pero no podía descartar que una de las 114 llamadas indirectas de las 283 funciones vivas conservadas llegara a ella. Esta parte lo resuelve en tres pasos.

**2.1 Las 114 llamadas (`--sitios`, [busqueda-registro-v48.csv](busqueda-registro-v48.csv)).** 39 `jmp [reg*4+tabla]`, 26 `call/jmp [reg+disp]` y 49 `call/jmp reg`:

* **39 tablas de saltos** (`_memmove` 16, `_memcpy` 16 y 7 de libpng/RenderWare): se leen las tablas del EXE y todas las entradas caen dentro de la propia función (los índices negativos de `_memmove`/`_memcpy` se resuelven hacia atrás); ninguna sale de la función.
* **Código de DLL del sistema**: 4 `jmp eax` de los thunks `DelayLoad_*` (los rellena `__delayLoadHelper2` con `GetProcAddress` de d3d9/dinput8/ddraw), `call reg` sobre punteros de la IAT (`GetModuleHandleA`, `DeleteCriticalSection`, `WideCharToMultiByte`, `GetEnvironmentStringsW`, `GetCPInfo`) y sobre punteros que devuelve `GetProcAddress` (user32, `CorExitProcess`).
* **Punteros de función guardados en estructuras o datos** (el resto): callbacks de `png_struct` y `z_stream`, `RwEngineInstance` (0xC97B24) y vtables COM de D3D9, manejadores de excepciones y `terminate`/`_inconsistency`/`signal`, tablas `_initterm` y `_onexit`, `_CallMemberFunction`, iteradores de vectores de constructores. Su destino solo puede ser una **dirección tomada por código vivo o por datos vivos** (aparece como literal en algún sitio) o código de DLL; se trata en 2.2.

**2.2 Punto fijo de alcance (`--alcance`).** Conjunto de funciones y huecos del `.text` alcanzables desde las raíces:

* raíces: referencias del C++ que no son enganche, la entrada del CRT (0x824570), los inicializadores y las 283 funciones vivas conservadas; una función enganchada (`hooks_src` de `source/`) no se recorre (su cuerpo no se ejecuta);
* aristas: `call/jmp/jcc` directos, inmediatos y desplazamientos que apuntan al `.text`, caída al trozo siguiente, y operandos que apuntan a datos. Una referencia a una tabla de datos marca vivos los punteros a código de la racha contigua que la contiene (solo cuentan inicios de función, entradas de hueco y destinos dentro de la propia función; así una cadena ASCII como «pow» no cuenta como puntero) y, en el modo amplio, los de las tablas a las que apuntan esos datos (ventana de 0x40 atrás y 0x100 adelante). El resultado definitivo es la **unión** de los dos modos;
* los huecos del `.text` (sin función de Ghidra) se recorren por flujo desde cada entrada alcanzada, no enteros;
* una llamada indirecta no resuelta puede ir a cualquier función cuya dirección esté tomada por código o datos vivos: eso ya está en el conjunto (aristas imm y de datos), así que no añade destinos; lo único que no excluye es un destino **calculado**.

**2.3 Destinos calculados (`--calculados`).** Sobre las ~86.000 instrucciones del código vivo alcanzable: sin `call/pop` (no hay código con posición relativa), sin tablas de bytes ni palabras indexadas dentro del `.text` que construyan direcciones, sin constantes cerca de la base de imagen salvo `mov eax, 0x400000` en `0x820DC4` (el ayudante de carga retardada `0x820DBD` suma la base a RVA de descriptores de importación y llama a `LoadLibraryA`/`GetProcAddress`: apunta a datos, no a código) y 2 aritméticas con una dirección del `.text` (`sub eax, 0x74A260` en `0x74A279`: comparación con `renderCallBack`; `and edx, 0x7E9300` en `0x7E1341`: `png_zalloc` como literal). Las únicas cargas de DLL son `GetModuleHandleA`/`LoadLibraryA`/`GetProcAddress` (10 sitios): devuelven direcciones de DLL, no del EXE.

**2.4 Isla de D3DX9.** Los archivos `D3dxErrorLog.cpp`, `D3dxHlslCompiler.cpp`, `D3dxShaderAssembler.cpp` y `D3dxShaderSource.cpp` (lote NEWDEL, ya en `main`) reconstruyen 13 rutinas internas del compilador de shaders de D3DX9 con enganches activos y **llaman a otras rutinas internas por dirección** (36 literales hacia V4: `0x766088`, `0x76B9E0`, `0x76B87F`…). Con la lectura plana (cada literal es una raíz) quedarían 838 de las 932 funciones de V4 vivas y V4 no se cerraría. Pero esos archivos solo se entran por los enganches que instalan, todos sobre direcciones internas del compilador embebido (`0x76E5FB`, `0x773022`, `0x77EE77`, `0x76FAEB`, `0x772BB1`…), y ningún otro archivo los usa (solo `InjectHooksMain.cpp` los registra). Las únicas entradas del compilador embebido son los 12 envoltorios `0x7652AE–0x7654B8` y sus 4 enganches están activos (E3, lote cerrado: reenvían a `d3dx9_43.dll`). `ISLAS` marca esos 4 archivos: sus literales solo son raíces si el alcance llega a una de sus direcciones enganchadas, y no llega (ninguna llamada de código vivo ni de datos vivos).

## 3. Lo que queda abierto, con el motivo exacto

**V7-1, 4 funciones (hallazgo: código vivo sin enganche).** `0x4CAD90`, `0x4CAEA0`, `0x4CA5A0` y `0x4CAAE0` (ayudantes `usercall` del controlador de rasters de D3D9) **no están muertas**: `_rwD3D9DeviceSystemStandards` (`rwcore.cpp:6854`) guarda en la tabla de funciones estándar del dispositivo 12 direcciones del EXE que no tienen función de Ghidra ni enganche (huecos de código vivos): `0x4CAE40/60/80`, `0x4CB4C0/E0`, `0x4CB520`, `0x4CBCB0`, `0x4CBD40/50`, `0x4CA4E0`, `0x4CD4D0`, `0x4CD820`. El hueco `0x4CAE60` llama a `0x4CAD90` (que salta por tablas a `0x4CAAE0` y `0x4CA5A0`) y `0x4CB4C0` llama a `0x4CAEA0`. Son los manejadores de raster (bloqueo/desbloqueo y similares) que sigue ejecutando el EXE. No se escribió C++: son 12 manejadores con ABI `usercall` y no es un cambio corto; pasan a la parte 6 (huecos H1) y a la sustitución S2. Las 4 pasan de «verificada» o «condicionada» a **abierta**.

**V5, 21 funciones.**

* *Llamadas por el C++ al cuerpo del EXE* (4): `0x822E83`, `0x822EBF`, `0x822EFA`, `0x822F94`. Los destructores enganchados de `CrtExceptionDtor.cpp` (líneas 53–56) llaman a estos cuerpos con `Body(0x…)`; el llamador antiguo (`0x822F40…`) está enganchado, pero el C++ los usa. Se cierran reescribiendo esos 4 cuerpos en C++.
* *Arranque y carga retardada, vivos mientras exista el EXE* (6 directas + 11 que cuelgan de ellas): `0x820DBD` (ayudante de carga retardada; lo llaman los 4 thunks `DelayLoad_*`, F3), `0x82134F`, `0x826EB2`, `0x826F04` (llamados por `0x821397`, F1), `0x823B37` (`0x821D18`, F1), `0x835996` (llamado por `0x830380`, locale, rama del agente del locale) y las que cuelgan de ellas: `0x824F86`, `0x825018`, `0x825104`, `0x825136`, `0x826EC4`, `0x82D726`, `0x831D46`, `0x831F4C`, `0x8338F9`, `0x836712`, `0x836842` (las cadenas completas están en `motivo_v48`). Los llamadores son funciones conservadas de F1/F3/E2 (decididas «enlazar» o «muere con el arranque»): se cierran cuando la fase 8 las retire.

Todo lo demás se cierra. Las 12 abiertas del encargo se resuelven así: 10 siguen abiertas (arriba, con el motivo corregido: 4 por el C++ y 6 por llamadores vivos) y 2 se cierran (0x82FE09 y 0x830068, enganchadas por `CrtLocale.cpp`).

## 4. Salvedades

* **Modelo de datos.** Las tablas de punteros se resuelven por racha contigua y ventana, no con un análisis de flujo completo; se aplican los dos modos y se exige que ninguno alcance la función. Una estructura con punteros intercalados entre campos que el código recorra desde un desplazamiento lejano podría escapar al modo estricto; el modo amplio la cubre solo hasta 0x40 atrás y 0x100 adelante.
* **C++ sin literal hexadecimal.** El escáner (`escanear_fuente`) solo ve literales hexadecimales de 6–8 dígitos; una dirección escrita de otra forma (decimal, base + desplazamiento) no se vería.
* **Documentos desfasados respecto a `main`.** Los CSV publicados eran anteriores a las ramas NEWDEL y locale, por eso `codigo_vivo_cierre.py` re-ejecutado sobre `main` marca 26 funciones de V4 como abiertas por los literales de la isla y 0x82FE09/0x830068 como llamadas desde funciones «vivas» que ya están enganchadas; este cierre las trata con `ISLAS` y con `hooks_src`.
* La rama del locale (`_updatetlocinfo`, `_tolower_lk`, `_isctype`, `_wctomb`, `_mbtowc`) y DatosExe no se tocaron.

## 5. Reproducir

```
python -X utf8 contrib/codigo_vivo_cierre_v48.py --cache hechos.pkl --cache-aristas aristas.pkl --sitios --alcance --calculados --aplicar
python -X utf8 contrib/codigo_vivo_plan.py        # vuelca cierre.csv en plan.csv y plan-lotes.md
```

(`--modo amplio` solo ejecuta el modo amplio; sin él se calculan los dos y se toma la unión. Borrar los `.pkl` tras cambiar el código fuente o el EXE.)
