# IK y avisos de datos — 23 de septiembre de 2026

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Petición: reparar los avisos pendientes y revisar el debug adjunto de las 15:02. Se conserva el trabajo previo de peatones y CEntryExit. No cambian indicadores de hooks.csv ni se retoman los 86 bloqueos aplazados.

## Diagnóstico y cambios

- **IK:** `SetupBones` (0x617CA0) excluye el pivote al construir la cadena. La reconstrucción intentaba buscarlo después, produciendo un aviso aunque su ausencia era correcta. Ahora omite ese enlace. El original hace una búsqueda interna y omite el padre ausente sin error. Se conserva el aviso para otras búsquedas fallidas. La cadena cabeza → cuello reproduce el caso del hueso 4 registrado anteriormente; no se afirma haber descartado cualquier otra causa de avisos IK.
- **TIMECYC.DAT:** una fila de 49 campos tenía incompleto el RGB ambiente. Se establece explícitamente `255 255 255`; es una decisión de reparación, no una recuperación demostrada de los valores perdidos. Las otras 183 filas tenían 51 campos; ambos lectores solicitan 52. Se añade el multiplicador direccional neutro `1.0` a las 184 filas, evitando depender de una variable sin inicializar. Esto corrige los datos y cambia deliberadamente el resultado antes indeterminado; no equivale a conservar el clima defectuoso.
- **vehicles.ide:** la definición de `skimmer` tenía 11 campos. Se añaden `-1, 0.7, 0.7`, exactamente los valores predeterminados que ya utilizaba la reconstrucción. El banco comprueba esos campos en el modelo cargado. No se declara validado todo `LoadVehicleObject` ni la conducción del hidroavión.
- **CARGRP.DAT:** los grupos 6 y 7 tenían 26 y 29 modelos. Su capacidad es 23; se mantienen los primeros 23 y se retiran únicamente las entradas sobrantes que ya se ignoraban. El mensaje «no memory» no significaba agotamiento de RAM. No se amplía la capacidad ni cambia la selección efectiva de esos grupos.

Los tres archivos se modificaron **solo en CVehicle Test**. [Manifiesto](data-repair.json) con hashes y líneas; originales y diferencias completas en el respaldo allí indicado. No se modificó el lector de clima ni el de vehículos ni Population.

## Validación

Sonda final: `resultados-ik-data/20260923-152556-990651`, dentro de la copia de pruebas. SHA-256 ASI `218564db47286769690da2734e5b34e9f2b6c0fecb61e4a5007fec4b9edf91f2`.

**131 pares únicos de SetupBones**, con efectores y pivotes presentes en el esqueleto del jugador: coinciden orden, etiquetas, padres, orientación, límites y velocidad. Incluye cabeza/cuello y raíces sin padre interno. No se compara padding, direcciones de nodos ni matrices mundiales aún sin calcular. No prueba la resolución dinámica del objetivo, todas las animaciones ni esqueletos modificados.

Cada ejecución original registra cero entradas al código candidato; cada ejecución candidata registra una. Se emplean cadenas privadas, el esqueleto real de solo lectura y Exit correspondiente a cada implementación. Se comprueban devolución de nodos, restauración del gestor, jugador, frames y keyframes intactos. Los hooks recuperan su estado al salir.

**9.384 bytes** de tablas de clima iguales tras cargar los datos corregidos con `Initialise` original y reconstruido. Se restaura el estado del clima después. Este número es una comparación de tablas, no 9.384 casos independientes. Las entradas activas y cantidades de los grupos de coches también coinciden con el original; se excluyen los huecos no utilizados. `skimmer` carga los valores explícitos esperados.

La primera pasada también aprobó 131 pares; no se suma como cobertura nueva. La sonda final añade la comprobación del esqueleto y del modelo skimmer.

**Integración normal aprobada:** 121,16 segundos, 3.605 fotogramas, cero avisos de IK, timecyc, argumentos de vehículos, capacidad de grupos y peatones fuera del mapa; cero fallos de CEntryExit, máximo 403/512 entradas. Evidencia `reversing/Unlocks/artifacts/current-20260923-152730`. No hubo cierre inesperado, aserción ni RTC registrado. Persisten el aviso inicial del ratón y el rechazo previsto de la partida incompatible, descritos abajo.

[Resumen verificable](validation-summary.json). ASI normal `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`. Presentes en bin/Debug y únicamente en la instalación de pruebas, con respaldo `_baseline/before-ik-normal-20260923-152717`. Configuración normal restaurada y partidas sin cambios por hash. No se probaron conducción, combate, guardado/carga completo ni sesiones prolongadas.

## Repetición y restauración

```text
python reversing/IKDataWarnings/repair_data.py
python reversing/IKDataWarnings/repair_data.py --apply
python reversing/IKDataWarnings/build_probe.py
python reversing/run.py --case ik-data
python reversing/Unlocks/run_current_build.py --ik-data --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600
```

El primer comando solo inspecciona; el segundo es idempotente y exige GTA cerrado. Se rechazan versiones de datos distintas de los hashes documentados. Para deshacer los datos con GTA cerrado, copiar únicamente `timecyc.dat`, `vehicles.ide` y `cargrp.dat` desde el respaldo indicado a `data` de la copia de pruebas. No copiar nada a la instalación personal.

El compilador de la sonda restaura fuentes y binarios normales incluso si falla. El ejecutor termina su propio proceso, restaura ASI/PDB e INI y verifica hashes de partidas. Integrado en el único PROBAR.cmd y `--all`: 41 casos, 40 de motor y uno aislado. No se ejecutó toda la suite.

## Otros mensajes del adjunto

El adjunto pertenece a la integración de las 15:02, no a la aceptación posterior de 3.609 fotogramas. No contiene avisos de peatones ni IK. Sí contiene los datos corregidos aquí, además de:

- **Ratón, HRESULT 0x80070005:** el adjunto no mostraba su recuperación. Una [observación posterior](../MouseRecovery/README.md) reprodujo el fallo con la ventana oculta y sin foco, seguido de adquisición y lectura correctas a los 1,485 s y 120 actualizaciones válidas. No hizo falta cambiar el código ni suprimir el mensaje; esto no prueba retrospectivamente todas las sesiones anteriores.
- **Partida incompatible:** la prueba intentó la ranura 2, marcada `v1 Modified`, frente a `v1 Unmodified`. El rechazo y el inicio de una partida nueva son la protección prevista. No se fuerza compatibilidad, no se convierte ni sobrescribe la partida. El ejecutor conserva esta configuración temporal; para jugar normalmente se restauran FastLoader=false y SaveGameToLoad=-2.

No hay un cierre inesperado documentado en ese fragmento. Esto no garantiza estabilidad general ni compatibilidad completa de partidas.
