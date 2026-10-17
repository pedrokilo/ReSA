<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FindCoverPointsBehindBox: banco preparado

2026-10-08. No compilado ni ejecutado. Identidad `CFormation::FindCoverPointsBehindBox`, `0x00699FF0`.

256 pares con rectángulos asimétricos, cuadrantes del objetivo, capacidad de salida 0/23/24, altura 7 exacto y distancia de corte 0/100. Compara toda la estructura CPointList y verifica bytes exactos de ambas entradas durante conmutación. Restauración efectiva de ambas entradas y flags/candados de todos los hooks.

El banco escribe un registro antes de cada llamada para conservar la identidad del caso si el candidato accede fuera de corners por `(i-2)%4`. No corregir antes de preservar el primer fallo. Después de corregir, ampliar con distancias de corte exactas, flotantes adyacentes a altura7, rotaciones y datos degenerados; el corpus actual es de reproducción y no una cobertura final suficiente.

Colaboradores Matrix/Vector/PointList no están sustituidos en este banco: cualquier replay requiere revisar sus fuentes y añadirlas al manifiesto de dependencias. No afirmar independencia integral de esos colaboradores. REA nuevo y ejecución son coordinados por root.
