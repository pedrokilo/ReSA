<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FileLoaderLoad2dEffect27: CFileLoader::Load2dEffect (0x5B7670)

**Estado: APROBADO** (18/10/2026, corrida `20261018-160630-995451`: 6144 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-fileloader2dfx2761018-160630-995451`; archivado en `reversing/CppTwelveLevel6Final/runs/FileLoaderLoad2dEffect27-20261018-160630-995451/`. Sonda compilada desde el worktree D:\Original\gta-wt-fl con `FileLoader.cpp` idéntico byte a byte (sha256 8ca2d704...) al del árbol principal. Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CFileLoader,Load2dEffect,0x005B7670,1,0,1,S` en `docs/hooks.csv`).

**Decisión del sscanf (nota, no evidencia).** La primera corrida (`20261018-160456-540095`), con `Load2dEffect` llamando al `sscanf` del UCRT de la DLL, FALLÓ: 338 diferencias de 6144 en tokens `%f`/`%d` límite (redondeo decimal->float y desbordes de `%d`). Se conserva sólo como nota y no cuenta como evidencia. La fuente aprobada llama al `sscanf` del EXE por dirección (`0x8220AD`, CRT estático del juego), igual que `Stats.cpp`, y con eso la corrida `20261018-160630-995451` dio 0 diferencias.

**Alcance.** Este banco cubre únicamente `CFileLoader::Load2dEffect`. Las otras ~40 llamadas a `sscanf` de `FileLoader.cpp` (otras funciones del cargador) siguen usando el CRT de la DLL y NO están cubiertas por este banco; tienen el mismo riesgo y sus bancos no cubren tokens límite.

Plantillas: `reversing/DecisionAdd27` y `reversing/DecisionMakeDecision27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` (el banco usa `__asm` en `Gate`/`SaveFP`, igual que los demás; los avisos «SUPERADO» sólo valen para producción). `reversing/FileLoader47` sustituía `Load2dEffect` por un stub (`l2dfx{0x5B7670,0}`): esta función no tenía banco.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FileLoaderLoad2dEffect27          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FileLoaderLoad2dEffect27/build_probe.py
python -B reversing/FileLoaderLoad2dEffect27/run_local.py
python -B reversing/FileLoaderLoad2dEffect27/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (escribe diferencias_crt.json si hay pares distintos)
python -I reversing/FileLoaderLoad2dEffect27/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/FileLoaderLoad2dEffect27/fixture_mirror.py                                              # espejo del generador: 0 casos inválidos + cobertura (sin juego)
python -B reversing/FileLoaderLoad2dEffect27/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(Load2dEffect, 0x5B7670)` (ya está en `FileLoader.cpp`, cambio sin commit; categoría/clase `CFileLoader`, hook `Simple`). Archivo de arranque en el clon: `fileloader2dfx27-runtime.start`; salida `fileloader2dfx27-runtime.jsonl`. 6144 pares. `docs/hooks.csv` no tiene la fila `CFileLoader,Load2dEffect,0x005B7670,1,0,1,S` (añadida al registrar).

## Qué hace la función (desensamblado 0x5B7670..0x5B8003, tabla de saltos en 0x5B8004)

`void Load2dEffect(const char* line)`, cdecl, `ret` normal (termina con `jmp 0x7316B0` = `CTxdStore::PopCurrentTxd`). Marco de 0x1CC bytes.
1. `sscanf(line,"%d %f %f %f %d",&modelId,&x,&y,&z,&type)` (`0x8220AD`, CRT estático del EXE). `PushCurrentTxd` (0x7316A0); `SetCurrentTxd(FindTxdSlot("particle"))` (0x731850/0x7319C0).
2. `mi = ms_modelInfoPtrs[modelId]` (0xA9B0C8, sin comprobar el índice). `store = Get2dEffectStore()` (0x4C5A60); `e = store+4+(count<<6)`; `++count`; `mi->Add2dEffect(e)` (0x4C4D20, thiscall).
3. `*(float*)e = x,y,z` (dwords), `e[0xC] = (u8)type`. `cmp type,0Ah ; ja` (SIN signo) y salto por tabla: 0 luz, 1 partícula, 3 atractor, 5 cartel, 6 tragaperras, 7 interior, 8, 9, 10; 2, 4 y >10 no leen nada más. Todas las ramas acaban en `PopCurrentTxd`.
4. Los campos se escriben **por desplazamiento**: `sscanf` escribe directamente en el bloque en los campos marcados «d»; los `%d` intermedios van a locales de las que sólo se guarda el byte (B) o la palabra (W) bajos. Destinos (verificados con un script que sigue los `lea/push` y las lecturas de pila del desensamblado):

| tipo | formato tras `type` | destinos en el bloque |
|---|---|---|
| 0 luz | sscanf1 `%d`×4 (r g b a); dos cadenas `"..."` buscadas con un bucle sobre la memoria **sin comprobar el fin de la cadena**; sscanf2 (tras la 2.ª comilla de cierre) `%f`×4 `%d`×9 | d: 14 18 1C 20 (floats). B: 10←r 11←g 12←b 13←a 29 26 27 28 2A 2B 2C 2D. W: 24←flags. 30,34←`RwTextureRead(corona,0)`, `RwTextureRead(sombra,0)`; si `(u8)flags & 4`: `[24] &= 0xFD` |
| 1 partícula | `%s` | d: nombre + NUL en 10, sin límite |
| 3 atractor | `%d` `%f`×9 `%d %d %s` | d: 10..30 (floats). B: 34 35 36. strcpy del nombre a 38 (con NUL, sin límite) |
| 5 cartel | `%d`(d) `%f`×3 (tamaños, **sin inicializar**) `%f`(d) `%d`×28 | d: 10 (dword), 30 (float). Tamaños → `_ftol2` (0x821B40) → byte en 12 13 14. Ints a10→15 y a11→2E (**sin inicializar**), a12→11, a13→2F (**defecto 0x32**), a14→16 17 1A 1B 18 19 1C 1D 20 21 1E 1F 22 25 28 2B 23 26 29 2C 24 27 2A 2D; defecto **-1** (25 locales) |
| 6 tragaperras | `%f`×7 `%d %d %s %d` | d: 10..28. W: 2C←v2c. B: 2E←v2e, 2F←v2f. `strncpy(30, nombre, 8)` (0x821F40, rellena con ceros hasta 8) |
| 7 interior | `%f`×5 `%d %s %s %s %s` | cuatro buffers de 33 bytes (byte0=' ', resto 0). F: 10..20. W: 24←f1a4. 28←`Malloc(0x40,0)` (0x72F420); 4×16 bytes de los buffers; dword[2C]=0 |
| 8 | `%d` | F: 10 |
| 9 | `%f %f %d` | F: 10, 14. B: 18 |
| 10 | `%f`×9 `%d` | F: 10 14 18 1C 20 24 28 2C 30 (en orden). B: 34←(v≠0) |

Orden observable: sscanf → Push → Find("particle") → Set → Get2dEffectStore → (contador++) → Add2dEffect → [sscanf de la rama / texturas / Malloc] → Pop. Nota de solapes: en el cartel el dword directo de 10 queda pisado por los bytes 11 (a12), 12 y 13 (tamaños); el oráculo respeta ese orden.

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **Entrada k** (0x40 bytes) y **las dos siguientes** (128 bytes: los desbordes de partícula/atractor caen ahí), el **contador** (k+1), que **nada fuera** de esas tres entradas ni del contador cambie (`outside_ok`, `first_outside`), y la **línea** (arena de entrada intacta, `line_ok`).
- **Traza** de colaboradores (idéntica en ambos lados y al oráculo): `[1]` Push; `[2,fnv32("particle"),8]` Find; `[3,resultado]` Set; `[4]` Store; `[5,etiqueta del model info o 0xFFFF,4+0x40·k,k+1]` Add2dEffect; `[6,ordinal,fnv32(nombre),1]` RwTextureRead (segundo argumento `nullptr`); `[7,0x40,0]` Malloc; `[8]` Pop. Más los nombres (hex) de las texturas y de `FindTxdSlot`, el **bloque de Malloc** (64 bytes + guardas) y que `ms_modelInfoPtrs[modelId]` siga intacto.
- Los punteros se comparan como etiquetas: el de `RwTextureRead` es `0x4E000000+((ordinal+1)<<16)+(fnv32(nombre)&0xFFFF)` (distinto y determinista, lo calcula el oráculo); el del bloque de Malloc en `+0x28` se sustituye por `0xB10C0000` si coincide con el bloque.
- ABI: ESP (`ret` normal: `popBytes=4` en `Gate`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW completa, TOP, tags y ST válidos: no debe quedar ninguno; MXCSR sin banderas). FCW fijada a 24 bits, redondeo al más cercano y todas las excepciones enmascaradas (estado del juego): el `sscanf` y el `_ftol2` del EXE corren con ella.
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`; `kCompareVolatileRegisters=false`). `python -I reversing/Dependencies25A/residue_scan.py 0x5B7670` da un único llamador (0x5B86B7) sin lecturas de EAX/ECX/EDX tras el `call`; **el revisor debe confirmarlo a mano** (el escáner marca `CALL@5b8603:unwritten=ecx`).
- Rutas (`route_ok`): original → el hook de 0x5B7670 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; **en ambos lados** 0x8220AD (sscanf), 0x821B40 (`_ftol2`) y 0x821F40 (strncpy) no están enganchados. Se vigilan 16 bytes de esas tres entradas, de 0x5B7670 y de los ocho colaboradores parcheados (deben seguir siendo el código original al volver cada llamada).

## Colaboradores

**Sustituidos** (mismo falso en ambos lados, parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL, resuelto antes de parchear; activos sólo durante la llamada): `CModelInfo::Get2dEffectStore` 0x4C5A60 (devuelve una tienda propia: `[u32 contador][16 bloques de 0x40]` entre guardas, así no se toca la tienda real 0xB4C2D8), `CBaseModelInfo::Add2dEffect` 0x4C4D20 (sólo traza; `this` es un model info falso de 16 posibles o `nullptr`), `CTxdStore::PushCurrentTxd` 0x7316A0, `FindTxdSlot(const char*)` 0x731850 (devuelve un valor por caso, que debe llegar a `SetCurrentTxd`), `SetCurrentTxd` 0x7319C0, `PopCurrentTxd` 0x7316B0, `RwTextureRead` 0x7F3AC0, `CMemoryMgr::Malloc(size,hint)` 0x72F420. Consecuencia: **no se valida aquí** el cuerpo real de `Add2dEffect` ni del almacén de TXD; sólo que `Load2dEffect` los llama igual. `collaborators_stubbed=true` en `begin`.
**Reales** (se ejecutan de verdad): `sscanf` (EXE 0x8220AD / DLL: el de su CRT), `_ftol2` 0x821B40 (sólo el EXE; la DLL convierte con su CRT), `strncpy` 0x821F40 (sólo el EXE). `ms_modelInfoPtrs[modelId]` (0xA9B0C8) es la única global del juego que se escribe (por caso, con restauración; ids 0..19999).

## sscanf: ¿el mismo en los dos lados? NO (y el banco no lo oculta)

- **EXE**: llama a `0x8220AD`, el `sscanf` del CRT estático del juego (MSVC viejo, ya conocido en `Stats.cpp` y `WaterLevel.cpp`, que lo invocan explícitamente por esa dirección).
- **DLL**: `FileLoader.cpp` llama a `sscanf(...)` a secas (los 40 usos del archivo, no sólo éste): el de su propio CRT, **UCRT estático de depuración** (`CMAKE_MSVC_RUNTIME_LIBRARY = MultiThreadedDebug`, `CMakeLists.txt:26`).
- Diferencias plausibles (no verificadas; el sondeo las mostrará): `inf`/`nan`/`1#INF`/flotantes hexadecimales (`0x1p3`) que UCRT acepta y el viejo no; redondeo decimal→float (UCRT correcto; el viejo pasa por 80 bits → doble redondeo en cadenas justo a un lado de la mitad); desbordamiento de `%d` (envoltura vs otra política); `1e`/`1e+` sin dígitos de exponente; `-0`; cadenas muy largas.
- **Diseño**: (1) el evento `begin` lleva un **sondeo del CRT** (`crt.probe`, 66 textos × `%d/%f/%s`, ambos `sscanf` lado a lado, con el FCW del juego), independiente de `Load2dEffect`; `run_local.py` lo resume (`crt_probe_differences`) sin convertirlo en compuerta. (2) Los fixtures usan tokens del **texto común** (enteros int32 y reales de ≤9 cifras significativas) y, aparte, tokens **sensibles al CRT** en campos cuyo valor sigue siendo definido en ambos lados: reales «a mitad de camino» entre dos floats ±1e-n (`kHalfway`, 30 literales calculados con aritmética exacta), enteros fuera de int32 (`kIntOverflow`), tamaños de cartel en la frontera entera (`255.999993`, `0.99999999999999999999`...), tipos desbordados (`kTypeOverflow`, todos caen en «tipo desconocido» con envoltura o saturación). (3) El oráculo trata esos tokens como **desconocidos** (bytes `??`), no los resuelve: lo que decide es la igualdad EXE == DLL, que sigue siendo estricta. (4) Si el banco falla, `run_local.py` escribe `diferencias_crt.json` con el desglose por grupo / familia / clase de CRT (`solo_texto_comun` = diferencia real de lógica; `crt_sens_float`, `crt_sens_int`, `crt_unknown`, `crt_plain_fail` = candidata a diferencia de CRT) y las claves que difieren, y sale con error igual. (5) Si salen diferencias de CRT, el arreglo de fuente es el que ya usan `Stats.cpp`/`WaterLevel.cpp`: llamar al `0x8220AD` del EXE (`reinterpret_cast<int(__cdecl*)(const char*,const char*,...)>(0x8220AD)`) en `Load2dEffect`; el banco se repite entonces con `exeOnly` ya correcto.
- Tokens que podrían **fallar** en un CRT y no en otro (`inf`, `nan`, hex, `1e`...) sólo aparecen donde un fallo deja un estado definido: campos del cartel con valores por defecto (familias 9–12), y los bytes que serían basura de pila se enmascaran (ver abajo). Nunca en x/y/z/type ni en tipos cuyo resto de campos quedaría sin inicializar.

## Fixtures (`Fixtures.hpp`): 6144 casos = 12 grupos × 512, 16 familias por grupo (32 repeticiones)

Grupo `n % 12`: 0..10 = tipo 0..10; 11 = tipos fuera de rango (`-1, 11, 12, 13, 15, 16, 31, 32, 63, 64, 127..129, 254..258, 383, 384, 511..513, 1000, 1023, 1024, 4095, 4096, 65535..65538, 16777216/7, 2147483647, -2147483648, -2, -100, -255..-257, -65536, -65535, +11` y ocho desbordamientos de int32); se comprueba el truncado a byte de `e[0xC]` con rama por defecto (256→0, 257→1, 511→255...). Por caso: `modelId` (0, 1, 2, 7, 100, 1000, 4095, 10000, 18000, 19998, 19999 o aleatorio 0..19999, con `+` o ceros a la izquierda en una cuarta parte de las familias), model info falso 0..14 o `nullptr`, contador previo k=0..12, bloque previo con **patrón por caso** (todo 0x00, todo 0xFF, bytes distintos por posición y caso, constante por caso, o aleatorio) y entradas k+1/k+2 aleatorias para ver desbordes, resultado de `FindTxdSlot`, semillas de pila y EAX/ECX/EDX.

- **Luz (0)**: nombres normales, vacíos (`""`), corona vacía + sombra de 60–100, corona de 60–90, **segundo par ausente**, **ambos ausentes** (lee la cola `tail` como el EXE, que no comprueba el fin), comillas pegadas (`"a""b"`), puntuación y espacios en los nombres, separadores aleatorios, flags con bit 4 (`4+8k`), 4/5/6/7/255..65540/-1/-4/-253/4294967295 (palabra y byte bajo), r g b a y v* con bordes/desbordes, reales «a mitad».
- **Partícula (1)**: nombres de 1..7, 7–8, 9–20, 21–46, 47 (llena justo el bloque), 48–62 y 63–100 (desbordan a la entrada siguiente), sin nombre (conserva el estado previo), punteo, numéricos (`1.5`, `0x10`, `nan`...), tokens `%s%d`, líneas con cola.
- **2 y 4**: sólo el prefijo; colas de todo tipo (comillas, `%s %d`, 250 caracteres), coordenadas planas / decimales / enteras / «a mitad» / exponentes / ±1e30 / ±1e-30; `type` con `+`, ceros a la izquierda, `2.7`...
- **Atractor (3)**: v34/v35/v36 con bordes y desbordes, floats de todas las clases, nombre 1..7 (cabe), 8..30 y 31..100 (desbordan a k+1/k+2).
- **Cartel (5)**: línea completa; **truncada** tras a11 (siempre se conservan a5 sx sy sz f30 a10 a11; el resto toma -1 y 0x32 en a13); tamaños de `kSizeEdge` (0, 0.5, 255.9, 256, 3e9, 9.2e18, 1e19, 1e30, negativos...), a5 con bordes, f30 variado, ints con bordes y desbordes, tokens exóticos de entero (`3.7`, `0x1F`, `1e3`, `+`, `-`, `abc`, `08`...) en una posición a12..a37 (el resto queda en -1), **familias con máscara** (10: real exótico en f30; 11: real exótico en el tamaño X; 12: entero exótico en a5), tamaños «a un paso de la frontera», espacios dobles.
- **Tragaperras (6)**: nombres de 1..7, 8, 9–30, 31–100 (`strncpy(...,8)`), v2c de palabra (65535/65536/70000/-1), v2e/v2f de byte, desbordes, tokens con `+`/ceros.
- **Interior (7)**: 0..4 nombres (y un 5.º ignorado), 15–17 y 18–32 caracteres (copia de 16 bytes), f1a4 de palabra, reales «a mitad», puntuación, nombres de 1 carácter y dígitos.
- **8, 9, 10**: v con bordes, `+`, ceros, negativos, prefijos válidos de `%d` (`3.9`, `1e3`, `0x10`, `7abc`), desbordes (4294967296: 0 con envoltura de 32 bits, ≠ 0 con saturación → `setne`); floats planos y «a mitad».

**Reglas de seguridad** (el banco jamás depende de comportamiento indefinido por pila): los cinco primeros campos se leen siempre; toda local sin inicializar del EXE llega siempre leída; ninguna cadena rebasa los buffers medidos en el desensamblado (corona ≤90, sombra ≤100, partícula/atractor/tragaperras ≤100, interior ≤32); sólo ASCII imprimible (isspace con `char` negativo hace saltar asserts del CRT de depuración de la DLL); la cola de la luz garantiza que el recorrido de comillas termina dentro de la arena. El oráculo (`Fail`) rechaza como fixture inválido cualquier caso que lo incumpla, y `fixture_mirror.py` comprueba que el generador no produce ninguno.
**`mask`** (sólo familias 10, 11 y 12 del cartel, 96 casos): bit i = el byte i del bloque es basura de pila en el EXE si la lectura falla (`+0x15`, `+0x2E`, y los bytes de tamaño `+0x12..+0x14` cuando falla el tamaño X o a5). Esos bytes se ponen a 0 en la observación de **ambos** lados y en el oráculo; el resto del bloque (incluido `+0x30` y los -1 por defecto) se compara entero, así que «un CRT sigue leyendo y el otro se detiene» se detecta igual.

`run_local.py` exige cobertura mínima por grupo/familia/rama/clase (tipos fuera de rango y truncados, nombres por tramo, luz con comillas ausentes/vacías/pegadas, flags con bit 4, cartel completo/truncado/con defecto a13/tamaños ≥256/negativos/enormes, tokens sensibles al CRT, máscara, contador previo 0..12, `mi==nullptr`, ids 0 y 19999, patrones previos 0x00/0xFF...). **Los mínimos se calibraron con `fixture_mirror.py` (espejo en Python, mismas familias y clases pero no el mismo orden de RNG) dejando ~40–50 % de margen; no se comprobaron contra el generador C++.** Si alguno falla por poco, bajar el umbral o ajustar la familia.

## Fuente actual (`FileLoader.cpp` con cambios sin commit): concordancias y discrepancias vistas (otro agente revisa la fuente)

- **Coinciden con el desensamblado** (comprobado campo a campo con el script de mapeo): todos los desplazamientos, anchuras (byte/palabra/dword), orden de las operaciones (Push → Find → Set → AddItem → Add2dEffect → rama → Pop), el truncado `(uint8)type`, el salto sin signo (`ja`), `flags & 4` sobre el byte bajo, `strncpy(…,8)`, la copia de 16 bytes por nombre y `Malloc(0x40,0)`, `e[0x34] = v != 0`.
- **Diferencias deliberadas/benignas que los fixtures evitan**: (1) cartel: la DLL inicializa a 0 `f184`, `f180` y los tres tamaños; el EXE **no** (son basura de pila si la línea se corta antes): las líneas truncadas del banco conservan siempre hasta a11. (2) tragaperras: la DLL inicializa v2c/v2e/v2f a 0; el EXE no. (3) interior: la DLL pone a cero 128 bytes por nombre; el EXE sólo 33 (se copian 16, igual hasta 32 caracteres) y sus buffers son de 36 bytes (>35 caracteres pisa el siguiente buffer del EXE). (4) tamaños de cartel: `(int64)float` de la DLL frente a `_ftol2`: coinciden en el byte bajo (NaN/inf/|x|≥2^63 → 0).
- **Pendiente real**: `sscanf` del CRT de la DLL (ver arriba). `docs/hooks.csv` sin fila de `Load2dEffect`. Varios `sscanf` de otras funciones de `FileLoader.cpp` tienen el mismo riesgo y sus bancos no cubren estos tokens.

## Suposiciones y riesgos al compilar/correr

- Sin compilar: posibles errores de sintaxis en `Core.hpp`/`LiveComparison.hpp`. Puntos a vigilar: `std::bit_cast` de punteros a función / a miembro (`&CBaseModelInfo::Add2dEffect`, `&RwTextureRead` si fuera `extern "C"`), `static_assert(offsetof(CStore<C2dEffect,100>, m_aObjects)==4)` y `sizeof(C2dEffect)==0x40` (`LiveComparison.hpp`), listas de inicialización de nlohmann con enteros y cadenas mezclados, `__fastcall` del falso de `Add2dEffect` (equivale a thiscall con un argumento y `ret 4`), avisos de `sscanf` obsoleto.
- Los parches de entrada exigen ≥5 bytes por función y que el cuerpo de la DLL no esté en línea: `Get2dEffectStore` es una función suelta de `ModelInfo.cpp` (no se inlina sin LTCG). Si la DLL llamara a `Add2dEffect`/`Get2dEffectStore` en línea, `trace` lo delataría (faltarían eventos).
- `mi == nullptr` (1/16 de los casos) supone que la DLL no desreferencia el model info antes de llamar al `Add2dEffect` sustituido (es una llamada no virtual).
- `ms_modelInfoPtrs` tiene 20000 entradas (`NUM_MODEL_INFOS`); los ids del banco están en 0..19999.
- El EXE jamás se prueba con `Add2dEffect` real, `Get2dEffectStore` real ni TXD reales; sólo se verifica el uso (ver «Colaboradores»).
- Si `HookRoute` no encuentra el hook (`registered_hook=false`) no se ejecuta ningún caso y `run_local.py` lo rechaza.
- Un fallo de los 12 vigilados (p. ej. un hook de otra función que reescriba una de las entradas parcheadas) se refleja en `globals_restored`.
- No hay arnés `offline/`; sólo el oráculo y el espejo se autoprueban.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `README.md`.
