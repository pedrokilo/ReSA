<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWeaponEffects::Render (0x742CF0)

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruida en C++ y activada después de 301 comparaciones automatizadas dentro de GTA. El progreso pasó de 8/9 a 9/9 solamente después de verificar el informe aprobado. Los campos field_1C y field_20 son flotantes; se corrigieron sus tipos conservando el tamaño y la distribución de memoria de CWeaponEffects.

## Evidencia dentro del juego

La sonda se ejecuta en RenderEffects, durante el renderizado real. Alterna el hook, llama a la dirección original, restaura las entradas y llama al candidato. Un contador exige cero entradas al candidato al ejecutar el original y exactamente una al ejecutar la reconstrucción. Registra argumentos de triángulos y sprites y llamadas de estados RenderWare, reenviándolos al motor gráfico real; compara orden, texturas, colores, intensidad, alfa, coordenadas, tamaños, ángulos y estado mutable de las miras.

- 5 casos de salida temprana, jugador ausente, mira inactiva y vencimiento.
- 96 casos de triángulos: ambos índices de jugador, tamaños, tiempos de desactivación y rotación.
- 90 casos de mira de vuelo: ambos índices, cinco tiempos, tres radios, tres profundidades y alfa 0/127/255.
- 8 casos fuera de pantalla, con y sin contexto cooperativo, en cuatro direcciones.
- 6 casos de cámara cooperativa sin fijación.
- 24 casos de radio de arma: pistola, escopeta y rifle, agachado o de pie, contador de ataque y dos profundidades.
- 72 casos de fijación cooperativa sobre un automóvil real creado temporalmente: distancias 30/150, seis edades de fijación, objetivo nuevo o previo y tres relaciones entre jugador y vehículo. Se exige que se dibujen las dos miras. Se crean dos vehículos modelo 560 y se configura temporalmente el filtro de objetivos de frenesí; después se restauran los campos y se eliminan los vehículos de prueba.

Los dos índices de jugador comparten temporalmente el jugador real para ejercer las ramas. Esto no es una partida cooperativa manual. La comparación verifica las llamadas de dibujo, no capturas de píxeles ni una revisión visual humana. La tolerancia numérica se fijó en 0.002; máximo observado: 0.00006103515625. Los campos discretos coinciden exactamente. No se certifica estabilidad general de todas las funciones del juego.

La prueba detectó y permitió corregir una diferencia real en la flecha cooperativa: el desplazamiento del vértice interior también se aplica a los otros dos vértices. runtime/regression-detected.json conserva esa evidencia. Los primeros intentos también corrigieron la restauración de estados del propio instrumento y las precondiciones de los objetivos; no se aceptaron como pruebas aprobadas.

La compilación completa Debug x86 sin instrumentación pasó. El ASI normal se instaló en `D:\JUEGOS\GTA San Andreas - CVehicle Test`, con respaldo en `_baseline/gta_reversed-before-weaponeffects.*`. Los ejecutables del juego no cambiaron. Los hashes de las partidas originales y de pruebas se conservaron. Audio y Global/SurfaceInfos_c permanecieron desactivados por los fallos ajenos ya documentados.

## Repetir sin IA

Cerrar GTA y ejecutar `PROBAR.cmd --case weaponeffects` en la raíz del repositorio. El ejecutor es Python local, sin red, Codex ni servicios de IA. Verifica hashes del ejecutable y de la sonda archivada en `_baseline`, limita la ejecución a 180 segundos, cierra únicamente el proceso que inicia y restaura el ASI previo. No modifica el progreso documentado automáticamente.

Los informes se guardan en `resultados-weaponeffects` dentro de la copia de pruebas. `runtime/results.json` conserva cobertura, hashes y resultado final. El registro completo y los desensamblados son locales e ignorados por Git. `verify_analyzer.py` comprueba que el analizador rechace evidencia vacía, incompleta, duplicada, alteraciones gráficas, contador incorrecto, ausencia de la mira objetivo y aserciones.

Las cabeceras `runtime/Probe.hpp` y `runtime/LiveComparison.hpp` conservan el código de la sonda. La compilación normal no las incluye: Sprite.cpp y app_game.cpp se restauraron byte por byte, y Render conserva únicamente su reconstrucción. La sonda instrumenta temporalmente los dos métodos de dibujo de CSprite, el contador al entrar en Render y una llamada a WeaponEffectsLive::Update antes de CWeaponEffects::Render en RenderEffects.
