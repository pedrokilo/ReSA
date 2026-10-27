<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies22A (grupo g22a de BRIEF_21): CPointLights

Evidencia previa sin juego: arnes de `offline/` (bytes reales del EXE contra el C++ de la DLL); no sustituye a la corrida dentro del juego.

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| CPointLights::GenerateLightsAffectingObject | 0x6FFBB0 | cdecl, 3 args, float en ST0 | 2048 |
| CPointLights::GetLightMultiplier | 0x6FFE70 | cdecl, 1 arg, float en ST0 sin redondear | 1536 |
| CPointLights::ProcessVerticalLineUsingCache | 0x6FFFF0 | cdecl, CVector por valor + float* | 2048 |
| CPointLights::RenderFogEffect | 0x7002D0 | cdecl, sin args (el EXE sale con `jmp 0x70CF20`) | 2048 |

Total 7680 pares, hook RH apagado (EXE) frente a encendido (DLL) con `OriginalDependencies499/HookRoute.hpp`.

## Que se compara (en bits)
Retorno x87 (ST0 dentro del FNSAVE), AL de la funcion booleana, *outZ y total de iluminacion (`observed`), hash de la region 0xC3F050..0xC3F868 (cache, NumLights, luces), traza completa de
colaboradores (hash, longitud y recuentos por tipo; la traza entera se vuelca solo si los lados difieren), ESP, EBX/ESI/EDI/EBP, y el estado x87 completo tras la llamada: FCW, FSW (banderas acumuladas
y C0..C3), FTW/TOP, FOP/FCS/FDP/FDS y los registros con datos. FIP se pone a cero en ambos lados (cada uno ejecuta su ultima instruccion x87 en otro modulo) y se comprueba el modulo.
ECX/EDX al volver se registran (`*_residue`) pero NO se exigen: ningun llamador del EXE los lee (ver `g22a-audit.txt`).

## Limites declarados
- `kCompareDeadX87Registers = true` (no se relaja): los diez bytes de cada registro x87 VACIO coinciden con los del EXE. Los ayudantes de X87Math llevan la pila de la DLL tan baja como la del EXE
  (variables en memoria, constantes `volatile`) y, antes de salir, plantan en T-1 .. T-m los ultimos valores que el EXE deja en esos registros (`PointLightsShadow` repite en orden las instrucciones del
  EXE que escriben un registro, incluido el `_ftol2` del EXE: v - R en la cima y R debajo; `PointLightsPlant*` es el patron X87Plant* de CTaskSimpleGangDriveBy con -0,0 como elemento neutro).
- Oraculo de valores independiente solo para ProcessVerticalLineUsingCache (`fixture_oracle.py`); el resto se decide por igualdad EXE/DLL. Las formulas de Fixtures.hpp se repiten en Python solo para ese oraculo.
- Colaboradores sustituidos (sin credito): AddAnExtraDirectionalLight, CWorld::ProcessVerticalLine, CSprite::{InitSpriteBuffer, FlushSpriteBuffer, CalcScreenCoors, RenderBufferedOneXLUSprite_Rotate_Aspect},
  CPointLights::ProcessVerticalLineUsingCache (en los pares de RenderFogEffect) y fpRenderStateSet. Se parchea la entrada del EXE y el cuerpo de la DLL (Resolve) con el mismo falso.
- GetLightMultiplier no tiene llamadores en el EXE: el banco la llama directo con fixtures. Comprobado el 17/10/2026 con un barrido de todo `.text` del EXE (gta_sa.exe 1.0 US): ningún `call`/`jmp` rel32 apunta a 0x6FFE70 y la dirección no aparece como puntero de 4 bytes en ninguna sección (ni vtables ni tablas), así que es código muerto o solo alcanzable de forma indirecta calculada; el EXE nunca la ejecuta por llamada directa, de ahí que el banco la valide por llamada directa y que el hook sea inocuo en el juego normal.

## Evidencia sin juego (offline/)
`make_image.py` + `reloc_gen.py` generan la imagen del EXE y las reubicaciones; `build_a.sh`/`build_b.sh` (usan X87Math.h/.cpp del arbol: `gen_inc.sh`) enlazan el C++ real de X87Math.cpp + anexo y
PointLights (core4.inc) contra los bytes del EXE reubicados (desplazamiento 0x2FC00000 porque 0x400000 no esta libre en este equipo), con falsos identicos.
Resultados al entregar (registros muertos incluidos): GenerateLights, GetLightMultiplier y ProcessVerticalLineUsingCache 500 000 casos cada una con estres 1, 12 y 100; RenderFogEffect 100 000 casos con
las fixtures del banco (cobertura por caso: CalcScreenCoors 84 893, ProcessVerticalLine 68 851, sprites 84 031), 200 000 con estres 12/40/100 y 1 000 000 con mutaciones de 1 a 8 campos
denormales/NaN/inf, todo con cero diferencias de memoria, retorno, traza de colaboradores en bits y estado x87 completo.

## Ejecucion
```
python -B reversing/Dependencies22A/build_probe.py
python -B reversing/Dependencies22A/run_local.py
python -B reversing/Dependencies22A/run_local.py --analyze <comparisons.jsonl>
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> Dependencies22A
```
Antes: aplicar `g22a.patch`, anexar `g22a-x87.cpp.txt`/`g22a-x87.h.txt` al final de `X87Math.cpp`/`X87Math.h` y compilar el proyecto.
