<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CBDNRenderCB30: CCustomBuildingDNPipeline::CustomPipeRenderCB (0x5D6480)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar GTA; lo corre el coordinador en serie). Lo único ejecutado sin juego: `fixture_oracle.py` (autoprueba), `fixture_check.py --synthetic` (Fixtures.hpp real compilado con `cl` + oráculo + `run_local.analyze` sobre un registro sintético perfecto) y `host_check.py` (el TEXTO real de la función C++ compilado contra una maqueta con los mismos falsos, 6144 fixtures × 3 desplazamientos de plugin, 0 diferencias con el oráculo; con una mutación del COLORARG2 o de la máscara lo detecta). Sintaxis de `Core.hpp` comprobada con `cl /Zs` y una maqueta de las cabeceras del juego.

Plantilla: `reversing/FileLoaderLoad2dEffect27` y contrato de `reversing/_handoff/BRIEF.md` + adenda lote 30.

## Identificación de la dirección

`docs/implementaciones/envolturas-pendientes.csv` listaba `CustomPipeRenderCB` **sin dirección**. Es **0x5D6480**: el comentario de la envoltura ya la daba (`plugin::Call<0x5D6480>`) y se confirma por referencia cruzada: `CCustomBuildingDNPipeline::CreateCustomObjPipe` (0x5D6750) hace `push 0x5D6480` justo antes de `RxD3D9AllInOneSetRenderCallBack` (0x5D67F9 → 0x7573E0, escribe en `[node+0x14]+0xC`), y el nodo AllInOne de atómicos lo invoca con `call eax` en 0x75E52D (`push flags; push 0xFF; push objeto; push entry`). Rango de la función 0x5D6480..0x5D674B (`ret` normal; cdecl; 4 argumentos: `RwResEntry* entry, void* object, uint8 type, uint32 flags`). Es el callback de dibujo de esta tubería (igual que `CCustomBuildingPipeline::CustomPipeRenderCB` 0x5D77D0 y `CCustomCarEnvMapPipeline::CustomPipeRenderCB` 0x5D9900).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh CBDNRenderCB30          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/CBDNRenderCB30/build_probe.py
python -B reversing/CBDNRenderCB30/run_local.py
python -B reversing/CBDNRenderCB30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe)
python -I reversing/CBDNRenderCB30/fixture_oracle.py                      # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/CBDNRenderCB30/fixture_check.py --synthetic           # Fixtures.hpp real (cl) vs constantes del oráculo + cobertura + run_local.analyze sintético
python -I reversing/CBDNRenderCB30/host_check.py                          # la fuente C++ real contra el oráculo, sin juego
python -B reversing/CBDNRenderCB30/register_hook.py <carpetaEvidencia>    # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol con `RH_ScopedInstall(CustomPipeRenderCB, 0x5D6480)` (clase `CCustomBuildingDNPipeline`, categoría `Pipelines`) — parche en `source.patch` (contra HEAD, `git apply --check` verificado). Archivo de arranque: `cbdnrendercb30-runtime.start`; salida `cbdnrendercb30-runtime.jsonl`. **6144 pares** (12 grupos × 512).

## Qué hace la función (desensamblado 0x5D6480..0x5D674B)

Cabecera = `entry + 0x18`, mallas = cabecera + 0x40 (0x24 bytes cada una: `+8` material, `+0xC` alfa por vértice).
1. `_rwD3D9EnableClippingIfNeeded(object, (u8)type)` (0x756D90). `RwD3D9GetRenderState(0x89 = LIGHTING, &l)` (0x7FC320). **Sin luz** si `l == 0` (DWORD completo) y `!(flags & 8)` (byte bajo): `SetTexture(0,0)`, `SetRenderState(0x3C, 0xFF000000)`, TSS(0,1,3) (0,3,3) (0,4,3) (0,6,3).
2. Si `header[+8]` (indexBuffer) ≠ 0: SetIndices (0x7FA1C0). `_rwD3D9SetStreams(&header[+0x10], header[+0x30])` (0x7FA090). SetVertexDeclaration(`header[+0x34]`) (0x7F9F70).
3. `n = header[+4]` **con signo** (`jle`): por malla: material `m`, `bit = byte[m+0x10] & 1`, `env = [m+[0x8D12C4]]`; TSS(1,1,1) y (2,1,1) (DISABLE). Si `bit`: `fpRenderStateSet(2, 1)` por la tabla del motor (`[[0xC97B24]+0x20]`); matriz global 0xC02C28: `_11 = (int8)env[0]*0.125`, `_44 = _33 = 1.0`, `_22 = (int8)env[1]*0.125`; `SetTexture(env[+8], 1)`; brillo = `_ftol2(((float)env[4]*(1/255))*254.0)` saturado a 255 (inalcanzable: máx 254), factor opaco `0xFF000000|v<<16|v<<8|v`; TSS de la etapa 1: COLOROP=25, COLORARG0=1, COLORARG1=2, **COLORARG2=0 (DIFFUSE)**, ALPHAOP=3, ALPHAARG1=2, ALPHAARG2=1, TEXCOORDINDEX=0x10001, TTF=0; etapa 2 COLOROP=1.
4. `0x7FE0A0(vertexAlpha ≠ 0 ? 1 : (m.alpha ≠ 0xFF ? 1 : 0))`. Sin luz: `0x5DA640(cabecera, malla)`; si no: si `l ≠ 0` `SetSurfaceProperties(m+0xC, m+4, flags)` (0x7FC4D0) y `0x5DA6A0(cabecera, malla, flags, m.textura)`.
5. Final: TSS(1,1,1) (1,4,1) (1,0xB,1) (1,0x18,0).

## Fuente C++ y revisión contra el EXE (fase 2)

Fuente: `CustomBuildingDNPipeline.cpp` (hunks en `source.patch`: la función, 5 envolturas locales por dirección, la matriz 0xC02C28 y el hook). **No había borrador** de esta función en el árbol principal (sólo de `CustomPipeMaterialSetup`). Se escribió desde el desensamblado, siguiendo la estructura de `CCustomBuildingPipeline::CustomPipeRenderCB`, y se contrastó dos veces línea a línea (orden de llamadas, argumentos empujados, signo del contador, lectura única de `numMeshes`, relecturas). Diferencias del EXE respecto a la función hermana de `CCustomBuildingPipeline` (el DLL de esa hermana usa `TFACTOR` en COLORARG2 de la etapa 1, esta usa **DIFFUSE**; la hermana lee las banderas con `GetMaterialFlags` = palabra completa, ésta sólo el bit 0 del byte +0x10; la matriz es 0xC02C28 y no 0xC02C70): están reflejadas en la fuente. **No se verificó** si la hermana (0x5D77D0) tiene ese COLORARG2 realmente en el EXE; conviene revisarlo cuando se valide.
Detalles de fidelidad: el EXE reutiliza las ranuras de argumentos de la pila como variables locales (contador de mallas, indicador «sin luz», temporales de `fild`): no es observable por los llamadores (cdecl, el llamador retira los argumentos); el 3.er argumento se lee como byte (los 24 bits altos de la pila se ignoran; el banco los ensucia).

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **Traza completa** de los colaboradores sustituidos, en orden y con argumentos (ver cabecera de `fixture_oracle.py` para el formato), la **matriz de textura global 0xC02C28** (64 bytes: imagen previa aleatoria; sólo se escribe la diagonal), y memoria intacta con guardas: `RwResEntry`+cabecera+mallas, los 4 materiales, los 4 datos de entorno y el objeto falso.
- ABI: ESP (`popBytes=16`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW 24 bits, pila vacía; el EXE usa `fild/fmul/_ftol2`).
- **EAX/ECX/EDX**: función `void`; el único llamador con la ruta observable (0x75E52D, `call eax; add esp,10h; …; mov eax,1; ret`; y el hermano de sectores 0x75E125, `call eax; add esp,10h; mov eax,[esp+60h]`) no lee EAX/ECX/EDX tras el `call` → sólo se registran (`original_residue`/`candidate_residue`). `residue_scan.py 0x5D6480` no halla llamadas directas (la referencia es `push 0x5D6480`). **El revisor debe confirmar los dos sitios de llamada a mano.**
- Rutas (`route_ok`): original → el hook 0x5D6480 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; el `_ftol2` del EXE (0x821B40) no está enganchado en ambos lados. Se vigilan 16 bytes de 0x5D6480, 0x821B40 y de los doce puntos de entrada parcheados; la tabla del motor (`fpRenderStateSet`) y la matriz se restauran y se comprueban.

## Colaboradores

**Sustituidos** (mismo falso en ambos lados; parche de entrada en la dirección del EXE y, para `_rwD3D9EnableClippingIfNeeded`, también en el cuerpo de la DLL de `rpworld.cpp`, resuelto antes de parchear): 0x756D90 `_rwD3D9EnableClippingIfNeeded`, 0x7FC320 `RwD3D9GetRenderState` (devuelve el valor de LIGHTING del caso), 0x7FDE70 `SetTexture`, 0x7FC2D0 `SetRenderState`, 0x7FC340 `SetTextureStageState`, 0x7FA1C0 `SetIndices`, 0x7FA090 `_rwD3D9SetStreams`, 0x7F9F70 `SetVertexDeclaration`, 0x7FE0A0 alfa por vértice, 0x7FC4D0 `SetSurfaceProperties`, 0x5DA640 y 0x5DA6A0 (dibujo de la malla), y `fpRenderStateSet` (la entrada `[[0xC97B24]+0x20]` se cambia durante la llamada; `LiveComparison.hpp` exige con un `static_assert` que `offsetof(RwGlobals,dOpenDevice)+offsetof(RwDevice,fpRenderStateSet) == 0x20`, como el `call [edx+20h]` del EXE). Las envolturas `RwD3D9*` de la DLL (`rwcore.cpp`) sólo llaman a esas direcciones del EXE, por eso basta parchear el EXE. **Real**: `_ftol2` del EXE (el `(int32)float` de la DLL debe dar lo mismo: valores 0..254).
**Decisión de diseño que el revisor debe conocer**: `SetIndices`, `SetVertexDeclaration`, `0x5DA640` y `0x5DA6A0` se llaman **por dirección del EXE** y no con las macros/funciones inline de la DLL (`RwD3D9SetIndices`/`RwD3D9SetVertexDeclaration` expanden a accesos directos al dispositivo D3D y a los globales `0x8E2450/0x8E2444`; `RxD3D9InstanceDataRender*` de `PipelinesCommon.hpp` son reconstrucciones inline): así la traza es comparable sin un dispositivo falso, y además se evita una diferencia real (la macro no copia el `HRESULT` negativo como hace 0x7F9F70: el EXE pone el último declarado a −1 si falla). `_rwD3D9EnableClippingIfNeeded` sí se llama en C++ (la reconstrucción de `rpworld.cpp`, sin hook); **su equivalencia con 0x756D90 no la cubre este banco** (el EXE: `type == 1` → esfera del atómico, si no caja en `+0x60`; luego `SetRenderState(0x88, !dentro)`).

## Fixtures (`Fixtures.hpp`) y cobertura

12 grupos (n % 12) por número de mallas: 0, 1, 2, 3, 4, 1, 2, 3, 4, −1, INT_MIN, 0. Dimensiones aleatorias: LIGHTING (0, 1, 2, 0x100, 0xFFFFFFFF, 0x80000000), banderas (PRELIT en el byte bajo / sólo en el alto), tipo con basura en los 24 bits altos, objeto (arena / nulo / valor crudo), indexBuffer nulo/no nulo, `useOffsets` extremos, declaración nula/no nula, materiales compartidos entre mallas, bit de entorno con la ranura válida o (bit limpio) nula / dato válido / puntero basura, escalas −128..127, brillos 0..255, alfa de vértice y de color. `run_local.py` exige mínimos por familia (calibrados con `fixture_check.py` dejando ~50 % de margen).

## Suposiciones y riesgos

- **Arenas**: C++ y oráculo comparten constantes (`kResSize=0xE8`, `kResArena=0x168`, `kMatArena=0xC0`, `kEnvArena=0x50`, `kObjArena=0x60`, guarda 0xA7, `OBJ_RAW=0x0BADF00D`); `fixture_check.py` compila `fixture_dump.cpp` con `cl` y falla si difieren.
- Los punteros a texturas/buffers/declaraciones son valores crudos nunca desreferenciados. El desplazamiento 0x8D12C4 se lee del juego (4-alineado, ≥ 0x14, < 0x7C).
- Un compilador que inlinease `_rwD3D9EnableClippingIfNeeded` dentro de la función haría fallar el banco (la traza perdería el evento 1), no pasar en silencio.
- Si el stub de `FakeFpRS` no se invoca (la DLL llamase a otra entrada de la tabla), el banco fallaría por traza.
