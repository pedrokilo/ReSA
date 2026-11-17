<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 996 grupos / 12438 funciones reconstruidas; 11156 con evidencia histórica y 1282 pendientes de validación en 142 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan de reconstrucción de RenderWare y D3D por partes y lotes

[Mapa de la región](README.md) · [Pendientes por clase](pendientes-por-clase.md) · [Lista de funciones por lote (CSV)](plan-lotes.csv)

Región **0x750000–0x81FFFF**: 0 funciones por reconstruir (0 bytes) repartidas en **0 lotes** y **0 partes**. Generado por `contrib/rw_plan.py` a partir de `funciones.csv`.

Reglas: lotes de hasta 32 funciones y unos 5 KB; una función de 3 KB o más va sola («grande»); las funciones de un lote son contiguas dentro de su clase o módulo; seis lotes por parte. Los lotes se numeran desde 240A para continuar el plan del código del juego (191A–239A).

Orden de trabajo: primero el SDK de RenderWare (RwPlCore, RwCore, RpWorld y los plugins), después el controlador D3D9 y al final las bibliotecas de terceros (libpng + zlib y D3DX9). Estas últimas no son código de Rockstar ni de RenderWare: se sustituyen enlazando la biblioteca original; están en el plan por completitud y van al final.

Aviso: la clase o módulo de cada función es exacta solo si la confianza lo dice; en el resto es una estimación por vecindad (ver README).

## Partes

| Parte | Lotes | Funciones | Bytes | Clases / módulos |
| --- | ---: | ---: | ---: | ---: |

## Por grupo

| Grupo | Lotes | Funciones | Bytes |
| --- | ---: | ---: | ---: |

