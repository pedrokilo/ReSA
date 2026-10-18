<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CClouds — RenderBottomFromHeight

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Validación del 2026-09-22: **1.170 comparaciones aprobadas dentro de GTA, con igualdad numérica exacta**. La clase pasa de 20/21 a 21/21 funciones registradas. Este banco cubre la última función, `RenderBottomFromHeight` (`0x7154B0`); no valida exhaustivamente las otras veinte.

Se reconstruyó la función vacía a partir del ejecutable original verificado: geometría de nubes, desplazamiento por viento, distribución alrededor de la cámara, transparencia por altura/distancia, proyección, sprites y estados de RenderWare. Se conserva la peculiaridad original de aumentar el color únicamente en el primer vértice de cada cuadrilátero. Los sprites usan el color sin aumentar. La aritmética utiliza precisión simple, acorde con el control x87 de 24 bits observado durante el renderizado.

## Ejecutar

Desde la raíz del repositorio:

```powershell
.\PROBAR.cmd --case clouds
python reversing/run.py --case clouds
```

El segundo comando termina sin pausa ni supervisión. Se utiliza `runtime_runner.py`: instala temporalmente la sonda archivada, abre la copia de pruebas, espera al jugador, analiza los logs, termina su propio proceso y restaura ASI/PDB. Comprueba los hashes de las partidas. `PROBAR.cmd --all` incluye ahora este banco; no se creó otro CMD.

Sonda local: `_baseline/gta_reversed-clouds-probe.asi` y `.pdb` en la copia de pruebas. `build_probe.py` permite reconstruirla y restaura las fuentes instrumentadas mediante `finally`. Una recompilación requiere volver a validar su hash y actualizar `PROBE_HASH`; no se acepta automáticamente un binario diferente. El build produce `artifacts/build-manifest.json` con hashes de fuentes e instrumentación. La compilación normal se genera después, con las fuentes restauradas.

## Comparación dentro del motor

El banco alterna el hook de la dirección original, llama primero al original y luego al candidato, y comprueba los contadores de entrada: cero llamadas al candidato durante la ejecución original y una durante la ejecución candidata. Entre ambas restaura y verifica las entradas globales y los buffers; la API usa estado global y no permite dos instancias independientes simultáneas.

Matriz de 13 alturas × 2 posiciones XY × 5 transparencias × 3 configuraciones de viento/tiempo × 3 colores = 1.170 pares. Incluye alturas de −100 a 800, límites alrededor de −90 y 200, alpha 0/0,9/1/127,75/255, desplazamientos positivos y negativos y saturación RGB.

Se registran y comparan coordenadas XYZ/UV, colores, índices, banderas de transformación, entradas de proyección, argumentos de sprites, vértices 2D enviados al dispositivo, orden de estados de renderizado y estado global final. Las llamadas continúan al RenderWare real. En la ejecución final se observaron por versión 486 transformaciones y dibujos 3D, 9.666 proyecciones, 2.748 sprites y 162 envíos 2D. Todos los valores registrados coinciden; tolerancia final: cero. El analizador también rechaza registros duplicados, contadores falsos, diferencias discretas/numéricas y restauraciones fallidas; un registro incompleto no aprueba.

## Evidencia y límites

Consultar `validation-summary.json` para hashes, resultados y rutas. El log final está en `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clouds/20260922-202449-837866/comparisons.jsonl`. Ensayos anteriores detectaron y permitieron corregir el color de los sprites y las diferencias de precisión; sus informes fallidos se conservan. El desensamblado local está en `artifacts/RenderBottomFromHeight.asm`.

Se usa la matriz de vista real del momento: al variar la orientación inicial puede cambiar el número de sprites visibles entre ejecuciones. Ambas versiones usan la misma matriz dentro de cada comparación; el banco no recorre todas las orientaciones de cámara. La precisión x87 observada y comprobada es de 24 bits. No se ensayaron otros modos de precisión, fallos de asignación/transformación de RenderWare ni valores RGB fuera de 0–255.

Las dependencias existentes, incluidas `CSprite::CalcScreenCoors` y el dibujo de sprites, son compartidas por ambas versiones. Se comparan comandos y datos, no imágenes finales ni todas las interacciones del juego. Se excluyen los hooks `Audio` y `Global/SurfaceInfos_c`, como en los otros bancos. La sonda archivada valida esta reconstrucción; no demuestra integración con cambios futuros ni estabilidad general del juego. La campaña integral sigue reservada para cuando estén completas todas las clases.

El ASI normal, sin instrumentación, quedó instalado solo en la copia de pruebas con respaldo en `_baseline/before-clouds-20260922`. Instalación personal conservada y partidas sin cambios. Sin commit ni push.
