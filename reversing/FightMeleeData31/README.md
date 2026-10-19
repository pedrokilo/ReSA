# Banco FightMeleeData31: CTaskSimpleFight::LoadMeleeData (0x5BEDC0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatAbort30` (Gate, estructura), `reversing/FileLoaderLoad2dEffect27` (modelo de sscanf del EXE, hook Simple) y `reversing/Data23` / `reversing/AnimMgr31` (falsos de OpenFile / LoadLine / CloseFile).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FightMeleeData31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FightMeleeData31/build_probe.py
python -B reversing/FightMeleeData31/run_local.py
python -B reversing/FightMeleeData31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/FightMeleeData31/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/FightMeleeData31/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl: 0 casos inválidos + constantes de región C++ == oráculo + cobertura frente a los mínimos
python -I reversing/FightMeleeData31/selftest_source.py                                             # compila el cuerpo REAL contra falsos y lo compara con el oráculo (imprime «mismatching cases: 0 of 4608»)
python -B reversing/FightMeleeData31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(LoadMeleeData, 0x5BEDC0)` (hook `Simple`, clase `CTaskSimpleFight`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `fightmeleedata31-runtime.start`; salida `fightmeleedata31-runtime.jsonl`. 4608 pares. `docs/hooks.csv`: fila `CTaskSimpleFight,LoadMeleeData,0x005BEDC0,1,0,1,S`.

Ya comprobado sin el juego (worktree `D:\Original\ws-4`): `fixture_oracle.py` → `ok`; `fixture_mirror.py` → `ok` (4608 casos, 0 inválidos, constantes de región idénticas, 0 faltas de cobertura); `selftest_source.py` → 0 diferencias en 4608 casos (el fuente C++ real, con el sscanf del UCRT en lugar del del EXE, y el oráculo coinciden; el sscanf del EXE sólo lo prueba el banco en juego); `Core.hpp` compila con un arnés de stubs; `source.patch` pasa `git apply --check` contra HEAD. **No** se ha compilado `LiveComparison.hpp` con el proyecto.

## Qué hace la función (desensamblado 0x5BEDC0..0x5BF2EE + tabla de saltos 0x5BF2F0 / 0x5BF314, `ret`, sin marco SEH, frame de 0x80 bytes)

Estática, cdecl, sin argumentos, `void`.
1. Inicializa **14** filas (no 12) de la tabla de combos (0xC170D0, 0x88 bytes) mientras `fila + 4 < 0xC177BC`: `[+0]=0x21`, `[+4]=1.5f`, `[+0x08+4i]=[+0x1C+4i]=100.0f`, `[+0x30+4i]=1.0f`, `byte [+0x50+i]=7`, `byte [+0x55+i]=0`, `[+0x5C+4i]=[+0x70+4i]=0`, `[+0x44]=0`, `[+0x48]=[+0x4C]=100.0f`, `word [+0x84]=0` (las filas 12 y 13 pisan el principio de los niveles), y después los siete vectores de niveles (0xC177D0, 12 bytes) a (0, 0.75f, 0).
2. `CFileMgr::OpenFile("DATA\melee.dat", "rb")` (0x538900) y bucle con `CFileLoader::LoadLine` (0x536F80): `#` o NUL se salta; `strncmp(.., "END_MELEE_DATA", 14)` termina; sin bloque abierto sólo `START_COMBO` (11 caracteres) / `START_LEVELS` (12) abren el bloque; `END_COMBO` (9) cierra (con bloque de combo: fila += 0x88) y pone el contador de líneas a 0.
3. Bloque de niveles: `sscanf(.., "%s %f %f %f")` a los tres floats persistentes (valen (0, 0.75f, 0) al principio) y los copia al vector `contador` (sin límite).
4. Bloque de combo, por contador (`ebx++`, índice > 8 se ignora; salto por tabla): 0 `"%s %s"` + búsqueda del nombre en `GetAnimGroupName(i)` (0x4D3A20) para `i < [0xB4EA28]` **con signo** → `[fila+0]=i`; 1 `"%s %f"` → `[fila+4]` (copia en crudo); 2..6 (golpe `mov = contador-2`) `"%s %f %f %f %s %d %d %d %f"` → `[+0x08+4m]=f20/30`, `[+0x1C+4m]=f24/30` (`fmul` por 0x3D088889), `[+0x30+4m]=` tercer float en crudo, `byte [+0x50+m]=GetHitLevel(cadena)` (0x5BD360: 'H' 0, 'L' 1, 'G' 2, 'B' 3, otro 7), `byte [+0x55+m]=(byte)i30`, `[+0x5C+4m]` / `[+0x70+4m]` = `movsx16(GetHitSound(i))` (0x5BD3B0: 1..8 → 0x3D..0x44, otro → 0x3E) y, si el noveno valor `> 0`, `[+0x44]=f*30⁻¹`; 7 `"%s %f %f"` → `[+0x48]`, `[+0x4C]` (× 1/30); 8 `"%s %x"` → `word [+0x84]`.
5. `CFileMgr::CloseFile`. Los escalares de sscanf (`f20, f24, raw2c, i30, i34, i38, raw28, x3c`, `word2`) **conservan su valor entre líneas** (huecos de pila); antes de la primera lectura contienen basura de pila (el banco no depende de ellos).

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskSimpleFight.cpp`: includes (`FileMgr.h`, `FileLoader.h`), un bloque anónimo con las direcciones/constantes (`FightLevelsAddress`, 1/30), el **sscanf del EXE por dirección (0x8220AD)**, `FightAt<T>`, `FightHitLevel` y `FightHitSound` (en línea, verificadas contra el desensamblado), `RH_ScopedInstall(LoadMeleeData, 0x5BEDC0)` y el cuerpo. `source-all.patch` (en `FightProcessPed31/`) contiene las tres funciones; los individuales tocan líneas contiguas: aplícalos con `--3way` o usa el combinado.

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador

| EXE | C++ | Resultado |
|---|---|---|
| 0x5BEDC0..0x5BEE36 (bucle de 14 filas) | bucle `do { .. } while (fila + 4 < 0xC177BC)` | igual (14 filas; campos y orden de las escrituras) |
| 0x5BEE38..0x5BEF47 (siete vectores, residuos de pila) | siete vectores (0, 0.75f, 0) y `level[] = {0, 0x3F400000, 0}` | igual |
| 0x5BEF47..0x5BEF70 (OpenFile, primera LoadLine) | `OpenFile("DATA\\melee.dat","rb")`, `for (line = LoadLine(file); line; ...)` | igual (ruta/modo leídos del EXE) |
| 0x5BEF80..0x5BF030 (`#`, NUL, END_MELEE_DATA, START_*, END_COMBO) | mismas pruebas y orden | igual |
| 0x5BF030..0x5BF07E (niveles) | `sscanf("%s %f %f %f")` + vector `contador` | igual |
| 0x5BF07E..0x5BF2C0 (tabla de saltos 0x5BF2F0 y 0x5BF314) | `switch (counter++)` | igual: casos 0..8 y subtabla de 5 entradas para `contador-3` |
| 0x5BF092..0x5BF10C (búsqueda de grupo) | bucle sobre `[0xB4EA28]` con signo, `GetAnimGroupName(i)` en cada vuelta | igual |
| 0x5BF1C0..0x5BF258 (golpe) | tres `fld/fmul/fstp`, copia en crudo del radio, nivel, sonidos | igual; `f1c > 0` (`fcomp` + `test ah,41h`) |

**Discrepancias halladas en el borrador sin commit (nunca compilado) y corregidas:**
1. **sscanf.** El borrador llamaba al `sscanf` de la DLL (UCRT); el EXE usa el del CRT estático (0x8220AD) y la DLL debe hacer lo mismo (lección de `FileLoaderLoad2dEffect27`: el UCRT redondea los `%f` y trata los `%d/%x` de otra forma). Ahora el fuente llama a 0x8220AD en ambos lados.
2. **`GetHitSound`**: `static_cast<uint32>(n - 1)` desborda con signo en `INT_MIN` (comportamiento indefinido); ahora `static_cast<uint32>(n) - 1u > 7u` (el EXE hace `dec eax ; cmp eax,7 ; ja`).
3. **Contador de grupos**: `CAnimManager::GetAssocGroups().size()` devuelve el tamaño de un `span` sin signo; el EXE relee `[0xB4EA28]` en cada vuelta y compara **con signo** (0 o negativo → no entra). Ahora `static_cast<int32>(StaticRef<uint32>(0xB4EA28))` (el miembro es privado en `CAnimManager`).
4. Sin más diferencias en el orden de efectos, el reparto de offsets ni las constantes (0x858F10 = 1/30, 0x858B50 = 0.0 leídos del `.rdata` del EXE).

**Residuo de registros / llamadores.** Función `void`; `residue_scan.py 0x5BEDC0` muestra un único llamador directo (0x5BF9AF, dentro de una rutina de arranque); la instrucción siguiente es `call 0x438450`, que escribe EAX y ECX (`xor eax,eax ; mov ecx,17h`) antes de leerlos y no usa EDX: nadie lee EAX/ECX/EDX de salida. EAX/ECX/EDX de salida sólo se registran. Registros no volátiles conservados (EBX/ESI/EDI/EBP: prueba de ABI).

## Qué compara el banco (por lado; original == candidato == oráculo)

Original = bytes del EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskSimpleFight","LoadMeleeData",0x5BEDC0, RegisteredTarget{})`). La memoria que toca son **globales del EXE**: se observa la **región [0xC17090, 0xC17940)** (guarda 0x40 + tabla/niveles/CColModel estático 0x830 + guarda 0x40 = 0x8B0), rellenada con un patrón pseudoaleatorio (semilla del caso; el oráculo la reproduce y el banco registra su FNV-1a para comprobarlo) antes de cada lado y restaurada al final, y el contador 0xB4EA28 (fijado por caso y restaurado).
- `observed`: hash del flujo de eventos de los falsos (OpenFile con el hash de ruta/modo, cada LoadLine con el índice de línea o fin, CloseFile, cada GetAnimGroupName con su índice), `n_events/n_open/n_read/n_close/n_name`, `group_count_after`, **FNV-1a de 64 bits de la región entera**, número de palabras cambiadas y las primeras 48 (desplazamiento, valor) para diagnóstico.
- ABI: ESP (`popBytes = 0`), EBX/ESI/EDI/EBP, DF = 0, `fs:[0]`, x87 normalizado (la función usa `fmul/fstp`; deja la pila vacía), entradas 0x538900 / 0x536F80 / 0x5389D0 / 0x4D3A20 intactas, región y contador restaurados, ruta (`route_ok`).
- **Colaboradores sustituidos** (mismo falso en ambos lados, parche de entrada en el EXE y en el cuerpo de la DLL resuelto antes de parchear): `CFileMgr::OpenFile`, `CFileLoader::LoadLine` (sobrecarga de fichero), `CFileMgr::CloseFile`, `CAnimManager::GetAnimGroupName`. **Reales en ambos lados**: `sscanf` del EXE (0x8220AD, por dirección), `strncmp`/`strcmp` (puros).

### Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

El fichero es una lista de líneas (de 0 a ~150) y 1–6 nombres de grupo; el contador de grupos puede ser 0, negativo (0xFFFFFFFF, 0x80000000), menor o igual que los nombres. Familias: 0 canónico (1–12 combos + niveles + `END_MELEE_DATA`); 1 sin `END_MELEE_DATA`; 2 comentarios y líneas vacías; 3 basura y directivas casi iguales (`START_COMBOS` sí, `start_combo` no…); 4 líneas truncadas o con basura numérica; 5 cadenas de nivel de golpe; 6 sonidos/enteros extremos; 7 cuarto escalar (`f1c` ≤ 0 / > 0 / ausente); 8 más de 9 líneas, bloques cortos, `END_COMBO` suelto; 9 niveles con 0..9 vectores y basura (más de 7 pisa el CColModel); 10 13..15 combos (pisan niveles y CColModel); 11 búsqueda del grupo (primero, último, ninguno); 12 `START_*` dentro de bloques; 13 fichero vacío / sólo `END_MELEE_DATA` / sólo comentarios; 14–15 mezcla. Un generador (`Builder`) sólo emite líneas truncadas o garabateadas cuando el valor obsoleto que el EXE reutilizará ya lo definió una línea completa anterior (si no, el oráculo lanza `Fail` y `fixture_mirror.py` aborta). Mínimos de `run_local.py` ≈ 50 % de lo medido por `fixture_mirror.py`.

## Suposiciones y riesgos

- **Modelo de sscanf**: el oráculo sólo confía en enteros ≤ 12 cifras dentro de int32, reales con ≤ 9 cifras significativas en [1e-30, 1e30] (redondeo correcto), cadenas sin blancos y hexadecimales de ≤ 8 cifras con signo opcional (el CRT, como `strtoul`, acepta `-` en `%x`: complemento a dos; `selftest_source.py` lo confirma con el UCRT, pero **el CRT estático del EXE no se ha probado fuera del juego**; si difiriera, el banco lo mostraría como diferencia respecto al oráculo y no entre EXE y DLL, que usan el mismo sscanf).
- Antes de la primera lectura completa los escalares de sscanf son basura de pila en el EXE: el fuente los inicializa a 0 y el banco nunca depende de ellos.
- Más de 9 vectores de niveles o 15 combos escribirían fuera de la región observada: no se generan.
- La región incluye el CColModel estático de `FightStrike`: se restaura tras cada caso (nada más corre durante la sonda).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `selftest_source.py`, `register_hook.py`, `source.patch`, `README.md`.
