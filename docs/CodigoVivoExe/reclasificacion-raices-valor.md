<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reclasificación de las raíces por valor (huecos «muertos» que el EXE propio ejecuta)

Rama `vivo-raices` (desde `origin/main` 0366e8fabf, 21/11/2026). Análisis estático sobre el EXE canónico, el código fuente y los CSV; no se compiló el proyecto (ver §8). Generado por `contrib/huecos_p6.py` + `contrib/raices_valor_p6.py` (regenerable, §9). Base de la medida: [fase8-experimento.md](fase8-experimento.md) (rama `fase8-exp`).

## 1. Qué estaba mal

El experimento de la fase 8 construyó un EXE propio y vio que direcciones clasificadas como «muerto» (`huecos-p6.csv`: `citada-en-cpp`/`muerto`) o sin lista **se ejecutan**: `0x4CD820`, `0x750430`, `0x750460`, `0x750480`, `0x750AC0`, `0x7F98D0`, `0x7F99B0`. «Muerto» se decidía por falta de llamadores directos (`call`/`jmp`/inmediato en una función viva) y no vale para una dirección que el C++ **escribe como valor** (tabla estándar de rasters de D3D9, callbacks de `RwCamera`, `mov [reg+x], imm32` de los nodos de RenderWare): la llama el motor, por un puntero que el análisis no sigue. Además `citada-en-cpp` agrupaba cualquier cita, incluidos los comentarios, y la trataba igual: ni viva ni muerta, «documentar».

## 2. Regla nueva

Una dirección citada por el C++ **como VALOR** (asignación, inicializador, argumento, `reinterpret_cast`, llamada por dirección), no como comentario, es una **raíz viva** hasta demostrar lo contrario (`escanear_citas` de `contrib/raices_valor_p6.py`):

* un autómata separa en `source/` el **código**, los **comentarios** y las **cadenas**; cada literal `0x4xxxxx`–`0x8xxxxx` (sin carácter de identificador delante: `f0x57C330` es un nombre) se clasifica en `enganche` (línea `RH_Scoped*`), `llamada-directa` (`plugin::Call*<0x…>`), `valor` (todo lo demás en código), `libjpeg-cb` (`LIBJPEG_CB` o `JPegCompressPlugin`), `protector` (tablas de `ExeProtectionStub.cpp`), `isla-d3dx9` (los cuatro `D3dx*.cpp` que `ejecucion-v48.md` 2.4 ya trata como isla) y `comentario`;
* se cruza con el **código compilado**: [`fase8-experimento/dependencias_codigo.csv`](fase8-experimento/dependencias_codigo.csv) (direcciones del `.text` que los `.obj` del proyecto llevan como inmediato; confianza A/B/C). A/B cuentan como evidencia de valor; una C solo si hay literal de código en el C++ **y** el destino es un inicio de rutina plausible (alineado a 16 o tras `int3`/`nop`/`ret`) **y** la instrucción es `push`/`mov` (así `0x4CAE40`, entrada real de la tabla de rasters, sale de la clase «ruido» y `0x475441`, un identificador de temporizador, no);
* destino: **viva sin hook** (cola de reconstrucción); **viva enlazada** (libjpeg, libpng+zlib, D3DX9, CRT de MSVC: decisiones vigentes del brief); **viva de QuickTime** (se retira); **viva del protector** (tablas de `ExeProtectionStub.cpp`, se retiran con el archivo, V1); **cerrada por comentario** (solo la cita un comentario y ninguna otra entrada es viva); **cerrada por constante** (inmediato numérico); **cerrada isla D3DX9**. Nada se cierra por «no tiene llamadores».

La clase `citada-en-cpp` **desaparece**. Sigue sin verse una cita sin literal hexadecimal y no se distingue código no compilado (`#if 0`); en ambos casos el error es hacia «viva» o «sin ver», nunca hacia «muerta» con certeza (§9).

## 3. Cifras antes y después: piezas de huecos (914 piezas, 118.544 B)

| Destino | Antes (piezas / B) | Después (piezas / B) |
| --- | ---: | ---: |
| reconstruida | 314 / 59.034 | 314 / 59.034 |
| viva-sin-hook | 0 / 0 | 7 / 5.427 |
| viva-enlazada | 0 / 0 | 55 / 8.261 |
| viva-quicktime | 0 / 0 | 1 / 108 |
| viva-protector | 0 / 0 | 8 / 2.531 |
| crt-enlazar | 5 / 729 | 5 / 729 |
| d3dx9-enlazar | 2 / 1.188 | 2 / 1.188 |
| pendiente-dato | 3 / 1.910 | 0 / 0 |
| citada-en-cpp | 151 / 16.701 | 0 / 0 |
| cerrada-comentario | 0 / 0 | 83 / 2.284 |
| muerta-por-reemplazo | 94 / 27.381 | 94 / 27.381 |
| destructor-atexit | 134 / 1.657 | 134 / 1.657 |
| sin-referencia-real | 211 / 9.944 | 211 / 9.944 |
| **Total** | 914 / 118.544 | 914 / 118.544 |

Las **131 piezas `citada-en-cpp`/`muerto` (15.654 B)** del encargo se reparten así:

| Nuevo destino | Piezas | Bytes |
| --- | ---: | ---: |
| cerrada-comentario | 64 | 1.581 |
| viva-enlazada | 54 | 7.917 |
| viva-protector | 8 | 2.531 |
| viva-sin-hook | 4 | 3.517 |
| viva-quicktime | 1 | 108 |

Las 94 piezas `muerta-por-reemplazo` no cambian (muerta-por-reemplazo 94): ninguna tiene cita de código ni inmediato compilado, solo llamadores que son funciones reemplazadas (su cuerpo ya no corre).

Huecos cerrados (`cierre-p6.csv`): antes 269 de 270 → después **266 de 270**. Un hueco con una pieza «viva sin hook» no está cerrado: su pieza está en la cola.

## 4. Las 379 direcciones de código citadas por el C++ sin gancho (fase 8)

Antes: 119 de confianza A, 147 B y 113 «ruido» C. Después, cada una cae en una clase (la columna «Direcciones» suma 379); «Objetos» agrupa las que comparten pieza de hueco:

| Clase | Direcciones | Objetos | Bytes |
| --- | ---: | ---: | ---: |
| viva sin hook | 12 | 10 | 15.747 |
| viva, enlazada (libjpeg) | 58 | 58 | 11.371 |
| viva, enlazada (libpng/zlib) | 104 | 104 | 9.670 |
| viva, enlazada (CRT) | 21 | 21 | 1.813 |
| viva, QuickTime (se retira) | 1 | 1 | 108 |
| viva, del protector | 0 | 8 | 2.531 |
| cerrada (isla D3DX9) | 33 | 33 | 0 |
| cerrada (comentario) | 0 | 83 | 2.284 |
| cerrada (constante) | 107 | 107 | 0 |
| enganchada desde la medida | 43 | 43 | 73 |
| **Total** | 379 | 468 | 43.597 |

«Enganchada desde la medida» = ya tiene fila en `docs/hooks.csv` (la medida es anterior a integrar `vivo-crtf8`). Los «ruido C» que eran reales:  (entradas de la tabla estándar de rasters); el resto son constantes (`imul`, `add`, `cmp`, desplazamientos de tabla, un identificador de temporizador `0x475441`, máscaras `0x7FFFFF`/`0x800000`, colores).

## 5. Cola de reconstrucción: «viva sin hook»

**10 objetos, 15.747 B.** Completa en [`cola-raices-valor-p6.csv`](cola-raices-valor-p6.csv) (todas las clases en [`raices-valor-p6.csv`](raices-valor-p6.csv)). Orden: primero lo **medido** en la fase 8 (se ejecuta en los dos primeros segundos), luego lo que comparte rutina de registro con una medida (mismo arranque), luego el resto; dentro de cada nivel por tamaño descendente. Los tamaños marcados `*` son una estimación (barrido lineal hasta el primer `ret`/`jmp` sin saltos pendientes; no sigue tablas de saltos). `RECONSTRUIR` = escribir C++ y engancharlo.

| # | Dirección | B | Ejecución | Quién la cita | Tabla / estructura destino | Decisión |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `0x0075F0B0`* | 7.153 | sin medir | rpworld.cpp:9691 | rpworld.cpp:9691 `void* const shader = reinterpret_cast<CreateVertexShader>(0x75F0B0)(desc, dispatch);` | RECONSTRUIR |
| 2 | `0x0075A960`* | 2.933 | sin medir | rpworld_s30a31a.cpp:1502; rpworld_s30a31a.cpp:562 | rpworld_s30a31a.cpp:562 `return reinterpret_cast<RpMeshHeader*(__cdecl*)(RpBuildMesh*, void*, RwInt32)>(0x75A960)(b` | RECONSTRUIR |
| 3 | `0x007624A0` | 1.205 | sin medir | rpworld_s30a31a.cpp:1844; rpworld_s30a31a.cpp:1848 | rpworld_s30a31a.cpp:1844 `if (!reinterpret_cast<SectorWriteFn>(0x7624A0)(world->rootSector, stream, mutableWorld, fl` | RECONSTRUIR |
| 4 | `0x00762F30` | 980 | sin medir | rpworld_s30a31a.cpp:1961 | rpworld_s30a31a.cpp:1961 `world->rootSector = reinterpret_cast<SectorReadFn>(0x762F30)(stream, &memoryCursor, world,` | RECONSTRUIR |
| 5 | `0x007F40F0` | 966 | sin medir | rwcore.cpp:13967 | rwcore.cpp:13967 `S20ModuleField<void*>(0xC97B4C, 0x14) = reinterpret_cast<void*>(0x7F40F0);` | RECONSTRUIR |
| 6 | `0x0074A770` | 669 | sin medir | rpworld_s30a31a.cpp:1054; rpworld_s30a31a.cpp:1219; rpworld_s30a31a.cp | rpworld_s30a31a.cpp:913 `inline RwBool BuildGeometryList(RpGeometryList* list, RpClump* clump) { return reinterpret` | RECONSTRUIR |
| 7 | `0x00763310` | 663 | sin medir | rpworld_s30a31a.cpp:1970 | rpworld_s30a31a.cpp:1970 `world->rootSector = reinterpret_cast<SectorReadFn>(0x763310)(stream, &memoryCursor, world,` | RECONSTRUIR |
| 8 | `0x007F5140` | 652 | sin medir | rwcore.cpp:13968 | rwcore.cpp:13968 `S20ModuleField<void*>(0xC97B4C, 0x2C) = reinterpret_cast<void*>(0x7F5140);` | RECONSTRUIR |
| 9 | `0x007F53D0` | 292 | sin medir | rwcore.cpp:13966 | rwcore.cpp:13966 `S20ModuleField<void*>(0xC97B4C, 0x18) = reinterpret_cast<void*>(0x7F53D0);` | RECONSTRUIR |
| 10 | `0x007F2930`* | 234 | sin medir | rwplcore.cpp:1466 | rwplcore.cpp:1466 `reinterpret_cast<void(__cdecl*)(const RwMatrix*, RwV3d*)>(0x7F2930)(matrix, unitAxis);` | RECONSTRUIR |

Las 7 direcciones medidas (`0x4CD820`, `0x750430`, `0x750460`, `0x750480`, `0x750AC0`, `0x7F98D0`, `0x7F99B0`) están dentro de objetos de la cola (0 de 7): `0x750480` va dentro de la pieza `0x750460` (682 B) y `0x750AC0` es la segunda entrada de la pieza de `LightWorldPluginConstruct`. «Ejecución» solo es una medida en las siete; el resto es una prioridad por vecindad (misma rutina de registro), no una medida.

## 6. Efecto sobre las cifras de lo que falta

* **Cierre por llamadas del código original (fase 8)**: 691 funciones / 309.142 B, 266 raíces. De los 10 objetos de la cola, **10 ya eran raíces de ese cierre** (14.769 B) y **0 no estaban** (): el cierre medido era correcto; lo que estaba mal era la **lista de pendientes** (P6 los daba por «muertos» o «citados»). Con la cola, el cierre sube a 691 funciones / 309.142 B.
* **P6**: de «269 de 270 huecos cerrados» a **4 huecos abiertos** (con piezas «viva sin hook»). Lo enlazado (libjpeg 8.261 B en piezas) y lo del protector (2.531 B) no suma a lo que falta reconstruir.
* **Por reconstruir con hook** (toda la cola, no solo P6): 10 objetos, 15.747 B; **0** se ejecutan al arrancar o comparten rutina de registro con una medida (0 B); 0 tienen ≤ 30 B.
* **Cierres nuevos con motivo**: 83 piezas (2.284 B) solo citadas en comentarios (antes `citada-en-cpp`), 107 direcciones del escaneo de `.obj` que son constantes y 33 citas de la isla D3DX9; la clase `citada-en-cpp` ya no existe.
* **Plan**: nueva parte P7 en `plan.csv`/`plan-lotes.md` (lotes R1–R3 = la cola por temas; `contrib/codigo_vivo_plan.py`), y H1–H6 con su estado real (antes `pendiente` sin recalcular).

## 7. Descartes P1/V1–V8 cerrados que dependían de una pieza viva

(comprobación no ejecutada: use `--cierres`)

## 8. Piezas pequeñas y claras (≤ 30 B) de la cola

No se reconstruyó ninguna en esta rama: exige compilar el worktree (build completo, un solo hilo y bajo el candado compartido) y el humo de ropero + agua, que espera a que no haya otro `gta_sa.exe`. Quedan **listas para el lote R3/R1** (cuerpo ya desensamblado; ninguna toca memoria que no sea la indicada):

| Dirección | B | Equivalente C++ (cdecl) | Dónde se registra |
| --- | ---: | --- | --- |

## 9. Cómo regenerar y límites

```
python -X utf8 contrib/huecos_p6.py --exe <gta_sa.exe>      # huecos-p6.csv (regla de valor), cierre-p6.csv, H1..H6 en plan.csv y plan-lotes.md
python -X utf8 contrib/raices_valor_p6.py --exe <gta_sa.exe> --cierres --aplicar --cache <carpeta>   # cola, raices-valor-p6.csv, este informe y la comprobacion de cierres
python -X utf8 contrib/codigo_vivo_cierre_v48.py --alcance --aplicar --cache hechos.pkl --cache-aristas aristas.pkl   # V4-V8 con la cola como raices
python -X utf8 contrib/codigo_vivo_plan.py                    # plan.csv / plan-lotes.md (lotes R1..R3); ejecutar huecos_p6.py despues para el estado de H1..H6
```

Límites: (1) `dependencias_codigo.csv` es el escaneo de `.obj` de la fase 8 (solo se regenera con las herramientas de esa rama); (2) una cita sin literal hexadecimal (base + desplazamiento, decimal) no se ve; (3) un literal en código no compilado (`#if 0`) cuenta como valor; (4) «cuánto se ejecuta» solo está medido para 7 direcciones, el resto es por rutina de registro compartida (el censo `--censo` de `reversing/FnFase8` exige enlazar y ejecutar el EXE propio y no se repitió aquí); (5) el tamaño de un tramo de hueco es una estimación; (6) los módulos enlazados salen de `docs/RenderWareExe/funciones.csv` (D3DX9 0x7652AE–0x7C4220, libpng+zlib 0x7CF600–0x7EB260), QuickTime 0x818430–0x81DAB0 y el CRT hasta 0x8568AB.

