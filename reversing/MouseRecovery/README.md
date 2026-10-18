<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ratón: recuperación comprobada tras el aviso inicial

El 23 de septiembre de 2026 se reprodujo el `Acquire` inicial con HRESULT `0x80070005` y se observó su recuperación natural. **No fue necesario cambiar el código normal ni suprimir el mensaje.** ASI normal conservado: `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`, con las reparaciones anteriores de IK y datos.

## Evidencia dentro del motor

Proceso propio, únicamente en `D:\JUEGOS\GTA San Andreas - CVehicle Test`, sin exclusiones de hooks. Sonda temporal sobre las llamadas reales de DirectInput; no se sustituyó el dispositivo por un simulador, no se inyectó movimiento ni se pulsaron botones.

| Momento desde el primer intento | Observación |
| --- | --- |
| 0 ms | `Acquire` devuelve `0x80070005`; la ventana del juego está oculta y otra ventana tiene el foco. |
| 1.485 ms | Ventana visible y en primer plano. La primera lectura devuelve `0x8007000C`; el reintento de `Acquire` devuelve `S_OK` y la segunda lectura también devuelve `S_OK`, dentro de la misma llamada a GetMouseState. |
| 1.532 y 1.547 ms | Las siguientes inicializaciones de DirectInput adquieren el dispositivo con éxito. |
| 15.110 ms | Se han registrado 120 lecturas correctas y 120 actualizaciones de estado en UpdateMouse. Un fallo inicial de adquisición, un fallo inicial de lectura y un reintento exitoso. |

La secuencia corresponde a `diMouseInit` → `CPad::GetMouseState` → `CPad::UpdateMouse`. El modo de cooperación configurado es `DISCL_FOREGROUND`; los estados observados explican el fallo transitorio antes de que la ventana sea visible y tenga el foco. El registro prueba la recuperación en esta ejecución; no reconstruye retroactivamente el resultado exacto de cada sesión anterior.

Evidencia: `resultados-mouse-recovery/20260923-154347-737501`, dentro de la copia de pruebas. [Resumen verificable](validation-summary.json). Sonda ASI SHA-256 `c09c409dc44c30ade89ed3237dccbf2a67ea9210c436fa89e9616439901adccc`; registro `81c7768545e71b7bfb2038f8d93772763076105720eeb1b51295857bc9ef0e2f`.

La sonda captura resultados y visibilidad/foco sin añadir llamadas a Acquire o GetDeviceState. Las fuentes, ASI/PDB normales e INI se restauraron y verificaron por hash. Partidas intactas y GTA cerrado automáticamente. FastLoader=false y SaveGameToLoad=-2 para jugar normalmente.

## Alcance y repetición

```text
python reversing/MouseRecovery/build_probe.py
python reversing/MouseRecovery/run_local.py
```

El ejecutor exige reproducir el fallo inicial y observar después 120 actualizaciones válidas; si no se reproduce el fallo, no aprueba esta comprobación específica. Se usa el ejecutor compartido, con plazo de 180 segundos y restauración automática. No se creó otro PROBAR.cmd. Este diagnóstico observacional no se suma como pares contra el original ni como un caso diferencial del catálogo: permanecen 41 casos, 40 de motor y uno aislado.

No prueba movimiento físico, clics, desconexión/reconexión, todos los controladores ni todas las transiciones de foco. La comparación previa de lógica de ratón/mando sigue en [UnlockInput](../UnlockInput/README.md), y la comprobación previa de teclado/ratón con la interfaz en [UnlockPlatform](../UnlockPlatform/README.md). No se cambiaron indicadores CSV ni bloqueos, ni se certifica estabilidad general.

## Revisión documental

Se revisaron **62 Markdown propios** inventariados por Git, incluidos los no rastreados y `.github`; se excluyen cachés, dependencias descargadas y artefactos ignorados. Se comprobaron **944 enlaces a archivos locales**, sin destinos inexistentes; no se verifican URLs remotas ni fragmentos de encabezados. El inventario final y la comprobación están en `artifacts/docs-audit.json`; el inventario inicial y los 40 informes con navegación añadida se conservan junto a él. Se actualizaron referencias vigentes y se distinguieron los avisos históricos de los diagnósticos posteriores. Las guías técnicas y cifras de progreso que siguen siendo correctas se conservan.
