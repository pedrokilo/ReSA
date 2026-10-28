<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# AutoRender75: CAutomobile::Render (EXE 0x006A2B10, virtual)

**Estado: APROBADO dentro del juego (27/10/2026, worktree `D:\Original\wy-5`, HEAD 98ff78aaa + `source.patch`). 16384 pares, 0 diferencias. Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-autorender75/20261027-143247-254745` (corrida previa idéntica: `20261027-135521-285223`).** Plantillas: `reversing/AutomobileWrappers60` (caso Render del lote 60, del que deriva), `reversing/CrosshairRender74`, `reversing/FxWood73`. Contrato: `reversing/_handoff/BRIEF.md` y `D:\Original\BRIEF_LOTE75.md`.
esultados-autorender7561027-143247-254745` (corrida final) y `20261027-135521-285223` (corrida previa idéntica).** Plantillas: `reversing/AutomobileWrappers60` (caso Render del lote 60, del que deriva), `reversing/CrosshairRender74`, `reversing/FxWood73`. Contrato: `reversing/_handoff/BRIEF.md` y `D:\Original\BRIEF_LOTE75.md`.

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `CAutomobile::Render()` | 0x6A2B10 | `CAutomobile` / `Render` / V (`RH_ScopedVMTInstall`, fila existente de `docs/hooks.csv`: `CAutomobile,Render,0x00000000,1,0,1,V`) | `GetFirstObject` 0x7348C0, `CVehicle::Render` 0x6D0E60, `CEntity::GetColModel` 0x535300, `CVehicle::GetVehicleAppearance` 0x6D1080, `CVehicle::DoHeadLightBeam` 0x6E0E20, `RwMatrixTranslate` 0x7F2450, `RwFrameUpdateObjects` 0x7F0910, `renderCallBack` de los átomos y `fpRenderStateSet/Get` (`RwEngineInstance` +0x20/+0x24); falsos idénticos en la dirección del EXE y en el cuerpo de la DLL |

Quién valida a cada colaborador: `CVehicle::Render`, `GetVehicleAppearance`, `DoHeadLightBeam`, `GetColModel` y `GetFirstObject` tienen hook propio en `docs/hooks.csv` (y banco en `reversing/validation-registry.json`); `RwMatrixTranslate`, `RwFrameUpdateObjects`, `RwRenderStateGet/Set` y el `renderCallBack` del átomo son RenderWare (sin cuerpo propio en la DLL ni banco). Este banco NO les da crédito.

## Hallazgo (qué seguía llamando al EXE)

`Automobile.cpp` ya tenía `RH_ScopedVMTInstall(Render, 0x6A2B10)` y un cuerpo C++ completo (lote 60, banco `AutomobileWrappers60`, 4096 pares). `lista-por-dificultad.csv` / `classification.csv` conservan la línea antigua (`plugin::CallMethod<0x6A2B10, CAutomobile*>(this)`, `Automobile.cpp:7077`): es una fila obsoleta. El cuerpo actual no tiene `plugin::Call*` ni direcciones del EXE para código propio; solo llama a: `CTimer::GetTimeInMS`, `CCheat::IsActive`, `RwRenderStateGet/Set`, `GetFirstObject` + `renderCallBack`, `CVehicle::Render`, `CEntity::GetColModel`, `RwMatrixTranslate`, `RwFrameUpdateObjects`, `GetVehicleAppearance`, `DoHeadLightBeam` (`grep` de `plugin::Call|CallMethod<` en `Automobile.cpp`: ninguna dentro de `Render`).

Lo que SÍ faltaba, y este lote corrige: la aritmética de las orugas del Rhino (0x6A2CA1..0x6A2E68) estaba en `float` SSE. El EXE la hace en x87 a 24 bits con exponente extendido: los valores de registro (`ratio`, `v`, `u`, `u*inicio.z`, `v*fin.x/y`) no se redondean a float ni se desnormalizan, y solo los guardados en memoria (`fstp dword`: `sx`, `sy`, `ez`, `x`, `y`, `z`, `move`) sí. Con desnormales, valores grandes o NaN con carga, SSE (con FTZ si el MXCSR del juego lo tiene) da resultados distintos. Ahora esa parte está en `AutoRender75RhinoMoveCpp` y `AutoRender75NegateCpp` (bloque «Lote 75» de `X87Math.cpp/.h`, compilado /arch:IA32, `volatile double` para los registros y `volatile float` para los guardados).

## Fuente (`source.patch`)

- `Automobile.cpp`: el bucle del Rhino llama a `AutoRender75RhinoMoveCpp(pista, &linea0, &muelle0, línea, &nodo->modelling.pos, &move)` y `AutoRender75NegateCpp(&move)`; los operandos van por puntero (el EXE los carga con `fld dword`, que silencia los NaN de señal).
- `X87Math.cpp/.h`: bloque «Lote 75» añadido al final.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido (cuerpo de la tabla virtual). Por caso: objeto `CAutomobile` completo (0x988 bytes con guardas; solo cambia +0x4D0), posiciones de los nodos 2..7, arenas auxiliares, traza de eventos en orden y en bits (`sget/sset`, `gfo`, `rcb`, `vrender`, `col`, `xlate`, `upd`, `appear`, `beam`), ABI (ESP, EBX/ESI/EDI/EBP, DF), ruta, x87/MXCSR, globales (temporizador 0xB7CB84, truco 0x96914B). EAX/ECX/EDX solo se registran (ningún llamador del EXE los lee). Oráculo Python independiente (`fixture_oracle.py`, fracciones exactas, reglas de NaN/infinito de la x87) que reproduce cada lado.

Familias (1024 casos cada una, 16 familias, 16384 pares): Rhino finito con y sin truco; líneas diminutas/desnormales; muelle = línea (ratio 0) con pistas diminutas; pistas alrededor de 1 (ratio 1); línea0 cero/diminuta/enorme (ratio inf/indefinido); un origen infinito; un origen NaN no canónico (de señal y silencioso, con carga); ceros con signo; nodos y líneas enormes/diminutos; desbordamiento con cancelación (el registro excede el rango float y el guardado final es finito); palabras crudas finitas; cancelación exacta (x - pos = +0); y tres familias no Rhino (441, -2, 539, otros) con basura en las zonas del Rhino.

## Lo que NO prueba este banco

- El interior de los colaboradores (`CVehicle::Render`, `DoHeadLightBeam`, `GetColModel`, `GetVehicleAppearance`, `GetFirstObject`) ni RenderWare real (`RwMatrixTranslate`, `RwFrameUpdateObjects`, `RwRenderStateSet/Get`, el `renderCallBack`): son falsos idénticos que solo registran sus argumentos.
- Dos NaN de origen distinto en una misma operación (las fixtures usan un único origen no finito por caso; `nan_conflict` = 0 es compuerta).
- Vehículo sin matriz (el EXE se estrella) ni punteros de nodos nulos fuera de los nodos 3 y 6.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh AutoRender75
python -B reversing/AutoRender75/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl
python -I reversing/AutoRender75/fixture_oracle.py --selftest
python -I reversing/AutoRender75/fixture_oracle.py <carpetaEvidencia>/comparisons.jsonl
python -B reversing/AutoRender75/register_hook.py "<carpetaEvidencia>"      # solo el coordinador
```

## Negativa

Mutación de producción en `AutoRender75RhinoMoveCpp` (restaurada después): `volatile double usz = (double)(float)(u * (double)line[2]);` (el producto en registro se redondea a float, como haría SSE). **FALLO por diferencia de resultado: 1199 de 16384 pares difieren, ninguna caída** (informe `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-autorender75/20261027-184436-127977`). Fuente restaurada y banco repetido hasta APROBADO (`git diff` solo muestra `Automobile.cpp`, `X87Math.cpp/.h`).

## Registro

Render ya figura en `validation-registry.json` y `approvals.json` con la evidencia del lote 60 (AutomobileWrappers60, 4096 pares). Este banco la sustituye por 16384 pares (el coordinador usa refresh_bank/register; `register_hook.py` es idempotente y no reemplaza entradas existentes). Fila de `docs/hooks.csv`: ya existe (`CAutomobile,Render,0x00000000,1,0,1,V`); no hay fila nueva.
