<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed: progreso comprobado

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

145/145 entradas registradas reconstruidas y habilitadas. Las seis pendientes se completaron el 2026-09-22 tras compararlas con el original dentro de GTA. Este porcentaje mide entradas registradas, no cobertura exhaustiva ni todas las funciones virtuales no registradas.

PositionAttachedPed coloca el peaton en la matriz de la entidad a la que esta unido, aplica el desplazamiento y la orientacion de torreta, limita la rotacion de peatones no jugadores y copia velocidades cuando corresponde. Reproduce la variante de MODEL_FIRELA con CAR_MISC_B y el desplazamiento z codificado por debajo de -900. Limpia m_standingOnEntity (0x568), conserva m_pContactEntity y marca bIsStanding.

La primera prueba no diferenciaba los dos campos de contacto al estar nulos. La revision del desensamblado detecto ese hueco: se corrigio el campo y se repitieron las pruebas con ambos punteros no nulos. Solo el ultimo informe runtime/results.json valida la version definitiva.

Prueba final dentro del juego: 361 comparaciones (caso sin entidad adjunta y 360 combinaciones), con modelos 560 y 544, cinco modos angulares, tres orientaciones, dos desplazamientos, jugador/peaton y tres valores controlados del tipo de entidad sobre vehiculos reales creados para el ensayo. Esto ultimo comprueba las ramas por tipo; no significa que se hayan creado tres clases diferentes de objetos. Incluye 90 casos de la matriz especial del camion de bomberos.

Se comparan las bases y posicion de la matriz, velocidades lineal/angular, rotacion y los dos campos de contacto, ademas de bIsStanding. Estados discretos iguales; tolerancia numerica fijada en 0.00001 y error maximo observado 0.00000262260437. Cada par confirma que el original no entra al candidato y que la version nueva entra exactamente una vez. El estado del jugador se restaura y las partidas no cambian. No certifica aspecto visual, todas las matrices posibles ni estabilidad general del mod.

Compilacion completa Debug x86 aprobada. Los documentos de progreso se actualizaron despues de verificar la evidencia real. Repetir sin IA: PROBAR.cmd --case cped-attached. El ejecutor usa la copia de pruebas, cierra su proceso y restaura el ASI previo. Usa los binarios instrumentados locales en _baseline.

UpdatePosition ya paso 2.534 comparaciones dentro del juego. Ver UpdatePosition/README.md y UpdatePosition/runtime/results.json. CalculateNewVelocity tambien esta completada; ver el banco conjunto siguiente.

## Cierre de las seis entradas pendientes

| Funcion | Direccion | Pares original/candidato |
|---|---|---:|
| CalculateNewVelocity | 0x5E4C50 | 4096 |
| DoFootLanded | 0x5E5380 | 2048 |
| PlayFootSteps | 0x5E57F0 | 8192 |
| MakeTyresMuddySectorList | 0x6AE0D0 | 2048 |
| PositionPedOutOfCollision | 0x5E0820 | 4096 |
| KillPedWithCar | 0x5F0360 | 8192 |

Total: 28.672 pares aprobados; error maximo 0.000000476837158203125, tolerancia absoluta 0.00001. Ver `validation-summary.json` para hashes y ruta de la evidencia final. Se conservaron los bancos anteriores de PositionAttachedPed (361 pares) y UpdatePosition (2534); sus sondas no se volvieron a ejecutar en este cierre.

Se reconstruyeron el giro y desplazamiento animado, las transiciones de pie y aterrizaje, sangre/lluvia/polvo/agua, el recorrido de vehiculos con contacto de ruedas, las alternativas de salida de colision y las respuestas a atropellos. Se corrigieron los limites estrictos y la lectura de `m_vecLastCollisionImpactVelocity.z` en el impacto leve. El algoritmo de neumaticos conserva incluso el puntero anterior cuando el siguiente vehiculo es de un tipo no admitido, como hace el original.

Cada par ejecuta la direccion original con el hook apagado y despues el candidato con el hook activo, usando almacenamiento independiente. Los contadores comprueban cero entradas al candidato en el primer recorrido y una en el segundo. Se comparan estados, resultados, secuencia y argumentos de dependencias y escrituras fuera de los campos previstos. Los cambios temporales de hooks, parches y globals se restauran antes de continuar el frame. El jugador real no se modifica; el runner comprueba tambien hashes de partidas y restaura el ASI/PDB instalado.

### Alcance de las dependencias

- CalculateNewVelocity usa matrices, temporizador y consultas de superficie del motor. Las tareas de la fixture estan vacias: no se cubren todas las cadenas activas de tareas.
- DoFootLanded controla huesos, consultas de sangre/superficie/lluvia/agua, sombras, particulas y audio. Comprueba argumentos y cambios de estado; no valida el resultado grafico ni sonoro.
- PlayFootSteps controla asociaciones, RNG, aterrizaje, adhesion, ropa, audio y recepcion del evento; usa el constructor real de CEventSoundQuiet y su limpieza de referencias.
- Neumaticos compara listas sinteticas, duplicados, scan codes, limites de distancia/velocidad, sangre y suspension. Las fuerzas de CPhysical son reales; caja de colision, audio y vibracion estan controlados.
- Recolocacion controla colisiones, linea de vision, puertas, nodos y altura del suelo para recorrer salidas iniciales, laterales, extremos, techo, fracaso y fallback de nodos. No representa toda la geometria de los interiores/mapa.
- Atropellos controla tareas consultadas, RNG, daño, recepcion de eventos, animacion, audio, crimen, capó y aplicacion final de fuerza. Compara el impulso solicitado; la fuerza angular sobre el capó usa CPhysical real. Incluye excepciones, impactos leves, atropellos y rebotes, pero no una simulacion completa de trafico o lesiones.

La sonda redirige temporalmente llamadas C++ directas de esas dependencias a sus entradas originales para aplicar el mismo control a ambos lados. Esa redireccion NO queda en el ASI normal. DoFootLanded conserva dos dependencias originales independientes (0x5E3630 y 0x5E37C0) mediante adaptadores de su ABI en registros; no son delegaciones de las seis funciones reconstruidas.

Se excluyen `Audio` y `Global/SurfaceInfos_c`, como en la infraestructura existente. Este banco finito no certifica estabilidad general, sesiones largas, todas las misiones ni compatibilidad de mods. La campaña integral sigue aplazada hasta completar todas las clases.

### Repeticion autonoma

`PROBAR.cmd --case cped-completion` o `python reversing/run.py --case cped-completion`. Integrado en `--all`, conservando un solo PROBAR.cmd. El runner usa la sonda archivada y termina sin supervision; no es una prueba integrada de todas las clases en la compilacion actual. `build_completion_probe.py` recompila instrumentacion temporal y restaura los fuentes incluso si falla: revisar su manifiesto y renovar conscientemente el hash y los binarios archivados para repetir con otra revision.

Solo se instala en `D:/JUEGOS/GTA San Andreas - CVehicle Test`. Instalacion personal y partidas conservadas. Sin commit ni push.
