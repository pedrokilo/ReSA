# RenderWare y D3D en el EXE: mapa de las 1,889 funciones

[Lista completa (CSV)](funciones.csv) · [Pendientes por clase](pendientes-por-clase.md) · [Plan de validación](plan-validacion.md) · [Mapa del código del juego](../JuegoExe/README.md)

Región **0x750000–0x81FFFF**: 1,889 funciones, 600,971 bytes. Es RenderWare 3.7 y los envoltorios D3D9 enlazados estáticamente. Generado por `contrib/rw_function_map.py` (mismos datos y cotas que `contrib/exe_coverage.py`).

Estas funciones **no cuentan** en las cifras de reconstrucción del juego: para un ejecutable independiente se sustituyen enlazando una implementación equivalente (por ejemplo librw) y reconstruyendo solo los pipelines propios de San Andreas. Este mapa sirve para saber qué hay y por dónde empezar si se quiere reconstruir.

**Hallazgo:** gran parte de la región (≈0x765000–0x7C4000, más de 800 funciones y 320 KB) no es RenderWare sino **D3DX9** (compilador de shaders HLSL, preprocesador y ensamblador) enlazado estáticamente; entre 0x7D0000 y 0x7EA000 hay **libpng + zlib**. Ninguno de los dos es código de Rockstar ni de RenderWare: se sustituyen enlazando las bibliotecas originales.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| A · Falta: llamada desde código ya reemplazado | 0 | 0.0% | 0 | 0.0% |
| B · Falta: método virtual / por tabla | 0 | 0.0% | 0 | 0.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 0 | 0.0% | 0 | 0.0% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 0 | 0.0% | 0 | 0.0% |
| No reconstruible en C++ puro (ABI de registros, 3DNow!, fragmento) · documentada | 54 | 2.9% | 9,908 | 1.6% |
| Biblioteca de terceros: se enlaza la original, no se reconstruye | 1,033 | 54.7% | 380,721 | 63.4% |
| Envoltura pendiente (la DLL aún llama al EXE) | 0 | 0.0% | 0 | 0.0% |
| Reemplazada por C++ | 802 | 42.5% | 210,342 | 35.0% |

Clase/módulo identificado por nombre: 782 de 1,889 funciones; el resto se estima por vecindad (columna `confianza`).

## Por bloque de direcciones (64 KB)

| Bloque | Funciones | Faltan | Bytes que faltan | Clases/módulos con más faltantes |
| --- | ---: | ---: | ---: | ---: |
| 0x750000 | 77 | 0 | 0 | — |
| 0x760000 | 238 | 0 | 0 | — |
| 0x770000 | 71 | 0 | 0 | — |
| 0x780000 | 241 | 0 | 0 | — |
| 0x790000 | 106 | 0 | 0 | — |
| 0x7A0000 | 130 | 0 | 0 | — |
| 0x7B0000 | 160 | 0 | 0 | — |
| 0x7C0000 | 120 | 0 | 0 | — |
| 0x7D0000 | 134 | 0 | 0 | — |
| 0x7E0000 | 115 | 0 | 0 | — |
| 0x7F0000 | 208 | 0 | 0 | — |
| 0x800000 | 191 | 0 | 0 | — |
| 0x810000 | 98 | 0 | 0 | — |

## Por clase o módulo

Ordenado por funciones que faltan.

| Clase / módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan |
| --- | ---: | ---: | ---: | ---: |
| Carga retardada de DLL | 5 | 0 | 0 | 0 |
| Controlador D3D9 (RwD3D9) | 118 | 115 | 0 | 0 |
| D3DX9 (compilador de shaders) | 950 | 17 | 0 | 0 |
| QuickTime/QTML (puentes) | 32 | 0 | 0 | 0 |
| RpHAnim | 7 | 7 | 0 | 0 |
| RpMatFX | 58 | 58 | 0 | 0 |
| RpSkin | 37 | 34 | 0 | 0 |
| RpUVAnim | 16 | 16 | 0 | 0 |
| RpWorld | 105 | 105 | 0 | 0 |
| RtAnim | 10 | 9 | 0 | 0 |
| RtBMP | 2 | 2 | 0 | 0 |
| RtDict | 12 | 12 | 0 | 0 |
| RtQuat | 6 | 6 | 0 | 0 |
| RtSlerp | 1 | 1 | 0 | 0 |
| RwCore | 202 | 202 | 0 | 0 |
| RwPlCore | 132 | 132 | 0 | 0 |
| libpng + zlib | 196 | 86 | 0 | 0 |
