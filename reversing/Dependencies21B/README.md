<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21B — CCamera::ProcessObbeCinemaCamera{Car, Train, Heli, Plane, Boat}

PENDIENTE de ejecución: banco escrito y compilable, todavía sin correr dentro del juego. No constituye validación hasta que `run_local.py` produzca un `comparisons.jsonl` aceptado.

Funciones (cohorte inicial de 499, grupo B de los 21 destinos inciertos; clasificadas GAME, ver `gb-classification.md`):

| Función | Entrada EXE | Instrucciones | Tabla | Entradas |
|---|---|---|---|---|
| ProcessObbeCinemaCameraCar | 0x5267C0 | 0x5267C0..0x526944 | 0x8CC828 | 11 (+1 de reserva en 0x8CC854) |
| ProcessObbeCinemaCameraTrain | 0x526950 | 0x526950..0x526AD4 | 0x8CC858 | 6 (+1 de reserva en 0x8CC870) |
| ProcessObbeCinemaCameraHeli | 0x526AE0 | 0x526AE0..0x526C7B | 0x8CC874 | 7 |
| ProcessObbeCinemaCameraPlane | 0x526C80 | 0x526C80..0x526E1B | 0x8CC894 | 6 |
| ProcessObbeCinemaCameraBoat | 0x526E20 | 0x526E20..0x526FB0 | 0x8CC8B0 | 3 |

El desensamblado completo está en `original-disassembly.txt`.

## Técnica de reconstrucción (source/game_sa/Camera.cpp)

Las cinco funciones comparten el mismo ciclo y se reconstruyen con un ayudante común `CameraObbeCinemaCycle(camera, kind)` y una tabla `ObbeCinemaKind` por tipo (dirección del índice, del tiempo y de la tabla, número de entradas y tres indicadores: `checkBusy`, `flagFirst`, `modeFallback`):

1. Si `bDidWeProcessAnyCinemaCam` (0xB6EC2D) es 0: índice = -1; si el contador de mensaje (0x8CC381, int8) es > 0 y `m_bCinemaCamera` es 0 lo decrementa y muestra el texto `CINCAM` (`CHud::SetHelpMessage(TheText.Get("CINCAM"), true, false, false)`); marca 0xB6EC34 = 1 (la del avión, antes del mensaje; las demás, después). Se relee 0xB6EC2D tras el mensaje.
2. Si 0xB6EC2D es distinto de 0: (helicóptero y avión) pregunta antes a 0x50B830 (la cámara activa está bajo el agua) y si es true salta al ciclo; en otro caso `IsItTimeForNewcam` (0x51D770, no reconstruida: se llama por la dirección del EXE) con `tabla[índice]` y el tiempo guardado; si es false sale con `m_nModeObbeCamIsInForCar = índice` y 0xB6EC2D = 1.
3. Ciclo: avanza `índice = (int8(0x8CC471) + índice) % n` (resto con signo; negativo → n-1) y llama a `TryToStartNewCamMode(tabla[índice])`; mientras falle y `intentos <= n` repite. `encontrada = intentos < n`.
4. Coche/tren: guarda el tiempo (0xB7CB84) siempre; si no encontró, usa la entrada de reserva `tabla[n]` con índice = n. Helicóptero/avión/barco: si encontró guarda el tiempo; si no, índice = n y, si la cámara activa (palabra en `m_aCams[m_nActiveCam].m_nMode`, 0x180 + activa*0x238) no es 0x12, `TryToStartNewCamMode(0x1D)` y guarda el tiempo (si es 0x12 no llama ni toca el tiempo).
5. Epílogo: `m_nModeObbeCamIsInForCar` (0xC34) = índice (releído tras cada colaborador) y 0xB6EC2D = 1.

Todo es aritmética entera y de bytes: no hay x87 (no se añade ningún ayudante a X87Math). Ningún llamador lee EAX/ECX/EDX (ver `gb-audit.txt`), así que no se usa ensamblador ni ayudantes de residuo. Los llamadores de CamControl (`plugin::CallMethod<0x526C80...>`) no se han tocado: siguen pasando por la entrada del EXE (que ahora salta al hook), con lo que el banco CamControl355 conserva su `EntryPatch(0x526C80, …)`.

Hooks: `RH_ScopedInstall(ProcessObbeCinemaCameraCar|Train|Heli|Plane|Boat, 0x5267C0|0x526950|0x526AE0|0x526C80|0x526E20)`.

## Banco

- 5 funciones x 2048 casos = 10240 pares OFF/ON (original = EXE con el hook RH apagado; candidato = DLL con el hook encendido, vía `HookRoute`). La entrada del EXE es la misma en los dos lados.
- `this` = un búfer privado de `sizeof(CCamera)` = 0xD78 bytes rodeado de guardas, relleno con bytes aleatorios deterministas; se fijan `m_nActiveCam` (0..5), `m_bCinemaCamera`, el modo de las cámaras (0x12 / otros / con bit alto) y 0xC34. La DLL y el EXE ven los mismos bytes.
- Estado estático del EXE (guardado antes y restaurado después de cada lado): tablas 0x8CC820..0x8CC8CF (valores normales, 0, 1, 0x12, 0x1D, INT_MAX, INT_MIN, -1, -2), los cinco índices (0x8CCEEC..), los cinco tiempos (0xB70120..), 0xB6EC2D (0, 1, 2, 0x80, 0xFF), 0xB6EC34, contador de mensaje (int8 completo, 0, 1, -1, 127, -128), paso (int8 completo, ±1, ±2, ±7, 127, -128, 10, 11, 12), CTimer (0xB7CB84, incluido 0xFFFFFFFF/0x80000000). Índice de entrada: -1, 0..n+1, ±1000, 0x40000000, 0x80000000, 0xC0000000, 0x7FFFFFFF, 0x3FFFFFFF (todos leen memoria válida del EXE por el envoltorio de 32 bits).
- Colaboradores falsos, con traza completa (argumentos, `this` es el búfer, y una foto del estado compartido en cada llamada): `IsItTimeForNewcam` 0x51D770 (solo EXE, la DLL lo llama por dirección), `TryToStartNewCamMode` 0x51E560 (EXE y `Body()` de la DLL), 0x50B830 (solo EXE), `CText::Get` 0x6A0050 (EXE y DLL; se registra la clave hasta el NUL) y `CHud::SetHelpMessage` 0x588BE0 (EXE y DLL; los cuatro argumentos como dwords completos). Sus resultados salen de máscaras de bits por caso (todo falso, todo verdadero, alternos, un único acierto en la posición k, aleatorios) y, según el caso, modifican el estado compartido (0xB6EC2D, índices, tiempos, contador, CTimer) para exigir que se relea como el EXE.
- Observables comparados en bits: traza de colaboradores completa; diferencias de memoria en todas las zonas modificables; zonas vecinas vigiladas sin cambios; diferencias del búfer de la cámara y guardas; modo final (0xC34); ESP, EBX/ESI/EDI/EBP, FS:[0]; entorno x87 (fcw, fsw con TOP, ftw) y estado FP restaurado; cadena `CINCAM` del EXE intacta. EAX/ECX/EDX no se comparan (no son observables, ver `gb-audit.txt`).
- `run_local.py` exige los 10240 pares, todas las compuertas verdaderas en ambos lados, trazas idénticas y, por función, cobertura de: mensaje mostrado, comprobación de tiempo y salida anticipada por «no es hora», primer intento, intentos tras reintentos, reintentos agotados (reserva/0x1D), índice final de tabla y de reserva, y (helicóptero/avión) la comprobación 0x50B830; además falla si el coche, el tren o el barco llaman a 0x50B830. Nunca se relaja una compuerta.
- Verificación previa fuera del juego (no sustituye al banco): un intérprete de las instrucciones reales del EXE frente al C++ con los mismos colaboradores falsos dio 0 diferencias en 20000 escenarios por función (índices iniciales de -1000 a 1000; los extremos 0x40000000/0x80000000/0xC0000000/0x7FFFFFFF/0x3FFFFFFF solo se ejercitan en el banco porque dependen del espacio de direcciones real; fuentes en `gb-harness/`); cinco mutaciones del C++ (orden de 0xB6EC34 del avión, orden de la escritura del tiempo, sentido de 0x50B830, orden de `index = n` y límite del bucle) fueron detectadas.

## Ejecución

Desde la raíz del repositorio, con el proyecto compilado con la fuente convertida (Camera.cpp):

```
python -B reversing/Dependencies21B/build_probe.py
python -B reversing/Dependencies21B/run_local.py
python -B reversing/Dependencies21B/run_local.py --analyze <comparisons.jsonl>
```

o `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de evidencia> Dependencies21B`.
