<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CRT1A: qué falta por ejecutar dentro del juego (para el agente principal)

El agente de este lote no lanzó `gta_sa.exe` (denegado). Todo lo de `README.md` está validado **fuera del juego** (banco con el EXE mapeado). Lo que sigue necesita el juego.

## 1. Qué se instala por defecto y qué no

| Grupo | Archivo | Gancho | Estado |
|---|---|---|---|
| qsort (0x8247E0) | `source/game_sa/Crt/CrtQsort.cpp` | `CrtQsortInjectHooks()` | **instalado por defecto**; sus 4 usos conocidos (rpworld ×3, rpskin) llaman a 0x8247E0 por dirección |
| strlen/strcmp/strncmp/strcpy/strcat/strncpy/strncat/strstr | `CrtString.cpp` | `CrtStringInjectHooks()` | **instalado por defecto** |
| malloc/free/realloc/calloc/_nh_malloc/operator new | `CrtHeap.cpp` | `CrtHeapInjectHooks()` | **apagado**: solo con `GTASA_CRT_HEAP_HOOKS` |

`docs/hooks.csv` lleva las 9 filas nuevas (clase `ExeCrt`, `validated=0`). Los ganchos del montón no están en el CSV porque no se instalan por defecto. Regenerar los docs con el procedimiento habitual.

## 2. Prueba 1: qsort y cadenas (ya encendidos)

1. Compilar el proyecto con el patrón de `D:\Original\_build_INT1.sh` (candado, `CMAKE_BUILD_PARALLEL_LEVEL=1`, `_CL_=-MP1`) desde el worktree de este lote (`D:\Original\wv-14`, rama `crt1a`). CMake debe reconfigurar (carpeta nueva `source/game_sa/Crt/`, se recoge con el `GLOB_RECURSE`).
2. Ejecutar la prueba automática habitual de pares con las partidas de siempre (la misma que usa el resto de lotes: `reversing/run.py`) y comprobar que en la consola salen los ganchos nuevos y que el resultado del mundo (RenderWare: las mallas ordenadas por `RpWorld`/`RpSkin`) no cambia: el `qsort` propio es idéntico al del EXE (misma secuencia de comparaciones), así que cualquier diferencia visual sería un defecto del gancho, no del orden.
3. Con `--unhook-except` o equivalente, desactivar solo el grupo `Crt` y repetir: las dos ejecuciones deben dar exactamente el mismo resultado.

## 3. Prueba 2: montón (apagado por defecto)

1. Reconstruir con los ganchos del montón: `touch source/game_sa/Crt/CrtHeap.cpp` y compilar con `_CL_="-MP1 -DGTASA_CRT_HEAP_HOOKS=1"` (mismo patrón de candado).
2. Arrancar el juego **en ventana** (nunca a pantalla completa ni cerrar con taskkill): menú, cargar la partida, 10 minutos de juego libre, cambiar de zona (streaming) y salir por el menú. Comprobaciones: sin excepciones; la consola debe decir que se instalaron los ganchos de `ExeCrtHeap` (6); `HeapValidate(*(HANDLE*)0xC9C2F4, 0, nullptr)` debe seguir dando 1 al salir (se puede llamar desde la consola de depuración o con un contador que lo haga cada 60 s).
3. Condición previa: leer `*(int*)0xC9C2F8` en ejecución; **debe valer 1**. Si valiera 3, `CrtHeapInjectHooks()` no instala nada (comportamiento previsto) y el diseño de `DISENO.md` no aplica.
4. Si todo va bien: poner el gancho por defecto (quitar el `#ifdef`) y pasar a la fase (c) de `DISENO.md` (`_heap_init` propio).

## 4. Pendiente de pruebas con el juego dentro de este lote

- Que el `qsort` propio ordene igual en las cuatro llamadas reales (los bancos prueban los comparadores reales 0x7C8720, 0x7567A0 y 0x758240; el 0x759640 de `rpworld` desreferencia punteros a materiales y no se pudo aislar).
- Ganchos del montón (punto 3).
- Nada de x87: no se tocó (ver `README.md`, sección 4).
