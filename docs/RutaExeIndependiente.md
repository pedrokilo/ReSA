<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 01/11/2026 (Ciudad de México):** 820 grupos / 8988 funciones reconstruidas; 8988 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ruta hacia un exe que corra sin el original

Documento de planificación (01/11/2026). Las cifras de catálogo e imports están medidas; los costes y plazos son estimaciones y están marcados como tales. Los datos de `librw` salen del experimento en `D:\Original\librw-test` (no versionado).

## Objetivo

Un ejecutable de San Andreas que no necesite `gta-sa.exe`: todo el código del juego reconstruido en C++ y las dependencias externas sustituidas o enlazadas.

## Lo que depende hoy del exe

### Dependencias externas (imports de `gta-sa.exe` 1.0 US)

| Dependencia | Imports | Plan |
|---|---|---|
| KERNEL32, USER32, ADVAPI32, GDI32, ole32, WS2_32, WINMM | 142, 48, 10, 5, 7, 4, 8 | Sistema operativo: nada que hacer |
| `vorbisfile.dll` | 11 | Biblioteca abierta, se enlaza tal cual |
| `d3d9.dll` | 7 | Lo trae Windows |
| `DINPUT8` | 7 | Sistema, o SDL3 como extra |
| `DSOUND` | 8 | Sistema, o OpenAL |
| `DDRAW` | 10 | Solo lista modos de vídeo; reescribir con D3D9/DXGI |
| `EAX.DLL` | 4 | Obsoleto (Creative); descartar o dejar sin efecto |

### Código enlazado dentro del exe

| Bloque | Tamaño (catálogo) | Plan |
|---|---|---|
| RenderWare SDK | 134 entradas | `librw` cubre la lectura de datos; faltan plugins de SA y el puente |
| Driver D3D9 de RenderWare | 58 entradas | Backend D3D9 de `librw` + pipelines de SA |
| QuickTime/QTML | 2 entradas en catálogo; unas 27 llamadas y 4 ayudantes (`0x4E7990`, `0x4E7A80`, `0x4E7B80`, `0x4E7CB0`) usados por `AEMFDecoder.cpp`; unas 180 funciones puente en `0x818000-0x81B000` (cuenta por patrón) | Sustituir por Media Foundation u otro decodificador de MP3 (radio personal) |
| CRT estático | 7 entradas | CRT del compilador; cuidado con los redondeos que el juego usa |
| Código del juego | unas 70 envolturas pendientes | Lotes de validación en curso |

## Lo que se midió de `librw`

- Compila como biblioteca estática x86 con VS 18.
- Abre 15331 de 15356 DFF y 4049 de 4049 TXD de SA (RW 3.6.0.3). Los 25 DFF restantes son diccionarios de animación UV (chunk 0x2B); el exe tampoco los lee como clump.
- Comparado con el RenderWare original del exe dentro del juego (15309 modelos, 19186 geometrías): 0 diferencias en recuentos, posiciones, normales, UVs, prelit, triángulos, mesh, materiales (color y propiedades), flags, frames, HAnim y skin.
- Única diferencia real: el exe pide más texturas (929 modelos, 560 sin `vehiclespecdot64`) por plugins de SA que `librw` salta sin leer (2dfx, Breakable y material especular; la atribución a Breakable es una inferencia).
- No comparado: MatFX, UVAnim, derechos de render, datos de los plugins de SA, nombres de textura por material (se sustituyeron por "no encontrada"), escritura de archivos.
- El backend D3D9 dibuja un coche y un peatón con skin de SA con texturas correctas. Falta el color de pintura, el filtrado de partes por nombre de frame, las animaciones y los reflejos.
- Adaptaciones de las pruebas: el plugin de colisión de coches (0x253F2FA) se renombró en la copia en memoria para el exe, porque su pool aún no existe en el menú; el hilo principal se suspendía durante cada lectura.

## Hoja de ruta propuesta

1. **Cerrar el código del juego** (lotes actuales). Hasta entonces el exe sigue haciendo falta.
2. **Capa de RenderWare propia.** Enlazar `librw`, registrar los plugins de SA (2dfx, Breakable, Specular y Environment Material, Extra Normals, Node Name, Pipeline Set, COL embebido) y verificarlos con los hashes de `librw-test`.
3. **Pipelines de SA** sobre el backend D3D9: coches (environment map, specular), skin de peatones, vegetación, cristales. Bloque de coste desconocido.
4. **Puentes `RwD3D9*` y del SDK** (58 + 134). Dependen del punto 3.
5. **QuickTime, DDraw y CRT.**
6. **Cambio de método de validación.** Sin el exe, la comparación par a par desaparece: guardar salidas del exe como referencia (como `exe_vals.csv`) o comparar imágenes.

## Riesgos sin resolver

- Paridad visual: reflejos, iluminación nocturna, colores dinámicos.
- Redondeos del CRT y del x87 donde el juego los usa.
- Tablas y globales que el código lee por dirección fija y hay que extraer del binario.
- Escritura de archivos con `librw` sin probar.

## Estimaciones (sin medir)

- Vista correcta de vehículo en la demo: de una a pocas horas.
- Peatones animados (ANP): de uno a dos días.
- Plugins de SA en `librw`: poco riesgo, verificables con hashes.
- Pipelines y puentes D3D9: no estimado.
