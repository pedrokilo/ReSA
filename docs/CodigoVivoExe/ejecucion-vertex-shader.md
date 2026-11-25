<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Generador de vertex shaders `0x75F0B0` en C++ (lote VS1, 22/11/2026, rama `vertex-shader`)

Antes (`ejecucion-skin.md` sec. 4): el EXE independiente de fase 8 se detenía en el primer átomo con skin con `FALLO 0x80000003 en EIP=0x0075F0B0`: nodo de skin -> `_rxD3D9VertexShaderDefaultGetMaterialShaderCallBack` -> `_rpD3D9GetVertexShader` (`rpworld.cpp`) -> `0x75F0B0` por dirección (7.153 B).

## 1. Qué hace `0x75F0B0` (`_rpD3D9VertexShaderCreate`)

`void* __cdecl (const _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch)`.

1. Decodifica el descriptor de 4 bytes (byte 0: luces direccionales | puntuales << 4; byte 1: focales | nº de coordenadas de textura << 4; byte 2: bit 0 morphing, bits 1-3 nº de pesos, bit 4 prelit, bit 5 normales, bit 6 normalizar, bit 7 modular material; byte 3: bits 0-1 niebla, bits 2-7 efecto).
2. Asigna los desplazamientos de constantes en el despacho (8 bytes, `0xFF` = no usado): 5 + 2 por luz direccional; por grupo de luces puntuales `(n+3)/4 + 2n`, focales `(n+3)/4 + 4n`; después color de material, rango de niebla, coeficiente de morphing y matrices de hueso (offset del efecto = tabla `0xC94AF8[efecto]` con huesos: `0xC94C04 - cuenta`; sin huesos: a continuación).
3. Con el `sprintf` del motor (`RwEngineInstance + 0xF0`) escribe el texto de ensamblador en el buffer de 4 KB de `0xC93AF8`: cabecera `vs_1_1` / `vs_2_0` / `vs_2_x` según `0xC94C00` (0xA / 0x14 / 0x19; otro valor: devuelve NULL con el despacho ya escrito), declaraciones `dcl_*`, posición (morphing, huesos con `mova`/`mov a0.x`, `m4x3`/`m3x3`), niebla (lineal, exp, exp2), normalización, iluminación (direccionales en grupos de 4, puntuales, focales, `r11` acumulador, `oD0`), efecto de coordenadas de textura (1 env map, 2 y 3 proyectivos, 4-7 UV, 8/9 base tangente, 10 y 11 reflexión) o la lista `mov oTn.xy, v(n+5)`.
4. `D3DXAssembleShader(buf, len, 0, 0, 4, &code, 0)` (`0x7652AE`, enlazado; no se reconstruye aquí) y, si no falla, `RwD3D9CreateVertexShader(code->GetBufferPointer(), &shader)` (`0x7FAC60`); libera `code` (Release). Devuelve el shader (NULL si falló).

Reconstrucción: `rpworld.cpp`, `BuildVertexShaderSource` (texto + despacho, parametrizada por el `sprintf`, la versión y la tabla de constantes para poder probarla fuera del juego) y `_rpD3D9VertexShaderCreate` (hook `RH_ScopedGlobalInstall`, `docs/hooks.csv`: `RpWorld,_rpD3D9VertexShaderCreate,0x0075F0B0,1,0,0,S`). `_rpD3D9GetVertexShader` la llama directamente (antes por dirección). Las tablas de cadenas del `.rdata` (`0x8D63F8..0x8D7364`, `0x87522C`, `0x875240`) son literales C++ con el mismo texto; las constantes `0xC970A0/0xC970E0` no intervienen aquí (son de `0x7C8060`).

Detalles conservados del original: variables no inicializadas que no se usan (base de luces puntuales/focales sin luces), `%s` con puntero NULL (efectos con normales sin el bit de normales: sale `(null)` como en el CRT), índices de swizzle más allá de `xyzw` con más de 4 pesos (se leen los bytes siguientes de la tabla), y el buffer de 4 KB sin comprobación de longitud (el texto de un descriptor con 15+15+15 luces superaría 8 KB; el juego no los usa).

## 2. Verificación del texto: EXE original frente a C++

Arnés en `reversing/FnVertexShader/` (`LEEME.md`): el `0x75F0B0` **original** se ejecuta en un emulador x86 (Unicorn) con las secciones de `gta-sa.exe` mapeadas, el `sprintf` del motor implementado en Python y `D3DXAssembleShader` interceptado para capturar el texto; el generador C++ de `rpworld.cpp` se compila aparte (MSVC) y se alimenta con los mismos descriptores. Se compara texto y despacho **byte a byte** para cada descriptor y para las tres versiones (0xA, 0x14, 0x19).

* Cobertura estructurada: efectos 0-15 x 0-4 pesos x normales x luces (0, 1 direccional, mezcla 2/1/1, 5/6/0, 0/3/0, 0/0/2, 0/5/5, 4/4/4, 1/1/1) x prelit x modulate x niebla 0-3 x morphing (submuestra 12 %).
* Aleatorios totalmente libres (bytes cualesquiera, incluidos 5-7 pesos, efectos 12-63 y más de 4 luces).
* Resultado: **0 diferencias en 34.599 + 16.450 ejecuciones** (texto y despacho). El máximo de texto observado es 8.304 B (descriptores irreales); los descriptores del juego (ver sec. 3) son de unos 300-900 B.

## 3. Estado del EXE independiente (fase 8)

Procedimiento: ASI con el lote (`_build_VS.sh`, `EXIT=0`), `gta_sa_standalone.exe` enlazado con esos `.obj` (`_build_FASE8VS.sh`, `link exit=0`; hubo que copiar a mano `libjpeg6b/libpng1012/zlib113.lib` a `reversing/FnFase8/lib/` porque la copia automática de `enlazar.cmd` no las encontró) y partida nueva con `correr_f8b.ps1 -Et vs3 -Lim 100 -Cap 60 -Extra "--shims"` (registro en `reversing/FnFase8/ejecuciones/vs1-*`, `vs2-*`, `vs3-*`).

Resultado (**una sola iteración bastó**): `0x75F0B0` ya no se pide por dirección. En las tres ejecuciones (80, 90 y 100 s) **no hay ninguna llamada a código original** (`codigo-original-distinto=0`, `hits=0`, `excepciones=0`, `fase8_codigo_original.csv` vacío) y el juego sigue en `estado-juego=9` (jugando) con 1.850-2.100 fotogramas hasta que la guardia de tiempo lo cierra (el `FALLO ... 0xDEAD0002` final es la guardia, no un fallo del juego). Antes se detenía a los 9,7 s (129 fotogramas) con `0x80000003 en EIP=0x0075F0B0`.

**Se ven CJ y los peatones** (capturas `D:\Original\sim_out\vs_standalone_vs3_t35.png`, `..._t60.png`, `..._t85.png`): a los 35 s CJ de espaldas (camiseta blanca, vaqueros) y una mujer de morado con el maletín, bien deformados por el esqueleto (sin polígonos estirados); a los 85 s dos policías de uniforme y CJ en el suelo; a los 65 s (vs2) el coche patrulla LSPD. Esos modelos solo pueden dibujarse con el nodo de skin (`0x7C8060`) y un vertex shader creado por `_rpD3D9VertexShaderCreate` + `D3DXAssembleShader` + `CreateVertexShader`, así que el texto generado es válido para el ensamblador de D3DX9 enlazado.

Lo que sigue mal en las capturas **no es de este lote**: cielo blanco/rojo-amarillo con restos de «arrastre» (los fotogramas no se limpian bien; agentes de timecyc/CRT lo trabajan), líneas `timecyc` incorrectas en el registro, la escena de inicio sin avanzar (agente del jugador inmóvil). `0x80E270` (SubmitNoLight) sigue con shim (otro agente). No saltó un siguiente INT3, así que no hubo nada más que reconstruir ni describir en esta ronda.

## 4. Verificación con el ASI (humo) y compilación

* `_build_VS.sh` (candado `compile_lock`): log `D:\Original\_build_VS.log`, `EXIT=0`. `python contrib/audit_hook_catalog.py --strict`: OK.
* `docs/hooks.csv`: +1 fila (`RpWorld,_rpD3D9VertexShaderCreate,0x0075F0B0,1,0,0,S`, sin validar por banco: se validó con el arnés de la sec. 2 y con el humo).
* Humo en `wv-8
eversing\FnBIS1` con `AGUA_ASI` = el ASI construido: ver sec. 5.

## 5. Resultado del humo

ASI Debug con todos los ganchos activos (incluido `_rpD3D9VertexShaderCreate`), `AGUA_ASI` = el ASI construido, archivo de argumentos sin opciones (`" 
"`: un archivo totalmente vacío rompe `jugar.ps1`, que hace `.Trim()` sobre `$null`):

| Prueba | Resultado |
| --- | --- |
| `prueba_ropero.py` | `BIEN` (puerta y marcador; captura `D:\Original\sim_out\ropero_vs1.png`: CJ entero, de espaldas, con gorra verde, cadena y tatuaje, sin deformaciones) |
| `prueba_agua.py` | `BIEN` (brillo medio 48,0; `UnderWaterness = 1.0`) |
| `prueba_marcadores.py` | `BIEN` (12 marcadores en memoria, 12 cambian en 1 s) |

Nota: con el ASI el vertex shader solo se crea si el juego pide un descriptor no cacheado; el humo confirma que el camino de CJ (`0x7C8060`) sigue bien con la función ahora en C++.

## 6. Riesgos

* Buffer de 4 KB sin comprobación de longitud, como el original (los descriptores del juego quedan lejos del límite; los que lo superan son irreales).
* Los efectos que usan la normal sin el bit de normales imprimen `(null)` (igual que el EXE) y el ensamblado fallaría: no ocurre con los materiales del juego.
* La caché de shaders (`_rpD3D9GetVertexShader`) guarda NULL si el ensamblado falla, igual que el EXE.
* Más de 4 pesos de hueso (campo de 3 bits: 5-7) usa tablas del original más allá de `xyzw`; reproducido por bytes, sin uso real.
* El `D3DXAssembleShader` sigue siendo el enlazado dentro del EXE (`d3dx9math.cpp`/`D3dxShaderAssembler`), no una reconstrucción nueva.
