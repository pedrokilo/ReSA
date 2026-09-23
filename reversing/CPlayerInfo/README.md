# CPlayerInfo - 28/28

Funciones reconstruidas: FindObjectToSteal (0x56DBD0), FindClosestCarSectorList (0x56F4E0) y Process (0x56F8D0). La firma de FindObjectToSteal se corrigio a miembro thiscall. Se conservan el orden de busqueda, codigos de escaneo, criterios de seleccion, temporizadores, acrobacias, dinero, salud, persecuciones e interaccion con vehiculos del original.

El banco compara 16.896 pares dentro de GTA: 4.096 de objetos, 4.096 de vehiculos y 8.704 de Process. Comprueba estados, argumentos y orden de llamadas; exige cero entradas al candidato al ejecutar el original y una al ejecutar el candidato. Tolerancia absoluta de flotantes: 0.00001; bytes y enteros exactos. Incluye limites del mapa, listas vacias/llenas, escaneo, taxi, suspension, umbrales temporales, estadisticas, persecucion, replay y entrada/salida de vehiculos. Constructores de tareas reales, asignacion y varias dependencias controladas. Restaura globals, hooks, jugador y partidas.

Usar el unico PROBAR.cmd --case playerinfo, o --all. La sonda archivada termina automaticamente; el ejecutor cierra su propio proceso antes del analisis completo del registro grande y restaura el ASI/PDB anterior. Un evento de fin por si solo no aprueba: se auditan todos los pares y su cobertura.

Limites: no cobertura exhaustiva, ejecucion completa de tareas, cooperativo, recogida de objetos, audio, streaming ni efectos de camara de vehiculos RC. Las matrices de las busquedas ya existen en los fixtures. Se conserva el controlador original independiente de mirilla 0x56EC80 (receptor m_nCrosshairActivated, offset 0x158), CalcRoadDensity 0x44EFC0 y el constructor de la tarea de recogida, aun no reconstruidos en sus clases. No es una prueba de estabilidad general ni de todas las clases combinadas.

El constructor fue corregido y desbloqueado posteriormente; las 28 entradas registradas ahora estan habilitadas y ninguna esta locked. Evidencia inicial de las tres funciones en validation-summary.json y evidencia adicional del constructor en constructor-validation-summary.json.

Banco final aprobado con los tres hooks habilitados desde el arranque. Compilacion normal Debug x86 instalada solo en la copia de pruebas, con respaldo y partidas sin cambios.

## Constructor corregido (0x571920)

El cuerpo antiguo delegaba al ejecutable y dejaba inalcanzable una asignacion m_PlayerData = CPlayerPedData(). Al habilitar esa alternativa, el miembro ya se habia construido automaticamente; el temporal volvia a inicializarlo y copiaba campos/flags no inicializados. La correccion construye el miembro una sola vez, in situ, y conserva los campos que el original no escribe. No se llama al constructor original desde el candidato.

4.224 pares aprobados con el hook habilitado y sin bloqueo desde el arranque: 4.096 patrones de memoria (cero, FF, CC y datos deterministas), igualdad exacta de los 400 bytes mas guardas, retorno this y una sola llamada a SetInitialState por lado; 128 casos de los cuatro conjuntos de armas, repetidos de una a cuatro veces, con inventario, municion, seleccion y referencias de modelos verificados. Hubo ademas tres llamadas reales al candidato durante el arranque, antes del banco.

La regresion usa las funciones reales de cheats, GiveWeapon, recarga y seleccion sobre un ped aislado; intercepta streaming y adjuncion visual de modelos. SetInitialState y las operaciones de armas conservan sus hooks compartidos existentes. No prueba disparos, representacion visual, cooperativo ni estabilidad general. El defecto de copia esta eliminado; no se afirma haber reproducido cada manifestacion historica del fallo de armas.

PROBAR.cmd --case playerinfo-constructor, tambien en --all. Se conservan el banco anterior --case playerinfo y su sonda archivada. Nueva compilacion normal instalada solo en copia de pruebas con respaldo; partidas sin cambios. Sin commit/push.
