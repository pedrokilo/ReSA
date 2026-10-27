<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CBDNMaterialSetup30: CCustomBuildingDNPipeline::CustomPipeMaterialSetup (0x5D7120)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar GTA; lo corre el coordinador en serie). Lo único ejecutado sin juego: `fixture_oracle.py` (autoprueba), `fixture_check.py --synthetic` (Fixtures.hpp real compilado con `cl` + oráculo + `run_local.analyze` sobre un registro sintético perfecto) y `host_check.py` (el TEXTO real de la función C++ compilado contra una maqueta, 5120 fixtures × 3 pares de desplazamientos, 0 diferencias con el oráculo). Comprobación de sintaxis de `Core.hpp` con `cl /Zs` y una maqueta de las cabeceras del juego.

Plantilla: `reversing/FileLoaderLoad2dEffect27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` + adenda lote 30 (`__asm` sólo en el banco).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh CBDNMaterialSetup30        # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/CBDNMaterialSetup30/build_probe.py
python -B reversing/CBDNMaterialSetup30/run_local.py
python -B reversing/CBDNMaterialSetup30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe)
python -I reversing/CBDNMaterialSetup30/fixture_oracle.py                      # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/CBDNMaterialSetup30/fixture_check.py --synthetic           # Fixtures.hpp real (cl) vs constantes del oráculo + cobertura + run_local.analyze sintético
python -I reversing/CBDNMaterialSetup30/host_check.py                          # la fuente C++ real contra el oráculo, sin juego
python -B reversing/CBDNMaterialSetup30/register_hook.py <carpetaEvidencia>    # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol con `RH_ScopedInstall(CustomPipeMaterialSetup, 0x5D7120)` (clase `CCustomBuildingDNPipeline`, categoría `Pipelines`; `HookRoute` lo busca por esos nombres) — parche en `source.patch` (contra HEAD, `git apply --check` verificado). Archivo de arranque en el clon: `cbdnmatsetup30-runtime.start`; salida `cbdnmatsetup30-runtime.jsonl`. **5120 pares** (10 grupos × 512).

## Qué hace la función (desensamblado 0x5D7120..0x5D71B4)

`RpMaterial* __cdecl(RpMaterial* material, void* data)`, `ret` normal. Es el callback que `CustomPipeAtomicSetup` (0x5D71C0, `push 0x5D7120`) pasa a `RpGeometryForAllMaterials` (0x74C790). El segundo argumento no se usa.
1. `dword[material+0x10] = 0` (la palabra de banderas = campo `specular` de las propiedades de superficie).
2. `RpMatFXMaterialGetEffects(material)` (0x812140: `[material+[0xC9AB74]]` nulo → 0, si no `[ptr+0x30]`). Si **== 2** (no una máscara):
   - `SetFxEnvTexture(material + [0x8D12C4])` (0x5D9570). Si devuelve no nulo `e`: `tex = [[material+[0xC9AB74]]+4]` (textura del primer pase), `[e+8] = tex`, y si `tex != 0`: `byte[tex+0x51]=0x11; byte[tex+0x50]=2`.
3. **Se vuelve a leer** `env = [material+[0x8D12C4]]` (no se usa lo que devolvió SetFxEnvTexture): `has = env != 0 && (float)byte[env+4]*(1/255) != 0 && dword[env+8] != 0`.
4. `dword[material+0x10] = (dword[material+0x10] & 0xFFFFFFF8) | has` (la palabra se **relee**: conserva lo que haya escrito alguien entre medias), `return material`.

## Fuente C++ y revisión contra el EXE (fase 2)

Fuente: `CustomBuildingDNPipeline.cpp` (hunks en `source.patch`). Había un borrador sin commit en el árbol principal (`git diff HEAD`); se contrastó línea a línea con el desensamblado y es **funcionalmente idéntico** (sin discrepancias: orden de lecturas, relectura de la ranura, máscara `0xFFFFFFF8`, comparación `!= 0.0f` equivale al `fcomp`/`test ah,44h`/`jnp` del EXE salvo NaN, inalcanzable con un byte). Cambios respecto al borrador: comentario completo, `flags` como referencia relida, `ms_envMapPluginOffset` del propio `CCustomCarEnvMapPipeline` en lugar de un local, y el hook `RH_ScopedInstall(CustomPipeMaterialSetup, 0x5D7120)`. Constantes verificadas en el EXE: `[0x859A3C] = 0x3B808081` (1/255), `[0x858B50] = 0.0`.

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- Ventana del material (0x80 bytes con guardas), con las tres palabras especiales (banderas +0x10, ranura de entorno, ranura de MatFX) aparte: **valor final de las banderas**, etiqueta de cada ranura; datos de entorno `envA`/`envB` (16 bytes, palabra +8 como etiqueta), texturas `tex0`/`tex1` (0x60 bytes: aquí se ven los bytes +0x50/+0x51), datos de MatFX (0x40 bytes), guardas de todas las arenas y la **traza** del falso `[1, desplazamiento de la ranura, etiqueta de *ranura antes, 0]`. `EAX == material` (el recorrido de `RpGeometryForAllMaterials` hace `test eax,eax`).
- ABI: ESP (`popBytes=8`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW 24 bits, TOP, tags y ST válidos: pila vacía).
- **EAX/ECX/EDX**: el único llamador (`0x74C790`, llamada indirecta `call ebp`) sólo lee EAX tras el `call` (`test eax,eax`); EAX se exige igual a `material`; ECX/EDX sólo se registran (`original_residue`, `candidate_residue`; el EXE deja ECX = banderas y EDX = `has`). `residue_scan.py 0x5D7120` no halla llamadas directas (las dos referencias son `push 0x5D7120`).
- Rutas (`route_ok`): original → el hook 0x5D7120 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; `RpMatFXMaterialGetEffects` (0x812140) no enganchado en ambos lados. Se vigilan 16 bytes de 0x5D7120, 0x5D9570 y 0x812140.

## Colaboradores

**Sustituido** (mismo falso en ambos lados, parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL resuelto antes de parchear; activo sólo durante la llamada): `CCustomBuildingDNPipeline::SetFxEnvTexture` 0x5D9570. Siete modos (`fixture_oracle.py`): 0 devuelve `*ranura`; 1 `*ranura = envA`; 2 `*ranura = nulo`; 3 `*ranura = envB`; 4 devuelve envA sin tocar la ranura; 5 `*ranura = envA` y devuelve nulo; 6 escribe `flags_word` en +0x10 (ejercita la máscara `0xFFFFFFF8`). **Real**: `RpMatFXMaterialGetEffects` (EXE 0x812140; la envoltura de la DLL llama a esa dirección). Los desplazamientos de plugin 0x8D12C4/0xC9AB74 se leen del juego en cada caso (deben estar 4-alineados, ≥ 0x14, distintos y dentro de 0x80; `begin.plugin_offsets`); no se escribe ninguna global.

## Fixtures (`Fixtures.hpp`) y cobertura

10 grupos (n % 10): pide dato y textura (modo 1) / ranura ya con dato (modo 0) / sin dato (modos 2,5) / efectos ≠ 2 / textura nula / brillo 0 / sin MatFX o efectos 0 / modo 6 / efectos «casi 2» (`0x102`, `0x80000002`, …) / aleatorio. Estados previos con palabras de banderas extremas, brillos 0/1/2/127/128/254/255, punteros de textura basura (nunca se desreferencian). `run_local.py` exige mínimos por familia (calibrados con `fixture_check.py` dejando ~50 % de margen).

## Suposiciones y riesgos

- **Arenas**: C++ (`Fixtures.hpp`) y oráculo comparten constantes (`kMatArena=0x100`, `kEnvArena=0x50`, `kTexArena=0xA0`, `kMfxArena=0x80`, guarda 0xA7); `fixture_check.py` compila `fixture_dump.cpp` con `cl` y falla si difieren.
- Si el compilador inlinease `SetFxEnvTexture` dentro de la función (misma TU), el parche del cuerpo no interceptaría la llamada de la DLL y el banco FALLARÍA (no pasaría en silencio): en ese caso marcar `SetFxEnvTexture` como `__declspec(noinline)` para la sonda.
- `SetFxEnvTexture` (hook activo por defecto) no se valida aquí; sólo su contrato con este callback.
- La forma de los datos de MatFX (`[ptr+4]` = textura del primer pase, `[ptr+0x30]` = efectos) está tomada del desensamblado, no de las cabeceras de la DLL.
