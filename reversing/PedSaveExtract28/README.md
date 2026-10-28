<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PedSaveExtract28: CPedSaveStructure::Extract (0x5D44B0)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175707-784669`: 4608 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pedsaveextract28\20261018-175707-784669`; archivado en `reversing/CppTwelveLevel6Final/runs/PedSaveExtract28-20261018-175707-784669/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CPedSaveStructure,Extract,0x005D44B0,1,0,1,S` en `docs/hooks.csv`).

Plantillas: `reversing/FileLoaderLoad2dEffect27` (Core/LiveComparison/run_local/register_hook) y `reversing/DecisionAdd27`; contrato general `reversing/_handoff/BRIEF.md` (el banco usa `__asm` en `Gate`/`SaveFP`, igual que los demás; los avisos «SUPERADO» sólo valen para producción).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PedSaveExtract28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PedSaveExtract28/build_probe.py
python -B reversing/PedSaveExtract28/run_local.py
python -B reversing/PedSaveExtract28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no terminó)
python -I reversing/PedSaveExtract28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PedSaveExtract28/fixture_mirror.py                                              # compila fixture_dump.cpp (MSVC, no el proyecto), 0 casos inválidos + cobertura contra los mínimos (imprime «ok»)
python -I reversing/PedSaveExtract28/fixture_emulate.py                                             # oráculo contra los bytes del EXE emulados (capstone) en los 4608 casos (imprime «ok»)
python -B reversing/PedSaveExtract28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(Extract, 0x5D44B0)` (ya está en `PedSaveStructure.cpp`; el parche aislado está en `source.patch`). Archivo de arranque en el clon: `pedsaveextract28-runtime.start`; salida `pedsaveextract28-runtime.jsonl`. 4608 pares. `docs/hooks.csv` no tiene la fila `CPedSaveStructure,Extract,0x005D44B0,1,0,1,S` (se añade al registrar; este encargo no toca docs/).

## Fuente (fase 1) y `source.patch`

Cambios respecto a HEAD, sólo de esta función, en `source/game_sa/Entity/Ped/PedSaveStructure.cpp`: cuerpo C++ de `Extract` (antes `plugin::CallMethod<0x5D44B0,...>`) y `RH_ScopedInstall(Extract, 0x5D44B0)`. No hace falta tocar el `.h` (la declaración ya existía). `source.patch` es `git diff HEAD` de ese archivo (y del `.h`, sin cambios); se verificó con `git apply --check` contra `git show HEAD:...` en un directorio temporal del scratchpad y el resultado coincide con el árbol (salvo finales de línea CRLF del árbol). El archivo `PedSaveStructure.cpp` ya se compila con `/O2 /Oi` (por `Construct`); `Extract` no necesita opciones propias (sin x87 ni residuo de registros).

Orden de operaciones (todo contrastado con el desensamblado, ver abajo): posición (matriz si `ped->m_matrix`, si no la colocación) → vida (`+0x540`) → armadura (`+0x548`) → ranura activa (`+0x718`) → `SetCharCreatedBy` (0x5E47E0) → estilo de lucha (`+0x72D`) → movimientos permitidos (`+0x72E`) → trece armas (si el tipo ≠ 0: `GetWeaponInfo(tipo,1)` 0x743C60, modelo 1 ≠ -1 → `RequestModel(m,8)` 0x4087E0 + `LoadAllRequestedModels(false)` 0x40EA10; **segunda** `GetWeaponInfo`, modelo 2 igual; `GiveWeapon(tipo, municionTotal, false)` 0x5E6080; después `m_aWeapons[i].m_AmmoInClip = guardada`) → `SetCurrentWeapon((int8)ranura)` 0x5E61F0 → código de área (`+0x2F`) → entrada/salida (`+0x78C`).

## Revisión propia contra el EXE (fase 2)

Se desensambló `0x5D44B0..0x5D461A` (`dis.sh`) y se contrastó línea a línea con el C++ y, además, **se emuló el código del EXE** (`fixture_emulate.py`, intérprete mínimo de las 19 instrucciones de la función sobre capstone, con los seis colaboradores simulados y EAX/ECX/EDX destruidos tras cada llamada): la memoria final del ped y de la matriz, la entrada/salida y la traza de las 4608 fixtures coinciden con el oráculo; la pila x87 queda vacía, ESP/EBX/ESI/EDI/EBP se conservan.

Discrepancias halladas en el borrador (sin commit) y corregidas:
1. **Entrada/salida.** El borrador usaba `CEntryExitManager::GetPool()->GetAt(size_t(idx))`. El EXE (0x5D45D7..0x5D4606) **no comprueba el rango**: lee el byte de estado `[estados + idx]` (con signo; bit 7 = libre) y devuelve `almacenamiento + idx*0x3C`. `GetAt` aserta `IsIndexInBounds` y trata el índice como `size_t`, así que difería para índices fuera de rango (p. ej. -2) y abortaría en depuración. Ahora se lee en crudo (primer miembro del pool = almacenamiento, segundo = estados), igual que `Construct` lee el primer miembro.
2. Sin más diferencias funcionales: orden, anchuras (dword/byte), `(int8)` extendido con signo hacia `SetCurrentWeapon`, `STD`=1 como segundo argumento de `GetWeaponInfo`, flags 8 y `LoadAllRequestedModels(false)`, segunda `GetWeaponInfo` para el modelo 2, `likeUnused=false`, `m_AmmoInClip` de la ranura **i** (`ebp = ped+0x5A8 + 0x1C*i`) coinciden. Los `static_assert(offsetof…)` de `LiveComparison.hpp` fijan los desplazamientos de `CPed` (0x540, 0x548, 0x5A0, 0x718, 0x72D, 0x72E, 0x78C), de `CWeapon` y de `CWeaponInfo`.

**Límite conocido (no cubierto):** la posición se copia en el EXE con `fld`/`fstp` de 32 bits, que convierte un NaN de **señalización** en NaN silencioso; el C++ copia los bits tal cual. Un guardado normal no contiene SNaN en la posición (y vida/armadura sí se copian como dwords, sin diferencia). Las fixtures generan NaN silenciosos, infinitos, denormales y -0 en la posición, pero **no** SNaN (`Quiet()` en `Fixtures.hpp`); si algún día importa, la función tendría que usar x87.

## ¿Algún llamador lee EAX/ECX/EDX de salida?

No. Único llamador del EXE: `0x5D46A4` (dentro de `CPed::Load`, que en la DLL ya es C++: `Ped.cpp`); no hay más `call`/`jmp` ni referencias de datos a `0x5D44B0` (escaneo de la sección `.text`). `python -I reversing/Dependencies25A/residue_scan.py 0x5D44B0` → `('0x5d46a4','call',[],['CALL@5d46c2:unwritten=ecx,edx'])`: ninguna lectura de EAX/ECX/EDX tras el `call` (la llamada marcada es a `0x821E02`, cdecl con los cuatro argumentos en pila; ECX se reescribe justo después con `mov ecx,[esp+194h]`); **el revisor debe confirmarlo a mano**. Por eso EAX/ECX/EDX de salida sólo se registran (`original_residue`/`candidate_residue`, `kCompareVolatileRegisters=false`).

## Qué compara el banco (fase 3)

Original = bytes del EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CPedSaveStructure","Extract",0x5D44B0)`).

- **Memoria** (por lado, en bits; original == candidato == oráculo): cabecera del ped `[0,0x40)` (con el puntero de matriz `+0x14` enmascarado y comprobado aparte, `mat_ptr_kept`), vida/armadura `[0x540,0x550)`, armas y ranura activa `[0x5A0,0x720)`, estilo/movimientos `[0x728,0x730)`, **resumen FNV-32 del resto del ped**, entrada/salida resuelta (`null` o desplazamiento respecto al almacenamiento del pool falso), matriz completa (0x48 bytes), guardas del ped y de la matriz, estructura guardada sin tocar (con guardas), estados del pool sin tocar y pool falso/global `0x96A7D8` restaurados.
- **Traza de colaboradores** `[tipo,a,b,c,d,resumen]` idéntica en ambos lados y al oráculo: `SetCharCreatedBy(createdBy&0xFF, etiqueta del ped)`, `GetWeaponInfo(tipo, skill&0xFF)`, `RequestModel(id, flags)`, `LoadAllRequestedModels(arg&0xFF)`, `GiveWeapon(etiqueta, tipo, municion, likeUnused&0xFF)`, `SetCurrentWeapon(etiqueta, ranura extendida)`. El **resumen** es el FNV-32 de 88 bytes del estado del ped en la entrada de cada llamada (posición del ped y de la matriz, vida, armadura, bytes `+0x718/+0x72D/+0x72E/+0x2F` y la munición del cargador de las trece armas): detecta cualquier cambio de orden entre escrituras y llamadas.
- **Efectos de los falsos** (prueban el orden): `SetCharCreatedBy` escribe `A1/A2/A3` en `+0x72D/+0x72E/+0x2F` (Extract los reescribe después); `GiveWeapon` marca `C0DE0000|n` en la munición del cargador del arma `n % 13` (Extract la reescribe para las armas guardadas); `SetCurrentWeapon` escribe la ranura en `+0x718` y `A5` en `+0x2F`.
- **ABI**: ESP (`ret 4`, `popBytes=0` en `Gate`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW completa, TOP, tags y ST válidos: pila vacía; MXCSR sin banderas), FCW fijada a 24 bits/al más cercano/excepciones enmascaradas. **ECX = la estructura guardada** (thiscall).
- **Estado global**: sólo se escribe el global `0x96A7D8` (pool de entradas/salidas, se sustituye por un pool falso `{almacenamiento, estados}` y se restaura al volver) y la memoria de las arenas del banco. Se vigilan 16 bytes de `0x5D44B0` y de los seis colaboradores parcheados (deben seguir siendo el código original al volver) y se exige la restauración final del hook, de las entradas y del x87.

### Colaboradores

**Sustituidos** (mismo falso en ambos lados, parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL, resuelto antes de parchear; activos sólo durante la llamada): `CPed::SetCharCreatedBy` 0x5E47E0, `CWeaponInfo::GetWeaponInfo` 0x743C60, `CStreaming::RequestModel` 0x4087E0, `CStreaming::LoadAllRequestedModels` 0x40EA10, `CPed::GiveWeapon` 0x5E6080, `CPed::SetCurrentWeapon(int32)` 0x5E61F0. Consecuencia: **no se valida aquí** el cuerpo real de ninguno (varios están en C++ en la DLL y tienen sus propios bancos o no); sólo que `Extract` los llama con los mismos argumentos, en el mismo orden y con el mismo estado del ped. `collaborators_stubbed=true` en `begin`. No se sustituye nada más (la función no usa `sscanf`, `rand` ni x87 con conversiones).

### Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`Fixtures.hpp` es C++ estándar puro; se compila también aparte (`fixture_dump.cpp`) y el volcado alimenta `fixture_mirror.py` y `fixture_emulate.py` (no es un espejo en Python: es **el mismo generador**). Familias (`n % 16`): 0 sin armas con matriz; 1 y 12 trece armas (12 sin matriz); 2/7/8/9/10/11/14 armas dispersas; 3 tipos extremos (`47,255,256,0x7FFF…,0x80000000,0xFFFFFFFF`…); 4 identificadores de modelo extremos (`-1,0,-2,0x7FFFFFFF,INT_MIN,20000…`); 5 tipos repetidos (la tabla de modelos se comparte); 6 bytes extremos de la cola (creador `≥4`, ranura `0x80..0xFF`/12/13 → `SetCurrentWeapon` con signo, área, estilo, movimientos); 7 floats extremos en posición/vida/armadura (-0, denormales, máximos, ±inf, NaN silenciosos con cargas); 8 índice de salida en rango y **usada**; 9 en rango y **libre**; 10 fronteras (`-16,-2,0,1,2046,2047…`) con estados `00/7F/80/FF/01/81`; 11 sin matriz con índice -1; 13 todo aleatorio; 14 munición extrema (`0,0xFFFFFFFF,0x80000000`…); 15 rellenos previos del ped (ceros / 0xFF / aleatorio / constante, para detectar bytes no escritos). Por caso: con/sin matriz, relleno previo del ped, de la matriz y de los estados (ceros, 0xFF, aleatorio, constante), tabla de modelos por tipo, semillas de EAX/EDX/pila.

El índice de salida sólo toma `-1` o valores en `[-16, 2047]` (el EXE no comprueba el rango, así que las fixtures no pueden salirse de la arena de estados); los tipos de arma son siempre conocidos por la tabla del caso.

`run_local.py` exige cobertura mínima (familias, matriz/sin matriz, trece/cero armas, modelos -1/odd, índice libre/usado/negativo, estados de borde, ranuras ≥128, creador ≥4, NaN/inf/denormal/-0, rellenos…) **calibrada con el volcado real de `Fixtures.hpp`** (`fixture_mirror.py`), dejando ~45 % de margen. Peor caso medido: `wcount1`=3 (no se exige); `idx_0`=26 e `idx_2047`=28 (mínimo 10).

## Suposiciones y riesgos al compilar/correr

- **No compilado en el proyecto.** `Core.hpp` se comprobó con `cl /Zs` contra stubs de `ReversingProbe`/`HookRoute`/`json` (sólo un aviso de `_controlfp` obsoleto); `LiveComparison.hpp` **no** se pudo comprobar aparte (depende de los encabezados del juego). Puntos a vigilar: los `static_assert(offsetof(CPed, …))` (MSVC admite `offsetof` en tipos no estándar con aviso), los `static_cast` de punteros a función/miembro de `Body` (firmas `CWeaponInfo*(*)(eWeaponType,eWeaponSkill)`, `void(*)(int32,int32)`, `void(*)(bool)`, `eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)`, `void(CPed::*)(int32)`), los `#include` de `LiveComparison.hpp` (`Entity/Ped/Ped.h`, `Streaming.h`, `WeaponInfo.h`, `EntryExitManager.h`).
- Los parches de entrada exigen ≥5 bytes por función y que el cuerpo de la DLL no esté en línea: los seis colaboradores viven en otros `.cpp` (sin LTCG no se inlinan); si `Extract` los llamara en línea, la traza lo delataría (faltarían eventos). `PedSaveStructure.cpp` se compila con `/O2 /Oi`: el optimizador puede cambiar cómo se materializan las escrituras pero no su orden respecto a las llamadas (el `ped` escapa).
- Los falsos de `thiscall` se declaran `__fastcall(this, edx, args…)` (equivale a `ret N`); el de `GetWeaponInfo`/`RequestModel`/`LoadAll…` es `cdecl`. Los argumentos `bool`/`uint8` se registran enmascarados a un byte (la convención no define los bits altos).
- `SetCharCreatedBy`, `GiveWeapon` y `SetCurrentWeapon` reales no se ejecutan en ningún lado: el ped del banco es una arena sin constructor, sin task manager ni intelligence.
- Si `HookRoute` no encuentra el hook (`registered_hook=false`) no se ejecuta ningún caso y `run_local.py` lo rechaza.
- Si otro hook reescribe una de las entradas parcheadas se refleja en `globals_restored`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `fixture_emulate.py`, `fixture_dump.cpp`, `register_hook.py`, `source.patch`, `README.md`.
