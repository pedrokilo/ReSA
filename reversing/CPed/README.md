# CPed: progreso comprobado

139/145 [96%]. Se completaron PositionAttachedPed (0x5DFDF0) y UpdatePosition (0x5E1B10). Quedan seis entradas, enumeradas en pending.json, sin reconstruir ni activar.

PositionAttachedPed coloca el peaton en la matriz de la entidad a la que esta unido, aplica el desplazamiento y la orientacion de torreta, limita la rotacion de peatones no jugadores y copia velocidades cuando corresponde. Reproduce la variante de MODEL_FIRELA con CAR_MISC_B y el desplazamiento z codificado por debajo de -900. Limpia m_standingOnEntity (0x568), conserva m_pContactEntity y marca bIsStanding.

La primera prueba no diferenciaba los dos campos de contacto al estar nulos. La revision del desensamblado detecto ese hueco: se corrigio el campo y se repitieron las pruebas con ambos punteros no nulos. Solo el ultimo informe runtime/results.json valida la version definitiva.

Prueba final dentro del juego: 361 comparaciones (caso sin entidad adjunta y 360 combinaciones), con modelos 560 y 544, cinco modos angulares, tres orientaciones, dos desplazamientos, jugador/peaton y tres valores controlados del tipo de entidad sobre vehiculos reales creados para el ensayo. Esto ultimo comprueba las ramas por tipo; no significa que se hayan creado tres clases diferentes de objetos. Incluye 90 casos de la matriz especial del camion de bomberos.

Se comparan las bases y posicion de la matriz, velocidades lineal/angular, rotacion y los dos campos de contacto, ademas de bIsStanding. Estados discretos iguales; tolerancia numerica fijada en 0.00001 y error maximo observado 0.00000262260437. Cada par confirma que el original no entra al candidato y que la version nueva entra exactamente una vez. El estado del jugador se restaura y las partidas no cambian. No certifica aspecto visual, todas las matrices posibles ni estabilidad general del mod.

Compilacion completa Debug x86 aprobada. Los documentos de progreso se actualizaron despues de verificar la evidencia real. Repetir sin IA: PROBAR.cmd --case cped-attached. El ejecutor usa la copia de pruebas, cierra su proceso y restaura el ASI previo. Usa los binarios instrumentados locales en _baseline.

UpdatePosition ya paso 2.534 comparaciones dentro del juego. Ver UpdatePosition/README.md y UpdatePosition/runtime/results.json. El siguiente objetivo pendiente es CalculateNewVelocity.
