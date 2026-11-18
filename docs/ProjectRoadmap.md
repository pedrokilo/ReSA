<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11732 con evidencia histórica y 709 pendientes de validación en 30 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ReSA: reconstrucción, validación e independencia

[Inicio](../README.md) · [Progreso](ReversedClasses.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas](../reversing/README.md) · [Ruta hacia el EXE independiente](RutaExeIndependiente.md)

## Objetivo

Recrear GTA San Andreas PC de forma funcionalmente equivalente mediante código fuente reconstruido en C++, reduciendo hasta eliminar la dependencia de funciones originales del ejecutable, y después construir un ejecutable propio. Las cifras actuales no se repiten aquí: viven en el [informe de progreso](ReversedClasses.md), que se regenera desde `hooks.csv`.

## Qué mide cada cosa

| Medida | Qué significa | Dónde se ve |
| --- | --- | --- |
| **Reconstruida** | La función existe en C++ puro, tiene su hook y compila (`reversed=1`) | [Progreso](ReversedClasses.md) |
| **Validada** | Se comparó con el EXE dentro del juego (≥16.384 pares sin diferencias, oráculo independiente, prueba negativa de dos rondas) y tiene evidencia registrada (`validated=1`) | [Progreso](ReversedClasses.md) · [registro](../reversing/ValidationRegistry/README.md) |
| **Vigente** | La evidencia sigue valiendo para el árbol actual (fuentes y dependencias sin cambios desde la prueba) | [Inventario](ValidationInventory.md) |
| **Independiente** | La función ya no ejecuta código original del EXE, ni siquiera de forma transitiva | Sin medir todavía |

Se miden por separado dos zonas del EXE: el **código del juego** (0x401000–0x74FFFF) y **RenderWare y D3D** (0x750000–0x81FFFF). El CRT de MSVC queda fuera del objetivo.

## Fases

| # | Fase | Estado |
| ---: | --- | --- |
| 1 | Reconstruir el código del juego en C++ puro | Hecha (quedan 3 funciones y las excluidas documentadas) |
| 2 | Reconstruir el SDK de RenderWare | Hecha en lo reconstruible; libpng, zlib y D3DX9 se enlazan, no se reconstruyen |
| 3 | **Validar el código del juego** por oleadas de lotes | **En curso** |
| 4 | Validar RenderWare (dispositivo D3D, flujos y listas libres falsos) | Por planificar |
| 5 | Pase final: repetir los bancos antiguos cuyos colaboradores ahora tienen hook, y revalidar contra el árbol actual | Pendiente |
| 6 | Validar el juego integrado: nueva partida, guardado y carga con partidas desechables, misiones, vehículos, muerte y arresto, interiores, clima, combate, streaming, audio y cinemáticas | Pendiente |
| 7 | Medir y eliminar dependencias originales por subsistema (llamadas directas, indirectas y transitivas) | Pendiente |
| 8 | Sustituir RenderWare, D3D y CRT por bibliotecas y construir el **ejecutable propio**: punto de entrada, memoria y globales, carga de recursos, plataforma, render y audio | Después |

## Hitos

- **Catálogo reconstruido:** todo lo reconstruible está en C++ con su hook. No equivale a independencia ni a validación.
- **ReSA — Zero Original Function Dependency:** la lógica del juego deja de ejecutar funciones del EXE original, con inventario auditado. No se declara por tener todas las funciones reconstruidas.
- **Ejecutable propio:** corre sin `gta_sa.exe`.

## Integridad del catálogo

La auditoría bidireccional entre fuente, registro global y `hooks.csv` debe terminar en OK:

```powershell
python contrib/audit_hook_catalog.py --strict
```

## Reglas de trabajo

- Todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)).
- Un solo `PROBAR.cmd`; las pruebas locales se hacen en la copia `E:\JUEGOS\GTA San Andreas - CVehicle Test`, conservando la instalación personal y las partidas.
- Lo anterior a esta versión (planes y estados de septiembre y octubre de 2026) está en [archivo/](archivo/README.md).
