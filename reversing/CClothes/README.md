<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CClothes

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**11/11 entradas registradas**, habilitadas y sin bloqueos. Cierre: 23 de septiembre de 2026.

El switch existente coincide con el original. Se habilitó después de validar todos los valores del enum, entradas negativas y valores fuera del intervalo.

**4,096 pares original/candidato** aprobados dentro de GTA, con hooks finales habilitados desde el arranque. Funciones contrastadas: GetTextureDependency. Los otros métodos de la clase no se volvieron a validar.

[Resumen verificable](validation-summary.json) · [Banco compartido, cobertura y límites](../ClothesScriptedSprint/README.md)

```powershell
python reversing/run.py --case clothes-scripted-sprint
```

También disponible desde el único PROBAR.cmd. Ejecución automática con restauración de binarios y comprobación de partidas. No certifica estabilidad general; la campaña integral sigue aplazada hasta completar todas las clases. Sin commit ni push.
