# PGTASA: reconstrucción, validación e independencia

[Inicio](../README.md) · [Indicadores del catálogo](ReversedClasses.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas](../reversing/README.md)

## Objetivo

Recrear GTA San Andreas PC de forma funcionalmente equivalente mediante código fuente reconstruido, reduciendo hasta eliminar la dependencia de funciones originales del ejecutable. Completar `ReversedClasses.md` es un paso intermedio: el objetivo es una reconstrucción completa, validada y ejecutable, seguida de un ejecutable propio.

El contador técnico decisivo será **cuántas funciones siguen necesitando código original**, incluidas sus dependencias transitivas. El 93,09% registrado como reconstruido no representa el porcentaje recorrido hacia un GTA SA independiente.

## Cuatro dimensiones de progreso

Corte del 30 de septiembre de 2026. Clases y reconstrucciones se calculan sobre `hooks.csv`; la validación procede del registro parcial de evidencia y versiones. La independencia aún no tiene medición y no se rellena con estimaciones.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas en el catálogo | `█████████████████░░░` | 666/771 · 86,38% |
| Clases validadas de las completas | `█████████████░░░░░░░` | 428/666 · 64,26% |
| Funciones registradas como reconstruidas | `███████████████████░` | 7.857/8.440 · 93,09% |
| Funciones con evidencia aprobada (`validated=1`) de las reconstruidas | `█████░░░░░░░░░░░░░░░` | **1.901/7.857 · 24,19%** |

**Siguientes mediciones** (todavía sin porcentaje útil):

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | Ninguna validada confirmada como vigente | Volver a correr los bancos de cada función validada sobre el árbol actual, comprobar que sus dependencias no cambiaron desde la evidencia y registrarlo en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | No medido | Auditar las llamadas directas, indirectas y transitivas de cada función reconstruida que aún saltan al EXE y contar las que ya no dependen de ninguna. |

Los extras NOTSA sin equivalente en el EXE (modo ventana) están en [hooks_notsa.csv](hooks_notsa.csv) y quedan fuera de la cuenta.

El catálogo contiene 8.440 entradas, no un inventario demostrado de todas las funciones del EXE. Hay 583 entradas pendientes y 105 clases incompletas. De las entradas registradas, 675 tienen `locked=1` y 7.765 tienen `locked=0`: estas últimas son **desbloqueadas**, no necesariamente validadas, activas o independientes. No se conoce todavía el total de funciones que usan el original. Tampoco puede calcularse cuántos hooks originales se han eliminado restando los locks.

`reversed`, `validated`, vigencia de validación, `locked` y dependencia del original son dimensiones distintas. `validated=1` conserva que la función pasó una evidencia aprobada; la vigencia se calcula aparte con hashes y revisión de dependencias. Un cuerpo C++ puede seguir pendiente de validación; una función validada puede llamar a otras implementaciones originales. Ya existe un [registro separado de validaciones](../reversing/ValidationRegistry/README.md), con 40 funciones vinculadas a evidencia. El constructor de animación por nombre, SetupBones y RpAnimBlendClumpUpdateAnimations obtuvieron acreditación en la versión anterior; los cambios locales de CustomBuilding requieren revisar la huella global de dependencias. Las 40 conservan evidencia histórica o coincidencia de fuentes. El [inventario completo](ValidationInventory.md) clasifica las 7.857 funciones reconstruidas. El panel calcula la acreditación desde evidencia y hashes, sin añadir columnas ni cambiar flags del CSV. Se mantiene el requisito de comparar contra el original dentro del motor antes de marcar nuevas entradas `reversed=1` o retirar locks.

## Integridad del catálogo

Además del generador del panel, la rama mantiene una auditoría bidireccional entre fuente, registro global y `hooks.csv`:

```powershell
python contrib/audit_hook_catalog.py --strict
```

En el corte actual las seis comprobaciones críticas están en cero, y la opción `--runtime` no encuentra diferencias entre `hooks.csv` y el `runtime-hooks.csv` del juego: hooks activos fuera del CSV, registros indirectos ausentes, entradas del CSV sin scope fuente, llamadas activas con instalaciones propias sin owner catalogado, owners catalogados cuyo registro nunca se invoca y llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base. Los owners inactivos se reportan aparte y no alteran el denominador mientras no formen parte del registro global.

## Orden de trabajo y criterios de cierre

1. **Completar las 583 entradas pendientes.** Reconstruir contratos, ABI y efectos, y contrastar con el original antes de actualizar progreso. Registrar por separado mejoras deliberadas y diferencias esperadas.
2. **Reducir los 677 locks a cero.** Retirar cada bloqueo tras demostrar que la implementación sustituye al original en los escenarios documentados y supera las regresiones pertinentes. Quitar un flag por sí solo no acredita estabilidad.
3. **Alcanzar 772/772 clases completas.** Es un resultado del cierre de funciones registradas, no una tercera carga de trabajo independiente ni una certificación de todo el juego.
4. **Medir la independencia del EXE.** Crear un inventario por función, identidad inequívoca, versión de fuentes y configuración: reconstrucción, validación y evidencia, bloqueo, uso directo del original y dependencias transitivas. Resolver alias, virtuales, callbacks y trampolines; conservar como desconocidas las llamadas sin resolver. Informar por separado entradas catalogadas y funciones únicas, así como accesos a datos o servicios del host.
5. **Consolidar la suite reproducible de equivalencia.** Para original y candidato, usar el mismo input y estados independientes; comparar retornos, estructuras, efectos secundarios y llamadas relevantes. Registrar tolerancias, diferencias previstas, dependencias controladas, hashes y restauración. No sumar repeticiones como funciones nuevas validadas. Adaptar progresivamente los bancos a la misma compilación vigente; las sondas archivadas solo prueban sus versiones.
6. **Validar el juego integrado.** Tras completar las clases, ejecutar regresiones de nueva partida, guardar/cargar con partidas desechables, misiones, vehículos, muerte/arresto, interiores, clima, nivel de búsqueda, combate, streaming, audio y cinemáticas, además de sesiones prolongadas. Registrar escenarios, duración y configuración; las pruebas finitas no garantizan estabilidad general.
7. **Eliminar dependencias originales por subsistema.** Armas, clima, vehículos, cámara, peatones/tareas, scripts y streaming son candidatos para hitos `standalone`; su independencia aún no está auditada. Exigir cierre de llamadas originales directas y transitivas, resolución de llamadas indirectas y regresión del subsistema. Distinguir independencia del código original de la dependencia restante de datos, inicialización y servicios del host. No basta con que todos sus hooks estén habilitados.
8. **Alcanzar “no original code execution”.** Demostrar que la lógica reconstruida no ejecuta implementaciones del juego alojadas en el EXE original. Combinar auditoría estática y trazas de escenarios; la ausencia de llamadas en una sesión no prueba que no existan rutas pendientes. El oráculo original puede conservarse separado para las comparaciones.

## Hitos

**PGTASA 95%** puede utilizarse como hito del catálogo de funciones reconstruidas, siempre acompañado por las otras dimensiones y sus límites.

**PGTASA — Zero Original Function Dependency** será el hito de eliminación de dependencias de funciones originales de la lógica del juego, con alcance e inventario auditados. No se declara alcanzado por tener 100% de funciones registradas ni cero locks.

Después se evaluará y construirá **un ejecutable propio**: punto de entrada, inicialización y cierre, memoria y globales, carga de recursos, plataforma, render, audio y demás servicios actualmente aportados por el host. Independencia de funciones no implica que esa infraestructura ya exista. Los recursos del juego y las bibliotecas externas son dependencias separadas; no se promete eliminarlas mediante este hito.

## Alcance autorizado ahora

La petición de completar `CConversationForPed` queda implementada: 2/2, con 4.702 pares exactos original/candidato y cierre natural. Binario normal instalado en la copia de pruebas tras tres arranques de tres segundos, ambos hooks comprobados y cierres con código 0; partidas y configuración conservadas. [Alcance y evidencia](../reversing/ConversationForPed/README.md). La entrega anterior de `CPedToPlayerConversations` conserva su 3/3 y sus 6.266 pares: [alcance, entrega e incidencia de audio](../reversing/PedToPlayerConversations/README.md). Permanecen aplazados CCollision 59, CFileMgr 13, CTaskComplexGangLeader 10 y Game 3: **85 bloqueos**. No aplicar `reversing/UnlockBatch2/deferred-filemgr-game.patch` sin petición expresa.

Mantener un solo `PROBAR.cmd`, reutilizar `reversing` y realizar pruebas automáticas locales con plazo y restauración exclusivamente en `D:\JUEGOS\GTA San Andreas - CVehicle Test`. Conservar instalación personal y partidas; para uso normal, `FastLoader=false` y `SaveGameToLoad=-2`. No forzar la ranura 2 incompatible. Sin commit, push ni apagado.
