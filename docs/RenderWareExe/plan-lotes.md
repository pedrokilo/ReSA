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

