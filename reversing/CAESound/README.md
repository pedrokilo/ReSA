<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAESound: referencia inválida tras copiar un sonido

> Informe de esta entrega, conservado como evidencia histórica. El estado instalado más reciente y las entregas posteriores se documentan en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Los hashes y pruebas de este documento corresponden a su propia entrega.

El cierre registrado el 23 de septiembre de 2026 a las 00:36:43 ocurrió en `CAESound::UpdateParameters`, línea 252, al leer la posición de la entidad física de un sonido. Excepción `0xC0000005`, dirección leída `0xDDDDDE0D`, con `EDX=0xDDDDDDDD`. Este patrón es compatible con memoria liberada en una compilación de depuración. El usuario indicó que acababa de chocar.

## Defecto reproducido y reparación

`CAESound` utilizaba una asignación generada por el compilador. El miembro `EntityRef` no tiene asignación de copia explícita: esa operación copiaba el puntero sin retirar la referencia anterior ni registrar la nueva ubicación.

La prueba anterior a la corrección reprodujo la diferencia dentro de GTA: el original de `operator=` en `0x4EF680` registra dos referencias a la entidad de origen y limpia la anterior; nuestra copia dejaba una referencia antigua y no registraba el destino. Al ejecutar `ResolveReferences`, el destino original quedaba nulo y el candidato conservaba el puntero.

Se implementó `CAESound::operator=` de forma explícita: evita la autoasignación, retira la referencia previa, copia los campos que copia el original y registra el destino mediante la asignación de puntero de `EntityRef`. Conserva `m_IsAudioHardwareAware` y el relleno, igual que el original. La reparación se limita a CAESound; no cambia globalmente EntityRef.

## Validación

**1.024 pares exactos dentro del motor**, con Audio activo desde el arranque. Casos con entidad nula, entidad compartida o distinta, destino ya asociado, autoasignación y patrones diferentes en toda la estructura.

Se comprueban retorno, memoria completa —normalizando únicamente la dirección de la entidad—, conservación del origen, número de referencias, anulación de los dos punteros al retirar las entidades y detención del sonido al actualizarlo después. Los contadores prueban cero entradas al candidato durante el original y una durante el candidato.

Las entidades y los 16 nodos de referencias son estructuras aisladas. Se usan las funciones existentes de registro y resolución del motor; no se agota ni modifica la lista global de referencias. Se verifican jugador, tabla global, partidas, ASI/PDB y restauración byte por byte de los INI.

```powershell
python reversing/run.py --case sound-copy
```

[Resumen con hashes](validation-summary.json) · [Sonda](LiveComparison.hpp) · [Compilación](build_probe.py) · [Ejecutor](run_local.py).

Evidencia final: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-sound-copy\20260923-004932-410885`. Reproducción defectuosa conservada en la ejecución `20260923-004623-831808`. Registro del cierre original: `../InicioNormal/artifacts/crash-20260923-003643.log`.

Al cerrar esta entrega se instaló este ASI normal solo en la copia de pruebas: `08ac3949fc40b466eac6d61c600a4df6987247e692a43d295824f9c473b7668d`. Respaldo `_baseline/before-sound-copy-20260923`. Partidas intactas. No se cambiaron los indicadores del CSV: la función ya estaba registrada como reconstruida y habilitada.

## Límites

El defecto de referencias es real y reproduce el mecanismo compatible con el cierre observado; no se reprodujo exactamente la misma colisión del usuario. No se validaron todas las rutas de audio ni una sesión prolongada. El registro también contiene fallos de asignación de CEntryExit antes del cierre. Esa incidencia se investigó posteriormente: se reprodujo el agotamiento de 400 plazas también con el original y se amplió la reserva a 512. Consultar la [evidencia de CEntryExit y sus límites](../CEntryExitPool/README.md); no se atribuye a esa saturación el cierre de audio.

Este banco solo excluye Global/SurfaceInfos_c; Audio permanece activo. La prueba separada de inicio normal no aplica exclusiones. No se promete estabilidad general. Sin commit ni push.
