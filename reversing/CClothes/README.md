<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
