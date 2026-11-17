# Cobertura del EXE

Este documento quedó anticuado y se archivó: medía el catálogo contra las entradas de Ghidra sin nombre, un enfoque que sustituyeron los mapas del EXE por región. La cobertura actual está en:

- [Código del juego (0x401000–0x74FFFF)](JuegoExe/README.md): estado por función, funciones que faltan y excluidas.
- [RenderWare y D3D (0x750000–0x81FFFF)](RenderWareExe/README.md): reemplazadas, enlazadas, no reconstruibles y pendientes.
- [Progreso del reversing](ReversedClasses.md): validado, reconstruido sin validar, por zona.

La revisión del 10 de octubre de 2026 se conserva en [archivo/BinaryCoverageAudit-2026-10-10.md](archivo/BinaryCoverageAudit-2026-10-10.md). Los datos de aquella auditoría siguen en `reversing/BinaryCoverageAudit/`.
