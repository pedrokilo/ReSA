<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Población, fábrica de vehículos y amenazas

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

[Informe de las diez clases](../Top10/README.md) · [Resumen verificable](validation-summary.json) · [Catálogo de pruebas](../README.md)

**Resultado final: 18.432 pares exactos, cero diferencias.** Segunda pasada con los hooks objetivo habilitados y desbloqueados. Las primeras ejecuciones se usaron para corregir los candidatos y no se suman como cobertura distinta.

| Clase | Entradas registradas | Comparaciones |
| --- | ---: | ---: |
| `CPopCycle` | 15/15 | 8.192 |
| `CCarCtrl` | 22/22 | 2.048 |
| `CTaskAllocatorKillThreatsBasic` | 6/6 | 8.192 |

| ID en el log | Función comparada | Pares |
| ---: | --- | ---: |
| 0 | `PickPedMIToStreamInForCurrentZone` | 8.192 |
| 1 | `GetNewVehicleDependingOnCarModel` | 2.048 |
| 2 | `ComputeClosestPeds` | 8.192 |

## Correcciones y cobertura

**Población:** el selector calcula el porcentaje entero mediante rand × 100 / 32768 y usa límites estrictos; el helper de grupos ya existente no tiene exactamente esas fronteras. Se comparan hasta diez intentos, índices circulares, grupos vacíos/ocupados, modelos cargados y razas. RNG controlado por lado, 18 grupos, tres zonas y 21 modelos de ped preparados; tablas globales restauradas.

**Fábrica:** comparación del subtipo elegido, tamaño de reserva, argumentos, byte createdBy y retorno. Automóvil con suspensión activa es la rama predeterminada; motos y BMX activan el caballete. Se prueban tipos de -2 a 13 y reserva correcta/fallida. Los constructores se interceptan tanto en el ejecutable como en la DLL para registrar el despacho y escribir buffers comparables.

**Amenazas:** matriz 8×8, distancias x87, mínimo global con desempate por orden de recorrido y retirada de fila/columna. Se contrasta el fallback al líder vivo o primer seguidor vivo y su asignación a miembros sin pareja, incluidos jugador o muertos cuando así actúa el original. Casos con nulos, muertos, jugador, distancias y empates; identificadores de asignación y guardas verificadas.

## Fallo nativo de asignación y límites

Si no hay memoria para una moto/BMX, el original intenta acceder a 0x614. El candidato conserva ese comportamiento; **no es un arreglo del agotamiento de memoria**. El banco captura esa excepción esperada mediante SEH. Solo durante la instrumentación, el manejador global deja pasar esa falta concreta cuando el caso la anuncia. El parche se retira antes de la compilación normal; la prueba inicial que encontró el conflicto con el manejador agotó su plazo y restauró el entorno.

No se revalidan los cuerpos de los constructores de vehículos ni toda la IA de tráfico. Las tablas de población contienen pesos válidos; no se pretende cubrir formatos corruptos, todos los modos de precisión FP o combate completo. El orden x87 y sus redondeos son parte del comportamiento comparado.

## Ejecución y trazabilidad

La comparación se ejecuta dentro de GTA contra la entrada original, cambiando temporalmente el hook. Cada par exige cero llamadas al candidato en el lado original y una en el lado candidato. Los hooks objetivo están habilitados y sin bloqueo al entrar al banco. El ejecutor comprueba cobertura exacta, ausencia de diferencias, restauración y hashes de los archivos.

Se excluyen Audio y SurfaceInfos durante estos bancos, como en el ejecutor común. FastLoader solicita temporalmente la ranura 2; en este entorno el log rechaza la versión del guardado y empieza una partida nueva. No se fuerza la carga ni se altera el archivo. ASI/PDB e INI se restauran y ambas copias de la partida conservan su hash.

Los artefactos locales archivados no se incluyen en Git. `build_probe.py` genera la instrumentación y restaura las fuentes en finally; `run_local.py` usa la sonda archivada de hash aprobado, sin recompilar. El comando siguiente termina solo y cierra únicamente el GTA que abrió. También está disponible en el único PROBAR.cmd y en --all.

```powershell
python reversing/run.py --case population-factory
```

Evidencia final local: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-population-factory\20260923-014955-867189`. El resumen conserva el SHA-256 del log, sonda, fuentes y dependencias; los archivos extensos quedan fuera de Git.
