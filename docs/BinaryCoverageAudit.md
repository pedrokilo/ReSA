<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cobertura del EXE

Este documento quedó anticuado y se archivó: medía el catálogo contra las entradas de Ghidra sin nombre, un enfoque que sustituyeron los mapas del EXE por región. La cobertura actual está en:

- [Código del juego (0x401000–0x74FFFF)](JuegoExe/README.md): estado por función, funciones que faltan y excluidas.
- [RenderWare y D3D (0x750000–0x81FFFF)](RenderWareExe/README.md): reemplazadas, enlazadas, no reconstruibles y pendientes.
- [Progreso del reversing](ReversedClasses.md): validado, reconstruido sin validar, por zona.

La revisión del 10 de octubre de 2026 se conserva en [archivo/BinaryCoverageAudit-2026-10-10.md](archivo/BinaryCoverageAudit-2026-10-10.md). Los datos de aquella auditoría siguen en `reversing/BinaryCoverageAudit/`.
