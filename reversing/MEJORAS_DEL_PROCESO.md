<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reconstrucción y pruebas: proceso y mejoras

> **Estado vigente:** [objetivo y continuidad](#continuidad-vigente) y [hoja de ruta de ReSA](../docs/ProjectRoadmap.md). Las secciones de continuidad anteriores son históricas; sus cifras, pendientes y hashes describen cada entrega. Comprobar siempre Git antes de continuar.

## Continuidad vigente

La migración a C++ cuenta con 353 funciones fuente convertidas y revalidadas, incluidos CanSunbathe, Get2dEffectStore, tres helpers trigonométricos de nubes y diez de aves, luces, estelas, jetpack y natación. Evidencias y alcance individual en [approvals.json](CppOnlyMigration/approvals.json): cero diferencias, comparaciones no nulas y restauración verificada. El corte aprobado deja 736 apariciones de ensamblador y 244 declaraciones naked en 59 archivos. Nubes pasó 3072 pares estrictos de tres consumidores y una regresión de 20 métodos con 8384 pares. TrigSimple10 pasó otros 5632 pares sobre seis consumidores, con código generado y callers contrastados. TrigCompound6 pasó 9216 pares compartidos en cuatro campañas; seis helpers conservan sus operaciones y trece cuerpos consumidores fueron contrastados en sus objetos compilados. Los helpers no añaden entradas del EXE ni multiplican sus pares compartidos. Estas cifras cuentan apariciones, no funciones. Memoria superó las ocho calibraciones tras preservar el contexto del registro de errores; falta una campaña completa sin logger y la conversión de MemoryMgr.cpp. La conversión total sigue pendiente; cada cambio requiere pruebas exactas dentro del juego.

Actualización del 10/10/2026. Rama `main`, remoto `pedrokilo/ReSA`; último resultado publicado en `b63c9146`: 105 funciones de juego de la cohorte original aprobadas, 243.392 pares finales, cero diferencias y 130 funciones de juego pendientes. El catálogo principal tiene 8.550 entradas con evidencia histórica; su 100% no acredita todo el EXE ni independencia transitiva. Consultar [la cohorte](OriginalDependencies499/README.md) y [el alcance actual](../docs/CurrentStatus.md).

La orden vigente del usuario cambia la prioridad: convertir `source/game_sa` a C++ sin ensamblador en línea ni funciones `naked`, conservando resultados exactos frente al EXE. El inventario inicial, después de excluir comentarios y cadenas, detectó 185 archivos afectados, 965 apariciones de ensamblador y 312 declaraciones `naked`. Son apariciones léxicas, no cantidades de funciones. Ver [inventario de conversión](CppOnlyMigration/README.md).

Primera conversión comprobada: `CCarCtrl::FindSequenceElement` (`0x421770`), 2.048 pares exactos dentro del juego; el nuevo banco [CppSequence1](CppSequence1/README.md) pasó 4.352 pares en total, con dos funciones acompañantes, cero diferencias y restauración completa. No añade funciones a la cohorte: vuelve a probar una implementación ya catalogada tras cambiar su lenguaje fuente. El resto del ensamblador sigue pendiente.

No sustituir automáticamente instrucciones x87 por `std::sin`, expresiones equivalentes o cálculos con tolerancias: pueden cambiar precisión, orden de redondeo, NaN, retornos o estados observables. Tampoco reemplazar una implementación por llamadas al cuerpo original para presentarla como C++ reconstruido. Revisar por separado cálculo, accesos a memoria y contratos de llamada. Cada conversión necesita una comparación nueva dentro del juego; los registros históricos no se reescriben para aparentar que ya probaron la nueva fuente. Las sondas de prueba pueden conservar ensamblador para medir el EXE y su ABI: la exigencia de conversión corresponde al código de producción.

La copia de pruebas actual es `E:/JUEGOS/GTA San Andreas - CVehicle Test`. Conservar la instalación personal, los 28 documentos congelados, sus hashes y `runtime_runner.py`. Compilar y ejecutar sólo en serie; conservar binarios originales, partidas y configuración. No registrar ni publicar propuestas pendientes como aprobadas.

El análisis global del EXE sí utilizó Ghidra 12.1.4 para inventariar entradas y llamadas; los cuerpos concretos se contrastan también mediante el desensamblador local. Las afirmaciones anteriores sobre herramientas, ramas, rutas y cifras describen sus entregas históricas y no sustituyen esta continuidad.

## Qué se hizo realmente y qué faltaba mostrar

Las pruebas anteriores fueron comparaciones automatizadas dentro del proceso de GTA. Una sonda preparaba entradas, ejecutaba la función original y la reconstrucción, y comparaba resultados. CWeaponEffects también registró las llamadas de dibujo y estados gráficos, reenviándolos al motor. Esto aporta evidencia técnica, pero no equivale a observar una situación jugable ni a comparar capturas.

El arranque seguía mostrando el prólogo. La condición utilizada —jugador existente y CCutsceneMgr::IsRunning() falso— no asegura que haya terminado toda la introducción controlada por scripts. Además, un banco completo puede ejecutarse en una sola actualización o fotograma: los estados se restauran antes de que el usuario pueda distinguir los escenarios. Fue incorrecto presentar «probado dentro del juego» sin explicar esa diferencia. El porcentaje representa funciones reconstruidas, no una certificación visual o de estabilidad general.

## Cómo se reconstruye una función

1. Identificar la entrada pendiente, su dirección, firma, llamadas y estructuras. Comprobar la versión del ejecutable mediante hash.
2. Leer las instrucciones originales con el desensamblador local y contrastarlas con el C++ existente. El inventario global también dispone del análisis Ghidra; no es necesario repetirlo para cada función.
3. Traducir o corregir el comportamiento: orden de operaciones, tipos, flags, memoria, callbacks, efectos secundarios y casos límite. Una implementación existente puede estar desactivada precisamente porque necesita revisión.
4. Compilar, mantener disponible el original y preparar comparaciones con entradas equivalentes. Los resultados esperados deben proceder del original, no de copiar la implementación nueva en una prueba.
5. Ejecutar un banco determinista dentro del motor cuando corresponda. Registrar qué se ejecutó, qué ramas se cubrieron, tolerancias, diferencias y restauración. Separar dependencias reales de cualquier simulación.
6. Usar comparaciones automáticas como método principal. La revisión visual es opcional: realizarla si el usuario la pide o si una diferencia no puede explicarse con los registros. No convertir el acceso al escritorio en requisito para validar una función mediante pruebas automáticas.
7. Documentar cada nivel por separado. Actualizar hooks.csv y el listado únicamente tras la prueba dentro del juego exigida por el usuario; indicar qué pruebas se hicieron y si hubo revisión visual, sin exigirla cuando no sea necesaria.

## Revisión visual opcional

- Preparar una partida de pruebas reproducible después del prólogo, separada de las partidas originales. Comprobar control del jugador y ausencia de misiones/cinemáticas que interfieran; no basta con un único flag de cutscene.
- Mostrar en pantalla función, versión ORIGINAL/RECONSTRUIDA, número y nombre del caso y resultado. Mantener cada escenario el tiempo suficiente para observarlo.
- Alternar A/B en el mismo escenario. Para animaciones, observar caminar, correr, mezclas, pausas, transiciones y entrada/salida de cámara; para miras, enseñar objetivos y fijación en pantalla.
- Guardar evidencia visual cuando aporte valor. Comparar llamadas de dibujo no significa comparar píxeles. Un fotograma idéntico tampoco demuestra por sí solo que el estado interno coincida.
- Priorizar el banco automático rápido. Una demostración visible sería una mejora opcional, no un requisito para continuar. Los informes deben distinguir entre comparación de datos y observación visual.

## Un único acceso a las pruebas

No crear un CMD por cada función. Usar PROBAR.cmd y un catálogo de casos con identificadores estables. El menú invoca los ejecutores existentes y permite añadir nuevas funciones sin añadir lanzadores a la raíz. También debe ofrecer --list y --case para uso automatizado.

Los siete CMD específicos fueron retirados; el menú común conserva el acceso a sus pruebas, incluida la comparación aislada de guardados. Las carpetas actuales contienen pruebas, evidencias o dependencias activas y se conservan. La consolidación inicial del menú no convierte en común la lógica interna de todos los ejecutores: esa extracción es un paso posterior.

## Organización del directorio reversing

Una carpeta por clase o módulo, con varios casos dentro cuando sea necesario; no una nueva estructura completa por cada función. Compartir el lanzador, configuración, bloqueo de instalación, respaldo/restauración, hashes, límites de tiempo y formato de informes. Mantener específicos de cada caso las entradas, captura de resultados y criterios de aprobación.

Separar fuentes de prueba y documentación de los artefactos locales voluminosos: ASI/PDB, logs, desensamblados y cachés de dependencias. Los artefactos locales deben quedar ignorados por Git y enlazados por el informe, con hash. La caché de Conan existente bajo CVehicle necesita una migración específica porque las rutas de compilación actuales dependen de ella; no moverla a ciegas.

## Estado de implementación

- Formato del listado: panel generado desde hooks.csv, con resumen, tablas y bloqueos; las clases completas muestran nombre y total, y las parciales muestran fracción, pendientes y porcentaje. Ver [ReversedClasses.md](../docs/ReversedClasses.md).
- Acceso común: PROBAR.cmd, reversing/run.py y reversing/cases.json.
- Ejecutores históricos: conservados, pendientes de extraer su infraestructura repetida.
- Demostración visible después del prólogo: opcional y fuera del flujo obligatorio, según la preferencia del usuario. No confundirla con las comparaciones automáticas realizadas.
- RpAnimBlend: 43/43; 652 comparaciones aprobadas dentro del motor con datos independientes. Infraestructura compartida en runtime_runner.py; fuentes e informe en RpAnimBlend/.

## Porcentajes de avance

Se calculan a partir del indicador reversed en docs/hooks.csv y se muestran con dos decimales mediante contrib/hooks2md.py. El encabezado mide clases del grupo respecto a clases totales; cada fila mide funciones reconstruidas respecto a funciones registradas de esa clase. No representan cobertura de pruebas. Un recálculo del informe no cambia el estado de ninguna función.

## Continuidad para otro chat (2026-09-22)

### Instrucciones del usuario

Trabajar en español y ejecutar el trabajo técnico de principio a fin. El usuario no necesita manejar Ghidra ni preparar pruebas manualmente. Priorizar scripts locales que puedan terminar sin un agente activo; consultar el resultado al finalizar, evitando supervisión constante. La reconstrucción y el diagnóstico siguen requiriendo trabajo técnico: el runner automatiza la ejecución de pruebas, no reconstruye funciones por sí solo.

La revisión visual es opcional. No detener el trabajo porque computer-use no esté disponible. Anteriormente falló la conexión nativa al escritorio; eso no impidió ejecutar comparaciones automáticas en GTA. No instalar Ghidra por rutina: se ha usado dumpbin y el código del proyecto.

No cambiar docs/hooks.csv ni completar una clase en docs/ReversedClasses.md antes de comparar la función contra el original dentro del motor. Compilar, arrancar GTA o ver un hook habilitado no demuestra que la función se haya probado. No afirmar cobertura exhaustiva ni estabilidad general a partir de los casos controlados.

### Rutas y estado comprobado

- Repositorio: D:\Original\gta-reversed-master.
- Rama local comprobada: ReSA. Destino indicado por el usuario: pedrokilo/RePorteoGTA-XBOXONESS. Verificar el remoto antes de cualquier publicación.
- Al comenzar esta actualización documental, git status --short estaba limpio. No suponer que siguen pendientes los cambios mencionados en el chat anterior; consultar Git. Esta actualización del Markdown es un cambio nuevo. No se realizó commit ni push en esta tarea de traspaso.
- Juego de pruebas: D:\JUEGOS\GTA San Andreas - CVehicle Test.
- Instalación personal: E:\JUEGOS\GTA San Andreas. Conservarla y conservar las partidas originales.
- Ejecutable original analizado: D:\Grand Theft Auto San Andreas\gta-sa.exe. SHA-256 esperado: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Verificar también el ejecutable de la copia de pruebas antes de usar direcciones fijas.
- Los cambios se cargan mediante scripts\gta_reversed.asi en la copia de pruebas. Los hooks modifican el proceso en memoria; no se necesita parchear el ejecutable en disco.
- ASI normal instalado al finalizar RpAnimBlend: SHA-256 48b5332d9d96f75a69aa20471ef801f38649c94f581567d18dd5eba20199ab15. Verificar si posteriormente cambió.

### Último trabajo completado: RpAnimBlend

RpAnimBlendClumpUpdateAnimations, dirección 0x004D34F0, quedó habilitada tras 652 comparaciones aprobadas dentro del motor. RpAnimBlend figura completa con 43 funciones. Error máximo 2.9802322387695312e-08 frente a tolerancia 1e-05. Pose del jugador y partidas sin cambios.

Leer RpAnimBlend/README.md, RpAnimBlend/validation-summary.json y RpAnimBlend/fixture-notes.md. El registro completo local está en D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-rpanimblend\20260922-191537-770481. Su hash se conserva en el resumen. No usar los ensayos descartados como evidencia de aprobación.

Lección importante: RpClumpClone podía dejar atomics apuntando a la jerarquía original. La sonda final reasigna la jerarquía clonada, crea su animación y exige almacenamiento de poses independiente entre original, candidato y jugador. La igualdad entre dos resultados no es suficiente si comparten memoria modificable.

El ASI normal se compiló sin la instrumentación temporal, se instaló con respaldo y se retiraron las copias .before utilizadas para restaurar las fuentes. La sonda archivada para repetir el banco está en la carpeta _baseline del juego de pruebas; su hash esperado está en RpAnimBlend/run_local.py. Las fuentes de la sonda se conservan en Probe.hpp y LiveComparison.hpp, pero no están conectadas al build normal. No confundir recompilar el ASI normal con regenerar esa sonda.

También se trabajaron CVehicle, C_PcSave, CPlayerPed, dos funciones de CPed y CWeaponEffects. Consultar sus informes en reversing y los contadores actuales en docs; no repetirlas ni cambiar sus estados por memoria del chat. CPed quedó documentada en 139/145. No hay una siguiente función elegida por el usuario después de RpAnimBlend.

### Herramientas locales y ejecución

Desde la raíz del repositorio:

```powershell
.\PROBAR.cmd --list
.\PROBAR.cmd --case rpanimblend
& 'C:\Users\pedro\AppData\Local\Programs\Python\Python314\python.exe' reversing/run.py --case rpanimblend
```

El último comando evita la pausa interactiva del CMD. Consultar reversing/cases.json para los demás identificadores. Mantener un solo PROBAR.cmd y carpetas por clase, reutilizando infraestructura. Los siete CMD específicos se eliminaron. La carpeta reversing sí se utiliza; no borrarla ni eliminar CVehicle/conan-cache, todavía referenciada por CMake.

Compilación normal utilizada:

```powershell
& 'D:\Original\gta-reversed-master\reversing\CVehicle\conan-cache\p\cmake20ac4e0137fee\p\bin\cmake.exe' --build build --config Debug
```

Salida: bin\Debug\gta_reversed.asi y .pdb. Compilador Visual Studio 18 Community, MSVC 14.51.36231, x86. dumpbin está bajo C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x86. Comprobar rutas si el entorno cambió.

El runner compartido reversing/runtime_runner.py comprueba hashes, evita concurrencia, respalda el ASI instalado, instala la sonda, inicia GTA con límite de tiempo, analiza registros y restaura el ASI anterior. Solo termina el proceso iniciado por él y verifica las partidas. Los ejecutores antiguos aún conservan parte de su infraestructura propia: no asumir que todos usan el runner compartido.

La configuración de pruebas utilizada excluye los hooks Audio y Global/SurfaceInfos_c mediante --unhook=Audio,Global/SurfaceInfos_c; registra también runtime-hooks.csv. Esta configuración limita el alcance de la prueba y no demuestra compatibilidad con todos los hooks activos. No modificar partidas para forzar una carga incompatible.

### Procedimiento para la siguiente reconstrucción

1. Leer este archivo, comprobar instrucciones locales, Git, docs/hooks.csv y el informe de la clase elegida. Si el nuevo chat no indica otra función, pedir únicamente cuál quiere abordar; no reabrir RpAnimBlend como pendiente.
2. Identificar dirección y contrato original; desensamblar el ejecutable verificado y contrastar con el C++. Revisar ABI, registros, tamaños, tipos, orden aritmético, enlaces, callbacks y efectos globales.
3. Implementar el cambio mínimo y compilar. Mantener el original disponible para comparación y no publicar todavía el progreso.
4. Preparar un caso reproducible con entradas iguales y memoria independiente. Alternar el hook para llamar al original y al candidato. Registrar contadores que demuestren cuál se ejecutó, estados antes/después, callbacks, errores y tolerancias justificadas.
5. Cubrir ramas pertinentes y estados válidos. Si falla el propio original por una entrada artificial inválida, documentarlo y corregir el banco; no ocultarlo ni contarlo como éxito del candidato. Nunca sustituir la comparación por una prueba que repita la misma implementación.
6. Ejecutar el banco mediante scripts locales sin requerir control visual. Guardar logs y resumen con hashes. Corregir diferencias y repetir los casos afectados antes de declarar éxito.
7. Restaurar fuentes instrumentadas, compilar el ASI normal e instalarlo únicamente en la copia de pruebas con respaldo. Conservar fuentes y evidencia de la sonda. Verificar que no quedan llamadas de instrumentación en el build normal.
8. Solo tras aprobar: cambiar la fila correspondiente de hooks.csv, regenerar ReversedClasses.md y documentar alcance y limitaciones. No importar indiscriminadamente todo runtime-hooks.csv: contiene selecciones temporales de hooks.
9. Revisar el diff, el menú y los archivos necesarios. No borrar dependencias activas, informes útiles ni cambios ajenos. Publicar o crear commits solo cuando lo pida el usuario; un pedido de nombre de commit no es una orden de publicarlo.

Regeneración del listado:

```powershell
& 'C:\Users\pedro\AppData\Local\Programs\Python\Python314\python.exe' contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

Clases completas: `- RpAnimBlend (43)<br />`. Parciales: fracción y porcentaje con dos decimales. Los porcentajes cuentan funciones registradas como reconstruidas, no porcentaje del comportamiento del juego demostrado.

## Fase futura: estabilidad del conjunto, después de completar todas las clases

Decisión del usuario (2026-09-22): realizar esta fase cuando todas las clases estén completas. Hasta entonces, continuar la reconstrucción y validación individual exigida antes de habilitar cada función y actualizar el progreso. No iniciar ahora la campaña integral ni sustituir con ella las comparaciones contra el original.

Los trabajos de `reversing` aportan evidencia sobre funciones, entradas y escenarios documentados; no garantizan la estabilidad general del juego. Una clase marcada como 34/34 tiene sus 34 funciones registradas como reconstruidas, pero eso no significa que las 34 hayan sido verificadas exhaustivamente. Las pruebas aprobadas conservan su valor dentro de su alcance.

Ejemplos de alcance: CBaseModelInfo tiene 65.536 comparaciones de flags de SwaysInWind; RpAnimBlend tiene 652 comparaciones de actualización de animaciones; las 2.088 comparaciones de C_PcSave verifican GenerateGameFilename, no un ciclo completo de guardar/cargar; CWeaponEffects compara llamadas de dibujo, no todos los efectos durante una partida.

### Trabajo previsto al completar todas las clases

1. **Regresión sobre una misma compilación actualizada.** Adaptar y ejecutar los bancos contra la versión conjunta vigente, vinculando fuentes, configuración, binarios e informes mediante hashes. Los ejecutores actuales usan sondas archivadas: repetirlas comprueba aquellas versiones, no automáticamente los cambios posteriores.
2. **Pruebas de integración reproducibles.** Combinar animaciones, peatones, vehículos, combate, interiores y streaming para comprobar interacciones entre sistemas.
3. **Ciclos completos de guardar y cargar.** Utilizar exclusivamente partidas desechables y aisladas en la copia de pruebas; comprobar que el estado esperado se conserva tras la carga.
4. **Sesiones prolongadas automatizadas.** Detectar cierres, bloqueos, referencias residuales y crecimiento anormal de memoria. Registrar duración, escenarios, resultados y diagnósticos suficientes para reproducir los fallos.
5. **Configuración real de uso.** Resolver o delimitar los problemas de los hooks Audio y Global/SurfaceInfos_c, excluidos en las comparaciones anteriores. Documentar exactamente qué hooks están activos y qué configuraciones se han probado.

Reutilizar `reversing`, el runner compartido y un solo `PROBAR.cmd`. Priorizar ejecuciones locales autónomas con límites de tiempo, registros, respaldo/restauración y protección de la instalación personal y las partidas originales. La revisión visual sigue siendo opcional.

El resultado deberá expresarse como «esta compilación pasó estos escenarios durante tantas horas, con esta configuración», acompañado de evidencia y limitaciones. Las pruebas finitas no garantizan ausencia absoluta de fallos; esta fase busca aportar evidencia de estabilidad del conjunto mucho más amplia que las validaciones individuales.

## Actualización de continuidad: CBaseModelInfo y FxManager_c (2026-09-22)

Esta actualización complementa el estado histórico de RpAnimBlend descrito arriba. CBaseModelInfo quedó en 34/34 tras corregir el retorno de SwaysInWind y aprobar 65.536 comparaciones. FxManager_c quedó en 25/25 tras corregir la lectura del siguiente nodo después de destruirlo en DestroyAllFxSystems y Update. El banco final aprobó 720 comparaciones: 180 de destrucción y 540 del recorrido de Update. Consultar los README y validation-summary.json de ambas carpetas; no tratarlas como pendientes.

El ASI normal más reciente, sin instrumentación, está instalado únicamente en la copia de pruebas y tiene SHA-256 `6999d5daf6a7e4a9051ed773dda66dcd0d66e8572b1211bcbf9059b58c8b5176`. Respaldo anterior en `_baseline/before-fxmanager-20260922`. Partidas sin cambios. Ejecutar `PROBAR.cmd --case fxmanager` o el runner Python para repetir el banco con la sonda archivada. El alcance de Update es el recorrido de sistemas en estados FX_KILLED/FX_3, no toda la simulación de efectos.

No hubo commit ni push. Hay cambios de código, documentación y carpetas de pruebas de estos trabajos en el árbol de trabajo; comprobar Git antes de continuar y conservarlos. No se ha elegido otra clase después de FxManager_c. La campaña integral sigue aplazada hasta completar todas las clases.

## Actualización de continuidad: CClouds (2026-09-22)

El usuario eligió CClouds después de FxManager_c. Quedó en 21/21 al reconstruir RenderBottomFromHeight (0x7154B0), antes vacía y deshabilitada. El banco final aprobó 1.170 comparaciones dentro del motor con igualdad numérica exacta (tolerancia cero), verificando geometría, proyección, sprites, estados y restauración. Los contadores distinguen original y candidato. Se corrigieron diferencias detectadas en el color de los sprites y la precisión aritmética: el hilo de renderizado usa x87 de 24 bits. Consultar reversing/CClouds/README.md y validation-summary.json; no tratar esta función como pendiente.

La clase figura como completa por sus 21 funciones registradas; este banco valida específicamente RenderBottomFromHeight, no todas las funciones de CClouds ni la estabilidad general del juego. Usa la matriz de vista real y dependencias de dibujo compartidas. Continúan excluidos Audio y Global/SurfaceInfos_c. La campaña de estabilidad del conjunto permanece aplazada hasta completar todas las clases.

El ASI normal más reciente, sin instrumentación, está instalado solo en la copia de pruebas y tiene SHA-256 `1500be85a8d953d1c8a8485eabc3e6439445c03b19671d36e25b881027ac78f0`. PDB: `3ca09924b8ea7694287e3fe66203445af22cacc3ad439c65c8179e3205b7bcce`. Respaldo en `_baseline/before-clouds-20260922`. Partidas verificadas sin cambios. La sonda final archivada tiene SHA-256 `548c54113a6673f7db9aee8ee55fe38582bf71b36d97c8dad859f6eefee99e50` y se repite mediante `PROBAR.cmd --case clouds` o `python reversing/run.py --case clouds`, que termina sin pausa. Está integrada también en --all, conservando un solo PROBAR.cmd.

La ejecución conjunta documentada en ULTIMA_EJECUCION.md es anterior a CClouds: sus nueve bancos aprobaron 76.319 pares con sondas archivadas. No confundir ese informe histórico con una ejecución de los diez bancos actuales ni con una compilación integrada. La última ejecución de CClouds está en `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clouds/20260922-202449-837866`.

No hubo commit ni push. Conservar los cambios de las clases anteriores y de la infraestructura. No se ha elegido la siguiente clase después de CClouds.

## Actualización de continuidad: CMenuSystem (2026-09-22)

El usuario eligió CMenuSystem después de CClouds. Quedó en 21/21 al reconstruir FillGridWithCarColours (0x5820E0), antes delegada al original. El banco aprobó 784 pares dentro del motor, con 196 paletas, ambos IDs de menú y dos patrones de memoria inicial: 50.176 índices de color iguales. Verifica estructuras independientes, contadores de ejecución, escrituras limitadas a la cuadrícula, tabla de colores inmutable y restauración. Consultar reversing/CMenuSystem/README.md y validation-summary.json; no tratarla como pendiente.

La reconstrucción conserva los límites RGB estrictos de 15, la comparación adicional con una entrada negra y el uso del índice 127 al agotar alternativas. El banco cubre FillGridWithCarColours; no valida el dibujo, navegación ni todas las funciones de CMenuSystem. Continúan las exclusiones de Audio y Global/SurfaceInfos_c y la campaña integral sigue aplazada hasta completar todas las clases.

ASI normal actual, sin instrumentación, instalado solo en la copia de pruebas: SHA-256 `ce812b18c0f2154af48dede876b863e68040d4e0fa3d85f6c0aa68f303f1feb3`. PDB: `d51c5b1332d30eb38f523f987feeb1d161143db926fc3f2795e79a56a4d8fa06`. Respaldo en `_baseline/before-menusystem-20260922`. Partidas verificadas sin cambios. Sonda archivada: `31f0abcaad1fd61e590e4aa26b3da26c01ae224f1d4883a18d26349b129dee00`. Ejecución: `PROBAR.cmd --case menusystem` o `python reversing/run.py --case menusystem`, sin pausa. Caso añadido a --all, conservando un solo CMD.

Evidencia final: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-menusystem/20260922-203512-507016`. El informe ULTIMA_EJECUCION.md sigue siendo histórico y anterior a CClouds y CMenuSystem; no se volvió a ejecutar toda la serie en este trabajo. No hubo commit ni push. Conservar los cambios previos. No se ha elegido otra clase después de CMenuSystem.

## Actualización de continuidad: CTaskSimpleUseGun (2026-09-22)

El usuario eligió CTaskSimpleUseGun después de CMenuSystem. Quedó en 19/19 al reconstruir ProcessPed (0x62A380), antes delegada al original con un borrador incompleto. El banco final aprobó 3.232 comparaciones dentro del motor: 64 casos con dependencias reales alcanzadas y 3.168 con dependencias de animación/apuntado controladas. Diferencia numérica máxima 0 con tolerancia 1e-5. Incluye órdenes, cuentas atrás, disparo simple/continuo/doble, ráfagas, recarga, objetivos vivos/muertos, orientación y cambio de arma. Consultar reversing/CTaskSimpleUseGun/README.md y validation-summary.json; no tratarla como pendiente.

Límite esencial: los casos controlados registran y sustituyen efectos de AimGun, SetMoveAnim, RemoveStanceAnims, StartAnim y FightStrike. No equivalen a validar un combate completo con poses, impactos y cadenas IK activas. Se compara la función original real en ambos modos, con almacenamiento independiente, contadores y restauración; los callbacks se normalizan por identidad de función. Los detalles y las dependencias que sí continúan al motor están en su README. La campaña integral sigue aplazada hasta completar todas las clases.

ASI normal actual, sin instrumentación, instalado solo en la copia de pruebas: SHA-256 `05e7cbda3213066e8ae6452d8578a3e23b1c37a45631313303183fb3e2e571d5`. PDB: `1c20494da65b4b41a0cbd3957d0be7ada4ef4aee14148291dc307d0a80102c0c`. Respaldo en `_baseline/before-usegun-20260922`. Partidas y jugador real verificados sin cambios. Sonda archivada: `a10f00f4cb717fa3e556b9ea42f4f019036bbf997f6f4f6d0ab621029a8ba345`. Se repite mediante `PROBAR.cmd --case usegun` o `python reversing/run.py --case usegun` sin pausa. Integrada en --all, con un solo PROBAR.cmd.

Evidencia final: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-usegun/20260922-205940-939686`. Se corrigió durante las comparaciones la exclusión exacta del final del bucle de disparo continuo. No se ejecutó toda la serie de sondas; ULTIMA_EJECUCION.md sigue siendo un informe histórico anterior. Se mantienen excluidos Audio y Global/SurfaceInfos_c durante las comparaciones. No hubo commit ni push. Conservar los trabajos anteriores y los cambios de esta clase. No se ha elegido la siguiente clase después de CTaskSimpleUseGun.


## Cola autorizada por el usuario (2026-09-22)

Continuar en este orden sin volver a preguntar la siguiente clase entre entradas. Completar reconstrucción, comparación contra el original dentro del motor, compilación normal, instalación en la copia de pruebas y documentación por clase. No actualizar progreso antes de validar, no hacer commit/push. Los porcentajes siguientes son el estado inicial, no cobertura de pruebas.

| Orden | Clase | Estado inicial | Pendiente registrada | Estado del trabajo |
|---|---|---|---|---|
| 1 | CTaskComplexWander | 13/14 | CreateNextSubTask | Completada: 14/14, 10.240 pares |
| 2 | CTaskSimpleJetPack | 13/14 | DropJetPack | Completada: 14/14, 128 pares |
| 3 | CQuadBike | 11/12 | ProcessControl | En curso |
| 4 | CTaskSimplePlayerOnFoot | 11/12 | PlayerControlFighter | En cola |
| 5 | CClothes | 11/11 | GetTextureDependency | Validada: 4.096 pares |
| 6 | CConversations | 11/11 | DoneSettingUpConversation | Validada: 2.048 pares |
| 7 | CEntryExit | 10/11 | TransitionFinished | Completada: 11/11 (sin cambio de CSV, ya estaba reversed=1), 960 pares dentro del motor + 9 pares de fase 2 con jugador real y bloque invasivo completo |
| 8 | CCarFXRenderer | 9/10 | InitialiseDirtTexture | En cola |
| 9 | CTaskComplexDestroyCar | 8/9 | ControlSubTask | En cola |
| 10 | CRoadBlocks | 7/8 | CreateRoadBlockBetween2Points | En cola |
| 11 | CAEStaticChannel | 7/8 | SetAudioBuffer | En cola |
| 12 | CAEAmbienceTrackManager | 6/7 | UpdateAmbienceTrackAndVolume | En cola |
| 13 | CTaskSimpleFightingControl | 6/7 | ProcessPed | En cola |
| 14 | CTaskAllocatorKillThreatsBasic | 5/6 | ComputeClosestPeds | En cola |
| 15 | CAEWeatherAudioEntity | 4/5 | AddAudioEvent | En cola |
| 16 | CScripted2dEffects | 5/5 | Init | Validada: 256 pares |
| 17 | CEntityScanner | 3/4 | ScanForEntitiesInRange | En cola |
| 18 | FxPrimBP_c | 1/2 | Load | En cola |
| 19 | CTaskComplexDragPedFromCar | 1/2 | CreateFirstSubTask | En cola |
| 20 | CPedGroupDefaultTaskAllocatorSitInLeaderCar | 1/2 | AllocateDefaultTasks | En cola |
| 21 | CPedGroupDefaultTaskAllocatorRandom | 1/2 | AllocateDefaultTasks | En cola |

## Continuidad: CTaskComplexWander (2026-09-22)

14/14 tras 10.240 comparaciones de CreateNextSubTask dentro del motor. Se corrigio la comparacion contra el nodo anterior guardado, se elimino una transicion ausente en el original y se conservo el orden de creacion de tareas. Banco con rutas y consultas controladas, constructores reales; no valida ejecutar toda la navegacion. README y validation-summary.json contienen evidencia. PROBAR.cmd --case wander, tambien integrado en --all. ASI normal instalado solo en copia de pruebas: `a170d33dba406d745f272f3fbde87fa37ed1f2092edc4ee550aaf5ed80b4cbfd`. Partidas sin cambios. Sin commit/push. Sigue CTaskSimpleJetPack y despues el resto de la cola autorizada. La campana integral sigue aplazada.

## Continuidad: CTaskSimpleJetPack (2026-09-22)

14/14 tras 128 pares de DropJetPack. La implementacion existente coincide: se habilito despues de generar realmente 32 pickups por lado y verificar el fallo controlado de generacion. Se usan coordenadas/collision reales; RNG determinista temporal y aborto virtual controlado. No se valido vuelo completo ni recogida/visualizacion/FX/audio. README y validation-summary.json detallan evidencia. PROBAR.cmd --case jetpack y --all. ASI normal en copia de pruebas: `b2ad166660a0fbcb3d38d1f5e76900b5c052fede16dcb5a601d6db4dd6ae2d76`. Partidas y tabla de pickups restauradas. Sin commit/push. Sigue CQuadBike y el resto de la cola.

## Punto de interrupcion: CQuadBike (2026-09-22)

Trabajo interrumpido tras completar Wander y JetPack. CQuadBike::ProcessControl tiene un candidato nuevo en source/game_sa/Entity/Vehicle/QuadBike.cpp, todavia SIN compilar ni validar; hook sigue reversed=false y progreso 11/12. No habilitar antes de comparar. Desensamblado local en reversing/CQuadBike/artifacts/ProcessControl.asm (ignorado por Git). Se corrigieron sobre el borrador: uso de resistencia X ajustada, resistencia dependiente de velocidad X, pruebas exactas de suspension y TransformVector del centro de masa. Revisar contra el original en motor antes de darlo por correcto.

Constantes leidas del ejecutable: 0x871ABC=.995, 0x871AC0=.5, 0x871AC4=1, 0x871AC8=1. Original toma velocidad angular local (0x59C810), ajusta resistenciaX y coeficiente de velocidadX (.995 o .5), divide X por (vx*vx*coef+1), Y por (vy*vy+1), eleva a timestep; aplica dos fuerzas (0x542A50), primero -up*deltaY*turnMass en right+TransformVector(COM), despues up*deltaX*turnMass en forward+TransformVector(COM); termina CAutomobile::ProcessControl(0x6B1880). Caso delanteras con compresion exactamente 1: rueda trasera <1 y forward.z>0 ajusta wheelie; de lo contrario fija resistenciaX=.995. Si delanteras apoyadas y WheelCounts traseras exactamente1, ajusta stoppie para forward.z<0. Falta banco con entradas independientes, fuerzas reales y ProcessControl de Automobile controlado, restauracion de timestep/resistencia/hooks. Se agrego reversing/ProbeSupport.hpp con EntryPatch/Bytes reutilizables, usado por JetPack.

No hay GTA ni MSBuild ejecutandose al comprobar este punto. ASI normal instalado sigue siendo el de JetPack (b2ad166660a0fbcb3d38d1f5e76900b5c052fede16dcb5a601d6db4dd6ae2d76), no contiene el candidato nuevo de QuadBike. Continuar la cola autorizada desde CQuadBike; quedan esta y las 18 posteriores. Sin commit/push.


## Continuidad: CPed completada (2026-09-22)

El usuario dio prioridad a CPed sobre la cola anterior. CPed queda en 145/145 entradas registradas tras reconstruir CalculateNewVelocity, DoFootLanded, PlayFootSteps, MakeTyresMuddySectorList, PositionPedOutOfCollision y KillPedWithCar. No tratar estas seis como pendientes. El banco final paso 28.672 pares original/candidato dentro de GTA, tambien arrancando con los seis hooks habilitados: 4096 + 2048 + 8192 + 2048 + 4096 + 8192 respectivamente. Diferencia maxima 0.000000476837158203125, tolerancia 0.00001. README y validation-summary.json detallan evidencia, hashes y limites. Los bancos historicos de PositionAttachedPed y UpdatePosition se conservaron, sin repetirlos en este cierre.

PROBAR.cmd --case cped-completion, tambien integrado en --all. Una sola infraestructura y un solo CMD. El nuevo banco termina automaticamente, compara estados y argumentos de dependencias controladas, valida los contadores y restaura hooks/globals/jugador, ASI/PDB y partidas. Algunas fuerzas fisicas y constructores permanecen reales; no se validaron todas las cadenas de tareas, colisiones reales del mapa, trafico completo, dano real ni representacion audiovisual. DoFootLanded conserva dos helpers originales separados con ABI en registros. El 145/145 no incluye virtuales no registradas ni significa cobertura exhaustiva o estabilidad general. La campana integral sigue aplazada hasta completar todas las clases.

ASI normal sin instrumentacion instalado SOLO en la copia de pruebas: `971e1d165ec8392b9c2301c2225988d708c4313292b8d745f04d2385bc26b5b6`. Respaldo `_baseline/before-cped-completion-20260922`. Partidas verificadas sin cambios; instalacion personal intacta. Evidencia final: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-cped-completion\20260922-221131-711733`. La unica diferencia del fuente normal frente al de la sonda final es la inclusion explicita de PedPlacement.h; la implementacion es identica. Compilacion Debug x86 aprobada. No se ejecuto de nuevo toda la serie de sondas y ULTIMA_EJECUCION.md sigue siendo historico.

Sin commit/push. Conservar todos los trabajos previos. CQuadBike sigue pendiente de validacion y con hook deshabilitado (11/12), aunque su candidato ahora compila como parte del proyecto. La cola autorizada de 21 clases sigue conservada; tras Wander y JetPack quedan CQuadBike y las 18 posteriores. CPed fue la prioridad de esta peticion, no una anulacion de esa cola.

## Continuidad: lote CConversations, CPlayerInfo y CPedList (2026-09-22)

Lote completado: CConversations 11/11 con 2.048 pares; CPedList 7/7 con 4.096 pares (1.024 por funcion); CPlayerInfo 28/28 registradas con 16.896 pares (4.096 objetos, 4.096 vehiculos y 8.704 Process). Comparaciones originales/candidatos dentro de GTA, con contadores de entrada, estados y trazas. CPlayerInfo final arranco con los tres hooks habilitados. Evidencia y hashes por clase en README.md y validation-summary.json. No tratar estas ocho funciones como pendientes.

Constructor de CPlayerInfo permanece deshabilitado y locked por el fallo conocido con armas; ya figuraba reversed y sigue delegando al original. El 28/28 no significa que este reparado. Se corrigio la firma thiscall de FindObjectToSteal. Process conserva el helper independiente de mirilla 0x56EC80 con receptor m_nCrosshairActivated (0x158), CalcRoadDensity 0x44EFC0 y constructor de recogida aun original. Dependencias controladas y cobertura finita; no cooperativo, ejecucion integral de tareas, recogida, efectos de camara RC ni estabilidad general. Campana integral aplazada hasta completar todas las clases.

hooks.csv sincronizado reversed/locked tras validar; ReversedClasses.md regenerado con contrib/hooks2md.py. Unico PROBAR.cmd: --case conversations, --case playerinfo y --case pedlist; integrados en --all. runtime_runner puede cerrar su propio proceso al evento final antes de auditar registros grandes, sin aceptar el evento por si solo. Los bancos archivados no son una prueba combinada de la compilacion actual; no se repitio toda la suite historica y ULTIMA_EJECUCION.md sigue historico.

ASI normal Debug x86 sin instrumentacion instalado SOLO en copia de pruebas: 9f6dc8c8db43ea47e9cd377c489a8fe5691f3dd0f24979426c8a5e3812221f80. Respaldo _baseline/before-playerinfo-conversations-pedlist-20260922. Partidas sin cambios; instalacion personal intacta. Sin commit/push. Rama ReSA; conservar cambios previos. CQuadBike sigue 11/12, compilada pero NO validada y hook deshabilitado. La cola anterior conserva las demas clases pendientes; este lote no las completa.

## Continuidad: constructor de CPlayerInfo corregido (2026-09-22)

Sustituye la excepcion de constructor locked descrita en el cierre anterior. CPlayerInfo sigue 28/28, ahora todas sus entradas registradas estan habilitadas y desbloqueadas. Se retiro la llamada original 0x571920 y la asignacion redundante del temporal: m_PlayerData se construye una vez in situ, preservando los campos que el original no inicializa. El temporal anterior podia copiar campos/flags indeterminados.

4.224 pares originales/candidatos aprobados: 4.096 patrones completos de memoria con guardas/retorno/contador de una inicializacion y 128 regresiones de cuatro conjuntos de armas con entregas repetidas, inventario, municion, seleccion y referencias de modelos. Ultima prueba con hook activo y unlocked desde arranque; tres llamadas reales al candidato antes del banco. Streaming y representacion de armas controlados; SetInitialState y funciones de armas compartidas existentes. No prueba disparos ni estabilidad general, ni certifica todas las manifestaciones historicas del fallo.

Evidencia: reversing/CPlayerInfo/constructor-validation-summary.json y README.md; logs en D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playerinfo-constructor\20260922-233000-236046. PROBAR.cmd --case playerinfo-constructor, integrado en --all sin nuevos CMD. Constructor actualizado a reversed=1, locked=0 solo tras validar. Banco anterior de 16.896 pares conservado sin repetir: las tres funciones no cambiaron.

ASI normal sin instrumentacion instalado SOLO en copia de pruebas: dc6281acd032e459eb86eeab373be832c9d203a7d0c00cf38ab6ea4446195009. Respaldo _baseline/before-playerinfo-constructor-20260922. Fuentes restauradas; partidas sin cambios; instalacion personal intacta. Rama ReSA; sin commit/push. Conservar los trabajos anteriores y cola pendiente. Campana integral sigue aplazada.

## Continuidad: FurnitureSubGroup_c (2026-09-22)

Completada 3/3: AddFurniture 0x5C00C0, GetFurniture 0x590EE0 y GetRandomId 0x590FD0. 20.480 pares exactos dentro de GTA (4.096 + 8.192 + 8.192), banco final con los tres hooks habilitados y sin locked desde arranque. IDs no negativos ignoran riqueza; negativos seleccionan por intervalo inclusivo. Riqueza 255 no es comodin (comparacion original contra -1 inalcanzable). Se consume RNG incluso sin coincidencias. AddFurniture conserva low-word del contador, almacen 256, insercion al principio y umbral dimensional >=0.02.

Fixtures de listas/cajas aisladas y RNG controlado; AddItem compartido existente. No colocacion/robo completos, representacion ni estabilidad general. README y validation-summary.json detallan evidencia: D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-furniture\20260922-234110-237502. PROBAR.cmd --case furniture, incluido en --all sin nuevos CMD. hooks.csv actualizado reversed=1/locked=0 tras validar; ReversedClasses.md regenerado.

ASI normal sin instrumentacion instalado solo en copia de pruebas: 36910037caae88d3f125d9492f1987e4d025b78677133b37b25efd622ac24ca1. Respaldo _baseline/before-furniture-20260922. Fuentes restauradas, partidas sin cambios e instalacion personal intacta. Rama ReSA, sin commit/push. Conservar trabajos anteriores y cola pendiente; CQuadBike sigue sin validar/deshabilitada. CPlayerInfo constructor sigue corregido y habilitado. No se repitio toda la suite ni se inicio la campana integral.

## Continuidad documental: progreso y prioridades (2026-09-23)

Se actualizó README.md para describir ReSA, los bancos aprobados, copia de pruebas, partidas, hooks y límites reales. Se retiró la estimación genérica heredada de 50-60%; los números proceden de hooks.csv. ReversedClasses.md ahora es un panel con resumen, navegación, tablas y columna locked; su formato vive en contrib/hooks2md.py y se conserva al regenerar, incluida la ejecución desde CI.

Nueva guía manual docs/ReversingPriorities.md: las 100 clases incompletas (98 parciales, 2 sin revertir), 555 funciones pendientes, cuatro grupos de dificultad y Top 10 con 23 entradas pendientes. Inspección de fuentes/headers, registros, referencias y algunos originales para candidatos inmediatos; no se presenta como desensamblado exhaustivo de 555 funciones. Incluye código candidato frente a wrappers/borradores, límites y enlaces al pendiente concreto.

La propuesta de prioridad no reemplaza automáticamente la cola autorizada ni habilita funciones. No se cambiaron fuentes del juego, flags de hooks.csv, binarios ni partidas en esta tarea documental. Se comprobaron enlaces locales, integridad de los totales, presencia única de las 701 clases y generación con casos completos/parciales/cero y locked. Sin nuevas pruebas GTA, commit ni push. El ASI normal instalado continúa siendo el de FurnitureSubGroup_c del cierre anterior.


## Continuidad: CClothes, CScripted2dEffects y Sprint (2026-09-23)

Completadas por petición expresa: CClothes 11/11, CScripted2dEffects 5/5 y CTaskComplexUseClosestFreeScriptedAttractorSprint 4/4. No tratar las cuatro funciones como pendientes. 8.448 pares exactos finales con hooks habilitados y unlocked desde arranque: 4.096 GetTextureDependency, 256 Init, 2.048 Clone y 2.048 GetTaskType. Primera ejecución del mismo banco también aprobada antes de habilitar; no suma cobertura distinta.

GetTextureDependency existente coincide. Init ahora conserva segundo PartnerTask y relleno como el helper original 0x6F9EB0. Clone construye una tarea nueva con sprint, no copia a través del constructor de copia de la base que reiniciaba a caminar. GetTaskType=262. Detalles, límites y hashes en reversing/ClothesScriptedSprint/README.md y validation-summary.json; cada clase dispone además de su README y resumen.

Banco automático integrado en el único PROBAR.cmd: --case clothes-scripted-sprint y --all. Compara tablas completas con guardas, pool real y objetos; restaura globals/hooks/jugador y comprueba partidas. No se probaron interacción prolongada, ropa completa, agotamiento del pool, tareas reales de atractores ni estabilidad general. No se repitió toda la suite. Campaña integral sigue aplazada; ULTIMA_EJECUCION.md es histórico.

Evidencia final: D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-clothes-scripted-sprint\20260923-001346-817023. ASI normal instalado solo en copia de pruebas: d62c1071aadb1b459964a46efc4bfc3b9c6b3ea094521374408c14de7e9da034. Respaldo _baseline/before-clothes-scripted-sprint-20260923. Fuentes normales coinciden con el manifiesto; partidas sin cambios e instalación personal intacta.

hooks.csv actualizado solo para cuatro filas tras validar, ReversedClasses.md regenerado y README actualizado. Guía de prioridades conserva su corte inicial y ahora señala estos tres cierres; quedan 97 clases incompletas y 551 entradas pendientes. Rama ReSA, sin commit/push. Conservar cambios anteriores, incluido CQuadBike sin validar/deshabilitado y el constructor CPlayerInfo ya corregido.


## Continuidad: CGeneral desbloqueado (2026-09-23)

CGeneral sigue 9/9 y ahora tiene cero locked. GetRandomNumberInRange<float> 0x41BD90 estaba desactivada/bloqueada por una aserción con extremos invertidos (caller 0x6DF26D). Especialización float local x87 reproduce orden, control de precisión y retorno ST(0) del original sin esa aserción. El primer candidato double falló y se descartó. No eliminar la aritmética x87 sin volver a comparar: double/SSE no son equivalentes en este contexto.

65.536 pares exactos finales, cero diferencias float y retorno de 80 bits, una llamada RNG por lado. Banco con 16 intervalos y entradas RNG controladas; ver README/validation-summary.json en reversing/CGeneral. Hook habilitado y desbloqueado desde arranque, luego compilación normal sin instrumentación. No se revalidaron las otras ocho entradas ni todos los callers, incluido el template entero que usa el helper float. No prueba todos los modos FP, NaN/infinito, conducción prolongada ni estabilidad general.

Evidencia: D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-general\20260923-003344-869382. Caso --case general en el único PROBAR.cmd y en --all. Fuentes restauradas y comparadas con el manifiesto. ASI normal instalado solo en copia de pruebas: e5b8d16815fb880a46fe0a23e012d825e3607304ef3429f7a248a3ad03192a67; respaldo _baseline/before-general-20260923. Partidas sin cambios, instalación personal intacta.

hooks.csv actualizado solo en locked de la fila float tras validar; ReversedClasses.md regenerado y eliminado el candado de CGeneral. README y hash de trazabilidad de ReversingPriorities actualizados. La guía conserva la actualización anterior completa: 97 clases pendientes, 551 funciones y Top 10 renovado, no volver a introducir clases ya cerradas. Sin commit/push. Campaña integral aplazada y ULTIMA_EJECUCION.md histórico.


## Continuidad: inicio normal, cierre de audio y documentación en español (2026-09-23)

El usuario pidió traducir todos los MD y que GTA arranque desde el principio, sin carga automática. Se revisaron los 38 Markdown propios existentes, incluidos los ocultos en .github; seis documentos en inglés fueron traducidos. Se corrigieron acentos dañados, manteniendo símbolos, comandos y documentación histórica identificada. Se añadieron README en español para CAESound e InicioNormal. No se tradujeron cachés o dependencias descargadas.

Ambos gta-reversed.ini de la copia de pruebas tenían FastLoader activo y SaveGameToLoad=2. Ahora Enable=false y SaveGameToLoad=-2; opciones de omitir pantallas en false. Se conservó Misc.SaveDirectoryPath. Respaldos de INI y del log del cierre en reversing/InicioNormal/artifacts. El comando normal es Set-Location a la copia de pruebas y & '.\gta_sa.exe', sin argumentos ni exclusiones.

runtime_runner.py ahora respalda y restaura ambos INI byte por byte: FastLoader y ranura 2 solo se activan temporalmente durante bancos. La restauración se verificó en una reproducción fallida y en el banco aprobado (configs_restored=true). No dejar FastLoader activado permanentemente otra vez. No añadir nuevos CMD.

Cierre registrado a las 00:36:43: acceso inválido 0xC0000005 en CAESound::UpdateParameters:252, lectura 0xDDDDDE0D. El usuario indicó un choque previo. Reproducción dentro del motor confirmó que la asignación implícita de CAESound copia EntityRef sin registrar el destino ni retirar la referencia anterior; ResolveReferences deja el destino colgando. Se implementó operator= explícito, con referencia registrada, autoasignación y preservación de m_IsAudioHardwareAware/relleno como el original 0x4EF680. La reparación no modifica globalmente EntityRef.

1.024 pares exactos aprobados con Audio activo desde arranque; solo SurfaceInfos excluido en este banco. Caso sound-copy integrado en el único PROBAR.cmd y --all. README y validation-summary.json en reversing/CAESound. Evidencia: D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-sound-copy\20260923-004932-410885. La reproducción fallida quedó en 20260923-004623-831808. No se revalidó toda la clase ni se reprodujo exactamente el mismo choque. Los errores de asignación CEntryExit del log siguen como incidencia separada.

ASI normal instalado solo en copia de pruebas: 08ac3949fc40b466eac6d61c600a4df6987247e692a43d295824f9c473b7668d; respaldo _baseline/before-sound-copy-20260923. Después se abrió sin exclusiones y se comprobó por lectura de gGameState que llegó al menú en 27,32 s, permaneció allí diez segundos y no cargó partida. Estados observados 0,1,2,3,5,7; sin revisión visual de vídeos ni sesión prolongada. Evidencia en reversing/InicioNormal/validation-summary.json.

Partidas e instalación personal conservadas. No se cambiaron flags del CSV para CAESound: operator= ya estaba habilitado y registrado. Los cambios previos de CGeneral y documentos permanecen. Sin commit/push. Campaña integral sigue aplazada; los bancos históricos archivados no prueban conjuntamente todos los cambios del ASI actual.


## Continuidad: diez candidatos completados y arranque SDL (2026-09-23)

Petición completada: CTaskComplexGoToPointShooting 3/3, CScriptResourceManager 4/4, CTaskComplexLeaveCarAndWander 8/8, CTaskComplexWaitForBus 7/7, CPopCycle 15/15, CCarCtrl 22/22, CTaskAllocatorKillThreatsBasic 6/6, FxPrimBP_c 2/2, CEntityScanner 4/4 y CTaskComplexDragPedFromCar 2/2. **No volver a tratarlas como pendientes.** Las 22 nuevas entradas están habilitadas y sin bloqueo tras comparación con el original dentro del motor.

Cuatro bancos finales, todos aprobados: ShootingResources 18.432 pares, LeaveCarBus 5.632, PopulationFactory 18.432 y ScannerDragFx 9.216. Total **51.712 pares exactos sobre 24 funciones**, incluidos 2.048 Initialise de recursos y 2.048 constructor Aiming. No sumar las pasadas iniciales como cobertura nueva. Aiming continúa 5/8; CQuadBike permanece 11/12 y ProcessControl sin validar/deshabilitado. RpAnimBlend sigue 43/43 con su evidencia anterior.

[Informe conjunto](Top10/README.md) y [resumen verificable](Top10/validation-summary.json). Cada banco conserva README, fuentes, ejecutor y validation-summary con rutas/hashes. Los artefactos extensos, sondas y logs quedan ignorados. Los casos shooting-resources, leave-car-bus, population-factory y scanner-drag-fx se integraron en cases.json y run_all.py; un solo PROBAR.cmd. Para ejecución autónoma usar python reversing/run.py --case ID.

Hallazgos: Shooting::Clone construye Aiming y luego instala la vtable Shooting; se corrigió la copia de *this. Aiming conserva un indicador si la memoria reutilizada ya coincide. Los recursos almacenan tipo uint16 y conservan relleno; Add usa última ranura libre, Remove última coincidencia propia y devuelve según otros propietarios. LeaveCarAndWander asigna Wander también si el default es nulo. PopCycle usa rand/32768 y límites estrictos. La fábrica elige automóvil por defecto y activa caballete de moto/BMX; conserva el fallo original de asignación a 0x614 en esas dos ramas. Amenazas usa selección global 8×8, desempates y fallback exactos.

EntityScanner necesita registro virtual con vtable exportada; el registro simple anterior apuntaba al thunk. FxPrimBP::Load usa adaptador no virtual con nombre Load por la misma razón. Se compararon sectores, temporizadores, orden, referencias, ocupantes, flags, versiones FX, matriz, texturas y posición del archivo. Los intentos iniciales con diferencias, thunk recursivo y captura de excepción bloqueada quedaron archivados; todos fueron restaurados y los defectos detectados se resolvieron antes de aprobar.

Límites: bancos con Audio y SurfaceInfos excluidos, dependencias controladas documentadas, sin rutas completas, combate prolongado, todas las animaciones, constructores completos de vehículos ni render exhaustivo. PopulationFactory captura solo la excepción nativa esperada de 0x614 con instrumentación temporal del manejador; el manejador normal fue restaurado. RNG/tablas/hooks/referencias y fuentes restaurados, verificados contra manifiestos. FastLoader solicita ranura 2 temporalmente; el log rechaza su versión y empieza partida nueva. No forzar carga ni alterar el guardado. Campaña integral continúa aplazada.

Después de los bancos, el arranque normal agotó dos veces el plazo en el logo. La segunda lectura descartó pérdida de foco. SDLWrapper no reenviaba WM_GRAPHNOTIFY al manejador de vídeo; se añadió un callback para la ventana del juego y estados 2/4 que llama al OnGraphNotify existente. Este cambio de integración posterior no alteró fuentes comparadas ni filas adicionales del CSV. Las sondas finales preceden a ese callback. Arranque normal final validado aparte, sin argumentos/exclusiones/teclas: estados 0, 1, 2, 3, 4, 5, 7, menú a los 130.49 s y diez segundos estable; sin carga automática. Ver InicioNormal/validation-summary.json. Su ejecutor ahora registra foco y extrae solamente el log nuevo.

ASI normal Debug x86 final, sin instrumentación: `4bfe388ecf97ee2ad8c7d4a296bdc2d720e69d29f7fda000312890f5276a9805`, instalado solo en copia de pruebas. Respaldo inmediato `_baseline/before-top10-sdl-20260923`; respaldo del ASI CAESound anterior `_baseline/before-top10-20260923`. Partidas e INI conservados por hash, instalación personal intacta. El comando directo a gta_sa.exe sigue mostrando inicio y menú.

hooks.csv actualizado solo en las 22 filas validadas (Scanner pasa a tipo virtual); ReversedClasses regenerado, README y ReversingPriorities actualizados. **614 completas, 85 parciales, 2 sin revertir; 7.485/8.014 entradas y 529 pendientes.** La guía contiene 87 clases incompletas y nuevo Top 10 intermedio de 28 pendientes; no volver a introducir clases cerradas. Comprobados 734 enlaces locales, coherencia del catálogo, totales y diff sin errores de espacios. ULTIMA_EJECUCION.md sigue siendo histórico; no se repitió toda la suite.

Rama ReSA. Cambios previos conservados. Sin commit ni push.




## Continuidad: desbloqueos terminados y plataforma conmutable (2026-09-23)

Petición completada: **24 candados retirados en 16 clases/namespaces**. CGeneral, número 17 de la lista, ya estaba desbloqueado; no repetir sus 65.536 pares ni tratarlo como pendiente. El usuario autorizó rehacer SDL/ImGui para poder conmutar los siete hooks de CPad, CMenuManager, Ps y Win; ya no conservan esos bloqueos. RpAnimBlend sigue 43/43 con su evidencia anterior.

[Informe conjunto](Unlocks/README.md) y [resumen verificable](Unlocks/validation-summary.json). Bancos finales: Core 7.968, Tasks 7.168, Physical 3.072, Audio 9.216, Radio 224, Driving 16.640 e Input 6.144: **50.432 pares**. Plataforma: nueve arranques, **324 pares repetidos y 18 comprobaciones de entrada**. Los primeros siete bancos y cada modo final verifican los estados de hooks. No sumar las pasadas preliminares como cobertura nueva.

Core corrige ABI uint8/x87 de CCover, la entrada New del pool antes registrada como GetAt, la llamada thiscall de RemoveDecisionMaker y el hook duplicado de VehicleRecording::Render. TestCurves y Render son retornos vacíos nativos; las pruebas extensas de curvas se conservan como RunLocalTests. EntryInfoNode conserva cursor/referencias/free del pool original sin cambiar globalmente todos los pools.

Tasks implementa CreateSubTask de DestroyCarArmed y ProcessPed de ThrowControl. SeekEntityStandard::Destructor pasa de 0x46AD50 (wrapper scalar deleting) a 0x46AD70; adaptador Constructor con intervalos en orden correcto. Esta ABI incorrecta provocaba el fallo ESP. Physical conserva listas, pools, sectores negativos y aritmética x87. AudioEnvironment recibe buffer de salida sin prometer EAX, y se revalidó también su adaptador Placeable. Driving conserva ECX y EDX a través de GetVehicleAppearance mediante adaptador y 256 casos ABI adicionales.

Radio usa vector nativo de punteros y CRT compatible, orden de argumentos correcto, enlaces de archivo/carpeta y propiedad de memoria reparados. Mantiene mejoras Unicode y recarga inmediata, documentadas y comprobadas como contratos propios; no se afirma identidad binaria de archivos. Fixtures privados reproducibles mediante UnlockRadio/prepare_fixtures.ps1; no música personal. Audio activo en Audio y Radio; los demás bancos excluyen Audio y SurfaceInfos.

Input conserva la ABI y las máscaras 0x80 del ratón, DirectInput bajo SDL, adquisición/lectura, ejes x87, POV, zona muerta y padNumber. Plataforma mantiene PsGlobal nativo de 40 bytes, HWND compartido con SDL y bombeo independiente mediante temporizador. WM_GRAPHNOTIFY llega al vídeo y la captura de ImGui no requiere bloquear hooks. Matriz 127, 0, 126, 125, 123, 119, 111, 95 y 63 aprobada; no son todas las 128 combinaciones.

La aserción RwCameraGetRaster(camera) se reprodujo con modo solicitado 1920×1080 y cliente real 1920×1055. WindowedMode ahora asigna RsGlobal.maximumWidth/Height después de conocer el backbuffer final. Se mantiene la aserción, no se oculta. Las sondas capturan RTC/aserciones para evitar diálogos detenidos. La captura temprana final de plataforma deja actuar a SEH local y solo termina por excepciones no manejadas; un intento anterior detenía primera oportunidad durante Direct3D y quedó fallido. El manejador normal deja continuar la búsqueda y evita repetir indefinidamente la instrucción fallida. Los fallos previos siguen archivados.

ASI normal final sin instrumentación: **112cdb045c60ce0a3ca3114f77fc321e8482f569fd7d5458bdf3ce6b225208e3**. Instalado exclusivamente en la copia de pruebas; respaldo `_baseline/before-unlocks-20260923-122454`. Fuentes de las funciones comparadas coinciden con sus manifiestos. Después de la matriz solo se quitaron líneas vacías finales en WinInput.cpp/.h, con hashes antes/después registrados; se recompiló normal.

Inicio normal final sin argumentos/exclusiones/teclas: estados muestreados 0,1,2,4,5,7, menú a los **68,89 s**, diez segundos estable, sin carga automática. Evidencia InicioNormal/artifacts/startup-20260923-122459. Observación conjunta del mismo ASI: **30 s en IDLE**, 863 frames de avance, sin exclusiones y Audio activo; Unlocks/artifacts/current-20260923-122728. No simula conducción, combate ni sesión prolongada. Partidas e INI conservados por hash; instalación personal intacta.

Los ocho casos unlock-* se integraron en cases.json y run_all.py. Un solo PROBAR.cmd; matriz en UnlockPlatform/run_matrix.py y comprobación breve del ASI instalado en Unlocks/run_current_build.py. Los bancos son autónomos, con plazos y restauración; runtime_runner reintenta brevemente la copia si Windows tarda en liberar el ASI. FastLoader y ranura 2 solo temporales; al terminar ambos INI siguen Enable=false y SaveGameToLoad=-2. La ranura incompatible no se fuerza ni se altera.

hooks.csv actualizado solo después de validar: 24 flags locked y dos correcciones de registro (pool y destructor). ReversedClasses regenerado; README, guía de prioridades, InicioNormal y documentación de bancos actualizados. Totales intactos: **614 completas, 85 parciales, 2 sin revertir; 7.485/8.014 entradas, 529 pendientes**. Quedan otros 630 candados fuera de esta petición. La guía conserva sus 87 clases pendientes; no reintroducir clases completadas. ULTIMA_EJECUCION sigue siendo histórico.

Rama ReSA, cambios previos conservados, sin commit ni push. La campaña integral sigue aplazada hasta completar las clases; las sondas históricas no prueban conjuntamente todo el ASI actual. No se garantiza estabilidad general. No hay orden vigente de apagar el equipo: el apagado pedido para la tarea anterior no es permanente.

Revisión final de logs: avisos de Acquire antes de mostrar ventana, timecyc y rechazo del guardado permanecen documentados. En la ejecución conjunta, el driver Intel/D3D9On12 generó una excepción de primera oportunidad; el motor continuó 863 frames. El rótulo legado UNHANDLED EXCEPTION lo escribe el VEH antes de los manejadores SEH y no prueba un cierre por sí solo. No atribuir esa línea a un nuevo fallo terminal sin comprobar salida del proceso y continuidad.

Revisión documental posterior a petición del usuario: inventario de 54 Markdown propios, nueve documentos corregidos. CVehicle ya no figura pendiente de habilitación/prueba; los ASI de Top10, CAESound, CGeneral y ClothesScriptedSprint se identifican como históricos. ULTIMA_EJECUCION enlaza la tanda vigente sin inventar otra pasada de la suite. Actualizados formato del catálogo y recuento documental; 17 clases solicitadas sin candados, 630 bloqueos fuera de la petición. Verificados 813 enlaces locales y concordancia del catálogo generado y la guía con hooks.csv. Solo documentación; sin cambios de CSV, código, binarios ni partidas, y sin repetir GTA. Registro local: artifacts/docs-audit-20260923.json. Sin commit/push.




## Continuidad: segundo lote de desbloqueos en curso (2026-09-23)

Petición vigente: CCollision 59, CFileMgr 13, CTaskComplexGangLeader 11, CAEDataStream 6, CTaskComplexTrackEntity 4 y Game 3: **96 bloqueos reales** inventariados en UnlockBatch2/artifacts/initial-state.json. No confundir este lote con los 24 candados del informe anterior.

CAEDataStream ya tiene sus seis candados retirados tras 6.144 comparaciones y 64 combinaciones aprobadas. Pasada final: resultados-unlock-data-stream/20260923-125257-865113; [detalle](UnlockDataStream/README.md). HANDLE nativo sustituye FILE* incompatible, con Stat adaptado. Audio activo; SurfaceInfos excluido. ASI de prueba final 2ed02de894c2bf0dd1eb325d687705b049c6867b228a8147e77988e783bffb54. La versión normal instalada sigue siendo 112cdb045c60ce0a3ca3114f77fc321e8482f569fd7d5458bdf3ce6b225208e3, restaurada automáticamente; partidas/INI conservados.

**Trabajo restante: 90 candados.** TrackEntity está en reparación, aún sin validar: dos helpers eran passthrough y el temporizador sumaba start+start. El original ControlSubTask conserva ECX tras GoToPointFine::SetTargetPos; ese contrato exige adaptador y también comprobar sus flags. CFileMgr mezcla runtimes FILE incompatibles. GangLeader tiene lógica/transiciones pendientes de auditar. Game requiere separar la integración de interfaz del camino conmutable. Collision necesita banco y revisión de ABI/rendimiento, no quitar flags a ciegas.

CSV y catálogo siguen sin actualizar en esta tanda. No recompilar/instalar normal hasta validar las modificaciones candidatas; continuar el trabajo técnico y sincronizar documentación al cerrar. Sin commit/push, sin apagar, instalación personal intacta.


Avance posterior del segundo lote: TrackEntity **4/4 desbloqueados**, 9.216 pares finales aprobados en resultados-unlock-track-entity/20260923-131036-501962. Ver UnlockTrackEntity/README.md. ABI ECX y máscara del setter reparadas; temporizador y rama FollowNodeRoute contrastados. Total de esta tanda validado en fuentes: 10/96; quedan 86 candados. Catálogos aún sin sincronizar.

CFileMgr está en reparación y pruebas (13 candados conservados): FILESTREAM ahora es un tipo opaco del CRT del ejecutable; consola mantiene FILE del DLL. Menú/estadísticas usan lectura/formato seguidos por CFileMgr; los demás consumidores FILE* se adaptaron al tipo. Un puente acotado por thread_local en CreateFileA permite a fopen nativo abrir UTF-8 mediante CreateFileW. LoadFile conserva extensión acotada con terminador solo si cabe. La primera comparación encontró relleno 0xFE de strcpy_s en SetDirMyDocuments: corregido a cola cero; siguiente pasada pendiente. No instalar este candidato como normal.

Game está en preparación: se movió DrawLoop a RenderBeforePresent en RwCameraShowRaster, con cámara principal, estados 7/9 y escena D3D abierta; aún sin validar y sus tres candados siguen. Collision/GangLeader siguen pendientes. No afirmar cierre del lote.

Git HEAD actual b7b3dc4 (commit externo durante la compilación inicial) capturó instrumentación temporal de DataStream. El árbol de trabajo ya restaura esas fuentes sin sonda; conservar esa limpieza y no revertirla al HEAD instrumentado. El agente no hizo commit ni push.


### Detenido a petición del usuario (2026-09-23)

El usuario pidió detener el trabajo y dar estado. No continuar automáticamente. Estado final de esta pausa: **10/96 candados retirados y validados en fuentes**, CAEDataStream 6 y TrackEntity 4; 15.360 pares finales entre ambos bancos y 64 combinaciones de DataStream. No sumar pasadas preliminares.

CFileMgr conserva 13 candados y cambios candidatos sin aprobación final. Última pasada fallida: resultados-unlock-file-mgr/20260923-131422-691585, LoadFile caso 1: original devuelve 0 y candidato 256; causa pendiente de investigar. La diferencia de cola del directorio ya fue corregida. No afirmar equivalencia de LoadFile ni cierre del banco. Game conserva 3 candados: traslado de ImGui a presentación escrito, todavía sin compilar/validar. Collision 59 y GangLeader 11 siguen sin cambios de implementación en esta tanda; solo inspección. Total pendiente: **86**.

No hay procesos GTA/MSBuild/cl/link activos al detener. ASI normal instalado comprobado: 112cdb045c60ce0a3ca3114f77fc321e8482f569fd7d5458bdf3ce6b225208e3; la sonda fue retirada. Fuentes sin instrumentación temporal. bin/Debug puede contener la última sonda: no instalarlo directamente. CSV, ReversedClasses y prioridades no sincronizados para este lote; faltan validación del resto, documentación final, compilación normal e integración conjunta. No commit/push del agente, no apagado, instalación personal y partidas conservadas.


<a id="continuidad-entrega-parcial"></a>

## Continuidad histórica: entrega parcial cerrada; 86 bloqueos aplazados (2026-09-23)

El usuario acotó la tarea a sincronizar catálogos, compilar normal y comprobar la integración de lo ya validado. **Completado:** CAEDataStream 6 candados y TrackEntity 4, con su dependencia GoToPointFine::SetTargetPos reparada. Total: 15.360 pares finales y 64 combinaciones de DataStream. TrackEntity se recompiló sin los borradores y repitió 9.216 pares: resultados-unlock-track-entity/20260923-132113-463503. Las repeticiones no aumentan la cobertura única.

[Informe actual](UnlockBatch2/README.md) y [resumen verificable](UnlockBatch2/validation-summary.json). Los 15 archivos candidatos de CFileMgr y Game se archivaron y separaron de las fuentes compilables: UnlockBatch2/deferred-filemgr-game.patch, deferred-manifest.json y copias exactas en artifacts/deferred-20260923. git apply --check aprobado sobre la base actual. No recuperarlos automáticamente ni incorporarlos al ASI normal. CFileMgr mantiene su diferencia LoadFile pendiente; Game aún no tiene validado el cambio de presentación. Collision/GangLeader solo inspeccionados. **Quedan para después CCollision 59, CFileMgr 13, GangLeader 11 y Game 3: 86.**

CSV actualizado exclusivamente en diez flags locked después de validar. ReversedClasses regenerado, README/prioridades/índice de pruebas/InicioNormal/documentación de bancos sincronizados. Añadidos unlock-data-stream y unlock-track-entity a cases.json y run_all.py: 38 casos, un solo PROBAR.cmd. Totales de progreso intactos: 614 completas, 85 parciales, 2 sin revertir; 7.485/8.014 entradas y 529 pendientes. Quedan **620 candados globales**. RpAnimBlend conserva 43/43 y su evidencia previa. La suite histórica completa no se repitió.

ASI normal Debug x86 final, sin instrumentación: **16f5542f9ff8fc2e84d8adb31caddccbb29cecb8ca633076efda5c51df0600b9**. PDB cf09026d3fd7f169f9d36c3ae235ab91efd7fed389f5df6d4ce38a80e27cf3d9. bin/Debug y copia de pruebas contienen la versión normal. Respaldo anterior en `_baseline/before-unlock-batch2-20260923-132316`. Fuentes validadas coinciden con manifiestos; fuentes diferidas coinciden con base. No restaurar globalmente HEAD b7b3dc4: ese commit externo capturó instrumentación temporal de DataStream y el árbol actual conserva su limpieza.

Inicio normal sin argumentos/exclusiones/teclas: estados muestreados 0,1,2,5,7; menú a **30,82 s**, diez segundos estable, sin carga automática. Evidencia InicioNormal/artifacts/startup-20260923-132328. Integración del mismo ASI: **30,03 s en IDLE, 873 frames**, Audio activo, sin exclusiones. Evidencia Unlocks/artifacts/current-20260923-132450. Proceso cerrado automáticamente al terminar. INI restaurados y partidas conservadas por hash; instalación personal intacta. FastLoader=false/SaveGameToLoad=-2 para uso normal.

Limitaciones: el log conjunto registró 13 fallos de asignación de CEntryExit; el motor siguió funcionando. No se investigó ni atribuyó su causa. Persisten aviso timecyc y rechazo de la ranura 2 incompatible; no forzar su carga. El arranque mostró un aviso de adquisición del ratón. No hubo cierre inesperado, aserción ni RTC observado en las comprobaciones finales. No se probó conducción, combate, carga/guardado completo ni sesión prolongada; no garantiza estabilidad general. Campaña integral sigue aplazada. Rama ReSA, sin commit/push del agente, sin apagar. Esperar otra petición antes de continuar los 86 pendientes.


<a id="continuidad-entryexit"></a>

## Continuidad histórica: reserva de CEntryExit reparada (2026-09-23)

Petición completada: investigar los fallos de asignación de CEntryExit. Git comenzó limpio en ReSA, HEAD cba65c3, posterior a la entrega parcial. No recuperar los borradores de CFileMgr/Game: siguen aplazados los mismos 86 candados.

Traza: 444 altas, 25 bajas y 19 fallos con 400 plazas; ninguna búsqueda de una baja falló. Banco con pools privados: el original y el candidato fallan en las mismas 19 altas con 400, y aceptan todas con 512 (419 entradas activas). Se cambió únicamente la reserva inicial de CEntryExitManager::Init a 512; 6.832 bytes adicionales. Ampliación deliberada del límite original, no equivalencia de Init ni una corrección general del pool. Los objetos no se mueven después de asignarse.

[Informe](CEntryExitPool/README.md) y [resumen verificable](CEntryExitPool/validation-summary.json). Banco final resultados-entryexit-pool/20260923-141006-579167: 1.854 comprobaciones, 1.833 pares normalizados iguales y 21 retornos distintos esperados de FIX_BUGS (-1 frente al 0 original). También vacía y reutiliza las 512 plazas. Índices/generaciones y padding normalizados; no prueba puertas rotadas, enlaces ni todas las funciones de CEntryExit. Sonda 12a1673274333d01b296e4ec83e1d567180e494222d8193a0dd87acc696dba11; caso entryexit-pool añadido al único PROBAR.cmd y --all. Total 39 casos, 38 de motor y uno aislado. CSV y ReversedClasses sin cambios: ningún estado de hook varió, continúan 614/85/2 clases y 620 candados.

ASI normal Debug x86 instalado solo en la copia de pruebas y presente en bin/Debug: **f01faea570319f6f3f7b5a7278ae7b80e3fc27507e478fa367a811b677fdaa93**; PDB 0e77e137a91dc56f9390013ed991cdd7a3a3709f075b80ba0f8a49b6105587f6. Respaldo `_baseline/before-entryexit-pool-20260923-141213`. Fuentes sin instrumentación y hashes verificados contra la sonda. Integración sin exclusiones: **120,24 segundos, 3.404 fotogramas, cero fallos de CEntryExit**, máximo 394/512. Evidencia Unlocks/artifacts/current-20260923-141219. La pasada natural no alcanzó 400; el desbordamiento y las plazas extra se prueban en el banco determinista.

INI restaurados (FastLoader=false, SaveGameToLoad=-2), partidas sin cambios por hash, instalación personal intacta. No se repitió el banco de llegada al menú, ni conducción o guardar/cargar completo. La capacidad sigue siendo finita; no se garantiza compatibilidad de guardados nuevos con índices superiores a 399 en un motor limitado a 400. El log conserva avisos de datos, ranura incompatible, cinco de IK y 18 de peatones fuera del mapa; no se investigaron y no se afirma estabilidad general. Ya había un aviso de peatón en una observación del ASI anterior. No hubo cierre inesperado, aserción o RTC en la prueba final. Campaña integral y 86 desbloqueos aplazados. Sin commit/push, sin apagado. Esperar otra petición.


Revisión documental posterior: se inspeccionaron los 59 Markdown propios del repositorio y se corrigieron ocho documentos con referencias vigentes desactualizadas o ambiguas. CAESound ya enlaza el diagnóstico de CEntryExit; ULTIMA_EJECUCION apunta a la reparación reciente; los informes históricos remiten al ancla estable de continuidad. Se comprobaron 855 enlaces locales, sin rutas rotas. Inventario y resultado en CEntryExitPool/artifacts/docs-inventory.json. Esta revisión solo modificó documentación; no recompiló, no repitió GTA ni cambió CSV, binarios o partidas. Sin commit/push.

<a id="continuidad-peatones"></a>

## Continuidad histórica: desplazamiento de peatones por animación reparado (2026-09-23)

Petición: investigar y reparar los avisos de peatones fuera del mapa. Git comenzó limpio en **ReSA, HEAD 4516c03**, que ya contenía CEntryExit y su documentación. Esta tarea no hizo commit ni push; comprobar el estado real antes de continuar y conservar los cambios nuevos.

Los avisos anteriores tenían coordenadas compatibles con `0xCDCDCDCD`. Una traza registró 74 saltos de posición y 14 avisos; otra acotó 39 saltos a `CPedIntelligence::Process` → `CTaskManager::ManageTasks` (117 registros al observar los mismos saltos en tres niveles, no 117 sucesos independientes). Las pasadas naturales son variables y dos no reprodujeron. No se identificó el nombre concreto de la animación de los 18 avisos históricos.

El desensamblado y el banco determinista identificaron tres inicializaciones omitidas en el constructor con parámetros de **CTaskSimpleRunNamedAnim, 0x61A990**: vector `m_vecOffsetAtEnd` a cero, bandera `m_bOffsetAvailable` desactivada y `m_pAnimHierarchy` nulo si falta el bloque. `ProcessPed` original consulta primero esa bandera y aplica el desplazamiento; con memoria 0xCD, el constructor anterior provocó exactamente el salto de coordenadas en el banco. Solo se repararon esas inicializaciones; no se alteraron el constructor predeterminado, la función de desplazamiento ni los avisos de CWorld.

[Informe](FallenPeds/README.md) y [resumen verificable](FallenPeds/validation-summary.json). **8.832 pares aprobados:** 8.192 constructores, 128 ejecuciones controladas de ProcessPed original y 512 aplicaciones de OffsetPedPosition. Sin diferencias en los resultados comparados, error máximo de posición 0. El control anterior registró 8.160 diferencias de inicialización y 64 de posición; las 512 aplicaciones de desplazamiento ya coincidían. Campos semánticos, objetos independientes y clump privado vacío; se excluyen vtable, padding, colas de cadenas y el identificador no inicializado por el original. No prueba todas las animaciones ni cierra la clase.

Banco corregido: `resultados-namedanim-init/20260923-150054-230081`. Sonda SHA-256 `d2096562927cb082cfce314b1419ed81343f15cbc9b4178cd1a73636449bcdfc`. Repetición: `python reversing/run.py --case namedanim-init`. Integrado en el único PROBAR.cmd y `--all`: **40 casos, 39 dentro del motor y uno aislado**. No se repitió toda la suite histórica. CSV y ReversedClasses intactos: no cambiaron estados de hooks ni porcentajes.

**ASI normal instalado solo en la copia de pruebas y presente en bin/Debug:** `6835a6eebc877f597a0d1cffe3992b7e8889cc0ddbff80446b4928d7978cd311`; PDB `9276a2b633756577980ed15fe390220a80ce7bd03be8886cd15029d95d8bdd3f`. Sin instrumentación, fuentes verificadas contra el manifiesto del banco. Respaldo `_baseline/before-fallen-peds-20260923-150204`.

Integración final sin exclusiones: **120,64 segundos, 3.609 fotogramas, cero avisos de peatones y cero fallos de CEntryExit**, máximo 402/512 entradas. Evidencia `Unlocks/artifacts/current-20260923-150550`. La primera pasada solo avanzó 815 fotogramas; se repitió exigiendo 3.600. Comando: `python reversing/Unlocks/run_current_build.py --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600`. No hubo cierre inesperado, aserción ni RTC. INI restaurados (`FastLoader=false`, `SaveGameToLoad=-2`), partidas conservadas por hash, instalación personal intacta y GTA cerrado al terminar.

El log final mantiene avisos de timecyc/skimmer; no apareció IK en esta muestra, pero no se investigó ni reparó. No se certifican otras causas posibles de peatones fuera del mapa, conducción, combate, guardar/cargar completo ni estabilidad general. CEntryExit conserva sus 512 plazas y su banco de 1.854 comprobaciones (1.833 resultados normalizados iguales y 21 diferencias esperadas de FIX_BUGS); RpAnimBlend sigue 43/43. **Siguen aplazados los mismos 86 bloqueos: CCollision 59, CFileMgr 13, GangLeader 11 y Game 3.** No aplicar el parche diferido sin petición. Campaña integral aplazada; sin apagado.

<a id="continuidad-ik-datos"></a>

## Continuidad histórica: aviso del pivote IK y datos reparados (2026-09-23)

El usuario pidió reparar IK/datos y revisar el debug adjunto de las 15:02. Rama **ReSA**, HEAD **4516c033c3e8e205af47aac18c9ca4f58f323b68**. Se conservaron todos los cambios existentes de peatones y su documentación; no hubo commit ni push. Comprobar Git real antes de continuar.

**IKChain_c::SetupBones:** evita buscar como padre interno el pivote excluido deliberadamente de la cadena. El original también omite ese enlace; la reconstrucción emitía un aviso falso, incluido el caso cabeza/cuello (hueso 4). Se mantiene el diagnóstico para otras búsquedas fallidas. No se modifica el solver ni se completa la clase.

**Datos, solo copia de pruebas:** `timecyc.dat` tenía 183 filas de 51 campos y una de 49; ambos lectores solicitan 52. Se completa la fila defectuosa con ambiente `255 255 255` y se añade multiplicador neutro `1.0` a las 184 filas. Es una reparación explícita del dato indeterminado; no se conocen con certeza los valores ambiente perdidos. `vehicles.ide` explicita los valores predeterminados actuales de skimmer `-1, 0.7, 0.7`. `cargrp.dat` conserva los primeros 23 modelos de los grupos 6/7 y elimina únicamente los 3/6 sobrantes ignorados por el motor. Su aviso de «no memory» no indicaba falta de RAM. Lectores CTimeCycle/CFileLoader/Population intactos. Respaldo exacto `_baseline/before-ik-data-20260923-152119`; [manifiesto de datos](IKDataWarnings/data-repair.json). El reparador es idempotente, comprueba hashes y exige GTA cerrado para escribir.

[Informe](IKDataWarnings/README.md) y [resumen verificable](IKDataWarnings/validation-summary.json). **131 pares únicos de SetupBones aprobados**, comparando orden, etiquetas, padres, orientación, límites y velocidad; contadores original 0/candidato 1. Jugador, gestor de nodos, frames y keyframes intactos; devolución de nodos comprobada. No se comparan padding, direcciones ni matrices aún sin calcular. **9.384 bytes de tablas de clima coincidentes** usando los datos corregidos; no son 9.384 casos independientes. Cantidades y entradas activas de grupos de coches iguales al original; skimmer carga los valores explícitos esperados. No se valida toda la clase IK ni todo LoadVehicleObject.

Banco final `resultados-ik-data/20260923-152556-990651`, sonda ASI `218564db47286769690da2734e5b34e9f2b6c0fecb61e4a5007fec4b9edf91f2`. La primera pasada también aprobó 131 pares; no se suma como cobertura nueva. Repetición: `python reversing/run.py --case ik-data`. Único PROBAR.cmd reutilizado; **41 casos, 40 de motor y uno aislado**. CSV y ReversedClasses intactos, sin cambios en indicadores ni porcentajes. No se ejecutó la suite completa.

**ASI normal instalado y presente en bin/Debug:** `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`. Sin sonda; fuentes normales verificadas contra el manifiesto. Respaldo anterior `_baseline/before-ik-normal-20260923-152717`.

Integración sin exclusiones: **121,16 segundos, 3.605 fotogramas**, cero avisos de IK/timecyc/argumentos de vehículos/capacidad de grupos/peatones y cero fallos de CEntryExit; máximo 403/512. Evidencia `Unlocks/artifacts/current-20260923-152730`. Comando: `python reversing/Unlocks/run_current_build.py --ik-data --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600`. INI restaurados (`FastLoader=false`, `SaveGameToLoad=-2`), partidas sin cambios por hash, instalación personal intacta y proceso propio cerrado.

**Otros mensajes:** persiste `Acquire` del ratón con HRESULT 0x80070005 al arrancar; existe reintento en CPad, pero esta prueba no verificó su recuperación. La carga automática temporal intenta la ranura 2 `v1 Modified`; el motor espera `v1 Unmodified`, rechaza la carga y comienza partida nueva. No forzar compatibilidad ni modificar partidas. El adjunto no contenía avisos IK/peatones y pertenecía a la primera integración de 815 fotogramas, no a la posterior de 3.609.

No se observó cierre inesperado, aserción ni RTC en la integración final. No se probaron conducción, combate, guardar/cargar completo, todos los esqueletos ni sesiones prolongadas; no se garantiza estabilidad general. CEntryExit mantiene 512 plazas y su evidencia de 1.854 comprobaciones: 1.833 resultados normalizados iguales y 21 diferencias esperadas FIX_BUGS. RpAnimBlend sigue 43/43. Permanecen aplazados **CCollision 59, CFileMgr 13, GangLeader 11 y Game 3: 86 bloqueos**; no aplicar el parche diferido sin petición. Sin apagado.

<a id="continuidad-raton"></a>

## Continuidad histórica: recuperación del ratón comprobada y revisión de Markdown (2026-09-23)

Petición: comprobar el fallo inicial de adquisición del ratón y actualizar todos los Markdown. Git revisado en **ReSA**, HEAD **4516c033c3e8e205af47aac18c9ca4f58f323b68**, conservando los cambios existentes de peatones e IK/datos. Sin commit ni push.

**Diagnóstico confirmado:** una sonda temporal observa las llamadas reales, sin añadir Acquire/GetDeviceState ni inyectar movimiento/clics. El primer Acquire devuelve `0x80070005` con la ventana oculta y otra ventana en primer plano. A los **1,485 segundos**, con la ventana visible y con foco, la primera lectura falla (`0x8007000C`), el reintento de adquisición da `S_OK` y la segunda lectura también, dentro de la misma llamada. Se registran **120 lecturas correctas y 120 actualizaciones de estado**; termina a los 15,110 s desde el primer intento. Las inicializaciones posteriores también adquieren el dispositivo. No hizo falta cambiar el código normal ni ocultar el aviso. Esto demuestra recuperación en esta ejecución, no retrospectivamente en todas las sesiones anteriores.

[Informe](MouseRecovery/README.md) y [resumen verificable](MouseRecovery/validation-summary.json). Evidencia `resultados-mouse-recovery/20260923-154347-737501`. Sonda ASI `c09c409dc44c30ade89ed3237dccbf2a67ea9210c436fa89e9616439901adccc`; registro SHA-256 `81c7768545e71b7bfb2038f8d93772763076105720eeb1b51295857bc9ef0e2f`. Repetición: `python reversing/MouseRecovery/build_probe.py`, después `python reversing/MouseRecovery/run_local.py`. Es observación del dispositivo real, no pares de equivalencia. El catálogo conserva **41 casos, 40 de motor y uno aislado**; único PROBAR.cmd intacto.

**ASI normal sin cambios:** `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`, tanto en bin/Debug como en la copia de pruebas. Fuentes, binarios normales e INI restaurados por hash. Partidas conservadas y proceso propio cerrado. FastLoader=false y SaveGameToLoad=-2. Se conservan los tres archivos de datos reparados y sus respaldos; instalación personal intacta.

**Documentación:** revisados **62 Markdown propios**, incluidos `.github` y archivos no rastreados, con cachés/dependencias/artefactos ignorados excluidos. Se actualizan diagnóstico del ratón, estado instalado de InicioNormal, referencias históricas a avisos ya investigados, índices y navegación a continuidad de 40 informes que no la tenían. Las guías técnicas y el catálogo generado conservan sus contenidos válidos. **944 enlaces a archivos locales comprobados, sin destinos inexistentes**; no se verifican URLs remotas ni fragmentos de encabezados. Inventario final en `MouseRecovery/artifacts/docs-audit.json`; no se alteran resultados históricos ni indicadores CSV.

Última integración normal sigue siendo la de IK/datos: **121,16 s y 3.605 fotogramas**, sin avisos comprobados de IK/datos/peatones y sin fallos de CEntryExit. Sus 131 pares de IK y tablas de clima/grupos coincidentes conservan su alcance. El nuevo diagnóstico no vuelve a ejecutar esa integración ni toda la suite. No prueba clics/movimiento físico, hotplug, todos los controladores, conducción, combate, guardar/cargar completo ni estabilidad general. La partida incompatible sigue rechazándose de forma prevista; no forzar carga ni modificar partidas.

**Aplazados sin cambios:** CCollision 59, CFileMgr 13, GangLeader 11, Game 3: 86 bloqueos. No aplicar `UnlockBatch2/deferred-filemgr-game.patch` sin petición. RpAnimBlend 43/43; CEntryExit 512 plazas, 1.854 comprobaciones (1.833 resultados normalizados iguales y 21 diferencias esperadas FIX_BUGS). Sin apagado.

<a id="continuidad-independencia-20260923"></a>

## Continuidad histórica: objetivo de independencia del ejecutable (2026-09-23)

Petición documental: orientar ReSA hacia una reconstrucción completa, funcionalmente equivalente y validada, reduciendo hasta eliminar la dependencia de funciones originales; después, construir un ejecutable propio. La [hoja de ruta](../docs/ProjectRoadmap.md) define el orden y los criterios. Sustituye el 100% del Markdown como objetivo final; no invalida los bancos anteriores.

Git comprobado al comenzar: **ReSA**, HEAD **11fd6912f759d3bc93208a9def001b6dd751dbdf**, árbol limpio, sin archivos no rastreados no ignorados. Ese commit posterior a 4516c03 incorpora peatones, IK/datos y documentación del ratón. No asumir que los cambios descritos en continuidades anteriores siguen sin commit. Esta actualización deja cambios documentales y del generador sin commit; comprobar siempre Git real.

Cifras del CSV: **7.485/8.014** entradas reconstruidas (93,40%), **614/701** clases completas (87,59%), 529 pendientes, 87 clases incompletas y 620 locks. Las 7.394 entradas desbloqueadas no son una medida de independencia. El número único de funciones validadas por versión y el de funciones que todavía usan código original **no están consolidados ni medidos**. No inferirlos del número de casos o pares. El panel mantiene estas dimensiones separadas mediante su generador; no se añaden flags ni se modifica el CSV.

Orden estratégico: completar funciones, eliminar locks con evidencia, cerrar las clases registradas, inventariar dependencias originales, consolidar equivalencia reproducible, probar el juego integrado, independizar subsistemas y acreditar ausencia de ejecución de lógica original. Hito posterior a los porcentajes: **ReSA — Zero Original Function Dependency**. El ejecutable propio exige además sustituir inicialización y servicios del host. La validación deberá tener seguimiento separado y vinculado a evidencia, sin rebajar la comparación dentro del motor exigida para nuevas marcas reversed=1.

Se actualizan README, prioridades, índice de pruebas, referencia del plan histórico, panel y proceso. Esta tarea no compila, instala ni ejecuta GTA; no cambia resultados históricos, binarios, configuración o partidas. El estado instalado y las limitaciones siguen siendo los documentados en la [continuidad del ratón](#continuidad-raton): ASI ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f; última integración normal de 121,16 s y 3.605 fotogramas; 41 casos, sin repetir toda la suite ni afirmar estabilidad general.

**Aplazamientos y protección intactos:** CCollision 59, CFileMgr 13, CTaskComplexGangLeader 11 y Game 3 (86); no aplicar el parche diferido sin petición expresa. La campaña integral sigue prevista tras completar las clases. Solo copia de pruebas, instalación personal y partidas conservadas, ranura 2 incompatible sin forzar, FastLoader=false y SaveGameToLoad=-2 para uso normal. Un solo PROBAR.cmd y reutilización de reversing. Esta petición no selecciona una nueva clase ni inicia los hitos técnicos. Sin commit, push ni apagado.
### Ampliación vigente: registro separado de validaciones

El usuario autorizó crear el registro separado y conectarlo al panel. Se añadieron `reversing/validation-registry.json`, `contrib/validation_registry.py`, su documentación en [ValidationRegistry](ValidationRegistry/README.md) y ocho pruebas automáticas. No se añadió una columna validated al CSV ni se cambió ningún flag.

Inventario inicial: tres funciones con resúmenes aprobados vinculados por hash (constructor con parámetros de CTaskSimpleRunNamedAnim, SetupBones y RpAnimBlendClumpUpdateAnimations); las dos primeras conservan los hashes de fuentes registrados. Las tres siguen pendientes de revisión de dependencias para acreditar vigencia completa. El panel muestra **0/8.014 acreditadas con vigencia**, no «cero funciones probadas». No se invalida evidencia histórica ni se clasifica el resto como fallido. RpAnimBlend requiere además recuperar la procedencia de las fuentes de su prueba. No usar el hash actual como sustituto del hash de la versión probada.

El auditor comprueba identidad única en el catálogo, resumen aprobado con hash y archivos de fuentes/revisión. Cambios o archivos ausentes retiran la acreditación al regenerar. La exhaustividad del mapa de dependencias necesita revisión humana/técnica; el script no analiza automáticamente llamadas C++. La independencia del original sigue sin medirse. Se conserva el alcance limitado de los bancos y no se vuelve a ejecutar GTA en esta tarea.

Ocho pruebas del registro aprobadas, incluida detección de cambios en fuentes, dependencias e informes, evidencias ausentes/no aprobadas e identidades duplicadas. Panel regenerado con el mismo CSV. Cambios previos conservados; sin commit, push, instalación ni modificación de partidas. Los 86 bloqueos continúan aplazados.
### Ampliación vigente: primera función con vigencia acreditada

Autorizada la revisión del constructor de peatones. Se revisaron sus constructores base, timer/vector, copias CRT, búsqueda de bloque/jerarquía, CRC y tipos/tablas. Para resolver la falta de un manifiesto histórico completo se recompiló y ejecutó el banco existente con las fuentes actuales. Resultado: 8.192 pares del constructor, 128 de ProcessPed original y 512 de desplazamiento, cero diferencias. Las repeticiones no aumentan cobertura única.

[Informe y dependencias](ValidationRegistry/namedanim-review.md) y [resumen](ValidationRegistry/namedanim-validation.json). Ejecución 20260923-164058-543160, sonda 7902d9c275ac7c7c00ecfab8790b8c01aac9e95bd59c384edc8f93187f6e3874. El registro ahora acredita **1/8.014 (0,01%)**: Constructor-Anim, únicamente dentro del alcance probado y entorno documentado. IK y RpAnimBlend siguen pendientes de completar su revisión. Independencia del EXE sin medir.

Se capturaron antes de compilar y verificaron después las huellas de source/cmake/conan y configuración/fuentes del banco. El auditor retira la acreditación por cambios, archivos añadidos o eliminados. Nueve pruebas del auditor aprobadas. Entorno externo acotado a la compilación probada, sin certificar cambios de toolchain. La sonda nueva y su configuración quedan archivadas, conservando los artefactos históricos predeterminados.

Fuentes, ASI/PDB normales, INI y partidas restaurados/verificados por hash. ASI normal sigue ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f. Proceso propio cerrado automáticamente, instalación personal intacta. No se cambió hooks.csv ni se completó toda la clase; no se repitió la suite ni se acredita estabilidad general. Cambios anteriores conservados, 86 bloqueos aplazados, sin commit/push ni apagado.
### Ampliación vigente: SetupBones acreditada

Se revisaron SetupBones, acceso a jerarquías/matriz del pivote, BoneNodeManager, BoneNode::Init/InitLimits/AddChild, listas y liberación. Se recompiló la sonda IK con fuentes actuales y se repitió el banco: **131 pares únicos coincidentes**, contadores original/candidato 0/1, nodos devueltos y jugador/gestor/frames/keyframes conservados. También coinciden 9.384 bytes de clima y grupos de coches con los datos ya corregidos. No suma cobertura única ni otras funciones acreditadas.

[Revisión de dependencias](ValidationRegistry/ik-review.md) y [evidencia](ValidationRegistry/ik-validation.json). Ejecución 20260923-164625-027968, sonda 511a491a0856c02e88124f1bc49995ca73be6be2b119587253a320385fdcf44c. Registro: **2/8.014 (0,02%)**, constructor de animación por nombre y SetupBones, dentro del alcance y entorno documentados. RpAnimBlend sigue pendiente de revisión. SetupBones utiliza servicios originales RenderWare; no se acredita independencia del EXE.

Se reutilizó el ejecutor de revalidación con --case ik-data, sin nuevo CMD ni caso de catálogo. Huellas de source/cmake/conan y banco capturadas antes y verificadas después. Fuentes, sondas históricas, ASI/PDB normales e INI restaurados; tres archivos de datos y partidas intactos por hash. ASI normal conserva ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f. Proceso propio cerrado, instalación personal intacta. CSV y código normal sin cambios. Nueve pruebas del auditor aprobadas. No se repitió toda la suite ni se garantiza estabilidad general; 86 bloqueos aplazados, sin commit/push ni apagado.
### Ampliación vigente: RpAnimBlend y consolidación de todas las reconstruidas

Revalidada RpAnimBlendClumpUpdateAnimations con sonda recompilada de las fuentes actuales: **652 pares aprobados**, error máximo 2.9802322387695312e-08, tolerancia 1e-5, poses independientes y jugador conservado. Se mantienen exclusiones Audio/SurfaceInfos y límites del fixture. Se revisaron recorrido, asociaciones, tiempos y callbacks; persisten llamadas originales. [Revisión](ValidationRegistry/rpanimblend-review.md), [resumen](ValidationRegistry/rpanimblend-validation.json). Ejecución 20260923-170100-430075; sonda a163757411fb3e80a39a964b2d9d571d30df62b81b6c397e6f174dc1f259d2bc. No se acredita toda RpAnimBlend 43/43.

Incorporadas 30 funciones adicionales mediante identidad exacta y resumen aprobado: son evidencia histórica, no vigencia actual. Registro **33 con evidencia / 3 con vigencia**; barra **3/8.014 (0,04%)**. El [inventario completo](../docs/ValidationInventory.md) clasifica las 7.485 entradas reversed=1: 3 vigentes, 30 históricas por revisar y 7.452 sin evidencia vinculada todavía. Esta última etiqueta no significa nunca probada. Las 529 pendientes de reconstrucción quedan aparte. La siguiente tanda está listada por función y resumen, sin reabrir los 86 bloqueos aplazados.

Nuevo build_probe.py para RpAnimBlend; el ejecutor de revalidación admite --case rpanimblend, conserva el runner histórico y su sonda predeterminada. Importador histórico idempotente; inventario regenerable con contrib/validation_inventory.py. CSV, clases reconstruidas y flags intactos. Fuentes normales, ASI/PDB, INI y partidas restaurados por hash; ASI normal ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f. Instalación personal intacta, GTA propio cerrado, sin commit/push ni apagado. No se repitió toda la suite ni se afirma estabilidad general. La independencia del EXE sigue sin medirse globalmente.
### Pausa vigente: CCustomBuildingRenderer (2026-09-23)

Por petición del usuario, se aplaza el trabajo para evaluar qué ofrece menor coste: reconstrucciones nuevas o desbloqueos. [Estado completo y punto de reanudación](CustomBuilding/README.md), [resumen](CustomBuilding/validation-summary.json). La prueba ya iniciada terminó con **95.168 pares exactos**, cinco hooks habilitados desde arranque y restauración verificada. No se inició otra prueba después de la pausa. Fuentes candidatas conservadas; CSV y catálogo siguen 3/7. Falta compilación normal, integración, instalación y cierre del registro; no declarar la clase terminada. ASI/PDB normales conservan los hashes de la continuidad del ratón. Partidas e INI restaurados y GTA cerrado. Git: ReSA, HEAD 11fd6912f759d3bc93208a9def001b6dd751dbdf; cambios locales y archivos no rastreados conservados, sin commit/push.

Panel e inventario regenerados sin alterar CSV: 33 evidencias históricas, 0 acreditaciones vigentes para el árbol modificado y 7.452 reconstruidas sin evidencia vinculada. Las tres anteriores requieren revisar el cambio de huella global de source; no es un fallo de sus pruebas. CustomBuilding aún no aporta acreditaciones al registro.

Criterio propuesto, no campaña iniciada: priorizar funciones con implementación próxima al original, causa del bloqueo entendida y banco reutilizable. Suelen costar menos que una reconstrucción desde cero. Un lock complejo puede costar más que reconstruir una función pequeña; comparar funciones y dependencias, no número de clases. La validación sigue siendo obligatoria en ambos casos. Sin estimaciones numéricas de tokens o tiempo ni reapertura de los 86 bloqueos aplazados.

### Continuidad posterior: GetTaskType de GangLeader desbloqueado (2026-09-24)

Autorizada únicamente `CTaskComplexGangLeader::GetTaskType`. Git real: **ReSA**, HEAD **aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51**, con cambio local previo en TaskSimpleClimb.cpp, conservado byte a byte. El HEAD y los binarios eran posteriores a los del traspaso. Sin commit, push ni apagado.

Original verificado `0x65DF20`: `mov eax,1201; ret`, tabla virtual `0x86F8FC`, índice 4. El cuerpo C++ ya coincidía. Su lock era una precaución colectiva con el hook deshabilitado, no una obligación de mantenerlo activo; no se atribuye a esta función la causa de los cierres mencionados en la clase. [Informe y reproducción](GangLeader/README.md), [24 pares aprobados](GangLeader/validation-summary.json): entrada directa y rutas virtuales, contadores original/candidato 0/1, bytes y tablas al conmutar, objetos y otros hooks conservados. Banco dentro del menú del motor; no sesión jugable. Intento previo descartado por índice incorrecto de la sonda conservado.

Único lock retirado: GetTaskType. CSV/catalogado 13/13 intacto; GangLeader **10 locks**, total **619**, restantes aplazados de clases completas **85**. Clone, otros métodos de GangLeader, CCollision, CFileMgr y Game siguen fuera del alcance. Registro: **34 evidencias históricas, 0 acreditaciones vigentes**, 7.451 reconstruidas sin evidencia vinculada. No se atribuye independencia del EXE ni estabilidad general; no se repitió toda la suite.

**Entrega normal aislada instalada solo en la copia de pruebas**, con [manifiesto y resumen](GangLeader/installation-summary.json). ASI en bin/Debug e instalado: **fa25398636579a405ab50c2067c28077c5c9cb2e4f7bf7a0d6dd855cf6d78558**; PDB **cd39cc7bbe38478e6787c0dd4798dece473dcda6d9d2f933d1b539e0e92e3fb9**. Respaldo anterior `_baseline/before-gang-leader-20260924-203727`: instalado d8b39a39 (impactos de bala), bin/Debug a81bb5fb (candidato de cierre). Integración sin sonda: menú a los 2,25 s y diez segundos estable, salto original y entrada virtual apuntando al candidato. Proceso propio terminado; no se prueba el cierre normal.

Para excluir entregas aplazadas se compilaron temporalmente los dos .cpp de CustomBuilding de `11fd6912`, Game.cpp/Cover.cpp/Cover.h de `aef6bcb^` y TaskSimpleClimb.cpp de `aef6bcb`. Las fuentes actuales y el cambio local de escalada se restauraron por bytes. **El ASI entregado no es una compilación sin filtros del árbol actual.** Usar `GangLeader/build_probe.py --normal` para reproducirlo; una compilación habitual incluiría los trabajos aplazados. CustomBuilding y la corrección de ShutdownLists permanecen pendientes, conservados y excluidos de esta entrega.

Runner compartido reutilizado con configuración temporal que omite vídeos, **sin cargar partidas** (`SaveGameToLoad=-2`). Se restauran ambos INI normales (`FastLoader=false`, `SaveGameToLoad=-2`) y se comprueban partidas por hash. La integración no verifica el recorrido de vídeos con FastLoader=false. Instalación personal intacta. Caso `gang-leader-type` incorporado al catálogo de 44 casos; único PROBAR.cmd conservado. InicioNormal admite un observador opcional de solo lectura para comprobar el hook en la integración normal.


## Continuidad: cierre Game/Cover corregido (2026-09-25)

El usuario autorizó retomar el fallo de cierre y pidió usar un inicio automático en vez de capturas y navegación por menús. [Informe vigente](ShutdownLists/README.md), [instalación y evidencia](ShutdownLists/installation-summary.json). Esta entrega sustituye el aplazamiento de ShutdownLists y los hashes instalados de GangLeader; CustomBuilding sigue aplazado.

Se conserva la limpieza de Cover al principio de CGame::Shutdown. Las pilas identificaron dos destructores demasiado tardíos: el trabajador periódico de spdlog y UIRenderer/SDL, este último con excepción 0x87a al cerrar DXGI. Ahora WinMain destruye la interfaz antes de RenderWare/ventana y cierra los trabajadores de logs antes de retornar. Se retira primero el manejador de excepciones que depende del logger.

Tres ejecuciones normales sin depurador aprobaron inicio automático, diez segundos en estado de juego y cierre mediante WM_CLOSE, todas con código 0. Banco instrumentado: 16 casos aprobados y cierre 0. Alcance: inicio/introducción de partida nueva, sin cargar partidas, conducir, combatir ni sesión prolongada; no se repitió toda la suite.

FastLoader admite SaveGameToLoad=0 para partida nueva. Las pruebas lo activan temporalmente y restauran Enable=false/SaveGameToLoad=-2. Evitar la ranura 2 incompatible; no hacen falta capturas ni menús para estas pruebas. Partidas y ambos INI conservados por hash, instalación personal intacta.

ASI normal en bin/Debug y copia de pruebas: b132855e4c96c1a02824b91569de158848a723718059962d5b83f91e02c5f980. PDB: 7fbdff7c8c62a74cdd266cb3898bf8f2160b41ff36fdc14687d8b4ba22751750. Reproducir con ShutdownLists/build_isolated.py: excluye los dos candidatos CustomBuilding y el cambio local de Climb, restaurados por bytes. Incluye GetTaskType ya entregado. El manifiesto identifica las fuentes exactas y el resumen el respaldo anterior. No usar la compilación aislada histórica de GangLeader para esta entrega: aquella excluía Game/Cover.

Se conservan 619 locks y los 85 aplazados de clases completas; no se alteraron CSV, catálogo ni acreditaciones en esta corrección. Rama ReSA, HEAD aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51, cambios anteriores preservados. Sin commit, push ni apagado.


## Continuidad histórica: auditoría y sincronización del catálogo (2026-09-26)

Se auditó de forma bidireccional el registro de hooks de `ReSA` para detectar owners activos presentes en fuente pero ausentes de `docs/hooks.csv`, registros indirectos, scopes sin instalaciones y código de hooks deliberadamente inactivo. La primera comparación sobreestimó candidatos porque incluía llamadas comentadas; el auditor se corrigió para eliminar comentarios antes del análisis y distinguir registro directo, indirecto, inactivo y aliases/namespaces.

Estado vigente del catálogo:

- **712 clases/grupos**.
- **8.080 entradas** registradas.
- **7.545 reconstruidas** (93,38%).
- **34 con `validated=1`** (0,42%): evidencia aprobada vinculada.
- **0 validaciones vigentes** para el árbol actual según hashes/dependencias.
- **535 pendientes**.
- **621 clases completas**, **88 parciales** y **3 sin revertir**.
- **625 entradas bloqueadas** y **7.455 sin bloqueo**.
- **91 clases incompletas**.
- 34 funciones con evidencia histórica vinculada; 0 con vigencia acreditada para el árbol actual en el registro consolidado.

`validated` se sincroniza automáticamente desde `reversing/validation-registry.json` mediante `python contrib/sync_validated_flags.py`; no se edita a mano.

La comprobación local vigente es:

```powershell
python contrib/audit_hook_catalog.py --strict
```

Resultado actual: **OK**. Las cuatro comprobaciones críticas están en cero: hooks activos fuera del CSV, registros indirectos activos ausentes, entradas del CSV sin scope fuente y llamadas directas con instalaciones propias sin owner catalogado. El modo `--strict --verbose` muestra además owners inactivos, registros indirectos catalogados y aliases informativos sin convertirlos en progreso activo.

La auditoría incorporó al catálogo owners activos que antes no estaban reflejados, entre ellos las clases del subsistema Conversations y varias tareas de Interior. El estado de Conversations queda separado correctamente: `CConversations` 11/11, `CConversationNode` 1/1, `CPedToPlayerConversations` 2/3 y `CConversationForPed` 0/2. Por tanto, el antiguo 11/11 nunca debe interpretarse como cobertura completa del subsistema.

También se añadió `contrib/audit_hook_catalog.py`, se documentó en `docs/HookRegistration.md` y se creó un workflow independiente de auditoría. Los workflows de build/progreso se ajustaron para usar `windows-2025` en repositorios privados y el de progreso queda preparado para sincronizar `hooks.csv`, `ReversedClasses.md` y `ValidationInventory.md`. La auditoría local estricta es la referencia inmediata cuando GitHub Actions no llega a ejecutar pasos.

Documentación sincronizada en este corte: README principal, hoja de ruta, panel de progreso, prioridades, inventario y registro de validación, guía de hooks, índice de pruebas, plan histórico contextualizado y README del subsistema Conversations. Los informes de bancos anteriores conservan sus cifras originales como evidencia histórica y enlazan al estado vigente cuando procede; no se reescriben resultados, hashes ni conteos pertenecientes a una entrega pasada.

No se afirma independencia del EXE ni estabilidad general. El crecimiento del catálogo cambia denominadores de progreso, no invalida pruebas anteriores. Los 86 bloqueos aplazados de la campaña documentada siguen siendo una cola histórica concreta y no deben confundirse con los 625 locks globales del catálogo vigente.


## Entrega anterior: CPedToPlayerConversations completada (2026-09-27)

Esta sección conserva el estado y los hashes de aquella entrega. La [continuidad vigente](#continuidad-vigente) incluye la finalización posterior de CConversationForPed.

Petición expresa de completar esta clase, en ReSA sobre HEAD `143b5e73b51cf4ae2a642401f0045213c05bab4e`. `Update` (0x43B0F0) reconstruido, conectado y sin lock; clase **3/3**. [Informe y procedimiento](PedToPlayerConversations/README.md), [evidencia](PedToPlayerConversations/validation-summary.json), [instalación](PedToPlayerConversations/installation-summary.json).

Banco aprobado: **6.266 pares exactos** dentro del motor, con estados independientes, conmutación original/candidato, contadores 0/1, objetos intactos y restauración. Audio, entrada, estadísticas, azar e IK usan proveedores controlados; geometría, referencias y tareas utilizan el motor real. No acredita reproducción de audio, combate, todo el subsistema ni independencia. El banco inicial de 5.910 pares es un subconjunto, no cobertura adicional.

**Entrega normal instalada solamente en la copia de pruebas**, también conservada en bin/Debug. Tres arranques breves de partida nueva, **tres segundos** en estado de juego, hook activo y cierre WM_CLOSE con código 0. ASI `e3e81bf86239ed4bded6ab1783b760c411dcdbfe080131a2a6e0c9569472b358`; PDB `65dab3022f7c838c51b80d69abbe1585c64bc844402898f5432b78c0f1992467`. Sustituyen la entrega ShutdownLists b132855e/7fbdff7c; respaldos y manifiesto exactos en installation-summary.json.

**Incidencia abierta de audio:** dos tandas de diez segundos con el candidato terminaron con timeout después de dos y un cierres correctos. La pila capturada está en la precarga de PROLOG1, dentro de CAECutsceneTrackManager::PreloadCutsceneTrack. La prueba de control con el ASI anterior de ShutdownLists y Update original reprodujo el bloqueo después de dos cierres correctos, con la misma ruta en la pila. [Intentos y trazas](PedToPlayerConversations/integration-observations.json). No se identifica su causa raíz ni se declara corregido. La prueba breve de entrega cubre arranque, hook y cierre antes de esa fase; no acredita la introducción completa ni estabilidad general.

Se reutilizan exactamente las exclusiones de ShutdownLists: dos fuentes CustomBuilding desde `11fd6912f759d3bc93208a9def001b6dd751dbdf` y TaskSimpleClimb desde `aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51`. Fuentes restauradas tras compilar; sin sondas en el ASI entregado. Se corrigió únicamente texto que había quedado fuera de un comentario en ColModel.cpp para permitir la compilación.

Catálogo actual: **712 grupos, 8.080 entradas, 7.546 reconstruidas, 534 pendientes; 622 completas, 87 parciales y 3 sin revertir; 624 locks y 7.456 sin lock**. Registro: **35 evidencias aprobadas vinculadas, 0 acreditaciones vigentes del árbol completo y 7.511 reconstruidas sin evidencia vinculada**. Auditoría estricta ejecutada con Python 3.14: OK; cuatro comprobaciones críticas en cero. `validated` sincronizado desde el registro. CConversations sigue 11/11 y CConversationNode 1/1; CConversationForPed continúa 0/2.

Caso `ped-to-player` añadido al único PROBAR.cmd. Reproducir con `python reversing/PedToPlayerConversations/build.py --probe`, `PROBAR.cmd --case ped-to-player`, compilación normal sin --probe y `python reversing/PedToPlayerConversations/check_normal.py --startup-only --install`. Conservar los intentos fallidos al repetir; sin --startup-only se comprueban diez segundos y se captura pila al agotar el plazo. El informe explica el alcance de cada prueba.

INI normales restaurados (FastLoader desactivado, SaveGameToLoad=-2), partidas comprobadas por SHA-256 e instalación personal intacta. Pruebas temporales con partida nueva=0, nunca ranura 2. No se repitió toda la suite. Siguen aplazados **CCollision 59, CFileMgr 13, GangLeader 10 y Game 3: 85 locks**, además de la integración CustomBuilding. No aplicar deferred-filemgr-game.patch. Esta entrega no autoriza otra clase ni una campaña de audio. Sin commit, push ni apagado.


<a id="continuidad-vigente"></a>

## Continuidad vigente: CConversationForPed completada (2026-09-30)

Petición de completar las dos entradas de `CConversationForPed`. `IsPlayerInPositionForConversation` (0x43AC40) y `Update` (0x43C190) quedan reconstruidas y habilitadas: **2/2, cero pendientes y cero bloqueos**. [Informe y reproducción](ConversationForPed/README.md), [evidencia diferencial](ConversationForPed/validation-summary.json), [entrega normal](ConversationForPed/installation-summary.json).

Banco de **4.702 pares exactos**: 2.414 de posición y 2.288 de Update, con original/candidato separados, contadores 0/1, entradas independientes, comparación del orden de efectos y restauración. Incluye límites geométricos, daño reciente, seguidores, disponibilidad, ramas sí/no, subtítulos, voz normal/de misión y aritmética temporal unsigned con desbordamiento. Se conservan las operaciones x87; 18 casos específicos contrastan precisión 24/53/64. Entrada, disponibilidad, traducción, mensajes y audio son dependencias controladas: no se reproduce audio ni se dibujan subtítulos. Los límites completos están en el informe. Se repitió el banco tras regenerar únicamente el encabezado de estado de esta clase, con idéntico log; no se duplican los pares únicos.

Entrega normal instalada solamente en la copia de pruebas y conservada en bin/Debug, sin sondas. Tres arranques de partida nueva, tres segundos en estado 9, ambos hooks apuntando al módulo instalado y tres cierres normales con código 0. ASI `cfb7ccd82ed7b4a289574699fa3a736c195f346e6abde04cd19d9333482949b9`; PDB `876716f9244e88fa45b38442ba041e16412c3442049bf9fd5692c8e0cbab01d4`. El informe de instalación conserva las rutas y hashes de respaldo. Se mantienen las exclusiones de CustomBuilding/TaskSimpleClimb de la entrega anterior; las fuentes se restauran por bytes después de compilar. La incidencia anterior de precarga PROLOG1 sigue abierta y fuera del alcance.

Catálogo: **712 grupos, 8.080 entradas, 7.548 reconstruidas y 532 pendientes; 623 completas, 87 parciales y 2 sin revertir; 622 locks y 7.458 sin lock**. Registro: **37 evidencias aprobadas vinculadas, dos con fuentes registradas coincidentes, cero acreditaciones de dependencias para el árbol completo y 7.511 reconstruidas sin evidencia vinculada**. Catálogo estricto y revisión de integridad: OK. El subsistema Conversations registra ahora 17/17 entradas entre sus cuatro owners; esto no acredita integración exhaustiva ni independencia del EXE.

Caso `conversation-for-ped` añadido al único PROBAR.cmd y a `--all`. También se completan cuatro prefijos de informes que faltaban para casos ya catalogados; ver [run_all.py](run_all.py). Catálogo de pruebas: 46 casos, 45 de motor y uno aislado. No se ejecutó toda la suite. Para repetir este banco, compilar la sonda con `python reversing/ConversationForPed/build.py --probe` y ejecutar `PROBAR.cmd --case conversation-for-ped`; el runner rechaza fuentes distintas del manifiesto. Después compilar sin --probe y usar `python reversing/ConversationForPed/check_normal.py --install` para comprobar/promover el ASI normal.

INI restaurados a FastLoader=false y SaveGameToLoad=-2; pruebas temporales con partida nueva=0, nunca ranura 2. Partidas verificadas por SHA-256. Se conservaron los cambios previos del árbol. Permanecen aplazados los 85 locks de CCollision/CFileMgr/GangLeader/Game y CustomBuilding. Sin commit, push ni apagado.

## Continuidad vigente: CEntryExit::TransitionFinished validada dentro del motor (2026-10-01)

`CEntryExit::TransitionFinished` (`0x004404A0`) ya figuraba `reversed=1,locked=0` en `docs/hooks.csv` y CEntryExit ya mostraba 11/11 **antes** de esta tarea, sin que existiera la comparación dentro del motor exigida por la sección "Cómo se reconstruye una función" de este mismo archivo. Esta entrega aporta esa comparación; no cambia ningún indicador del CSV ni de `docs/ReversedClasses.md` porque ya estaban correctos, y no fue necesario corregir `source/game_sa/EntryExit.cpp` porque no se encontraron diferencias en el alcance probado.

**Alcance:** la función es una máquina de estados (`EXIT_ENTER_STATE_0..3`) cuyo bloque final (streaming de escena, teleporte de peatón/vehículo, tienda, población, scripts, `CGame::TidyUpMemory`) solo se alcanza cuando `EXIT_ENTER_STATE_2` pasa a `EXIT_ENTER_STATE_3` (`TheCamera.GetFading() == false`). El banco fuerza `GetFading()` a devolver siempre `true`, por lo que esa transición final y todo el bloque posterior **no se validaron**. Se cubrieron los cinco escenarios que devuelven `false` sin llegar a él (ambas ramas del condicional `bFoodDateFlag||bUnknownPairing||ped->bInVehicle`, con sus estados 0/1/2 alcanzables), con 3 variantes de bandera disparadora, 2 valores de `ms_bWarping`, 8 códigos de área y 4 posiciones: **960 pares originales/candidatos, 192 por escenario, 0 diferencias**.

Se alternó el hook ya instalado (`hook->State(false/true)`) para llamar, en `0x4404A0`, primero al código máquina original y después a la reconstrucción, con un peatón y un `CEntryExit` sintéticos e independientes por cada lado del par (memoria propia, nunca en los pools reales). Dependencias reales alcanzadas sin interceptar: `CPedGroups::GetPedsGroup` (siempre `nullptr` para el peatón sintético), las colas de streaming (`CColStore`, `CIplStore`, `CStreaming::AddModelsToRequestList`, `ThePaths`, `CRenderer::RequestObjectsInDirection`) y `TheCamera.Fade`/`SetFadeColour` (solo campos de estado, sin streaming ni audio). La única dependencia que toca entidades reales, `CEntryExitManager::SetAreaCodeForVisibleObjects`, se dejó ejecutar de verdad: el banco reinicia `ms_numVisibleEntities` antes de cada llamada y restaura `SetAreaCode(AREA_CODE_NORMAL_WORLD)` en cada entidad tocada inmediatamente después (la propia condición de coincidencia de la función garantiza ese valor previo); se tocaron y restauraron 7 entidades reales por llamada en el escenario correspondiente. Globales reales (`ms_exitEnterState`, `CGame::currArea`, `ms_spawnPoint`, `ms_bWarping`, `m_loadingPriority`, `m_bFading`, `ms_numVisibleEntities`, `ms_oldAreaCode`, `ms_visibleEntityList`) se guardaron antes y se restauraron verbatim al final.

**No se validaron:** el bloque final completo de teleporte/streaming/tienda/población/scripts; el peatón sosteniendo una entidad real (`SetAreaCode` del objeto sostenido); un grupo de peatones real (evento `CEventLeaderEntryExit`); `GetEntryExitToDisplayNameOf()` devolviendo un `CEntryExit` no nulo (`CHud::SetZoneName`); la sub-rama de `EXIT_ENTER_STATE_1` con tarea primaria `TASK_COMPLEX_GOTO_DOOR_AND_OPEN`; los valores exactos (duración, marca de tiempo) que escriben `Fade`/`SetFadeColour`. Detalles completos, hashes y evidencia en [reversing/CEntryExitTransition/README.md](CEntryExitTransition/README.md) y [validation-summary.json](CEntryExitTransition/validation-summary.json).

ASI normal recompilado sin instrumentación e instalado solo en la copia de pruebas: SHA-256 `34801e37752db66f73e01c8b79e7f5c1cf1d27b7a7866116c3557699287e36e5`; PDB `bb28eda79b1e2d8d17ec56522852b48a4d353450e30cba6861fd55f80668825e`. Respaldo del ASI/PDB anteriores en `_baseline/before-enextransition-20261001-125342` (ASI `cfb7ccd82ed7b4a289574699fa3a736c195f346e6abde04cd19d9333482949b9`, PDB `876716f9244e88fa45b38442ba041e16412c3442049bf9fd5692c8e0cbab01d4`). Partidas (`GTASAsf2.b`) verificadas sin cambios; instalación personal `E:\JUEGOS\GTA San Andreas` no se tocó. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-enextransition/20261001-125022-681196/comparisons.jsonl` (SHA-256 `806c86bdcb96c59a1948136fbab27f6b1ac184ec503c9170c18a0882ae0c6519`).

Caso `enextransition` añadido a `reversing/cases.json`, disponible en `PROBAR.cmd --case enextransition`, `python reversing/run.py --case enextransition` y `--all`. Se siguió la cola autorizada: tras CEntryExit siguen `CCarFXRenderer` y el resto de las 14 clases pendientes de esa lista (ver tabla arriba). Sin commit, push ni apagado. Conservar los trabajos anteriores.

## Continuidad vigente: fase 2 de CEntryExit::TransitionFinished — bloque invasivo (2026-10-01)

El usuario pidió explícitamente ampliar la validación anterior para cubrir también la parte que se dejó fuera a propósito: la transición `EXIT_ENTER_STATE_2`→`EXIT_ENTER_STATE_3` y el bloque final completo ("validalo aunque sea invasivo"). Se amplió la misma sonda (`reversing/CEntryExitTransition/LiveComparison.hpp`), tras restaurar todos los globales de la fase 1, para alcanzar ese bloque con el **jugador real** (`FindPlayerPed()`), ya que `ped->AsPlayer()->m_pEnex` exige un `CPlayerPed` auténtico.

El `CEntryExit` sintético usado como destino usa `AREA_CODE_NORMAL_WORLD` y la posición/orientación actuales del propio jugador, de forma que `Teleport()`/`SetHeading()` lo devuelven exactamente a donde ya estaba. Solo se ejecuta si el jugador está a pie, en el mundo normal, sin nada sostenido y sin `m_pEnex` previo (se cumplió: `heavy_precondition.safe_to_run=true`). Dos subcasos alternando `hook->State(false/true)`: **ligero** (`bFoodDateFlag=true`, prefijo común hasta el retorno temprano, 8 pares) y **completo** (`bFoodDateFlag=false`, el bloque entero de verdad —streaming de escena, `ped->Teleport` vía `FindValidTeleportPoint`, `CAudioZones`, `CWaterLevel`, `CGarages`, `g_interiorMan`, `CPopulation`, `CTheScripts::Process`, `TheCamera.Fade(..,FADE_OUT)`, `CGame::TidyUpMemory`—, 1 par). Tras cada llamada se restauran posición/cabeceo/área/`m_pEnex`/`bCanExitCar`/flag de `CPad`/pila de entry-exits por las rutas normales del motor (`Teleport`, `SetHeading`, `SetAreaCode`), nunca con escritura cruda de memoria.

**Resultado: 9/9 pares iguales** (8+1), cero diferencias; contador de llamadas 0/1 original/candidata en los 9. `heavy_complete.player_restored=true`: posición, área, `m_pEnex`, `bInVehicle` y la pila de entry/exits del jugador real coinciden con los valores previos tras ambas pasadas. Tampoco hizo falta corregir `source/game_sa/EntryExit.cpp`. Lo que sigue sin validarse (detalle completo en el README): `GetEntityThatThisPedIsHolding()` real, un grupo de peatones real, `GetEntryExitToDisplayNameOf()` no nulo, la sub-rama de `TASK_COMPLEX_GOTO_DOOR_AND_OPEN`, y vehículos/guardado/misiones/estabilidad general.

Nota de proceso: el primer intento de esta fase, delegado a un subagente, quedó bloqueado porque el clasificador de modo automático denegó por "Irreversible Local Destruction" la ejecución de `build_probe.py` (instrumentación temporal de fuente con restauración garantizada en `finally`, el mismo patrón ya usado sin incidentes en la fase 1). Se verificó el patrón, se ejecutó directamente con autorización expresa del usuario, y compiló/corrió sin problemas.

ASI normal reinstalado sin instrumentación tras esta fase, mismo hash que la fase 1 (no cambió): SHA-256 `34801e37752db66f73e01c8b79e7f5c1cf1d27b7a7866116c3557699287e36e5` / PDB `bb28eda79b1e2d8d17ec56522852b48a4d353450e30cba6861fd55f80668825e`. Sonda de esta fase: ASI `48207c1f405c8448e41d28897a6bfbf44afd0e11f6257c3e09096a11acf7bbb3` / PDB `416fb6d0d7845300095f926c098ccab58751c91db94a133c5bfab825ccb5957a`; `run_local.py` actualizado con este `PROBE_HASH` y con la verificación automática de los eventos `heavy_*`. Partidas (`GTASAsf2.b`) verificadas sin cambios; instalación personal no se tocó. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-enextransition/20261001-132428-655341/comparisons.jsonl` (SHA-256 `909b67706f6d2457e89f16e5421decc6e2f6371206a03a7ba0fc3540d173d734`) y `summary.json` en la misma carpeta.

Sin commit, push ni apagado. CEntryExit queda con su comparación dentro del motor ampliada al bloque invasivo; CCarFXRenderer y el resto de la cola siguen en el mismo estado que antes de esta tarea.

## Continuidad vigente: CTaskInteriorSitAtDesk::ProcessPed validada dentro del motor (2026-10-01)

`CTaskInteriorSitAtDesk::ProcessPed` (`0x00677920`, virtual) ya figuraba `reversed=1,locked=0` en `docs/hooks.csv` y la clase ya mostraba 9/9 **antes** de esta tarea, con el proyecto compilando limpio, sin que existiera todavía la comparación dentro del motor exigida por la sección "Cómo se reconstruye una función" de este mismo archivo. Esta entrega aporta esa comparación; no cambia ningún indicador del CSV ni de `docs/ReversedClasses.md` porque ya estaban correctos, y no fue necesario corregir `ProcessPed` porque no se encontraron diferencias en el alcance probado.

**Alcance validado: 144 pares, 0 diferencias discretas, error máximo 2.86e-06 (tolerancia 1e-5).** Seis categorías de `m_Anim->m_AnimId` × `m_bUpdatePedPos` × `m_bTaskFinished` × estado de `m_TaskTimer`/`m_AnimTimer` × dos valores de RNG controlado, siempre con `m_Anim` ya asociado y `m_bTaskAborting=false`. Cubre el retorno anticipado de `m_bTaskFinished` (contra el clump real del jugador), el reposicionamiento `m_bUpdatePedPos`, el vencimiento de ambos temporizadores (incluida la elección RNG entre `StartRandomOneOffAnim`/`StartRandomLoopAnim`) y la rama final `OFF_SIT_IN` con matemática real de posición/rotación. Se usa el jugador real (clump real) clonado en memoria independiente por cada lado del par, igual que `CTaskSimpleUseGun`.

**Hallazgo de RNG real vs. reconstruido:** el ASM original de `ProcessPed`, para decidir entre animación suelta y en bucle, llama directamente al generador interno `0x821B1E` (el LCG que usa el resto del EXE), mientras que la reconstrucción llama a `CGeneral::GetRandomNumber()`, que delega en `rand()` de la CRT — dos secuencias independientes incluso en partida real sin modificar, limitación ya documentada en `General.cpp:200` y ajena a esta función. El banco controla ambos puntos (mismo parche usado por `reversing/CGeneral/LiveComparison.hpp`) para comparar la misma decisión de rama.

**No se validó `m_bTaskAborting` ni `!m_Anim`:** ambas ramas llaman primero a `InteriorManager_c::AreAnimsLoaded(2)`, cuya resolución de índice (`GetAnimationBlockIndex` por nombre, reutilizado como `AssocGroupId`) depende del orden real de streaming de una partida normal. Tres mitigaciones distintas (forzar streaming+`CreateAnimAssocGroups`, parchear la función, enlazar manualmente la tabla de grupos) permitieron que una llamada aislada a `AreAnimsLoaded` funcionara, pero la misma llamada dentro del ASM **original** sin hook de `ProcessPed` seguía abortando con acceso a memoria inválido (`0xC0000005`, confirmado con `__try`/`__except` que el fallo es específico de ese lado). No se identificó la causa exacta; no se afirma que `ProcessPed` tenga un error, solo que esta rama no se pudo validar con seguridad en un banco aislado. Detalle completo de los tres intentos en [reversing/CTaskInteriorSitAtDesk/README.md](CTaskInteriorSitAtDesk/README.md) y `validation-summary.json`.

**Hallazgo fuera de `ProcessPed` (y corrección real, ver nota más abajo):** `InjectHooksMain.cpp` define, dentro de `Tasks()`, un lambda local `Interior` con las llamadas `InjectHooks()` de toda la familia `CTaskInterior*` (incluida `CTaskInteriorSitAtDesk`), pero nunca lo invocaba — otro lambda homónimo de ámbito distinto (`Interior_c`/`InteriorManager_c`) se llamaba en su lugar, ocultándolo. Ninguna de las ocho clases `CTaskInteriorBeInHouse/BeInOffice/BeInShop/GoToInfo/LieInBed/ShopKeeper/SitAtDesk/UseInfo` tenía sus hooks instalados en tiempo de ejecución, sin importar `hooks.csv`. El informe original de esta sub-tarea afirmó haber añadido la llamada `Interior();`, pero el diff real la dejaba sin invocar (solo un comentario explicando el motivo); el banco de `CTaskInteriorSitAtDesk` instaló su propio `InjectHooks()` de forma aislada para no depender de ella.

**Corrección real aplicada por la sesión principal, misma fecha:** se corrigió el defecto que bloqueaba `Interior()` — `CTaskInteriorLieInBed::InjectHooks` registraba `FinishAnimCB` (función `static`, no virtual) con `RH_ScopedVMTInstall` en vez de `RH_ScopedInstall`, abortando el proceso con *"Couldn't find function in vtable"*. Corregido a `RH_ScopedInstall` (idéntico al patrón ya correcto de `CTaskInteriorSitAtDesk::FinishAnimCB`) y activada la llamada `Interior();`. Se revisaron las seis clases hermanas restantes: todas registran únicamente virtuales genuinos, sin el mismo defecto. Compilación limpia y prueba de humo real: ASI normal nuevo (SHA-256 `4c2d1e55740713650ef25f38176e5fd7bfac277753b687e9b3785aecddfcf62f`) instalado en la copia de pruebas, el juego arrancó y permaneció vivo 25 s sin abortar, se cerró limpio y se restauraron partidas/respaldo sin cambios (`_baseline/before-interiorfix-smoketest-20261001-145916`, hash previo `c72f79320beae01a658cae2036a0c26f9a7713d39256d6a967f2a84a03367b40`). Con esto, las 8 clases `CTaskInterior*` ya instalan sus hooks en el ASI normal, no solo en el banco aislado — un alcance más amplio que el pedido originalmente para `CTaskInteriorSitAtDesk::ProcessPed`, pero necesario para que ese `reversed=1` tenga efecto real en el juego.

Respaldo del ASI/PDB previos a la sub-tarea de `sitatdesk` (antes de esta corrección) en `_baseline/before-sitatdesk-20261001` (ASI `34801e37752db66f73e01c8b79e7f5c1cf1d27b7a7866116c3557699287e36e5`, PDB `bb28eda79b1e2d8d17ec56522852b48a4d353450e30cba6861fd55f80668825e`). Partidas (`GTASAsf2.b`) verificadas sin cambios en todos los pasos; instalación personal `E:\JUEGOS\GTA San Andreas` no se tocó. Evidencia del banco de `ProcessPed`: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sitatdesk/20261001-145144-156757/comparisons.jsonl`.

Caso `sitatdesk` añadido a `reversing/cases.json`, disponible en `PROBAR.cmd --case sitatdesk`, `python reversing/run.py --case sitatdesk` y `--all`. Se mantienen excluidos los hooks Audio y Global/SurfaceInfos_c durante la prueba. Sin commit, push ni apagado. Conservar los trabajos anteriores; la cola autorizada de clases pendientes no se vio afectada por esta tarea (CTaskInteriorSitAtDesk no formaba parte de ella).

## Continuidad vigente: cuarto intento interrumpido sobre `m_bTaskAborting`/`!m_Anim` (2026-10-01, sesión posterior)

El usuario pidió explícitamente intentar validar también `m_bTaskAborting` y `!m_Anim` ("validalo bien y haz que funcione"). Hipótesis nueva: `CPed::Initialise` (`0x5DEBB0`) → `CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle` (`0x64F860`) ya carga `"int_house"`/`"int_office"` por el streaming normal durante el arranque del juego, antes de que exista cualquier jugador; para cuando la sonda llega a `"player_ready"` (tras cargar una partida real), `InteriorManager_c::AreAnimsLoaded(2)` debería resolver sin parches. Se añadió un diagnóstico de solo lectura (sin tocar memoria del motor) para comprobarlo.

En 6 ejecuciones vía el runner compartido más una observada en vivo, el juego **nunca llegó a escribir `"player_ready"`** — se quedó indefinidamente en `"waiting_for_player"`, antes de que el diagnóstico pudiera ejecutarse. Ese tramo es idéntico byte a byte al ya validado con 144 pares; el Visor de sucesos de Windows mostraba en paralelo un servicio de terceros (Daemon Tools) reiniciándose en bucle cada ~10 min, lo que apunta a contención de recursos del sistema, sin confirmación.

Durante los reintentos aparecieron dos diálogos de aserción de la Runtime de Visual C++ (vistos directamente por el usuario en pantalla):

1. **`HookCategory.h:60`, `!FindItem(item->Name())`** — causa identificada: nombre de hook duplicado. El `InjectHooks()` aislado propio de esta sonda (diseñado cuando el camino normal del juego todavía no registraba estos hooks) ahora choca con el camino normal ya corregido (ver sección anterior), que también los registra.
2. **`app_debug.h:14`, `Expression: result`** (macro `VERIFY`) — causa no identificada con certeza; no se confirmó que viniera del diagnóstico de esta tarea (que según los registros nunca se ejecutó) ni se descarta la misma contención de recursos.

El usuario pidió detener el trabajo ("ya para we") antes de aislar ninguna de las dos causas. Se detuvo el agente (`TaskStop`), se forzó el cierre del proceso de GTA, y se verificó manualmente: sin candados (`reversed-tests.lock`) ni marcadores (`*-runtime.start`) colgados, ASI instalado = el normal correcto con las correcciones de `InjectHooksMain.cpp`/`TaskInteriorLieInBed.cpp` (`4c2d1e55740713650ef25f38176e5fd7bfac277753b687e9b3785aecddfcf62f`), `gta-reversed.ini` restaurado a modo normal (`FastLoader=false`), partidas sin cambios. El diagnóstico de solo lectura se revirtió de `LiveComparison.hpp` sin dejar cambios sin probar en el banco; **los 144 pares ya aprobados no se tocaron ni se repitieron**, y `docs/hooks.csv` no se modificó en este intento (sigue en `reversed=1` desde antes).

`m_bTaskAborting` y `!m_Anim` siguen sin validar, en el mismo estado que al cierre de la sección anterior. La hipótesis del cuarto intento sigue sin refutarse, solo sin confirmarse en vivo por la inestabilidad observada del entorno. No se descarta repetirlo en una sesión con el sistema en reposo. Detalle completo (incluidos los dos diálogos de aserción) en [reversing/CTaskInteriorSitAtDesk/README.md](CTaskInteriorSitAtDesk/README.md#cuarto-intento-sesión-posterior-interrumpido-por-el-usuario) y `validation-summary.json` (clave `fourth_attempt_interrupted`). Sin commit, push ni apagado.

## Continuidad vigente: CTaskInteriorUseInfo completada y causa raíz de SitAtDesk (2026-10-01)

La sesión principal reconstruyó `CTaskInteriorUseInfo::CreateFirstSubTask` (`0x675B60`) y `CreateNextSubTask` (`0x676880`) a partir de `dumpbin`. También corrigió `InteriorManager_c::AreAnimsLoaded` (`0x5980F0`) para que leyera `ms_aAnimBlocks[idx].IsLoaded`, como el original, en lugar de pasar por la tabla de grupos. Esta entrega revisó de nuevo el ASM y validó las tres funciones dentro del motor. [Informe](CTaskInteriorUseInfo/README.md), [evidencia](CTaskInteriorUseInfo/validation-summary.json).

**Defecto nuevo encontrado y corregido: convención de llamada.** `AreAnimsLoaded` (`ret 4`) y `AddSameGroupEffectInfos` (`ret 8`) son `__thiscall` en el original: el llamador carga `ecx=0xBAF670` (`g_interiorMan`). En C++ estaban declaradas `static` (cdecl). Cuando un código **original** llamaba a cualquiera de ellas a través del hook, la pila del llamador quedaba desplazada. Pasan a ser miembros no estáticos (`RH_ScopedInstall`), y los llamadores (`TaskInteriorLieInBed.cpp`, `TaskInteriorSitAtDesk.cpp`, `TaskInteriorUseInfo.cpp`) usan `g_interiorMan.AreAnimsLoaded(...)`. **Esta, junto con el defecto del índice, era la causa raíz de los cinco intentos fallidos de `CTaskInteriorSitAtDesk`.** La afirmación anterior de que el original hacía lo mismo era falsa y se corrigió en su informe. Además, en el caso 9 se cambió `new CTaskComplexSequence{}` por `new CTaskComplexSequence;`, porque la inicialización por valor ponía a cero el relleno que el original no toca. Solo el relleno era distinto, pero el banco lo detectó (primera ejecución fallida, conservada).

**Resultados dentro del motor (los tres aprobados, cero diferencias):**

- Fase A, `AreAnimsLoaded`: **203 pares**. Argumentos 0..154 sobre el estado real (155 bloques; `int_house`=77, `int_shop`=79 e `int_office`=78) y las 8 combinaciones de `IsLoaded` de los tres bloques, restauradas tras cada una. Se mide `esp` alrededor de la llamada `__thiscall`: delta 0 en ambos lados.
- Fase B, `CreateFirstSubTask`/`CreateNextSubTask`: **412 pares** con fixtures sintéticos independientes por lado y una copia del jugador. Cubren `Type` 0..11, `m_IntInfo` nulo, subtarea `GOTO_INFO` u otra, `m_bDoInstantly`, `EntityIgnoredCollision`, `AreAnimsLoaded` verdadero/falso y tendero `SHOPKEEPER` u otro. `new`/`delete` de tareas usan una arena propia por lado rellenada con `0xCD` (comparación byte a byte, incluido el relleno); `Say` se registra sin audio.
- Fase C, `CTaskInteriorSitAtDesk::ProcessPed`: **312 pares** (los 144 previos, idénticos a la ejecución aprobada anterior, más 168 nuevos). Quedan validadas `m_bTaskAborting` (72) y `!m_Anim` (96). `IsLoaded` de `int_office` se controla por caso y el lado original deshabilita también el hook de `AreAnimsLoaded`. [Informe actualizado](CTaskInteriorSitAtDesk/README.md).

Catálogo: `CTaskInteriorUseInfo` pasa a **8/8** (`CreateFirstSubTask`/`CreateNextSubTask` con `reversed=1,locked=0`). `validated` no se tocó. Se ejecutó `contrib/sync_docs.py --update-source-status`: 7.554/8.080 entradas, 628/712 clases completas, 616 locks; la clase se retiró de las prioridades y se añadió la nota correspondiente.

ASI normal, recompilado desde el árbol actual sin instrumentación (se comprobó que no contiene cadenas de las sondas), instalado solo en la copia de pruebas: SHA-256 `a77c61c4e55fee47307ea17d78bea9f0a253911fd9d2555b90e19d0458c56090`; PDB `cd41d09711b6d1944c6b62b9214bfddbb0ffd5c27d94114b43e077a6a0aa59e1`. Prueba de humo: 25 s vivo, respondiendo y sin diálogo de aserción; cierre forzado por el script. Respaldo previo en `_baseline/before-interioruseinfo-20261001` (ASI `4c2d1e55740713650ef25f38176e5fd7bfac277753b687e9b3785aecddfcf62f`, PDB `a5196d705276aeb878a1742c8f9b73aea6974d2b9827e3c942b6106b04bd1af5`). Sondas en `_baseline`: `interioruseinfo` `1486eeaf…` y `sitatdesk` `ee06d242…` (se conserva copia de la anterior, `065ace24…`, en `CTaskInteriorSitAtDesk/artifacts`). Partidas (`GTASAsf2.b`) sin cambios e INI restaurados; la instalación personal no se tocó.

Casos `interioruseinfo` y `sitatdesk` registrados en `cases.json` y en `run_all.py`. Recordatorio: `run_local.py` no reconstruye la sonda; después de cambiar un `.hpp` hay que ejecutar `build_probe.py`, copiar a `_baseline` y actualizar `PROBE_HASH`. **Límites:** la corrección de ABI de `AddSameGroupEffectInfos` no tiene banco propio. No se probaron un tendero sin tarea primaria, un grupo o interior nulo en el tipo 9, ni `GetAnimationBlockIndex` = -1. Tampoco hubo NPC real en un interior real ni revisión visual. Sin commit ni push.

## Continuidad vigente: CAEWeaponAudioEntity — registro real, 11 correcciones y banco de audio (2026-10-01)

La auditoría estricta mejorada marcaba `CAEWeaponAudioEntity` como owner que nunca registra sus hooks: su llamada estaba comentada en `InjectHooksMain.cpp`. **En tiempo de ejecución sí se registraban:** `CAEPedWeaponAudioEntity` no declara `InjectHooks`, así que `CAEPedWeaponAudioEntity::InjectHooks()` llamaba al heredado. El `runtime-hooks.csv` del ASI anterior ya listaba las 20 entradas. Descomentar sin más habría duplicado nombres (assert `HookCategory.h:60`). Ahora se llama a `CAEWeaponAudioEntity::InjectHooks()` y la heredada queda comentada con su explicación; el registro sigue siendo único. Lección: **la auditoría textual no resuelve `Derivada::InjectHooks()` heredado**. Antes de "activar" un owner, mirar el `runtime-hooks.csv` real.

La revisión con dumpbin encontró defectos reales en código que **ya se ejecutaba en el juego**. Se corrigieron en C++:

- Sonidos de recarga A/B intercambiados en ocho armas.
- `PlayGunSounds`: doppler 0, orden de sonidos, interpolación de distancia media que daba volumen NaN, cola con volumen ajustado y sin tope.
- `ReportStealthKill` con sonido 471 en vez de 47.
- `PlayCameraSound` sin `audability` y con evento.
- Velocidad/atenuación de la minigun en aviones intercambiadas.
- `Reset` reasignando el objeto entero.
- Límites de `UpdateParameters`.
- `WeaponFire` abortando con `NOTSA_UNREACHABLE` para tipos sin sonido.
- `m_ClientVariable` 0 en vez de -1 y atenuación 2/3 en vez de 0.66.

`Constructor` (0x5DE990, en realidad constructor de `CAEPedWeaponAudioEntity`) y `Destructor` (0x507560, en realidad constructor de `CAEWeaponAudioEntity`) no tienen llamadores en el EXE. Pasan a `{ .enabled = false, .locked = true }` y `locked=1` en `hooks.csv`, manteniendo `reversed=1`. [Informe](CAEWeaponAudioEntity/README.md), [evidencia](CAEWeaponAudioEntity/validation-summary.json).

Banco con **Audio activo** (solo `--unhook=Global/SurfaceInfos_c`): **3.178/3.178 pares**, 12 dentro de la tolerancia por log10 x87 y diferencia máxima 9,5e-7. Se comparan los **3.632 sonidos pedidos** al gestor (banco, id, volumen, atenuación, velocidad, doppler, flags, evento, variable cliente, posición...) en vez de oírlos. Los gestores, bancos, aleatoriedad, reloj y área están controlados; Initialise/referencias/cámara/atenuación son reales. El contraste con el C++ anterior diverge en 15 de las 18 funciones y aborta en `WeaponFire:301`. Una primera ejecución con el C++ nuevo falló solo por el sonido 471 y se conserva. Caso `weaponaudio` en `cases.json` y `run_all.py`; sonda archivada `f7065c02…`.

Prueba de humo (`CAEWeaponAudioEntity/smoke_test.py`): 35 s en partida con Audio activo, sin diálogos de assert, 20 hooks registrados una vez. ASI normal final instalado solo en la copia de pruebas, compilado del árbol actual tras `sync_docs`: `a0c5e7fb049292646cccad293cc175302e54e78270605d48f575c14d330914bd`; PDB `3ded71739c3bbbd78d0005fdf6c173cdd62515c13e1f9641f65dadfac3a652e1`. Respaldo en `_baseline/before-weaponaudio-20261001` (`a77c61c4…`/`cd41d097…`). Partidas e INI sin cambios; instalación personal intacta. Auditoría estricta: OK. Se ejecutó `contrib/sync_docs.py --update-source-status`; las cifras están en `docs/`.

**Límites:** no se oye ni se valida la mezcla o reproducción real. Constructor/Destructor siguen bloqueados (C++ no corresponde a esas direcciones). Un intento de contraste con el C++ anterior terminó con código 3 al iniciar el audio sin causa identificada. Coincidió además otra sesión usando la copia de pruebas (`InicioNormal`); se esperó a que liberara el candado. Sin commit ni push.

## Continuidad vigente: hooks activados en 39 clases y auditoría de `InjectHooks` heredado (2026-10-01)

Se registraron 39 clases cuyos hooks no se instalaban: nueve con `InjectHooks()` comentado en `InjectHooksMain.cpp` y treinta con `InjectHooks()` nuevo, con cada vtable verificado contra `gta_sa.exe`. Son 216 entradas nuevas en `hooks.csv`. Los stubs `plugin::Call` y tres `Clone` no fieles quedan con `reversed=0`, así que 16 clases quedan parciales con 43 pendientes. Diez clases se descartaron con motivo. [Informe](HooksActivados/README.md).

La lección anterior ("la auditoría textual no resuelve `Derivada::InjectHooks()` heredado") queda automatizada. `audit_hook_catalog.py` tiene una sexta comprobación crítica, la de llamadas `X::InjectHooks()` sin `InjectHooks` propio en `X`, con pruebas en `contrib/tests/test_audit_hook_catalog.py`. Sobre `143b5e7` detecta `CAEPedWeaponAudioEntity`. Hoy da 0, porque esa clase tiene ya su propio `InjectHooks()` (cuatro métodos sin duplicar el constructor bloqueado de `CAEWeaponAudioEntity`).

Catálogo: **750 grupos, 8.296 entradas, 7.727 reconstruidas y 569 pendientes; 650 completas, 97 parciales y 3 sin revertir; 661 locks y 7.635 sin lock**. Registro de validación sin cambios: 37 `validated=1`, 0 vigentes. Se ejecutó `contrib/sync_docs.py --update-source-status`; `ReversingPriorities.md` incluye las 16 clases nuevas en una sección propia que el sincronizador ya reconoce.

Compilación Debug y arranque al menú sin errores con el ASI de este árbol; el primer intento superó los 180 s por un estado 0 de 132 s. **Sin comparaciones contra el original.** El ASI instalado en la copia de pruebas sigue siendo `a0c5e7fb…`, restaurado tras cada prueba. Commits: un commit por clase en `ReSA`, sin push. Los cambios de `InjectHooksMain.cpp` y `hooks.csv` se commitearon solo con las líneas propias. Los documentos y la auditoría quedan sin commit porque comparten archivos con trabajo previo pendiente.

## Continuidad vigente: plantillas de 21 clases sin código (2026-10-01)

Se crearon plantillas, **sin reconstrucción**, para 21 clases que solo aparecían comentadas en `InjectHooksMain.cpp`. Cada una tiene un `.h` con su `Type` y la declaración de `InjectHooks`, y un `.cpp` con `InjectHooks()` vacío. No se registran ni entran en `hooks.csv`, así que el catálogo no cambia: **750 grupos, 8.296 entradas, 7.727 reconstruidas y 569 pendientes**. De 13 se localizó y verificó el vtable en `gta_sa.exe` a partir de su `GetTaskType`; 8 quedan con el vtable por localizar. `CTaskComplexDriveWander` no se duplicó: es `CTaskComplexCarDriveWander`. Detalle en [HooksActivados](HooksActivados/README.md#plantillas-de-clases-sin-código-2026-10-01-posterior).

Lección: `source/CMakeLists.txt` usa `GLOB_RECURSE` sin `CONFIGURE_DEPENDS`. Al añadir archivos `.h` o `.cpp` hay que reconfigurar (`cmake .` en `build/`) o no se compilan. Un commit por plantilla, sin push.

## Continuidad vigente: segunda tanda de hooks y comparación EXE ↔ código (2026-10-01)

Se registraron 15 clases cuyo `InjectHooks()` nunca se llamaba (`CHud` 38 hooks, `CPedAttractor` 20, `CIdleCam` 14…) y 6 tareas con vtable localizado en el EXE: 144 entradas nuevas. Antes de activarlas se corrigieron dos defectos. `CDecision::Set` era un stub instalado como reconstruido y habría entrado en recursión infinita. `CLodTimeModelInfo` usaba las direcciones de `CTimeModelInfo` y habría abortado el arranque. La prueba descubrió además una dirección enganchada dos veces (`CTaskSimpleNone::Destructor` = `CTaskSimpleLeaveGroup::Destructor`, por funciones plegadas en el EXE). Resultado: 35 s en partida sin asserts y `runtime-hooks.csv` coherente. Catálogo: **771 grupos, 8.440 entradas, 7.854 reconstruidas y 586 pendientes; 663 completas, 105 parciales y 3 sin revertir; 678 locks**. [Informe](HooksActivados/README.md#segunda-tanda-injecthooks-sin-llamar-y-tareas-con-vtable-2026-10-01).

Lecciones:

1. **`runtime-hooks.csv` es la fuente de verdad del registro**: compararlo con `docs/hooks.csv` destapó 3 hooks sin catalogar y 7 diferencias de `reversed`.
2. **Las direcciones duplicadas se buscan en el código**, porque las filas `V` de `hooks.csv` con dirección 0 las esconden.
3. **Antes de activar un `InjectHooks` antiguo** hay que cotejar sus vtables con el EXE y buscar stubs `plugin::Call` instalados como reconstruidos.

Siete hooks heredados del upstream apuntan a mitad de función; quedan pendientes de corregir.

## Continuidad vigente: siete direcciones de hooks corregidas (2026-10-01)

Se corrigieron los siete hooks heredados del upstream que apuntaban a mitad de función. Seis tienen ahora su dirección real, comprobada en el EXE contra el C++: `SetClumpForAllAtomicsFlag` 0x7322F0, `CLoadingScreen::IsActive` 0x744DB0, `ValidateVersion` 0x5BA060, `SetRenderPipelinesCB` 0x4C88F0, `FinishGiveCPRAnimCB` 0x658990 y `CConversationNode::ClearRecursively` 0x43A710. `CConversations::EnableConversation` se retiró: no existe como función propia en el EXE, así que `CConversations` pasa a 10/10. Catálogo: **771 grupos, 8.439 entradas, 7.853 reconstruidas y 586 pendientes**. 60 s en partida sin asserts; `runtime-hooks.csv` confirma las direcciones. [Detalle](HooksActivados/README.md#direcciones-corregidas-2026-10-01).

Lección: un hook en mitad de una función no produce error al registrarse; corrompe en silencio la función que lo contiene. Hay que comprobar, para cada dirección del catálogo, que es un inicio de función: que sea destino de un `call`, que la preceda un relleno o un `ret`, o que esté referenciada desde datos. Esta comprobación todavía no está en la auditoría.

## Continuidad vigente: CAEAmbienceTrackManager completada (2026-10-01)

`UpdateAmbienceTrackAndVolume` (0x4D6E60) quedó reconstruida, habilitada y desbloqueada con `validated=1`, y `CAEAmbienceTrackManager` pasa a **7/7**. El C++ ya estaba escrito en el árbol, pero el banco de `AudioCompletion` encontró una diferencia en `water-fade-pitch`. La supuesta "sobrepasada" del original al bajar la frecuencia no existía: en 0x4D7485 un `pop edi` desplaza la pila antes de leer `[esp+14h]`, y ese operando es la frecuencia objetivo. Corregido, los **7.846 pares** coinciden. Se añadieron `build.py`, `run_local.py`, el caso `audio-completion` en `cases.json`/`run_all.py` y la entrada del registro. El ASI normal pasó 60 s en partida con el audio activo, y se restauró el ASI instalado (`a0c5e7fb…`). Catálogo: **771 grupos, 8.439 entradas, 7.854 reconstruidas y 585 pendientes; 664 completas, 104 parciales y 3 sin revertir; 677 locks; 38 `validated=1`**. [Informe](AudioCompletion/README.md).

Lección: un desplazamiento `[esp+N]` hay que leerlo con la pila **en esa instrucción**. Un `pop` intercalado cambia a qué variable apunta.

Pendiente del mismo lote: `CAEWeatherAudioEntity::AddAudioEvent` difiere en un ulp (`thunder-gates-7897`) y `SetAudioBuffer` no se comparó.

## Continuidad vigente: lote de audio completo y catálogo igual al registro real (2026-10-01)

**Audio.** `AddAudioEvent` y `SetAudioBuffer` se suman a `UpdateAmbienceTrackAndVolume`: el banco `AudioCompletion` compara **10.086/10.086 pares** y las tres quedan con `validated=1` y fuentes coincidentes. Los defectos corregidos fueron dos:

- La constante 0x862E44 vale `0x3EB674D2`, un ulp más que el literal `0.35636f`.
- El buffer de DirectSound usa `GLOBALFOCUS` (`lea ecx,[ecx*4+80B4h]`), no `GETCURRENTPOSITION2`.

`CAEWeatherAudioEntity` pasa a 5/5 y `CAEStaticChannel` a 8/8, esta última ya con evidencia. [Informe](AudioCompletion/README.md).

**Catálogo frente a juego.** `audit_hook_catalog.py --runtime` compara `hooks.csv` con el `runtime-hooks.csv` real. Para que cuadren se hicieron cuatro cambios:

- `DoneSettingUpConversation` se activa en el código, porque ya estaba validada con 2.048 pares y nunca había corrido en el juego normal.
- `InitialiseDirtTexture` vuelve a pendiente, porque no tiene banco y el código la tenía desactivada; `CCarFXRenderer` queda en 9/10.
- Los 4 hooks aplazados de `CCustomBuildingRenderer` pasan a `{ .reversed = false }`, porque no tienen banco.
- Se catalogan los 3 hooks del modo ventana.

Resultado: **0 diferencias**, 60 s en partida sin asserts.

**Catálogo:** 772 grupos, 8.442 entradas, 7.857 reconstruidas y 585 pendientes; 665 completas, 104 parciales y 3 sin revertir; 677 locks; 40 `validated=1`.

**Disco y mudanza.** E: se llenó con artefactos de sondas (unos 790 MB por compilación); se borraron los binarios de compilaciones superadas. El repo y la copia de pruebas viven ahora en D:, y `D:\Original\gta-reversed-master` y `D:\JUEGOS\GTA San Andreas - CVehicle Test` son uniones hacia allí, así que las rutas `E:` de CMake, conan, `runtime_runner.py` y las evidencias siguen siendo válidas.

## Continuidad vigente: segunda lista de clases (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA:
- [PcSave](PcSave/README.md): `C_PcSave`, 5 funciones, 832 pares.
- [TasksBatch3](TasksBatch3/README.md): `UseGoggles`, `UseAttractor`, `UseAttractorPartner`, `UseEffect` y `UsePairedAttractor`, 38 funciones, 19.168 pares.
- [Markers](Markers/README.md): `C3dMarker` (5) y `C3dMarkers` (15), 12.768 pares.
- [IplStore](IplStore/README.md): 26 funciones de `CIplStore`, 6.240 pares.
- [BreakObj](BreakObj/README.md): 7 funciones de `BreakObject_c`, 8.704 pares.

**Defectos de alcance general encontrados:**
- La vtable de `CPedAttractor` estaba desordenada: en el EXE, `GetType` (virtual pura) va antes del destructor.
- El hook de 0x407180 (`GetRandomNumberInRange_int32`) usaba la plantilla entera de `GetRandomNumberInRange`, que puede salir desfasada en uno. Ahora usa `GetRandomIntInRange`.

**Lecciones:**
- **Constantes.** Hay que leerlas del EXE. 0x8717A8 es el float *siguiente* a 0,015, no `0.015f`; con el literal, `m_StdSize` salía desfasado en un ulp en el 3 % de los casos.
- **`fsin` x87.** Trabaja con precisión interna completa aunque el control esté a 24 bits. Se replica con `__asm` (`fld; fmul; fsin; fmul; fadd`).
- **Ensamblador en línea.** `size` (operador MASM), `dx` (registro) y `far` (macro de windows.h) no sirven como nombres de variable.
- **Bancos grandes.** Las regiones enormes (arrays globales, peatones completos) se comparan por hash FNV, volcando solo lo que cambió; volcarlas enteras en JSON hacía que el banco no terminara a tiempo.
- **Aserciones.** `InstallFailureCapture` añade ahora una pila simbolizada (dbghelp). Así se localizó el `cast<>` de depuración de `CScripted2dEffects::GetEffect`.
- **`acos` del CRT.** La rutina del juego (0x822394) conserva la precisión del FPU (`(cw & 0x300) | 0x7F`), así que calcula a 24 bits. Se emula con `fpatan` y `naked`.
- **Sin reservas en los stubs.** Si se sustituye `operator new`, la traza de los stubs no puede usar `json` (se recursa); se usa un array fijo.
- **Bytes basura.** El pool de la DLL rellena con 0xCD los objetos nuevos y `strcpy_s` rellena con 0xFE en depuración. Eso solo afecta a bytes sin significado, que se enmascaran en la comparación.

**Tercera tanda (02/10/2026, mañana):** se cierra lo pendiente.
- `CAEAudioChannel` 12/12: EAX reactivado con los GUID y la tabla de entornos del propio EXE.
- `BreakObject_c` 9/9 (`Init` y `Render`).
- `CIplStore` 34/34: banco [IplStore2](IplStore2/README.md) con pools y quadtree falsos.
- `C3dMarker`/`C3dMarkers` 26/26: dibujado, `Init` y `Shutdown`.
- `CLoadingScreen` 20/20: reescrita fiel al EXE manteniendo el cargador rápido.

**Hallazgos de alcance general:**
- `CMatrix operator*` del EXE agrupa las sumas de cada elemento de forma distinta; ahora la DLL lo replica.
- `CMatrix::WithUp` no normaliza (el original sí, donde construye esa matriz a mano).

**Lecciones:**
- Ante un acceso inválido sin pista, un manejador vectorizado (`FirstChanceAV`) registra la pila antes de que se deshaga. `InstallFailureCapture` lo instala ahora siempre.
- Los fixtures deben ser coherentes con el juego (atomic ⇒ matriz adjunta, clumps de dibujo no nulos, normales realistas); si no, aparecen falsos fallos.
- RenderWare: la DLL usa las macros de cámara (`[cam+0x18]`) en lugar de la función del EXE; una cámara falsa cubre los dos caminos.

**Pendiente de la lista anterior (ya resuelto en la tercera tanda):**
- `BreakObject_c`: `Init` y `Render`.
- `CIplStore`: 8 funciones (carga de archivos, borrado de entidades y del quadtree; `SetupRelatedIpls` está incompleta en la DLL).
- `C3dMarker::Render` y el render, la inicialización y el cierre de `C3dMarkers`.
- `CLoadingScreen`: desviaciones NOTSA deliberadas.

## Continuidad vigente: tercera lista de clases (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (201 funciones, 108.016 pares): [Small2](Small2/README.md), [Mirrors](Mirrors/README.md), [Messages](Messages/README.md), [TasksBatch4](TasksBatch4/README.md), [TasksBatch5](TasksBatch5/README.md) y [MenuSystem2](MenuSystem2/README.md).

**Defectos de alcance general encontrados:**
- El envoltorio del hook de `CTaskComplexSeekEntity` (0x661DC0) intercambiaba los dos intervalos: todas las llamadas del EXE los recibían al revés.
- `CTaskComplexUseSwatRope::ControlSubTask` descartaba el resultado de `FindZCoorForPed`.
- `CMirrors::TypeOfMirror`/`MirrorFlags` y `CAEExplosionAudioEntity::m_Speed` tenían otro tamaño que en el EXE.

**Lecciones:**
- Los inicializadores por valor (`: CMatrix()`, `CTaskTimer m_timer{}`) ponen a cero bytes que el EXE deja sin escribir (relleno, matriz): se usan constructores sin inicialización por valor.
- El EXE a veces mete en línea funciones que la DLL llama (p. ej. `CScriptedBrainTaskStore::GetTask`): se rellena el estado real en vez de simular la función.
- `std::array` aborta en depuración cuando el EXE lee fuera del array a propósito: `.data()[]`.
- Algunas funciones de RW de la DLL no son envoltorios del EXE (`RwCameraClear`, `RwCameraEndUpdate`): hay que simular también el símbolo de la DLL.
- Revisión EXE ↔ código y escritura de bancos repartidas entre agentes en paralelo; la compilación y las ejecuciones se mantienen en serie.

**Pendiente (fuentes corregidas contra el EXE, sin banco todavía):**
- `CAECollisionAudioEntity` (17), `CAECutsceneTrackManager` (8), `CAEBankLoader` (6).
- `CAEDataStream` (20), `CAEAudioHardware` (54).
- `CMenuManager` (61); el bloque de deslizadores de `DrawStandardMenus` no se revisó.
- Fuera de la lista: el tiempo de reproducción de canales y de `CAEStreamThread` es de 16 bits en la DLL y de 32 en el EXE.

## Continuidad vigente: lista de 56 clases de una función (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (56 funciones, 23.872 pares): [Pools56](Pools56/README.md), [EventsTasks56](EventsTasks56/README.md) y [Misc56](Misc56/README.md).

**Defectos de alcance general encontrados:**
- `CPool::New` de la DLL buscaba con otra política y rellenaba con 0xCD; los 14 pools usan ahora la del EXE (`NewLegacy`).
- El pool de atractores del EXE tiene slots de 0xC4 bytes y la DLL usaba 0x8C: indexaba mal el pool real.
- Los calculadores de posición de `SeekEntity` declaraban un destructor virtual que el EXE no tiene; tres hooks redirigían `ComputeEntitySeekPos` al destructor.
- La envoltura del constructor de `CEventScriptedAttractor` recibía un argumento de más (pila desbalanceada para el EXE).
- `CTaskComplexPartner` registraba la referencia sobre el parámetro, no sobre el miembro.

**Lecciones:**
- Los informes de los agentes pueden leer mal los registros (p. ej. `bl`/`al` en `CRegisteredCorona::Update`): el banco lo detecta; se corrige leyendo el desensamblado.
- `TIME_MS` es una macro de Windows: no usarla como nombre en los bancos.

## Continuidad vigente: lista de 70 clases de dos funciones (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (138 funciones, 51.824 pares; las otras 2 ya estaban validadas): [Events70](Events70/README.md), [Tasks70](Tasks70/README.md), [Core70](Core70/README.md) y [Misc70](Misc70/README.md).

**Defectos de alcance general encontrados:**
- `CDraw::CalculateAspectRatio` no devolvía el valor en ST0 y desbalanceaba la pila x87 de los 5 llamadores del EXE.
- `CMatrix::Reorthogonalise` sumaba en otro orden y comprobaba cero; ahora es una transcripción exacta del EXE.
- `CEventLeanOnVehicle::IsValid` se llamaba a sí misma sin fin con un peatón nulo.
- `CTaskAllocatorKillThreatsBasicRandomGroup` tenía dos virtuales instaladas como funciones normales (el hook recursaba).
- `CTaskComplexFollowLeaderAnyMeans` tenía un `DebugBreak()` en el constructor y registraba la referencia sobre el parámetro; `CTaskSimpleChat` no tenía destructor revertido.
- `CSimpleVariablesSaveStructure` guardaba como `bool` un byte que vale 2.
- `CPedGroupIntelligence::FlushTasks` está instalada como función cdecl, pero en el EXE termina en `ret 8` (pendiente, fuera de la lista).

**Lecciones:**
- El x87 silencia los NaN señalizadores al copiar con `fld`/`fstp`; la DLL los copia en crudo. Los fixtures aleatorios no deben generarlos (los datos reales nunca los tienen).
- La asignación miembro a miembro no copia el relleno; el EXE copia estructuras con `rep movs`.
- Cuando el EXE pasa memoria de pila sin inicializar (z de un `CVector`, argumento sin escribir), la diferencia se enmascara en el banco y se documenta.
- Las cadenas leídas sin inicializar (p. ej. nombres opcionales de un `sscanf`) rompen el JSON del banco con UTF-8 inválido: el fixture debe incluirlas siempre.
- Los stubs de `operator delete` no pueden escribir en el `json` de la traza (puede liberar su búfer con el propio stub).

## Continuidad vigente: lista de 52 clases de tres funciones (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (152 funciones, 55.248 pares): [Events52](Events52/README.md), [SimpleTasks52](SimpleTasks52/README.md), [PedTasks52](PedTasks52/README.md) y [World52](World52/README.md). `CPedToPlayerConversations::Update` ya estaba validada; los 3 `WindowedModeHooks` (NOTSA, modo ventana sobre D3D9) quedan fuera.

**Defectos de alcance general encontrados:**
- `CMatrix::SetRotate` redondeaba senos y cosenos antes de los productos; ahora es una transcripción exacta del EXE.
- `CEventSoundQuiet` es `final`: la DLL desvirtualizaba las llamadas sobre el evento actual del historial (cualquier evento pasaba por uno de sonido).
- El destructor con borrado de `CCivilianPed` no recibía su argumento (pila desbalanceada) ni liberaba el peatón.
- `CPlantLocTri::GetPlantDensity` recibe el triángulo en EAX; el hook era cdecl.
- `CTaskSimpleRunTimedAnim` escribía el tipo de tarea en 32 bits (pisaba el relleno) y no ponía el tipo en el constructor-1.
- `CSetPiece::Update` esperaba 4 s en vez de 40 entre generaciones de policías.
- Pendientes fuera de la lista: `CTaskAllocatorAttack::ProcessGroup` despacha `AllocateTasks` por la vtable en vez de llamar directamente; `PI`/`TWO_PI` de `common.h` son imprecisos.

**Lecciones:**
- En clases `final`, la DLL resuelve en compilación las virtuales sobre punteros de ese tipo: si el EXE despacha por la vtable de otro objeto (p. ej. un evento del historial), hay que llamar a través del tipo base.
- `small` es una macro de Windows (como `TIME_MS`): no usarla en los bancos.
- Un comentario `//` en medio de una línea larga del fixture se come el resto del código: usar `/* */`.
- Los fixtures de pilas (empujar/sacar) deben vaciarse antes del límite del array para no desbordar.

## Continuidad vigente: lista de 23 clases de cuatro funciones (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (92 funciones, 34.448 pares): [Render23](Render23/README.md), [Events23](Events23/README.md) y [Data23](Data23/README.md).

**Defectos de alcance general encontrados:**
- `common.h` declara los contadores del búfer temporal (0xC4B950/0xC4B954) como `uint16`, pero el EXE los usa como dwords con signo; las clases de esta lista usan referencias locales `int32` y otras pueden tener el mismo problema.
- `HALF_PI` (como `PI`) de `common.h` es impreciso: las constantes de ángulo se leen del EXE.
- `CEventStuckInAir::GetEventPriority` tenía la condición de la cola invertida.
- `CPlayerSkin::GetSkinTexture` usaba `GetAtRef` (referencia) con un índice de slot y siempre obtenía nulo.
- `CPlaneTrails::Update` nunca mostraba las coronas (`ClockHoursInRange(22,7)` nunca es cierto).
- Pendiente fuera de la lista: `CPlayerInfo::CancelPlayerEnteringCars` (0x56E860) está enganchada como cdecl estática, pero en el EXE es thiscall con `ret 4`.

**Lecciones:**
- El ensamblador en línea no distingue mayúsculas: variables como `fSin`/`fCos` chocan con las instrucciones `fsin`/`fcos`.
- Revisar qué bit lee el EXE en las banderas de `CPed` (p. ej. bit 8 de 0x46C = `bInVehicle`), no fiarse del nombre que usa la DLL.
- Los índices de modelo `int16` se guardan en dwords con `movsx`: extender con signo.

## Continuidad vigente: lista de 19 clases de tareas (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (76 funciones, 30.976 pares): [STasks19](STasks19/README.md) y [CTasks19](CTasks19/README.md).

**Defectos de alcance general encontrados:**
- `CTask::operator delete` rellenaba el slot con 0xDD y comprobaba nulo; el EXE solo marca el slot libre y baja `m_LastFreeSlot` (`CTaskPool::Delete` → `DeleteLegacy`; la comprobación de nulo queda bajo `FIX_BUGS`).
- `CTask::IsTaskPtr` aceptaba punteros al interior de un slot.
- Los constructores de `CTaskComplexCarDrive` y `CTaskComplexWander` escribían campos que el EXE no toca (y no escribían otros), lo que afecta a todas sus derivadas.
- `CTaskSimpleUseAtm` tenía el tipo de tarea de otra clase.
- `CTaskSimpleInAir::MakeAbortable` no quitaba `bIsInTheAir`.

**Lecciones:**
- Los constructores de las clases base se inlinean en los de las derivadas: validar una derivada obliga a revisar la base.
- Los miembros con inicializador por defecto (`CNodeAddress`) se pueden envolver en uniones anónimas para no escribir lo que el EXE deja sin tocar.

## Continuidad vigente: listas de 8 y 43 clases (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (201 funciones, 77.312 pares): [Misc8](Misc8/README.md), [RenderVideo8](RenderVideo8/README.md), [STasks43](STasks43/README.md), [CTasks43](CTasks43/README.md), [Events43](Events43/README.md) y [World43](World43/README.md). Después se completó [Fx43](Fx43/README.md) (44 funciones, 15.552 pares).

**Defectos de alcance general encontrados:**
- `RwGlobals` no tenía `fileFuncs` (11 punteros): `stringFuncs` y `memoryFuncs` quedaban 0x2C bytes desplazados, así que `rwsprintf`/`rwfree` de la DLL llamaban a otras funciones.
- `GetVideoModeList` compara la proporción exactamente con 0,8/0,75/0,6/0,625 (sin 16:9) y resta de la VRAM **total** (0xC8CF80), como el EXE.
- `CTaskAllocatorKillThreatsDriveby` medía 0x24 bytes en vez de 0x14 y leía la amenaza de un desplazamiento equivocado.
- `CTaskSimpleDie` (base de varias tareas) escribía las banderas completas y borraba bits de más en `StartAnim`.
- `CPlayerPedData` llamaba a `operator delete` con punteros nulos; `CTaskSimpleGangDriveBy` no ponía a cero `m_vecCoords`.
- Pendientes fuera de las listas: `CPlayerInfo::CancelPlayerEnteringCars` y `CPedGroupIntelligence::FlushTasks` están enganchadas como cdecl pero en el EXE limpian la pila; `IsPointInsideLine` de `common.cpp` difiere con NaN; los constructores de `FxInfo_c` derivados ponen a cero campos que el EXE no escribe.

**Lecciones:**
- El x87 conserva denormales (exponente extendido) que SSE vacía a cero: los productos que pueden caer en ese rango se hacen con ensamblador x87.
- El ensamblador en línea no se admite dentro de lambdas: usar funciones estáticas.
- Algunos `build_probe.py` antiguos reescribían las fuentes en modo texto y duplicaban los CR (aviso C4335 "formato Mac"): ahora escriben bytes.
- No ejecutar dos bancos a la vez: el segundo se niega a arrancar con GTA abierto y su "APROBADO" sería del ASI anterior.
- Los resúmenes de bancos antiguos tienen el hash de su versión CRLF: al restaurarlos con git hay que conservar esos finales de línea.

## Continuidad vigente: clases de 6 funciones (2026-10-02)

Bancos nuevos, todos con pares exactos dentro de GTA (305 funciones, 116.992 pares): [AnimAudio6](AnimAudio6/README.md), [Misc6](Misc6/README.md), [CTasks6a](CTasks6a/README.md), [CTasks6b](CTasks6b/README.md), [STasks6a](STasks6a/README.md), [STasks6b](STasks6b/README.md) y [STasks6c](STasks6c/README.md). `NOTSA_WinMain` queda fuera (punto de entrada del juego).

**Defectos de alcance general encontrados:**
- `CAnimBlendClumpData` liberaba la dirección del campo en vez del puntero de los frames.
- El "destructor" enganchado de `CAnimBlendStaticAssociation` (0x4CDF50) es un destructor con borrado (`ret 4`): la envoltura sin argumento desbalanceaba la pila.
- `CAEGlobalWeaponAudioEntity::UpdateParameters` estaba enganchada como función normal siendo virtual (podía entrar en bucle con la vtable del EXE); ahora es un hook de vtable. El mismo patrón aparece en `AESound.cpp` y `AEPedAudioEntity.cpp` (pendiente).
- `CPedPlacement::FindZCoorForPed` probaba los puntos en otro orden y aceptaba el suelo en -99 (el EXE exige `> -99`).
- `CTaskSimpleUseGun` (constructor) intercambiaba `m_BurstLength`/`m_BurstShots` y no escribía el byte de banderas +0x0D completo.
- `CTaskComplexWander::ControlSubTask` decía la frase con probabilidad 1,0 en vez de 0,1.
- `CTaskSimpleSetPedAsAutoDriver` encendía el motor al revés; `CTaskSimpleWaitUntilPedIsOutCar` usaba grados en vez de radianes; `CCredits` dibujaba la sombra en azul transparente en vez de negro.
- Pendientes fuera de las listas: `CPedGroupIntelligence::FlushTasks` (cdecl contra `ret 8`); `FurnitureManager_c` guarda contadores de 16 bits como `uint32`.

**Lecciones:**
- Un tamaño que se desborda (`n*4` con n = 0x40000000) tira por igual al EXE y a la DLL: los fixtures deben quedarse en rangos que el EXE soporta.
- Varios constructores de copia de la DLL delegaban en el constructor por defecto con `{}` y ponían a cero el relleno; el EXE solo escribe los campos.
- Un `bool` de la DLL normaliza a 0/1 un byte que el EXE copia tal cual.

## Continuidad vigente: clases de 7 funciones (2026-10-02)

Bancos aprobados (255 funciones, 84.160 pares exactos): [CTasks7a](CTasks7a/README.md), [CTasks7b](CTasks7b/README.md), [STasks7](STasks7/README.md), [Data7](Data7/README.md), [Audio7](Audio7/README.md) y [Mgr7](Mgr7/README.md).

**Defectos de alcance general encontrados:**
- `CStreamingInfo::GetCdPosn` devolvía `CdStreamPos` por un puntero oculto; el EXE lo espera en EAX (llamadas desde el EXE leían basura).
- `CTaskSimpleDie` usa el bit 5 de +0x478 (`bIsDyingStuck`), no el 4, en `MakeAbortable` y `ProcessPed`.
- `CTaskComplexDrivePointRoute` indexaba la ruta con una vista con comprobación de límites que aborta en depuración donde el EXE no comprueba.
- `CTaskSimpleStealthKill` no tenía destructor (la referencia al objetivo quedaba registrada).
- `CTaskSimpleKillPedWithCar` usaba el arma 49 en vez de 50 (atropello) y la velocidad del capó en vez de la del peatón.
- `CTaskInteriorUseInfo` (destructor) no comprobaba el puntero de la información antes de escribir (0x675A93).
- `CAEFireAudioEntity` cargaba otro banco en otra ranura; la constante 0,06 del EXE no es el float más cercano (0x3D75C290).
- `FurnitureManager_c::GetGroupId`/`GetSubGroupId` son `thiscall` en el EXE y estaban enganchadas como `cdecl` estáticas.

**Mejora del proceso:**
- El ejecutor común (`runtime_runner.py`) aborta ahora cuando la ventana de GTA queda en "No responde" y además el registro lleva 30 s sin crecer, o cuando el registro deja de crecer 120 s (240 s antes del primer resultado), además de los cuadros de diálogo. Como los bancos corren dentro de un solo fotograma, Windows marca la ventana "No responde" aunque avancen: la ventana colgada sola no basta. Antes un cuelgue esperaba hasta el tiempo límite completo.

**Lecciones:**
- Construir json o cualquier objeto con memoria dinámica mientras `operator new` está sustituido corrompe el montón (cuelgues o 0xC0000409): guardar resultados en locales y serializar después de quitar los parches.
- Un comentario `//` dentro de una línea larga de stub puede tragarse el código que escribe la salida: los dos lados leen basura distinta.
- Las direcciones de búferes en la pila deben registrarse como "stack".

## Mejora del proceso: bancos que no parecen colgados (2026-10-03)

- **Ceder el control a Windows:** los bancos ejecutan todos sus casos dentro de un solo fotograma y Windows marcaba la ventana "No responde" a los ~5 s, aunque avanzaran. `ProbeSupport.hpp` tiene ahora `ReversingProbe::PumpMessages()`: como mucho cada 250 ms llama a `PeekMessageA(..., PM_NOREMOVE)`, que basta para que el hilo cuente como atendido sin sacar ni despachar mensajes (sin reentradas ni efectos en el juego). Se llama al principio del `Write` de los 89 bancos que usan `ReversingProbe`; los 15 antiguos que no lo incluyen llevan la misma lógica en línea. Los bancos nuevos deben hacerlo igual (regla en el encargo para agentes).
- **Detección de cuelgues reales:** `runtime_runner.py` aborta si la ventana sigue "No responde" y además el registro lleva 30 s sin crecer, o si el registro no crece 120 s (240 s antes del primer resultado), además de los cuadros de diálogo; cierra GTA y restaura la copia como siempre.
- **Bancos antiguos y "Device Selection":** los 40 bancos de estilo antiguo (sin `probe_case`) ejecutan una sonda congelada en `_baseline` con hash fijo, anterior a `SkipDeviceSelection`. Con más de un subsistema de vídeo visible (situación de esta máquina desde el 02/10 23:56) esa sonda muestra el cuadro "Device Selection" y el banco se aborta. No prueban el código actual, así que la regresión amplia (`reversing/_handoff/regress.sh`) corre solo los 60 bancos modernos, que compilan e instalan la sonda del árbol actual. Su evidencia histórica no cambia.
- **Detener una regresión sin restos:** parar el script no mata los `python` hijos. Cerrar GTA y esperar a que `run_local.py`/`build_probe.py` terminen solos (restauran la copia y las fuentes); cortar una compilación a la mitad dejó instrumentación en `Game.cpp` y `app_debug.cpp`.

## Continuidad vigente: clases de 8 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (351 funciones, 119,360 pares): [Data8a](Data8a/README.md), [Data8b](Data8b/README.md), [Fx8](Fx8/README.md), [CTasks8a](CTasks8a/README.md), [CTasks8b](CTasks8b/README.md), [CTasks8c](CTasks8c/README.md), [STasks8a](STasks8a/README.md), [STasks8b](STasks8b/README.md).

**Defectos de alcance general encontrados:**
- **Dos patrones de bucle infinito entre hooks** (cuelgues reales en los bancos):
  - una función virtual enganchada con `RH_ScopedInstall` (hook normal): el símbolo de la DLL es un trampolín que vuelve a llamar por la vtable del EXE al punto enganchado (`CEventScriptCommand`, como antes `UpdateParameters`); deben usar `RH_ScopedVMTInstall`;
  - dos hooks del EXE que apuntan a la misma función heredada de la DLL (`CTaskComplexFleeShooting`/`CTaskComplexSmartFleeEntity::CreateNextSubTask`): si se desactiva uno, rebotan sin fin; cada clase necesita su propia función mínima que llame a la base, como el EXE.
- `CTaskSimpleBeHit` registraba la referencia al atacante sobre la copia del parámetro en la pila (referencia colgante que corrompe la pila cuando el atacante desaparece).
- `CTaskComplexLeaveBoat`: el destructor estaba enganchado en el destructor con borrado (0x642250); el real es 0x63BBD0.
- Convenciones de llamada distintas al EXE (pila descuadrada): `CTaskInteriorBeInOffice::GetInfoForPedToUse`, `CTaskComplexMoveBackAndJump::CreateSubTask`; `CCheckpoints::PlaceMarker` recibe el color como 4 bytes sueltos (12 argumentos cdecl).
- `CRect::IsPointInside` trataba distinto un NaN (afecta a todo el juego); `CCarGenerator` nunca detectaba la noche (`ClockHoursInRange(21,7)`); `CTaskSimpleBikeJacked` no guardaba al atacante; `CCollisionData` liberaba punteros nulos.
- Pendiente fuera de las listas: `CColBox::operator=` no copia `m_nPiece` (el EXE copia 27 bytes).

**Lecciones:**
- Un `bool` de retorno de la DLL frente al byte crudo del EXE (p. ej. 0x80): los llamadores solo comprueban distinto de cero; los bancos comparan esos retornos como verdadero/falso.
- Hilos de audio del juego pueden llamar a los stubs de un banco: no simular funciones que otros hilos usan sobre objetos reales.

## Continuidad vigente: clases de 9 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (351 funciones, 127.232 pares): [Core9](Core9/README.md), [Models9](Models9/README.md), [Ped9](Ped9/README.md), [CTasks9a](CTasks9a/README.md), [CTasks9b](CTasks9b/README.md), [CTasks9c](CTasks9c/README.md), [CTasks9d](CTasks9d/README.md), [STasks9](STasks9/README.md).

**Defectos de alcance general encontrados:**
- **`SetBaseModelInfoFlags` (0x5B3AD0) y `SetClumpModelInfoFlags` (0x5B3C30)** estaban enganchadas como métodos `thiscall` cuando en el EXE son `cdecl` de dos argumentos: cada llamada del EXE descuadraba la pila 4 bytes, también en juego normal. Además `SetClumpModelInfoFlags` del EXE sigue usando EDX tras llamar a `SetBaseModelInfoFlags`. Ahora se enganchan con envolturas `naked` cdecl que conservan EDX. `SetBaseModelInfoFlags` también activaba `bDrawLast` solo con el bit 2 (el EXE usa la máscara 0xC).
- **`CMemoryMgr::MallocAlign(size, align)`** llamaba al `MallocAlign` del EXE (3 argumentos) con 2: el EXE leía basura de la pila como hint en toda reserva alineada.
- Hooks en direcciones equivocadas o virtuales mal enganchadas: `CTaskComplexSeekEntity<XYOffset>::CreateSubTask` (0x496DC0 era otra función; la real es 0x665270) y las virtuales de `CAEWaveDecoder` (hook normal en vez de vtable: bucle infinito).
- `CAEWaveDecoder` era una versión distinta del decodificador (búsqueda de chunk `data`, conversión mono); ahora sigue al EXE.
- Constructores con inicialización por valor que ponían a cero relleno que el EXE no toca: `CTaskComplexSequence` creada con llaves vacías, el temporizador de `CTaskSimpleCarDriveTimed` y varias tareas de esta lista.
- Referencias sin registrar o mal limpiadas: `CTaskSimpleCarFallOut`, `CTaskComplexEvasiveDiveAndGetUp`, `CTaskComplexCopInCar::Clone`; `CTaskComplexInvestigateDeadPed` nunca guardaba el peatón muerto.
- Pendiente fuera de las listas: `CWeaponInfo::m_nAimOffsetIndex` es `uint16` en la DLL y el EXE lo lee con signo; `CDamageManager::GetDoorStatus` devuelve un byte y el EXE prueba EAX completo.

**Lecciones:**
- Comprobar siempre cuántos argumentos y qué registros usa la función del EXE antes de enganchar un método: un método `thiscall` no sustituye a una función `cdecl` aunque haga lo mismo.
- Los asserts NOTSA de la DLL (grupos activos, raster de caracteres, habilidad de armas) saltan con fixtures que el EXE acepta: se quita el assert si no existe en el EXE, o el fixture se queda dentro.
- El registro (`spdlog`) reserva memoria: un banco que sustituye `operator new` no debe hacerlo durante funciones que escriben en el log.

## Continuidad vigente: clases de 10 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (206 funciones, 76.864 pares): [Sys10](Sys10/README.md), [Data10](Data10/README.md), [CTasks10a](CTasks10a/README.md), [CTasks10b](CTasks10b/README.md), [STasks10](STasks10/README.md).

**Defectos de alcance general encontrados:**
- `CPedGroups::GetGroup` tenía un assert NOTSA (grupo activo) que el EXE no tiene; saltó en tres oleadas y podía cerrar el juego. Ahora indexa como el EXE.
- `CConversationNode::ClearRecursively` (0x43A710): los llamadores del EXE conservan un puntero en EDX a través de la llamada; la versión C++ de la DLL lo estropeaba y en juego se limpiaban conversaciones equivocadas. Se engancha con una envoltura que conserva EDX (mismo patrón que `SetBaseModelInfoFlags`).
- `CAEMFDecoder` y `CAEWaveDecoder` enganchaban sus virtuales como funciones normales (bucle infinito); ahora son hooks de vtable con la vtable exportada.
- Referencias registradas sobre la copia del parámetro en la pila (referencia colgante): `CTaskSimpleCarOpenDoorFromOutside` (la DLL), y `CTaskComplexDestroyCarArmed` (**el propio EXE**: se conserva la versión correcta bajo `FIX_BUGS` y la del EXE en el `#else`).
- `CWeaponInfo::m_nAimOffsetIndex` pasa a `int16` (el EXE lo lee con signo); `GetWeaponReloadTime` calculaba otra cosa; `FxSystemBP_c::Load` reservaba los emisores en el montón general en vez del pool de efectos.
- `CAESmoothFadeThread::Service` olvidaba el `+1` del logaritmo del fundido.

**Lecciones:**
- Hay funciones del EXE que respetan EDX y sus llamadores lo usan después: al sustituirlas por C++ hay que comprobar los llamadores y, si dependen de EDX, usar una envoltura `naked` que lo conserve.
- Cuando el propio EXE tiene un bug (p. ej. referencia sobre la pila) y la DLL lo corrige bajo `FIX_BUGS`, el banco normaliza esa diferencia concreta y el README del banco la documenta.

## Continuidad vigente: clases de 11 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (164 funciones, 62.528 pares): [CTasks11](CTasks11/README.md), [Audio11](Audio11/README.md), [World11](World11/README.md), [Mix11](Mix11/README.md).

**Defectos de alcance general encontrados:**
- `CAEWMADecoder` enganchaba sus virtuales como funciones normales (bucle infinito del thunk); ahora usa `RH_ScopedVirtualClass` con la vtable exportada.
- `CClothes::GetTextureDependency` (0x5A7EA0): `Init`, `SetTextureAndModel` y 0x5A8626 del EXE conservan ECX/EDX a través de la llamada; era la causa del comentario "crashes when enabled". Ahora se engancha con una envoltura `naked` que conserva ambos registros.
- `CEntryExit::GetPositionRelativeToOutsideWorld` y `CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange`: llamadores del EXE que dependen de EDX (y ECX); también con envoltura `naked`.
- `cHandlingDataMgr::GetVehiclePointer` usaba indexado comprobado (`0xC0000409` con ids fuera de rango); ahora indexa sin comprobar como el EXE. `LoadHandlingData` se reescribió como el `switch` de `strtok`/`atof` del EXE.
- `CRestart::FindClosestHospitalRestartPoint` leía el índice −1 si no había punto válido (`closestIdx >= 0u` siempre cierto).
- `CTaskComplexCarSlowBeDraggedOut`: el EXE borra la utilidad de alineación sin poner el campo a nulo y solo si existe; un `std::unique_ptr` no reproduce eso.
- `CTaskSimpleSwim::ProcessControlInput` tenía la dirección de marcha permutada (`(0,-sin,cos)` en vez de `(-sin,cos,0)`).

**Lecciones:**
- Los falsos del banco deben escribir parámetros de salida con el tamaño exacto del tipo de la DLL: un `int32` escrito sobre un `int16` local disparó un fallo RTC de pila.
- Las trazas JSON no deben reservar memoria mientras `operator new/delete` están sustituidos por una arena falsa (se liberan después con el `free` real).
- Una bugfix reversible NOTSA activa por defecto (`PS2CoronaRendering`) se desactiva durante el banco en lugar de tocar el fuente.
- Los datos de relleno con denormales producen diferencias de doble redondeo x87 ajenas a la función; en `LoadHandlingData` el relleno usa flotantes normales.
- `CTaskSimpleSwim::ApplyRollAndPitch` llama a `CMatrix::RotateX/RotateY` del EXE (0x59B1E0/0x59B2C0), que la DLL no engancha y cuya versión propia redondea distinto: pendiente transcribirlas exactas.

## Continuidad vigente: clases de 12 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (106 funciones, 39.200 pares): [Sys12](Sys12/README.md), [World12](World12/README.md), [Mix12](Mix12/README.md). Sin cubrir: `Game::Idle` y `Game::FrontendIdle` (alojan el propio banco y contienen código NOTSA de ImGui y carga rápida).

**Defectos de alcance general encontrados:**
- `CPickup::IsVisible`: el llamador del EXE en 0x458E42 reutiliza EDX tras la llamada (la nota "something broken" del hook); ahora con envoltura `naked`.
- `CPickup::Update`: la regeneración en `ON_STREET_SLOW` era 10 veces más corta (30000/36000 ms en vez de 300000/360000), la propiedad en venta no se compraba y `StaticCamCoors` solo reasignaba el puntero de la DLL.
- `CExplosion`: `SetCreator`/`SetVictim` registraban la referencia sobre el parámetro y no sobre el miembro; la caducidad usaba una resta sin signo (`<= 0` roto); el byte de sonido se guarda tal cual.
- `CTrafficLights::ShouldCarStopForBridge` tenía invertida la comprobación de zona cargada; `LightForPeds` terminaba el verde un milisegundo antes.
- `CTaskComplexWalkRoundBuildingAttempt::ComputeMoveDir` proyectaba desde la posición equivocada y `ControlSubTask` transformaba `offset − objetivo` en vez de `(matriz·offset) − objetivo`.
- `CStreamedScripts::ReadStreamedScriptData` y `CQuadTreeNode<T>` (pool y NaN) reescritos según el EXE; `Game::RenderDebugShit` queda vacío como en el EXE (se pierde el dibujo NOTSA de líneas de depuración de scripts).

**Lecciones:**
- Una función con un `HookInstall` permanente además del hook reversible (`Render2dStuff`, por ImGui) no se puede apagar para la pasada original: se cuelga en bucle. El banco restaura temporalmente los 5 bytes originales de la entrada del EXE solo durante la llamada.
- Las escrituras en `validation-registry.json` pueden fallar por un bloqueo pasajero del fichero (`OSError 22`); basta con repetir el registro.

## Continuidad vigente: clases de 13 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (116 funciones, 43.776 pares): [Audio13](Audio13/README.md), [CTasks13](CTasks13/README.md), [Sys13](Sys13/README.md).

**Defectos de alcance general encontrados:**
- `CAEVorbisDecoder` enganchaba sus virtuales como funciones normales (bucle infinito); ahora hooks de vtable con la vtable exportada. Quedan bajo `FIX_BUGS` el rechazo de más de 2 canales y el tratamiento mono, que el EXE no tiene (pendiente de decisión del usuario).
- `CTaskComplexGangLeader`: `TryToPassObject` es cdecl estática en el EXE y la DLL la tenía como thiscall (descuadraba la pila de los llamadores); `ControlSubTask` ponía la subtarea como tarea secundaria casi siempre; `ScanForStuff` borraba su evento dos veces. Son causas probables del crash por el que sus hooks siguen desactivados y bloqueados.
- `CTimer::UpdateVariables`: la DLL calculaba el paso sin recortar con SSE y aplanaba los denormales a 0; ahora es asm copiado del EXE.
- `CTrailer::ProcessAI` comprobaba el estado 8 en vez de 10 (remolcado); `SetTowLink` usaba un `clamp` con los límites invertidos; `SetupSuspensionLines` escribía 2 de las 4 ruedas.
- `CTaskComplexFollowPointRoute::CalcGoToTaskType` borraba el bit equivocado; `CTaskComplexSequence::Contains` desreferenciaba huecos nulos y su `MakeAbortable` compartía destino con otro hook.
- `CPedType::LoadPedData` comparaba palabras clave por prefijo.

**Lecciones:**
- Un constructor `= default` usado como `T{}` inicializa a cero todo el objeto (incluidos bits que el EXE conserva); se declara en la cabecera y se define `= default` en el `.cpp`.
- Para el hilo de streaming de audio: el banco lo aparca durante toda la corrida (al apagar un hook de vtable la vtable de la DLL apunta al EXE) y los parches viven solo durante cada pasada.
- `0x406DA0` es `SquaredMagnitude`, no `Magnitude`: comprobar si hay `fsqrt` antes de suponer distancias.

## Continuidad vigente: clases de 14 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (138 funciones, 53.056 pares): [Audio14](Audio14/README.md), [Render14](Render14/README.md), [Mix14](Mix14/README.md), [CTasks14a](CTasks14a/README.md), [CTasks14b](CTasks14b/README.md).

**Defectos de alcance general encontrados:**
- `SArray` (cabecera compartida): `erase` usaba `move_backward` y escribía delante del elemento borrado; el crecimiento y la reserva siguen ahora al EXE (capacidad + capacidad/2, `operator new`/`delete`, sin borrar nulos).
- `CDamageManager::GetDoorStatus` / `GetDoorStatus_Component`: el EXE devuelve el byte extendido a 32 bits y sus llamadores comparan EAX completo; la DLL dejaba basura en los bits altos (puertas que no se abrían al caer del coche, etc.). Ahora se enganchan envoltorios que extienden el resultado y comparan el argumento completo, como el EXE.
- `CPedPizzaAttractor`: máximo de peatones 5 y rango de llegada 0,2 (la DLL tenía 1 y 0,125).
- `CAnimBlendSequence::Uncompress` desbordaba el montón (reservaba el tamaño comprimido y escribía el descomprimido); `SetBoneTag` comparaba solo 16 bits con -1.
- `CAEStreamThread`: tiempos y duraciones de pista recortados a 16 bits (pistas de más de 65 s).
- `CAESoundManager::PauseManually` y `CExplosion` (lista 12): bytes que el EXE guarda tal cual y la DLL convertía a `bool`.
- `CTaskComplexWander::UpdateDir`: dirección inicial tomada del byte bajo de `m_NextNode` (reutilización de pila del EXE).
- `CTaskSimpleJetPack::ProcessPed`: un error de precedencia dejaba la deriva de la IA en ±1.

**Lecciones:**
- spdlog con `enable_backtrace` reserva memoria aunque el nivel esté apagado: ningún log de la DLL puede ejecutarse mientras `operator new` está sustituido (se usa el `printf` del EXE como el original).
- Los falsos instalados durante toda una función reciben llamadas de otros hilos (p. ej. `GetCurrentTimeInMS`); deben registrar solo en el hilo principal y vivir solo en las funciones que los usan.
- Un stub que responde distinto en dos llamadas seguidas que en el juego dan lo mismo (`GetTaskDuck`) puede hacer que el EXE desreferencie nulo.
- Las bugfixes reversibles activas por defecto (`GeneratePlateText`, `PS2CoronaRendering`) se desactivan durante el banco.

## Continuidad vigente: clases de 15 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA (105 funciones, 48.032 pares): [Audio15](Audio15/README.md), [World15](World15/README.md), [CTasks15](CTasks15/README.md).

**Defectos de alcance general encontrados:**
- `CPopCycle`: `UpdateDealerStrengths` tenía la comprobación de minutos invertida y recorría las 380 zonas; `UpdatePercentages` tomaba el máximo en vez del mínimo para los camellos; `IsPedAppropriateForCurrentZone` comprobaba un puntero (siempre cierto).
- `CSprite::RenderBufferedOneXLUSprite2D` usaba el ancho como alto; los sprites no almacenados escriben en el array global de vértices del EXE.
- `CAnimBlendAssocGroup`: el envoltorio del destructor tomaba un argumento que el EXE no tiene (`ret 4`); `IsCreated` miraba el contador.
- `CAEUserRadioTrackManager`: tabla de extensiones del EXE (faltaba `.mov`); el escaneo ANSI del EXE queda tras la nueva bugfix reversible `CAEUserRadioTrackManager_UnicodeUserTracks` (activa por defecto, conserva el UTF-8).
- `CTask::IsGoToTask` (base compartida): el EXE consulta el tipo una vez por comparación; ahora la DLL también.
- `CTaskSimpleClimb::GetCameraTargetPos` mezclaba desplazamientos horizontales y verticales; `CTaskSimpleDuck::AbortBecauseOfOtherDuck` desreferenciaba la animación ya anulada; `CTaskComplexAvoidOtherPedWhileWandering::Clone` leía los campos del objeto nuevo.

**Lecciones:**
- Los bytes que `CMemoryMgr::Malloc` deja sin escribir (bits altos de las banderas y 0x20..0x2F de `CCollisionData`) se enmascaran: son basura en ambos lados.
- Las entidades falsas sin matriz hacen que `GetMatrix` reserve del pool real en cada pasada (direcciones distintas y fugas): darles una matriz preasignada.
- Los recorridos de listas de la DLL validan nodos y entidades contra los pools; con listas falsas se recorre como el EXE (sin validar).
- Una función que pasa con pocos casos alcanzando una rama (p. ej. `GetTaskUseGun` casi siempre nulo) puede esconder una llamada duplicada del EXE.

## Continuidad vigente: clases de 16 y 17 funciones (2026-10-03)

Banco nuevo con pares exactos dentro de GTA: [Anim16](Anim16/README.md) (32 funciones, 14.080 pares).

**Defectos de alcance general encontrados:**
- `CAnimBlendNode::CalcTheta` es cdecl con 4 argumentos en el EXE (la DLL lo enganchaba como método con un float); `FindKeyFrame` comparaba siempre con el fotograma 1; `CalcDeltasCompressed` negaba el fotograma equivocado; la interpolación de traslación seguía otra fórmula.
- `CAnimBlendHierarchy::MoveMemory` y `CAnimBlendNode::NextKeyFrameNoCalc` no estaban implementadas.

**Lección:** el propio ayudante de parches del banco libera memoria al desmontarse; el `delete` falso solo debe registrar punteros de la arena del banco y reenviar el resto al `free` real.

Bancos nuevos tras la pausa, todos con pares exactos dentro de GTA: [World16](World16/README.md) (32 funciones, 19.680 pares), [Audio17](Audio17/README.md) (17 funciones, 11.088 pares), [Sys17](Sys17/README.md) (34 funciones, 13.248 pares).

**Defectos de alcance general encontrados:**
- `CCover::FindAndReserveCoverPoint` elegía el punto más lejano (casi siempre devolvía nulo); `CalculateHorizontalSize`, `DoLineCheckWithinObject` y `Find2HighestPoints` no estaban implementadas.
- `CPlaceable::GetRoll` usaba la magnitud al cuadrado y no seguía la regla de signo del EXE; `GetHeading`/`GetRoll` devuelven ahora ST0 sin redondear y el heading de las matrices usa `fpatan`.
- `CFileMgr`: rutas con `strcpy`/`strcat` sin `memset`, `LoadFile` por bloques de 0x4000 con terminador; `IKChainManager_c::CanAccept`/`LookAt` en el orden del EXE y sin comprobar el tipo de las tareas IK.
- `CAECollisionAudioEntity::ChooseCollisionSoundID` se rendía a los 100 sorteos (NOTSA); el EXE repite sin límite.

**Lecciones:**
- `PtrListDoubleLink::IsNodeValid` (DLL de depuración) comprueba el nodo contra su pool **y el elemento contra el pool de edificios**: las listas de edificios falsos necesitan nodos del pool real y un pool de edificios falso que cubra las entidades.
- `CVector v = {a, b, c}` es ambiguo (`operator=` de `CVector` y de `RwV3d`): escribir `CVector{...}`.

## Arreglos fuera de las listas (2026-10-03)

Bancos nuevos con pares exactos dentro de GTA: [FixAudio](FixAudio/README.md) (7 funciones, 5.120 pares), [FixCore](FixCore/README.md) (17 funciones, 11.776 pares), [FixPeds](FixPeds/README.md) (21 funciones, 9.664 pares). Se repitieron Audio13 (Vorbis sin `FIX_BUGS`) y CTasks13 (hooks de `CTaskComplexGangLeader` activados): aprobados.

**Defectos corregidos (decisión del usuario: lo que sea parte del EXE):**
- Audio:
  - `CAEVorbisDecoder` sin los `FIX_BUGS` de canales;
  - `CAESound::UpdatePlayTime` suma en float y luego trunca;
  - `GetPlayTime`/`GetLength` de los canales devuelven 32 bits;
  - `CAECollisionAudioEntity::UpdateParameters` reconstruida (hueco 0 de 0x862E64);
  - `CAEPedAudioEntity::UpdateParameters` pasa de hook plano a hook de vtable.
- Núcleo:
  - `CMatrix::RotateX/Y/Z` y `SetRotateX/YOnly` son copias exactas del EXE (sin el argumento NOTSA `bKeepPos`), y `CVehicle::SetComponentRotation` se reescribió en consecuencia;
  - `CPool::New/Delete/NewAt` siguen al EXE (búsqueda desde `m_LastFreeSlot+1`, sin rellenos 0xCD/0xDD);
  - `CColBox::operator=` copia `m_nPiece`;
  - los contadores del búfer temporal son `int32`.
- Peatones:
  - las cuatro virtuales de `CPedAttractor` pasan a hooks de vtable, y se corrigió el cuerpo (cola contra lista de atraídos, `ms_tasks`, ranuras);
  - `CPedGroupIntelligence::FlushTasks` y `CPlayerInfo::CancelPlayerEnteringCars` pasan a thiscall.
- Sin cambios: los contadores de `FurnitureManager_c` ya eran de 16 bits sin signo, como en el EXE, y `SetRotateZOnly` ya era exacta.

**Lecciones:**
- Comprobar el conmutador del hook (`JumpsToDll`) **después** de activar los hooks de la pasada; si no, todos los casos fallan en `switches`.
- `reversing/_handoff/dup_hooks.py` detecta funciones de la DLL enganchadas desde dos direcciones del EXE y virtuales con hook plano.
- `probe_summary.py` admite funciones sin fila en `hooks.csv` (`CMatrix`, `CColBox`, que no instalan hooks): quedan en el banco pero no se registran.

## Continuidad vigente: clases de 18 y 19 funciones (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA: [FixHooks](FixHooks/README.md) (20 funciones, 6.144 pares), [Audio18](Audio18/README.md) (16, 9.216), [World18](World18/README.md) (17, 7.808), [Group18](Group18/README.md) (18, 10.112), [Tasks18](Tasks18/README.md) (18, 12.160), [Plant19](Plant19/README.md) (19, 10.688), [Render19](Render19/README.md) (37, 9.920), [Game19](Game19/README.md) (38, 17.792), [Route19](Route19/README.md) (19, 11.584), [Gun19](Gun19/README.md) (18, 9.920).

**Defectos de alcance general encontrados:**
- Hooks: dos funciones de la DLL enganchadas desde dos direcciones del EXE (`ControlSubTask` de `EvasiveDiveAndGetUp`/`DiveFromAttachedEntity` y `Wander`/`WanderFlee`) y 47 virtuales con hook plano (entre ellas los 14 métodos IStream de `CAEDataStream` y `CEntity::Add(rect)`), pasadas a hooks de vtable.
- `CTaskManager::ManageTasks` hace hasta 11 pasadas; borrar y luego anular la ranura; `HasTaskSecondary(nullptr)` es cierto con una ranura vacía.
- `CPedGroupMembership`: los seguidores ocupan las ranuras 0..6 (la DLL podía llenar la del líder); `RemoveMember` limpia la referencia antes de mirar al líder y usa la referencia del pool para el blip; `Process` compara el área.
- `CEntryExitManager`: el pool se recorre del último al primero; `LinkEntryExit` tenía la condición del enlace inverso invertida; `EnableBurglaryHouses` escribía la bandera equivocada en todas las entradas.
- `CGlass`: 45 paneles y `PanePolyPositions[5][3]`; `HasGlassBeenShatteredAtCoors` miraba el bit equivocado. `CShadowCamera::Destroy` desenganchaba el marco en vez de la cámara.
- `CPlantMgr::UpdateAmbientColor` dejaba todos los canales a 255; `CDarkel::RegisterKillByPlayer` podía contar dos veces una muerte; `CTagManager` recorre las etiquetas de la última a la primera.
- `CTaskComplexFollowNodeRoute`: la mezcla de velocidad estaba invertida y los NaN tomaban la rama contraria; `CTaskSimpleUseGun::AimGun` leía los datos de jugador del objetivo en vez del que apunta.

**Lecciones:**
- `test ah,41h; jne` salta con menor, igual o NaN; `test ah,5; jp` deja pasar solo el «menor» estricto. Los fallos con NaN suelen venir de leer mal estas pruebas.
- Las direcciones de variables locales en las trazas difieren entre EXE y DLL: el banco las debe mostrar como `stack`.
- Un hook de vtable también escribe un salto en la entrada del EXE: los parches del banco sobre esa misma entrada se ponen después de fijar el estado de los hooks y se quitan antes de cambiarlo.
- Con `CPool::New` como el EXE (desde `last+1`), restaurar el `m_LastFreeSlot` de todos los pools tocados (también el de nodos del quadtree) en cada pasada.
- Los PDB de las sondas ocupan ~800 MB por banco: `bench.sh` los borra al terminar (llegaron a llenar D:).
- Agentes editando mientras corre la regresión producen BUILD FAIL falsos: lanzar la regresión cuando no haya agentes trabajando.

**No validables por pares:** `CAEMFDecoder::Initialise/FillBuffer/InitLibrary` (el EXE usa QuickTime; la DLL, Media Foundation) y `CGlass::CalcAlphaWithNormal` (la rama de la DLL es `FIX_BUGS`; la versión exacta del EXE ya está escrita en la rama sin `FIX_BUGS`).

## Continuidad vigente: clases de 20 funciones y `FIX_BUGS` quitados (2026-10-03)

Bancos nuevos, todos con pares exactos dentro de GTA: [Anim20](Anim20/README.md) (20 funciones, 10.368 pares), [Time20](Time20/README.md) (20, 7.488), [IK20](IK20/README.md) (19, 11.264), [Event20](Event20/README.md) (20, 6.976), [Audio20](Audio20/README.md) (24, 10.176), [FixBugs](FixBugs/README.md) (21, 10.304).

**Decisión del usuario:** las funciones validadas se comportan como el EXE. FixBugs quitó 52 bloques `FIX_BUGS` (50 en funciones validadas, más `CGlass::CalcAlphaWithNormal` y `CStreaming::GetNextFileOnCd`); la tabla completa está en su README. Vuelven fallos del original: el bloqueo raro del streaming de `CdStream`, la escalada sin tope de velocidad a más de 60 fps y referencias colgantes en `CCover`, `CarFallOut` y `DestroyCarArmed`. Los 15 bugfixes reversibles siguen activos y solo se cambian en el menú de depuración (Settings → Bugs).

**Defectos de alcance general encontrados:**
- `CAEWeaponAudioEntity` usaba la vtable de `CAEPedWeaponAudioEntity` y sonidos locales en vez de `m_tempSound`.
- `CTimeCycle::CalcColoursForPoint`: lectura de bytes de las tablas de clima, mezcla del túnel con la entrada equivocada y senos/cosenos sin redondear.
- `IKChain_c::IsFacingTarget` restaba el efector normalizado en vez del hueso raíz; `MoveBonesToTarget` replica el `acos` del CRT del EXE.
- `CAnimBlendAssociation::UpdateBlend` siempre ponía la callback de fin (el EXE vuelve a leer el tipo).
- `CEventDamage::ComputeDeathAnim`/`ComputeDamageAnim`: animaciones elegidas con otras reglas, fuerza con la dirección 3D en vez de 2D.

**Lecciones:**
- `CPedAcquaintanceScanner` está definida en `EventScanner.h` y en su propio encabezado: los bancos no deben incluir los dos.
- `TaskStop` no mata el `bash.exe` de Windows que ejecuta `regress.sh`: hay que cerrarlo con `taskkill` (no cerrar GTA, que solo aborta el banco en curso).
- Una función `void` en el EXE deja basura en EAX: no se compara su retorno.

## Continuidad vigente: clases de 9, 21, 22, 23 y 24 funciones (2026-10-04)

Bancos nuevos, todos con pares exactos dentro de GTA: [WeapAudio](WeapAudio/README.md), [Buoy9](Buoy9/README.md), [Clouds21](Clouds21/README.md), [Zones21](Zones21/README.md), [Garages21](Garages21/README.md), [Interior21](Interior21/README.md), [CarCtrl22](CarCtrl22/README.md), [Game22](Game22/README.md), [Ps22](Ps22/README.md), [Boat23](Boat23/README.md), [ColStore23](ColStore23/README.md), [Clump24](Clump24/README.md), [Fire24](Fire24/README.md), [RwHelper24](RwHelper24/README.md).

**Defectos de alcance general encontrados:**
- Catálogo: el "constructor" de `CAEWeaponAudioEntity` era el de `CAEPedWeaponAudioEntity` y su "destructor" era su constructor real; corregido.
- `CClumpModelInfo` comparaba nombres de marcos con `strcmp`; el EXE usa `_stricmp`.
- `CGame::Process` dividía el paso de tiempo entre 50 (el EXE multiplica por 0.02); `CameraDestroy` de `Game.cpp` leía rasters ya destruidos.
- `CCullZones`/`CTheZones` recorrían arrays enteros en vez del número de zonas; `GetLevelFromPosition` devuelve 32 bits.
- `cBuoyancy`: orden de sumas de los productos de matriz; la salpicadura +X usaba otros puntos.
- `psSelectDevice` ya no usa la resolución nativa (`FIX_BUGS` quitado): busca 800x600x32 como el EXE.

**Lecciones:**
- Un ayudante de parches que sigue el salto del símbolo de la DLL **después** de parchearlo acaba parcheando el propio stub con un salto a sí mismo: bucle infinito en la primera llamada ("GTA no responde"). Resolver el cuerpo antes de escribir ningún parche y no parchear nunca un stub registrado.
- Para localizar cuelgues: migas de pan escritas en vivo con un solo `WriteFile` sin reservas (`FILE_FLAG_WRITE_THROUGH`).
- Los stubs registran en un array fijo y el json se construye tras la llamada.
- `run_local.py` lanza el juego aunque se le pase `--help`: los agentes no deben ejecutarlo nunca.

## Continuidad vigente: clases de 25 a 33 funciones (2026-10-04)

Bancos nuevos, todos con pares exactos dentro de GTA: [CarAI25](CarAI25/README.md), [Fx25](Fx25/README.md), [PGI26](PGI26/README.md), [Txd26](Txd26/README.md), [EnExInit](EnExInit/README.md), [Damage27](Damage27/README.md), [GameLogic27](GameLogic27/README.md), [Light27](Light27/README.md), [Rs27](Rs27/README.md), [Player28](Player28/README.md), [VehRec29](VehRec29/README.md), [Shop30](Shop30/README.md), [AnimMgr31](AnimMgr31/README.md), [Stats32](Stats32/README.md), [Renderer33](Renderer33/README.md), [Sprite2d33](Sprite2d33/README.md), [Wanted33](Wanted33/README.md).

**Defectos de alcance general encontrados:**
- `CGameLogic::GameState` y `ActivePlayers` son bytes en el EXE (la DLL escribía 4 bytes y pisaba 0x96A8B1..B4).
- `CEntryExitManager::Init` reserva 400 entradas como el EXE (antes 512, NOTSA).
- `FxEmitterPrt_c` medía 0x44 por una base `ListItem_c` duplicada; el EXE usa 0x3C.
- `CDamageManager`: anchos de argumentos/retornos y dos convenciones de llamada incorrectas; `ProgressEngineDamage` corrompía la pila.
- `CShopping::gpCurrentProperty` era una variable propia de la DLL en vez de una referencia a 0xA97294.
- `CRenderer::ProcessLodRenderLists` nunca ejecutaba su segundo bucle y `CanLodChildrenRender` siempre era cierto.
- `CCarAI::TellOccupantsToLeaveCar` recorría el número de pasajeros en vez de todos los asientos.

**Lecciones:**
- Un `NOTSA_LOG_*` dentro de una función probada con `operator new` parcheado reserva en la arena del banco (spdlog guarda un historial); evitar logs en esos caminos.
- Las aserciones de depuración de la DLL que llaman a virtuales (p. ej. `GetRwModelType` dentro de `GetRwObject`) añaden llamadas que el EXE no hace: dar al vtable falso esa entrada sin traza.
- Funciones que devuelven un `enum : uint8` y el EXE usa EAX completo: devolver `int32`.

## Continuidad vigente: clases de 34 a 58 funciones y regresión completa (2026-10-05)

Bancos nuevos, todos con pares exactos dentro de GTA: Model34, Cutscene35, EnvMap37, RpAnim43, FileLoader47, Replay47, Surface47, Object48, PlayerPed51, PedIntel52, AudioHw54, Physical56, PedSpeech58 y CtrlConfig58. Regresión completa de 169 bancos terminada el 05/10 a las 13:36; **5322/8441 validadas, 221 casos**.

**Diferencias reales corregidas:**
- `RpAnimBlendClumpGetMainAssociation_N` y `GetMainPartialAssociation_N` tenían las direcciones del EXE intercambiadas (0x4D69F0 y 0x4D6A30).
- `CReplay::TriggerPlayback` usaba 1000000.0f; el EXE pasa 0x497423FE (999999,875).
- `CPlayerInfo::operator=` copia campo a campo como 0x45DEF0 (relleno sin tocar, mezcla de banderas incluida).
- Destructores de `CTaskComplexDrivePointRoute`, `FollowPointRoute`, `FollowPedFootsteps` y `WalkRoundObject`: `delete` con guarda de nulo como el EXE (la regresión de CTasks7a lo destapó).

**Lecciones:**
- Comparar punteros de pila entre EXE y DLL da falsos fallos: enmascarar con una función que normalice también las direcciones entre 0x400000 y 0x1000000.
- La comprobación de desbordamiento de una arena falsa con `size_t` de 32 bits da la vuelta: usar `uint64` y devolver nulo.
- Un crash que cambia de caso en cada corrida y cae en `ntdll` suele ser corrupción del heap por un fixture; escribir y vaciar un evento `case_start` antes de cada caso para localizarlo.
- Los bytes de pila no inicializados que lee el EXE hay que fijarlos igual en ambos lados (rutina `naked` que pone a cero bajo la dirección de retorno).
- Carrera del directorio de trabajo: el hilo de audio del EXE llama a `CFileMgr::SetDir("")` entre `SetDir` y `fopen` de `CTheScripts::Init`; un banco que muere antes de crear su registro puede ser eso, y repetirlo basta.
- Cuelgues intermitentes (IplStore, CTasks10b) pasan al repetir el banco aislado.

## Continuidad vigente: CCollision completa y CPhysical 56/56 (2026-10-05)

Bancos nuevos: ColGeom59a (25 funciones, 19.712 pares), ColProc59b (13 funciones, 8.192 pares) y ColWorld59c (20 funciones, 13.568 pares); Physical56 gana casos propios de `Add` (512) y `RemoveAndAdd` (1.024), con lo que CPhysical queda 56/56. **5382/8441 validadas, 224 casos.** Los 59 hooks de CCollision dejaron de estar bloqueados (`bEnableHooks` ya no gobierna esas líneas); `SphereVsEntity` queda con hook normal pero sin validar (el EXE tiene cuerpo, nadie la llama y lee una esfera local sin inicializar).

**Diferencias reales corregidas** (la DLL no coincidía con el EXE en casi todas): `TestLineTriangle`, `TestLineSphere`, `TestSphereTriangle`, `ClosestSquaredDistanceBetweenFiniteLines`, `ClosestPointOnLine` (firma con el punto primero), `ProcessLineBox`, `ProcessLineSphere`, `ProcessLineTriangle`, `ProcessVerticalLineTriangle`, `ProcessSphereTriangle`, `ProcessColModels`, `TestLineOfSight`, `ProcessLineOfSight`, `Shutdown`, `ResetMadeInvisibleObjects`, `SphereCastVsEntity` y varias de cámara. `ProcessDiscCollision` escribía con NaN donde el EXE no toca la salida.

**Lecciones:**
- Dos agentes que editan el mismo fichero definieron el mismo ayudante en espacios de nombres distintos: calificar las llamadas (`ColProc59b::...`), porque ADL anula un `using` local.
- Un `CColTrianglePlane` ocupa 10 bytes pero el EXE escribe 9: hashear 9, el último es basura de heap.
- El 05/10 los cuelgues "GTA no responde" de ColProc59b y PlayerPed51 desaparecieron al repetir el banco aislado.

**Sin comprobar:** el rendimiento en juego con los hooks de CCollision activos (antes estaban apagados por lentitud en depuración). La medición de 30 s de `Unlocks/run_current_build.py` dio 741 fotogramas con y sin los hooks, pero ese estado no ejercita colisiones; hay que probarlo jugando.

## Continuidad vigente: nivel 1 de la cola de validación, 34 clases (2026-10-05)

Bancos nuevos: CTasks61a (9 clases de tareas de coche), CTasks61b (9 de deambular, esquivar, pareja, señal, cruzar, destruir coche y ahogarse), CTasks61c (8 de ir a punto, puerta, apuntar, atractores, anim nombrada y rodear coche), Misc61a (CScriptsForBrains, CPedGroup, CWeather, CRopes) y Misc61b (CBmx, CQuadBike, CCarFXRenderer, CStencilShadows). Antes: BeCop60 y DragOut60. **5.662/8.441 validadas, 231 casos, 701 clases completas.** Quedan fuera del banco `RegisterStencilShadows` y los constructores de CBmx y CQuadBike.

**Diferencias reales corregidas** (la DLL no coincidía con el EXE): tiempo de `CTaskSimpleChoking` (1000..1999), `WanderProstitute::ScanForStuff` (campo +0xA8), rotación escrita en `m_fAimingRotation` (+0x55C), `GotoDoorAndOpen` (animación 0, no 3), `ComputeClosestFreeScriptedEffect` es `stdcall`, `GetDisplacement` devuelve puntero, `CPedGroup::Teleport` (3 errores), `RenderRainStreaks` reescrita, `CQuadBike::ProcessAI` llama a `DoHoverSuspensionRatios` y no a `Remove`, `CBmx::PreRender` (nodo trasero y signo del NaN), `CEventCarUpsideDown` sin `FIX_BUGS`, `ComputeTargetDoorToExit` sin `assert` y destructores de `CTaskComplexLeaveCar`/`EnterCar` con guarda de nulo.

**Lecciones:**
- Al reconstruir una virtual que un banco antiguo parchea solo en la dirección del EXE, la vtable de la DLL ya no pasa por ese parche: hay que parchear también el cuerpo de la DLL (`Body(OurVirtual(...))`). Cuatro bancos viejos caían por eso (CTasks10a, CarAI25, Data8b, STasks10).
- Un parche especulativo de un agente en un banco que ya pasaba lo rompió (STasks6a): revertir y repetir antes de aceptar.
- En ensamblador en línea MSVC, nombres como `fAdd` o `fScale` se confunden con instrucciones x87.
- Dos agentes que definen el mismo ayudante en espacios de nombres distintos obligan a calificar la llamada (ADL).
- Los diálogos "Device Selection" y los cuelgues de unos 50 s de CarAI25 y FixHooks eran intermitentes o de sondas congeladas: repetir el banco aislado antes de investigar.

## Continuidad vigente: cierre del nivel 1, funciones que se habían dejado fuera (2026-10-06)

Bancos nuevos: StencilReg62 (`RegisterStencilShadows`, con pool de vehículos y sectores del mundo falsos y dobles trazados de las llamadas de salida) y VehCtor62 (constructores de `CBmx` y `CQuadBike`: búfer completo del EXE frente a la DLL, con las bases `CBike`/`CAutomobile` simuladas). Las 34 clases del nivel 1 quedan al 100 %. **5.665/8.441 validadas, 233 casos, 702 clases completas.**

**Diferencias reales corregidas:** `StencilSignedDistSq` usaba otra transformación (0x59C890 con el orden up, forward, right, pos; sin matriz 0x54ECE0 con `fcos`/`fsin`); `CQuadBike::CQuadBike` leía el modelo de `m_nModelIndex` y el EXE usa el índice recibido; `cHandlingDataMgr::GetBikeHandlingPointer` restaba 162 solo si el id era >= 162, y el EXE indexa con `(byte id)<<6` desde la base (afecta también a `CBike::CBike`, aún sin validar).

**Verificado y descartado:** la sospecha de que el enlazador fusionara funciones idénticas (aliasado de hooks) no aplica: `source/CMakeLists.txt` usa `/INCREMENTAL`, que desactiva ICF. Los cuelgues esporádicos de CarAI25, FixHooks, STasks6a y CTasks6b no se reprodujeron en 12 repeticiones seguidas; su causa sigue sin conocerse. El parche especulativo de un agente a STasks6a (parchear el cuerpo de `SetPedInAsDriver::ProcessPed` si el hueco de la vtable es un salto) se revirtió porque colgaba el banco; no se sabe si el riesgo que temía existe.

TrigExtra3 aprobado: FollowPointRoute pasó 512 pares y Climb 4608, cero diferencias, sin resultados nulos y restauración verificada. Quince cuerpos COFF conservan instrucciones y relocations salvo los tres destinos autorizados. El primer banco Climb alcanzaba entidades externas: se aislaron todas las cabezas de sectores 120x120 y repeat 16x16, con restauración y guardia de ámbito; la campaña final no alcanzó entidades externas. Se acreditaron 51 casos de cosine en rutas y ocho casos de seno/coseno en escalada mediante inferencia de flujo y trazas. Evidencias en CppTrigFollowPointRoute/runs y CppTrigClimb/runs.

Estado de la migracion a C++ al 16/10/2026: 821 funciones fuente convertidas y revalidadas (2 más, convertidas, esperan la corrida aprobada de su banco en la regresión final: [estudio del lote](CppTwelveLevel6Study/README.md)). Quedan 0 apariciones de ensamblador y 0 declaraciones naked en 0 archivos de source/game_sa; son conteos lexicos, no funciones. Excepcion documentada: source/app/platform/win/WindowedMode.cpp conserva __asm (extension NOTSA, fuera de source/game_sa). Lote final IntegerMinimal2: 8704 pares compartidos, cero diferencias, cero resultados nulos y restauracion verificada. Trabajo detenido por orden del usuario tras este lote. Lista completa: [archivos pendientes](CppOnlyMigration/FILES.md); evidencia y alcance: [migracion C++](CppOnlyMigration/README.md).
