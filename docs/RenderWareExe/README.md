# RenderWare y D3D en el EXE: mapa de las 1,889 funciones

[Lista completa (CSV)](funciones.csv) · [Pendientes por clase](pendientes-por-clase.md) · [Mapa del código del juego](../JuegoExe/README.md)

Región **0x750000–0x81FFFF**: 1,889 funciones, 600,971 bytes. Es RenderWare 3.7 y los envoltorios D3D9 enlazados estáticamente. Generado por `contrib/rw_function_map.py` (mismos datos y cotas que `contrib/exe_coverage.py`).

Estas funciones **no cuentan** en las cifras de reconstrucción del juego: para un ejecutable independiente se sustituyen enlazando una implementación equivalente (por ejemplo librw) y reconstruyendo solo los pipelines propios de San Andreas. Este mapa sirve para saber qué hay y por dónde empezar si se quiere reconstruir.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| A · Falta: llamada desde código ya reemplazado | 213 | 11.3% | 34,098 | 5.7% |
| B · Falta: método virtual / por tabla | 0 | 0.0% | 0 | 0.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 1,070 | 56.6% | 416,110 | 69.2% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 367 | 19.4% | 99,468 | 16.6% |
| Envoltura pendiente (la DLL aún llama al EXE) | 178 | 9.4% | 49,757 | 8.3% |
| Reemplazada por C++ | 61 | 3.2% | 1,538 | 0.3% |

Clase/módulo identificado por nombre: 396 de 1,889 funciones; el resto se estima por vecindad (columna `confianza`).

## Por bloque de direcciones (64 KB)

| Bloque | Funciones | Faltan | Bytes que faltan | Clases/módulos con más faltantes |
| --- | ---: | ---: | ---: | ---: |
| 0x750000 | 77 | 39 | 10,161 | RpWorld (39) |
| 0x760000 | 238 | 222 | 37,628 | RpWorld (222) |
| 0x770000 | 71 | 71 | 59,186 | RpWorld (71) |
| 0x780000 | 241 | 241 | 58,481 | RpWorld (241) |
| 0x790000 | 106 | 106 | 55,603 | RpWorld (106) |
| 0x7A0000 | 130 | 130 | 61,260 | RpWorld (130) |
| 0x7B0000 | 160 | 160 | 55,516 | RpWorld (160) |
| 0x7C0000 | 120 | 91 | 26,938 | RpWorld (33), RpSkin (32), RpUVAnim (9) |
| 0x7D0000 | 134 | 134 | 51,839 | RtDict (134) |
| 0x7E0000 | 115 | 100 | 46,613 | RtDict (65), RwPlCore (19), RwCore (16) |
| 0x7F0000 | 208 | 149 | 38,404 | RwCore (126), RwPlCore (23) |
| 0x800000 | 191 | 114 | 26,270 | RwCore (106), RwPlCore (8) |
| 0x810000 | 98 | 93 | 21,777 | RpMatFX (86), RwCore (6), RpWorld (1) |

## Por clase o módulo

Ordenado por funciones que faltan.

| Clase / módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan |
| --- | ---: | ---: | ---: | ---: |
| RpWorld | 1059 | 11 | 1003 | 348,760 |
| RwCore | 348 | 27 | 254 | 64,683 |
| RtDict | 215 | 2 | 205 | 92,633 |
| RpMatFX | 91 | 0 | 86 | 19,860 |
| RwPlCore | 105 | 14 | 50 | 10,056 |
| RpSkin | 36 | 1 | 32 | 6,462 |
| RpUVAnim | 16 | 2 | 9 | 1,090 |
| RtAnim | 12 | 3 | 7 | 3,501 |
| RpHAnim | 7 | 1 | 4 | 2,631 |
