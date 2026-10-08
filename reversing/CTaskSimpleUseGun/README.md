<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskSimpleUseGun — ProcessPed

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Validación del 2026-09-22: **3.232 comparaciones aprobadas dentro del motor**, con diferencia numérica máxima cero. CTaskSimpleUseGun pasa de 18/19 a 19/19 funciones registradas al reconstruir `ProcessPed` (`0x62A380`). El banco se centra en esta función, no valida exhaustivamente las otras dieciocho.

La implementación anterior delegaba al original y conservaba un borrador incompleto comentado. Se reconstruyó el flujo de adquisición/cambio de arma, finalización, pérdida de control, bloqueo de brazos, arranque/transición de animaciones, recarga, golpe con el arma, disparos simples/continuos/dobles, ráfagas y límites de apuntado. Se conserva la interpretación con signo de los contadores de ráfaga al compararlos con cero. Se corrigieron condiciones invertidas del borrador. El banco detectó además que el disparo continuo exige estar estrictamente antes del final del bucle: la igualdad con ese límite no dispara.

## Repetir

Desde la raíz del repositorio:

```powershell
.\PROBAR.cmd --case usegun
python reversing/run.py --case usegun
```

El segundo comando termina sin pausa ni supervisión. Se utiliza el runner compartido y solo la copia `D:/JUEGOS/GTA San Andreas - CVehicle Test`, con verificación de hashes, exclusión de ejecuciones concurrentes, respaldo/restauración de ASI/PDB, cierre del proceso iniciado y comprobación de partidas. El caso también está incluido en `--all`; se conserva un solo PROBAR.cmd.

`build_probe.py` instrumenta temporalmente TaskSimpleUseGun.cpp y Game.cpp y restaura ambas fuentes en `finally`. Archiva la sonda, sus símbolos y hashes de fuentes/instrumentación en `artifacts`. Para repetir se usa `_baseline/gta_reversed-usegun-probe.asi`, identificado por `PROBE_HASH`. Recompilar requiere revisar su nuevo hash y volver a validar. El ASI normal se compila posteriormente con las fuentes restauradas; no contiene la sonda.

## Qué se compara

Se alterna el hook y se llama a la dirección original con dos conjuntos independientes de tarea, peatón, datos de jugador, inteligencia, matrices, asociación y jerarquía de animación. El contador exige cero entradas al candidato durante la llamada original y exactamente una durante la candidata.

El banco comprende:

- **64 casos con dependencias reales alcanzadas:** espera por cambio de arma, rechazo de armas cuerpo a cuerpo/lanzables y pérdida de control con cuenta atrás no agotada. No se ejecutan aquí disparos ni transiciones completas de animación.
- **2.592 casos controlados:** combinaciones de las nueve órdenes y 32 configuraciones de control, finalización, postura, bloqueo, transparencia/tiempo de animación, recarga y banderas del arma. Incluye fronteras temporales y contadores de ráfaga 0/1/3/65535.
- **512 casos controlados con objetivos temporales:** peatones vivos/muertos y entidades no peatonales, distintas direcciones y alturas, límites angulares y orientación durante el golpe con el arma.
- **64 casos controlados de cambio del descriptor de arma**, que ejercitan la cancelación de la tarea.

Se comparan retorno, almacenamiento de tarea y peatón, datos de jugador, banderas/tiempos/callback de la asociación y secuencia de llamadas auxiliares. Se normalizan las direcciones de objetos independientes, los bits no usados de las banderas de disparo y el callback por identidad de función: el callback original `0x4D1390` y su reconstrucción tienen direcciones distintas. Los punteros de salida deben pertenecer al conjunto permitido. Los estados discretos son exactos; la tolerancia numérica es 1e-5 y el máximo observado fue 0.

Se comprueba que las matrices, objetivos, inteligencia, jerarquías y descriptor de arma de entrada permanezcan sin cambios. La tabla de arma y el timestep se restauran; el jugador real, sus datos, su inteligencia y su matriz deben coincidir con las copias anteriores al banco. Las partidas también permanecieron sin cambios.

El analizador rechazó registros con contadores falsos, independencia o inmutabilidad fallidas, retornos/estados numéricos diferentes, llamadas adicionales y casos duplicados. Un log incompleto no aprueba.

## Dependencias controladas y límites

En los 3.168 casos controlados se registran pero no se ejecutan los efectos completos de `AimGun`, `SetMoveAnim` y `RemoveStanceAnims`. `StartAnim` se registra y devuelve de forma controlada una asociación sintética o ninguna. `FightStrike` se intercepta y registra sus argumentos sin aplicar daño. En la ejecución final se registraron por versión 619 solicitudes de AimGun, 2.942 de SetMoveAnim, 236 de RemoveStanceAnims, 648 de StartAnim y 30 de FightStrike.

`AbortIK` continúa a su implementación real, con inteligencias temporales sin cadenas IK activas. Las consultas de arma, las operaciones de tiempo/callback sobre asociaciones y la obtención de posiciones de huesos continúan a las dependencias existentes. Las jerarquías sintéticas no tienen nodos de huesos. Las dependencias compartidas por original y candidato no quedan validadas por separado con este banco.

Esto verifica el flujo y los estados de ProcessPed en los casos descritos. **No demuestra una secuencia completa de combate con animaciones reales, impactos, efectos, cadenas IK activas o reproducción de poses.** Tampoco prueba todas las cámaras, armas o interacciones con tareas simultáneas. No hubo revisión visual. Continúan excluidos los hooks Audio y Global/SurfaceInfos_c durante la prueba. El 19/19 es progreso de reconstrucción, no cobertura exhaustiva ni garantía de estabilidad general; la campaña de integración permanece aplazada según lo solicitado.

## Evidencia

Consultar `validation-summary.json` para los hashes y resultados. Log final local: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-usegun/20260922-205940-939686/comparisons.jsonl`. Desensamblado original: `artifacts/ProcessPed.asm`; la función termina en `0x62AF42` y el volcado también incluye instrucciones posteriores. Se conservan los informes fallidos que permitieron corregir el límite del disparo y ajustar la comparación semántica del callback.

El ASI normal quedó instalado únicamente en la copia de pruebas, con respaldo en `_baseline/before-usegun-20260922`. Instalación personal y partidas conservadas. Sin commit ni push.
