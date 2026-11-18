<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnF1A: lote F1A (6 funciones del juego) mas las 4 descompresoras DXT de R10A

Estado: **APROBADO dentro del juego** (18/11/2026, worktree `wv-5` desde a94366deac + la correccion de `BugstarWriteScreenShotEntry`; plantilla `Fn173A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fnf1a/20261118-150836-862010` (archivada en `reversing/CppTwelveLevel6Final/runs/FnF1A-20261118-150836-862010`, `comparisons.jsonl.gz` de 70 MB). 10 x 16384 = **163840 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `be0c0732aee4e149dbc96af0e62afccabc56101176ce16977b742bdca06ccdf3`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

| # | Funcion | Entrada | Colaboradores |
|---|---|---|---|
| 0 | `IKChainManager_c::Constructor` | 0x617FC0 | reales con hook apagado: 0x617850, 0x4A8DB0, 0x4A8DC0, 0x4A8DD0, 0x4A8DE0 |
| 1 | `IKChainManager_c::Destructor` | 0x618030 | los mismos |
| 2 | `WaterCreatureManager_c::Constructor` | 0x6E3EB0 | 0x6E3E30 y los de arriba |
| 3 | `WaterCreatureManager_c::Destructor` | 0x6E3F20 | los mismos |
| 4 | `CWaterLevel::IsPointInCameraDepthRange` | 0x6E67A0 | reales: `CMatrix` 0x59C050, 0x59C890, 0x59ACD0; camara falsa en `Scene.m_pRwCamera` (0xC1703C, restaurado tras cada lado) |
| 5 | `CAudioZones::BugstarWriteScreenShotEntry` | 0x509230 | falsos solo en la direccion del EXE (la DLL los llama por ella): 0x508B80, 0x508D30, 0x509190; real: `TextBufferAppend` 0x508930, operator new/delete y sprintf |
| 6 | `D3D9RasterFormat::DecompressDxt1` | 0x81E720 | ninguno |
| 7 | `D3D9RasterFormat::DecompressDxt3` | 0x81EA70 | ninguno |
| 8 | `D3D9RasterFormat::DecompressDxt5` | 0x81EDC0 | real: `DecodeDxt5AlphaBlock` 0x81F0C0 con el hook apagado |
| 9 | `D3D9RasterFormat::DecodeDxt5AlphaBlock` | 0x81F0C0 | ninguno |

Todas son S (`RH_ScopedInstall` / `RH_ScopedNamedGlobalInstall`); las diez filas de `docs/hooks.csv` pasan a `validated=1`.

## Defecto encontrado y corregido por el banco
- **`BugstarWriteScreenShotEntry` (0x509230)**: la primera corrida dio 1687 de 16384 pares con diferencias, todas en el texto de las coordenadas. La DLL formateaba con el `sprintf` de la biblioteca de C moderna (UCRT) y el EXE con el CRT antiguo de 0x821BB5: NaN sale `1.#QNAN0` en el EXE y `nan` en la DLL, el cero negativo sale `0.000000` (sin signo) y no `-0.000000`, los valores de mas de 17 digitos salen con 17 digitos significativos y ceros (`53236809204322322000000000000000000.000000`) y no con todos los digitos exactos, y los empates se redondean hacia arriba (`86131.507813`, no `86131.507812`). **Correccion**: la DLL llama al `sprintf` del EXE por direccion fija (`reinterpret_cast<int(__cdecl*)(char*, const char*, ...)>(0x821BB5)`), como ya hacen otras funciones con `delete[]` y otras rutinas del CRT. Tras el cambio, 163840 pares sin diferencias. El oraculo (`fmt_coord`) reproduce ese formateo y se calibro con el lado EXE (46816 valores distintos, ninguno fuera del modelo: `calib.py`).
- Sin defectos en las otras nueve: los constructores solo escriben los dos punteros de cada elemento (y el tercer campo de `WaterCreature_c`) y las seis palabras de las listas; `IsPointInCameraDepthRange` coincide con el modelo x87 de 24 bits del oraculo (Fraction exacta con exponente extendido, guardado como float) incluidos NaN, infinitos, empates y bits altos del banderin; las descompresoras DXT coinciden con el modelo de formato.

## Notas de la revision contra el EXE
- 0x6E67A0 devuelve el resultado solo en AL (el EAX alto es basura del ultimo `call`): el banco compara AL; EAX/ECX/EDX quedan como informativos (`pairs_with_different_eax_ecx_edx_info_only`).
- Las tres funciones de cadena de 0x509230 (0x508B80, 0x508D30, 0x509190) son falsas y anotan el hash y la longitud del texto acumulado en cada llamada; el texto final completo se compara byte a byte con el oraculo.
- Las descompresoras DXT se observan por la cuenta y un FNV de los bytes cambiados de cada arena (el volumen de bytes impedia listarlos); el origen (arena C) no debe cambiar. Retoques del banco: extremos de color y de alfa iguales en bloques elegidos.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) de 0x2000 bytes con guardas de 0x40. 16 familias de 1024 casos por funcion (`Fixtures.hpp`): posicion del objeto (alineado, desalineado, al borde, cerca del borde) por relleno (aleatorio, ceros, unos, 0xCD). Cobertura de ramas medida con el oraculo (minimo 50 por marca). `python -I fixture_selfcheck.py` pasa los 163840 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh FnF1A`; las rondas negativas: `bash reversing/FnF1A/negrounds.sh` (una sola sesion del candado del juego). El analizador es en flujo (sin cargar el registro de ~740 MB).

## Prueba negativa
`mutate.py` (10 mutaciones por ronda; las de constructores, destructores y DXT condicionadas a un puntero desalineado, que el juego real no produce; las de 0x6E67A0 y 0x509230 sin condicion o con radio NaN porque no tienen llamadores). **Las 10 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261118-154039-840670` (8211 de 20480 pares con diferencias) y ronda 2 `20261118-154911-635393` (7336 de 20480); el lado EXE coincide con el oraculo en todos los pares. Incidencia: la primera version de la mutacion 4 de la ronda 2 (NaN en z tras la primera comparacion) no se alcanzaba nunca (0 diferencias); se sustituyo por `<=` en la primera comparacion y se repitio la ronda. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) tras cada ronda.

## Fuera de los fixtures
`this` nulo o crudo, camaras reales de RenderWare, imagenes de mas de 200 bytes en 0x509230, dimensiones DXT mayores de 36 o con desbordamiento, memoria del monton del juego real y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep de las direcciones en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x617FC0 -> `Fn124A` (banco del elemento 0x617850, colaborador de 0x617FC0: **repetir**) y `Fn173A` (texto); 0x618030, 0x6E3EB0, 0x6E3F20 y 0x6E67A0 -> `Fn173A` (texto); 0x509230 -> `Fn172A` (banco de 0x508930, 0x508A10 y 0x509140, colaboradores: **repetir**); las cuatro DXT no aparecen citadas. Ademas, `CMatrix` (`Fn97A`, `Fn97B`, `Fn65*`/`Helpers65D`: 0x59C050, 0x59ACD0, 0x59C890) y los bancos de `ListItem_c` / `TList_c` (0x4A8DB0..0x4A8DE0) son colaboradores reales de este banco (sin cambios de codigo).
