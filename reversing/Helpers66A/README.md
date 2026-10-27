<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers66A: PlayerWantsToAttack 0x60CC50, EvaluateNeighbouringTarget 0x60D1C0, HasClearLineOfSight 0x621B10, AddPunchImpact 0x49F670 y RsSavePresetView 0x619FA0

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wu-1`, HEAD 5765c0b5f + `source.patch`). 20480 pares (5 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados, `fp_difference` vacío, cobertura por familia (16 x 256 por función) y de ramas (`coverage.json`, medida con `fixture_selfcheck.py`) y el oráculo (`fixture_oracle.py`) coincide con el lado EXE de los 20480 pares.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-helpers66a61021-111947-901294`. Plantilla: `reversing/Helpers65D` (a su vez `CarCtrlRest63B`); contrato: `reversing/_handoff/BRIEF.md`, `BRIEF28.md`, `BRIEF60.md`.

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `CPlayerPed::PlayerWantsToAttack()` | 0x60CC50 | `CPlayerPed` / `PlayerWantsToAttack` / S (`RH_ScopedInstall`) | `CPedGroupMembership::CountMembersExcludingLeader` 0x5F6AA0, `CPedGroupIntelligence::ReportAllBarScriptTasksFinished` 0x5F8780, `CTagManager::IsTag` 0x49CCE0, `CObject::CanBeTargetted` 0x59F320, `CPlayerPed::FindPedToAttack` 0x60C5F0, `CPedGroup::PlayerGaveCommand_Attack` 0x5F7CC0 (todos con hook) |
| `CPlayerPed::EvaluateNeighbouringTarget(CEntity*, CEntity**, float*, float, float, bool)` | 0x60D1C0 | `CPlayerPed` / `EvaluateNeighbouringTarget` / S | `CPlayerPed::DoesTargetHaveToBeBroken` 0x60C0C0, `CGeneral::GetATanOfXY` 0x53CC70 (con hook) |
| `CTaskSimpleGangDriveBy::HasClearLineOfSight(CPed*, const CVector*)` (helper sin símbolo 0x621B10) | 0x621B10 | `CTaskSimpleGangDriveBy` / `HasClearLineOfSight` / S | `CWorld::GetIsLineOfSightClear` 0x56A490 (con hook), `rand` del EXE 0x821B1E (sin hook; la DLL lo llama por dirección) |
| `Fx_c::AddPunchImpact(const CVector&, const CVector&, int32)` | 0x49F670 | `Fx_c` / `AddPunchImpact` / S | `FxSystem_c::AddParticle(v3d)` 0x4AA440 (con hook); el constructor `FxPrtMult_c` 0x4AB290 corre de verdad |
| `RsSavePresetView(RwCamera*)` (ayudante usercall 0x619910 incluido) | 0x619FA0 | `Rs` / `RsSavePresetView` / S (`RH_ScopedGlobalInstall`) | `psPathnameCreate` 0x745470, `psPathnameDestroy` 0x7454E0, `psErrorMessage` 0x7451D0 (con hook); `RwFrameGetLTM` 0x7F0990, `RwV3dNormalize` 0x7ED9B0, `RsLoadPresetViews` 0x619D60 (sin cuerpo propio: la DLL llama a la dirección del EXE); `RsSetPresetView` 0x619780 (reconstruida sin hook); y, durante la llamada, las entradas `fopen` (+4), `fclose` (+8), `fputs` (+0x18), `sprintf` (+0xF0) y `strlen` (+0x120) de la tabla de RenderWare (`RwEngineInstance` + 0xC4 / + 0xF0 / + 0x120) |

Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CPlayerPed,PlayerWantsToAttack,0x0060CC50,1,0,1,S`, `CPlayerPed,EvaluateNeighbouringTarget,0x0060D1C0,1,0,1,S`,
`CTaskSimpleGangDriveBy,HasClearLineOfSight,0x00621B10,1,0,1,S`, `Fx_c,AddPunchImpact,0x0049F670,1,0,1,S`, `Rs,RsSavePresetView,0x00619FA0,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh Helpers66A        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/Helpers66A/build_probe.py
python -B reversing/Helpers66A/run_local.py
python -B reversing/Helpers66A/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/Helpers66A/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/Helpers66A/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual) con cl, comprueba las constantes de arena contra la suma a mano y calcula la cobertura prevista (imprime «ok»)
python -B reversing/Helpers66A/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66a/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `helpers66a-runtime.start`; salida `helpers66a-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas cinco funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

- `Entity/Ped/PlayerPed.cpp`: `PlayerWantsToAttack` y `EvaluateNeighbouringTarget` en C++ + `RH_ScopedInstall` (partieron de los borradores del lote 27 del árbol principal, corregidos: ver «Defectos»).
- `X87Math.cpp/.h` (bloque «Lote 66» al final, prefijo `Hp66`): `Hp66NeighbourInRangeCpp` (distancia `(dz² + dy²) + dx²` y `fsqrt` a 24 bits con exponente extendido), `Hp66NeighbourAngleCpp` (las dos restas a float, `GetATanOfXY` y la resta de la compensación en un solo
  redondeo) y `Hp66PresetViewAnglesCpp` (el ayudante usercall 0x619910: matriz global del marco, `acos` del CRT del EXE por `ExeAcosDoubleCpp`, normalización por `RwV3dNormalize`).
- `Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp/.h`: `HasClearLineOfSight` (0x621B10, nombre elegido por el desensamblado: cachea la visibilidad del ped al punto de mira en `m_nLOSBlocked`/`m_nLOSCheckTime`) en C++ + `RH_ScopedInstall`; `AITarget` deja de llamarla por dirección.
- `Fx/Fx.cpp`: `AddPunchImpact` (dos partículas de humo del `g_fx` global, la segunda con tiempo 0.05f) en C++ + `RH_ScopedInstall`.
- `app/platform/preset_view.cpp`, `platform.h`, `platform.cpp`: `RsSavePresetView` (de `void ()` a `bool (RwCamera*)`: la camara es su argumento de pila) en C++ + `RH_ScopedGlobalInstall`; `RsLoadPresetViews` pasa de `void` a `bool` (el EXE devuelve `NumPresetViews != 0` en EAX) y sigue siendo una llamada por dirección.

## Revisión contra el EXE (re-desensamblado y contrastado)

**0x60CC50 (PlayerWantsToAttack).** `grupo = ms_groups[[datosJugador+0x38] * 0x2D4]` sin acotar; `CountMembersExcludingLeader(grupo+8) < 1` (con signo) -> sale; byte `grupo+4` = 0 -> sale; `ReportAllBarScriptTasksFinished(grupo+0x30)`; objetivo = `[ped+0x71C]` o, si la bandera de ratón (0xB6EC2E, cualquier byte) y no hay objetivo,
`[ped+0x79C]`; tipo (`byte[+0x36] & 7`): 3 = ped -> ataque directo; si no `IsTag` (cdecl; `test al,al`) -> sale, y tipo 4 con `CanBeTargetted` -> sale; sin ped, `FindPedToAttack`; nulo -> sale; `PlayerGaveCommand_Attack(grupo, this, ped)`. No define EAX; el único llamador (`0x60D5A0`, un `jmp`, llamado desde 0x685A7A) no lo lee.

**0x60D1C0 (EvaluateNeighbouringTarget).** `ret 18h`. Distancia = `sqrt((dz² + dy²) + dx²)` (d = objetivo - jugador, x87 a 24 bits con exponente extendido, `fcomp [maxDist]; test ah,41h; jp`: NaN sale); `DoesTargetHaveToBeBroken(objetivo, &armas[ranura con signo])` (`movsx`, sin acotar);
rumbo = `GetATanOfXY(objetivo.x - cam.x, objetivo.y - cam.y)` (cada resta redondeada a float) menos `arg4` sin redondear antes; bucles `> pi: -= 2 pi` y `< -pi: += 2 pi`; `|rumbo| < 0.87266463 (0x86D1D0)` o sale (NaN sale); prioridad: `arg5` (byte bajo) derecha: `rumbo > 0 ? -rumbo : -100000.0 (0x86D1E8)`;
izquierda: `rumbo < 0 ? rumbo : -100000.0`; si `prioridad > *salida` (ordenada) guarda `*outTarget = objetivo` y `*outPrio`. Llamadores 0x60E75A, 0x60E83B, 0x60E922: no leen EAX/ECX/EDX (`residue_scan.py`).

**0x621B10 (HasClearLineOfSight).** `ret 8`, `ecx = tarea`. Si `now (0xB7CB84) <= [tarea+0x18]` (SIN signo) salta a la salida; si no: `[+0x1C] = 0`; con objetivo (`[+0x34]`): origen = (x tal cual, y cargado/guardado por la x87 (un NaN senalizante sale silenciado), z + 0.5f (0x858B8C));
`CWorld::pIgnoreEntity = objetivo`; `GetIsLineOfSightClear(origen, *punto, 1, 0, 0, 0, 0, 1, 0)` (cdecl, 9 dwords); AL = 0 -> `[+0x1C] = 1`; `pIgnoreEntity = 0`. Siempre: `[+0x18] = now - ftol(((rand() & 0xFFFF) * 2^-15) * -500.0) + 1750` (rand del EXE, 0x821B1E; `_ftol2`: 32 bits bajos del entero de 64 bits;
`now` se relee). Devuelve `EAX = ([+0x1C] == 0)` (`xor eax,eax; sete al`). Único llamador 0x62780F (`CTaskSimpleGangDriveBy::AITarget`): lee AL (`test al,al; mov [esi+0Bh],al`); el `or dl,0FFh` siguiente sobrescribe DL antes de usarlo, así que EDX/ECX no se leen.

**0x49F670 (AddPunchImpact).** `ret 0Ch`. `d = camara - pos` (cámara = `[0xB6F03C] ? +0x30 : 0xB6F02C`), `(dx² + dz²) + dy²` > 625.0 (0x85A6E8; NaN no sale) sale; `FxPrtMult_c(1, 1, 1, 0.4, 0.1, 0, 0.1)` (0x4AB290); dos `AddParticle(pos, vel, t, mult, -1.0, 1.2, 0.6, false)` con t = 0 y 0.05f sobre `[0xA9AE24]` (relido cada vez).
Ni `this` ni el tercer argumento se usan. Llamadores 0x61D3DF y 0x61D5DF: no leen registros.

**0x619FA0 (RsSavePresetView).** cdecl `(RwCamera*)`. `fs = RwEngineInstance + 0xC4` (nunca nulo); `ruta = psPathnameCreate("./views.txt")`; `f = fs->fopen(ruta, "a")`; `psPathnameDestroy(ruta)`; si `f` es nulo: `RsErrorMessage("Cannot open preset view file")` (el EXE lo tiene expandido: aviso de teclado `[0xC17060](0x1D, &0x420)` si `[0xC1705C]`
y `psErrorMessage`) y devuelve 0. Si no: ayudante usercall 0x619910 (EAX = camara, EDI = &pos, EBX = &rotY, pila = &rotX) con `ltm = RwFrameGetLTM(camara->parent)`: `pos = ltm.pos`; `d = (0 * at.x + 1 * at.y) + 0 * at.z` (guardada como float, `> 1` -> 1, el float releido `< -1` -> -1, NaN pasa);
`rotX = 90 - acos(d) * 57.29578`; `t = at - Y * d` (productos y restas a float) normalizado en el sitio por `RwV3dNormalize`; `rotY = acos(clamp(Z . t)) * 57.29578` (recorte sobre el valor sin redondear), negativo si `X . t < 0`; `rwsprintf(buf, "%0.6f %0.6f %0.6f  %0.6f %0.6f  %0.6f %0.6f %s%d\n", pos, rotX, rotY, near, far, "View", NumPresetViews)`
(siete `double`); `rwstrlen(buf)` (resultado sin uso); `fputs(buf, f) <= 0` -> `RsErrorMessage("Cannot write to preset view file")` y devuelve 0; si no `RsLoadPresetViews()` (EAX completo) y, si no es 0, `RsSetPresetView(camara, NumPresetViews - 1)` (relee); `fclose(f)`; devuelve lo que devolvió `RsLoadPresetViews`. Sin llamadores en el EXE (ni `call` ni punteros);
el ayudante 0x619910 sólo lo llama esta función (se absorbe en `Hp66PresetViewAnglesCpp`).

## Hallazgos (discrepancias halladas y corregidas)

1. **`EvaluateNeighbouringTarget`, borrador del lote 27: `GetActiveWeapon()`.** El EXE indexa las armas con la ranura **con signo y sin acotar** (`movsx eax, byte [this+0x718]; imul eax, eax, 0x1C`); `GetActiveWeapon()` pasa por `std::array::operator[]` con un índice sin signo y comprobado: con una ranura fuera de 0..12 el juego se cerró
   (`0xC0000409`, primera corrida, caso 3 de la segunda función, `std::array<CWeapon,13>::operator[]`). Corregido con `m_aWeapons.data()[static_cast<int8>(m_nActiveWeaponSlot)]`. (`EvaluateTarget` 0x60D020 conserva `GetActiveWeapon()` y tiene el mismo defecto latente con una ranura inválida; fuera de alcance.)
2. **Aritmética del borrador en float SSE.** El juego corre con MXCSR FTZ: la distancia `(dz² + dy²) + dx²`, las dos restas de la cámara y la resta de la compensación en SSE vacían los subnormales y desbordan float donde la x87 del EXE (24 bits, exponente extendido) no; pasan a `Hp66NeighbourInRangeCpp`/`Hp66NeighbourAngleCpp`
   (X87Math.cpp). El banco las ejercita con familias de subnormales y enormes.
3. `HasClearLineOfSight` y `RsSavePresetView` son nuevas (sólo había una llamada por dirección). Detalles que sólo salen del desensamblado: comparación SIN signo del temporizador, `y` silenciado por la x87 al construir el origen, `_ftol2` del retardo (32 bits bajos), `rand` del EXE, `strlen` sin uso y `RsErrorMessage` expandida en línea.
4. Sin discrepancias en `PlayerWantsToAttack` ni `AddPunchImpact` respecto al desensamblado (la segunda difiere de `AddTyreBurst` en la constante 0.1 frente a 0.12 y en dos vueltas frente a cuatro).
5. Defectos del banco (no de las funciones) corregidos en el camino: `static_assert` de arenas mal sumadas (`kPedArena` 0x820, `kCamArena` 0x108: ver la autoprueba `fixture_selfcheck.py`), los macros `near`/`far` de Windows en los campos de las fixtures, y el modelo del oráculo (el falso de `RwV3dNormalize` multiplica con SSE y MXCSR.FTZ vacía los subnormales).

**Banco dependiente:** `reversing/Dependencies21D` (AITarget, 0x627600) sustituía 0x621B10 sólo en la dirección del EXE porque la DLL lo llamaba por dirección; ahora `AITarget` llama a `HasClearLineOfSight` directamente (hook activo), así que ese banco parchea también la entrada de la DLL (una línea en su `LiveComparison.hpp`) y se repitió.

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con SOLO ese hook encendido (los colaboradores con hook deben ser saltos a la DLL, los sin hook código del EXE). Falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto
ANTES de parchear), que registran sus argumentos en orden. Por función se compara toda la memoria que cambia en las arenas (con guardas de 0x40), la traza de eventos, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado con TOP/etiquetas/ST válidos), ruta, `collaborators_dll`,
un FNV de todas las regiones globales tocadas (grupos, cámara, bandera del ratón, temporizador, `pIgnoreEntity`, humo, teclado, `NumPresetViews`, tablas de RenderWare) y estado restaurado tras quitar los parches. ECX/EDX de salida sólo se registran; EAX sólo donde un llamador lo lee (AL de
`HasClearLineOfSight`, que lee 0x62780F con `test al,al`; `RsSavePresetView` no tiene llamadores, su AL se compara como valor de retorno).

- **0 PlayerWantsToAttack**: grupo 0..7 (byte `+4` del grupo del jugador escrito en `ms_groups`), recuento de miembros ≤ 0 y > 0 (con signo), bandera del grupo (cualquier byte), bandera del ratón en tercera persona (cualquier byte), objetivo fijado y del ratón nulos o de la arena con byte de
  tipo (3 = ped, 4 = objeto, otros) y bits altos de basura, `IsTag`/`CanBeTargetted` con basura en los bits altos de EAX, `FindPedToAttack` nulo / de la arena / puntero bruto. EAX de salida no se compara (indefinido; ningún llamador lo lee).
- **1 EvaluateNeighbouringTarget**: posición del ped, del objetivo y de la cámara con o sin matriz (la posición no usada es ruido), distancias exactas y alrededor de `maxDist`, NaN, subnormales y enormes (los cuadrados desbordan float pero no la x87), `GetATanOfXY` falso con valores que obligan a los bucles de `±2π`,
  fuera de 50 grados, exactamente en fronteras, NaN en rumbo o compensación, prioridad inicial alrededor de la nueva y NaN, `arg5` con bits altos, ranura de arma de -128 a 127. **No se prueban rumbos ±inf ni gigantescos**: los bucles de normalización del propio EXE no terminan.
- **2 HasClearLineOfSight**: temporizador contra `m_nLOSCheckTime` (comparación SIN signo, incluidos 0x7FFFFFFF/0x80000000), byte `m_nLOSBlocked` cualquiera, objetivo nulo / de la arena / bruto, posición del ped con matriz o sin ella con NaN (sNaN en y: la x87 lo silencia; z + 0.5f), infinitos, ceros y subnormales,
  resultado de la línea con basura en los bits altos, `rand` del EXE con cualquier valor (se enmascara a 16 bits), `pIgnoreEntity` inicial cualquiera (debe quedar a 0 si hubo línea; si no, intacto).
- **3 AddPunchImpact**: cámara con y sin matriz, posición cerca/lejos/alrededor de 625 (± ulps), NaN, infinitos, subnormales, `this` cualquiera (no se usa), `num` cualquiera (no se usa); dos `AddParticle` con el humo global recargado.
- **4 RsSavePresetView**: `fopen` nulo / ficha pequeña / puntero alto, `fputs` ≤ 0 y > 0, `RsLoadPresetViews` 0/1 con `NumPresetViews` antes y después, `psPathnameCreate` nulo, teclado usado (cualquier dword) con manejador falso, matriz con `at` normal, fuera de [-1, 1] por redondeo, ±1, NaN, ±inf,
  subnormales, `RwV3dNormalize` falso que escala por un factor del caso (para forzar los recortes de `acos`), plano cercano y lejano cualesquiera (incluido NaN/inf), posición con NaN/inf (sólo se copia).
  El `sprintf` falso registra las siete `double` en bits, el nombre y el entero; el texto que escribe depende sólo de las palabras exactas (x, y, z, near, far, entero) para que el oráculo lo reproduzca.

## Oráculo (`fixture_oracle.py`)

Modelo en Python escrito desde el desensamblado (no desde el C++): aplica a la fila `input` de cada caso y devuelve la traza de eventos de los falsos, AL y los cambios predecibles de las arenas. `run_local.py` lo compara con el lado ORIGINAL (EXE) de cada par; como original == candidato
ya se exige aparte, el oráculo valida además los propios fixtures y la lectura del desensamblado. La aritmética es la x87 de 24 bits (`r24`); sólo `acos` (rotX/rotY del sprintf) se compara con 4 ulps de float. Autoprueba: `python -I fixture_oracle.py`.

## Suposiciones y riesgos

- `rand()` del EXE (0x821B1E) se sustituye por un falso en ambos lados durante la llamada (avanzaría la semilla del juego de forma distinta en cada pasada).
- Las tablas de funciones de RenderWare (`RwEngineInstance` + 0xC4: fopen/fclose/fputs; + 0xF0: sprintf; + 0x120: strlen) se sobrescriben sólo durante la llamada y se restauran antes de comprobar las regiones.
- `RsKeyboardEventHandler`/`RsErrorMessage` corren de verdad (el EXE tiene `RsErrorMessage` expandida en línea en la rama de fallo de fopen): el manejador de teclado de `RsGlobal` se sustituye por uno falso.
- Los índices de grupo > 7 no se prueban (leerían memoria ajena a `ms_groups` sin control).
