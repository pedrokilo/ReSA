<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ensamblador en dieciséis archivos: doce convertidos y cuatro que no se pueden pasar a C++ (12/10/2026)

Archivos pedidos: `VisibilityPlugins.cpp`, `Wanted.cpp`, `WeaponInfo.cpp` (un bloque x87) y `BreakObject_c.cpp`, `BulletInfo.cpp`, `CarCtrl.cpp`, `Clothes.cpp`, `Clouds.cpp`, `Collision/Collision.cpp`, `Curves.cpp`, `Entity/Ped/PedSaveStructure.cpp`, `Events/EventScriptCommand.cpp`, `Fx/Fx.cpp`, `Fx/FxFrustumInfo.cpp`, `Fx/FxManager.cpp` y `Garages.cpp` (un bloque x87 que es una función naked entera, salvo en los dos primeros). Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md). Continúa el [lote anterior](../CppThirteenFilesStudy/README.md).

## Convertidos (12 funciones en 12 archivos)

| Archivo / función | Fragmento original | Solución en C++ |
| --- | --- | --- |
| `VisibilityPlugins.cpp`, `SetupVehicleVariables` (0x733160) | `fpatan` de (altura, distancia horizontal) | `TrigAtan2Cpp` |
| `Wanted.cpp`, `ReportCrimeNow` (0x562120) | `fld; fiadd; fstp` | `FloatPlusIntCpp` |
| `WeaponInfo.cpp`, `GetTargetHeadRange` (0x743D50) | `fild; fmul; fmul; fstp` | `IntTimesTwoFloatsCpp` |
| `Clouds.cpp`, `CloudsRotationSumX87` (`Update`, 0x712FF0) | `fild` + 2^32 + `faddp` sin redondear | `UnsignedPlusFloatCpp` |
| `BreakObject_c.cpp`, `AcosX87` (`Update`, 0x59E220) | acos del CRT (palabra de control, `fpatan`) sobre un double | `ExeAcosDoubleCpp` (y `ExeAcosCpp` pasa a apoyarse en él) |
| `BulletInfo.cpp`, `AsinDegreesX87` (`Update`) | asin del CRT por 57.29578f | `ExeAsinDegreesCpp` |
| `Curves.cpp`, `CalcCorrectedDistX87` (0x43C880) | 36 líneas x87 con `fcos`/`fsin` | `CurvesCorrectedDistCpp` (aritmética en modo `precise`, trigonometría por llamadas aparte) |
| `Fx/FxManager.cpp`, `FxFrustumRadiusX87` (`CalcFrustumInfo`, 0x4A9140) | `fpatan`, `fsqrt`, dos `fsin` | `FxFrustumRadiusCpp` |
| `Fx/Fx.cpp`, `Fx_c::CreateMatFromVec` (0x49E950) | 83 líneas naked (identidad, normalización, producto cruzado) | C++ float en el mismo orden de escrituras; `RwV3dNormalize` y `RwMatrixUpdate` de RenderWare |
| `CarCtrl.cpp`, `CCarCtrl::FindGhostRoadHeight` (0x422370) | naked: interpolación de altura entre dos nodos | C++ con cuatro llamadas a `CPathNode::GetPosition` (A, B, A, B) |
| `Clothes.cpp`, envoltorio `KeepRegs` de `GetTextureDependency` (0x5A7EA0) | naked que conservaba ECX y EDX | la propia función como hook; por pragma sólo toca EAX (comprobado en el objeto) |
| `Entity/Ped/PedSaveStructure.cpp`, `Construct` (0x5D43D0) | 69 líneas naked | C++ que devuelve el residuo EDX:EAX del EXE como `uint64`; ECX = 0 por el `rep movsd` |

Helpers nuevos en `X87Math`: `ExeAcosDoubleCpp`, `ExeAsinDegreesCpp`, `UnsignedPlusFloatCpp`, `FloatPlusIntCpp`, `IntTimesTwoFloatsCpp`, `CurvesCorrectedDistCpp` y `FxFrustumRadiusCpp`. El resto reutiliza los existentes (`TrigAtan2Cpp`). En el objeto de `X87Math`: `ExeAsinDegreesCpp` hace `fpatan` bajo la palabra de control del EXE y multiplica por la constante después de restaurarla (como el original), y las constantes son `static constexpr` (la primera versión con una función `Float32` dejaba llamadas en línea).

## Cuatro que siguen naked (y por qué)

> **Actualización:** estas cuatro se convirtieron después sin relajar los bancos; ver [CppFourNakedStudy](../CppFourNakedStudy/README.md). Lo que sigue describe el primer intento.

Se convirtieron, se probaron y se **volvieron a su versión naked validada**, porque el comportamiento de C++ no puede igualar lo que el EXE deja en los registros y la comparación del banco (o los llamadores del EXE) lo exige:

| Función | Motivo |
| --- | --- |
| `FxFrustumInfo_c::IsCollision` (0x4AA030) | El EXE conserva ECX y deja EDX = esfera. La versión en C++ (devolviendo `uint64` para EDX:EAX) pasaba la lógica pero el compilador usa ECX como temporal, incluso desenrollando el bucle a mano, y los llamadores del EXE lo necesitan: `FxManager_c::ShouldCreate` dio un resultado distinto en Fx25 y DependenciesWorldLeaf3 falló 4096/4096. |
| `CGarages::FindGarageForObject` (0x44A240) | DependenciesWorld4 compara EAX, ECX y EDX; el EXE deja en ECX el puntero al final del arreglo de garajes y en EDX la matriz del objeto. C++ no puede dejar ECX. |
| `CEventScriptCommand::Destructor` (0x4B0A50) | DependenciesDecisionPed7 compara EAX y ECX; el EXE deja en ambos la cabeza anterior de la cadena de excepciones (`fs:[0]`). C++ no puede dejar ECX. |
| `CCollision::SphereVsEntity` (0x41A5A0) | Lee 16 bytes de pila sin escribir bajo su marco; para que el relleno de `/RTC1` no los destruya hay que compilar `Collision.cpp` entero sin comprobaciones de ejecución, y eso cambió `ColWorld59c` (pasaba con 13568 pares y pasó a fallar en nueve funciones de `SphereCast*` y `CheckCameraCollision*`). |

Las evidencias de los intentos fallidos están en `reversing/CppSixteenFilesUnified/runs/` (corrida con las cuatro convertidas) y `reversing/CppSixteenFilesBaseline/runs/` (línea base con el código de `HEAD`: Fx25 y ColWorld59c pasaban; BreakObj y Data23 fallaban igual). Si se quisieran convertir, harían falta o bien relajar esos bancos para comparar sólo el contrato de la convención de llamada (no lo he hecho: sería relajar una compuerta), o bien que el EXE no dependa de esos residuos.

## Comprobaciones en los objetos reales

- `X87Math.obj`: `ExeAcosDoubleCpp` y `ExeAsinDegreesCpp` hacen `_control87` con `(anterior & PC) | RC_NEAR | EM`, `fld1; fadd; fsub; fmulp; fsqrt; fpatan` y restauran; `UnsignedPlusFloatCpp`, `FloatPlusIntCpp` e `IntTimesTwoFloatsCpp` son `fild`/`fiadd` con un solo redondeo; `CurvesCorrectedDistCpp` y `FxFrustumRadiusCpp` hacen la aritmética en orden estricto con llamadas aparte a `TrigSimple*Cpp`/`TrigAtan2Cpp` (en modo `precise` el compilador convierte las funciones trigonométricas y `sqrt` en llamadas a la CRT, por eso `sqrt` va en un auxiliar fuera de ese modo).
- `Clothes.obj`: `GetTextureDependency` sólo escribe EAX (`push ebp; mov ebp,esp; mov eax,[ebp+8]; cmp eax,11h; … pop ebp; ret`).
- `PedSaveStructure.obj`: tras el `rep movsd` ECX = 0 y el valor devuelto es EDX:EAX con el residuo del EXE.

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppSixteenFilesUnified/runs/`, `CppSixteenFilesUnified2/runs/` (segunda tanda, tras volver a naked las cuatro) y `CppBankFixes3/runs/` (bancos corregidos).

| Banco | Funciones afectadas | Pares | Resultado |
| --- | --- | ---: | --- |
| VisPluginsFinal4 | `SetupVehicleVariables` | 18176 | aprobado |
| Wanted33 | `ReportCrimeNow` | 15616 | aprobado |
| Data10 | `GetTargetHeadRange` | 8896 | aprobado |
| Clouds21 | `CClouds::Update` | 8384 | aprobado |
| BreakObj (corregido) + BreakBatch | `BreakObject_c::Update` | 10752 + 3232 | aprobado |
| Data23 (corregido) | `CBulletInfo::Update` | 13904 | aprobado |
| Misc6 | `CalcCorrectedDist` | 19456 | aprobado |
| Fx25 | `CalcFrustumInfo` | 14336 | aprobado |
| DependenciesWorldFxFont5, Object48, Cutscene35 | `CreateMatFromVec` | 15360, 18368, 13184 | aprobado |
| DependenciesWorld14, CarCtrl22, Physical56 | `FindGhostRoadHeight` | 2048, 10304, 23808 | aprobado |
| ClothesScriptedSprint, Shop30 | `GetTextureDependency` | 8448, 9024 | aprobado |
| DependenciesTaskLeaf3, CPedD | `Construct` | 4096, 8384 | aprobado |
| DependenciesWorldLeaf3, DependenciesWorld4, DependenciesDecisionPed7, ColWorld59c, GeoFinal1, DependenciesGarage2 | las cuatro naked restauradas | 8192, 4608, 8192, 13568, 4608, 2560 | aprobado |
| FileLoader47 | `ExeAcosCpp` tras apoyarse en la versión double | 11968 | aprobado |

## Dos bancos corregidos

BreakObj y Data23 **fallaban igual con el código de `HEAD`** (línea base): BreakObj 796/2048, 279/1024 y 368/2048 en `BreakObject_c`, y Data23 con una violación de acceso en `CBulletInfo::Update`. Eran defectos de instrumentación: el banco intercepta a los colaboradores sólo en las direcciones del EXE y el candidato en C++ llama a la DLL. No se retiró ni relajó ninguna compuerta.

- **BreakObj:** faltaban `FxSystem_c::AddParticle` y `Fx_c::AddSparks` de la DLL. 10752 pares, cero diferencias.
- **Data23:** faltaban `CWeapon::CheckForShootingVehicleOccupant` (la causa del cuelgue), `CCamera::IsSphereVisible` (versión con matriz), `CGarage::IsPointInsideGarage` (punto) y `CGarage::EntityHasASphereWayOutsideGarage`. 13904 pares, cero diferencias; `CBulletInfo::Update` 1024/1024.

## Límites

- Sin auditor propio por banco; sin cobertura exhaustiva de x87 más allá de sus fixtures.
- `CurvesCorrectedDistCpp` y `FxFrustumRadiusCpp` suponen precisión de 24 bits, como todos los helpers de `X87Math`.
- Cuatro funciones siguen en ensamblador (tabla anterior).
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| BreakObject_c.actual-after.obj | d433867ee03b18a6978fdebfa536a562ae176850a907cdb1fe4927562c631751 |
| BreakObject_c.actual-before.obj | c9b61a23df5afa16b4402e3b2744a07516771f6b6ca78ef17b1a215507c06425 |
| BulletInfo.actual-after.obj | 9c6a5466dbd0cc9403a663f7a889306c1b3ccd421d3569f468893c14d0211c56 |
| BulletInfo.actual-before.obj | 2e03a4e29b425066e4e9831dcdae7f03b2a45b8709effff3da1f4afac28d62ba |
| CarCtrl.actual-after.obj | 0aaf77e8ec132e73dec4e7158d3d6504f18ec6fe7ee28605f14c836124e09ab8 |
| CarCtrl.actual-before.obj | e85104c30e1d7753d7489fd74d37dc87d76f2d9544b097c82066cbd1ecccf8ce |
| Clothes.actual-after.obj | 99fb0ada9d97774362ed6b0314822c0a9390409cd5763963f7f48bf177e05536 |
| Clothes.actual-before.obj | 1e0115f6c5d65acb9c569b06446bfabc45a8cf5bb5445c8ec6c492cea818dcc3 |
| Clouds.actual-after.obj | af57cb256024c60884f3b2dddff722d962371d6b793967108bb527c6475842bb |
| Clouds.actual-before.obj | 2f00839faf5884587beb0d76891cb9553fbef772b383db8e2d99cf7524b1d9b3 |
| Collision.actual-before.obj | 9b53527030137ba8523ad6dd5c3e615f856522de49fe906895e76fe0ba7ebd76 |
| Curves.actual-after.obj | 2ca230422297ca6d617fe44084920ce081fda37046007aadc93a49b767bcd464 |
| Curves.actual-before.obj | 77e7b5e955f765cac92a8c514ba489038402e3603a45909a723217ab7bb5d89f |
| EventScriptCommand.actual-before.obj | 003a0c20b062750a62024513d35d771edaf223373db481bf3950e52b24504a48 |
| Fx.actual-after.obj | 3abc236b7fb94d4b9212e5270cb20a1c246bcc2fbb4d53a74cd311dfde56f04d |
| Fx.actual-before.obj | a54b5c3d927d6792eb2a3d5a4aaea0ab0d5607898f972542f7bbcf6a7b87b232 |
| FxFrustumInfo.actual-before.obj | 4447a70df9ec875d536cf26868669aa540a7c7911cea0eec0e5717ccd3b8f258 |
| FxManager.actual-after.obj | 3aeb558a4a48634b200d6395a4e07776b5bcd9b4350a08dda7d410b4be881b59 |
| FxManager.actual-before.obj | 4acc9b046753d5f54c8dd2dffdfc9d8e4f2e1a9cdd553698f25c6ef5f5a6a536 |
| Garages.actual-before.obj | 2e476a881484486483cc99ed4968842d681285d7518cfb2a37a422b180f67eb9 |
| PedSaveStructure.actual-after.obj | 1fa7d5d2ee68cdba52870506eb480b9bfbce566adf037ff25825063ca2b8a6a5 |
| PedSaveStructure.actual-before.obj | b28485813ff88ac8b4cc812adeb9b13d0185c453eaec143a95bd8e7892fc2b4d |
| VisibilityPlugins.actual-after.obj | 92a9ae9cca48daee5bdf957ae471de47f418e6cf7342221e4740938a57a14a98 |
| VisibilityPlugins.actual-before.obj | 16baad4a0514aee9d0e29f42675ab9516ff8099b41b26cb396d9e066db30389a |
| Wanted.actual-after.obj | 43090e84d29e811d49b24d55963f85a07a30d969835c5b083700d1f626d3ba49 |
| Wanted.actual-before.obj | e3a81ba75b8f184c602bb553f524c5f8bd2662cb5610077205bb0d9dcc3d8077 |
| WeaponInfo.actual-after.obj | 0abacbbc8a3b9ecb948bf07accc00075cb0d868a08dcdf14dedc7eb3421650d5 |
| WeaponInfo.actual-before.obj | 85e1e50e8d410a00d58752118c567c367016be2d07bb3ec99dd10c2cf018debf |
| X87Math.actual-after.obj | 46c99b2b16611cb757e66d844b0bd6980f7dfb4be7466ac0f62760d6733dda8b |
| X87Math.actual-before.obj | e58246b911ecc8581b56bde1f7595d2f07da55078f5bedd7b1919d765f3b4908 |
