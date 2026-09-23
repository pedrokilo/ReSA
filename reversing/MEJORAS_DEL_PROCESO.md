# Reconstrucción y pruebas: proceso y mejoras

> **Estado vigente:** [continuidad de peatones](#continuidad-vigente) e [informe del desplazamiento de animación](FallenPeds/README.md). Las secciones de continuidad anteriores son históricas; sus cifras, pendientes y hashes describen cada entrega. Comprobar siempre Git antes de continuar.

## Qué se hizo realmente y qué faltaba mostrar

Las pruebas anteriores fueron comparaciones automatizadas dentro del proceso de GTA. Una sonda preparaba entradas, ejecutaba la función original y la reconstrucción, y comparaba resultados. CWeaponEffects también registró las llamadas de dibujo y estados gráficos, reenviándolos al motor. Esto aporta evidencia técnica, pero no equivale a observar una situación jugable ni a comparar capturas.

El arranque seguía mostrando el prólogo. La condición utilizada —jugador existente y CCutsceneMgr::IsRunning() falso— no asegura que haya terminado toda la introducción controlada por scripts. Además, un banco completo puede ejecutarse en una sola actualización o fotograma: los estados se restauran antes de que el usuario pueda distinguir los escenarios. Fue incorrecto presentar «probado dentro del juego» sin explicar esa diferencia. El porcentaje representa funciones reconstruidas, no una certificación visual o de estabilidad general.

## Cómo se reconstruye una función

1. Identificar la entrada pendiente, su dirección, firma, llamadas y estructuras. Comprobar la versión del ejecutable mediante hash.
2. Leer las instrucciones originales con dumpbin y contrastarlas con el C++ existente. Actualmente no se usa Ghidra; no es imprescindible para estas funciones.
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

- Repositorio: E:\Original\gta-reversed-master.
- Rama local comprobada: PGTASA. Destino indicado por el usuario: pedrokilo/RePorteoGTA-XBOXONESS. Verificar el remoto antes de cualquier publicación.
- Al comenzar esta actualización documental, git status --short estaba limpio. No suponer que siguen pendientes los cambios mencionados en el chat anterior; consultar Git. Esta actualización del Markdown es un cambio nuevo. No se realizó commit ni push en esta tarea de traspaso.
- Juego de pruebas: E:\JUEGOS\GTA San Andreas - CVehicle Test.
- Instalación personal: E:\JUEGOS\GTA San Andreas. Conservarla y conservar las partidas originales.
- Ejecutable original analizado: D:\Grand Theft Auto San Andreas\gta-sa.exe. SHA-256 esperado: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Verificar también el ejecutable de la copia de pruebas antes de usar direcciones fijas.
- Los cambios se cargan mediante scripts\gta_reversed.asi en la copia de pruebas. Los hooks modifican el proceso en memoria; no se necesita parchear el ejecutable en disco.
- ASI normal instalado al finalizar RpAnimBlend: SHA-256 48b5332d9d96f75a69aa20471ef801f38649c94f581567d18dd5eba20199ab15. Verificar si posteriormente cambió.

### Último trabajo completado: RpAnimBlend

RpAnimBlendClumpUpdateAnimations, dirección 0x004D34F0, quedó habilitada tras 652 comparaciones aprobadas dentro del motor. RpAnimBlend figura completa con 43 funciones. Error máximo 2.9802322387695312e-08 frente a tolerancia 1e-05. Pose del jugador y partidas sin cambios.

Leer RpAnimBlend/README.md, RpAnimBlend/validation-summary.json y RpAnimBlend/fixture-notes.md. El registro completo local está en E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-rpanimblend\20260922-191537-770481. Su hash se conserva en el resumen. No usar los ensayos descartados como evidencia de aprobación.

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
& 'E:\Original\gta-reversed-master\reversing\CVehicle\conan-cache\p\cmake20ac4e0137fee\p\bin\cmake.exe' --build build --config Debug
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

La ejecución conjunta documentada en ULTIMA_EJECUCION.md es anterior a CClouds: sus nueve bancos aprobaron 76.319 pares con sondas archivadas. No confundir ese informe histórico con una ejecución de los diez bancos actuales ni con una compilación integrada. La última ejecución de CClouds está en `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clouds/20260922-202449-837866`.

No hubo commit ni push. Conservar los cambios de las clases anteriores y de la infraestructura. No se ha elegido la siguiente clase después de CClouds.

## Actualización de continuidad: CMenuSystem (2026-09-22)

El usuario eligió CMenuSystem después de CClouds. Quedó en 21/21 al reconstruir FillGridWithCarColours (0x5820E0), antes delegada al original. El banco aprobó 784 pares dentro del motor, con 196 paletas, ambos IDs de menú y dos patrones de memoria inicial: 50.176 índices de color iguales. Verifica estructuras independientes, contadores de ejecución, escrituras limitadas a la cuadrícula, tabla de colores inmutable y restauración. Consultar reversing/CMenuSystem/README.md y validation-summary.json; no tratarla como pendiente.

La reconstrucción conserva los límites RGB estrictos de 15, la comparación adicional con una entrada negra y el uso del índice 127 al agotar alternativas. El banco cubre FillGridWithCarColours; no valida el dibujo, navegación ni todas las funciones de CMenuSystem. Continúan las exclusiones de Audio y Global/SurfaceInfos_c y la campaña integral sigue aplazada hasta completar todas las clases.

ASI normal actual, sin instrumentación, instalado solo en la copia de pruebas: SHA-256 `ce812b18c0f2154af48dede876b863e68040d4e0fa3d85f6c0aa68f303f1feb3`. PDB: `d51c5b1332d30eb38f523f987feeb1d161143db926fc3f2795e79a56a4d8fa06`. Respaldo en `_baseline/before-menusystem-20260922`. Partidas verificadas sin cambios. Sonda archivada: `31f0abcaad1fd61e590e4aa26b3da26c01ae224f1d4883a18d26349b129dee00`. Ejecución: `PROBAR.cmd --case menusystem` o `python reversing/run.py --case menusystem`, sin pausa. Caso añadido a --all, conservando un solo CMD.

Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-menusystem/20260922-203512-507016`. El informe ULTIMA_EJECUCION.md sigue siendo histórico y anterior a CClouds y CMenuSystem; no se volvió a ejecutar toda la serie en este trabajo. No hubo commit ni push. Conservar los cambios previos. No se ha elegido otra clase después de CMenuSystem.

## Actualización de continuidad: CTaskSimpleUseGun (2026-09-22)

El usuario eligió CTaskSimpleUseGun después de CMenuSystem. Quedó en 19/19 al reconstruir ProcessPed (0x62A380), antes delegada al original con un borrador incompleto. El banco final aprobó 3.232 comparaciones dentro del motor: 64 casos con dependencias reales alcanzadas y 3.168 con dependencias de animación/apuntado controladas. Diferencia numérica máxima 0 con tolerancia 1e-5. Incluye órdenes, cuentas atrás, disparo simple/continuo/doble, ráfagas, recarga, objetivos vivos/muertos, orientación y cambio de arma. Consultar reversing/CTaskSimpleUseGun/README.md y validation-summary.json; no tratarla como pendiente.

Límite esencial: los casos controlados registran y sustituyen efectos de AimGun, SetMoveAnim, RemoveStanceAnims, StartAnim y FightStrike. No equivalen a validar un combate completo con poses, impactos y cadenas IK activas. Se compara la función original real en ambos modos, con almacenamiento independiente, contadores y restauración; los callbacks se normalizan por identidad de función. Los detalles y las dependencias que sí continúan al motor están en su README. La campaña integral sigue aplazada hasta completar todas las clases.

ASI normal actual, sin instrumentación, instalado solo en la copia de pruebas: SHA-256 `05e7cbda3213066e8ae6452d8578a3e23b1c37a45631313303183fb3e2e571d5`. PDB: `1c20494da65b4b41a0cbd3957d0be7ada4ef4aee14148291dc307d0a80102c0c`. Respaldo en `_baseline/before-usegun-20260922`. Partidas y jugador real verificados sin cambios. Sonda archivada: `a10f00f4cb717fa3e556b9ea42f4f019036bbf997f6f4f6d0ab621029a8ba345`. Se repite mediante `PROBAR.cmd --case usegun` o `python reversing/run.py --case usegun` sin pausa. Integrada en --all, con un solo PROBAR.cmd.

Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-usegun/20260922-205940-939686`. Se corrigió durante las comparaciones la exclusión exacta del final del bucle de disparo continuo. No se ejecutó toda la serie de sondas; ULTIMA_EJECUCION.md sigue siendo un informe histórico anterior. Se mantienen excluidos Audio y Global/SurfaceInfos_c durante las comparaciones. No hubo commit ni push. Conservar los trabajos anteriores y los cambios de esta clase. No se ha elegido la siguiente clase después de CTaskSimpleUseGun.


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
| 7 | CEntryExit | 10/11 | TransitionFinished | En cola |
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

ASI normal sin instrumentacion instalado SOLO en la copia de pruebas: `971e1d165ec8392b9c2301c2225988d708c4313292b8d745f04d2385bc26b5b6`. Respaldo `_baseline/before-cped-completion-20260922`. Partidas verificadas sin cambios; instalacion personal intacta. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-cped-completion\20260922-221131-711733`. La unica diferencia del fuente normal frente al de la sonda final es la inclusion explicita de PedPlacement.h; la implementacion es identica. Compilacion Debug x86 aprobada. No se ejecuto de nuevo toda la serie de sondas y ULTIMA_EJECUCION.md sigue siendo historico.

Sin commit/push. Conservar todos los trabajos previos. CQuadBike sigue pendiente de validacion y con hook deshabilitado (11/12), aunque su candidato ahora compila como parte del proyecto. La cola autorizada de 21 clases sigue conservada; tras Wander y JetPack quedan CQuadBike y las 18 posteriores. CPed fue la prioridad de esta peticion, no una anulacion de esa cola.

## Continuidad: lote CConversations, CPlayerInfo y CPedList (2026-09-22)

Lote completado: CConversations 11/11 con 2.048 pares; CPedList 7/7 con 4.096 pares (1.024 por funcion); CPlayerInfo 28/28 registradas con 16.896 pares (4.096 objetos, 4.096 vehiculos y 8.704 Process). Comparaciones originales/candidatos dentro de GTA, con contadores de entrada, estados y trazas. CPlayerInfo final arranco con los tres hooks habilitados. Evidencia y hashes por clase en README.md y validation-summary.json. No tratar estas ocho funciones como pendientes.

Constructor de CPlayerInfo permanece deshabilitado y locked por el fallo conocido con armas; ya figuraba reversed y sigue delegando al original. El 28/28 no significa que este reparado. Se corrigio la firma thiscall de FindObjectToSteal. Process conserva el helper independiente de mirilla 0x56EC80 con receptor m_nCrosshairActivated (0x158), CalcRoadDensity 0x44EFC0 y constructor de recogida aun original. Dependencias controladas y cobertura finita; no cooperativo, ejecucion integral de tareas, recogida, efectos de camara RC ni estabilidad general. Campana integral aplazada hasta completar todas las clases.

hooks.csv sincronizado reversed/locked tras validar; ReversedClasses.md regenerado con contrib/hooks2md.py. Unico PROBAR.cmd: --case conversations, --case playerinfo y --case pedlist; integrados en --all. runtime_runner puede cerrar su propio proceso al evento final antes de auditar registros grandes, sin aceptar el evento por si solo. Los bancos archivados no son una prueba combinada de la compilacion actual; no se repitio toda la suite historica y ULTIMA_EJECUCION.md sigue historico.

ASI normal Debug x86 sin instrumentacion instalado SOLO en copia de pruebas: 9f6dc8c8db43ea47e9cd377c489a8fe5691f3dd0f24979426c8a5e3812221f80. Respaldo _baseline/before-playerinfo-conversations-pedlist-20260922. Partidas sin cambios; instalacion personal intacta. Sin commit/push. Rama PGTASA; conservar cambios previos. CQuadBike sigue 11/12, compilada pero NO validada y hook deshabilitado. La cola anterior conserva las demas clases pendientes; este lote no las completa.

## Continuidad: constructor de CPlayerInfo corregido (2026-09-22)

Sustituye la excepcion de constructor locked descrita en el cierre anterior. CPlayerInfo sigue 28/28, ahora todas sus entradas registradas estan habilitadas y desbloqueadas. Se retiro la llamada original 0x571920 y la asignacion redundante del temporal: m_PlayerData se construye una vez in situ, preservando los campos que el original no inicializa. El temporal anterior podia copiar campos/flags indeterminados.

4.224 pares originales/candidatos aprobados: 4.096 patrones completos de memoria con guardas/retorno/contador de una inicializacion y 128 regresiones de cuatro conjuntos de armas con entregas repetidas, inventario, municion, seleccion y referencias de modelos. Ultima prueba con hook activo y unlocked desde arranque; tres llamadas reales al candidato antes del banco. Streaming y representacion de armas controlados; SetInitialState y funciones de armas compartidas existentes. No prueba disparos ni estabilidad general, ni certifica todas las manifestaciones historicas del fallo.

Evidencia: reversing/CPlayerInfo/constructor-validation-summary.json y README.md; logs en E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playerinfo-constructor\20260922-233000-236046. PROBAR.cmd --case playerinfo-constructor, integrado en --all sin nuevos CMD. Constructor actualizado a reversed=1, locked=0 solo tras validar. Banco anterior de 16.896 pares conservado sin repetir: las tres funciones no cambiaron.

ASI normal sin instrumentacion instalado SOLO en copia de pruebas: dc6281acd032e459eb86eeab373be832c9d203a7d0c00cf38ab6ea4446195009. Respaldo _baseline/before-playerinfo-constructor-20260922. Fuentes restauradas; partidas sin cambios; instalacion personal intacta. Rama PGTASA; sin commit/push. Conservar los trabajos anteriores y cola pendiente. Campana integral sigue aplazada.

## Continuidad: FurnitureSubGroup_c (2026-09-22)

Completada 3/3: AddFurniture 0x5C00C0, GetFurniture 0x590EE0 y GetRandomId 0x590FD0. 20.480 pares exactos dentro de GTA (4.096 + 8.192 + 8.192), banco final con los tres hooks habilitados y sin locked desde arranque. IDs no negativos ignoran riqueza; negativos seleccionan por intervalo inclusivo. Riqueza 255 no es comodin (comparacion original contra -1 inalcanzable). Se consume RNG incluso sin coincidencias. AddFurniture conserva low-word del contador, almacen 256, insercion al principio y umbral dimensional >=0.02.

Fixtures de listas/cajas aisladas y RNG controlado; AddItem compartido existente. No colocacion/robo completos, representacion ni estabilidad general. README y validation-summary.json detallan evidencia: E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-furniture\20260922-234110-237502. PROBAR.cmd --case furniture, incluido en --all sin nuevos CMD. hooks.csv actualizado reversed=1/locked=0 tras validar; ReversedClasses.md regenerado.

ASI normal sin instrumentacion instalado solo en copia de pruebas: 36910037caae88d3f125d9492f1987e4d025b78677133b37b25efd622ac24ca1. Respaldo _baseline/before-furniture-20260922. Fuentes restauradas, partidas sin cambios e instalacion personal intacta. Rama PGTASA, sin commit/push. Conservar trabajos anteriores y cola pendiente; CQuadBike sigue sin validar/deshabilitada. CPlayerInfo constructor sigue corregido y habilitado. No se repitio toda la suite ni se inicio la campana integral.

## Continuidad documental: progreso y prioridades (2026-09-23)

Se actualizó README.md para describir PGTASA, los bancos aprobados, copia de pruebas, partidas, hooks y límites reales. Se retiró la estimación genérica heredada de 50-60%; los números proceden de hooks.csv. ReversedClasses.md ahora es un panel con resumen, navegación, tablas y columna locked; su formato vive en contrib/hooks2md.py y se conserva al regenerar, incluida la ejecución desde CI.

Nueva guía manual docs/ReversingPriorities.md: las 100 clases incompletas (98 parciales, 2 sin revertir), 555 funciones pendientes, cuatro grupos de dificultad y Top 10 con 23 entradas pendientes. Inspección de fuentes/headers, registros, referencias y algunos originales para candidatos inmediatos; no se presenta como desensamblado exhaustivo de 555 funciones. Incluye código candidato frente a wrappers/borradores, límites y enlaces al pendiente concreto.

La propuesta de prioridad no reemplaza automáticamente la cola autorizada ni habilita funciones. No se cambiaron fuentes del juego, flags de hooks.csv, binarios ni partidas en esta tarea documental. Se comprobaron enlaces locales, integridad de los totales, presencia única de las 701 clases y generación con casos completos/parciales/cero y locked. Sin nuevas pruebas GTA, commit ni push. El ASI normal instalado continúa siendo el de FurnitureSubGroup_c del cierre anterior.


## Continuidad: CClothes, CScripted2dEffects y Sprint (2026-09-23)

Completadas por petición expresa: CClothes 11/11, CScripted2dEffects 5/5 y CTaskComplexUseClosestFreeScriptedAttractorSprint 4/4. No tratar las cuatro funciones como pendientes. 8.448 pares exactos finales con hooks habilitados y unlocked desde arranque: 4.096 GetTextureDependency, 256 Init, 2.048 Clone y 2.048 GetTaskType. Primera ejecución del mismo banco también aprobada antes de habilitar; no suma cobertura distinta.

GetTextureDependency existente coincide. Init ahora conserva segundo PartnerTask y relleno como el helper original 0x6F9EB0. Clone construye una tarea nueva con sprint, no copia a través del constructor de copia de la base que reiniciaba a caminar. GetTaskType=262. Detalles, límites y hashes en reversing/ClothesScriptedSprint/README.md y validation-summary.json; cada clase dispone además de su README y resumen.

Banco automático integrado en el único PROBAR.cmd: --case clothes-scripted-sprint y --all. Compara tablas completas con guardas, pool real y objetos; restaura globals/hooks/jugador y comprueba partidas. No se probaron interacción prolongada, ropa completa, agotamiento del pool, tareas reales de atractores ni estabilidad general. No se repitió toda la suite. Campaña integral sigue aplazada; ULTIMA_EJECUCION.md es histórico.

Evidencia final: E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-clothes-scripted-sprint\20260923-001346-817023. ASI normal instalado solo en copia de pruebas: d62c1071aadb1b459964a46efc4bfc3b9c6b3ea094521374408c14de7e9da034. Respaldo _baseline/before-clothes-scripted-sprint-20260923. Fuentes normales coinciden con el manifiesto; partidas sin cambios e instalación personal intacta.

hooks.csv actualizado solo para cuatro filas tras validar, ReversedClasses.md regenerado y README actualizado. Guía de prioridades conserva su corte inicial y ahora señala estos tres cierres; quedan 97 clases incompletas y 551 entradas pendientes. Rama PGTASA, sin commit/push. Conservar cambios anteriores, incluido CQuadBike sin validar/deshabilitado y el constructor CPlayerInfo ya corregido.


## Continuidad: CGeneral desbloqueado (2026-09-23)

CGeneral sigue 9/9 y ahora tiene cero locked. GetRandomNumberInRange<float> 0x41BD90 estaba desactivada/bloqueada por una aserción con extremos invertidos (caller 0x6DF26D). Especialización float local x87 reproduce orden, control de precisión y retorno ST(0) del original sin esa aserción. El primer candidato double falló y se descartó. No eliminar la aritmética x87 sin volver a comparar: double/SSE no son equivalentes en este contexto.

65.536 pares exactos finales, cero diferencias float y retorno de 80 bits, una llamada RNG por lado. Banco con 16 intervalos y entradas RNG controladas; ver README/validation-summary.json en reversing/CGeneral. Hook habilitado y desbloqueado desde arranque, luego compilación normal sin instrumentación. No se revalidaron las otras ocho entradas ni todos los callers, incluido el template entero que usa el helper float. No prueba todos los modos FP, NaN/infinito, conducción prolongada ni estabilidad general.

Evidencia: E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-general\20260923-003344-869382. Caso --case general en el único PROBAR.cmd y en --all. Fuentes restauradas y comparadas con el manifiesto. ASI normal instalado solo en copia de pruebas: e5b8d16815fb880a46fe0a23e012d825e3607304ef3429f7a248a3ad03192a67; respaldo _baseline/before-general-20260923. Partidas sin cambios, instalación personal intacta.

hooks.csv actualizado solo en locked de la fila float tras validar; ReversedClasses.md regenerado y eliminado el candado de CGeneral. README y hash de trazabilidad de ReversingPriorities actualizados. La guía conserva la actualización anterior completa: 97 clases pendientes, 551 funciones y Top 10 renovado, no volver a introducir clases ya cerradas. Sin commit/push. Campaña integral aplazada y ULTIMA_EJECUCION.md histórico.


## Continuidad: inicio normal, cierre de audio y documentación en español (2026-09-23)

El usuario pidió traducir todos los MD y que GTA arranque desde el principio, sin carga automática. Se revisaron los 38 Markdown propios existentes, incluidos los ocultos en .github; seis documentos en inglés fueron traducidos. Se corrigieron acentos dañados, manteniendo símbolos, comandos y documentación histórica identificada. Se añadieron README en español para CAESound e InicioNormal. No se tradujeron cachés o dependencias descargadas.

Ambos gta-reversed.ini de la copia de pruebas tenían FastLoader activo y SaveGameToLoad=2. Ahora Enable=false y SaveGameToLoad=-2; opciones de omitir pantallas en false. Se conservó Misc.SaveDirectoryPath. Respaldos de INI y del log del cierre en reversing/InicioNormal/artifacts. El comando normal es Set-Location a la copia de pruebas y & '.\gta_sa.exe', sin argumentos ni exclusiones.

runtime_runner.py ahora respalda y restaura ambos INI byte por byte: FastLoader y ranura 2 solo se activan temporalmente durante bancos. La restauración se verificó en una reproducción fallida y en el banco aprobado (configs_restored=true). No dejar FastLoader activado permanentemente otra vez. No añadir nuevos CMD.

Cierre registrado a las 00:36:43: acceso inválido 0xC0000005 en CAESound::UpdateParameters:252, lectura 0xDDDDDE0D. El usuario indicó un choque previo. Reproducción dentro del motor confirmó que la asignación implícita de CAESound copia EntityRef sin registrar el destino ni retirar la referencia anterior; ResolveReferences deja el destino colgando. Se implementó operator= explícito, con referencia registrada, autoasignación y preservación de m_IsAudioHardwareAware/relleno como el original 0x4EF680. La reparación no modifica globalmente EntityRef.

1.024 pares exactos aprobados con Audio activo desde arranque; solo SurfaceInfos excluido en este banco. Caso sound-copy integrado en el único PROBAR.cmd y --all. README y validation-summary.json en reversing/CAESound. Evidencia: E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-sound-copy\20260923-004932-410885. La reproducción fallida quedó en 20260923-004623-831808. No se revalidó toda la clase ni se reprodujo exactamente el mismo choque. Los errores de asignación CEntryExit del log siguen como incidencia separada.

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

Rama PGTASA. Cambios previos conservados. Sin commit ni push.




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

Rama PGTASA, cambios previos conservados, sin commit ni push. La campaña integral sigue aplazada hasta completar las clases; las sondas históricas no prueban conjuntamente todo el ASI actual. No se garantiza estabilidad general. No hay orden vigente de apagar el equipo: el apagado pedido para la tarea anterior no es permanente.

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

Limitaciones: el log conjunto registró 13 fallos de asignación de CEntryExit; el motor siguió funcionando. No se investigó ni atribuyó su causa. Persisten aviso timecyc y rechazo de la ranura 2 incompatible; no forzar su carga. El arranque mostró un aviso de adquisición del ratón. No hubo cierre inesperado, aserción ni RTC observado en las comprobaciones finales. No se probó conducción, combate, carga/guardado completo ni sesión prolongada; no garantiza estabilidad general. Campaña integral sigue aplazada. Rama PGTASA, sin commit/push del agente, sin apagar. Esperar otra petición antes de continuar los 86 pendientes.


<a id="continuidad-entryexit"></a>

## Continuidad histórica: reserva de CEntryExit reparada (2026-09-23)

Petición completada: investigar los fallos de asignación de CEntryExit. Git comenzó limpio en PGTASA, HEAD cba65c3, posterior a la entrega parcial. No recuperar los borradores de CFileMgr/Game: siguen aplazados los mismos 86 candados.

Traza: 444 altas, 25 bajas y 19 fallos con 400 plazas; ninguna búsqueda de una baja falló. Banco con pools privados: el original y el candidato fallan en las mismas 19 altas con 400, y aceptan todas con 512 (419 entradas activas). Se cambió únicamente la reserva inicial de CEntryExitManager::Init a 512; 6.832 bytes adicionales. Ampliación deliberada del límite original, no equivalencia de Init ni una corrección general del pool. Los objetos no se mueven después de asignarse.

[Informe](CEntryExitPool/README.md) y [resumen verificable](CEntryExitPool/validation-summary.json). Banco final resultados-entryexit-pool/20260923-141006-579167: 1.854 comprobaciones, 1.833 pares normalizados iguales y 21 retornos distintos esperados de FIX_BUGS (-1 frente al 0 original). También vacía y reutiliza las 512 plazas. Índices/generaciones y padding normalizados; no prueba puertas rotadas, enlaces ni todas las funciones de CEntryExit. Sonda 12a1673274333d01b296e4ec83e1d567180e494222d8193a0dd87acc696dba11; caso entryexit-pool añadido al único PROBAR.cmd y --all. Total 39 casos, 38 de motor y uno aislado. CSV y ReversedClasses sin cambios: ningún estado de hook varió, continúan 614/85/2 clases y 620 candados.

ASI normal Debug x86 instalado solo en la copia de pruebas y presente en bin/Debug: **f01faea570319f6f3f7b5a7278ae7b80e3fc27507e478fa367a811b677fdaa93**; PDB 0e77e137a91dc56f9390013ed991cdd7a3a3709f075b80ba0f8a49b6105587f6. Respaldo `_baseline/before-entryexit-pool-20260923-141213`. Fuentes sin instrumentación y hashes verificados contra la sonda. Integración sin exclusiones: **120,24 segundos, 3.404 fotogramas, cero fallos de CEntryExit**, máximo 394/512. Evidencia Unlocks/artifacts/current-20260923-141219. La pasada natural no alcanzó 400; el desbordamiento y las plazas extra se prueban en el banco determinista.

INI restaurados (FastLoader=false, SaveGameToLoad=-2), partidas sin cambios por hash, instalación personal intacta. No se repitió el banco de llegada al menú, ni conducción o guardar/cargar completo. La capacidad sigue siendo finita; no se garantiza compatibilidad de guardados nuevos con índices superiores a 399 en un motor limitado a 400. El log conserva avisos de datos, ranura incompatible, cinco de IK y 18 de peatones fuera del mapa; no se investigaron y no se afirma estabilidad general. Ya había un aviso de peatón en una observación del ASI anterior. No hubo cierre inesperado, aserción o RTC en la prueba final. Campaña integral y 86 desbloqueos aplazados. Sin commit/push, sin apagado. Esperar otra petición.


Revisión documental posterior: se inspeccionaron los 59 Markdown propios del repositorio y se corrigieron ocho documentos con referencias vigentes desactualizadas o ambiguas. CAESound ya enlaza el diagnóstico de CEntryExit; ULTIMA_EJECUCION apunta a la reparación reciente; los informes históricos remiten al ancla estable de continuidad. Se comprobaron 855 enlaces locales, sin rutas rotas. Inventario y resultado en CEntryExitPool/artifacts/docs-inventory.json. Esta revisión solo modificó documentación; no recompiló, no repitió GTA ni cambió CSV, binarios o partidas. Sin commit/push.

<a id="continuidad-peatones"></a>

## Continuidad histórica: desplazamiento de peatones por animación reparado (2026-09-23)

Petición: investigar y reparar los avisos de peatones fuera del mapa. Git comenzó limpio en **PGTASA, HEAD 4516c03**, que ya contenía CEntryExit y su documentación. Esta tarea no hizo commit ni push; comprobar el estado real antes de continuar y conservar los cambios nuevos.

Los avisos anteriores tenían coordenadas compatibles con `0xCDCDCDCD`. Una traza registró 74 saltos de posición y 14 avisos; otra acotó 39 saltos a `CPedIntelligence::Process` → `CTaskManager::ManageTasks` (117 registros al observar los mismos saltos en tres niveles, no 117 sucesos independientes). Las pasadas naturales son variables y dos no reprodujeron. No se identificó el nombre concreto de la animación de los 18 avisos históricos.

El desensamblado y el banco determinista identificaron tres inicializaciones omitidas en el constructor con parámetros de **CTaskSimpleRunNamedAnim, 0x61A990**: vector `m_vecOffsetAtEnd` a cero, bandera `m_bOffsetAvailable` desactivada y `m_pAnimHierarchy` nulo si falta el bloque. `ProcessPed` original consulta primero esa bandera y aplica el desplazamiento; con memoria 0xCD, el constructor anterior provocó exactamente el salto de coordenadas en el banco. Solo se repararon esas inicializaciones; no se alteraron el constructor predeterminado, la función de desplazamiento ni los avisos de CWorld.

[Informe](FallenPeds/README.md) y [resumen verificable](FallenPeds/validation-summary.json). **8.832 pares aprobados:** 8.192 constructores, 128 ejecuciones controladas de ProcessPed original y 512 aplicaciones de OffsetPedPosition. Sin diferencias en los resultados comparados, error máximo de posición 0. El control anterior registró 8.160 diferencias de inicialización y 64 de posición; las 512 aplicaciones de desplazamiento ya coincidían. Campos semánticos, objetos independientes y clump privado vacío; se excluyen vtable, padding, colas de cadenas y el identificador no inicializado por el original. No prueba todas las animaciones ni cierra la clase.

Banco corregido: `resultados-namedanim-init/20260923-150054-230081`. Sonda SHA-256 `d2096562927cb082cfce314b1419ed81343f15cbc9b4178cd1a73636449bcdfc`. Repetición: `python reversing/run.py --case namedanim-init`. Integrado en el único PROBAR.cmd y `--all`: **40 casos, 39 dentro del motor y uno aislado**. No se repitió toda la suite histórica. CSV y ReversedClasses intactos: no cambiaron estados de hooks ni porcentajes.

**ASI normal instalado solo en la copia de pruebas y presente en bin/Debug:** `6835a6eebc877f597a0d1cffe3992b7e8889cc0ddbff80446b4928d7978cd311`; PDB `9276a2b633756577980ed15fe390220a80ce7bd03be8886cd15029d95d8bdd3f`. Sin instrumentación, fuentes verificadas contra el manifiesto del banco. Respaldo `_baseline/before-fallen-peds-20260923-150204`.

Integración final sin exclusiones: **120,64 segundos, 3.609 fotogramas, cero avisos de peatones y cero fallos de CEntryExit**, máximo 402/512 entradas. Evidencia `Unlocks/artifacts/current-20260923-150550`. La primera pasada solo avanzó 815 fotogramas; se repitió exigiendo 3.600. Comando: `python reversing/Unlocks/run_current_build.py --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600`. No hubo cierre inesperado, aserción ni RTC. INI restaurados (`FastLoader=false`, `SaveGameToLoad=-2`), partidas conservadas por hash, instalación personal intacta y GTA cerrado al terminar.

El log final mantiene avisos de timecyc/skimmer; no apareció IK en esta muestra, pero no se investigó ni reparó. No se certifican otras causas posibles de peatones fuera del mapa, conducción, combate, guardar/cargar completo ni estabilidad general. CEntryExit conserva sus 512 plazas y su banco de 1.854 comprobaciones (1.833 resultados normalizados iguales y 21 diferencias esperadas de FIX_BUGS); RpAnimBlend sigue 43/43. **Siguen aplazados los mismos 86 bloqueos: CCollision 59, CFileMgr 13, GangLeader 11 y Game 3.** No aplicar el parche diferido sin petición. Campaña integral aplazada; sin apagado.

<a id="continuidad-ik-datos"></a>

## Continuidad histórica: aviso del pivote IK y datos reparados (2026-09-23)

El usuario pidió reparar IK/datos y revisar el debug adjunto de las 15:02. Rama **PGTASA**, HEAD **4516c033c3e8e205af47aac18c9ca4f58f323b68**. Se conservaron todos los cambios existentes de peatones y su documentación; no hubo commit ni push. Comprobar Git real antes de continuar.

**IKChain_c::SetupBones:** evita buscar como padre interno el pivote excluido deliberadamente de la cadena. El original también omite ese enlace; la reconstrucción emitía un aviso falso, incluido el caso cabeza/cuello (hueso 4). Se mantiene el diagnóstico para otras búsquedas fallidas. No se modifica el solver ni se completa la clase.

**Datos, solo copia de pruebas:** `timecyc.dat` tenía 183 filas de 51 campos y una de 49; ambos lectores solicitan 52. Se completa la fila defectuosa con ambiente `255 255 255` y se añade multiplicador neutro `1.0` a las 184 filas. Es una reparación explícita del dato indeterminado; no se conocen con certeza los valores ambiente perdidos. `vehicles.ide` explicita los valores predeterminados actuales de skimmer `-1, 0.7, 0.7`. `cargrp.dat` conserva los primeros 23 modelos de los grupos 6/7 y elimina únicamente los 3/6 sobrantes ignorados por el motor. Su aviso de «no memory» no indicaba falta de RAM. Lectores CTimeCycle/CFileLoader/Population intactos. Respaldo exacto `_baseline/before-ik-data-20260923-152119`; [manifiesto de datos](IKDataWarnings/data-repair.json). El reparador es idempotente, comprueba hashes y exige GTA cerrado para escribir.

[Informe](IKDataWarnings/README.md) y [resumen verificable](IKDataWarnings/validation-summary.json). **131 pares únicos de SetupBones aprobados**, comparando orden, etiquetas, padres, orientación, límites y velocidad; contadores original 0/candidato 1. Jugador, gestor de nodos, frames y keyframes intactos; devolución de nodos comprobada. No se comparan padding, direcciones ni matrices aún sin calcular. **9.384 bytes de tablas de clima coincidentes** usando los datos corregidos; no son 9.384 casos independientes. Cantidades y entradas activas de grupos de coches iguales al original; skimmer carga los valores explícitos esperados. No se valida toda la clase IK ni todo LoadVehicleObject.

Banco final `resultados-ik-data/20260923-152556-990651`, sonda ASI `218564db47286769690da2734e5b34e9f2b6c0fecb61e4a5007fec4b9edf91f2`. La primera pasada también aprobó 131 pares; no se suma como cobertura nueva. Repetición: `python reversing/run.py --case ik-data`. Único PROBAR.cmd reutilizado; **41 casos, 40 de motor y uno aislado**. CSV y ReversedClasses intactos, sin cambios en indicadores ni porcentajes. No se ejecutó la suite completa.

**ASI normal instalado y presente en bin/Debug:** `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`. Sin sonda; fuentes normales verificadas contra el manifiesto. Respaldo anterior `_baseline/before-ik-normal-20260923-152717`.

Integración sin exclusiones: **121,16 segundos, 3.605 fotogramas**, cero avisos de IK/timecyc/argumentos de vehículos/capacidad de grupos/peatones y cero fallos de CEntryExit; máximo 403/512. Evidencia `Unlocks/artifacts/current-20260923-152730`. Comando: `python reversing/Unlocks/run_current_build.py --ik-data --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600`. INI restaurados (`FastLoader=false`, `SaveGameToLoad=-2`), partidas sin cambios por hash, instalación personal intacta y proceso propio cerrado.

**Otros mensajes:** persiste `Acquire` del ratón con HRESULT 0x80070005 al arrancar; existe reintento en CPad, pero esta prueba no verificó su recuperación. La carga automática temporal intenta la ranura 2 `v1 Modified`; el motor espera `v1 Unmodified`, rechaza la carga y comienza partida nueva. No forzar compatibilidad ni modificar partidas. El adjunto no contenía avisos IK/peatones y pertenecía a la primera integración de 815 fotogramas, no a la posterior de 3.609.

No se observó cierre inesperado, aserción ni RTC en la integración final. No se probaron conducción, combate, guardar/cargar completo, todos los esqueletos ni sesiones prolongadas; no se garantiza estabilidad general. CEntryExit mantiene 512 plazas y su evidencia de 1.854 comprobaciones: 1.833 resultados normalizados iguales y 21 diferencias esperadas FIX_BUGS. RpAnimBlend sigue 43/43. Permanecen aplazados **CCollision 59, CFileMgr 13, GangLeader 11 y Game 3: 86 bloqueos**; no aplicar el parche diferido sin petición. Sin apagado.

<a id="continuidad-vigente"></a>

## Continuidad vigente: recuperación del ratón comprobada y revisión de Markdown (2026-09-23)

Petición: comprobar el fallo inicial de adquisición del ratón y actualizar todos los Markdown. Git revisado en **PGTASA**, HEAD **4516c033c3e8e205af47aac18c9ca4f58f323b68**, conservando los cambios existentes de peatones e IK/datos. Sin commit ni push.

**Diagnóstico confirmado:** una sonda temporal observa las llamadas reales, sin añadir Acquire/GetDeviceState ni inyectar movimiento/clics. El primer Acquire devuelve `0x80070005` con la ventana oculta y otra ventana en primer plano. A los **1,485 segundos**, con la ventana visible y con foco, la primera lectura falla (`0x8007000C`), el reintento de adquisición da `S_OK` y la segunda lectura también, dentro de la misma llamada. Se registran **120 lecturas correctas y 120 actualizaciones de estado**; termina a los 15,110 s desde el primer intento. Las inicializaciones posteriores también adquieren el dispositivo. No hizo falta cambiar el código normal ni ocultar el aviso. Esto demuestra recuperación en esta ejecución, no retrospectivamente en todas las sesiones anteriores.

[Informe](MouseRecovery/README.md) y [resumen verificable](MouseRecovery/validation-summary.json). Evidencia `resultados-mouse-recovery/20260923-154347-737501`. Sonda ASI `c09c409dc44c30ade89ed3237dccbf2a67ea9210c436fa89e9616439901adccc`; registro SHA-256 `81c7768545e71b7bfb2038f8d93772763076105720eeb1b51295857bc9ef0e2f`. Repetición: `python reversing/MouseRecovery/build_probe.py`, después `python reversing/MouseRecovery/run_local.py`. Es observación del dispositivo real, no pares de equivalencia. El catálogo conserva **41 casos, 40 de motor y uno aislado**; único PROBAR.cmd intacto.

**ASI normal sin cambios:** `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`, tanto en bin/Debug como en la copia de pruebas. Fuentes, binarios normales e INI restaurados por hash. Partidas conservadas y proceso propio cerrado. FastLoader=false y SaveGameToLoad=-2. Se conservan los tres archivos de datos reparados y sus respaldos; instalación personal intacta.

**Documentación:** revisados **62 Markdown propios**, incluidos `.github` y archivos no rastreados, con cachés/dependencias/artefactos ignorados excluidos. Se actualizan diagnóstico del ratón, estado instalado de InicioNormal, referencias históricas a avisos ya investigados, índices y navegación a continuidad de 40 informes que no la tenían. Las guías técnicas y el catálogo generado conservan sus contenidos válidos. **944 enlaces a archivos locales comprobados, sin destinos inexistentes**; no se verifican URLs remotas ni fragmentos de encabezados. Inventario final en `MouseRecovery/artifacts/docs-audit.json`; no se alteran resultados históricos ni indicadores CSV.

Última integración normal sigue siendo la de IK/datos: **121,16 s y 3.605 fotogramas**, sin avisos comprobados de IK/datos/peatones y sin fallos de CEntryExit. Sus 131 pares de IK y tablas de clima/grupos coincidentes conservan su alcance. El nuevo diagnóstico no vuelve a ejecutar esa integración ni toda la suite. No prueba clics/movimiento físico, hotplug, todos los controladores, conducción, combate, guardar/cargar completo ni estabilidad general. La partida incompatible sigue rechazándose de forma prevista; no forzar carga ni modificar partidas.

**Aplazados sin cambios:** CCollision 59, CFileMgr 13, GangLeader 11, Game 3: 86 bloqueos. No aplicar `UnlockBatch2/deferred-filemgr-game.patch` sin petición. RpAnimBlend 43/43; CEntryExit 512 plazas, 1.854 comprobaciones (1.833 resultados normalizados iguales y 21 diferencias esperadas FIX_BUGS). Sin apagado.
