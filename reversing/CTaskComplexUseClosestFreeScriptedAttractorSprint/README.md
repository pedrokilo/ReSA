<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskComplexUseClosestFreeScriptedAttractorSprint

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**4/4 entradas registradas**, habilitadas y sin bloqueos. Cierre: 23 de septiembre de 2026.

Clone crea una tarea nueva con PEDMOVE_SPRINT (7). La copia anterior usaba el constructor de copia de la base, que reiniciaba el movimiento a caminar. GetTaskType devuelve 262.

**4,096 pares original/candidato** aprobados dentro de GTA, con hooks finales habilitados desde el arranque. Funciones contrastadas: Clone, GetTaskType. Los otros métodos de la clase no se volvieron a validar.

[Resumen verificable](validation-summary.json) · [Banco compartido, cobertura y límites](../ClothesScriptedSprint/README.md)

```powershell
python reversing/run.py --case clothes-scripted-sprint
```

También disponible desde el único PROBAR.cmd. Ejecución automática con restauración de binarios y comprobación de partidas. No certifica estabilidad general; la campaña integral sigue aplazada hasta completar todas las clases. Sin commit ni push.
