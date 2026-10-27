<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ReplUpgrade72: CVehicle::AddReplacementUpgrade (0x6D3830, lote 72)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-3`, HEAD 95f7d73eb + `source.patch`). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test
esultados-replupgrade7261026-204732-062452`. (Una corrida previa, `20261026-204625-143815`, se detuvo por un defecto del banco: fixtures con marco original del atomo nulo, que la DLL aserta en `RwFrameDestroy`; en el juego nunca ocurre.)

`source.patch`: solo `Vehicle.cpp` (`git apply --check` contra HEAD 95f7d73eb).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| AddReplacementUpgrade | 0x006D3830 | CVehicle,AddReplacementUpgrade | S |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,AddReplacementUpgrade,0x006D3830,1,0,1,S`. No es virtual.

## Que hace (desensamblado 0x6D3830..0x6D39D4, 421 bytes; la siguiente funcion es 0x6D39E0 RemoveReplacementUpgrade)

thiscall con `ret 8`, sin valor de retorno (ningun llamador lee EAX/ECX/EDX: los cinco `call` estan en AddVehicleUpgrade 0x6E3290 y solo conservan ESI/EDI/EBX/EBP).

1. `frame = GetFrameFromId([this+0x18], nodeId)` (0x4C53C0). `flags = 0`; `RwFrameForAllObjects(frame, RemoveObjectsCB 0x6D33B0, &flags)` y `RwFrameForAllChildren(frame, 0x6D3420, &flags)` (vacian el componente y dejan en `flags` el id del ultimo atomo quitado).
2. `mi = ms_modelInfoPtrs[modelIndex]`; `atomo = mi->CreateInstance()` (vtable +0x2C); `mi->AddRef()` (0x4C4BA0); el marco original del atomo (`atomo+4`) se lee antes de `RpClumpAddAtomic`, `RpAtomicSetFrame(atomo, frame)` y `RwFrameDestroy(marco original)`.
3. `SetAtomicId(atomo, flags & ~3)`, `SetAtomicFlag(atomo, 1)` y el bloque 0x6D38D5..0x6D390B (`SetupUpgradeAtomicRendering(atomo, false)`: HasAlphaMaterialCB sobre la geometria, callback de render HiDetail o HiDetailAlpha, `SetRenderPipelinesCB(atomo, 0)`); `ms_bCreateDamagedVersion` (0xA9B0B0) = 0.
4. `nodeId == 0x13` y bit 0x2000 de `[manejabilidad+0xCC]` (`m_bDoubleExhaust`): `r = CreateReplacementAtomic(mi, frame, flags, false, true)`; `x` del marco de `r` = `x` del marco del componente * `-2.0f` (0x858B18, leida tras la llamada) y `RwMatrixUpdate`. Con `nodeId` 0xC o 0xD: `SetupClumpAfterVehicleUpgrade(clump, info->m_pPlateMaterial, info->m_nPlateType)` con el model info del vehiculo (`(int16)[this+0x22]`).
5. Si `mi->AsDamageAtomicModelInfoPtr()` (vtable +8; el EXE prueba EAX completo) no es nulo: `CreateReplacementAtomic(mi, frame, flags, true, false)` y, con `frame` no nulo, `RwFrameForAllObjects/Children(frame, SetVehicleAtomicVisibilityCB, 1)`.

## Cambios de produccion (`source.patch`)

`Vehicle.cpp`: el cuerpo (antes una llamada a 0x6D3830) pasa a C++ puro (sin `__asm`/`naked`), reutilizando `SetupUpgradeAtomicRendering` (el bloque 0x6D38D5..0x6D390B ya identificado) y `CreateReplacementAtomic`; se activa `RH_ScopedInstall(AddReplacementUpgrade, 0x6D3830)`. Sin cambios en `.h`.

## Banco

`Original` = bytes del EXE con el hook apagado (entrada 0x6D3830 intacta); `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI, comprobado en cada lado: `route_ok`). Se llama a la entrada del EXE con ECX = vehiculo falso y la pila sembrada; se exigen las compuertas de ABI (`ret 8`, ESI/EDI/EBX/EBP conservados, DF limpia, SEH), x87 (FCW/TOP/ST validos) y restauracion de globales. EAX/ECX/EDX solo se registran.
Colaboradores (15) sustituidos por falsos identicos en la direccion del EXE y, si tienen hook, en el cuerpo de la DLL (los que tienen hook quedan apagados): GetFrameFromId 0x4C53C0, RwFrameForAllObjects 0x7F1200, RwFrameForAllChildren 0x7F0DC0, CBaseModelInfo::AddRef 0x4C4BA0, RpClumpAddAtomic 0x74A490, RpAtomicSetFrame 0x74BF20, RwFrameDestroy 0x7F05A0, SetAtomicId 0x732290, SetAtomicFlag 0x7322B0, RpGeometryForAllMaterials 0x74C790, SetAtomicRenderCallback 0x7328A0, SetRenderPipelinesCB 0x4C88F0, CreateReplacementAtomic 0x6D3700, RwMatrixUpdate 0x7F18A0 y SetupClumpAfterVehicleUpgrade 0x6FDFE0. `CreateInstance` y `AsDamageAtomicModelInfoPtr` son entradas de una vtable falsa (las demas ranuras registran un evento 99). Los falsos registran sus argumentos EN ORDEN (los callbacks se normalizan a un identificador igual en EXE y DLL; los punteros a la arena, a un desplazamiento); RpAtomicSetFrame escribe el marco en `atomo+4` y los de RwFrameForAll* escriben el `flags` de `data`, RpGeometryForAllMaterials un byte.
Vehiculo, model infos (ms_modelInfoPtrs[15..19]), manejabilidad, marcos, atomos y clump son FALSOS en una arena con guardas; se compara por oraculo independiente (`fixture_oracle.py`, escrito desde el desensamblado) la lista de eventos, los cambios de la arena y el byte final de `ms_bCreateDamagedVersion`, y ademas EXE contra DLL fila a fila.
16384 pares = 16 familias de 1024 (nodo 0x13 con/sin bandera 0x2000, 0xC, 0xD, otros nodos; marco nulo/arena/ajeno; id de atomo escrito por los dos recorridos o no; con/sin alfa; con/sin atomo danado (incluidos retornos solo con bits altos); x del componente cero/entero/normal/enorme (desborde)/infinito/NaN, `ms_bCreateDamagedVersion` inicial 0/1/otro, clump nulo).
`coverage.json` = minimos de ramas (50 % de lo medido con `fixture_mirror.py`). `fixture_oracle.py` autoprueba: `python -I fixture_oracle.py`.

## Limites

Limites originales (la ampliacion del 26/10/2026, al final, cubre `modelIndex` de la tabla, los desnormales/NaN/ceros con signo de x y documenta el resto): marco original del atomo nulo (la DLL aserta en `RwFrameDestroy`; en el juego CreateInstance siempre devuelve un atomo con marco) y el cuerpo real de los colaboradores sustituidos.
Dependencias: AddVehicleUpgrade (0x6E3290) llama a esta direccion cinco veces y sus bancos (Vehicle66, lo que llame a AddUpgrade/ProcessMods) la usan como falso o como llamada real: la integracion debe repetirlos.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh ReplUpgrade72   # compila la sonda y corre en el juego (candado global)
python -B reversing/ReplUpgrade72/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/ReplUpgrade72/fixture_oracle.py ; python -I reversing/ReplUpgrade72/fixture_mirror.py
python -B reversing/ReplUpgrade72/register_hook.py <carpeta-de-evidencia>                      # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `replupgrade72-runtime.start`; salida `replupgrade72-runtime.jsonl`.


## Ampliacion 2026-10-26 (worktree `D:\Original\wy-3`)

Se amplian fixtures, oraculo y compuertas sin relajar nada (los minimos de `coverage.json` anteriores se conservan y se anaden los nuevos).

### (a) `modelIndex` y la tabla `ms_modelInfoPtrs`
Desensamblado: `6D3871 mov ecx,[esp+40h]` (modelIndex) y `6D3875 mov ebp,[ecx*4+0A9B0C8h]`: indice de 32 bits con signo, 4 bytes por entrada, SIN comprobacion de rango ni de nulo (`6D3880 mov eax,[ebp]` desreferencia el puntero tal cual); lo mismo con el model info del vehiculo en `6D396D movsx edx,word ptr [ebx+22h]` / `6D3971 mov eax,[edx*4+0A9B0C8h]`. La tabla (`CModelInfo::NUM_MODEL_INFOS` = 20000) ocupa 0xA9B0C8..0xACE948. Los llamadores son las cinco llamadas de AddVehicleUpgrade 0x6E3290 (0x6E3300, 0x6E3327, 0x6E3331, 0x6E333B, 0x6E33B4; un escaneo de todo el EXE no encuentra otras): el `modelIndex` es el id de la mejora que recibe AddVehicleUpgrade (1000..1193; AddVehicleUpgrade ya desreferencia `ms_modelInfoPtrs[id]` en 0x6E3296, asi que nunca es nulo) o el id de reemplazo (0x6E3396, devuelto por 0x4C74D0) y `nodeId` es `(info+0x12 >> 10) & 0x1F` (0..31) o las constantes 5, 4 y 7.

Ampliacion: el pool de ranuras sobrescribibles pasa de 15..19 a 415 ranuras DENTRO de la tabla: 0/1, 15..19, 400..611 (ids de vehiculos, para `[this+0x22]`), 1000..1193 (ids reales de mejoras) y 19998/19999 (ultimas). Cada caso elige `modelIndex` (30 % legados 16..19, 40 % 1000..1193 al azar, 7 % 1000/1193, 8 % 0/1, 15 % 19998/19999) y el indice del vehiculo (15, 400..611, 400/611, 1000..1193, 0/1/19998/19999; siempre distinto de `modelIndex`), instala los model infos falsos en esas dos ranuras y repone TODAS las del pool (`Globals::Capture/Apply`; `globals_restored` las compara tras quitar los parches). El falso de la mejora es `U(modelIndex & 3)` de la arena (el oraculo lo modela) y el `int16` de `[this+0x22]` es el indice del vehiculo. Cobertura nueva: `idx_legacy`, `idx_game_upgrade`, `idx_game_edge`, `idx_table_first`, `idx_table_last`, `idx_plate_game`, `veh_idx_*`.

Queda fuera (el EXE no lo valida y no se puede probar sin fallar o leer fuera): indices negativos o >= 20000 (leen fuera de la tabla), entradas nulas de la tabla (el EXE desreferencia el puntero sin comprobarlo: acceso invalido en ambos lados) e indices dentro de la tabla que no estan en el pool (con el juego cargado son model infos reales cuyo CreateInstance no se puede falsear sin instalarlos). Ningun llamador del EXE puede producirlos (ver arriba).

### (b) x del componente: desnormales, NaN, ceros con signo e infinitos con MXCSR FTZ
Desensamblado: `6D3946 fld dword [edi+40h]` (x del marco del componente, tras CreateReplacementAtomic), `6D394F fmul dword [858B18h]` (-2.0f = 0xC0000000), `6D3956 fstp dword [eax+30h]` (x del marco nuevo = marco+0x10+0x30), x87 pura a 24 bits. El banco corre con el MXCSR del juego (FTZ; `begin.mxcsr_ftz` y `initial_mxcsr` se exigen en `run_local.py`). Se anaden fixtures `SpecialX` (85 % de los casos de rueda con x finita no nula): desnormales (mitad baja, mitad alta, 1, 2, 0x3FFFFF, 0x400000, 0x400001, 0x7FFFFF, con ambos signos), SNaN (bit 22 limpio; cargas 1, 2, 0x200000, 0x3FFFFF y aleatorias), QNaN con cargas 0, 1, 0x200000, 0x3FFFFF y aleatorias, +-0, +-inf, normales minimos, maximos (el doble desborda a +-inf) y normales medios. El oraculo (`f32_mul_m2`, exacto en doble) da: desnormal -> desnormal exacto (el doble), cero con signo -> cero de signo contrario, SNaN -> QNaN con la misma carga y signo, desborde -> inf. Estadisticas nuevas: `wheel_x_denormal(_low/_high/_result)`, `wheel_x_snan`, `wheel_x_qnan_payload`, `wheel_x_zero_neg/pos`, `wheel_x_inf_neg/pos`, `wheel_x_overflow`, `wheel_x_min_normal`.

**Defecto de produccion.** La DLL hacia `matrix->pos.x = frame->pos.x * -2.0f` con SSE (`mulss`): con MXCSR FTZ el resultado subnormal se vacia a cero cuando la entrada es un desnormal de mantisa < 0x400000 (el doble sigue siendo subnormal), mientras que el `fmul` x87 del EXE lo conserva. Demostracion independiente (`mulss` con MXCSR 0x9F80 frente a `fld/fmul/fstp`, programa de 32 bits aparte): x=0x00000001 -> SSE 0x80000000, x87 0x80000002; x=0x003FFFFF -> SSE 0x80000000, x87 0x807FFFFE; x=0x80000001 -> SSE 0x00000000, x87 0x00000002; 0x00400000 y 0x007FFFFF y el SNaN coinciden. Correccion: ayudante `AddReplUpgradeExhaustXCpp(const float* x, float* out)` en el bloque «Lote 72b» al final de `X87Math.cpp/.h` (archivo compilado con `/arch:IA32`: `fld/fmul/fstp`); `Vehicle.cpp` lo llama en lugar de la multiplicacion SSE.

### (c) Dos NaN en una operacion
El unico calculo de punto flotante de la funcion es `6D3946..6D3956` (arriba): `fmul` de un operando variable por la constante -2.0f en memoria. Revisados los 421 bytes (0x6D3830..0x6D39D4): no hay otra instruccion x87 ni SSE; el resto son accesos y comparaciones enteras (`cmp eax,13h/0Ch/0Dh`, `test ch,20h`, `test eax,eax`, `test al,al`, `test edi,edi`) y llamadas. El caso «dos operandos variables NaN» no existe en esta funcion; con un solo NaN las fixtures ya cubren QNaN y SNaN con cargas distintas.

### (d) Quien valida a los colaboradores sustituidos
Segun `docs/hooks.csv` y `reversing/validation-registry.json`:

| Colaborador | Direccion | Banco / registro (pares) |
|---|---|---|
| CClumpModelInfo::GetFrameFromId | 0x4C53C0 | Clump24 (1024) |
| CBaseModelInfo::AddRef | 0x4C4BA0 | Model34 (256) |
| CVisibilityPlugins::SetAtomicId / SetAtomicFlag | 0x732290 / 0x7322B0 | VisPluginsFinal4 (96 / 96) |
| CVisibilityPlugins::SetAtomicRenderCallback | 0x7328A0 | VisPluginsFinal4 (128) |
| CVehicleModelInfo::SetRenderPipelinesCB | 0x4C88F0 | VehModelInfoFinal4 (64) |
| CVehicle::CreateReplacementAtomic | 0x6D3700 | CVehC (512) |
| CCustomCarPlateMgr::SetupClumpAfterVehicleUpgrade | 0x6FDFE0 | Mix14 (256) |
| RemoveObjectsCB Object/Frame, SetVehicleAtomicVisibilityCB Object/Frame (callbacks reales, no sustituidos) | 0x6D33B0/0x6D3420, 0x6D2690/0x6D26D0 | CVehC (512/256, 384/256) |
| HasAlphaMaterialCB; RenderVehicleHiDetailCB / HiDetailAlphaCB (reales) | 0x4C77C0; 0x733240 / 0x733F80 | VehModelInfoFinal4 (128); VisPluginsFinal4 (512 / 512) |
| SetupUpgradeAtomicRendering (bloque 0x6D38D5..0x6D390B, funcion estatica de Vehicle.cpp) | sin direccion propia | ningun banco propio: solo lo cubren este banco y CVehC (CreateReplacementAtomic lo llama) |
| RwFrameForAllObjects, RwFrameForAllChildren, RpClumpAddAtomic, RpAtomicSetFrame, RwFrameDestroy, RpGeometryForAllMaterials, RwMatrixUpdate | 0x7F1200, 0x7F0DC0, 0x74A490, 0x74BF20, 0x7F05A0, 0x74C790, 0x7F18A0 | NINGUNO: funciones de la biblioteca RenderWare enlazada en el EXE, sin hook en `hooks.csv` ni entrada en `validation-registry.json` (otros bancos, p. ej. DependenciesWorld12, usan 0x7F0DC0 y 0x7F05A0 reales sin validarlos); se tratan como dependencias de biblioteca |

Este banco no valida la correccion interna de ninguno: valida que `AddReplacementUpgrade` los llame en el mismo orden y con los mismos argumentos que el EXE.

### Resultados de la ampliacion (26/10/2026)
- **APROBADO**, 16384 pares, 0 diferencias EXE/DLL, todas las compuertas en ambos lados y salida de cada lado igual a la del oraculo, con MXCSR FTZ (`mxcsr_ftz` verdadero). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-replupgrade72\20261026-221534-909917` (misma fuente; una corrida identica previa, `20261026-221029-402526`, tambien aprobada).
- Cobertura medida en la primera corrida aprobada (minimos nuevos en `coverage.json`, 50 % de lo medido por el espejo): `idx_game_upgrade` 7709, `idx_legacy` 4911, `idx_game_edge` 1207, `idx_table_first` 1298, `idx_table_last` 2466, `veh_idx_game` 9095, `idx_plate_game` 1995, `wheel_x_denormal` 411 (bajos 205, altos 206), `wheel_x_snan` 202, `wheel_x_qnan_payload` 586, `wheel_x_zero_neg` 158, `wheel_x_zero_pos` 190, `wheel_x_inf_neg/pos` 232/254, `wheel_x_overflow` 102, `wheel_x_min_normal` 94.
- Ayudante x87: el `.obj` compilado de `AddReplUpgradeExhaustXCpp` (configuracion Debug del proyecto) contiene `fmul dword ptr [eax]` + `fstp dword ptr [eax]` (la constante -2.0f llega por `std::bit_cast`, que deja el valor en ST0); sin ninguna instruccion SSE.
- **Prueba negativa** (cambio temporal en `Vehicle.cpp`, ya restaurado): con `matrix->pos.x = frame->pos.x * -2.0f` (SSE) el banco FALLA con 205 de 16384 pares (corrida `20261026-221310-773710`), todos con x del componente desnormal de mantisa < 0x400000 (la salida difiere en la memoria de la arena: SSE+FTZ da cero, el EXE el desnormal); exactamente los `wheel_x_denormal_low` (205). Con el ayudante x87: 0 diferencias.

## 2026-10-26b: el banco ya no toca ranuras reales de ms_modelInfoPtrs
Se elimina el riesgo operativo de la ampliacion anterior (sobrescribir temporalmente ranuras 400..611 y 1000..1193 del proceso del juego). La funcion solo usa `modelIndex` como indice de tabla (6D3875) y el indice del vehiculo (6D3971), por lo que la logica cubierta es la misma con cualquier ranura.
- En `begin` se recorre la tabla (20000 entradas en 0xA9B0C8) y `SelectNullSlots` elige 64 ranuras NULAS: ordinales 0..7 las nulas mas bajas, 8..15 las mas altas, 16..47 repartidas por la tabla, 48..63 un bloque de 16 consecutivas (zona 3). `MakeSpec` genera ordinales y `Realize` los sustituye por el indice real y rellena `v[19]` (falso U0..U3 = ordinal & 3), `v[20]`, `v[21]` (zonas). Modelo y vehiculo siempre en ranuras distintas. Si hubiera menos de 256 nulas, el banco no corre (`pool_all_null` falso); alternativa: menos ranuras o zonas mas pequenas.
- `globals_restored` captura y compara la tabla COMPLETA (20000 entradas) por caso y lado; solo se escribe en las 64 ranuras elegidas, que se reponen a 0. Compuerta nueva `pool_slots_null` (por caso y lado, antes de instalar los falsos). `begin` registra `pool_slots`, `pool_all_null`, `null_slots_total`, `nonnull_slots_total`, `table_first_null`, `table_last_null`, `table_hash_begin`; `global_state_final` registra `table_hash_final` (FNV-64 de la tabla entera) y `run_local.py` exige que sea igual al inicial, que el pool sean 64 indices distintos nulos y que cada fila use indices del pool con zona y selector coherentes.
- Cobertura: se sustituyen las categorias basadas en ids reales (`idx_game_*`, `idx_legacy`, `idx_table_*`, `veh_idx_*`, `idx_plate_game`) por `idx_zone_*`/`veh_zone_*` (low, high, spread, run), `idx_model_above_veh`/`below`, `idx_adjacent`, `idx_near`, `idx_far`, `idx_in_low_tenth`/`high_tenth`, `veh_in_*_tenth`, `idx_plate_far`. Los minimos de zonas y relaciones son el 50 % de lo medido por el espejo (mapa de nulas sintetico) y los dependientes del mapa real el 25 %; con el mapa real todos se cumplieron sin ajustar `coverage.json`. Los demas minimos (incluidos `wheel_x_*`) no cambian.
- Mapa real observado: 5160 ranuras nulas, 14840 no nulas; la entrada 0 no es nula y la 19999 si. Ranuras usadas: 2, 3, 4, 5, 6, 8, 42, 65 (bajas); 19999..19992 (altas); 3146, 5100, 6601, 6762, 11690, 11851, 12013, 12174, 12335, 12496, 12658, 13583, 14041, 14202, 14364, 15019, 15220, 15381, 15543, 15704, 15865, 16817, 16979, 18198, 18790, 18951, 19113, 19274, 19435, 19596, 19758, 19919 (repartidas); 19307..19322 (bloque consecutivo).
- Resultados: APROBADO, 16384 pares, 0 diferencias; evidencia final `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-replupgrade72\20261026-231116-096308` (corrida identica previa `20261026-230642-307841`). Hash FNV-64 de la tabla al inicio = al final (3438451981918888851) y `pool_slots_null` verdadero en todos los casos y lados. Prueba negativa con `x * -2.0f` en SSE (restaurado): FALLO con 198 de 16384 pares (corrida `20261026-230908-341058`; son los `wheel_x_denormal_low` con este generador, 198 segun el espejo).
