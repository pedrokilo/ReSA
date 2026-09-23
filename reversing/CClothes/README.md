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
