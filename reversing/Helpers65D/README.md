<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers65D: matriz * punto 0x59C890, CreateRouteTask 0x655140, RwTexDictionaryGtaStreamRead 0x730FC0 y FightStrikeDamageExe 0x61C740

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wr-4`, HEAD 633dceef0). 16384 pares (4 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados,
`fp_difference` vacío (el ST0 de `FightStrikeDamageExe` forma parte del estado x87 comparado) y cobertura por familia (16 x 256 por función) y de ramas.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers65d\20261021-031332-212627`.

Rebanco de funciones que ya usaban 0x59C890 (con el hook nuevo encendido): `AutomobileWrappers60` APROBADO (`resultados-automobilewrappers60\20261021-031527-358765`) y `CarCtrlRest63B` APROBADO (`resultados-carctrlrest63b\20261021-031754-291766`).

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `operator*(const CMatrix&, const CVector&)` (`TransformPoint`) | 0x59C890 | `CMatrix` / `operator*-Vec` / S (`RH_ScopedGlobalOverloadedInstall`, en `CMatrix::InjectHooksMultiplyVector`) | ninguno (aritmética pura) |
| `CTaskComplexWalkRoundObject::CreateRouteTask` | 0x655140 | `CTaskComplexWalkRoundObject` / `CreateRouteTask` / S (`RH_ScopedInstall`) | `CTask::operator new` 0x61A5A0 (con hook), `CPointRoute::operator new` 0x41B7C0 (sin hook); el constructor de `CTaskComplexFollowPointRoute` (0x671510) corre de verdad en ambos lados |
| `RwTexDictionaryGtaStreamRead` | 0x730FC0 | `RwHelper` / `RwTexDictionaryGtaStreamRead` / S (`RH_ScopedGlobalInstall`, en `RwHelperInjectHooks`) | `RwStreamFindChunk` 0x7ED2D0, `RwStreamRead` 0x7EC9D0, `RwTexDictionaryCreate` 0x7F3600, `RwTextureGtaStreamRead` 0x730E60, `RwTexDictionaryAddTexture` 0x7F3980, `RwTexDictionaryForAllTextures` 0x7F3730, `RwTexDictionaryDestroy` 0x7F36A0 (todas envolturas sin hook) |
| `FightStrikeDamageExe` | 0x61C740 | `CTaskSimpleFight` / `FightStrikeDamage` / S (`RH_ScopedNamedGlobalInstall`) | `CPed::IsPlayer` 0x5DF8F0 y `CStats::GetFatAndMuscleModifier` 0x559AF0 (ambos con hook) |

Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CMatrix,operator*-Vec,0x0059C890,1,0,1,S`, `CTaskComplexWalkRoundObject,CreateRouteTask,0x00655140,1,0,1,S`,
`RwHelper,RwTexDictionaryGtaStreamRead,0x00730FC0,1,0,1,S`, `CTaskSimpleFight,FightStrikeDamage,0x0061C740,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh Helpers65D        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/Helpers65D/build_probe.py
python -B reversing/Helpers65D/run_local.py
python -B reversing/Helpers65D/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/Helpers65D/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers65d/20261021-031332-212627"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `helpers65d-runtime.start`; salida `helpers65d-runtime.jsonl`. No hay `fixture_oracle.py`: la referencia es el propio EXE (hook apagado), no un modelo; las fixtures son C++ estándar en `Fixtures.hpp`.

## Fuente (`source.patch`: sólo los cambios de estas cuatro funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

- `Core/Matrix.cpp/.h`: `operator*(CMatrix, CVector)` en C++ (llama a `Hp65MatrixTimesPointCpp`) y `CMatrix::InjectHooksMultiplyVector()` con el hook. **`CMatrix::InjectHooks()` completo sigue desactivado** en `InjectHooksMain.cpp` («rendimiento»):
  se añade una sola línea `CMatrix::InjectHooksMultiplyVector();` tras `CQuaternion::InjectHooks()` para instalar sólo 0x59C890.
- `X87Math.cpp/.h` (bloque «Lote 65» al final, prefijo `Hp65`): `Hp65MatrixTimesPointCpp`. Sin NaN en los 15 elementos leídos delega en `AutoPreMultiplyMatrixWithVectorCpp` (ya existente: sumas x87 a 24 bits en el orden del EXE); con algún NaN usa un modelo
  de la secuencia exacta del EXE (ver «Defectos»).
- `Entity/Vehicle/Plane.cpp`: `PlaneTransformPoint` deja de llamar al EXE por dirección y usa el operador.
- `Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp`: `CreateRouteTask` (0x655140) en C++ + `RH_ScopedInstall`. Parte del borrador del lote 27, corregido (ver «Defectos»).
- `RenderWare/rw/rwtexdict.cpp` + `RwHelper.cpp`: `RwTexDictionaryGtaStreamRead` en C++ + `RH_ScopedGlobalInstall`.
- `Tasks/TaskTypes/TaskSimpleFight.cpp`: `FightStrikeDamageExe` pasa a `float __fastcall (task, edx, ped)` en C++ (el EXE la llama con `ret 4` y el resultado en ST0) + `RH_ScopedNamedGlobalInstall(..., "FightStrikeDamage", 0x61C740)`; `FightHitCar` la llama con `(this, 0, ped)`.
  Los productos usan `Cc63bCruiseSpeedCpp` (fild × float a 24 bits, ST0 sin redondear a float) de `X87Math`.

## Revisión contra el EXE (re-desensamblado y contrastado)

**0x59C890 (matriz × punto).** cdecl `(CVector* out, const CMatrix*, const CVector*)`, `EAX = out`. x = ((m[8]·z + m[4]·y) + m[0]·x) + m[12]; y = ((m[9]·z + m[1]·x) + m[5]·y) + m[13]; z = ((m[10]·z + m[2]·x) + m[6]·y) + m[14] (m = floats de la matriz; las lecturas de `m[0..2]`, `m[4..6]`, `m[8..10]` y `m[12..14]`);
cada componente se redondea a float al final; las tres se calculan antes de escribir ninguna (el destino puede solaparse con la matriz o el punto). **Llamadores:** `residue_scan.py` sobre los 272 llamadores: 225 leen EAX (valor de retorno, que la función devuelve igual);
ninguno lee ECX ni EDX (las apariciones son `push ecx` de relleno, `lea ecx,[ecx]` y llamadas a funciones cdecl/que reescriben ECX/EDX); se revisaron a mano 0x448CDB y 0x49DC49.

**0x655140 (CreateRouteTask).** `[this+0x20]` (ruta) → `[ruta]` (contador) a cero → devuelve 0 (sin reservar). Si no, `CTask::operator new(0x3C)`; nulo → 0; si no, constructor 0x671510 con `(this+0xC movimiento, ruta, 0 = ONE_WAY, 0.5f [0x86FC98], 0, 1, 0, 0)` y EAX = tarea. El argumento de pila (ped) no se usa (`ret 4`). Llamadores (0x6552DB, 0x657295, 0x657309, 0x657530): sólo EAX.

**0x730FC0 (RwTexDictionaryGtaStreamRead).** `RwStreamFindChunk(stream, 1, &len, &ver)` (test de todo EAX) → `RwStreamRead(stream, marco, len)` sobre un marco local de 12 bytes (contador|dispositivo, longitud, versión) y compara con la longitud RELEÍDA del marco (una lectura de 5 a 12 bytes la pisa) → `RwTexDictionaryCreate` (nulo → 0) →
bucle `dec word` (contador de 16 bits; 0 = diccionario vacío) de `RwTextureGtaStreamRead`/`AddTexture`; si una textura falla: `ForAllTextures(txd, 0x730E50, 0)` (callback del EXE: `RwTextureDestroy(tex); return 0`), `Destroy` y 0. Llamadores (0x5B388D, 0x731E09, FileLoader, TxdStore): sólo EAX.

**0x61C740 (FightStrikeDamageExe).** base = byte `[0xC170D0 + 0x55 + (combo-4)·0x88 + movimiento]` (ambos int8 con signo, sin acotar) → float; `CPed::IsPlayer` (AL): jugador con el bit 1 del byte +0x34 de `m_pPlayerData` (+0x480) → 50.0f (0x858B40); si no `GetFatAndMuscleModifier(4) × base`.
No jugador: arma activa (`movsx byte [ped+0x718]·0x1C + 0x5A0`): 1 → base × 1.5f (0x858CE8), 0 → base × `m_pStats(+0x59C)->m_fAttackStrength(+0x28)`, otro → base. Llamadores (0x61CCF1 → `_ftol2` con ST0, 0x61D0EB `fstp dword`, 0x61D428): no leen EAX/ECX/EDX.

## Defectos hallados y corregidos

- **Hook de `CMatrix` inexistente:** `CMatrix::InjectHooks()` está desactivado en `InjectHooksMain.cpp`; la primera corrida dio «Hook RH sin registrar» para 0x59C890. Se añadió la instalación selectiva descrita arriba.
- **Selección de NaN de la x87 (producción):** con `AutoPreMultiplyMatrixWithVectorCpp` el compilador elige qué operando va a registro: 22 de 4096 pares (familia de varios NaN) diferían del EXE. El EXE hace `fld` (silencia un NaN señalizante) + `fmul dword [punto]`
  y `fadd dword [pos]` (el señalizante en memoria pierde frente al silencioso del registro) y `faddp` entre registros (mayor mantisa). `Hp65MatrixTimesPointCpp` modela esa secuencia cuando hay algún NaN; los bits se leen de memoria (pasar el float por valor lo
  cargaría en la x87 y silenciaría el NaN señalizante; este fue el fallo de la primera versión del modelo, 38 diferencias). Quedó con cero diferencias. Los demás usuarios de `AutoPreMultiplyMatrixWithVectorCpp` (CarCtrl) conservan ese comportamiento.
- **Borrador de CreateRouteTask (lote 27):** usaba `new CTaskComplexFollowPointRoute(...)`, que no comprueba el nulo de `CTask::operator new`; el EXE sí. Ahora `CTask::operator new` + `::new(memoria)` con comprobación.
- Banco: en `LiveComparison.hpp` hubo que usar una struct derivada para `offsetof` de miembros protected de `CPed` (lección 1) y un error `LNK1106` por disco `D:` lleno (ajeno) obligó a repetir el enlace.

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con SOLO ese hook encendido (los demás hooks conservan su estado; los colaboradores con hook deben ser saltos a la DLL, los sin hook código del EXE). Falsos idénticos en ambos lados
(parche de entrada en el EXE y en el cuerpo de la DLL, resuelto ANTES de parchear), que registran sus argumentos en orden. Por función se compara EAX, todos los bytes que cambian en las arenas (con guardas), eventos, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado
con ST0), ruta, `collaborators_dll`, estado restaurado tras quitar los parches. ECX/EDX de salida sólo se registran; EAX se compara donde es el valor de retorno.

- **0**: 16 familias (valores del mundo, enteros, exponentes medios/amplios, cancelación, diminutos, enormes, inf, un NaN, varios NaN con señalizantes, ceros con signo, un elemento especial, bits crudos, solapes). Destino distinto del punto/matriz y solapado (`alias` 1-5: = punto, = +0x30, = +0, = +0x10, = +0x28).
- **1**: `m_pointRoute` con 0..8 entradas (incl. NaN), movimiento arbitrario, reserva de tarea nula, reserva de ruta nula; se compara el objeto construido (0x3C bytes), la ruta copiada y el tamaño pedido a cada `operator new`.
- **2**: FindChunk falso/verdadero con valores no canónicos, longitudes 2..100 (incl. > 12: el EXE destroza su pila, aquí sólo el falso escribe 12 bytes), retorno de la lectura ±1/0/el dword pisado, contador 0..24 y 0xFFFF/0x8xxx con fallo de textura, Create nulo, fallo de textura en cualquier posición.
  **No se prueban longitudes 0 ni 1**: el EXE usa bytes viejos de su pila como contador, estado indefinido.
- **3**: combo -3..27 y movimiento -128..127 sobre la tabla 0xC170D0 sobrescrita con bytes del caso (región 0xC16AD0..0xC182D0 guardada y restaurada), ranura -128..127, tipos de arma 0/1/otros incl. 0x80000000, multiplicadores ordinarios, 0, diminutos, enormes (producto fuera del rango de float pero no de la x87), inf, NaN.

## Suposiciones y riesgos

- `Cc63bCruiseSpeedCpp`, `AutoPreMultiplyMatrixWithVectorCpp` y los helpers x87 se compilan `/arch:IA32` en `X87Math.cpp`; si se cambian las opciones de ese archivo, repetir el banco.
- El empate exacto de mantisas de dos NaN de distinto signo no se modela (no aparece en las fixtures).
- El constructor de `CTaskComplexFollowPointRoute` corre de verdad en ambos lados (con su hook encendido en HEAD): validar `CreateRouteTask` no valida el constructor.
- Los llamadores en la DLL que usan copias propias de matriz × punto (`CTaskComplexWalkRoundObject`, `Collision.cpp`, etc.) no cambian; `CarCtrl.cpp:2240` sigue llamando 0x59C890 por dirección (ahora entra en la DLL con el hook encendido).
- `run_local.py` se calibró con la primera corrida completa (umbrales de ramas con margen).
