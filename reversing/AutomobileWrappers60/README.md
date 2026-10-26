<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco AutomobileWrappers60: HasCarStoppedBecauseOfLight (0x44D520), Render (0x6A2B10) y UpdateWheelMatrix (0x6AA290)

**Estado: PREPARADO (19/10/2026), sin compilar el proyecto y sin ejecutar GTA.** Tres envolturas al EXE de `CAutomobile` reconstruidas en C++ (`source/game_sa/Entity/Vehicle/Automobile.cpp`): 3 x 4096 = **12.288 pares** original (EXE, gancho apagado) / candidato (DLL, gancho encendido).

Plantilla: `reversing/ModifySpeed27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción: el banco usa ensamblador en `Gate`, `SaveFP`, etc.). Lecciones del lote 30 aplicadas (ver «Lecciones»).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh AutomobileWrappers60      # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/AutomobileWrappers60/build_probe.py
python -B reversing/AutomobileWrappers60/run_local.py
python -B reversing/AutomobileWrappers60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(HasCarStoppedBecauseOfLight, 0x44D520)`, `RH_ScopedInstall(UpdateWheelMatrix, 0x6AA290)` y `RH_ScopedVMTInstall(Render, 0x6A2B10)` (parche `source.patch`, sólo `Automobile.cpp`; `git apply --check` limpio contra HEAD). `build_probe.py`/`run_local.py` fijan `runtime_runner.GAME = E:/JUEGOS/GTA San Andreas - CVehicle Test`. Marcador/registro del juego: `automobilewrappers60-runtime.start` / `.jsonl`.

Pruebas SIN juego (todas hechas el 19/10/2026; ver «Revisión»):
```
cl /nologo /EHsc /O2 /std:c++20 /Fe:dump_fixtures.exe dump_fixtures.cpp        # x86 (vcvars32); volcado de las 12.288 filas `input`
dump_fixtures.exe > rows.jsonl ; python -I fixture_oracle.py --selftest rows.jsonl    # cobertura mínima del oráculo
python -I emu_check.py rows.jsonl                                                  # BYTES REALES del EXE en unicorn (pip install unicorn) contra el oráculo
python -I mirror/mirror_check.py <Automobile.cpp> rows.jsonl -Od                   # cuerpos LITERALES del C++ contra tipos mínimos (cl x86), -Od y -O2
```

## Qué se compara

Cada lado ejecuta la entrada del EXE (`0x44D520`, `0x6A2B10`, `0x6AA290`) con `ECX = this`, EAX/EDX/pila sembrados, EBX/ESI/EDI/EBP sembrados y x87 con control de precisión de 24 bits (`_controlfp`, como el juego). Por lado y en bits (original == candidato == oráculo):

- **Objeto `CAutomobile` completo (0x988 bytes)** con guardas de 0x40 bytes: sólo `Render` cambia el dword +0x4D0 (`m_nTimeTillWeNeedThisCar = ahora + 3000`); las otras dos no escriben el objeto (`car_diff`). Arenas auxiliares (nodos, enlaces, marcos, átomos, modelo/datos de colisión, información de modelo, handling, matriz) con guardas: `arenas_unchanged`.
- **Estado global restaurado**: `ThePaths` (0x96F854..0x96FEF4, sólo las 4 tablas usadas), temporizador (0xB7CB84), trucos (0x96914B, 0x969152), contador de fotogramas (0xB7CB4C), `ms_modelInfoPtrs[modelo]`, `g_fx.m_Sand/m_SmokeHuge/m_WaterSplash`, punteros `fpRenderStateSet/Get` del motor RW, 31 entradas de código vigiladas (16 bytes), gancho RH y x87/MXCSR.
- **ABI**: ESP (`ret 0` / `ret 8`, `popBytes = 0`), EBX/ESI/EDI/EBP, DF = 0, x87 normalizado (FCW completa, TOP, etiquetas, ST válidos; ninguno debe quedar), MXCSR sin banderas. `kCompareDeadX87Registers = false` y `kCompareVolatileRegisters = false` a propósito (ver «Registros»).
- **Colaboradores sustituidos por falsos idénticos** en la dirección del EXE y en el cuerpo de la DLL (traza `trace` comparada en orden y en bits):

| Función | EXE | Cuerpo de la DLL |
|---|---|---|
| `Render` | `0x7348C0` GetFirstObject, `0x6D0E60` CVehicle::Render, `0x535300` GetColModel, `0x7F2450` RwMatrixTranslate, `0x7F0910` RwFrameUpdateObjects, `0x6D1080` GetVehicleAppearance, `0x6E0E20` DoHeadLightBeam + punteros del motor `RwEngineInstance+0x20/+0x24` + `renderCallBack` (+0x48) de los átomos falsos | `GetFirstObject`, `CVehicle::Render` (cuerpo virtual por `Virtual::GetHookOurAddress`), `CEntity::GetColModel`, `CVehicle::GetVehicleAppearance`, `CVehicle::DoHeadLightBeam` (RwMatrixTranslate/RwFrameUpdateObjects son envolturas que llaman a la dirección del EXE) |
| `UpdateWheelMatrix` | `0x59BD10` Attach, `0x59AF00` SetScale, `0x59B460` Rotate, `0x59B2C0` RotateY, `0x59BBB0` UpdateRW, `0x55E710` IsWater, `0x55E6F0` IsSand, `0x4AA440` FxSystem_c::AddParticle | `CMatrix::{Attach,SetScale(fff),Rotate,RotateY,UpdateRW}`, `SurfaceInfos_c::{IsWater,IsSand}`, `FxSystem_c::AddParticle(CVector)` |
| `HasCarStoppedBecauseOfLight` | ninguno | ninguno |

Se ejecutan de verdad (puros): `GetWheelStatus` (0x6C21B0), `GetWheelSize` (0x6A06F0, inline en la DLL), `FxPrtMult_c::FxPrtMult_c` (0x4AB290; los 7 floats llegan al falso de AddParticle y se comparan), `CMatrix::~CMatrix`, los operadores `CVector` (0x40FE30/0x40FE90) y `_CIasin` (0x821E70) frente a `CnvAsinFloatCpp`. Los falsos de `CMatrix` reproducen el efecto de la matriz real (Attach copia 0x40 bytes y fija puntero/propietario, SetScale deja diagonal y posición 0, UpdateRW copia right/forward/up/pos al RwMatrix) para que la posición final y el marco queden observables. `RwMatrixTranslate` suma el vector a `pos` con sumas float.

**Orden de las llamadas** registrado en `trace`: Render = `sget(30)`, `sset(30,1)`, [ruedas con `gfo` A, `gfo` B, `rcb(B)` por nodo 4,7,2,5,(3),(6) con el truco de coche invisible | `vrender`], [Rhino: `col` y 8 x (`xlate`, `upd`, `gfo`,`gfo`,`rcb`, `xlate`)], `sset(30,guardado)`, `appear`, `beam`(s). UpdateWheelMatrix = `attach`, `scale`, `rot` o `rot`+`roty`... (`roty` aproximado en seno/atan2/asin), [`iswater`,`issand`,`addp`], `upd`.

## Qué hacen las funciones (desensamblado del EXE canónico, SHA256 72ae59e4…ecc9b)

**0x44D520 HasCarStoppedBecauseOfLight** (`ret`, bool en AL): estado `[+0x36] >> 3` ∈ {2 SIMPLE, 3 PHYSICS}; `ThePaths.m_pPathNodes[área de +0x390] != 0` y `[área de +0x394] != 0`; recorre los `numLinks` (byte +0x18 & 0xF) enlaces del nodo `cur` (índice `m_wBaseLinkId + i`, `m_pNodeLinks[área cur]`) buscando el igual a `start (+0x394/+0x396)`; si `m_pPathNodes[navi>>10] != 0` y `m_pNaviNodes[navi>>10][navi&0x3FF].byte(0xC) & 3` → true. Si no, con `end (+0x398)` ≠ 0xFFFF y cargado, lo mismo desde el nodo `end` buscando `cur` (enlaces de `m_pNodeLinks[área end]`). Sin comprobaciones de rango (áreas de 16 bits, índices con `movsx`).

**0x6A2B10 Render**: `[+0x4D0] = [B7CB84] + 3000`; `RenderStateGet(30, &v)` con `v = 1` previo y `RenderStateSet(30, 1)`; truco 27 (coche invisible) activo → sólo las ruedas (nodos 4,7,2,5 y 3,6 si no son nulos, `GetFirstObject` dos veces por nodo y `renderCallBack(átomo2)` del átomo1), si no `CVehicle::Render`; modelo 432 (Rhino): 8 líneas de colisión 4..11 con `t = tracks[i] - (línea0 - muelle0)/línea0` (floats de +0x618,+0x61C,+0x624,+0x628,+0x630,+0x634,+0x63C,+0x640), `t = max(t,0)` salvo NaN, punto = `(1-t)*inicio + t*fin`, el nodo 7 (líneas 4..7) o 4 (8..11) se traslada a ese punto, se dibuja y se deshace con el vector multiplicado por -1; restaura el estado 30 y, salvo modelos 441/-2/432, bit 7 de +0x42C y apariencia 3 (heli), dibuja los haces de faros según +0x584 (bit 0 derecho, bit 1 izquierdo).

**0x6AA290 UpdateWheelMatrix(nodo, flags)** (`ret 8`, frame SEH): nodo 2..7 (tabla de saltos 0x6AAB34; fuera de rango o marco nulo → no hace nada); rueda y signo (5→0,-1; 2→2,+1; 6/7→1,-1; 3/4→3,+1), dirección (`m_fSteerAngle`/`m_f2ndSteerAngle` según los bits 0x20 y 0x40 de +0x38C; π/0 fijos), factor 0,6 del lado contrario y +π en las ruedas izquierdas, espejo (`flags & 2`), cosechadora 532 (nodos 6 y 3), modelo 513 (nodo 5); CMatrix local unida a `frame->modelling`; escala (`m_fWheelScale`, tamaños de rueda del modelo, anchura del handling, ×3 en el modelo 571, 1,0 en camiones monstruo y quads); giro (`m_wheelRotation`; rueda pinchada: `+ sin(ángulo)*0,3`; helicóptero: sólo Rotate(0,0,dirección)); eje macizo / hidráulico / camión monstruo → `RotateY(atan2(Δaltura*signo*-1, 2|y|))`; sobre el agua (truco 34 o modelo 539, compresión previa < 1,0, no ahogado) → `RotateY(-π/2 * signo)` y partículas (agua alterna WaterSplash/SmokeHuge según el fotograma, arena) salvo `flags & 4/8`; si no, inclinación `asin(-(z + alturaDelantera - tamaño/2) * signo)` limitada a [-1, 1] (eje invertido: signo negado) salvo `flags & 1` y ejes sin inclinación; posición final = posición + (x, y, z) y `UpdateRW`.

## Fixtures (4096 por función; `Fixtures.hpp`, una sola fuente de verdad)

`MakeFS/MakeFR/MakeFW` devuelven todo lo que el caso necesita y `JsonFS/JsonFR/JsonFW` escriben la fila `input`; el oráculo **no repite ninguna constante de arena** (sólo los valores `poison_link` = 0xA5A5 y `beyond_cl` = 0 viajan en la fila). Los tamaños de arena de `Core.hpp` se calculan con `constexpr` a partir de las mismas constantes (guarda 0x40 + sizeof + guarda 0x40, ventana de enlaces -16..79, 1024 enlaces de carretera por área). Cobertura calibrada con `dump_fixtures.exe` + `--selftest` (márgenes en `COVERAGE_MIN` de `fixture_oracle.py`):

- **S (HasCar...)**: 8 modos rotatorios (inicio enlazado, inicio de área descargada, área actual descargada, estado inválido, `end = 0xFFFF`, enlace desde `end`, `end` descargado, aleatorio); paleta de 6 áreas con nodos, enlaces, navi y bytes de semáforo explícitos; `numLinks` 0..15, `base` -3..40 (lee veneno fuera de 0..47), áreas descargadas (`m_pPathNodes = 0`), navi 0xFFFF (índice 1023, por eso 1024 enlaces por área). 1007 verdaderos / 3089 falsos (533 en la parte 1, 474 en la parte 2).
- **R (Render)**: Rhino 1294, mochila RC 441 (256) y modelo -2 (257); truco de coche invisible 2079 (nodos 3/6 nulos o no); `get_writes` y valor de `RenderStateGet`; tiempo con desbordamiento de +3000 (1790); pistas con NaN/inf (36) y `t < 0` (1145); faros izquierdo/derecho (787/808); apariencia heli.
- **W (UpdateWheelMatrix)**: nodos 2..7 (~550 cada uno), nodos inválidos (0, 1, 8, 9, -1: 552) y marco nulo (191); rama maciza 997, inclinación 656, agua 578 (efectos 308: agua 136, arena 115), ninguna 1122; rueda pinchada 756, helicóptero 408, cosechadora 136, modelo 513 (67), 571 (412), camión monstruo/quad 848, anchuras delanteras/traseras 348/670, ejes invertidos 139, recorte de asin ±1 (70/80), espejo (`flags & 2`) 794.
- Valores float: notables (±0, ±1, ±0,5, ±π, ±π/2, 0,1, 2, 10, 1e-6…) y uniformes; **como mucho un valor no finito por caso** (nunca denormales); semillas de pila, EAX y EDX por caso, ECX = `this`.

## Revisión contra el EXE (fase 2) y resultado

1. **Desensamblado leído línea a línea** (`dis.sh` 0x44D520..0x44D77C, 0x6A2B10..0x6A2EFC, 0x6AA290..0x6AAB2F + tabla de saltos 0x6AAB34 leída del PE) y contrastado con el C++ tras escribirlo (polaridad del truco 27, orden de las llamadas, `jp`/`jnp` tras `fnstsw` en las comparaciones de dirección, la lectura `movsx` del índice base, el marco SEH y el destructor de la `CMatrix` local).
2. **Borrador previo**: `HasCarStoppedBecauseOfLight` ya tenía un borrador sin compilar en el árbol principal (`git diff HEAD`); se comprobó contra el EXE (área del nodo del que se leen enlaces, orden de las dos pasadas, `0xFFFF` sólo en `end`, byte +0xC & 3) y se conservó con retoques de forma. Sin discrepancias.
3. **`emu_check.py`** ejecuta los bytes reales del EXE en un emulador x86 (unicorn) sobre las 12.288 filas con los mismos falsos que `Core.hpp`: **12.286/12.288 iguales al oráculo**; las 2 restantes (casos 595 y 3023 de UpdateWheelMatrix) son una peculiaridad de QEMU: `fsin(±inf)` deja el operando (el procesador real da el NaN indefinido, que es lo que modela el oráculo y hace la DLL). Se ejecutan en el emulador los cuerpos reales de `GetWheelSize`, `GetWheelStatus`, los operadores `CVector`, el constructor de `FxPrtMult_c`, el destructor de `CMatrix` y todo el x87 de las tres funciones (con 3 instrucciones de `fs:[0]` reescritas a una dirección absoluta sólo en la copia en memoria).
4. **`mirror/mirror_check.py`** compila con cl x86 los cuerpos **literales** del C++ (extraídos de `Automobile.cpp`, con `X87Math.cpp` de producción) contra tipos mínimos con la disposición del EXE (`static_assert(offsetof)`) y los mismos falsos, con x87 a 24 bits: **12.288/12.288 iguales al oráculo con `-Od` y con `-O2`**. Incluye la comprobación de que `TrigCompoundSinMulAdd` (fsin sin redondear antes del `*0,3`) reproduce el EXE sólo con el control de precisión de 24 bits del juego (con 53 bits difería 3 ulp en 5 casos de cancelación).

No se encontró ninguna discrepancia EXE/oráculo/C++ en la fase final; durante la escritura se corrigieron lecturas propias (polaridad del truco de coche invisible, ruedas 3/4 frente a 6/7, rama `bl` de la reducción 0,6) antes de las comprobaciones 3 y 4.

## Registros: ¿algún llamador lee EAX/ECX/EDX?

`python -I reversing/Dependencies25A/residue_scan.py <dir>`:

- **0x44D520**: un llamador directo, 0x6A382C (`CAutomobile::PlayHornIfNecessary`-zona 0x6A3820): `test al,al` → sólo se lee **AL** (el resultado); ECX/EDX no se leen. El banco registra EAX/ECX/EDX y exige `al` (`observed.al`).
- **0x6A2B10**: sin llamadores directos (se llama por la tabla virtual y `void`); se registran y no se exigen.
- **0x6AA290**: llamadores 0x6ABFDA.., 0x6C55BE.., 0x6C9675.. (CHeli/CPlane/CAutomobile PreRender). El escáner marca una lectura de ECX tras 0x6C96A0 que es el `push ecx` de 0x6C9CC7 (reserva de hueco que `fstp [esp]` sobrescribe justo después) y cadenas «EDX sin escribir» que acaban en llamadas posteriores sin lectura. Ninguno depende de EAX/ECX/EDX de salida. Sólo se registran (`original_residue`, `candidate_residue`).

## Suposiciones y riesgos del banco

- **Exponente extendido del x87**: el C++ de `Automobile.cpp` (opciones por defecto del proyecto, SSE y float por operación) no reproduce el rango de exponente extendido de los valores que el EXE deja en registro; sólo importa si una operación intermedia desborda/entra en denormales. Las fixtures evitan denormales y valores gigantes (un único no finito por caso), así que el banco no lo ejercita; no está cubierto.
- **Funciones trascendentes**: `fsin` (rueda pinchada), `fpatan` (eje macizo) y `_CIasin` (inclinación) se hacen con `TrigCompoundSinMulAdd`, `TrigAtan2Cpp` y `CnvAsinFloatCpp` de `X87Math` (compilado con `/O2 /Oi /arch:IA32 /fp:fast`). El oráculo Python acepta 2 ulp en esos valores; EXE == DLL se exige en bits. Si el banco falla sólo ahí, revisar el control de precisión que deja el llamador.
- **Rotaciones/escala con `CMatrix` real**: los falsos sustituyen `CMatrix::Rotate`/`RotateY`/`SetScale`/`Attach`/`UpdateRW`; la fidelidad de la `CMatrix` de la DLL frente al EXE (p. ej. `Rotate` genérico de `Matrix.cpp` frente a la versión x87 de `X87Math`) es asunto de su propio banco, no de éste.
- **Orden de los arrays**: `m_aCarNodes.data()[nodeIndex]` sin comprobar rango (nodos -1, 0, 1, 8 y 9 leen otros campos del objeto; el banco coloca el puntero del marco en esa ranura y comprueba que no se escribe nada).
- **Nombres RH**: `HookRoute("CAutomobile","Render",…)` busca un gancho virtual (`Symbol() == 'V'`) por nombre y clase; si `RH_ScopedVMTInstall(Render, 0x6A2B10)` no llega a registrarse en la categoría `CAutomobile`, la sonda lo dice en `begin.registered`.
- **Compilación**: `LiveComparison.hpp` se escribió sin compilar contra los encabezados reales (sólo se comprobó `Core.hpp` con cl /Zs y un `HookRoute`/`ProbeSupport` simulados). Direcciones de funciones miembro con `std::bit_cast` (miembros no virtuales de una sola herencia; para las sobrecargas `SetScale(fff)` y `AddParticle(CVector)` se usa `static_cast` explícito). `build_probe.py` lista las fuentes probadas, incluidos `Virtual.h/.cpp`.
- **Cobertura del Rhino**: las fixtures de Render fijan `colModel`, `colData` y 12 líneas falsas; el modelo de colisión real no interviene.
- Las arenas de 6 áreas de `ThePaths` ocupan ~90 KB en pila por caso (`CaseS`).

## Lecciones del lote 30 aplicadas

Arenas calculadas con `constexpr` en un único sitio (`Core.hpp`) y oráculo sin constantes de arena; `build_probe.py`/`run_local.py` con `runtime_runner.GAME`; `analyze` devuelve `None` sin registro; falsos en la dirección del EXE y en el cuerpo de la DLL resuelto con `Resolve` (saltos E9) **antes** de crear parches (`Addr`, `Body`); rand/sscanf no intervienen; EAX/ECX/EDX sólo registrados (salvo AL de 0x44D520); x87 fuera del banco con 2 ulp contra el oráculo y EXE == DLL estricto.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py` (plantilla), `README.md`, `source.patch` (sólo `Automobile.cpp`, respecto a HEAD), y las herramientas de revisión `dump_fixtures.cpp`, `emu_check.py`, `mirror/{mock_prelude.hpp,mirror_main.cpp,mirror_check.py}`.


## Depuración en el worktree de integración (19/10/2026): APROBADO
Defecto del banco: con el gancho de `SurfaceInfos_c::IsWater/IsSand` apagado, la entrada de la DLL salta al EXE y `Resolve` seguía el salto hasta fuera del ASI (`ready=false`, evento `error` con 0 pares). `Addr` sólo sigue saltos mientras caigan en el ASI. Sin cambios en producción.
