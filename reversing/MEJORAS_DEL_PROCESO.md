# Reconstrucción y pruebas: proceso y mejoras

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

- Formato del listado: clases completas como «CVehicle (142)<br />», sin repetir fracción ni porcentaje.
- Acceso común: PROBAR.cmd, reversing/run.py y reversing/cases.json.
- Ejecutores históricos: conservados, pendientes de extraer su infraestructura repetida.
- Demostración visible después del prólogo: opcional y fuera del flujo obligatorio, según la preferencia del usuario. No confundirla con las comparaciones automáticas realizadas.
- RpAnimBlend: 43/43; 652 comparaciones aprobadas dentro del motor con datos independientes. Infraestructura compartida en runtime_runner.py; fuentes e informe en RpAnimBlend/.

## Porcentajes de avance

Se calculan a partir del indicador reversed en docs/hooks.csv y se muestran con dos decimales mediante contrib/hooks2md.py. El encabezado mide clases del grupo respecto a clases totales; cada fila mide funciones reconstruidas respecto a funciones registradas de esa clase. No representan cobertura de pruebas. Un recálculo del informe no cambia el estado de ninguna función.
