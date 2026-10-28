<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco RTShadowSetup28: CRealTimeShadow::SetupForThisEntity (0x706520)

**Estado: APROBADO** (18/10/2026, corrida `20261018-180652-693943`: 5120 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-rtshadowsetup28\20261018-180652-693943`; archivado en `reversing/CppTwelveLevel6Final/runs/RTShadowSetup28-20261018-180652-693943/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CRealTimeShadow,SetupForThisEntity,0x00706520,1,0,1,S` en `docs/hooks.csv`). Nota (no evidencia): la primera corrida `20261018-175628-516713` tuvo 124/5120 diferencias por denormales de radio*1,1f (x87 a 24 bits en el EXE frente a SSE en la DLL) y se arregló con `X87Math::FloatTimesFloatCpp`.

Plantillas: `reversing/FileLoaderLoad2dEffect27` (estructura, falsos con parche de entrada en EXE y DLL) y `reversing/DecisionAdd27` (thiscall). Contrato general: `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` valen sólo para producción; el banco usa `__asm` en `Gate`/`SaveFP`).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh RTShadowSetup28                   # compila la sonda + corre + resume diferencias
# o a mano, siempre en serie:
python -B reversing/RTShadowSetup28/build_probe.py
python -B reversing/RTShadowSetup28/run_local.py
python -B reversing/RTShadowSetup28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe)
python -I reversing/RTShadowSetup28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -B reversing/RTShadowSetup28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (revisar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(SetupForThisEntity, 0x706520)` (ya está en `RealTimeShadow.cpp`; cambio sin commit; categoría/clase `CRealTimeShadow`, hook `Simple`). Archivo de arranque en el clon: `rtshadowsetup28-runtime.start`; salida `rtshadowsetup28-runtime.jsonl`. 5120 pares. `docs/hooks.csv` no tiene (a la hora de escribir esto) la fila `CRealTimeShadow,SetupForThisEntity,0x00706520,1,0,1,S`: la añade el coordinador al registrar.

`source.patch` (en esta carpeta) contiene SÓLO los cambios de esta función respecto a HEAD (`RealTimeShadow.cpp`); comprobado con `git apply --check` sobre `git show HEAD:source/game_sa/RealTimeShadow.cpp` en un directorio temporal. `RealTimeShadow.h` no cambia (la declaración ya existe).

`fixture_dump.cpp` es un ayudante autónomo (C++ estándar, sin tipos del juego): compilado aparte con `cl /EHsc /std:c++20` vuelca las 5120 filas `input` del generador REAL de `Fixtures.hpp`; sirvió para calibrar las cobertura mínimas de `run_local.py` y para comprobar que el oráculo acepta los 5120 fixtures (0 fixtures inválidos). No forma parte del banco.

## Qué hace la función (desensamblado 0x706520..0x7065FB, `ret 4`)

`bool CRealTimeShadow::SetupForThisEntity(CPhysical* owner)`, thiscall (ECX = this = esi), `ret 4`, guarda/restaura esi, edi (y ebx en las ramas internas).
1. `[this+0] = owner` (m_pOwner, ANTES de mirar nada). `edi = [owner+0x18]` (CEntity::m_pRwObject). Si es nulo: `AL = 0`, `ret 4` (sólo se escribió m_pOwner).
2. `type = movzx byte [edi]` ; `[this+0x24] = type` (m_nRwObjectType, DWORD). Si `type == 0` o `type > 2` (sin signo): `call 0x705990` (Destroy, ECX = this), `AL = 0`, `ret 4`.
3. Átomo (tipo 1): si `byte [edi+0x4C] & 2` (RpAtomic::interpolator.flags) → `_rpAtomicResyncInterpolatedSphere(edi)` (0x7491F0, cdecl). Copia 4 dwords `[edi+0x1C..0x2B]` (boundingSphere) → `[this+0x2C..0x3B]` (m_boundingSphere).
   Clump (tipo 2): `RpClumpGetBoundingSphere(edi, this+0x2C, 1)` (0x734FC0, cdecl) escribe m_boundingSphere.
4. `[this+0x48] = [this+0x38]` (radio de m_baseSphere = radio de la esfera de límites, dword). `RwV3dTransformPoints(this+0x3C, this+0x2C, 1, [edi+4]+0x10)` (0x7EDD90, cdecl): centro base = centro de límites transformado con la matriz del marco padre (`RwObject::parent` en +4, `RwFrame::modelling` en +0x10; padre nulo → matriz = 0x10, el EXE no lo comprueba).
5. `fld [this+0x38]; fmul [0x858F14] (1,1f); fstp [esp]` (redondeo a 24 bits) → `CShadowCamera::SetFrustum(this+8, radio*1,1f)` (0x7054C0, thiscall, `ret 4`); después `CShadowCamera::SetCenter(this+8, &m_baseSphere.center)` (0x705590, `ret 4`); `AL = 1`.

Bytes de la sombra que se escriben: [0..3], [0x24..0x27], [0x2C..0x3B] (esfera de límites), [0x3C..0x47] (centro base), [0x48..0x4B] (radio base). Orden de llamadas: Destroy | (Resync) , (ClumpBS) , Transform , SetFrustum , SetCenter.

## Fuente C++ (hunks, ver `source.patch`)

1. `RH_ScopedInstall(SetupForThisEntity, 0x706520)` añadido a `InjectHooks` (antes la función era una envoltura `plugin::CallMethodAndReturn<bool, 0x706520,...>` y no estaba enganchada).
2. Cuerpo C++ de `SetupForThisEntity` (sin ninguna llamada al EXE para la propia función). El comentario de dirección estaba mal (`// 0x706460`, que es `Create`): ahora `// 0x706520`.

## Fase 2: revisión contra el EXE (borrador sin commit → fuente final)

Se volvió a desensamblar 0x706520..0x7065FB y se contrastó línea a línea (ambas ramas, los dos `add esp` 0x10/0x1C que corresponden a los cdecl de 3/4 y 4 argumentos, el orden de empuje de los argumentos de `RwV3dTransformPoints` y las posiciones de los campos).
Discrepancias del borrador y correcciones:
- **No compilaba**: `CEntity::m_pRwObject` es privado → `owner->GetRwObject()` (accesor del repo, sin `assert`); `rwObjectGetType` no existe → macro `RwObjectGetType(object)` (lee el byte `type`).
- **El hook no estaba registrado** (la función ni siquiera estaba en `InjectHooks`) → añadido.
- La prueba de la bandera usaba `*(uint32*)(atomic+0x4C) & 2` con desplazamiento a mano → `atomic->interpolator.flags & 2` (`+0x4C` verificado con `static_assert(offsetof(RpAtomic, interpolator) == 0x4C)` en el banco; el EXE hace `test byte ptr [edi+4Ch],2`, mismo bit).
- Las copias de floats (centro y radio) pasan a `std::memcpy` de 4 bytes: el EXE copia con `mov` de dwords (conserva los bits de un sNaN); una copia float podría quietarlo en un archivo compilado con x87. Mismo orden y mismas lecturas tras la resincronización.
- Sin cambios de comportamiento respecto a lo que pretendía el borrador: guardar el dueño antes de mirar el objeto, `m_nRwObjectType` aunque el tipo sea inválido, `Destroy()` antes de devolver false, tipo > 2 sin signo, `1,1f` leído de 0x858F14 con `StaticRef<float>`.
Sin discrepancias de lógica pendientes.

**Llamadores / registros de salida.** Un único llamador en el EXE: `call 0x706A42` (en `CRealTimeShadowManager::GetRealTimeShadow`, 0x706970), que tras la llamada sólo usa ESI/EBP/EBX (`mov [ebp+0x134],esi`, `mov byte [esi+4],1`, `mov eax,esi`): **ningún llamador lee EAX/ECX/EDX** (`python -I reversing/Dependencies25A/residue_scan.py 0x706520` → `('0x706a42','call',[],[])`; búsqueda de `call/jmp 00706520` en todo el EXE con dumpbin: una sola coincidencia). El llamador en C++ (`RealTimeShadowManager.cpp:182`) ignora el `bool`. Por tanto EAX/ECX/EDX se registran en `original_residue`/`candidate_residue` pero no se comparan; el byte AL devuelto SÍ se compara.

**x87 y doble redondeo (riesgo conocido).** El EXE calcula `radio*1,1f` en el x87 (24 bits de mantisa, exponente extendido, luego `fstp` a float); la DLL usa SSE de precisión simple (un solo redondeo). Para resultados normales son idénticos; para resultados denormales (radio denormal) el x87 puede dar un segundo redondeo distinto. El generador sólo usa los radios denormales vetados (`kDenormalOk`) donde ambos coinciden y el oráculo lo comprueba con un modelo explícito de los dos redondeos (`RuntimeError` = fixture inválido). Si en juego fallara algún radio denormal por flush-to-zero (`MXCSR.FTZ`/`DAZ`), sería una diferencia real EXE/DLL sin relevancia en juego (los radios reales son normales); no se ha ocultado.

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **AL** devuelto; **sombra** (0x4C bytes completos; `m_pOwner` como etiqueta `0x0A110000` si es el puntero del dueño falso); **objeto RW falso** (0x80 bytes, el puntero al padre como etiqueta 0..4); **traza** de colaboradores (6 campos por evento); **guardas** (0x40 bytes antes y después de la sombra y del objeto), **dueño falso** y **marcos falsos** intactos (arena entera).
- Traza `[tipo, a, b, c, d, e]`: `[1, objeto, flags +0x4C]` Resync · `[2, objeto, esfera destino, 3.er argumento]` RpClumpGetBoundingSphere · `[3, destino, origen, cuenta, matriz]` RwV3dTransformPoints (matriz = desplazamiento en la arena de marcos, o el valor crudo 0x10 / 0x00C0FFF0) · `[4, this cámara, bits del radio·1,1f]` SetFrustum · `[5, this cámara, puntero, 3 dwords]` SetCenter · `[6, this]` Destroy.
- ABI: ESP (`ret 4`: `popBytes=0` en `Gate`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW completa, TOP, tags y ST válidos: no debe quedar ninguno; MXCSR sin banderas). FCW fijada a 24 bits, redondeo al más cercano y todas las excepciones enmascaradas (estado del juego).
- Rutas (`route_ok`): original → el hook de 0x706520 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI. Se vigilan 16 bytes de 0x706520 y de las seis entradas parcheadas, y los 4 bytes de la constante 1,1f (0x858F14, que `begin` registra y `run_local.py` exige que sea 0x3F8CCCCD).

## Colaboradores

**Sustituidos** (mismo falso en ambos lados, parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL, resuelto antes de parchear; activos sólo durante la llamada): `CRealTimeShadow::Destroy` 0x705990 (sólo traza; **no** destruye nada), `_rpAtomicResyncInterpolatedSphere` 0x7491F0 (traza y mezcla con XOR la esfera del átomo: así el orden resync→copia es observable), `RpClumpGetBoundingSphere` 0x734FC0 (traza y escribe la esfera destino desde los bytes del clump con otro XOR), `RwV3dTransformPoints` 0x7EDD90 (traza y escribe destino = origen XOR posición del marco XOR constante), `CShadowCamera::SetFrustum` 0x7054C0 y `SetCenter` 0x705590 (traza; el argumento float se lee como u32 en bruto). **No se valida aquí** el cuerpo real de ninguno de ellos (los de C++ tienen sus bancos propios); sólo que `SetupForThisEntity` los llama igual, con los mismos argumentos y en el mismo orden.
**Reales**: ninguno. No se toca ninguna global del juego; todo vive en cuatro arenas del banco (sombra, dueño, objeto, marcos).

## Fixtures (`Fixtures.hpp`): 5120 casos = 8 grupos × 640, 20 familias por grupo (32 repeticiones)

Grupos: g0 sin objeto · g1 tipo 0 · g2 tipos 3..255 (tabla y aleatorios, incluye 0x80..0xFF) · g3 átomo sin bit 1 · g4 átomo con bit 1 (resincroniza) · g5 clump · g6 átomo con +0x4C aleatorio · g7 clump.
Familias (esfera que llega a la sombra; el generador la guarda ya mezclada con el XOR de los falsos para que los valores especiales lleguen intactos): normal, ceros, radio negativo, ±0, ±inf, qNaN (con carga útil), cerca del desbordamiento (FLT_MAX, FLT_MAX/1,1 y vecinos), denormales vetados, normales mínimos, centro NaN/inf/sNaN (copia en bruto), centro denormal/−0, cuatro dwords aleatorios (radio sin sNaN), potencias de dos, mantisa llena, exponentes medios, centros enormes, radios enormes, 1,0, mezcla.
Además por caso: banderas de +0x4C con otros bits altos sin el bit 1 (prueba de que sólo cuenta ese bit), padre nulo (matriz = 0x10) / inválido (0x00C0FFE0+0x10) / uno de 3 marcos con posiciones aleatorias, sombra previa con patrones (aleatoria, ceros, 0xFF, constante, ascendente), semillas de EAX/EDX/pila.
Cobertura mínima (calibrada con el generador real vía `fixture_dump.cpp`; el oráculo aceptó los 5120 casos): ver `MINIMUMS` en `run_local.py` (grupos y familias exactos; el resto con ~40–50 % de margen).

## Suposiciones y riesgos del banco

- No se ha compilado: la sonda puede fallar al compilar por detalles de tipos del juego (`B(&RpClumpGetBoundingSphere)`, `static_cast` de `RwV3dTransformPoints`, `offsetof` sobre clases no estándar). Los `static_assert` de `LiveComparison.hpp` fijan la disposición usada (`sizeof(CRealTimeShadow)==0x4C`, `m_pOwner 0`, `m_camera 8`, `m_nRwObjectType 0x24`, esferas 0x2C/0x3C, `RpAtomic::boundingSphere 0x1C`, `interpolator 0x4C`).
- `CEntity::m_pRwObject` (privado) se lee en +0x18 en el banco (dueño falso); si el accesor de la DLL apuntara a otro desplazamiento el banco fallaría (no hay `static_assert` posible sobre un miembro privado).
- Los parches de entrada son de 5 bytes: los cuerpos de la DLL de los colaboradores deben tener ≥5 bytes (así es en Debug).
- Se asume que `0x858F14` vale 1,1f exacto (`0x3F8CCCCD`): `run_local.py` lo exige (`mul_constant_bits`).
- El valor de ECX/EDX/EAX tras la llamada difiere entre EXE y DLL (sólo se registra): ningún llamador lo lee (ver arriba).
