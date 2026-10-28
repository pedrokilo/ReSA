<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ReSA: reconstrucción, validación e independencia

**Prioridad vigente:** convertir la producción de `source/game_sa` a C++ sin ensamblador en línea ni `naked`, con resultados exactos frente al EXE y validación dentro del juego. [Inventario y evidencia de la migración](../reversing/CppOnlyMigration/README.md). Esta migración sigue pendiente y se mide por separado del 100% histórico del catálogo.

[Inicio](../README.md) · [Indicadores del catálogo](ReversedClasses.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas](../reversing/README.md)

## Objetivo

Recrear GTA San Andreas PC de forma funcionalmente equivalente mediante código fuente reconstruido, reduciendo hasta eliminar la dependencia de funciones originales del ejecutable. Completar `ReversedClasses.md` es un paso intermedio: el objetivo es una reconstrucción completa, validada y ejecutable, seguida de un ejecutable propio.

El contador técnico decisivo será **cuántas funciones siguen necesitando código original**, incluidas sus dependencias transitivas. El 96,87% registrado como reconstruido no representa el porcentaje recorrido hacia un GTA SA independiente.

## Cuatro dimensiones de progreso

Estado documental del 10 de octubre de 2026 (corte de validación 453). Clases y reconstrucciones se calculan sobre `hooks.csv`; la validación procede del registro parcial de evidencia y versiones. La independencia aún no tiene medición y no se rellena con estimaciones.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas en el catálogo | `████████████████████` | 815/825 · 98,79% |
| Clases validadas de las completas | `████████████████████` | 815/815 · 100,00% |
| Funciones registradas como reconstruidas | `███████████████████░` | 8.965/9.255 · 96,87% |
| Funciones con evidencia aprobada (`validated=1`) de las reconstruidas | `████████████████████` | **8.965/8.965 · 100,00%** |

**Siguientes mediciones** (todavía sin porcentaje útil):

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0 con vigencia acreditada en el árbol actual; las 52 de Interior_c conservan evidencia histórica | Volver a correr los bancos de cada función validada sobre el árbol actual, comprobar que sus dependencias no cambiaron desde la evidencia y registrarlo en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | No medido | Auditar las llamadas directas, indirectas y transitivas de cada función reconstruida que aún saltan al EXE y contar las que ya no dependen de ninguna. |

Las cuatro extensiones y adaptaciones NOTSA están en [hooks_notsa.csv](hooks_notsa.csv) y quedan fuera del progreso: tres hooks de modo ventana y Win::NOTSA_WinMain. Este último conserva validated=0 y su hook en 0x748710; su reclasificación no añade una validación. [Alcance NOTSA](HooksNOTSA.md).

El catálogo contiene 8.445 entradas, no un inventario demostrado de todas las funciones del EXE. Hay 290 entradas pendientes y 10 clases incompletas. De las entradas registradas, 0 tienen `locked=1` y 8.965 tienen `locked=0`: estas últimas son **desbloqueadas**, no necesariamente validadas, activas o independientes. No se conoce todavía el total de funciones que usan el original. Tampoco puede calcularse cuántos hooks originales se han eliminado restando los locks.

`reversed`, `validated`, vigencia de validación, `locked` y dependencia del original son dimensiones distintas. `validated=1` conserva que la función pasó una evidencia aprobada; la vigencia se calcula aparte con hashes y revisión de dependencias. Un cuerpo C++ puede seguir pendiente de validación; una función validada puede llamar a otras implementaciones originales. El [registro separado de validaciones](../reversing/ValidationRegistry/README.md) vincula 8.445 entradas con evidencia histórica aprobada; ninguna tiene vigencia actual acreditada. El [inventario completo](ValidationInventory.md) clasifica las 8.445 entradas reconstruidas. La [auditoría de cobertura](BinaryCoverageAudit.md) identifica 924 destinos originales explícitos sin nombre catalogado, 499 con aristas desde funciones catalogadas y 9.328 entradas binarias sin nombre del catálogo. Son conjuntos solapados que incluyen bibliotecas, helpers y código generado; no representan un total exacto de funciones de juego faltantes. El panel calcula la vigencia mediante evidencia, hashes y revisión de dependencias; no la deduce del flag de reconstrucción.

## Integridad del catálogo

Además del generador del panel, la rama mantiene una auditoría bidireccional entre fuente, registro global y `hooks.csv`:

```powershell
python contrib/audit_hook_catalog.py --strict
```

En el corte actual las seis comprobaciones críticas están en cero, y la opción `--runtime` no encuentra diferencias entre `hooks.csv` y el `runtime-hooks.csv` del juego: hooks activos fuera del CSV, registros indirectos ausentes, entradas del CSV sin scope fuente, llamadas activas con instalaciones propias sin owner catalogado, owners catalogados cuyo registro nunca se invoca y llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base. Los owners inactivos se reportan aparte y no alteran el denominador mientras no formen parte del registro global.

## Orden de trabajo y criterios de cierre

1. **Mantener completa la reconstrucción catalogada.** Las 52 entradas de `Interior_c` ya tienen implementación y aprobación final en sus fixtures. El cierre de su repetición final se documenta en [InteriorValidation.md](InteriorValidation.md); la validación de dependencias y las ramas no cubiertas siguen siendo trabajos separados.
2. **Conmutación de los seis locks acreditada en el alcance probado.** Se retiraron los bloqueos de los tres hooks de audio y de los tres del bucle/render tras pruebas de mezclas, transiciones y render real; se corrigió el contrato ECX del helper de motor y el parche duplicado de Render2dStuff. La [evidencia final](../reversing/HookUnlockFinal/README.md) separa los desbloqueos efectivos de los indicadores históricos obsoletos. La campaña completa de Alt-Tab/minimizar/reactivar con mensajes nativos y todos los controladores sigue fuera de lo acreditado; no se presenta como lock pendiente.
3. **Mantener el cierre de los 771/771 grupos catalogados.** El cierre de sus entradas está acreditado históricamente; existen clases y métodos fuera del catálogo. Ampliar el inventario desde la auditoría binaria antes de atribuir completitud a todas las clases del juego.
4. **Medir la independencia del EXE.** Crear un inventario por función, identidad inequívoca, versión de fuentes y configuración: reconstrucción, validación y evidencia, bloqueo, uso directo del original y dependencias transitivas. Resolver alias, virtuales, callbacks y trampolines; conservar como desconocidas las llamadas sin resolver. Informar por separado entradas catalogadas y funciones únicas, así como accesos a datos o servicios del host.
5. **Consolidar la suite reproducible de equivalencia.** Para original y candidato, usar el mismo input y estados independientes; comparar retornos, estructuras, efectos secundarios y llamadas relevantes. Registrar tolerancias, diferencias previstas, dependencias controladas, hashes y restauración. No sumar repeticiones como funciones nuevas validadas. Adaptar progresivamente los bancos a la misma compilación vigente; las sondas archivadas solo prueban sus versiones.
6. **Validar el juego integrado.** Tras completar las clases, ejecutar regresiones de nueva partida, guardar/cargar con partidas desechables, misiones, vehículos, muerte/arresto, interiores, clima, nivel de búsqueda, combate, streaming, audio y cinemáticas, además de sesiones prolongadas. Registrar escenarios, duración y configuración; las pruebas finitas no garantizan estabilidad general.
7. **Eliminar dependencias originales por subsistema.** Armas, clima, vehículos, cámara, peatones/tareas, scripts y streaming son candidatos para hitos `standalone`; su independencia aún no está auditada. Exigir cierre de llamadas originales directas y transitivas, resolución de llamadas indirectas y regresión del subsistema. Distinguir independencia del código original de la dependencia restante de datos, inicialización y servicios del host. No basta con que todos sus hooks estén habilitados.
8. **Alcanzar “no original code execution”.** Demostrar que la lógica reconstruida no ejecuta implementaciones del juego alojadas en el EXE original. Combinar auditoría estática y trazas de escenarios; la ausencia de llamadas en una sesión no prueba que no existan rutas pendientes. El oráculo original puede conservarse separado para las comparaciones.

## Hitos

**ReSA 95%** puede utilizarse como hito del catálogo de funciones reconstruidas, siempre acompañado por las otras dimensiones y sus límites.

**ReSA — Zero Original Function Dependency** será el hito de eliminación de dependencias de funciones originales de la lógica del juego, con alcance e inventario auditados. No se declara alcanzado por tener 100% de funciones registradas ni cero locks.

Después se evaluará y construirá **un ejecutable propio**: punto de entrada, inicialización y cierre, memoria y globales, carga de recursos, plataforma, render, audio y demás servicios actualmente aportados por el host. Independencia de funciones no implica que esa infraestructura ya exista. Los recursos del juego y las bibliotecas externas son dependencias separadas; no se promete eliminarlas mediante este hito.

## Alcance autorizado ahora

El catálogo principal está cerrado históricamente: 8.445/8.445 entradas y 771/771 grupos, cero pendientes y cero locks. La conmutación de los seis últimos locks se acredita en su [banco final](../reversing/HookUnlockFinal/README.md); no quedan los 85 bloqueos citados en el alcance anterior. La cobertura fuera del catálogo y la vigencia de dependencias siguen pendientes de investigación. Los resultados anteriores de [CConversationForPed](../reversing/ConversationForPed/README.md) y [CPedToPlayerConversations](../reversing/PedToPlayerConversations/README.md) conservan su alcance histórico.

Mantener un solo `PROBAR.cmd`, reutilizar `reversing` y realizar pruebas locales con plazo y restauración exclusivamente en `E:\JUEGOS\GTA San Andreas - CVehicle Test`. Conservar la instalación personal y las partidas. La autorización actual incluye actualizar los Markdown y publicar los cambios en Git; no autoriza alterar la instalación personal ni apagar el equipo. Las configuraciones y los perfiles históricos conservan su fecha y no sustituyen una campaña de integración vigente.

Estado de la migracion a C++ al 16/10/2026: 821 funciones fuente convertidas y revalidadas (2 más, convertidas, esperan la corrida aprobada de su banco en la regresión final: [estudio del lote](../reversing/CppTwelveLevel6Study/README.md)). Quedan 0 apariciones de ensamblador y 0 declaraciones naked en 0 archivos de source/game_sa; son conteos lexicos, no funciones. Excepcion documentada: source/app/platform/win/WindowedMode.cpp conserva __asm (extension NOTSA, fuera de source/game_sa). Lote final IntegerMinimal2: 8704 pares compartidos, cero diferencias, cero resultados nulos y restauracion verificada. Trabajo detenido por orden del usuario tras este lote. Lista completa: [archivos pendientes](../reversing/CppOnlyMigration/FILES.md); evidencia y alcance: [migracion C++](../reversing/CppOnlyMigration/README.md).
