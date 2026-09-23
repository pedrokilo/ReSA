# CConversations - 11/11

DoneSettingUpConversation (0x43ADB0) reconstruida y validada con 2.048 pares original/candidato dentro de GTA. Coincidencia exacta de tablas, nodos temporales, referencias, estados y contadores. Partidas y estado del jugador restaurados. Hook habilitado despues de validar.

Ejecutar desde el unico PROBAR.cmd: PROBAR.cmd --case conversations; tambien incluido en --all. Sonda archivada, no una prueba conjunta de estabilidad del ASI actual.

Casos: 0-12 nodos, nombres duplicados, enlaces ausentes y diferencia de mayusculas, falta de nodos libres, distintos huecos de conversaciones y subtitulos. Se conserva la seleccion original del ultimo nombre coincidente y el fallback al nodo 0 cuando la tabla esta llena. Se requiere un hueco de conversacion, como en el original.

Limites: comprueba configuracion y referencias; no reproduccion de dialogos, audio, subtitulos ni estabilidad general. Evidencia, hashes y restauracion en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.
