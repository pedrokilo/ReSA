<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Asignadores de tareas por defecto de grupo: Random y SitInLeaderCar

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Validadas el 01/10/2026 con **4.608 pares original/candidato dentro de GTA**, todos exactos:

| Función | Dirección | Pares |
| --- | --- | ---: |
| `CPedGroupDefaultTaskAllocatorRandom::AllocateDefaultTasks` | 0x5F6E90 (vtable 0x86C77C) | 2.048 |
| `CPedGroupDefaultTaskAllocatorSitInLeaderCar::AllocateDefaultTasks` | 0x5F6FC0 (vtable 0x86C784) | 2.048 |
| `CTaskComplexGangFollower::SetCanEnterLeadersCar` | 0x65EE90 | 512 |

Con esto, `CPedGroupDefaultTaskAllocatorRandom` y `CPedGroupDefaultTaskAllocatorSitInLeaderCar` quedan **2/2** con sus `AllocateDefaultTasks` en `validated=1` ([registro](../validation-registry.json)).

## Método

Cada par llama a la función por su entrada del EXE (ranura 0 de la vtable de la instancia estática 0xC09918/0xC0991C, o 0x65EE90). Primero con el hook desactivado (código original) y después activado (candidato), sobre los mismos fixtures. El banco comprueba además que el hook conmuta de verdad, que el original no entra en la DLL (contadores) y que el candidato entra exactamente una vez. Mientras se ejecuta el original también se desactiva `SetCanEnterLeadersCar`, para que todo el recorrido sea código del EXE.

- **Fixtures:** grupo, diez peatones y un vehículo en memoria propia. Se combinan miembros presentes o ausentes, los pares por defecto (peatón igual, nulo, extraño o desplazado) y tareas centinela ya asignadas. El líder puede faltar, ser un fixture o ser el jugador real. El `ped` pedido puede ser nulo, de un par, el líder o un extraño. También varían `m_bMembersEnterLeadersVehicle`, si el líder tiene vehículo y `m_nMaxPassengers` (0–9).
- **Comparación:** pares peatón/tarea resultantes y todas las tareas nuevas del pool real. De cada tarea se comparan el tipo y sus bytes, con los punteros traducidos a identificadores (tareas, fixtures, jugador). También se comparan la secuencia de consultas de puerta, las fugas (tareas nuevas no alcanzables), la restauración del pool tras liberar y la inmutabilidad de peatones, vehículo y grupo.
- **Cobertura:** 6.477 tareas comparadas (seguidor, líder, secuencia, subir como conductor/pasajero, conducir), 1.696 consultas de puerta, 5.809 asignaciones y 389 casos con el jugador como líder.

Al restaurar se comprueban los hooks, el jugador, la ocupación del pool, los INI, el ASI/PDB y las partidas.

## Límites

- `CCarEnterExit::ComputeTargetDoorToEnterAsPassenger` se controla en el EXE y en la DLL (puerta determinista por asiento y semilla); se compara su traza, no su lógica.
- En `CTaskComplexSequence` se excluyen los 3 bytes de relleno tras `m_bFlushTasks` (0x39..0x3B). El original los deja con el patrón del pool y la inicialización por valor de la DLL (`new CTaskComplexSequence{}`) los pone a cero. No tienen valor definido; la primera pasada lo detectó como diferencia ([informe fallido](#evidencia)).
- Los constructores y destructores de tareas son dependencias compartidas: no se acredita aquí su equivalencia.
- Sin revisión de dependencias: no acredita independencia del EXE ni estabilidad prolongada.

## Repetir

```powershell
python reversing/GroupTasksCompletion/build_probe.py   # sonda temporal; restaura las fuentes
python reversing/run.py --case group-alloc
python reversing/GroupTasksCompletion/write_summary.py # resumen y registro desde el último informe aprobado
```

## Evidencia

Resumen, hashes y manifiesto de la sonda: [validation-summary.json](validation-summary.json). Log aprobado: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-group-alloc/20261001-235300-178372/comparisons.jsonl`. La primera pasada (`20261001-234924-039832`) falló en el caso 640 de `SitInLeaderCar` solo por el relleno descrito. La segunda (`20261001-235047-738785`) aprobó antes de actualizar los comentarios de estado. Desensamblado de referencia: [allocators.disasm.txt](allocators.disasm.txt).
