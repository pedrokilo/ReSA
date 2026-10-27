<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FightHitObj72: CTaskSimpleFight::FightHitObj 0x61D400 (lote 72)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 95f7d73eb + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fighthitobj72\20261026-204314-530849`. (Corridas previas descartadas por defectos del banco, ver «Hallazgos».)

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `void CTaskSimpleFight::FightHitObj(CPed*, CObject*, CVector&, CVector&, uint32 component, uint32 surface)` (antes la envoltura libre `FightHitObjExe`) | 0x61D400 | `CTaskSimpleFight` / `FightHitObj` / S (`RH_ScopedInstall`) | `Tasks/TaskTypes/TaskSimpleFight.cpp/.h`, ayudantes en `X87Math.cpp/.h` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CTaskSimpleFight,FightHitObj,0x0061D400,1,0,1,S`.

## Que hace (desensamblado 0x61D400..0x61D5EB, 494 bytes, thiscall `ret 0x18`, ECX = tarea, sin valor de retorno)

Unico llamador en el EXE: 0x6245AF (`FightStrike`, ya en C++): nadie lee EAX/ECX/EDX. El argumento `component` (pila +0x14) no se lee.

1. Copia `*posn2` (tres `mov` enteros, bits crudos) a una variable local ANTES del danio; esa copia es la que usan la fuerza, `ObjectDamage` y `AddPunchImpact`. `posn1` se lee por el puntero cuando se usa.
2. `damage = FightStrikeDamage(tarea, creador)` (0x61C740, ST0 guardado como float).
3. Puertas: `byte [obj+0x144] < 0xC8` y `!(m_nPhysicalFlags & 4)` y `m_pObjectInfo->m_fColDamageMultiplier < 99.9f` (0x86D6B0; comparacion ordenada: NaN no pasa).
   - Si `m_nFlags & 4` o `& 0x40000` y `m_fUprootLimit <= 0.0f` (ordenada): `SetIsStatic(false)` (vtable +0x10) y `AddToMovingList` (0x542800).
   - Las banderas se RELEEN despues; si no tienen 4 ni 0x40000: `ApplyForce(fuerza = normal * f, punto = posn1 - posicion, true)` (0x542B50) con `f = -0.5f` (0x858F40) o `-0.1f` (0x858EF4) si el bit 7 del byte `+0x40`; posicion = matriz+0x30 o `+4`. Cada operacion en x87 a 24 bits guardada como float.
4. `ObjectDamage(danio * 10.0f, &posn1, &copia, creador, tipoArma)` (0x5A0D90; 0x85862C = 10.0f; tipo de arma = dword `[creador + 0x5A0 + movsx(byte [creador+0x718]) * 0x1C]`).
5. Si el tipo de arma (releido) es 9 (motosierra): `AddAudioEvent(0x9B)` del audio de arma (0x4E69F0). Sonido de golpe: `AddAudioEvent(tabla[combo*0x22+movimiento], 0, 1.0f, objeto, superficie, 0, 0)` (0x4E2BB0, tabla 0xC16F0C; combo y movimiento con signo, sin acotar) y `Fx_c::AddPunchImpact(posn1, copia, 4)` (0x49F670).

## Cambios de produccion (`source.patch`)

- `TaskSimpleFight.cpp/.h`: el metodo `CTaskSimpleFight::FightHitObj` en C++ puro (sin `__asm`/`naked`) con `RH_ScopedInstall(FightHitObj, 0x61D400)`; se elimina la envoltura `FightHitObjExe` y `FightStrike` llama al metodo. La declaracion pasa de `(…, int16, int8)` a `(…, uint32 component, uint32 surface)`.
  Comparaciones con `GangDriveByLtCpp`/`GangDriveByLeCpp` (x87 ordenadas ya existentes); la copia de `posn2` se hace con enteros.
- `X87Math.cpp/.h`, bloque propio «Lote 72» al final: `FightHitObjForceCpp` (punto y fuerza: `fld; fsub/fmul; fstp dword`, conserva desnormales) y `FightHitObjDamageCpp` (`fld; fmul 10.0f; fstp dword`).

## Banco

`Original` = bytes del EXE con el hook de la funcion apagado; `candidato` = DLL con SOLO ese hook encendido (`route_ok` en cada lado). Se llama a la entrada con ECX = tarea y seis argumentos en pila (`Gate`: siembra de pila/registros, compuertas de ESP, EBX/ESI/EDI/EBP, DF, SEH y x87).
Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN: `FightStrikeDamage` 0x61C740, `AddToMovingList` 0x542800, `ApplyForce` 0x542B50, `ObjectDamage` 0x5A0D90, `Fx_c::AddPunchImpact` 0x49F670 (todos con hook).
`CAEPedWeaponAudioEntity::AddAudioEvent` 0x4E69F0 y `CAEPedAudioEntity::AddAudioEvent` 0x4E2BB0 quedan SIN hook en todos los bancos (`runtime_runner` pasa `--unhook=Audio`): su cuerpo en la DLL salta al EXE y un unico falso en la direccion del EXE sirve a ambos lados (`Pair{exe,0,false}`).
`SetIsStatic` es una llamada virtual por la tabla del objeto: vtable falsa (ranura 4 = falso, las demas trampas).
Los falsos mutan memoria para verificar las relecturas: `FightStrikeDamage` reescribe `posn1`/`posn2` (la copia debe seguir con el valor viejo; `posn1` se lee por el puntero al usarlo), `SetIsStatic`/`AddToMovingList` cambian las banderas (se releen antes de la fuerza) y `ObjectDamage` cambia el tipo de arma activa (se relee antes del sonido de motosierra).
Se compara: eventos (argumentos y orden; los punteros a variables locales de la pila se etiquetan, los de arenas se expresan como arena/desplazamiento, los vectores se vuelcan por valor), cambios en las 7 arenas (tarea, ped, objeto, informacion del objeto, matriz, posn1, posn2),
hash de la tabla de sonidos y estado x87 (`fp_difference` vacio). EAX/ECX/EDX solo se registran.

16384 pares = 16 familias de 1024: puerta de efecto, de banderas fisicas y de multiplicador (incluye NaN, infinitos y el umbral +-3 ulp), bloque de SetIsStatic con/sin umbral cumplido, fuerza con matriz/posicion compacta y ambos factores, diferencias desnormales, danio especial,
motosierra con relectura, ranuras de arma negativas o fuera de rango, mutacion de `posn1`/`posn2`, mezclas. `coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

Prueba de sensibilidad (mutantes de produccion, ya revertidos): usar `posn2` en vez de la copia en `ObjectDamage` -> 16384/16384 diferencias; no releer las banderas -> 646/16384 diferencias (exactamente `flags_reread_enables_force`).

## Hallazgos

- Defecto del banco (no de produccion): la primera version trataba las dos `AddAudioEvent` como colaboradores con hook; con `--unhook=Audio` no lo tienen y la compuerta `collaborators_dll` fallaba en los dos lados (los eventos ya coincidian). Corregido con `Pair{exe,0,false}`.
- Produccion: sin discrepancias respecto al desensamblado tras la primera version; el banco no encontro defectos de produccion.
- Dominio: `surface` se limita a 0..255 (el unico llamador pasa un `uint8` y `AddAudioEvent` recibe un `eSurfaceType` de 8 bits; el EXE empujaria el dword completo). `component` no se lee.
- No cubierto: las colaboradoras reales (el banco no prueba su validez), dos NaN en una misma resta (la x87 elige operando segun la unidad), indices de la tabla de sonidos fuera de -8..15 / -8..40 (se leen globales vivos del EXE), `sNaN`.

## Comandos

```
bash reversing/_handoff/bench_here.sh FightHitObj72                               # compila la sonda y corre en el juego (candado global); CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/FightHitObj72/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/FightHitObj72/register_hook.py <carpeta-de-evidencia>         # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `fighthitobj72-runtime.start`; salida `fighthitobj72-runtime.jsonl`.

## Ampliacion 2026-10-26 (19864 pares, cero diferencias)

Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fighthitobj72\20261026-215958-706179` (APROBADO, todas las compuertas en ambos lados, sin tocar produccion). Las familias 0..15 (16384 pares) no cambian; se anaden cuatro familias y minimos nuevos en `coverage.json`.

Que se amplio:
- Familia 16 (1024): dos NaN en la misma resta `posn1 - pos` (y con `posn1` mutado por FightStrikeDamage): QNaN/QNaN, sNaN/sNaN, mixtos, signos iguales u opuestos, mismo significando, bits identicos, cada operando mayor. Se vieron ambas elecciones de la x87 (resultado igual a posn1 y a pos); EXE y DLL coinciden bit a bit (2806 ejes con dos NaN). No hay defecto de produccion en `FightHitObjForceCpp`.
- Familia 17 (1024): un NaN no canonico/sNaN en la normal (producto de la fuerza), el danio (x10), solo posn1, solo pos, umbral de arranque y multiplicador.
- Familia 18 (1176): barrido exhaustivo del indice de la tabla de sonidos, combo -8..15 x movimiento -8..40 (0xC16ABC..0xC177A4, dentro de la region vigilada).
- Familia 19 (256): superficie 0..255 completa con los 60 pares reales (combo 4..15, movimiento 0..4).

Que queda fuera y por que (desensamblado 0x61D400):
- (a) Solo la resta tiene dos operandos variables (`fld [posn1]; fsub [pos]`, 0x61D4D2/0x61D4E3/0x61D4ED). Los productos de la fuerza son `normal * constante` (`fld normal; fmul st(1)` con st(1) = -0.5f/-0.1f cargado en 0x61D4B1/0x61D4BD) y el danio es `fld; fmul [0x85862C]`: un operando es siempre una constante no NaN, asi que dos NaN no son alcanzables ahi (se cubre un NaN con cargas utiles y sNaN).
- (b) El EXE empuja `surface` como dword completo (`mov edx,[esp+0x4C]; push edx`, 0x61D5A5), pero su unico llamador (0x6245AF, unica referencia a 0x61D400 en el EXE) lo construye con `xor ecx,ecx; mov cl,[0xB9ACF3]; push ecx`: byte extendido con ceros, siempre 0..255. La DLL recibe `eSurfaceType` (uint8); un valor >255 solo mostraria esa truncacion y ningun llamador lo produce: inalcanzable. 0..255 queda cubierto entero.
- (c) Rango real: combo = `m_nComboSet` devuelto por 0x61C7F0 (4, el byte +0x6E de CWeaponInfo o el byte +0x72D del ped; tabla `m_aComboData` de 12 filas 0xC170D0..0xC17730, combos 4..15); movimiento = `eFightAttackType` 0..4 (0x623B10 guarda el byte en +0x25; `m_Hit` 5 entradas + `m_AltHit` 5). Todo eso esta ya cubierto exhaustivamente, y tambien el entorno -8..15 / -8..40. Mas alla (hasta +-128 del byte con signo) el EXE leeria globales vivos ajenos a la tabla y mutables: no se amplia; no es alcanzable en el juego.
- `component` no se lee; EAX/ECX/EDX solo se registran (sin cambio).

(d) Quien valida cada colaborador sustituido (docs/hooks.csv y reversing/validation-registry.json; todos son C++ con hook, ninguno es ya envoltura al EXE):

| Colaborador | Direccion | Banco / registro (pares) |
|---|---|---|
| `CTaskSimpleFight::FightStrikeDamage` (`FightStrikeDamageExe`) | 0x61C740 | `reversing/Helpers65D` (4096) |
| `CPhysical::AddToMovingList` | 0x542800 | `reversing/Physical56` (256) |
| `CPhysical::ApplyForce` | 0x542B50 | `reversing/Physical56` (256) |
| `CObject::ObjectDamage` | 0x5A0D90 | `reversing/Object48` (1024) |
| `Fx_c::AddPunchImpact` | 0x49F670 | `reversing/Helpers66A` (4096) |
| `CEntity::SetIsStatic` (virtual, ranura 0x10) | sin direccion (V) | `CObject::SetIsStatic`: `reversing/Object48` (512); `CEntity::SetIsStatic`: `reversing/CEntB` (128). Aqui es vtable falsa |
| `CAEPedWeaponAudioEntity::AddAudioEvent` | 0x4E69F0 | `reversing/Data23` (1024); en este banco sin hook (`--unhook=Audio`) |
| `CAEPedAudioEntity::AddAudioEvent` | 0x4E2BB0 | `reversing/AudioWave2` (1024); en este banco sin hook (`--unhook=Audio`) |

Este banco sigue sin validar la correccion de esos colaboradores (la valida cada banco citado) ni su integracion real con FightHitObj.
