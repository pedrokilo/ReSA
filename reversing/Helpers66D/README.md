<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers66D: cuatro envolturas al EXE reconstruidas en C++ (lote 66D)

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wu-4`, HEAD 5765c0b5f + `source.patch`). 16384 pares (4 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers66d\20261021-112303-666131` (una corrida anterior, `20261021-111645-699502`, completó los 16384 pares sin diferencias pero terminó con la aserción ajena de `CShadowCamera::DrawOutlineBorder`; se repitió).

| Función | Dirección | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `RpClumpGetBoundingSphere(RpClump*, RwSphere*, bool)` | 0x734FC0 | `RwHelper` / `RpClumpGetBoundingSphere` | S | `RwHelper.cpp` |
| `CGarage::FindDoorsWithGarage(CObject**, CObject**)` (helper «camera door helper» sin nombre) | 0x449FF0 | `CGarage` / `FindDoorsWithGarage` | S | `Garage.cpp` (declaración ya en `Garage.h`) |
| `CCam::GetArrestCamCopSourceSide(CEntity*, CPed*, const CVector*, CVector*)` (helper `copSource` alternativo) | 0x516010 | `CCam` / `GetArrestCamCopSourceSide` | S | `Cam.cpp/.h` |
| `CHeli::CHeli(int32, eVehicleCreatedBy)` (la «CHeli» de 0x6C4190 es el constructor) | 0x6C4190 | `CHeli` / `Constructor` | S | `Entity/Vehicle/Heli.cpp` |

Filas de `docs/hooks.csv` que debe añadir el coordinador (ninguna es virtual): `RwHelper,RpClumpGetBoundingSphere,0x00734FC0,1,0,1,S`, `CGarage,FindDoorsWithGarage,0x00449FF0,1,0,1,S`, `CCam,GetArrestCamCopSourceSide,0x00516010,1,0,1,S`, `CHeli,Constructor,0x006C4190,1,0,1,S`.

Producción: `X87Math.cpp/.h` (bloque «Lote 66», prefijo `Hp66`, al final), `RwHelper.cpp`, `Garage.cpp`, `Camera.cpp` (`CameraControlGarageDoors` ya no llama por dirección), `Cam.cpp/.h`, `Heli.cpp`, `Plane.cpp` (el `0x6C4190` por dirección pasa a `::new(storage) CHeli(modelo, RANDOM_VEHICLE)`).

## Qué hace cada una (desensamblado)

- **0x734FC0 esfera envolvente** (cdecl). Guarda el byte bajo del tercer argumento en el global 0x8D60BC; sin clump o sin esfera devuelve 0. Pone la esfera a 0 (x, y, radio, z); `n = RpClumpGetNumAtomics`; `n < 1` devuelve 0. Acumula con `RpClumpForAllAtomics(clump, 0x734970, &suma)`, `inv = 1/float(n)` y `suma[i] = float(suma[i]*inv)` (x87 de 24 bits, x, y, z), segunda pasada con 0x734ED0 sobre `{centro, radio 0}` (calcula el radio), y transforma el centro con la **inversa** (0x7F2070 es `RwMatrixInvert(out, in)`, no una copia) de la LTM del marco del clump (global ≠ 0, `RwFrameGetLTM`) o de su matriz local (marco + 0x10), con `RwV3dTransformPoints(&c, &c, 1, &m)`. Devuelve el clump. Los dos callbacks (0x734970, 0x734ED0) siguen siendo código del EXE (se pasan por dirección; fuera del alcance de este lote).
- **0x449FF0 puertas del garaje** (thiscall, `ret 8`). Pone `*segunda = *primera = 0`; esquina `P = (x + dirA.x·w/2 + dirB.x·h/2, y + dirA.y·w/2 + dirB.y·h/2)` con los redondeos a float del EXE (sólo `T1`, `Ax`, `T2` y el resultado se guardan); recorre el pool de objetos (0xB7449C; elementos de 0x19C) de la última ranura a la primera y salta ranuras libres (bit 7), punteros nulos y objetos cuyo byte +0x147 ≠ low8 de `(this − 0x96C048) / 0xD8` (`sizeof(CGarage)`, imul 0x4BDA12F7 + sar 6, con signo). Distancia plana a P (float guardado), mejores dos: primera vacía → primera; `d < d1` → segunda = primera, primera = o; si no, sin segunda o `d < d2` → segunda. NaN nunca es menor. Pool vacío termina sin recorrer.
- **0x516010 cámara de arresto, posición lateral** (thiscall, `ret 10h`; `this` sin usar; `target` sólo se comprueba que no sea nulo). `v = (dest − cop, 0)` normalizado (0x59C910); `source = dest + 5·v` (z con el producto guardado como float); `c = CrossProduct(v, (0,0,1))` (0x59C730); `source.xy += 10·c`; `source.z = dest.z + 5`; `FindGroundZFor3DCoord(source, &found, 0)` (0x5696C0) y, si `found` ≠ 0 (byte), `source.z = 0,7 + suelo`. Devuelve true (false con `target` o `cop` nulos). Nombre elegido: coloca la cámara detrás del objetivo y desplazada hacia un lado (la secuencia alternativa de `ProcessArrestCamOne`).
- **0x6C4190 `CHeli::CHeli`** (thiscall, `ret 8`). Constructor base `CAutomobile(model, createdBy, 1)` (0x6B0A90) y campos del helicóptero; con el Hunter (0x1A9) `SetDoorStatus(2, 0)` (0x6C21C0) y los campos de la puerta; `rand()` del CRT del EXE (0x821B1E) al final para `[+0xA14] = float(rnd·(1/32767)·6 + 2)`. EAX = this.

## Revisión propia contra el EXE (fase 2) y defectos reales hallados

Se desensambló de nuevo cada rango (0x734FC0..0x73513F, 0x449FF0..0x44A16C, 0x516010..0x51618F, 0x6C4190..0x6C431B y los callbacks 0x734970/0x734ED0) y se contrastó línea a línea; después se compararon por el banco. Defectos reales (el constructor ya existía en C++ sin hook ni banco):

1. **`CHeli`: el ángulo de la puerta del Hunter.** El EXE guarda `0x3F71463B`; el C++ calculaba `(3·PI)/10` (`0x3F71462D`). Corregido con el valor exacto.
2. **`CHeli`: ceros que faltaban.** El EXE pone a cero 0x9C0..0x9EF; el C++ sólo ponía los dos historiales. Faltaban `field_9C0`, `f9CC`, `f9D0` y `m_pParticlesList`.
3. **`CHeli`: `rand()` de la DLL.** `GetRandomNumberInRange` usaba el `rand()` de la DLL (otra semilla); ahora el del EXE (0x821B1E) y el lerp x87 `GeneralRandomInRangeDblCpp`.
4. Durante la escritura: el indice del garaje divide entre 0xD8 (no 0x19C: éste es el tamaño del elemento del pool de objetos); 0x7F2070 es `RwMatrixInvert`. El banco lo habría detectado (el oráculo recomputa el índice desde `this`).

## Llamadores del EXE y registros

`residue_scan.py`: 0x734FC0 (0x70655A), 0x449FF0 (0x44A6C6, 0x44A70A, 0x44A7AD, 0x44A7F1, 0x44BF43, 0x52838A, 0x52921D, 0x529954) no leen EAX/ECX/EDX; 0x516010 (0x51899F) lee sólo AL (se exige, `ret_al`); los doce llamadores de 0x6C4190 leen EAX = this (se exige, `eax_is_this`). El aviso de ECX en 0x45AB5A es falso: `push ecx` reserva el hueco de un argumento float que se sobrescribe con `fstp [esp]` (FindGroundZ), no lee el ECX del constructor. Por eso `kCompareVolatileRegisters = false` y los residuos sólo se registran.

## Qué compara

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con sólo ese hook encendido. Por lado: traza plana de los colaboradores sustituidos (falsos idénticos en la dirección del EXE y en el cuerpo de la DLL, argumentos leídos en la llamada; punteros de la arena como etiqueta), esfera / puertas / `source` / objeto de 0xA18 bytes (vptr propio de cada lado como etiqueta), resto de la arena y guardas, globales (0x8D60BC, 0xB7449C, 0xB7CB84 fijados por caso y restaurados), compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado con FCW 24 bits, ruta del hook, colaboradores) y entradas de código vigiladas. Colaboradores (13): `RpClumpGetNumAtomics` 0x7498E0 (con cuerpo en la DLL), `RpClumpForAllAtomics` 0x749B70, `RwFrameGetLTM` 0x7F0990, `RwMatrixInvert` 0x7F2070, `RwV3dTransformPoints` 0x7EDD90; `CVector::Normalise` 0x59C910, `CrossProduct` 0x59C730, `CWorld::FindGroundZFor3DCoord` 0x5696C0 (con cuerpo en la DLL); `CAutomobile::CAutomobile` 0x6B0A90, `CDamageManager::SetDoorStatus` 0x6C21C0 (con cuerpo en la DLL), `rand` 0x821B1E. El garaje no tiene colaboradores: el pool de objetos real se sustituye por uno de la arena durante la llamada.

Fixtures (`Fixtures.hpp`, 16 familias por función, 256 casos cada una): esfera (clump/esfera nulos, `n ≤ 0`, `n` grande con redondeo de float(n), sumas subnormales/enormes/NaN/inf, byte LTM con dwords no canónicos, marcos nulos/basura, radio y centro modificados por el falso), garaje (pool de 0..12 ranuras, banderas, byte del garaje, matriz o posición, empates, NaN/inf/subnormales/enormes, objeto en la dirección 0, garajes de datos constantes del EXE con diferencia negativa), cámara (nulos, geometría normal/NaN/inf/subnormal/enorme, `found` 0/1/no canónico/sin escribir, falsos que sustituyen v y c), constructor (Hunter, alias de 8/16 bits del modelo, `createdBy` 0..255, `rand` extremos, temporizador, 4 desplazamientos). `fixture_oracle.py` modela las cuatro desde el desensamblado (x87 de 24 bits con exponente extendido, ceros con signo, NaN, doble redondeo a float; autoprueba con `python -I fixture_oracle.py`); `fixture_selfcheck.py` compila `fixture_dump.cpp` (Fixtures.hpp tal cual), compara las constantes de arena con las del oráculo, pasa los 16384 casos por el oráculo (sin ambigüedades de NaN) y comprueba los mínimos de `run_local.py` (≈50 % de lo medido).

## Suposiciones y límites

- Los callbacks 0x734970 y 0x734ED0 no se ejecutan (el falso de `RpClumpForAllAtomics` escribe lo que dicta el caso); su validación es otra tarea.
- `createdBy` sólo en 0..255 (`eVehicleCreatedBy` es `uint8`; el EXE reenvía el dword completo al constructor base; los llamadores del EXE pasan constantes pequeñas). `bUseLTM` se prueba también con dwords no canónicos: sólo importa el byte bajo y la DLL lo conserva.
- Garaje: `this` en la arena o en datos constantes del EXE (cubre diferencias negativas); no se prueban garajes reales de `CGarages::aGarages`. Pool con tamaño ≤ 12 y sin tamaños negativos.
- `FindGroundZFor3DCoord` devuelve ST0 como float (el EXE real podría devolverlo sin redondear; el banco usa float-exacto). NaN sólo silenciosos y con significandos distintos entre sí (la x87 no define el empate de distinto signo).
- El vptr del constructor es el de la vtable de la DLL (capturado una vez). Una aserción ajena de `CShadowCamera::DrawOutlineBorder` puede aparecer tras completar los pares (repetir).

## Comandos

```
pwsh -NoProfile -File D:\Original\wu-4\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh Helpers66D          # compila la sonda y corre en el juego (candado global)
python -B reversing/Helpers66D/run_local.py --analyze "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66d/20261021-112303-666131/comparisons.jsonl"
python -I reversing/Helpers66D/fixture_oracle.py          # autoprueba del oráculo
python -I reversing/Helpers66D/fixture_selfcheck.py       # volcado compilado de Fixtures.hpp + oráculo + mínimos (imprime «ok»)
python -B reversing/Helpers66D/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66d/20261021-112303-666131"   # sólo el coordinador (SCOPE_NOTE ya relleno)
```
Archivo de arranque en el clon: `helpers66d-runtime.start`; salida `helpers66d-runtime.jsonl`.
