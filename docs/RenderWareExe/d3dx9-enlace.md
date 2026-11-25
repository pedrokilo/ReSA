# D3DX9: enlace y dependencia de DLL (24/11/2026, rama `enlazar-d3dx9`)

[← RenderWare y D3D en el EXE](README.md) · [Comparativa del independiente](../CodigoVivoExe/comparativa-standalone-original.md) · herramientas y salidas: `reversing/FnD3DX9C/`

**Resultado en una línea:** el independiente ya **no carga ninguna DLL de D3DX9 en una partida normal**. Lo que el juego usa de verdad (cuatro funciones de matemática y el ensamblador de *vertex shaders*) es ahora C++ propio, **bit a bit igual** que `d3dx9_43.dll` en bancos y en el propio juego; `d3dx9_43.dll` (+ `D3DCompiler_43.dll`) queda solo como **respaldo** para lo que el ensamblador propio no cubre (`vs_1_1`, HLSL, otras versiones) y se puede dejar junto al exe con `contrib/copiar_d3dx9_redist.ps1`. Enlazar D3DX9 *estáticamente de verdad* **no es posible** con lo que existe (sección 3a).

## 1. Qué usa realmente el juego (medido)

Llamadores estáticos (todo `source/`): solo `rpworld.cpp` (pipeline de *vertex shaders* de RenderWare D3D9). No hay ninguna otra llamada a D3DX9 en el proyecto (la búsqueda de `D3DX` fuera de `libs/imgui` y de `d3dx9math.cpp` solo da `rpworld.cpp`).

| Función D3DX9 | Quién la llama | ¿Se ejecuta en una partida normal? (contadores de `--d3dx9-log`, ejecución `propio_log`: arranque, escena inicial y 130 s) | Solución |
|---|---|---|---|
| `D3DXAssembleShader` | `_rpD3D9VertexShaderCreate` (0x75F0B0) | **Sí: 12 shaders distintos** (391–1058 B de texto, bytecode 292–732 B), todos `vs_2_x`, banderas del EXE = 4, ninguno necesitó reintento con `SKIPVALIDATION` | ensamblador propio (`D3DX9NativeAssembler`) |
| `D3DXMatrixMultiply` | `_rwD3D9VSGetComposedTransformMatrix` | **Sí: ≥ 2.048 llamadas** | propia (`D3DX9NativeMath`) |
| `D3DXVec3TransformNormal` | `_rwD3D9VSGetNormalInLocalSpace`, `_rwD3D9VSGetRadiusInLocalSpace` | **Sí: ≥ 8.192 llamadas** | propia |
| `D3DXMatrixMultiplyTranspose` | `_rwD3D9VSGetComposedTransformMatrix`, `_rwD3D9VSGetWorldViewTransposedMatrix` | **Sí: ≥ 8.192 llamadas** | ya era propia desde EXT1A |
| `D3DXMatrixTranspose` | seis `_rwD3D9VSGet*Matrix` | No en esa sesión (0 llamadas); alcanzable | propia |
| `D3DXVec3TransformCoord` | `_rwD3D9VSGetPointInLocalSpace` | No en esa sesión (0 llamadas); alcanzable | propia |
| `D3DXFloat16To32Array` | nadie (solo el gancho 3DNow! del EXE) | No (0) | propia (por completitud, bit a bit) |
| `D3DXCompileShaderFromFileA/W`, `FromResourceA` | nadie (EXT1A/FnD3DX9B: «sin uso por el juego») | No (0) | solo respaldo por DLL |

Las otras ~900 funciones de D3DX9 del EXE original (preprocesador, HLSL, efectos, texturas, mallas…) **ningún código del proyecto las llama**: eran código muerto enlazado. Los siete ganchos `D3DX9AcosPS`, `D3DX9SinCosPS*`, `D3DXVec2Transform_3DNow`… (0x7C1D80–0x7C381F) son rutinas internas del D3DX9 del EXE que nadie del proyecto invoca.

## 2. De dónde carga el exe la DLL (medido)

* Código: `D3DX9System::Get<>` (`d3dx9math.cpp`), carga **perezosa** (no al arrancar): `LoadLibraryExA("d3dx9_43.dll", LOAD_LIBRARY_SEARCH_SYSTEM32)` (en un proceso de 32 bits, `SysWOW64`) y, si falla, `LoadLibraryA` (carpeta del exe primero). Antes de este cambio se disparaba al ensamblar el primer shader (en las ejecuciones, a los ~17 s del lanzamiento, al crear los pipelines de RenderWare).
* Espía de archivos (`FnSinOriginal`, ejecución previa `sinorig1`): `\??\C:\WINDOWS\SYSTEM32\d3dx9_43.dll` (45 líneas del espía sobre `d3dcompiler_43.dll` de System32 y 30 sondas en la carpeta del juego: `d3dx9_43.dll` la carga al ensamblar). Con `--d3dx9-dll` (comportamiento anterior) en `_d3dx_run`: módulo `C:\WINDOWS\SYSTEM32\d3dx9_43.dll` presente a los 60 s y 120 s.
* En este PC existe porque se instaló el Runtime de usuario final de DirectX: `SysWOW64\d3dx9_24 … 43.dll` y `D3DCompiler_43.dll` con fecha de creación **24/05/2026 17:14–17:19**, firmadas por Microsoft (`d3dx9_43.dll` 9.29.952.3111, SHA256 `0B28546B…D64F5F`, idéntica a la del cab del SDK). El instalador sale de `D:\Grand Theft Auto San Andreas\Redistributables\DirectX\DXSETUP.exe` (el pack de GTA SA lo trae) y además está instalado el DirectX SDK (June 2010). **Un Windows 10/11 limpio no la trae.**
* **Qué pasaba sin ella (comportamiento anterior, ejecución `dll_ausente`: `--d3dx9-dll` con `d3dx9*`/`d3dcompiler*` inexistentes para el proceso):** no falla al arrancar; el juego corre los 134 s; al primer shader se registra `D3DX9: no se pudo cargar d3dx9_43.dll` y `D3DXAssembleShader` devuelve `D3DERR_NOTAVAILABLE`: **los modelos con esqueleto (CJ, peatones, ocupantes de los coches) no se dibujan** (captura `ejecuciones/dll_ausente-*/cap_t120.png`: coche de policía sin ocupantes; con el exe nuevo se ven) y las funciones de matemática devuelven `out` sin escribir. Es decir: **degradación visible, no fallo duro.**

## 3. Opciones evaluadas

**(a) Enlazar estáticamente un `d3dx9.lib` con el código — NO EXISTE.** Evidencia:
* `Microsoft DirectX SDK (June 2010)\Lib\x86\d3dx9.lib` (87.600 B) y `d3dx9d.lib` (87.926 B), y los x64 (81.324 / 81.648 B): `dumpbin /headers` → **329 miembros, todos `Type: code`, `Name type: undecorate`**, 658 símbolos `__imp_`: son **bibliotecas de importación** de `d3dx9_43.dll`/`d3dx9d_43.dll`, sin código.
* Búsqueda de `d3dx9*.lib/.a` en el SDK de DirectX, Windows Kits, Visual Studio, el `conan-cache` del repo y las redistribuibles del juego: **ninguna otra** (las únicas apariciones de «d3dx9» en el repo son el backend `imgui_impl_dx9`, que usa `d3d9.h`, y los documentos). El SDK no incluye código fuente de D3DX9 (las muestras usan la DLL). Microsoft nunca publicó D3DX9 como biblioteca estática.
* El «D3DX9 estático» que existía es el que estaba **compilado dentro del `gta-sa.exe` original** (0x765000–0x7C4000, 922 funciones), que **no vale para independencia** (opción d).

**(b) Importación + DLL junto al exe — implementada como respaldo.** `d3dx9_43.dll` (Microsoft, firmada) y `D3DCompiler_43.dll` salen **localmente** de los cab oficiales del DirectX SDK (June 2010): `…\Redist\Jun2010_d3dx9_43_x86.cab` y `Jun2010_D3DCompiler_43_x86.cab` (el juego trae los mismos en `Redistributables\DirectX`; la redistribución con juegos la permite la licencia del SDK/Runtime). `contrib/copiar_d3dx9_redist.ps1 -Destino <carpeta del exe>` extrae de los cab, **comprueba la firma Authenticode (Microsoft Corporation)** e imprime el SHA256 (`d3dx9_43.dll` 9.29.952.3111 `0B28546BE22C71834501F7D7185EDE5D79742457331C7EE09EFC14490DD64F5F`; `D3DCompiler_43.dll` `2F23182EC6F4889397AC4BF03D62536136C5BDBA825C7D2C4EF08C827F3A8A1C`). No se descargó nada de internet; si no estuvieran, la fuente sería el instalador web/offline «DirectX End-User Runtimes (June 2010)» de Microsoft (`directx_Jun2010_redist.exe`), **que no se ha descargado**. **Hace falta el par**: `d3dx9_43.dll` carga `D3DCompiler_43.dll` al ensamblar/compilar.
Probado (`fallback_carpeta`): con `System32`/`SysWOW64` bloqueados para el proceso y las dos DLL en la carpeta del juego, `--d3dx9-dll` carga `D:\Original\_d3dx_run\d3dx9_43.dll` (el espía registra el bloqueo del sistema y la apertura de la copia local) y el juego corre 134 s sin errores.

**(c) Reimplementar en C++ lo que el juego usa — IMPLEMENTADA (opción principal).** Ver sección 4.

**(d) Usar el D3DX9 que vive en el EXE original — descartada** (no es independencia: es el binario original).

## 4. Qué se implementó

| Archivo (dos por unidad) | Contenido |
|---|---|
| `source/game_sa/RenderWare/rw/D3DX9NativeMath.h/.cpp` | `Float16To32Array`, `Vec3TransformCoord`, `Vec3TransformNormal`, `MatrixMultiply`, `MatrixTranspose` con intrínsecos SSE (sin `__asm`), misma secuencia que la variante SSE de `d3dx9_43.dll` (desensamblada de la DLL en ejecución con `reversing/FnD3DX9C/volcar_dll.cpp`): p. ej. `MatrixMultiply` suma las filas 0 y 3 por parejas `(t0+t1)+(t2+t3)` y las 1 y 2 en cadena; `Vec3TransformCoord` hace `(y·f1 + x·f0) + (z·f2 + f3)` y divide con `rcpps` + una iteración de Newton (igual que la DLL, por lo que es igual de dependiente de la CPU); `Float16To32Array` **no** trata el exponente 31 como infinito/NaN (0x7C00 → 65536). Se conserva el orden de operandos para que coincida incluso la carga útil de los NaN. |
| `source/game_sa/RenderWare/rw/D3DX9NativeAssembler.h/.cpp` | Ensamblador de *vertex shaders* `vs_2_0`/`vs_2_x` para la gramática del generador del juego: `dcl_*`, `mov/mova/add/mul/mad/dp3/dp4/rsq/rcp/expp/max/min/slt/sge/m3x3/m4x3/m4x4`, `r/v/c/a0`, `c[N]`, `c[a0.x + N]`, `c[3 + 5]`, negación, máscaras y permutaciones, `oPos/oFog/oDn/oTn`. Lo que no reconoce devuelve `Unsupported` y se usa la DLL. |
| `source/game_sa/RenderWare/rw/d3dx9math.cpp/.h` | `D3DX9System`: modo **propio (por defecto)**, `--d3dx9-dll` (siempre la DLL: comportamiento anterior, para comparar) y `--d3dx9-verificar` (ejecuta ambos y compara bit a bit, registra `ERR` si difieren; carga la DLL, solo diagnóstico). `ID3DXBuffer` propio (`NativeBuffer`, misma tabla virtual: Release = ranura 2, GetBufferPointer = 3, GetBufferSize = 4). Contadores de uso con `--d3dx9-log` (primera llamada y cada potencia de dos). El respaldo por DLL sigue lazy: sin shaders no soportados no se carga nunca. |
| `source/extensions/CommandLine.cpp` | `--d3dx9-dll`, `--d3dx9-verificar`. |
| `contrib/copiar_d3dx9_redist.ps1` | respaldo (b), con comprobación de firma y hash. |

Casos que **van a la DLL** (y por tanto sí requieren `d3dx9_43.dll` + `D3DCompiler_43.dll` si ocurren): `vs_1_1` (GPU solo con vertex shader 1.1; el generador emite `mov a0.x`), banderas `D3DXSHADER_DEBUG`, `defines`/`include` no nulos, registros o instrucciones fuera de la lista, `D3DXCompileShader*` (nadie los llama). Si la DLL falta, se registra el error una vez y se devuelve `D3DERR_NOTAVAILABLE` (como antes).

## 5. Evidencia de igualdad con la DLL

| Banco / prueba | Resultado |
|---|---|
| `banco_matematica.exe` (contra `d3dx9_43.dll` del sistema; 6 distribuciones de entrada: ±100, valores especiales con NaN/inf/denormales/−0, mezcla, bits aleatorios, 1e-3, 1e6; también `out` = entrada) | `Vec3TransformCoord` 600.000 pares, `Vec3TransformNormal` 600.000, `MatrixMultiply` 2.400.000, `MatrixTranspose` 600.000: **0 diferencias de bits, ni siquiera en la carga útil de los NaN**. `Float16To32Array`: los 65.536 valores, en bloque de 65.536 (camino SSE2 de la DLL) y en bloques de 1/2/3/5/7 (camino escalar x87): **0 diferencias**. |
| `banco_ensamblador.exe` (descriptores de `BuildVertexShaderSource` — el mismo código de `rpworld.cpp`, extraído sin copiarlo —, versiones `vs_2_0` y `vs_2_x`, 3.000 descriptores aleatorios y estructurados, más ~48.000 líneas sueltas de gramática con todos los destinos/orígenes/permutaciones/negaciones/relativos) | 53.093 fuentes distintas: **47.583 ensambladas por ambos con bytecode idéntico, 0 diferentes**; 5.510 `Unsupported` (descriptores aleatorios fuera de lo que RenderWare usa: `oT8`, `v16`, `c256`, pesos > 4, `oFog.x`…) que van a la DLL. Cuando la DLL rechaza la validación (más estricta que el ensamblador del EXE) y se reintenta con `SKIPVALIDATION`, el bytecode propio coincide igualmente. |
| En el juego, `--d3dx9-verificar` (`verificar-*`): 130 s, el propio y la DLL a la vez en cada llamada | **0 diferencias** en `MatrixMultiply` (≥ 2.048 llamadas), `Vec3TransformNormal` (≥ 8.192) y en los 12 shaders reales (bytecode idéntico); sin ningún `ERR`. |

## 6. Demostración de que ya no depende de la DLL

Carpeta de juego propia `D:\Original\_d3dx_run` (`reversing/FnD3DX9C/crear_carpeta_d3dx.ps1`): junctions de solo lectura a los datos, **sin** `gta-sa.exe`, `d3dx9_43.dll` ni `D3DCompiler_43.dll`. `correr_d3dx.py` lanza `gta_sa_standalone.exe` con el espía de archivos inyectado antes de los callbacks TLS (`espia_bloqueo.dll`, variante de `espia_archivos` que puede **bloquear** nombres), 130 s, y a los 20/60/120 s vuelca los módulos (`EnumProcessModulesEx`) y captura la ventana.

| Ejecución | Qué es | `d3dx9*`/`d3dcompiler*` cargados | Accesos del espía a `d3dx9*`/`d3dcompiler*` | Resultado |
|---|---|---|---|---|
| `propio_log`, `propio2` | exe nuevo, modo por defecto | **ninguno** (116 módulos) | **0** | corre 133,9 s; sin `ERR` ni nada nuevo en el log |
| **`bloqueo`** | exe nuevo, `d3dx9_`/`d3dcompiler_4` **inexistentes** para el proceso | **ninguno** | **0 intentos** (0 bloqueados: el exe ni lo intenta) | corre 134,0 s; 12 shaders propios; ocupantes del coche visibles; sin `ERR` |
| `propio_con_dll_en_carpeta` | exe nuevo con las dos DLL **presentes** en la carpeta del juego | ninguno | 0 | no las toca |
| `verificar` | `--d3dx9-verificar` | `System32\d3dx9_43.dll` (a propósito) | 70 | 0 diferencias (sección 5) |
| `dll_log` | `--d3dx9-dll` (comportamiento anterior) | `C:\WINDOWS\SYSTEM32\d3dx9_43.dll` | 65 | referencia A/B |
| `fallback_carpeta` | `--d3dx9-dll` con `System32`/`SysWOW64` bloqueados y las DLL en la carpeta | **`D:\Original\_d3dx_run\d3dx9_43.dll`** | 48 (1 bloqueado) | opción (b) funciona, 133,6 s |
| `dll_ausente` | `--d3dx9-dll` sin DLL en ningún sitio | ninguno | 232 (116 bloqueados) | **degradación**: ocupantes del coche invisibles, `ERR` registrado (sección 2) |
| `exe_anterior` | el exe de `wv-int` anterior a este cambio | `System32\d3dx9_43.dll` | 43 | referencia |

El único aviso del log común a todas las ejecuciones es `TimeCycle.cpp:186` (línea de `timecyc` incorrecta), anterior a este trabajo. En 3 de las 8 ejecuciones (también en `exe_anterior`, sin relación con D3DX9) el registro muestra una excepción AV de primera oportunidad en `ntdll!RtlImageNtHeaderEx ← FreeLibrary ← igd12um32xel ← d3d9on12`: es el controlador Intel (D3D9 sobre D3D12) durante `Direct3DCreate9Ex`; no la causa este cambio.

### Comparación de imágenes

Las escenas del exe independiente **no son deterministas** a igualdad de segundos (la secuencia inicial de la partida avanza según el tiempo real: a los 120 s la cámara de la escena del coche de policía está en puntos distintos entre dos ejecuciones idénticas), así que una comparación píxel a píxel entre dos ejecuciones da diferencias grandes aunque el código sea el mismo (`propio_log` frente a `propio2`, mismos ejecutables: tamaños de ventana distintos por el estado de `gta_sa.set`, ya normalizado en `correr_d3dx.py`). Lo que sí se comprobó: las mismas escenas en modo propio y en modo DLL muestran los ocupantes con esqueleto (shaders de piel) y el resto; en modo «DLL ausente» desaparecen; y la prueba fuerte de igualdad es la verificación bit a bit dentro del propio juego (sección 5), no la captura. Las escenas fijas del ASI (ropero y marcadores, sección 7) dan BIEN.

## 7. Humo con el ASI

ASI de este árbol (`gta_reversed.asi`, SHA256 `9df49298ef932b3fa52a912bde01723fa79512b04ed32e07f1debf950a582d5b`, enlazado con `reversing/FnD3DX9C/enlazar_asi.py`: los `.obj` de `wv-int` con `d3dx9math`, `D3DX9NativeMath`, `D3DX9NativeAssembler` y `CommandLine` de `over\`) contra el `gta_sa.exe` original de la copia de pruebas, desde `wv-8/reversing/FnBIS1` con `AGUA_ASI=<asi>` y `cal_mal.args` (escena fija: interior, teletransporte frente al ropero):

| Prueba | Resultado |
|---|---|
| `python prueba_ropero.py cal_mal.args d3dx_rop` | **VEREDICTO BIEN** (15 entidades, 12 marcadores, puerta y marcador presentes; CJ con esqueleto y tatuajes correctos en `ropero_d3dx_rop.png`) |
| `python prueba_marcadores.py cal_mal.args d3dx_mar` | **VEREDICTO BIEN** (12 marcadores en memoria, 12 que cambian en 1 s, 6.357 píxeles rojos) |

(Una primera tentativa de `prueba_marcadores.py` abortó con «el candado esta tomado por otra prueba»: otro agente tomó `gta_lock` entre la espera y el lanzamiento; no llegó a tocar nada y se repitió con `humo_marcadores.sh`.) `prueba_agua.py` no se ejecutó (excluida por indicación).

## 8. Lo que NO se ha cerrado o no se verificó

* **No hay enlace estático posible** de D3DX9 (sección 3a). La independencia total es del camino normal; el respaldo (b) sigue requiriendo una DLL externa **solo** en los casos de la sección 4 («Casos que van a la DLL»).
* `vs_1_1` y las funciones `D3DXCompileShader*` no están cubiertas por el ensamblador propio (el generador no emite nada del segundo tipo; el primero solo ocurre en GPUs de 2002).
* `D3DXVec3TransformCoord`, `D3DXMatrixTranspose` y `D3DXFloat16To32Array` no se ejecutaron en la sesión medida: su igualdad con la DLL es de banco (millones de pares), no de partida. `Vec3TransformCoord` y `rcpps` dependen del hardware igual que la DLL: se comprobó en esta CPU, no en AMD.
* Las 3 ejecuciones largas fueron solo 130 s del arranque y la secuencia inicial (FastLoader), no una partida completa con mucha variedad de modelos/efectos; los 12 shaders corresponden a esa sesión (otros descriptores —iluminación puntual/foco, morphing, efectos de material— se cubren con el banco del generador, no con ejecuciones).
* Las capturas no son comparables píxel a píxel entre ejecuciones (sección 6).
* No se tocó `_fase8b_run`, `_fase8b_run_imagen` ni partidas reales; `gta_lock` solo lo tomaron los scripts de juego.
