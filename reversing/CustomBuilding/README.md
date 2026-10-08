<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CCustomBuildingRenderer: trabajo aplazado

[Continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente)

El usuario pidió pausar el 2026-09-23. Se conserva la implementación local y el banco; no continuar la integración sin una nueva petición. HEAD comprobado: `11fd6912f759d3bc93208a9def001b6dd751dbdf`, rama ReSA. Sin commit ni push.

## Resultado registrado

La última prueba ya iniciada terminó automáticamente: **95.168 pares exactamente coincidentes**, con los cinco hooks habilitados y sin bloqueo desde el arranque. [Resumen y manifiesto](validation-summary.json). Ejecución: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-custom-building/20260923-173028-884697`.

| Función | Dirección | Pares |
| --- | --- | ---: |
| AtomicSetup | 0x5D7F00 | 256 |
| IsCBPCPipelineAttached | 0x5D7F40 | 256 |
| UpdateDayNightBalanceParam | 0x5D7F80 | 90.496 |
| Update | 0x5D8050 | 4.096 |
| CCustomBuildingDNPipeline::CustomPipeAtomicSetup (dependencia) | 0x5D71C0 | 64 |

El reloj cubre los 86.400 segundos del día y 4.096 combinaciones de campos crudos. Update incluye desbordamiento del contador. Los fixtures son objetos RenderWare independientes, con combinaciones de colores nocturnos/preiluminados e identificadores. Se comparan retornos y estado, con contadores original/candidato 0/1. Jugador, entradas, globales y hooks restaurados. **Sin materiales ni dibujo: no valida pipelines completos ni estabilidad general.**

Se reconstruyeron las cuatro funciones pendientes. AtomicSetup comprueba NightColors, no solo el puntero al bloque de datos. El balance conserva redondeos del original. Se corrigió además el ID por atomic del helper DN a 0x53F20098; la constante de la cabecera y los metadatos de CreateCustomObjPipe no se cambiaron y requieren revisión aparte.

## Evidencia y limitaciones

Primer intento: 1.493 diferencias de balance y 8 de Update; segundo: 3 y 5. Se corrigieron los redondeos y se conservaron los intentos en resultados y artifacts. El banco previo a habilitar los hooks desde arranque también aprobó 95.168 pares (20260923-172853-300483). Las repeticiones no aumentan cobertura única. La prueba final es la del resumen enlazado.

`build_probe.py` compila una sonda temporal y restaura fuentes y binarios normales; `run_local.py` usa el ejecutor compartido. No se añadió otro CMD ni se registró todavía este caso en el menú o la suite. Los artefactos pesados locales están en `artifacts/`; el resumen conserva sus hashes.

## Punto de reanudación

- Fuentes candidatas conservadas con los cuatro hooks habilitados; **CSV y catálogo todavía 3/7**. Es una entrega aplazada, no una clase declarada terminada.
- Pendientes: revisión final de cambios y dependencias, compilación normal, integración sin sonda, instalación autorizada en la copia de pruebas y cierre documental/registro/catálogo.
- ASI normal de bin/Debug y copia de pruebas sigue en `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`; PDB `e84a25d8106924485ebfeb8648ebf219d65bc9ae255986509fa2399bffad48a1`. Comprobados al pausar. La nueva implementación no está instalada para juego normal.
- El runner cerró su GTA, restauró ambos INI y verificó las partidas por hash. No quedaba GTA activo al comprobar. FastLoader=false y SaveGameToLoad=-2 se conservan por restauración.
- El registro general conserva 33 evidencias históricas. Sus tres acreditaciones anteriores quedan pendientes por el cambio de huella del árbol source; no significa que esas funciones hayan fallado. Esta clase aún no se incorporó al registro.
- Siguen aplazados los 86 bloqueos expresamente excluidos. No aplicar el parche diferido.
