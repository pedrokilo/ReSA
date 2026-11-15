<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/11/2026 (Ciudad de México):** 912 grupos / 10706 funciones reconstruidas; 10706 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan de reconstrucción de RenderWare y D3D por partes y lotes

[Mapa de la región](README.md) · [Pendientes por clase](pendientes-por-clase.md) · [Lista de funciones por lote (CSV)](plan-lotes.csv)

Región **0x750000–0x81FFFF**: 1,828 funciones por reconstruir (599,433 bytes) repartidas en **150 lotes** y **25 partes**. Generado por `contrib/rw_plan.py` a partir de `funciones.csv`.

Reglas: lotes de hasta 32 funciones y unos 5 KB; una función de 3 KB o más va sola («grande»); las funciones de un lote son contiguas dentro de su clase o módulo; seis lotes por parte. Los lotes se numeran desde 240A para continuar el plan del código del juego (191A–239A).

Orden de trabajo: primero el SDK de RenderWare (RwPlCore, RwCore, RpWorld y los plugins), después el controlador D3D9 y al final las bibliotecas de terceros (libpng + zlib y D3DX9). Estas últimas no son código de Rockstar ni de RenderWare: se sustituyen enlazando la biblioteca original; están en el plan por completitud y van al final.

Aviso: la clase o módulo de cada función es exacta solo si la confianza lo dice; en el resto es una estimación por vecindad (ver README).

## Partes

| Parte | Lotes | Funciones | Bytes | Clases / módulos |
| --- | ---: | ---: | ---: | ---: |
| Parte 1 | 240A–245A | 132 | 25,206 | RwPlCore, RwCore |
| Parte 2 | 246A–251A | 116 | 27,189 | RwCore |
| Parte 3 | 252A–257A | 86 | 27,932 | RwCore |
| Parte 4 | 258A–263A | 75 | 25,142 | RwCore |
| Parte 5 | 264A–269A | 55 | 21,272 | RwCore, Controlador D3D9 (RwD3D9), RpWorld |
| Parte 6 | 270A–275A | 122 | 24,062 | RpWorld, RpMatFX |
| Parte 7 | 276A–281A | 112 | 20,085 | RpMatFX, RpSkin, RpHAnim |
| Parte 8 | 282A–287A | 49 | 13,269 | RpHAnim, RpUVAnim, RtAnim, RtDict, RtQuat |
| Parte 9 | 288A–293A | 32 | 14,844 | RtQuat, RtSlerp, RtBMP, RpAnisot, libpng + zlib |
| Parte 10 | 294A–299A | 70 | 20,654 | libpng + zlib |
| Parte 11 | 300A–305A | 38 | 23,958 | libpng + zlib |
| Parte 12 | 306A–311A | 52 | 27,227 | libpng + zlib |
| Parte 13 | 312A–317A | 119 | 22,300 | libpng + zlib, D3DX9 (compilador de shaders) |
| Parte 14 | 318A–323A | 88 | 27,648 | D3DX9 (compilador de shaders) |
| Parte 15 | 324A–329A | 27 | 41,724 | D3DX9 (compilador de shaders) |
| Parte 16 | 330A–335A | 192 | 16,749 | D3DX9 (compilador de shaders) |
| Parte 17 | 336A–341A | 40 | 20,093 | D3DX9 (compilador de shaders) |
| Parte 18 | 342A–347A | 15 | 24,736 | D3DX9 (compilador de shaders) |
| Parte 19 | 348A–353A | 63 | 27,000 | D3DX9 (compilador de shaders) |
| Parte 20 | 354A–359A | 36 | 23,890 | D3DX9 (compilador de shaders) |
| Parte 21 | 360A–365A | 40 | 26,192 | D3DX9 (compilador de shaders) |
| Parte 22 | 366A–371A | 60 | 25,687 | D3DX9 (compilador de shaders) |
| Parte 23 | 372A–377A | 54 | 25,256 | D3DX9 (compilador de shaders) |
| Parte 24 | 378A–383A | 116 | 26,234 | D3DX9 (compilador de shaders) |
| Parte 25 | 384A–389A | 39 | 21,084 | D3DX9 (compilador de shaders) |

## Por grupo

| Grupo | Lotes | Funciones | Bytes |
| --- | ---: | ---: | ---: |
| SDK de RenderWare | 52 | 767 | 194,185 |
| Controlador D3D9 | 1 | 1 | 6 |
| Bibliotecas de terceros | 97 | 1,060 | 405,242 |

## Parte 1 · lotes 240A–245A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 240A | RwPlCore | 0x007C8B50–0x007F17E0 | 29 | 4,952 | mediana | 11 | 23/29 |
| 241A | RwPlCore | 0x007F1930–0x007F3130 | 24 | 4,931 | mediana | 3 | 21/24 |
| 242A | RwPlCore | 0x007F3170–0x00801970 | 22 | 4,712 | mediana | 0 | 1/22 |
| 243A | RwPlCore | 0x008019B0–0x00808C10 | 32 | 4,194 | mediana | 21 | 22/32 |
| 244A | RwPlCore | 0x00809160–0x00810040 | 8 | 1,620 | mediana | 6 | 6/8 |
| 245A | RwCore | 0x007ED880–0x007EF550 | 17 | 4,797 | mediana | 0 | 12/17 |

## Parte 2 · lotes 246A–251A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 246A | RwCore | 0x007EF6B0–0x007F3500 | 32 | 4,776 | mediana | 5 | 27/32 |
| 247A | RwCore | 0x007F3530–0x007F4AE0 | 20 | 4,655 | mediana | 2 | 16/20 |
| 248A | RwCore | 0x007F5500–0x007F7100 | 17 | 4,621 | mediana | 6 | 4/17 |
| 249A | RwCore | 0x007F72E0–0x007F8580 | 8 | 4,364 | mediana | 1 | 3/8 |
| 250A | RwCore | 0x007F8640–0x007F9D30 | 7 | 4,932 | mediana | 2 | 5/7 |
| 251A | RwCore | 0x007F9D50–0x007FB2D0 | 32 | 3,841 | mediana | 13 | 27/32 |

## Parte 3 · lotes 252A–257A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 252A | RwCore | 0x007FB480–0x007FCAC0 | 12 | 4,833 | mediana | 4 | 7/12 |
| 253A | RwCore | 0x007FD100–0x007FE3D0 | 19 | 4,452 | mediana | 1 | 1/19 |
| 254A | RwCore | 0x007FE420–0x007FF270 | 10 | 3,928 | mediana | 0 | 0/10 |
| 255A | RwCore | 0x007FF450–0x00802AF0 | 9 | 4,993 | mediana | 3 | 6/9 |
| 256A | RwCore | 0x00802FD0–0x008057B0 | 23 | 4,944 | mediana | 9 | 14/23 |
| 257A | RwCore | 0x00805820–0x00806BE0 | 13 | 4,782 | mediana | 2 | 6/13 |

## Parte 4 · lotes 258A–263A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 258A | RwCore | 0x00806DE0–0x0080AED0 | 23 | 4,994 | mediana | 10 | 11/23 |
| 259A | RwCore | 0x0080AF60–0x0080BEB0 | 9 | 3,961 | mediana | 1 | 1/9 |
| 260A | RwCore | 0x0080BF20–0x0080D370 | 15 | 4,858 | mediana | 5 | 5/15 |
| 261A | RwCore | 0x0080D440–0x0080F570 | 21 | 4,671 | mediana | 3 | 2/21 |
| 262A | RwCore | 0x0080F690–0x00812AC0 | 6 | 3,379 | mediana | 0 | 0/6 |
| 263A | RwCore | 0x00812D40–0x00812D40 | 1 | 3,279 | grande | 0 | 0/1 |

## Parte 5 · lotes 264A–269A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 264A | RwCore | 0x00813A10–0x00816190 | 3 | 3,744 | mediana | 0 | 0/3 |
| 265A | Controlador D3D9 (RwD3D9) | 0x007FAE30–0x007FAE30 | 1 | 6 | pequeña | 0 | 1/1 |
| 266A | RpWorld | 0x00750050–0x00752320 | 19 | 3,898 | mediana | 4 | 12/19 |
| 267A | RpWorld | 0x00752AD0–0x00753B60 | 2 | 4,070 | mediana | 2 | 2/2 |
| 268A | RpWorld | 0x007544E0–0x00755B90 | 9 | 4,781 | mediana | 8 | 8/9 |
| 269A | RpWorld | 0x00755D80–0x00758D70 | 21 | 4,773 | mediana | 8 | 15/21 |

## Parte 6 · lotes 270A–275A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 270A | RpWorld | 0x00758EC0–0x0075A250 | 7 | 4,672 | mediana | 2 | 3/7 |
| 271A | RpWorld | 0x0075A370–0x007637F0 | 18 | 4,039 | mediana | 7 | 7/18 |
| 272A | RpWorld | 0x00763A80–0x00764D30 | 16 | 4,839 | mediana | 10 | 10/16 |
| 273A | RpWorld | 0x00764E70–0x007666F0 | 32 | 4,699 | mediana | 2 | 2/32 |
| 274A | RpWorld | 0x00766732–0x00805710 | 28 | 2,350 | mediana | 2 | 2/28 |
| 275A | RpMatFX | 0x0080FFE0–0x00814270 | 21 | 3,463 | mediana | 3 | 9/21 |

## Parte 7 · lotes 276A–281A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 276A | RpMatFX | 0x008149E0–0x008149E0 | 1 | 4,666 | grande | 0 | 0/1 |
| 277A | RpMatFX | 0x00815E60–0x00818670 | 32 | 1,059 | pequeña | 0 | 0/32 |
| 278A | RpMatFX | 0x00818970–0x0081E406 | 32 | 2,212 | mediana | 2 | 0/32 |
| 279A | RpMatFX | 0x0081E40C–0x0081F0C0 | 6 | 3,073 | mediana | 0 | 0/6 |
| 280A | RpSkin | 0x007C6820–0x007C9680 | 26 | 4,179 | mediana | 3 | 13/26 |
| 281A | RpHAnim | 0x007C1D80–0x007C3B40 | 15 | 4,896 | mediana | 0 | 0/15 |

## Parte 8 · lotes 282A–287A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 282A | RpHAnim | 0x007C4120–0x007C64C0 | 8 | 3,500 | mediana | 2 | 6/8 |
| 283A | RpUVAnim | 0x007CAAD0–0x007CC810 | 20 | 3,282 | mediana | 5 | 8/20 |
| 284A | RtAnim | 0x007CCC50–0x007CD8D0 | 8 | 1,535 | mediana | 2 | 6/8 |
| 285A | RtDict | 0x007CED40–0x007CF5B0 | 10 | 1,555 | mediana | 7 | 10/10 |
| 286A | RtQuat | 0x007E9430–0x007E94B0 | 2 | 313 | pequeña | 0 | 0/2 |
| 287A | RtQuat | 0x007E9570–0x007E9570 | 1 | 3,084 | grande | 0 | 0/1 |

## Parte 9 · lotes 288A–293A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 288A | RtQuat | 0x007EA1C0–0x007EB260 | 11 | 4,962 | mediana | 0 | 0/11 |
| 289A | RtQuat | 0x007EB5C0–0x007EBBB0 | 6 | 1,529 | mediana | 0 | 3/6 |
| 290A | RtSlerp | 0x007EC220–0x007EC220 | 1 | 949 | pequeña | 0 | 1/1 |
| 291A | RtBMP | 0x007CDF60–0x007CE5D0 | 2 | 2,588 | mediana | 0 | 1/2 |
| 292A | RpAnisot | 0x007FAD20–0x007FAD20 | 1 | 6 | pequeña | 0 | 1/1 |
| 293A | libpng + zlib | 0x007CF600–0x007D0AE0 | 11 | 4,810 | mediana | 1 | 2/11 |

## Parte 10 · lotes 294A–299A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 294A | libpng + zlib | 0x007D0DF0–0x007D2330 | 24 | 4,647 | mediana | 0 | 0/24 |
| 295A | libpng + zlib | 0x007D2410–0x007D3AB0 | 25 | 4,627 | mediana | 0 | 0/25 |
| 296A | libpng + zlib | 0x007D3E90–0x007D6020 | 12 | 3,046 | mediana | 0 | 0/12 |
| 297A | libpng + zlib | 0x007D61A0–0x007D61A0 | 1 | 3,242 | grande | 0 | 0/1 |
| 298A | libpng + zlib | 0x007D6FF0–0x007D7C00 | 7 | 3,234 | mediana | 0 | 0/7 |
| 299A | libpng + zlib | 0x007D7D40–0x007D7D40 | 1 | 1,858 | mediana | 0 | 0/1 |

## Parte 11 · lotes 300A–305A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 300A | libpng + zlib | 0x007D8520–0x007D8520 | 1 | 4,162 | grande | 0 | 0/1 |
| 301A | libpng + zlib | 0x007D95B0–0x007DA8C0 | 8 | 4,843 | mediana | 0 | 0/8 |
| 302A | libpng + zlib | 0x007DA940–0x007DBB40 | 14 | 4,468 | mediana | 0 | 0/14 |
| 303A | libpng + zlib | 0x007DBCB0–0x007DD020 | 8 | 4,467 | mediana | 0 | 0/8 |
| 304A | libpng + zlib | 0x007DD130–0x007DDAD0 | 6 | 2,977 | mediana | 0 | 0/6 |
| 305A | libpng + zlib | 0x007DDD10–0x007DDD10 | 1 | 3,041 | grande | 0 | 0/1 |

## Parte 12 · lotes 306A–311A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 306A | libpng + zlib | 0x007DE900–0x007DFF00 | 18 | 4,937 | mediana | 0 | 0/18 |
| 307A | libpng + zlib | 0x007DFFB0–0x007E1F70 | 15 | 4,509 | mediana | 0 | 0/15 |
| 308A | libpng + zlib | 0x007E21F0–0x007E2B20 | 4 | 3,271 | mediana | 0 | 0/4 |
| 309A | libpng + zlib | 0x007E2EE0–0x007E3CF0 | 3 | 4,767 | mediana | 0 | 0/3 |
| 310A | libpng + zlib | 0x007E41A0–0x007E5240 | 6 | 4,938 | mediana | 0 | 0/6 |
| 311A | libpng + zlib | 0x007E5520–0x007E6560 | 6 | 4,805 | mediana | 0 | 0/6 |

## Parte 13 · lotes 312A–317A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 312A | libpng + zlib | 0x007E6870–0x007E78B0 | 9 | 4,371 | mediana | 0 | 0/9 |
| 313A | libpng + zlib | 0x007E7A10–0x007E9300 | 2 | 1,042 | pequeña | 0 | 0/2 |
| 314A | D3DX9 (compilador de shaders) | 0x00766088–0x00769122 | 32 | 3,204 | mediana | 0 | 0/32 |
| 315A | D3DX9 (compilador de shaders) | 0x0076912F–0x0076A93E | 32 | 4,134 | mediana | 0 | 0/32 |
| 316A | D3DX9 (compilador de shaders) | 0x0076AB6F–0x0076C254 | 31 | 4,801 | mediana | 0 | 0/31 |
| 317A | D3DX9 (compilador de shaders) | 0x0076C2A4–0x0076D37E | 13 | 4,748 | mediana | 0 | 0/13 |

## Parte 14 · lotes 318A–323A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 318A | D3DX9 (compilador de shaders) | 0x0076D5A5–0x0076EA95 | 25 | 4,931 | mediana | 0 | 0/25 |
| 319A | D3DX9 (compilador de shaders) | 0x0076EB1C–0x0076FC31 | 21 | 4,191 | mediana | 0 | 0/21 |
| 320A | D3DX9 (compilador de shaders) | 0x0076FC66–0x00770859 | 6 | 4,924 | mediana | 0 | 0/6 |
| 321A | D3DX9 (compilador de shaders) | 0x0077107F–0x00771F63 | 13 | 3,688 | mediana | 0 | 0/13 |
| 322A | D3DX9 (compilador de shaders) | 0x00771F9D–0x00773D4F | 16 | 4,976 | mediana | 0 | 0/16 |
| 323A | D3DX9 (compilador de shaders) | 0x00773DAE–0x00774C33 | 7 | 4,938 | mediana | 0 | 0/7 |

## Parte 15 · lotes 324A–329A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 324A | D3DX9 (compilador de shaders) | 0x0077514F–0x00775EC5 | 5 | 4,891 | mediana | 0 | 0/5 |
| 325A | D3DX9 (compilador de shaders) | 0x007764AD–0x007765FA | 3 | 1,111 | pequeña | 0 | 0/3 |
| 326A | D3DX9 (compilador de shaders) | 0x00776904–0x00776904 | 1 | 21,406 | grande | 0 | 0/1 |
| 327A | D3DX9 (compilador de shaders) | 0x0077BE0F–0x0077C7D8 | 2 | 2,557 | mediana | 0 | 0/2 |
| 328A | D3DX9 (compilador de shaders) | 0x0077C8D2–0x0077C8D2 | 1 | 7,938 | grande | 0 | 0/1 |
| 329A | D3DX9 (compilador de shaders) | 0x0077E869–0x0077F407 | 15 | 3,821 | mediana | 0 | 0/15 |

## Parte 16 · lotes 330A–335A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 330A | D3DX9 (compilador de shaders) | 0x0077FA8F–0x00780E11 | 32 | 4,740 | mediana | 0 | 0/32 |
| 331A | D3DX9 (compilador de shaders) | 0x00780E2D–0x00781F69 | 32 | 3,150 | mediana | 0 | 0/32 |
| 332A | D3DX9 (compilador de shaders) | 0x00781F90–0x00782C32 | 32 | 1,873 | mediana | 0 | 0/32 |
| 333A | D3DX9 (compilador de shaders) | 0x00782E1D–0x00783897 | 32 | 1,580 | mediana | 0 | 0/32 |
| 334A | D3DX9 (compilador de shaders) | 0x007838B3–0x00784BD1 | 32 | 2,969 | mediana | 0 | 0/32 |
| 335A | D3DX9 (compilador de shaders) | 0x00784BF1–0x007855F9 | 32 | 2,437 | mediana | 0 | 0/32 |

## Parte 17 · lotes 336A–341A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 336A | D3DX9 (compilador de shaders) | 0x00785609–0x00785F11 | 15 | 2,199 | mediana | 0 | 0/15 |
| 337A | D3DX9 (compilador de shaders) | 0x00785F7C–0x00785F7C | 1 | 3,321 | grande | 0 | 0/1 |
| 338A | D3DX9 (compilador de shaders) | 0x00786C75–0x00787745 | 3 | 3,360 | mediana | 0 | 0/3 |
| 339A | D3DX9 (compilador de shaders) | 0x00787995–0x00787995 | 1 | 3,598 | grande | 0 | 0/1 |
| 340A | D3DX9 (compilador de shaders) | 0x007887A3–0x00789A92 | 15 | 4,621 | mediana | 0 | 0/15 |
| 341A | D3DX9 (compilador de shaders) | 0x00789BA9–0x0078A54C | 5 | 2,994 | mediana | 0 | 0/5 |

## Parte 18 · lotes 342A–347A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 342A | D3DX9 (compilador de shaders) | 0x0078A7CA–0x0078A7CA | 1 | 2,849 | mediana | 0 | 0/1 |
| 343A | D3DX9 (compilador de shaders) | 0x0078B343–0x0078B343 | 1 | 4,667 | grande | 0 | 0/1 |
| 344A | D3DX9 (compilador de shaders) | 0x0078C614–0x0078D3F8 | 5 | 4,938 | mediana | 0 | 0/5 |
| 345A | D3DX9 (compilador de shaders) | 0x0078D961–0x0078D961 | 1 | 5,062 | grande | 0 | 0/1 |
| 346A | D3DX9 (compilador de shaders) | 0x0078ED33–0x0078ED33 | 1 | 3,407 | grande | 0 | 0/1 |
| 347A | D3DX9 (compilador de shaders) | 0x0078FA8A–0x0079092D | 6 | 3,813 | mediana | 0 | 0/6 |

## Parte 19 · lotes 348A–353A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 348A | D3DX9 (compilador de shaders) | 0x0079099C–0x00791936 | 3 | 4,700 | mediana | 0 | 0/3 |
| 349A | D3DX9 (compilador de shaders) | 0x00791BFB–0x00792EC5 | 26 | 4,944 | mediana | 0 | 0/26 |
| 350A | D3DX9 (compilador de shaders) | 0x00793005–0x00794287 | 17 | 4,972 | mediana | 0 | 0/17 |
| 351A | D3DX9 (compilador de shaders) | 0x00794389–0x007951E4 | 12 | 3,806 | mediana | 0 | 0/12 |
| 352A | D3DX9 (compilador de shaders) | 0x00795267–0x00795267 | 1 | 4,769 | grande | 0 | 0/1 |
| 353A | D3DX9 (compilador de shaders) | 0x0079665B–0x00797168 | 4 | 3,809 | mediana | 0 | 0/4 |

## Parte 20 · lotes 354A–359A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 354A | D3DX9 (compilador de shaders) | 0x0079753C–0x00799203 | 10 | 4,602 | mediana | 0 | 0/10 |
| 355A | D3DX9 (compilador de shaders) | 0x0079927B–0x0079A3C0 | 5 | 4,435 | mediana | 0 | 0/5 |
| 356A | D3DX9 (compilador de shaders) | 0x0079A3CE–0x0079A88B | 2 | 1,697 | mediana | 0 | 0/2 |
| 357A | D3DX9 (compilador de shaders) | 0x0079AA6F–0x0079AA6F | 1 | 4,017 | grande | 0 | 0/1 |
| 358A | D3DX9 (compilador de shaders) | 0x0079D1E5–0x0079E025 | 13 | 4,140 | mediana | 0 | 0/13 |
| 359A | D3DX9 (compilador de shaders) | 0x0079E3BE–0x0079F184 | 5 | 4,999 | mediana | 0 | 0/5 |

## Parte 21 · lotes 360A–365A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 360A | D3DX9 (compilador de shaders) | 0x0079F745–0x007A0A92 | 8 | 4,997 | mediana | 0 | 0/8 |
| 361A | D3DX9 (compilador de shaders) | 0x007A0ACC–0x007A1774 | 6 | 3,353 | mediana | 0 | 0/6 |
| 362A | D3DX9 (compilador de shaders) | 0x007A17E5–0x007A17E5 | 1 | 4,715 | grande | 0 | 0/1 |
| 363A | D3DX9 (compilador de shaders) | 0x007A2A55–0x007A3C97 | 9 | 4,998 | mediana | 0 | 0/9 |
| 364A | D3DX9 (compilador de shaders) | 0x007A3DF6–0x007A4B64 | 9 | 3,399 | mediana | 0 | 0/9 |
| 365A | D3DX9 (compilador de shaders) | 0x007A4BE2–0x007A5E4F | 7 | 4,730 | mediana | 0 | 0/7 |

## Parte 22 · lotes 366A–371A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 366A | D3DX9 (compilador de shaders) | 0x007A5EEA–0x007A70D4 | 16 | 4,333 | mediana | 0 | 0/16 |
| 367A | D3DX9 (compilador de shaders) | 0x007A71CB–0x007A7E55 | 5 | 4,449 | mediana | 0 | 0/5 |
| 368A | D3DX9 (compilador de shaders) | 0x007A832D–0x007A8F74 | 4 | 2,406 | mediana | 0 | 0/4 |
| 369A | D3DX9 (compilador de shaders) | 0x007A8FF5–0x007A9D19 | 4 | 4,568 | mediana | 0 | 0/4 |
| 370A | D3DX9 (compilador de shaders) | 0x007AA21B–0x007AB769 | 13 | 4,992 | mediana | 0 | 0/13 |
| 371A | D3DX9 (compilador de shaders) | 0x007AB787–0x007ACD1E | 18 | 4,939 | mediana | 0 | 0/18 |

## Parte 23 · lotes 372A–377A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 372A | D3DX9 (compilador de shaders) | 0x007ACDE8–0x007ADC15 | 11 | 4,419 | mediana | 0 | 0/11 |
| 373A | D3DX9 (compilador de shaders) | 0x007ADFE1–0x007AF123 | 12 | 4,879 | mediana | 0 | 0/12 |
| 374A | D3DX9 (compilador de shaders) | 0x007AF2F0–0x007B00FC | 15 | 3,843 | mediana | 0 | 0/15 |
| 375A | D3DX9 (compilador de shaders) | 0x007B0316–0x007B1277 | 4 | 4,792 | mediana | 0 | 0/4 |
| 376A | D3DX9 (compilador de shaders) | 0x007B15CE–0x007B25AB | 10 | 4,104 | mediana | 0 | 0/10 |
| 377A | D3DX9 (compilador de shaders) | 0x007B25D6–0x007B2C7B | 2 | 3,219 | mediana | 0 | 0/2 |

## Parte 24 · lotes 378A–383A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 378A | D3DX9 (compilador de shaders) | 0x007B32C6–0x007B4533 | 15 | 4,868 | mediana | 0 | 0/15 |
| 379A | D3DX9 (compilador de shaders) | 0x007B4668–0x007B51C0 | 32 | 2,915 | mediana | 0 | 0/32 |
| 380A | D3DX9 (compilador de shaders) | 0x007B51E1–0x007B6102 | 17 | 3,814 | mediana | 0 | 0/17 |
| 381A | D3DX9 (compilador de shaders) | 0x007B6118–0x007B7656 | 18 | 4,945 | mediana | 0 | 0/18 |
| 382A | D3DX9 (compilador de shaders) | 0x007B7761–0x007B88E8 | 18 | 4,759 | mediana | 0 | 0/18 |
| 383A | D3DX9 (compilador de shaders) | 0x007B89F8–0x007B9D6E | 16 | 4,933 | mediana | 0 | 0/16 |

## Parte 25 · lotes 384A–389A

| Lote | Clase / módulo | Rango | Funciones | Bytes | Tamaño | Envolturas | Clase exacta |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 384A | D3DX9 (compilador de shaders) | 0x007B9E71–0x007BC0D2 | 18 | 4,095 | mediana | 0 | 0/18 |
| 385A | D3DX9 (compilador de shaders) | 0x007BC2C0–0x007BD3E1 | 2 | 2,674 | mediana | 0 | 0/2 |
| 386A | D3DX9 (compilador de shaders) | 0x007BD5A1–0x007BD5A1 | 1 | 4,504 | grande | 0 | 0/1 |
| 387A | D3DX9 (compilador de shaders) | 0x007BE739–0x007BEE12 | 2 | 702 | pequeña | 0 | 0/2 |
| 388A | D3DX9 (compilador de shaders) | 0x007BF309–0x007BF309 | 1 | 4,436 | grande | 0 | 0/1 |
| 389A | D3DX9 (compilador de shaders) | 0x007C045D–0x007C1BEF | 15 | 4,673 | mediana | 0 | 0/15 |

