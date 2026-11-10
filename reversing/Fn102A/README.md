<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 879 grupos / 9815 funciones reconstruidas; 9815 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn102A: nueve funciones sueltas de Fx, audio, transformaciones, vectores y RenderWare (lote 102A)

Estado: **APROBADO dentro del juego** (05/11/2026, worktree `wv-2` sobre main e9a8cb895 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn102a/20261105-170047-904870` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn102A-20261105-170047-904870`). Banco par a par dentro del juego, 9 x 16384 = 147456 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 (incluido ST0 de `SquaredMagnitudeExe`) igual, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`: modelo exacto de listas y arreglos, aritmetica racional a 24 bits y mpmath a 300 bits para fsin/fcos).

Nueve funciones de codigo de juego, todas con hook S (`RH_ScopedInstall`):

| # | Funcion | Direccion | Bytes | Convencion | Colaboradores sustituidos (falsos identicos en el EXE y en el cuerpo de la DLL) |
|---|---------|-----------|------:|------------|------------------|
| 0 | `FxPrtMult_c::Constructor7f` (constructor de 7 floats) | 0x4AB290 | 53 | thiscall, 7 floats en pila, `ret 1Ch`; devuelve this | ninguno |
| 1 | `CSimpleTransform::UpdateMatrix` | 0x54F1B0 | 42 | thiscall, matriz en pila, `ret 4` | `CMatrix::SetTranslate` 0x59AF40, `CMatrix::SetRotateZOnly` 0x59B020 (con hook) |
| 2 | `CSimpleTransform::UpdateRwMatrix` | 0x54EF40 | 77 | thiscall, RwMatrix en pila, `ret 4` | ninguno (fcos/fsin y `RwMatrixUpdate` 0x7F18A0 corren de verdad) |
| 3 | `CVector::SquaredMagnitudeExe` | 0x406DA0 | 31 | thiscall, float en ST0; ECX y EDX intactos | ninguno |
| 4 | `CAESound::Destructor` (~CAESound) | 0x4EF660 | 24 | thiscall | `CEntity::CleanUpOldReference` 0x571A00 (con hook) |
| 5 | `RpClumpForAllAtomics` | 0x749B70 | 57 | cdecl (clump, callback, dato) | callback del banco |
| 6 | `RpGeometryForAllMaterials` | 0x74C790 | 62 | cdecl (geometria, callback, dato) | callback del banco |
| 7 | `RpAtomicSetFrame` | 0x74BF20 | 31 | cdecl (atomico, marco) | `_rwObjectHasFrameSetFrame` 0x804EF0 (sin hook: el falso va en la direccion del EXE) |
| 8 | `AtomicDefaultRenderCallBack` | 0x7491C0 | 47 | cdecl (atomico) | `RxPipelineExecute` 0x805710 (sin hook) |

## Cambios de produccion
- `FxPrtMult.cpp/.h`: nuevo `Constructor7f` (envoltorio de `FxPrtMult_c(7 floats)`) con `RH_ScopedInstall(Constructor7f, 0x4AB290)`; el hook de 0x4AB270 (`Constructor`) no cambia de nombre.
- `SimpleTransform.cpp/.h`: `CSimpleTransform::InjectHooks` (solo `UpdateRwMatrix` 0x54EF40 y `UpdateMatrix` 0x54F1B0; `Invert` 0x54EF90 sigue sin hook) y su llamada en `InjectHooksMain.cpp`. Sin cambios de cuerpo.
- `Vector.cpp/.h`: `CVector::SquaredMagnitudeExe` (0x406DA0), `(x*x + y*y) + z*z` en la x87 sin guardar como float, instalada junto a Magnitude/Normalise (`InjectHooksExactMath`); `Vector.cpp` ya se compila con `/O2 /Oi /arch:IA32 /fp:fast`. `SquaredMagnitude()` (en linea, SSE) no se toca: los demas llamadores siguen igual.
- `AESound.cpp/.h`: `CAESound::Destructor` (envoltorio de `~CAESound`) con `RH_ScopedInstall(Destructor, 0x4EF660)` (categoria Audio).
- `rpworld.cpp`: cuerpos en C++ de `RpClumpForAllAtomics`, `RpGeometryForAllMaterials`, `RpAtomicSetFrame` y `AtomicDefaultRenderCallBack` (antes saltaban a la direccion del EXE) y sus hooks en `RpWorldInjectHooks25b`.
- Sin `__asm`, sin `naked`, sin cambios en `X87Math.cpp` ni en `source/CMakeLists.txt`.

## Detalles del EXE reproducidos
- `RpClumpForAllAtomics`: el enlace siguiente se lee ANTES del callback (el callback puede quitar o sobrescribir el atomico actual); atomico = enlace - 0x40; un resultado 0 corta; devuelve el clump.
- `RpGeometryForAllMaterials`: el numero de materiales (int32 en +0x24) se lee una sola vez y no se recorre si es <= 0; el arreglo (+0x20) se relee en cada vuelta.
- `RpAtomicSetFrame`: el byte +3 (`privateFlags`) se relee tras `_rwObjectHasFrameSetFrame` y se le hace `|= 1`.
- `AtomicDefaultRenderCallBack`: sin tuberia propia usa la del motor (`[0xC9BC60] + [0xC97B24] + 0x3C`); llama a `RxPipelineExecute(tuberia, atomico, 1)`.
- `CVector::SquaredMagnitudeExe`: orden (x*x + y*y) + z*z; el valor sale en ST0 sin redondear a float (24 bits de mantisa, exponente ampliado) y ECX/EDX no se tocan (varios llamadores del EXE, p. ej. 0x4724AE, hacen `push ecx` justo despues de la llamada).
- `CAESound::Destructor`: la referencia fisica (+8) se limpia con `CEntity::CleanUpOldReference(&referencia)` solo si no es nula y despues se pone a 0. El EXE no toca EDX (revisadas las 82 llamadas a 0x4EF660: ninguna lee EDX justo despues).
- `CSimpleTransform::UpdateRwMatrix`: `fcos`/`fsin` de precision completa guardados como float, `right = (cos, sen, 0)`, `up = (-sen, cos, 0)`, `at = (0, 0, 1)`, `pos = posicion`, y `RwMatrixUpdate` (flags &= 0xFFFDFFFC); los rellenos no se tocan.

## Banco
Observado en cada lado: valor devuelto (EAX como etiqueta de arena), ST0 y estado x87 completo, ECX/EDX (solo `SquaredMagnitudeExe`), bytes cambiados de las cuatro arenas (con guardas de 0x40 bytes a cada lado), eventos de los falsos y de los callbacks con todos sus argumentos (punteros a arenas como etiquetas), y los punteros que la funcion deja en memoria (enlaces de la lista, referencia, arreglo y contador de la geometria) como etiquetas (se enmascaran antes de comparar bytes, porque son direcciones reales).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): patrones de float con NaN (senalizantes y con carga), infinitos y desnormales (constructor y transformaciones); rumbos normales, multiplos de pi y pi/2 con ulps, minusculos, >= 2^63, infinito y NaN; sumas de cuadrados con cancelacion, enormes y diminutas; referencias nulas, de arena y crudas; listas de 0 a 5 atomicos, cortadas en la llamada elegida, con callbacks que sobrescriben el enlace siguiente o el anterior y anillos sin terminador; arreglos de 0 a 8 materiales, contadores negativos y enormes, y callbacks que cambian el arreglo o el contador a media vuelta; marcos nulos/de arena/crudos con el byte +3 en todos los estados; tuberia propia o la del motor con `RxPipelineExecute` devolviendo 0 o no nulo.
Memoria real del juego: solo se LEEN `[0xC9BC60]`, `[0xC97B24]` y la tuberia por defecto del motor (la lectura se registra en la fila `begin`, `default_pipeline`). No hay regiones del juego que se escriban.

## Fuera de los fixtures
- Rumbos de `UpdateRwMatrix` entre 1e6 y 2^63 (la reduccion de argumentos de la x87 usa pi de 66 bits; el oraculo no la reproduce; EXE y DLL ejecutan la misma instruccion).
- NaN con cargas distintas de 0xFFC00000 en `SquaredMagnitudeExe` (el modelo no reproduce la regla de dos NaN).
- Clumps con listas corruptas, geometrias con arreglos de mas de 8 materiales sin callback que corte y atomicos con enlace nulo (`RpClumpGetNumAtomics` tiene un caso especial que `RpClumpForAllAtomics` no tiene).
- La exactitud de los colaboradores sustituidos.

## Funciones del encargo que NO se hicieron en este lote
- `CAutomobile::SetRandomDamage` 0x6A2530 (703 B): el cuerpo en C++ actual usa `CGeneral::GetRandomNumberInRange` y no la secuencia exacta del EXE (`rand() & 0xFFFF` por un float, por otro, a `_ftol`, con 6 constantes distintas); hace falta reescribirlo con la x87, falsear la tabla de modelos 0xA9B0C8, el arreglo de componentes y siete colaboradores con hook. Queda para otro lote.
- 0x4AB250 (5 bytes: `jmp 0x5389B0`, un trampolin que llama al CRT 0x823798) y 0x4A8DF0 (`TList_c::AddItem`, plantilla compartida por muchos tipos: no se puede enganchar como funcion suelta).
- 0x749DC0 (`RpAtomicDestroy`): depende de mas de 2 funciones sin hook (0x808740, 0x807DE0, 0x74CCC0, 0x751050 con hook y otras).

## Prueba negativa
Copias buenas (SHA256 de las cinco fuentes mutadas, restauradas y comprobadas con `mutate.py restore`): `FxPrtMult.cpp` 228df3d5..., `SimpleTransform.cpp` c9314747..., `Vector.cpp` 66bb3ec8..., `AESound.cpp` 7080745e..., `rpworld.cpp` 60667fc7... (ver `artifacts/build-manifest.json`). Cada ronda aplica una mutacion a cada una de las nueve funciones (`mutate.py`); las mutaciones de funciones que el juego usa de verdad (RenderWare, Fx, matrices) se eligieron para que el juego siga arrancando (una primera ronda con un dato nulo en el callback de `RpClumpForAllAtomics` tumbo el juego al arrancar y se descarto).
- Ronda 1 (`SetUp` con tamano y rotacion intercambiados; `UpdateMatrix` con las dos llamadas en orden inverso; `up.x` con el signo del seno equivocado; suma `(z*z + y*y) + x*x`; destructor sin limpiar la referencia; `RpClumpForAllAtomics` devolviendo nulo si no llego al final; arreglo de materiales leido una sola vez; marca `|= 1` antes de la llamada; resultado de `RxPipelineExecute` ignorado): diferencias en las nueve funciones: 15530 / 16384 / 16384 / 631 / 11806 / 4907 / 1440 / 11841 / 8674 pares de 16384 (`20261105-180331-635459`).
- Ronda 2 (alfa por defecto; rumbo con el signo cambiado; sin `RwMatrixUpdate`; suma `x*x + (y*y + z*z)`; destructor que solo limpia referencias no alineadas; como mucho 8 llamadas al callback; contador de materiales releido en cada vuelta; sin marcar la esfera como sucia; tuberia propia impar sustituida por la del motor): 16384 / 16384 / 14674 / 631 / 8264 / 596 / 2947 / 11841 / 4158 (`20261105-182708-120820`).
Fuente restaurada con comprobacion SHA256 contra la copia buena tras cada ronda. Las corridas negativas no se archivan (solo `summary.json` y `RESULTADO.txt` quedan en el clon de pruebas).

## Bancos repetidos (todos con el codigo final de produccion)
De los 90 bancos que citan alguna de las nueve direcciones (grep sobre `reversing/*/` salvo `runs/`, `_handoff/` y este banco) se repitieron 23; el resto queda en la lista de abajo (se anticipa que los que parchean solo la direccion del EXE de `RpClumpForAllAtomics`, `RpGeometryForAllMaterials`, `RpAtomicSetFrame` o `AtomicDefaultRenderCallBack`, o que dan por sin hook `0x54F1B0`/`0x54EF40`, necesitaran el mismo ajuste que los adaptados).
- Aprobados sin cambios en el banco (12): `CEntB`, `WldD`, `PopFinal4`, `HudCamFinal2`, `Misc8`, `Car81C`, `Veh81A`, `Fx83A`, `Veh83C`, `CEntC`, `ScriptLoaded355`, `Boat23`.
- Bancos antiguos que parcheaban solo la direccion del EXE de una funcion de RenderWare que ahora tiene cuerpo y hook en la DLL (la DLL llamaba al EXE; ahora ejecuta su propio cuerpo y el candidato se caia con un acceso invalido sobre los datos falsos). Se les anade el parche del cuerpo de la DLL (mismo falso, `&RpClumpForAllAtomics`, `&RpGeometryForAllMaterials`, `&RpAtomicSetFrame`, `&AtomicDefaultRenderCallBack`) y despues se aprueban: `Render19` (749B70, 7491C0), `VisPluginsFinal4` (749B70, 74C790, 7491C0), `VehModelInfoFinal4` (749B70, 74C790, 74BF20), `Models9` (749B70, 74C790, 74BF20), `Clump24` (749B70), `Render14` (74C790, 749B70) y `Mix14` (74C790, 749B70).
- Bancos antiguos que daban por sin hook un colaborador que ahora lo tiene (`collaborators_dll`), corregidos y aprobados: `PickUpFirst68` (0x54EF40 y 0x54F1B0 pasan a colaboradores con hook, como 0x59B020 y 0x59AD70) y `FxWheel77` (0x54F1B0 por `MakeCollab`).
- `AutomobileWrappers60` (sin cambios en el banco) y `Clump24` fallaron una vez (el juego termino antes de completar en una de las dos; no se conserva el detalle porque el limpiador borro los crudos antes de leerlos) y se aprobaron al repetir con exactamente el mismo codigo y banco.
- NO aprobado y no adaptado: `DependenciesGarageSphere15` (`invalid fixture/native resources`: exige que 0x54F1B0 y 0x59C890 sean codigo original del EXE; 0x59C890 ya tenia hook desde antes de este lote, de modo que el banco ya no podia pasar; hace falta una HookRoute para cada una).
- Sin repetir (67; lista exacta): AnimAudio6, AnimMgr31, Audio15, Audio7, AutomobileFinal4, CBDNMaterialSetup30, CPedA, CPedB, CTasks14b, CVehB, CVehC, CameraCollision355, CarCtrlAir63, CarCtrlSteer63d, CarCtrlWeave63A, ClothesGeometry355, ClothesWave3, CollisionScan61, CppModelInfoRegression, CppPedFootEmitters1, CppPlaneX87Stress1, CppTrigCompound6Markers, CppTrigCompoundMarkers, CppTrigJetPack, Cutscene35, Dependencies21C, Dependencies21D, Dependencies23B, DependenciesWorld15, EnvMap37, EventScanners61, FileLoader47, Fn86B, Fn92B, Fn95C, FollowFootsteps62, FxBlood79, FxImpact78, FxWood73, FxWoodNeg73, Garage63, GeoFinal1, Helpers66A, Helpers66C, Helpers66D, InteriorOffice355, InteriorSpawn355, Markers, Misc56, Misc61b, Mix12, PedEntityCollision60, PedWrappers60, Plant19, RadarFinal3, ReplUpgrade72, ScriptsFinal3, ShadowSectors355, Small2, SmallHelpers64A, TwoDEffect, Vehicle66, VehicleAudio355Evidence, WalkRoundControl31, WalkRoundFirst31, WldC, World52.
Cada banco repetido se refresco con `reversing/_handoff/refresh_bank.py` (registro, `validation-summary.json` y copia de la corrida en `reversing/CppTwelveLevel6Final/runs/<Banco>-<corrida>`).

## Tiempo
Una corrida de los 147456 pares: unos 7 minutos (la compilacion incremental de la sonda, unos 30 segundos con los candados libres).
