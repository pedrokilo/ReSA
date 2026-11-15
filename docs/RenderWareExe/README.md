<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/11/2026 (Ciudad de México):** 912 grupos / 10706 funciones reconstruidas; 10706 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RenderWare y D3D en el EXE: mapa de las 1,889 funciones

[Lista completa (CSV)](funciones.csv) · [Pendientes por clase](pendientes-por-clase.md) · [Mapa del código del juego](../JuegoExe/README.md)

Región **0x750000–0x81FFFF**: 1,889 funciones, 600,971 bytes. Es RenderWare 3.7 y los envoltorios D3D9 enlazados estáticamente. Generado por `contrib/rw_function_map.py` (mismos datos y cotas que `contrib/exe_coverage.py`).

Estas funciones **no cuentan** en las cifras de reconstrucción del juego: para un ejecutable independiente se sustituyen enlazando una implementación equivalente (por ejemplo librw) y reconstruyendo solo los pipelines propios de San Andreas. Este mapa sirve para saber qué hay y por dónde empezar si se quiere reconstruir.

**Hallazgo:** gran parte de la región (≈0x765000–0x7C4000, más de 800 funciones y 320 KB) no es RenderWare sino **D3DX9** (compilador de shaders HLSL, preprocesador y ensamblador) enlazado estáticamente; entre 0x7D0000 y 0x7EA000 hay **libpng + zlib**. Ninguno de los dos es código de Rockstar ni de RenderWare: se sustituyen enlazando las bibliotecas originales.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| A · Falta: llamada desde código ya reemplazado | 213 | 11.3% | 34,098 | 5.7% |
| B · Falta: método virtual / por tabla | 0 | 0.0% | 0 | 0.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 1,070 | 56.6% | 416,110 | 69.2% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 367 | 19.4% | 99,468 | 16.6% |
| Envoltura pendiente (la DLL aún llama al EXE) | 178 | 9.4% | 49,757 | 8.3% |
| Reemplazada por C++ | 61 | 3.2% | 1,538 | 0.3% |

Clase/módulo identificado por nombre: 403 de 1,889 funciones; el resto se estima por vecindad (columna `confianza`).

## Por bloque de direcciones (64 KB)

| Bloque | Funciones | Faltan | Bytes que faltan | Clases/módulos con más faltantes |
| --- | ---: | ---: | ---: | ---: |
| 0x750000 | 77 | 39 | 10,161 | RpWorld (39) |
| 0x760000 | 238 | 222 | 37,628 | D3DX9 (compilador de shaders) (155), RpWorld (67) |
| 0x770000 | 71 | 71 | 59,186 | D3DX9 (compilador de shaders) (71) |
| 0x780000 | 241 | 241 | 58,481 | D3DX9 (compilador de shaders) (241) |
| 0x790000 | 106 | 106 | 55,603 | D3DX9 (compilador de shaders) (106) |
| 0x7A0000 | 130 | 130 | 61,260 | D3DX9 (compilador de shaders) (130) |
| 0x7B0000 | 160 | 160 | 55,516 | D3DX9 (compilador de shaders) (160) |
| 0x7C0000 | 120 | 91 | 26,938 | RpSkin (23), RpHAnim (21), D3DX9 (compilador de shaders) (15) |
| 0x7D0000 | 134 | 134 | 51,839 | libpng + zlib (134) |
| 0x7E0000 | 115 | 100 | 46,613 | libpng + zlib (44), RtQuat (20), RwCore (19) |
| 0x7F0000 | 208 | 149 | 38,404 | RwCore (123), RwPlCore (24), RpAnisot (1) |
| 0x800000 | 191 | 114 | 26,270 | RwCore (82), RwPlCore (31), RpMatFX (1) |
| 0x810000 | 98 | 93 | 21,777 | RpMatFX (86), RwCore (6), RwPlCore (1) |

## Por clase o módulo

Ordenado por funciones que faltan.

| Clase / módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan |
| --- | ---: | ---: | ---: | ---: |
| D3DX9 (compilador de shaders) | 878 | 0 | 878 | 323,180 |
| RwCore | 324 | 27 | 230 | 67,628 |
| libpng + zlib | 182 | 0 | 181 | 81,023 |
| RpWorld | 163 | 11 | 107 | 20,781 |
| RpMatFX | 92 | 0 | 87 | 13,629 |
| RwPlCore | 129 | 14 | 74 | 14,172 |
| RpSkin | 27 | 1 | 23 | 3,752 |
| RpHAnim | 24 | 1 | 21 | 8,033 |
| RtQuat | 20 | 0 | 20 | 9,888 |
| RpUVAnim | 22 | 2 | 15 | 2,312 |
| RtAnim | 11 | 3 | 6 | 956 |
| RtDict | 12 | 2 | 3 | 773 |
| RtBMP | 2 | 0 | 2 | 2,588 |
| Controlador D3D9 (RwD3D9) | 1 | 0 | 1 | 6 |
| RpAnisot | 1 | 0 | 1 | 6 |
| RtSlerp | 1 | 0 | 1 | 949 |
