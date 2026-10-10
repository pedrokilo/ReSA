<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ReSA: reconstrucción, validación e independencia

[Inicio](../README.md) · [Indicadores del catálogo](ReversedClasses.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas](../reversing/README.md)

## Objetivo

Recrear GTA San Andreas PC de forma funcionalmente equivalente mediante código fuente reconstruido, reduciendo hasta eliminar la dependencia de funciones originales del ejecutable. Completar `ReversedClasses.md` es un paso intermedio: el objetivo es una reconstrucción completa, validada y ejecutable, seguida de un ejecutable propio.

El contador técnico decisivo será **cuántas funciones siguen necesitando código original**, incluidas sus dependencias transitivas. El 100,00% registrado como reconstruido no representa el porcentaje recorrido hacia un GTA SA independiente.

## Cuatro dimensiones de progreso

Corte del 8 de octubre de 2026. Clases y reconstrucciones se calculan sobre `hooks.csv`; la validación procede del registro parcial de evidencia y versiones. La independencia aún no tiene medición y no se rellena con estimaciones.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas en el catálogo | `████████████████████` | 771/771 · 100,00% |
| Clases validadas de las completas | `████████████████████` | 771/771 · 100,00% |
| Funciones registradas como reconstruidas | `████████████████████` | 8.445/8.445 · 100,00% |
| Funciones con evidencia aprobada (`validated=1`) de las reconstruidas | `████████████████████` | **8.445/8.445 · 100,00%** |

**Siguientes mediciones** (todavía sin porcentaje útil):

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0 con vigencia acreditada en el árbol actual; las 52 de Interior_c conservan evidencia histórica | Volver a correr los bancos de cada función validada sobre el árbol actual, comprobar que sus dependencias no cambiaron desde la evidencia y registrarlo en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | No medido | Auditar las llamadas directas, indirectas y transitivas de cada función reconstruida que aún saltan al EXE y contar las que ya no dependen de ninguna. |

Los extras NOTSA sin equivalente en el EXE (modo ventana) están en [hooks_notsa.csv](hooks_notsa.csv) y quedan fuera de la cuenta.

El catálogo contiene 8.446 entradas, no un inventario demostrado de todas las funciones del EXE. Hay 0 entradas pendientes y 0 clases incompletas. De las entradas registradas, 0 tienen `locked=1` y 8.445 tienen `locked=0`: estas últimas son **desbloqueadas**, no necesariamente validadas, activas o independientes. No se conoce todavía el total de funciones que usan el original. Tampoco puede calcularse cuántos hooks originales se han eliminado restando los locks.

`reversed`, `validated`, vigencia de validación, `locked` y dependencia del original son dimensiones distintas. `validated=1` conserva que la función pasó una evidencia aprobada; la vigencia se calcula aparte con hashes y revisión de dependencias. Un cuerpo C++ puede seguir pendiente de validación; una función validada puede llamar a otras implementaciones originales. El [registro separado de validaciones](../reversing/ValidationRegistry/README.md) vincula 6.736 funciones con evidencia aprobada; 52 tienen vigencia acreditada para sus fixtures en el árbol actual y 6.684 conservan evidencia histórica por revisar. El [inventario completo](ValidationInventory.md) clasifica las 8.446 entradas reconstruidas. El panel calcula la vigencia mediante evidencia, hashes y revisión de dependencias; no la deduce del flag de reconstrucción.

## Integridad del catálogo

Además del generador del panel, la rama mantiene una auditoría bidireccional entre fuente, registro global y `hooks.csv`:

```powershell
python contrib/audit_hook_catalog.py --strict
```

En el corte actual las seis comprobaciones críticas están en cero, y la opción `--runtime` no encuentra diferencias entre `hooks.csv` y el `runtime-hooks.csv` del juego: hooks activos fuera del CSV, registros indirectos ausentes, entradas del CSV sin scope fuente, llamadas activas con instalaciones propias sin owner catalogado, owners catalogados cuyo registro nunca se invoca y llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base. Los owners inactivos se reportan aparte y no alteran el denominador mientras no formen parte del registro global.

## Orden de trabajo y criterios de cierre

1. **Mantener completa la reconstrucción catalogada.** Las 52 entradas de `Interior_c` ya tienen implementación y aprobación final en sus fixtures. El cierre de su repetición final se documenta en [InteriorValidation.md](InteriorValidation.md); la validación de dependencias y las ramas no cubiertas siguen siendo trabajos separados.
2. **Conmutación de los seis locks acreditada en el alcance probado.** Se retiraron los bloqueos de los tres hooks de audio y de los tres del bucle/render tras pruebas de mezclas, transiciones y render real; se corrigió el contrato ECX del helper de motor y el parche duplicado de Render2dStuff. La [evidencia final](../reversing/HookUnlockFinal/README.md) separa los desbloqueos efectivos de los indicadores históricos obsoletos. La campaña completa de Alt-Tab/minimizar/reactivar con mensajes nativos y todos los controladores sigue fuera de lo acreditado; no se presenta como lock pendiente.
3. **Alcanzar 772/772 clases completas.** Es un resultado del cierre de funciones registradas, no una tercera carga de trabajo independiente ni una certificación de todo el juego.
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

La petición de completar `CConversationForPed` queda implementada: 2/2, con 4.702 pares exactos original/candidato y cierre natural. Binario normal instalado en la copia de pruebas tras tres arranques de tres segundos, ambos hooks comprobados y cierres con código 0; partidas y configuración conservadas. [Alcance y evidencia](../reversing/ConversationForPed/README.md). La entrega anterior de `CPedToPlayerConversations` conserva su 3/3 y sus 6.266 pares: [alcance, entrega e incidencia de audio](../reversing/PedToPlayerConversations/README.md). Permanecen aplazados CCollision 59, CFileMgr 13, CTaskComplexGangLeader 10 y Game 3: **85 bloqueos**. No aplicar `reversing/UnlockBatch2/deferred-filemgr-game.patch` sin petición expresa.

Mantener un solo `PROBAR.cmd`, reutilizar `reversing` y realizar pruebas automáticas locales con plazo y restauración exclusivamente en `D:\JUEGOS\GTA San Andreas - CVehicle Test`. Conservar instalación personal y partidas; para uso normal, `FastLoader=false` y `SaveGameToLoad=-2`. No forzar la ranura 2 incompatible. Sin commit, push ni apagado.
