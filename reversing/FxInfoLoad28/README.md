<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FxInfoLoad28: FxInfoManager_c::Load (0x5C0B70)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175941-988715`: 4608 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fxinfoload28\20261018-175941-988715`; archivado en `reversing/CppTwelveLevel6Final/runs/FxInfoLoad28-20261018-175941-988715/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `FxInfoManager_c,Load,0x005C0B70,1,0,1,S` en `docs/hooks.csv`).

Plantillas: `reversing/FileLoaderLoad2dEffect27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` (el banco usa `__asm` en `Gate`/`SaveFP`, igual que los demás; los avisos «SUPERADO» sólo valen para producción).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FxInfoLoad28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FxInfoLoad28/build_probe.py
python -B reversing/FxInfoLoad28/run_local.py
python -B reversing/FxInfoLoad28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no terminó)
python -I reversing/FxInfoLoad28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
# comprobación de fixtures sin el juego (cl x86 en un directorio temporal):
#   cl /nologo /EHsc /std:c++20 /O2 /Ireversing/FxInfoLoad28 reversing/FxInfoLoad28/fixture_dump.cpp /Fe:fixture_dump.exe && fixture_dump.exe > casos.jsonl
python -I reversing/FxInfoLoad28/fixture_mirror.py casos.jsonl                                  # 0 inválidos + cobertura
python -I reversing/FxInfoLoad28/host_check.py [dirTemporal]                                    # cuerpo C++ real de Load + falsos de Core.hpp + oráculo, SIN el juego (0 diferencias)
python -B reversing/FxInfoLoad28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol con `RH_ScopedInstall(Load, 0x5C0B70)` en `FxInfoManager.cpp` (categoría `Fx`, clase `FxInfoManager_c`; `HookRoute` lo busca por esos nombres). Archivo de arranque en el clon: `fxinfoload28-runtime.start`; salida `fxinfoload28-runtime.jsonl`. 4608 pares. Fuente de producción: `source.patch` (parche contra HEAD de `FxInfoManager.cpp`, `git apply --check` verificado); el árbol de trabajo ya lo lleva aplicado. `docs/hooks.csv` no tiene (ni se toca aquí) la fila `FxInfoManager_c,Load,0x005C0B70,1,0,1,S`.

## Qué hace la función (desensamblado 0x5C0B70..0x5C16E6, `ret 8`; verificado dos veces)

`void FxInfoManager_c::Load(FILESTREAM file, int32 version)`, thiscall (ECX = this = `&m_nNumInfos`), marco de 0x188 bytes (`line[256]` y `field[128]`). Colaboradores: `ReadLine` = `0x4AB250` (`jmp 0x5389B0` = `CFileMgr::ReadLine` = `fgets`), sscanf del EXE `0x8220AD`, `FxMemoryPool_c::GetMem` `0x4A9CA0` (ECX = `0xA9AF2C`, thiscall, `ret 8`), `AddFxInfo` `0x4A7B00` (thiscall, `ret 4`), `FxInfo_c::Load` virtual (`call [vtable+4]`, `ret 8`).
1. `ReadLine` (cabecera descartada, **sin sscanf**) + `ReadLine` + `sscanf(line, "%s %d" (0x86A6AC), field, &m_nNumInfos)`. Si el sscanf no escribe (línea en blanco, sin número…), `m_nNumInfos` **conserva su valor previo**.
2. `m_MovementOffset = m_RenderOffset = 0xFF` (+8, +9), luego `m_pInfos = GetMem(n*4, 4)` (n*4 en 32 bits con envoltura; también con n ≤ 0).
3. Si n ≤ 0 (con signo) no hay bucle. Si no, para i = 0..n-1 (n **se relee de memoria** en cada vuelta): `flag = 1`; `ReadLine` + `sscanf(line, "%s" (0x85C714), field)`; el nombre se compara con la tabla de 32 nombres (`repe cmps` con la longitud incluyendo el NUL = igualdad exacta de cadena; tipos 0x1001.. / 0x2001.. / 0x4001.. / 0x8001; tabla verificada contra `FXINFOMANAGER_C_LOAD_MAPPING`).
4. Tipos 0x2xxx, 0x4xxx y 0x8001 (los 23 bloques son idénticos): si `(double)version >= 0.7` (`fild version ; fcomp qword [0x86B1D0] = 0.7 ; jb`, es decir **version ≥ 1** por ser int) → `ReadLine` + `sscanf(line, "%s %d", field, &flag)`; `flag` es un int de 32 bits que **conserva 1 si el sscanf no escribe**. El nombre del campo (`TIMEMODEPRT:`) no se comprueba.
5. `m_pInfos[i] = AddFxInfo(tipo)` ; `info->Load(file, version)` (virtual) ; `ReadLine` de cierre (**sin sscanf**). **Nombre desconocido:** no se reserva ni se carga nada y el EXE sigue con el puntero que ya hubiera en `m_pInfos[i]` (memoria de la arena sin inicializar).
6. `*(u8*)(m_pInfos[i]+6) = (u8)flag` (byte bajo del int, **sin normalizar a 0/1**) y, con `t = *(u16*)(m_pInfos[i]+4)` **leído tras el Load**: `t & 0x1000` → nada; si no, `t & 0x2000` → si `m_MovementOffset == -1` pasa a ser `(u8)i`; si no, `t & 0xC000` → si `m_RenderOffset == -1` pasa a ser `(u8)i`.
7. Final: `m_RenderOffset == -1` → `(u8)m_nNumInfos`; `m_MovementOffset == -1` → `m_RenderOffset`. Los offsets son bytes: un índice 255 equivale a «sin asignar», 256 → 0, etc.

## Fuente C++ y revisión propia contra el EXE (fase 2)

El borrador del árbol (`plugin::CallMethod` + un parser muerto tras el `return`) se sustituyó por una transcripción completa (`source.patch`). Se contrastó línea a línea con el desensamblado y **el parser muerto del repositorio tenía estos errores** (todos corregidos):
1. **Nunca asignaba `m_MovementOffset` ni `m_RenderOffset` dentro del bucle** (7.): sólo existía el relleno final; los offsets habrían quedado siempre en `n`.
2. Leía `TIMEMODEPRT` con `ReadField<bool>` (normaliza a 0/1 y **comprueba el nombre del campo con `NOTSA_UNREACHABLE`**); el EXE guarda el byte bajo del int tal cual, no comprueba el nombre y conserva 1 si el sscanf no escribe.
3. Usaba `ReadField<void>` (que hace `ReadLine` **+ sscanf**) para la cabecera y para la línea de cierre; el EXE hace sólo `ReadLine` (el `sscanf` extra podía además desbordar `field`).
4. Un nombre desconocido terminaba en `NOTSA_UNREACHABLE`; el EXE continúa con el puntero que hubiera en `m_pInfos[i]`.
5. Usaba el `sscanf` de la DLL (UCRT) y el contador de `GetInfos()` (span fijo) en lugar de releer `m_nNumInfos`; `if (... version >= 0.7f)` con `version` int es correcto pero con una constante float.
6. `m_nType` se leía de la variable local, no del objeto tras el `Load`.
7. El tipo de bloque `FX_INFO_ROTATE/TEXCOORDS/ROTATEOFFSET` (comentados NOTSA) no existe en el EXE: la tabla de 32 entradas coincide con la del EXE.
Nada más difiere: orden de llamadas (ReadLine, ReadLine, sscanf, GetMem, [ReadLine, sscanf, (ReadLine, sscanf), AddFxInfo, Load, ReadLine]×n), argumentos (`file`, `version` al Load virtual, `size`=256 en todos los ReadLine, `GetMem(n*4, 4)`) y efectos sobre el objeto (bytes +0, +8, +9, +4; +5, +10.. intactos). x87: el EXE sólo hace `fild`/`fcomp` (pila vacía al salir); la DLL compara con SSE2. **EAX/ECX/EDX:** `python -I reversing/Dependencies25A/residue_scan.py 0x5C0B70` da un único llamador (0x5C2403, dentro de `FxPrimBP_c::Load` del EXE) y, desensamblado a mano, tras el `call` sólo hay `pop edi/esi/ebp ; mov al,1 ; pop ebx ; add esp ; ret 0Ch`: **ningún llamador lee EAX/ECX/EDX**. El llamador C++ (`FxPrimBP.cpp:126`) tampoco usa valor. Por eso el banco sólo los registra.
Riesgos de la fuente: (a) nombre desconocido → escritura por un puntero sin inicializar, igual que el EXE (los datos del juego no lo producen; el banco lo prueba con un FxInfo «basura» válido); (b) tokens de nombre > 127 caracteres desbordan `field[128]` en ambos (el banco no los genera); (c) `m_nNumInfos` > capacidad de la arena FX no se comprueba (igual que el EXE).

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **Objeto** (0x14 bytes, con `m_pInfos` sustituido por su desplazamiento en la arena de punteros) y que **nada fuera de él cambie** en su arena (guardas de 0x40 y los 12 bytes reservados tras el objeto): `mgr`, `guard_ok`.
- **Arena de punteros** (320 ranuras prellenadas con un FxInfo «basura»): etiqueta de cada ranura < n (ordinal de creación, 0x10000 = basura) y que el resto no cambie (`slots`, `ptr_outside_ok`); `pinfos` = desplazamiento devuelto por el GetMem falso.
- **FxInfo falsos** creados (24 bytes cada uno, prellenados): tipo final (el Load falso puede cambiarlo), byte 6 (debe ser el byte bajo de la bandera, escrito DESPUÉS del Load; el falso deja 0x77) y que el resto (vptr y bytes 7..) no cambie; los no creados, intactos. FxInfo «basura»: tipo, byte 6 y resto.
- **Flujo**: cursor final y arena de texto sin tocar (`cursor`, `stream_ok`).
- **Traza** de colaboradores (idéntica en ambos lados y al oráculo), resumida en longitud + hash FNV-1a 64 + 24 primeros y 8 últimos eventos: `[1, antes, después, 256|1<<16|1<<17]` ReadLine (lee `file` correcto y tamaño 256; el cursor es el de `fgets`: líneas > 255 caracteres se parten); `[2, size, align, this==0xA9AF2C]` GetMem; `[3, this==objeto, tipo, ordinal]` AddFxInfo; `[4, ordinal, version, file_ok | tipo<<1]` Load virtual (lee `plan.body` líneas, cambia el tipo con `plan.retype`).
- ABI: ESP (`ret 8`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa, TOP, tags y ST válidos: no debe quedar ninguno; se ignoran banderas FSW, FIP/FDP y registros vacíos; MXCSR sin banderas), FCW fijada a 24 bits como en el juego.
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`; `kCompareVolatileRegisters=false`), ver arriba.
- Rutas (`route_ok`): original → el hook de 0x5C0B70 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; **en ambos lados** el sscanf del EXE `0x8220AD` no está enganchado. Se vigilan 16 bytes de 0x5C0B70, 0x8220AD y las tres entradas parcheadas (deben seguir siendo el código de partida al volver cada llamada). El pool de la DLL (`&g_fxMan.GetMemPool()`) debe ser 0xA9AF2C (se registra en `begin` y `run_local` lo exige).

## Colaboradores

**Sustituidos** (mismo falso en ambos lados, parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL, resuelto antes de parchear; activos sólo durante la llamada): `ReadLine` 0x4AB250 / `::ReadLine` de `FxTools.cpp` (sirve el flujo con semántica de `fgets`; el `FILESTREAM` es un puntero testigo que nunca se desreferencia), `FxMemoryPool_c::GetMem` 0x4A9CA0 (devuelve la arena de punteros), `FxInfoManager_c::AddFxInfo` 0x4A7B00 (devuelve un FxInfo falso de 24 bytes con vtable falsa) y el `Load` virtual del FxInfo falso. Consecuencia: **no se valida aquí** el cuerpo real de esos colaboradores (ni la creación de las subclases FxInfo ni `CFileMgr::ReadLine`); sólo que `Load` los llama igual y con los mismos argumentos. `collaborators_stubbed=true` en `begin`.
**Reales:** `sscanf` — **siempre el del EXE (0x8220AD)** en los dos lados: la DLL lo llama por dirección (como `Stats.cpp`, `WaterLevel.cpp` y `CFileLoader::Load2dEffect`; el UCRT de la DLL difiere en `%d` límite). No se necesita sondeo del CRT. `strcmp` del nombre (en el EXE `repe cmps` equivalente).

## Fixtures (`Fixtures.hpp`): 4608 casos = 32 clases × 144 (`kind = n % 32`)

Flujo de texto continuo con semántica de `fgets(buf,256)`; relleno final («FILLER n») para que nunca se llegue a EOF (con EOF el buffer queda sin tocar = basura de pila). Objeto previo con patrones (ceros, 0xFF, aleatorio), FxInfo «basura» con tipo (0x0000, 0x1001, 0x2001, 0x4001, 0x8001, 0x3000, 0xC000, 0xFFFF…) y byte 6 variables, semillas de EAX/EDX/pila.
- 0/1 secuencias bien formadas aleatorias de 0..12 infos (versiones 1/2/109/100/7/INT_MAX; la clase 1 usa además 0, −1, INT_MIN…); 2 fases emisión → movimiento → render; 3 fases desordenadas o ausentes; 4 valores de `TIMEMODEPRT` (36 formas: 0, 1, 2, −1, 255, 256, 257, 65536, −256, `+3`, `007`, `0x10`, `abc`, vacío, sin espacio tras `:`, `FOO: 3`, `3.9`, `1e3`, `12abc`, desbordes (byte desconocido para el oráculo), blancos…).
- 5 formas de la línea `NUM_INFOS` (22: sin espacio, `+N`, ceros, `0x3`, ilegible, en blanco, CRLF, texto detrás…; las que no escriben conservan el valor previo del objeto); 6/7 contadores 127..320 con el primer movimiento/render en índices 0, 1, 5, 126..129, 200, 253..257, 300 (offsets en byte con signo/envoltura, 255 = «sin asignar»); 8 contadores ≤ 0 (0, −1, −5, −127..−300, −2147483647…).
- 9 nombres desconocidos mezclados (20 formas: prefijos, sin `:`, mayúsculas/minúsculas, `ROTATE/TEXCOORDS/ROTATEOFFSET` no soportados, `NUM_INFOS:`…); 10 barrido de los 32 tipos con versiones 0/1/109; 11 líneas de cuerpo (0..3) y cambio de tipo en el Load (0x1000, 0x2000, 0x4000, 0x8000, 0x3000, 0xC000, 0xFFFF, 0x5000…); 12 líneas > 255 caracteres (cabecera, NUM_INFOS, nombre, tiempo, cuerpo, cierre: `fgets` las parte y el resto se lee como línea siguiente); 13/14 versión en los bordes de 0.7 con línea de tiempo presente/omitida aunque el EXE no la lea/la espere; 15 estructura típica SA; 16..31 mezcla aleatoria (0..24 entradas, 15 % desconocidas, cuerpos, cambios de tipo, cierres largos, cabecera larga).
- Reglas de seguridad (UB que el banco evita): nombres de ≤ 100 caracteres y nunca en blanco (`field` quedaría con basura de pila), contadores ≤ 320, sin EOF, restos de líneas largas < 100 caracteres, sin palabras > 127 caracteres.

## Oráculo (`fixture_oracle.py`)

Modelado desde el desensamblado (no desde el C++): re-analiza el texto de `input` con su propio `fgets`/`sscanf` (`%s`, `%s %d` con prefijo decimal; |v| ≥ 2³¹ o > 18 dígitos → byte «desconocido» que sólo se exige igual EXE == DLL), simula lecturas, creaciones de FxInfo y el reparto de offsets y genera lo esperado de cada clave observada. Autoprueba con 11 grupos (vacío, fases, versión 0, desconocido, NUM ilegible, retype, n ≤ 0, offsets en 255/256, nombre en blanco = fixture inválido, desbordes). `Fail` = fixture inválido.

## Comprobaciones sin el juego (hechas)

`fixture_dump.cpp` (compila el `Fixtures.hpp` real, sin tipos del juego) + `fixture_mirror.py`: 4608 casos, **0 inválidos**, cobertura medida: 2245 con nombre desconocido, 3482 con línea de tiempo leída, 568 con versión < 1 y tipo ≥ 0x2000, 2517 con bandera ∉ {0,1}, 1473 con cambio de tipo en el Load (511 cambian los offsets), 125 con n ≥ 256, 267 con n ≥ 128, 308 con n ≤ 0, 327 con NUM_INFOS ilegible, 1123 con líneas partidas por `fgets`, 91829 FxInfo en total. `run_local.py::MINIMUMS` deja 35–50 % de margen.

`host_check.py` + `host_check.cpp` (sin el juego): extrae el CUERPO REAL de `FxInfoManager_c::Load` de `source/game_sa/Fx/FxInfoManager.cpp` (con el sscanf del host en lugar del 0x8220AD), lo compila con cl x86 `/Od /RTC1` junto a los falsos de `Core.hpp` (ReadLine, GetMem, AddFxInfo, Load virtual, `Observe`) y ejecuta los 4608 casos: **0 diferencias frente al oráculo**. Para comprobar que la prueba muerde, 4 mutaciones de la fuente se detectan: bandera normalizada a bool (2691 casos distintos), prioridad `0x1000`/`0x2000` rota (361), `version >= 0` en vez de `>= 0.7` (268), sin la lectura de cierre (4235). Esto valida la lógica de la fuente, `Fixtures.hpp`, `Core.hpp` (falsos y observación) y el oráculo entre sí; NO valida el EXE ni el sscanf del EXE (eso lo hace la corrida real).

## Suposiciones y riesgos del banco

- **No se ha compilado la sonda ni ejecutado el juego.** `Core.hpp`/`LiveComparison.hpp` siguen la plantilla `FileLoaderLoad2dEffect27` (mismos `Gate`, `EntryPatch`, `HookRoute`), pero las firmas nuevas (`__fastcall` para thiscall con `ret 4`/`ret 8`, `&::ReadLine`, `&FxMemoryPool_c::GetMem`, `&FxInfoManager_c::AddFxInfo`, `&g_fxMan.GetMemPool()`) pueden requerir ajustes al compilar.
- Se supone que las funciones de la DLL (`::ReadLine`, `GetMem`, `AddFxInfo`) no están inlineadas y tienen ≥ 5 bytes (compilación Debug de la sonda); `Body()` salta los enlaces `E9` del enlazado incremental.
- Las expectativas de `%d` (prefijo decimal; `0x10` → 0; `3.9` → 3; `+3`, `-0`) son las del `sscanf` estándar; si el CRT del EXE se comportara distinto en algún formato raro, la corrida lo mostraría como «salida distinta del oráculo» en AMBOS lados (no como diferencia EXE/DLL, que usan el mismo sscanf) y habría que reclasificar ese formato como desconocido en `scan_s_d`.
- El FxInfo falso no imita constructores reales; el Load falso escribe 0x77 en el byte 6 y puede cambiar el tipo para comprobar el orden (Load → byte 6 → lectura del tipo).
- Nombre desconocido: el EXE y la DLL escriben el byte 6 del FxInfo «basura» al que apuntan todas las ranuras prellenadas (comportamiento que sólo es definido gracias al prellenado del banco).
- Tamaño de la corrida: ~92 000 FxInfo y filas JSON con el texto del flujo (hasta ~30 KB en las clases 6/7); `timeout=1800` s como las demás.
