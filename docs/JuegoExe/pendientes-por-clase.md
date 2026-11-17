# Funciones pendientes del EXE, por clase

Generado de `docs/JuegoExe/funciones.csv` (17/11/2026). Total: **3** funciones en 2 clases/módulos (rango 0x401000–0x74FFFF): 2 sin reemplazar (A–D) + 1 envolturas pendientes. Ordenado de menor a mayor por tamaño (bytes pendientes de la clase) y, a igualdad, por número de funciones; dentro de cada clase, de menor a mayor tamaño de función. Se resume en la [sección 11 de ReversedClasses](../ReversedClasses.md#juego).

Estados: **A** llamada desde código ya en C++ · **B** método virtual · **C** solo la llaman funciones pendientes · **D** sin llamadas ni vtable · **envoltura** ya tiene hook pero llama al EXE.

Aviso: la clase se atribuye por cercanía de direcciones y puede ser errónea cuando la confianza es baja; un nombre de catálogo sí es fiable.

## Índice

| Clase | Funciones | Bytes |
|---|---:|---:|
| CAEUserRadioTrackManager | 1 | 85 |
| CPathFind | 2 | 111 |
| **TOTAL (2 clases / módulos)** | **3** | **196** |

## CAEUserRadioTrackManager (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F39C0 | 85 | envoltura |  | 3 | 3 |  | alta |

## CPathFind (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004541E0 | 30 | C |  | 1 | 0 |  | baja |
| 0x00453F40 | 81 | D |  | 0 | 0 |  | baja |
