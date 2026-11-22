# Plan de retirada del código vivo del EXE por partes y lotes

[Auditoría y decisiones](README.md) · [Lotes (CSV)](plan.csv) · [Funciones vivas (CSV)](funciones-vivas.csv)

Generado por `contrib/codigo_vivo_plan.py`. Los lotes de código tienen 25–45 funciones; los descartes verificables se parten por rango solo para revisarlos. En los lotes de puentes (B) y de huecos (H) «funciones» cuenta direcciones.

| Parte | Descripción | Lotes | Funciones | Bytes |
| --- | --- | ---: | ---: | ---: |
| P1 | Descartes verificables (sin escribir código) | 19 | 4,804 | 439,332 |
| P2 | Retirar los puentes del C++ por dirección | 23 | 617 | 0 |
| P3 | Sustituir por C++ propio | 4 | 19 | 4,315 |
| P4 | Enlazar bibliotecas | 11 | 184 | 48,168 |
| P5 | Fase 8: arranque y enlace | 4 | 81 | 4,077 |
| P6 | Huecos del .text con código sin función | 6 | 270 | 128,357 |

## Lotes

| Lote | Parte | Tipo | Grupo | Decisión | Destino | Func. | Bytes | Rango | Riesgo | Depende de | Estado |
| --- | --- | --- | --- | --- | --- | ---: | ---: | --- | --- | --- | --- |
| V1 | P1 | verificar y retirar | Protección del EXE | DESCARTAR | protección | 18 | 2,551 | 0x004027CD–0x00746800 | bajo |  | parcial |
| V2-1 | P1 | verificar y retirar | Juego excluido (cerrado por inspección) | DESCARTAR | cubierta/salto/fragmento | 45 | 13,330 | 0x0048DE60–0x005CF1B0 | bajo |  | parcial |
| V2-2 | P1 | verificar y retirar | Juego excluido (cerrado por inspección) | DESCARTAR | cubierta/salto/fragmento | 40 | 6,478 | 0x005CF250–0x0085742B | bajo |  | parcial |
| V3-1 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 600 | 6,451 | 0x00781A04–0x0083A230 | bajo |  | parcial |
| V3-2 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 600 | 6,610 | 0x0083A250–0x0083CFCA | bajo |  | cerrado |
| V3-3 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 600 | 6,355 | 0x0083CFD5–0x0083FF71 | bajo |  | parcial |
| V3-4 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 600 | 6,024 | 0x0083FF7C–0x008434A6 | bajo |  | cerrado |
| V3-5 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 600 | 6,234 | 0x008434B1–0x00846770 | bajo |  | cerrado |
| V3-6 | P1 | verificar y retirar | Excepciones (Unwind/Catch) | DESCARTAR | huérfano de excepciones | 504 | 5,164 | 0x00846790–0x00848EFE | bajo |  | cerrado |
| V4-1 | P1 | verificar y retirar | D3DX9 | DESCARTAR | biblioteca embebida sin raíces vivas | 250 | 75,966 | 0x00765570–0x00776904 | bajo | E3 | verificado sin cerrar |
| V4-2 | P1 | verificar y retirar | D3DX9 | DESCARTAR | biblioteca embebida sin raíces vivas | 250 | 53,995 | 0x0077BE0F–0x0078A7CA | bajo | E3 | verificado sin cerrar |
| V4-3 | P1 | verificar y retirar | D3DX9 | DESCARTAR | biblioteca embebida sin raíces vivas | 250 | 137,259 | 0x0078B343–0x007B00BE | bajo | E3 | verificado sin cerrar |
| V4-4 | P1 | verificar y retirar | D3DX9 | DESCARTAR | biblioteca embebida sin raíces vivas | 182 | 63,375 | 0x007B00FC–0x007C4220 | bajo | E3 | verificado sin cerrar |
| V5-1 | P1 | verificar y retirar | CRT y arranque | DESCARTAR | sin alcance (posible muerto) | 70 | 8,204 | 0x00746480–0x00825E4C | medio |  | verificado sin cerrar |
| V5-2 | P1 | verificar y retirar | CRT y arranque | DESCARTAR | sin alcance (posible muerto) | 70 | 12,920 | 0x00825F61–0x0082F880 | medio |  | verificado sin cerrar |
| V5-3 | P1 | verificar y retirar | CRT y arranque | DESCARTAR | sin alcance (posible muerto) | 63 | 10,599 | 0x0082FE09–0x008370D0 | medio |  | verificado sin cerrar |
| V6-1 | P1 | verificar y retirar | QuickTime/QTML | DESCARTAR | sin alcance (posible muerto) | 31 | 1,237 | 0x004E7990–0x0081DAA0 | medio |  | verificado sin cerrar |
| V7-1 | P1 | verificar y retirar | RenderWare no reconstruible | DESCARTAR | sin alcance (posible muerto) | 17 | 5,656 | 0x004CA5A0–0x00857432 | medio |  | verificado sin cerrar |
| V8-1 | P1 | verificar y retirar | libpng y zlib | DESCARTAR | sin alcance (posible muerto) | 14 | 10,924 | 0x007D7470–0x007E18D0 | medio |  | verificado sin cerrar |
| B1 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x004032A4–0x00432B10 | bajo |  | parcial |
| B2 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x004385B0–0x004394B0 | bajo |  | pendiente |
| B3 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x004394E0–0x0043A4B0 | bajo |  | pendiente |
| B4 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x0043A4C0–0x00493360 | bajo |  | parcial |
| B5 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x00493420–0x004CB530 | bajo |  | parcial |
| B6 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x004CB640–0x004F3BA0 | bajo |  | parcial |
| B7 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x004F3CE0–0x005C6850 | bajo |  | parcial |
| B8 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x005C69E0–0x005CD6B0 | bajo |  | pendiente |
| B9 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x005CEBC0–0x005EAE70 | bajo |  | parcial |
| B10 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x005EAEA0–0x005ECB30 | bajo |  | pendiente |
| B11 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x005ECBB0–0x006348F0 | bajo |  | parcial |
| B12 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x0063BDD0–0x006CE370 | bajo |  | parcial |
| B13 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x006D03A0–0x00749120 | bajo |  | parcial |
| B14 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x00749160–0x007D2630 | bajo |  | parcial |
| B15 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x007D2C20–0x007FAC10 | bajo |  | parcial |
| B16 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x007FB480–0x0080D570 | bajo |  | parcial |
| B17 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 25 | 0 | 0x0080D600–0x00824257 | bajo |  | parcial |
| B18 | P2 | puente a C++ | Reemplazada | REEMPLAZAR_LLAMADA | llamar a la función C++ | 9 | 0 | 0x00824269–0x00833B96 | bajo |  | pendiente |
| B-R1 | P2 | revisar direcciones | No son entrada de función | REVISAR | revisar | 40 | 0 | 0x00402705–0x004032B3 | bajo |  | clasificado |
| B-R2 | P2 | revisar direcciones | No son entrada de función | REVISAR | revisar | 40 | 0 | 0x004032D7–0x005C5090 | bajo |  | clasificado |
| B-R3 | P2 | revisar direcciones | No son entrada de función | REVISAR | revisar | 40 | 0 | 0x005C53D0–0x005CD160 | bajo |  | clasificado |
| B-R4 | P2 | revisar direcciones | No son entrada de función | REVISAR | revisar | 40 | 0 | 0x005CD220–0x00763310 | bajo |  | clasificado |
| B-R5 | P2 | revisar direcciones | No son entrada de función | REVISAR | revisar | 23 | 0 | 0x007F12F0–0x00856BC0 | bajo |  | clasificado |
| S1 | P3 | sustituir | QuickTime/QTML | SUSTITUIR | Media Foundation / sin QuickTime | 3 | 78 | 0x00818650–0x0081A3C0 | medio |  | pendiente |
| S2 | P3 | sustituir | RenderWare no reconstruible | SUSTITUIR | C++ propio (RenderWare D3D9) | 6 | 2,563 | 0x004CB730–0x004CCE60 | alto |  | pendiente |
| S3 | P3 | sustituir | RenderWare no reconstruible | SUSTITUIR | C++ propio (vectores) | 6 | 844 | 0x00857000–0x00857404 | medio |  | pendiente |
| S4 | P3 | sustituir | CRT y arranque | SUSTITUIR | C++ propio (ExeCrt) | 4 | 830 | 0x00821B40–0x00833605 | alto |  | pendiente |
| E1-1 | P4 | enlazar | libpng y zlib | ENLAZAR | libpng 1.0.12 + zlib 1.1.3 (libs/) | 25 | 13,694 | 0x007D26E0–0x007E21F0 | medio |  | enlazado (parcial) |
| E1-2 | P4 | enlazar | libpng y zlib | ENLAZAR | libpng 1.0.12 + zlib 1.1.3 (libs/) | 23 | 19,230 | 0x007E2600–0x007E77E0 | medio |  | enlazado (parcial) |
| E2-excepciones-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 40 | 3,583 | 0x008212ED–0x0082871B | medio |  | pendiente |
| E2-excepciones-2 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 11 | 760 | 0x008289C0–0x00836ECE | medio |  | pendiente |
| E2-montón-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 21 | 1,731 | 0x00821195–0x00830141 | medio |  | pendiente |
| E2-locale-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 20 | 5,316 | 0x00822070–0x00834B8A | alto |  | pendiente |
| E2-E/S-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 15 | 853 | 0x008232CE–0x00835177 | medio |  | pendiente |
| E2-cadenas-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 12 | 2,507 | 0x008213D0–0x00836697 | medio |  | pendiente |
| E2-estado-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 15 | 481 | 0x008216FF–0x00834379 | medio |  | pendiente |
| E2-sin-1 | P4 | enlazar | CRT y arranque | ENLAZAR | CRT de MSVC | 1 | 13 | 0x00822E45–0x00822E45 | medio |  | pendiente |
| E3 | P4 | enlazar | D3DX9 | ENLAZAR | D3DX9 real (d3dx9_43.dll) | 1 | 0 | 0x007652AE–0x007652AE | bajo |  | cerrado |
| F1 | P5 | fase 8 | CRT y arranque | FASE8 | arranque del CRT enlazado | 27 | 3,117 | 0x00821397–0x00835335 | alto | E2-excepciones-1 | pendiente |
| F2 | P5 | fase 8 | CRT y arranque | FASE8 | inicializadores estáticos | 4 | 248 | 0x00826A72–0x008339CA | alto | F1 | pendiente |
| F3 | P5 | fase 8 | Importaciones y carga retardada | FASE8 | enlace (delay-load) | 6 | 120 | 0x00807C10–0x00836AFC | bajo |  | pendiente |
| F4 | P5 | revisar | Excepciones (Unwind/Catch) | REVISAR | dueño no resuelto | 44 | 592 | 0x00838190–0x00838A46 | bajo |  | pendiente |
| H1 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 21,802 | 0x00402705–0x0050E462 | medio |  | clasificado: 42 de 45 huecos cerrados |
| H2 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 15,986 | 0x005173F0–0x005CC289 | medio |  | clasificado: 45 de 45 huecos cerrados |
| H3 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 12,802 | 0x005CC698–0x00728B06 | medio |  | clasificado: 45 de 45 huecos cerrados |
| H4 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 24,479 | 0x0072F505–0x00761C26 | medio |  | clasificado: 42 de 45 huecos cerrados |
| H5 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 34,977 | 0x00761DDE–0x007EF33E | medio |  | clasificado: 42 de 45 huecos cerrados |
| H6 | P6 | verificar huecos | Huecos del .text | REVISAR | código sin función de Ghidra | 45 | 18,311 | 0x007F12E3–0x008568AB | medio |  | clasificado: 43 de 45 huecos cerrados |
