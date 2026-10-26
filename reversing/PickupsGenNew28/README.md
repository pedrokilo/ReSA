<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PickupsGenNew28: CPickups::GenerateNewOne (0x456F20)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175854-397966`: 6144 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pickupsgen28\20261018-175854-397966`; archivado en `reversing/CppTwelveLevel6Final/runs/PickupsGenNew28-20261018-175854-397966/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CPickups,GenerateNewOne,0x00456F20,1,0,1,S` en `docs/hooks.csv`). Los bancos históricos que sustituían 0x456F20 con un falso (PickWeap1, CTaskSimpleJetPack, FileLoader47, GangEvent1, GangWarsPending, InteriorLifecycleRuntime) ya no interceptan la llamada de la DLL (ahora C++ directo) y deben revalidarse.

Plantillas: `reversing/FileLoaderLoad2dEffect27` (colaboradores sustituidos por falsos con parche de entrada en el EXE y en el cuerpo de la DLL), `reversing/DecisionAdd27`, contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PickupsGenNew28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PickupsGenNew28/build_probe.py
python -B reversing/PickupsGenNew28/run_local.py
python -B reversing/PickupsGenNew28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si aún no hay registro terminado)
python -I reversing/PickupsGenNew28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -B reversing/PickupsGenNew28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol con `RH_ScopedNamedInstall(GenerateNewOne_ExeAbi, "GenerateNewOne", 0x456F20)` (clase `CPickups`, hook `Simple`; `HookRoute` lo busca por esos nombres) y `source/game_sa/Pickups.h` con `GenerateNewOne_ExeAbi`. El parche de la fuente está en `source.patch` (sólo esta función, `git apply` sobre HEAD comprobado con `git apply --check`). Archivo de arranque en el clon: `pickupsgen28-runtime.start`; salida `pickupsgen28-runtime.jsonl`. 6144 pares.

Para reproducir la calibración de cobertura sin juego: `cl /nologo /std:c++20 /EHsc /I. fixture_dump.cpp && fixture_dump.exe > inputs.jsonl` (std puro) y pasar cada fila por `fixture_oracle.expected`.

## Fuente (Pickups.cpp / Pickups.h)

* `CPickups::GenerateNewOne_ExeAbi(CVector, uint32 modelId, ePickupType, uint32 ammo, uint32 moneyPerDay, uint8 isEmpty, char* message)` → `int32`, enganchada con `RH_ScopedNamedInstall(..., "GenerateNewOne", 0x456F20)` (antes el hook estaba comentado «por la ABI»: el EXE devuelve el handle en EAX y MSVC devolvería `tPickupReference` por puntero oculto, igual que `GenerateNewOne_WeaponType_ExeAbi`). `isEmpty` es `uint8` porque el EXE lee sólo el byte (`mov dl,[esp+28h]`; `test dl,dl`; `and dl,1`) y un valor como 2 cuenta como «vacío» para elegir hueco pero `bEmpty` = bit 0.
* `CPickups::GenerateNewOne(..., bool isEmpty, ...)` pasa a llamar a `GenerateNewOne_ExeAbi` (ya no hay `plugin::CallAndReturn` hacia 0x456F20). Los demás llamadores C++ (`GenerateNewOne_WeaponType`, `CreateSomeMoney`, `FileLoader`, `GangWars`, `Ped`, `Interior_c`, `TaskSimpleJetPack`) usan ahora el cuerpo C++.
* Sin `rand`, sin `sscanf`; el único CRT que interviene es la conversión float→int64 de `CPickup::SetPosn` (inline en `Pickup.h`, ya existente; el EXE usa `_ftol2` 0x821B40).

## Qué hace la función (desensamblado 0x456F20..0x45737D; `ret` normal, el llamador retira 0x24)

Pila: +04 x, +08 y, +0C z, +10 modelId, +14 pickupType (byte), +18 ammo, +1C moneyPerDay, +20 isEmpty (byte), +24 message. Ranura = 0x20 bytes en 0x9788C0 (620 ranuras): +00 ingresos, +04 m_pObject, +08 ammo, +0C regeneración, +10/12/14 x y z (int16), +16 dinero/día, +18 modelo, +1A referencia, +1C tipo, +1D banderas.
1. **Hueco** (pasadas): (1) tipo 0: desde el FINAL si `pickupType` es 13 u 11 o el byte `isEmpty` ≠ 0, si no desde el principio; (2) primer tipo 8; (3) primer tipo 4 o 5; sin hueco `eax = -1` y no se escribe nada. En (2) y (3), si `m_pObject` ≠ 0: `CWorld::Remove(obj)` (0x563280), se relee `m_pObject` y, si ≠ 0, destructor virtual con borrado (`[vtbl+0]`, flag 1); `m_pObject = 0`. En (1) NO se toca el objeto.
2. **Relleno**, en este orden: ammo; dinero/día (palabra baja); banderas `= (banderas & F8) | ((isEmpty & 1) << 1)`; tipo; ingresos = 0; regeneración = `CTimer::m_snTimeInMilliseconds`; tipo 4: +20000, 5: +120000, 8: +30000, 9/10: tipo 9 y +1500, 11/12: tipo 11 y +1500 (mod 2^32); modelo (palabra baja); `FindTextIndexForString(message)` (0x455500) → bits 4..6 de las banderas; x, y, z: `trunc(v*8)` → int16 (`_ftol2`); `IsVisible` (0x454C70) → bit 3; `m_pObject = 0`; si bit 3: `GiveUsAPickUpObject(m_pObject, -1)` (0x4567E0) y, si `m_pObject` ≠ 0, `CWorld::Add` (0x563220).
3. **Handle**: `ref = word[+1A]`; `ref >= 0xFFFE ? 1 : ref + 1`; `eax = (ref << 16) | indice`.

## Fase 2: revisión propia contra el EXE (hecha tras escribir la fuente; segunda lectura línea a línea del desensamblado)

Contrastado (todo coincide, **sin discrepancias halladas tras la segunda lectura**; en la primera pasada se había considerado devolver un `tPickupReference` directamente, descartado por la ABI):
* 0x456F20..0x456F3A condición de la búsqueda por el final (13, 11, `isEmpty` byte ≠ 0; el resto de bits de los dwords empujados se ignora). Bucles 0x456F43..0x456F5A (final, 619→0) y 0x456F62..0x456FF4 (principio, desenrollado ×10, 62 vueltas = 620). Si la búsqueda por el final falla, el EXE ejecuta igualmente la del principio (sin efecto: no hay ninguna de tipo 0) y sigue a 0x457000; en C++ se pasa directo a la pasada 2.
* 0x457000.. (tipo 8) y 0x457097.. (tipos 4 y 5, primero por posición); salida −1 en 0x45715F / 0x4571D9.
* 0x45719A..0x4571CA: `Remove`, relectura de `m_pObject`, destructor virtual, `m_pObject = 0` (sólo si había objeto).
* 0x4571DF..0x457290: orden de escrituras y sumas del tipo; banderas `F8 | isEmpty&1<<1` (bits 3..7 conservados); tipos 9/10 → 9 y 11/12 → 11 con +0x5DC; 4: +0x4E20; 5: +0x1D4C0; 8: +0x7530.
* 0x457290..0x4572F9: modelo antes de `FindTextIndexForString`; texto `xor/and 70h`; tres conversiones `_ftol2` x→y→z a +10/+12/+14; `IsVisible` con ECX = ranura; bit 3 `xor/and 8`.
* 0x45731F..0x457342: `m_pObject = 0` antes de `GiveUsAPickUpObject(&m_pObject, -1)`; `CWorld::Add` sólo si el objeto devuelto ≠ 0.
* 0x457342..0x45737D: referencia (`cmp ax,0FFFEh; jae` → 1) y handle.
* Desplazamientos de pila de cada argumento recalculados con el número de `push` vigente en cada punto (2, 4, y 5 con `push ecx`): coinciden con la firma.
* **Llamadores del EXE** (`python -I reversing/Dependencies25A/residue_scan.py 0x456F20`): 0x445098, 0x445afd, 0x4573c2, 0x458a58, 0x477983, 0x47e636, 0x480658, 0x481744, 0x48b243, 0x48ceb9, 0x592103, 0x5b49c6, 0x67b6de. Seis de ellos (0x477983, 0x47e636, 0x480658, 0x481744, 0x48b243, 0x48ceb9) **leen EAX** (guardan el handle) y ninguno lee ECX/EDX tras el `call` (el escáner marca `unwritten=ecx/edx` en los que siguen con otro `call`, revisados a mano en 0x445098 y 0x458a58: ECX/EDX se reescriben antes de usarse). Por eso el banco **compara EAX** (valor devuelto) y sólo registra ECX/EDX (`original_residue`/`candidate_residue`; el EXE deja valores de x87/pila que C++ no reproduce).

## Qué se compara (por lado, en bits; original == candidato == oráculo)

* `ret` (EAX), las ranuras de la tabla que cambian (`changed`: índice + 32 bytes, con `m_pObject` como etiqueta), la **traza** de colaboradores y la hora/mensaje/objetos falsos intactos.
* **Traza**: `[tipo, a, b, c, índice de la primera ranura distinta de la inicial, sus 32 bytes en ese instante]`: 1 `CWorld::Remove(obj)`, 2 destructor virtual `(obj, 1)`, 3 `FindTextIndexForString` (mensaje nulo?, FNV-32 del texto), 4 `IsVisible(ranura)`, 5 `GiveUsAPickUpObject(ranura, &m_pObject−ranura = 4, −1)`, 6 `CWorld::Add(obj)`. Fija el orden de las escrituras respecto de cada llamada (el EXE ya ha escrito ammo/dinero/banderas/tipo/regeneración/modelo cuando llama a `FindText`, y además posición y texto cuando llama a `IsVisible`, y `m_pObject = 0` y bit 3 al llamar a `GiveUs…`).
* ABI: ESP (`ret` normal, `popBytes = 0x24`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa, TOP, tags y ST válidos: pila vacía; FCW = 24 bits, más cercano, excepciones enmascaradas), MXCSR sin banderas.
* Rutas (`route_ok`): original → el hook de 0x456F20 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; en ambos lados 0x821B40 (`_ftol2`) no está enganchado. Se vigilan 16 bytes de 0x456F20, 0x821B40 y las cinco entradas parcheadas (deben seguir siendo el código original al volver).
* Estado global: la tabla real de 620 ranuras (0x9788C0) y la hora (0xB7CB84) se guardan al inicio, se sustituyen por caso y se restauran y comprueban al final (`table_restored`).

## Colaboradores

**Sustituidos** con el mismo falso en ambos lados (parche de entrada en la dirección del EXE y en el cuerpo de la DLL, resuelto antes de parchear; activos sólo durante la llamada): `CPickup::FindTextIndexForString` 0x455500 (devuelve un valor del caso), `CPickup::IsVisible` 0x454C70 (devuelve AL = 0/1 con basura en el resto de EAX), `CPickup::GiveUsAPickUpObject` 0x4567E0 (escribe en `*out` un objeto falso del caso o nulo), `CWorld::Remove` 0x563280, `CWorld::Add` 0x563220; el destructor virtual lo atiende la vtbl falsa de los cuatro objetos falsos. Consecuencia: **no se valida aquí** el cuerpo de ninguno de ellos; sólo que `GenerateNewOne` los llama igual, con los mismos argumentos y el mismo estado de la ranura. `collaborators_stubbed=true`.
**Reales**: `_ftol2` 0x821B40 (EXE) y la conversión float→int64 de la DLL dentro de `CPickup::SetPosn` (no se sustituye: la comparación EXE/DLL de las coordenadas incluye NaN, ±inf, |v·8| ≥ 2^63, desbordes de float y ±32768).

## Fixtures (`Fixtures.hpp`): 6144 casos = 16 familias × 384

La tabla del caso son 620 ranuras de «relleno» (todo 0xA5 salvo el tipo, uno de {1,2,3,6,7,9..23,0x40,0x7F,0x80,0xFF}) más ranuras explícitas (índices de frontera 0/1/9/10/11/309/310/609/610/618/619 o aleatorios; 32 bytes aleatorios, tipo y objeto fijados). Los objetos de las ranuras sustituibles (tipos 8/4/5) son siempre nulos o uno de cuatro objetos falsos (etiquetas 0x0B100001..4); en las demás pueden ser crudos (0xA5A5A5A5, 0xDEADBEEF), nunca desreferenciados.
`kind = n % 16`: 0 hueco libre desde el principio (con ruido de dinero/tiempo límite que no debe usarse); 1 `ptype` 13; 2 `ptype` 11; 3 `isEmpty` ≠ 0 (1, 2, 3, 0x7F, 0x80, 0xFF, 0x55) → desde el final; 4 `isEmpty` = 0 con basura en los bytes altos; 5 sin libres, dinero con objeto; 6 sin libres, dinero sin objeto; 7 sólo tiempo límite; 8 tiempo límite 4/5 (gana el primero por posición); 9 dinero y tiempo límite (gana el dinero); 10 nada sustituible (−1, tabla intacta); 11 búsqueda por el final fallida → dinero/tiempo/−1; 12 barrido de los 256 valores de `pickupType`; 13 barrido de `isEmpty`, texto, visibilidad y objeto devuelto; 14 índice de referencia (0, 1, 2, 0x7FFE/F, 0x8000, 0xFFFA..0xFFFF); 15 coordenadas/modelo/ammo/dinero extremos. En todas: `pickupType`/`isEmpty` llegan en dwords con basura en los bytes altos, coordenadas (típicas y 31 especiales: 0, −0, 1/8, 4096, 8192, 1e10, 1.2e18, 5e37, FLT_MAX, ±inf, NaN, denormales), `modelId`/`moneyPerDay` de 32 bits, hora cerca del desbordamiento (suma mod 2^32), mensaje nulo/«PROP_3»/«PROP_4»/aleatorio, valor devuelto por `FindText` (0..0xFFFFFFFF), visibilidad 0/1 con basura, objeto devuelto 0 o falso, semillas de pila y EAX/ECX/EDX.

`fixture_oracle.py` modela la función desde el desensamblado (autoprueba con ~30 aserciones: búsqueda por delante/detrás, dinero y tiempo límite, trazas, regeneración, referencia, coordenadas) y `run_local.py` exige cobertura mínima (pasada 1 por delante/por detrás, 2, 3, objeto sustituido nulo/no nulo, cada `ptype` crítico, banderas previas, referencia que da la vuelta, coordenadas NaN/inf/enormes/±32768, visibilidad con y sin objeto, hueco en los extremos, etc.). Recuentos medidos con el volcado (6144 casos): pass1_front 1964, pass1_top 1492, pass2 1390, pass3 892, none 406, replaced_obj 1361, replaced_null 921, visible_give_obj 1954, ref_wrap 56, ptype_13 501, coord_nan 573, top_failed_none 22; los mínimos de `run_local.py` están al ~60-85 % de ellos.

## Suposiciones y riesgos

* El EXE lee `pickupType` e `isEmpty` como bytes; un llamador real los empuja como `push` de dwords limpios, pero el banco empuja basura en los bytes altos a propósito (la fuente C++ los declara `ePickupType : uint8` y `uint8`).
* Los falsos devuelven AL canónico (0/1) en `IsVisible`: un `IsVisible` real nunca da otra cosa. `FindTextIndexForString` devuelve cualquier valor de 32 bits; sólo importan sus tres bits bajos (el EXE usa AL; el C++ aplica `(x << 4) & 0x70`).
* El banco asume que `delete object` en la DLL llama por la vtbl a [0] con flag 1 (como `CPickup::GetRidOfObjects` y `Pools.cpp`), igual que el EXE. Si CObject tuviera `operator delete` de clase se ejecutaría dentro del destructor real, no del falso, así que no interviene.
* Si MSVC inlinease `CPickup::IsVisible`/`GiveUsAPickUpObject`/`FindTextIndexForString` o `CWorld::Add/Remove` dentro de `GenerateNewOne_ExeAbi`, el parche del cuerpo no se ejecutaría y la traza del candidato saldría vacía (se vería como diferencia). En Debug (`/Od`) no se inlinean.
* La tabla de pickups se sustituye por completo durante la corrida (un solo fotograma, dentro de `CGame::Process`): ningún otro código del juego se ejecuta entre medias; se restaura byte a byte y se comprueba.
* **Bancos existentes afectados**: los que sustituyen 0x456F20 con un falso en el EXE para aislar `GenerateNewOne_WeaponType`, `CreateSomeMoney`, etc. (`PickWeap1/PickupCases.hpp` `P.Add(0x456F20, S_GenerateNewOne)`, `CTaskSimpleJetPack`, `FileLoader47`, `GangEvent1`, `GangWarsPending`, `InteriorLifecycleRuntime`) ya no interceptarán la llamada de la DLL: ahora `GenerateNewOne` es C++ directo y el parche del EXE sólo vale cuando el hook de 0x456F20 está apagado. Hay que parchear también el cuerpo de la DLL (`Body(&CPickups::GenerateNewOne_ExeAbi)`) y, para llamadas C++ a `GenerateNewOne`, ese mismo cuerpo, o reejecutar esos bancos con la fuente nueva.
* `docs/hooks.csv` no contiene aún una fila para `CPickups,GenerateNewOne,0x00456F20` (no se toca aquí).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_dump.cpp`, `register_hook.py`, `source.patch`, `README.md`.
