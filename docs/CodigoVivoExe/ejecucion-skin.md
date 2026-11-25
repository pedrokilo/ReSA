<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución SKIN1: nodo de skin D3D9 de RenderWare en C++ (`nodeD3D9SkinAtomicAllInOne.csl`)

Rama `skin-nodo` (desde `origin/main` `1ec835719c`). Objetivo: quitar el shim del nodo de skin del EXE independiente (fase 8, ver `fase8-experimento.md` §8.2 fila 7 y §8.8 punto 1) reconstruyendo en C++ el `nodeBody` `0x7C7B90` y todo lo que cuelga de él. Sin `__asm`, sin `naked`; todas las funciones llevan `RH_ScopedGlobalInstall` y están en `source/game_sa/RenderWare/rw/rpskin.cpp` (sección «Lote SKIN1» al final y `RpSkinInjectHooksNode`).

## 1. Qué había

La definición del nodo `0x8DED08` (`RxNodeDefinition`, datos del EXE) apunta su `nodeBody` a `0x7C7B90`. Ghidra no separaba esa función ni las que usa (huecos), por eso el listado de RenderWare las daba por inexistentes:

| Dirección | Tamaño | Qué es | Nombre (inferido) |
| --- | ---: | --- | --- |
| `0x7C7B90` | 0x4CE | nodeBody: instancia, mezcla por CPU si hace falta y dibuja | `_rxD3D9SkinAtomicAllInOneNode` |
| `0x7C78A0` | 0x2E7 | constantes de matrices de hueso del vertex shader (12 floats por hueso) | `_rpSkinD3D9UpdateBoneConstants` |
| `0x7C8060` | 0x549 | dibujo del skin por vertex shader | `_rpSkinD3D9VertexShaderRender` |
| `0x7C85B0` | 0x13D | dibujo del skin sin vertex shader (callbacks del nodo) | `_rpSkinD3D9DefaultRender` |
| `0x7C8D60` | 0x359 | instanciado del atomo con skin (versión indexada de `0x756960`) | `_rpSkinD3D9InstanceAtomic` |
| `0x7C9DA0` | 0x494 | mezcla x87 de hasta 4 huesos | `_rpSkinX87MatrixBlend4` |
| `0x7CA410` | 0x2A0 | mezcla x87 de 2 huesos | `_rpSkinX87MatrixBlend2` |
| `0x7CA330` | 0xD1 | transformación x87 por una matriz | `_rpSkinX87TransformPoints` |
| `0x7CA240` | 0xE9 | transformación x87 por matriz indexada | `_rpSkinX87TransformPointsIndexed` |
| `0x7CA6C0` | 0x13A | producto de matrices guardado transpuesto 3x4 | `_rpSkinX87MatrixMultiplyTransposed` |
| `0x7CA850` | 0x1D4 | matrices de hueso para la x87 (formato transpuesto) | `_rpSkinX87GetBoneMatrices` |
| `0x7CAA30` | 0x98 | matrices de hueso para SSE (cache + columna w a 0) | `_rpSkinSSEGetBoneMatrices` |
| `0x7CB390` | 0x1B0 | matrices de hueso en formato `RwMatrix` | `_rpSkinGetBoneMatrices` |

Aclaraciones sobre el encargo:

* **`0x807DE0` ya estaba hecha**: es `RwResourcesFreeResEntry` (`rwplcore.cpp`, con gancho). El nodo la llama por su nombre C++.
* **`0x7C8D60` no es un mezclador de huesos**: es el instanciado del atomo (entrada de recursos + cabecera D3D9 + buffer de indices). Los mezcladores SSE (`0x7CAC60`, `0x7CAF50`, `0x7CAAD0`, `0x7CAB80`) y el genérico `0x7CB6A0` ya existían; faltaban sus gemelos x87 (`0x7C9DA0`, `0x7CA410`, `0x7CA330`, `0x7CA240`).
* Muertas en el EXE (sin llamadores ni referencias de datos, comprobado escaneando `.text` y las secciones): `0x7CA800` (copia transpuesta 3x4) y `0x7CB540` (variante de `0x7CB390` sin inversa). No se reconstruyen ni llevan gancho.

## 2. Flujo del nodo (`0x7C7B90`)

1. Átomo = primer dato de los parámetros; sin vértices o sin mallas: `TRUE`.
2. Entrada de recursos: con un solo morph target la de la geometría, si no la del átomo; número de serie distinto: `RwResourcesFreeResEntry`. Sin entrada se instancia con `0x7C8D60` (fallo: `FALSE`) y `lockedSinceLastInst = 0`; con entrada pasa al principio de la lista de usadas.
3. **Skin por CPU** (skin existe, `skin->field20 == 0` y hay jerarquía): bloquea el buffer de vértices (`Lock` con `0x800` si `_rpSkinD3D9AtomicLimitCheckOrOff`; si no, `Lock` con `0x2800` o buffer dinámico `RwD3D9DynamicVertexBufferLock`), calcula las matrices (`0x7CAA30` si `0xC980A4 != 0`, `0x7CA850` si no) y mezcla según `maxNumWeightsPerVertex` (>2, 2, un hueso usado, indexado). Después `Unlock` y `lockedSinceLastInst &= 0xF9`.
4. Con datos bloqueados reinstancia (`0x7C90D0`) y limpia la bandera 1 de la interpolación.
5. Con vertex shader (`field20 != 0` y jerarquía): `0x7C8060`; si no, `0x7C85B0`. Devuelve `TRUE`.

`0x7C8060` (el camino de CJ y los peatones en un PC normal: `0xC978E4 = 1`, `field20 != 0`) en orden: callback de inicio (descriptor con `numWeights`), estado `0x88 = 1`, matriz mundo y transformada compuesta a `0xC970A0`, iluminación a `0xC970E0` y subida de constantes, matrices de hueso (`0x7C78A0`), índices/flujos/declaración si cambian; por malla: shader del material, recorte del descriptor hasta que caben las constantes, colores de luces/material/niebla si cambian, subida de huesos (por rachas RLE con datos divididos; si no todos o solo los usados) y callback de dibujo.

Detalles que no se ven en el SDK y se han conservado: la caché de `0x7C78A0` guarda `~sello` en `0xC978D8` (las de `0x7CA850`/`0x7CAA30` guardan el sello tal cual, para no validarse mutuamente); el registro inicial de bone-matrices fuera del límite se empaqueta por orden de hueso usado (registro = `offset + 3 * posición`), dentro del límite va por índice de hueso (`offset + 3 * hueso`); tras recortar el descriptor se invalida a propósito la caché de colores (`ambiente = 1.0 - ambiente del material`).

## 3. x87 (24 bits)

El x87 del juego corre a 24 bits: cada `fadd`/`fmul` redondea a `float`. Los bloques C++ usan `float` por operación (SSE2, `float_control(precise)`; el bloque `#pragma float_control(precise, on, push)` evita que `/fp:fast` reasocie) con **la misma asociación de sumas que cada subrutina** (la suma y el producto son conmutativos bit a bit, la asociación no). Cada subrutina tiene su forma:

* `0x7C9DA0`, `0x7CA330` y la rama de un hueso de `0x7CA410`: `((m0·x + z·m2) + y·m1) + m3`, etc. (`SkinPosA`, `SkinNrmA`).
* `0x7CA240`: orden z, y, x (`SkinPosB`, `SkinNrmB`).
* `0x7CA410` en la mezcla: hueso 0 con `SkinPosA`/`SkinNrmC`, hueso 1 con `SkinPosD`/`SkinNrmD`.
* La mezcla acumula `término·peso + acumulado`. Condiciones sobre los pesos como entero con signo (`>= 0x3F800000`, `> 0`).

Las expresiones se verificaron contra una evaluación simbólica de la pila x87 del desensamblado (`contrib`-estilo, script de trabajo no incluido); las dos cargas de `float` de `0x7C85B0` (cuadrado de la longitud de las filas, comparado como entero con signo con `0x3F8CCCCD`/`0x3F666666`) también.

## 4. Verificación (22/11/2026)

**Compilación**: `_build_SKIN.sh` (patrón `_build_<LOTE>.sh`, candado `compile_lock`), log con `EXIT=0`, ASI `gta_reversed.asi` (Debug). `python contrib/audit_hook_catalog.py --strict`: OK (0 ausentes). `docs/hooks.csv` +13 filas `RpSkin` (tipo `S`, sin validar por banco).

**Humo con el ASI** (todos los ganchos activos, incluidos los 13 nuevos; `FnBIS1` de `wv-8`, `AGUA_ASI` = el ASI construido, sin argumentos de apagado):

| Prueba | Resultado |
| --- | --- |
| `prueba_ropero.py` | `BIEN` (puerta 2877 con modelo RW y marcador 3D) |
| `prueba_agua.py` | `BIEN` (brillo medio 42,3; `UnderWaterness = 1.0`) |
| `prueba_marcadores.py` | `BIEN` (12 marcadores en memoria, 12 cambian en 1 s) |

Como un veredicto por memoria puede pasar con el render roto, se revisó la captura `D:\Original\sim_outopero_skin1.png`: **CJ sale entero y bien deformado por el esqueleto** (gorra verde, cadena, tatuaje de la espalda, hombros), sin polígonos estirados. Esa escena pasa por `0x7C8060` (vertex shader), `0x7C78A0` y `0x7C8D60` reconstruidos. Las ramas de mezcla por CPU (SSE y x87) solo se ejecutan si el atomo supera el límite de huesos del shader o no hay vertex shader: **no se ejercitaron** en el humo (riesgo conocido, ver §6).

**Standalone (fase 8)**: se quitó el shim de `0x7C7B90` de `StandaloneExe.cpp` (queda solo el de `0x80E270`, de otro agente). Se enlazó `gta_sa_standalone.exe` con los `.obj` de este árbol (`enlazar.cmd`, `link exit=0`, `cl.rsp` regenerado con `gen_cl_rsp.py` para `wv-skin`) y se ejecutó la partida nueva (`correr_f8b.ps1 -Et skin1 -Lim 80 -Cap 70 -Extra "--shims"`, registro en `reversing/FnFase8/ejecuciones/skin1-20261122-094636/`).

Resultado: **el juego ya no llega a los 80 s**. A los 9,7 s (129 fotogramas) el nodo de skin, ahora C++, llega a `_rxD3D9VertexShaderDefaultGetMaterialShaderCallBack` -> `_rpD3D9GetVertexShader` (`rpworld.cpp:9691`) y ésta llama por dirección a **`0x75F0B0`** (7.153 bytes, «crea un vertex shader»: genera el texto de ensamblador del shader con `sprintf` del motor y lo ensambla; ya listada como «grande: lote propio» en `ejecucion-camara-d3d9.md` y `reclasificacion-raices-valor.md`). Ejecución: `FALLO (llamada a codigo original sin reemplazo) 0x80000003 en EIP=0x0075F0B0`, `CODIGO ORIGINAL #1: 0x0075F0B0 llamado desde 0x01D81957 [propio]`.

Es decir: con el shim el nodo devolvía sin dibujar y por eso nunca se pedía el shader; sin el shim el primer atomo con skin necesita el generador de vertex shaders. **En el standalone todavía no se ve a CJ ni a los peatones**: el siguiente paso es reconstruir `0x75F0B0` (con sus tablas de cadenas en `.rdata` `0x8D7220..0x8D7364` y el ensamblado posterior) y volver a ejecutar.

## 5. Cómo reproducir

```
bash /d/Original/_build_SKIN.sh                       # ASI (compile_lock); EXIT=0 en /d/Original/_build_SKIN.log
python contrib/audit_hook_catalog.py --strict
cd D:\Original\wv-8eversing\FnBIS1                 # AGUA_ASI=<ASI construido>; args vacios
python prueba_ropero.py <vacio.args> skin1 ; python prueba_agua.py <vacio.args> skin1a ; python prueba_marcadores.py <vacio.args> skin1m
python -I reversing/FnFase8/gen_cl_rsp.py <vcxproj> <raiz> <raiz> reversing/FnFase8/cl.rsp   # (no se commitea) y
bash /d/Original/_build_FASE8SKIN.sh                  # enlaza el standalone con los .obj de este arbol
pwsh -File reversing/FnFase8/correr_f8b.ps1 -Et skin1 -Lim 80 -Cap 70 -Extra "--shims"
```

## 6. Riesgos

* Las 13 funciones están **sin validar por banco** (por petición: solo humo). Las ramas de CPU (SSE `0x7CAC60`/`0x7CAF50`/`0x7CAAD0`/`0x7CAB80` ya existían; x87 `0x7C9DA0`, `0x7CA410`, `0x7CA330`, `0x7CA240`, `0x7CA850`, `0x7CA6C0`) no se han ejecutado. Un banco por pares (EXE vs C++) de los mezcladores x87 con pesos reales sería la validación natural.
* Detalles de comportamiento del original conservados a propósito (no son errores nuestros): bucle sin guardia con `count == 0` en las rachas RLE, lectura de variables sin inicializar en las normales del mezclador de 4 huesos si hay puntero de salida sin entrada (aquí inicializadas a 0), caché de matrices con sello negado en `0x7C78A0`.
* `0xC970A0`/`0xC970E0` (constantes del shader) se siguen usando por dirección fija (BSS del EXE), como el resto del módulo.
