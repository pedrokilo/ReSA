<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Escáner, expulsión y primitivas FX

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

[Informe de las diez clases](../Top10/README.md) · [Resumen verificable](validation-summary.json) · [Catálogo de pruebas](../README.md)

**Resultado final: 9.216 pares exactos, cero diferencias.** Segunda pasada con los hooks objetivo habilitados y desbloqueados. Las primeras ejecuciones se usaron para corregir los candidatos y no se suman como cobertura distinta.

| Clase | Entradas registradas | Comparaciones |
| --- | ---: | ---: |
| `CEntityScanner` | 4/4 | 4.096 |
| `CTaskComplexDragPedFromCar` | 2/2 | 4.096 |
| `FxPrimBP_c` | 2/2 | 1.024 |

| ID en el log | Función comparada | Pares |
| ---: | --- | ---: |
| 0 | `ScanForEntitiesInRange` | 4.096 |
| 1 | `CreateFirstSubTask` | 4.096 |
| 2 | `Load` | 1.024 |

## Correcciones y cobertura

**Escáner:** temporizador, borrado de referencias anteriores, rango máximo de vista/oído, límites de sectores, códigos de escaneo y rollover. Recorrido y orden estable de hasta 16 entidades más cercanas, distancia 3D estricta y filtro de muertos para peds no jugadores. Sectores y listas privados incluyen duplicados, entidad propia, límites del mapa y más candidatos que capacidad. Se comprueban IDs, orden, referencias y entidad más cercana. Se conserva aritmética x87 del original.

**Expulsión:** ocupante válido/inválido, conductor/pasajero, coche, moto y quad; sustitución y limpieza de la referencia al vehículo, flags de terminación y asiento/puerta. Se controlan la fábrica de la clase EnterCar y el helper de puerta en ambos lados, comparando llamadas, argumentos y bytes de la tarea. No se sustituye la gestión real de referencias.

**FX:** versiones 0, 100, 101, 102, 103, 104, 109 y 110; matriz identidad y no identidad, reserva de 24 bytes alineada a 2, compresión, nombres de texturas, alpha y blends de byte, guardas, posición del archivo y marca final. Las versiones <=101 preservan nombres adicionales. Se usa un pool privado y archivo temporal; FxInfoManager se llama en su implementación original con cero entradas.

## Registro de hooks y límites

ScanForEntitiesInRange se registra como virtual con RH_ScopedVMTInstall y vtable exportada: el registro simple anterior apuntaba al thunk de llamada virtual. Load tiene cuerpo base de una virtual pura; un adaptador no virtual Load_Reversed permite registrar el nombre Load sin interceptar el thunk. Un intento previo agotó tiempo por esa recursión y se restauró automáticamente.

No se prueba una secuencia completa de expulsión/animación, todas las escenas del mundo, todos los modos FP ni archivos FX malformados. La prueba de Load no certifica los tipos de FxInfoManager o los píxeles renderizados. La sonda final sí arrancó con Load habilitado y cargó los efectos usados por el inicio del juego.

## Ejecución y trazabilidad

La comparación se ejecuta dentro de GTA contra la entrada original, cambiando temporalmente el hook. Cada par exige cero llamadas al candidato en el lado original y una en el lado candidato. Los hooks objetivo están habilitados y sin bloqueo al entrar al banco. El ejecutor comprueba cobertura exacta, ausencia de diferencias, restauración y hashes de los archivos.

Se excluyen Audio y SurfaceInfos durante estos bancos, como en el ejecutor común. FastLoader solicita temporalmente la ranura 2; en este entorno el log rechaza la versión del guardado y empieza una partida nueva. No se fuerza la carga ni se altera el archivo. ASI/PDB e INI se restauran y ambas copias de la partida conservan su hash.

Los artefactos locales archivados no se incluyen en Git. `build_probe.py` genera la instrumentación y restaura las fuentes en finally; `run_local.py` usa la sonda archivada de hash aprobado, sin recompilar. El comando siguiente termina solo y cierra únicamente el GTA que abrió. También está disponible en el único PROBAR.cmd y en --all.

```powershell
python reversing/run.py --case scanner-drag-fx
```

Evidencia final local: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-scanner-drag-fx\20260923-015108-016690`. El resumen conserva el SHA-256 del log, sonda, fuentes y dependencias; los archivos extensos quedan fuera de Git.
