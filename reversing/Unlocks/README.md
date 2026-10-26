<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueos solicitados e integración de plataforma

> Informe histórico de esta tanda. Para el ASI instalado más reciente, consultar la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Después se entregaron [DataStream/TrackEntity](../UnlockBatch2/README.md) y la [ampliación de CEntryExit](../CEntryExitPool/README.md).

Se retiraron **24 bloqueos en 16 clases/namespaces** después de comparar sus funciones con el original dentro del motor. CGeneral, la clase número 17 de la petición, ya estaba desbloqueada y conserva sus 65.536 comparaciones anteriores. No aumenta el número de funciones registradas como revertidas: los candados y el progreso son indicadores distintos. Las 17 clases solicitadas tienen cero candados en el CSV; en aquel corte quedaban **630 bloqueos en otras clases**. La [entrega posterior de DataStream y TrackEntity](../UnlockBatch2/README.md) retira diez más y deja 620.

[Resumen conjunto verificable](validation-summary.json). Las evidencias separan pruebas de funciones, integración de arranque y comprobación del ASI normal.

| Caso del único PROBAR.cmd | Candados retirados | Pares finales | Evidencia y alcance |
| --- | ---: | ---: | --- |
| `unlock-core` | 8 | 7.968 | [Geometría, recursos y memoria](../UnlockCore/README.md) |
| `unlock-tasks` | 3 | 7.168 | [Tareas de combate](../UnlockTasks/README.md) |
| `unlock-physical` | 2 | 3.072 | [Entidades físicas y sectores](../UnlockPhysical/README.md) |
| `unlock-audio` | 1 | 9.216 | [Posición de audio](../UnlockAudio/README.md) |
| `unlock-radio` | 2 | 224 | [Radio del usuario](../UnlockRadio/README.md) |
| `unlock-driving` | 1 | 16.640 | [Animaciones de conducción](../UnlockDriving/README.md) |
| `unlock-input` | 2 | 6.144 | [Ratón y mando](../UnlockInput/README.md) |
| `unlock-platform` | 5 adicionales | 324 (9 × 36) | [Arranque, menú y SDL/ImGui](../UnlockPlatform/README.md) |

Los primeros siete bancos suman **50.432 pares**, incluyendo las dependencias identificadas en cada README. Las repeticiones de arranque de plataforma se cuentan por separado y no se presentan como casos de función distintos. Las pasadas preliminares no se suman como cobertura nueva.

Los ocho candados de Core corresponden a CCover (2), CEntryInfoNode (2), CCurves, CDecisionMakerTypesFileLoader, CEntryExitManager y CVehicleRecording. Tasks cubre DestroyCarArmed (2) y ThrowControl. Platform completa CMenuManager, Ps (2) y Win (2), y también comprueba la integración de los dos hooks de CPad.

## Reparaciones relevantes

- ABI de Cover, AudioEnvironment, radio, destructor SeekEntityStandard y registros ECX/EDX de GetVehicleAppearance. Los adaptadores conservan los contratos que usan los callers nativos.
- Inserción y redistribución física con orden de listas/pools, sectores negativos y precisión x87 originales.
- Plataforma nativa de 40 bytes; ventana Windows compartida con SDL sin añadir campos al bloque del ejecutable. El bombeo de eventos y la captura de ImGui ya no requieren mantener esos siete hooks bloqueados.
- Tamaño de cámara tomado del área final de la ventana: corrige el fallo de `RwCameraGetRaster(camera)` observado cuando 1920×1080 solicitados terminaban en 1920×1055 reales. Se mantiene la aserción.
- Diagnóstico de excepciones: no reintentar indefinidamente una instrucción que falla; los bancos capturan RTC/aserciones sin dejar diálogos pendientes. La captura temprana de plataforma permite primero los manejadores SEH locales.

## Repetir sin supervisión

```powershell
.\PROBAR.cmd --case unlock-input
python reversing/UnlockPlatform/run_matrix.py
```

Cada banco usa un proceso propio de la copia de pruebas, plazo máximo, respaldos y restauración de ASI/PDB y ambos INI. FastLoader solicita temporalmente la ranura 2; el guardado incompatible se rechaza y no se altera. Las partidas se comprueban por hash. Los casos también forman parte de `--all`; la matriz de nueve arranques tiene ejecutor propio en Python, sin añadir otro CMD.

## Límites

Audio está activo en los bancos Audio y Radio; los demás bancos excluyen Audio y SurfaceInfos. Se documentan las dependencias controladas y las diferencias deliberadas del escáner Unicode. Los informes de bancos archivados corresponden a sus binarios y hashes, no prueban conjuntamente todas las clases del ASI normal más reciente.

Esto no garantiza estabilidad general, sesiones largas, todos los dispositivos físicos, errores de disco o memoria, hotplug, todos los modos de pantalla ni todas las combinaciones de hooks. La campaña integral sigue aplazada hasta completar las clases. No se modifican la instalación personal ni las partidas. Sin commit ni push.


## ASI normal instalado y comprobado

Compilación histórica de esta tanda, Debug x86 sin instrumentación, instalada entonces solo en la copia de pruebas: `112cdb045c60ce0a3ca3114f77fc321e8482f569fd7d5458bdf3ce6b225208e3`. Respaldo anterior: `_baseline/before-unlocks-20260923-122454`.

El inicio normal, sin argumentos ni exclusiones, llegó al menú en **68,89 s** y permaneció allí diez segundos, sin carga automática. Después, una ejecución del mismo ASI con carga rápida temporal completó **30 segundos en estado IDLE** y 863 frames de avance, con Audio y todos sus hooks habilitados, sin exclusiones. Es una observación breve del motor, no una prueba de conducción o combate. Las partidas e INI quedaron intactos; el ASI normal permaneció instalado.

Para repetir esta observación breve: `python reversing/Unlocks/run_current_build.py`. Tiene un límite total de 180 segundos y restaura la configuración normal al finalizar. El informe registra el hash del ASI efectivamente instalado.


La revisión de logs conserva avisos de adquisición del ratón antes de mostrar la ventana, una línea de timecyc inválida y el rechazo conocido del guardado modificado. En la observación conjunta se registró también una excepción de primera oportunidad en la ruta Intel/D3D9On12: el juego continuó y completó los 863 frames. El rótulo legado `UNHANDLED EXCEPTION` del manejador vectorizado no distingue primera oportunidad de una excepción final no manejada; no debe interpretarse por sí solo como cierre. No se afirma que los logs estén libres de avisos.

Seguimiento posterior: los [datos fueron reparados](../IKDataWarnings/README.md) y se comprobó la [recuperación natural del ratón](../MouseRecovery/README.md). El rechazo de la partida incompatible se conserva como protección. Estos resultados posteriores no alteran el registro histórico anterior.
