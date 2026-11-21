<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

| Lote | Grupo | Fn | Bytes | x87 | SEH | EXE la llama | Arranque | Depende de | Banco estandar |
|---|---|---:|---:|---:|---:|---:|---:|---|---|
| S1A | juego: envolturas y callbacks sencillos (sin x87 ni SEH) | 13 | 1038 | 0 | 0 | 0 | 0 | - | si (metodo estandar) |
| S2A | juego: x87, SEH, COM de DirectInput, excluidas | 25 | 7724 | 14 | 3 | 5 | 1 | - | si (metodo estandar) |
| S3A | juego: libjpeg, compresion (0x5C2FF0-0x5C8xxx) | 29 | 7651 | 2 | 0 | 0 | 0 | - | si, con estructuras jpeg falsas |
| S4A | juego: libjpeg, descompresion, memoria y errores | 29 | 6062 | 2 | 1 | 0 | 0 | - | si, con estructuras jpeg falsas |
| S5A | RenderWare RtQuat+RtBMP+RtDict+RtAnim | 15 | 1925 | 2 | 0 | 0 | 0 | - (y posterior: S20A) | si, con memoria falsa |
| S6A | RenderWare RtAnim | 14 | 2442 | 3 | 0 | 0 | 0 | - | si, con memoria falsa |
| S7A | RenderWare RpAnisot | 8 | 233 | 0 | 0 | 0 | 8 | - | si, con memoria falsa |
| S8A | RenderWare RpUVAnim | 13 | 717 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S9A | RenderWare RpUVAnim | 13 | 1769 | 9 | 0 | 0 | 0 | S6A | si, con memoria falsa |
| S10A | RenderWare RpHAnim | 13 | 1173 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S11A | RenderWare RpHAnim | 13 | 4103 | 5 | 0 | 0 | 0 | S6A | si, con memoria falsa |
| S12A | RenderWare RpSkin | 25 | 2666 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S13A | RenderWare RpMatFX | 23 | 1510 | 0 | 0 | 0 | 9 | - | si, con memoria falsa |
| S14A | RenderWare RpMatFX | 22 | 5114 | 7 | 0 | 0 | 3 | - | si, con memoria falsa |
| S15A | RenderWare RwCore | 23 | 1379 | 0 | 0 | 0 | 1 | - | si, con memoria falsa |
| S16A | RenderWare RwCore | 23 | 1159 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S17A | RenderWare RwCore | 23 | 1302 | 0 | 0 | 0 | 0 | - | si, con memoria falsa |
| S18A | RenderWare RwCore | 23 | 2125 | 1 | 0 | 0 | 1 | - | si, con memoria falsa |
| S19A | RenderWare RwCore | 20 | 10177 | 3 | 0 | 0 | 0 | S15A | si, con memoria falsa |
| S20A | RenderWare RwPlCore | 24 | 2144 | 0 | 0 | 0 | 13 | - | si, con memoria y flujos falsos |
| S21A | RenderWare RwPlCore | 24 | 797 | 1 | 0 | 0 | 11 | - | si, con memoria y flujos falsos |
| S22A | RenderWare RwPlCore+RwTexDict | 24 | 4988 | 7 | 2 | 0 | 14 | - | si, con memoria y flujos falsos |
| S23A | RenderWare RpWorld | 24 | 992 | 0 | 0 | 0 | 7 | - | si, con memoria y flujos falsos |
| S24A | RenderWare RpWorld | 24 | 1139 | 0 | 0 | 0 | 5 | - | si, con memoria y flujos falsos |
| S25A | RenderWare RpWorld | 24 | 1128 | 0 | 0 | 0 | 17 | - | si, con memoria y flujos falsos |
| S26A | RenderWare RpWorld | 24 | 1242 | 0 | 0 | 0 | 6 | - | si, con memoria y flujos falsos |
| S27A | RenderWare RpWorld | 24 | 2275 | 0 | 0 | 0 | 0 | - | si, con memoria y flujos falsos |
| S28A | RenderWare RpWorld | 24 | 3325 | 0 | 0 | 0 | 6 | - | si, con memoria y flujos falsos |
| S29A | RenderWare RpWorld | 24 | 3170 | 1 | 0 | 0 | 2 | - | si, con memoria y flujos falsos |
| S30A | RenderWare RpWorld | 24 | 7638 | 10 | 0 | 0 | 0 | S24A,S29A | si, con memoria y flujos falsos |
| S31A | RenderWare RpWorld | 18 | 11211 | 9 | 1 | 0 | 0 | S15A,S17A,S18A,S19A,S20A,S22A,S24A,S26A,S28A,S29A,S30A | si, con memoria y flujos falsos |
| S32A | RenderWare RwCore | 14 | 441 | 0 | 0 | 0 | 0 | - | no hasta tener el dispositivo D3D falso |
| S33A | RenderWare RwCore+Controlador D3D9 (RwD3D9) | 14 | 871 | 0 | 0 | 0 | 0 | - | no hasta tener el dispositivo D3D falso |
| SQA | puentes QTML (no reconstruibles) | 31 | 806 |  |  |  |  | - | no: validacion por inspeccion |
| LIB | biblioteca (lote aparte): biblioteca: CRT 57; biblioteca: D3DX9 19; bi | 80 | 14418 |  |  |  |  | - | depende |
