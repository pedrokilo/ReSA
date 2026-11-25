<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Espejo del ropero distinto en el standalone (24/11/2026, rama `fix-espejo`, base `e7bf0896ee`)

## Resumen

El espejo del ropero salía distinto en `gta_sa_standalone.exe` (panel claro con marcos anidados) y bien en el original y en la ASI (reflejo oscuro de la habitación). **No era un fallo de la lógica de `CMirrors` ni de la capa RenderWare/D3D9 en C++.** El standalone se ejecutaba en otra GPU:

| proceso | adaptador D3D9 (medido dentro del proceso con `GetAdapterIdentifier`) |
| --- | --- |
| `gta_sa.exe` original + ASI | NVIDIA GeForce RTX 3050 Laptop GPU (`nvldumd.dll`, 32.0.16.1062) |
| `gta_sa_standalone.exe` (antes) | Intel(R) Iris(R) Xe Graphics (`igd10iumd32.dll`, 31.0.101.5081) |

En el portátil (Optimus) el controlador manda a la GPU dedicada solo los .exe que conoce (el `gta_sa.exe` original sí); el standalone, al no tener perfil, iba a la integrada. El pase del espejo crea una combinación que D3D9 no define: render target de textura de cámara de 1024x512 **sin** multimuestreo y z-buffer de 1024x512 **con 4x MSAA** (el z-buffer sale con el MSAA del juego y la textura de cámara no puede tenerlo; el original hace exactamente lo mismo). El resultado depende del controlador: con la NVIDIA la prueba de profundidad no tiene efecto en ese pase (se pinta en orden de dibujo) y sale el reflejo oscuro del original; con la Intel sí funciona y sale la habitación "bien iluminada" pero distinta.

## Corrección

`source/app/platform/win/StandaloneExe.cpp` exporta `NvOptimusEnablement = 1` y `AmdPowerXpressRequestHighPerformance = 1` (datos exportados, sin código): el controlador elige la GPU dedicada, como con el original. Además `rwcore.cpp` deja en `logs/log.log` una línea con el adaptador con el que se creó el dispositivo (`Adaptador de vídeo D3D9: ...`) para poder comprobarlo en cada ejecución.

## Cómo se demostró (cada paso con medida, no con suposición)

Instrumentación temporal idéntica en el standalone y en una ASI enlazada con los mismos `.obj` (parche completo guardado en `reversing/FnComparar/evidencia/espejo_diag_instrumentacion.patch`; no está en el código final). Misma partida (ranura 2), mismo punto, `reversing/FnComparar/ejecutar_comparar.ps1 ... --fase interior`.

1. `CMirrors` calcula lo mismo: tipo suelo (`tipo=2`), V=1013.75, normal (0,0,1), matriz espejada igual al dígito (P=(2495.900,-1710.911,1011.635)); búfer 1024x512 tipo 5 en los dos; `NeedToCopyFromBackBuffer` (0xC97C34) = 0 (se renderiza a un render target real, sin copia del backbuffer).
2. La lista de entidades del pase del espejo y del principal es idéntica (737 líneas, `diff` vacío) y la secuencia de átomos (textura, triángulos, vértices, materiales) también (17 fotogramas muestreados, todos iguales).
3. Estados D3D en cada dibujo (cull, z, zwrite, zfunc, alpha, blend, luces y material, vista, proyección, viewport, render target, z-buffer, clip, sampler) idénticos en los fotogramas del ropero; solo difieren en el primer fotograma tras cargar y por animación (posición de CJ).
4. Volcado del render target tras cada átomo (`GetRenderTargetData`): la primera diferencia aparece en el átomo de la cáscara de la habitación (321 triángulos): en la ASI sale encima de lo ya dibujado (un puf del fondo queda tapado aunque está más cerca). Con `ESPEJO_NOZ` (forzar ZTEST/ZWRITE a falso solo en el standalone, solo diagnóstico) el átomo sale igual que en la ASI: diferencia absoluta media 0,025 de 255 (sin forzarlo, 43 frente a 38 de media de color).
5. Estado del dispositivo en el pase del espejo, igual en ambos: RT 1024x512 `ms=0`, z-buffer 1024x512 `ms=4`, `Clear` con `hr=0` (flags 0x7), parámetros de presentación y flags de creación iguales (`beh=0x50`). Lo único distinto: el adaptador (tabla de arriba) y, por consiguiente, `D3DCAPS9` (`RasterCaps` 0xF732191 frente a 0x7732191, `DevCaps` 0x1BAFF0 frente a 0x1BBEF0).
6. Con la exportación, el log del standalone dice `NVIDIA GeForce RTX 3050 Laptop GPU` y el espejo coincide con el original.

## Resultado (captura completa de ventana, DPI-aware, 800x600, método BitBlt, tamaño validado)

Diferencia absoluta media por píxel respecto al original en el ropero (`reversing/FnComparar/comparar.py`, `png_util.diferencia`):

| | ventana completa | recorte del espejo |
| --- | --- | --- |
| standalone antes | 4,29 | 35,75 |
| standalone con la corrección (2 ejecuciones) | 1,92 / 1,83 | 2,03 / 1,96 |

El resto es animación de CJ y reloj del juego (12:17/12:18). Imágenes: `evidencia/ropero_espejo_original_std_antes_std_fix.png` (ventana completa: original, standalone antes, standalone corregido) y `evidencia/ropero_espejo_recorte_original_antes_fix.png` (recorte ampliado, mismo orden). Volcados del átomo de la cáscara: `evidencia/espejo_atom2_ASI_orig_GPU.png`, `espejo_atom2_std_Intel.png`, `espejo_atom2_std_sin_z.png`. Datos del adaptador y presentación: `evidencia/espejo_gpu_std_vs_asi.txt`.

## Lo que no se verificó / límites

* **No se demostró por qué** la NVIDIA ignora la prueba de profundidad con RT sin MSAA y z-buffer 4x MSAA y la Intel no: es comportamiento del controlador ante una combinación inválida (el depurador de D3D9 la rechazaría). Lo demostrado es que con estados, geometría y datos idénticos el resultado depende del adaptador, y que sin prueba de profundidad el standalone reproduce la ASI.
* El reflejo "oscuro" del original es, por tanto, un efecto del controlador NVIDIA ante ese par RT/z-buffer, no un comportamiento definido del juego. En una máquina sin GPU NVIDIA (o con el MSAA en 0) el original también se vería como el standalone sobre Intel. No se probó con MSAA desactivado ni con otra GPU.
* La exportación depende de que el controlador lea la tabla de exportaciones del .exe (Optimus/PowerXpress); no se probó en AMD ni en un equipo sin dos GPU (allí no tiene efecto).
* Solo se midió el ropero (partida ranura 2); la casa de CJ ya coincidía antes. Otras escenas con espejo (suelo reflectante, estadio 8-Track) no se probaron.
