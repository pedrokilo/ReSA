<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Conversión del código de producción a C++ sin ensamblador

Política y proceso: [PROCESO.md](PROCESO.md) · revisión de lo anterior: [AUDITORIA_ARCHIVOS_AUXILIARES.md](AUDITORIA_ARCHIVOS_AUXILIARES.md) · plan: [PLAN_POR_LOTES.md](PLAN_POR_LOTES.md).

**Estado actual (16/10/2026):** 821 funciones fuente convertidas y revalidadas dentro del juego (2 más, convertidas, esperan la corrida aprobada de su banco en la regresión final: ver el [estudio del lote](../CppTwelveLevel6Study/README.md)); quedan 0 apariciones de ensamblador y 0 declaraciones `naked` en 0 archivos de `source/game_sa` (conteos léxicos, no funciones). Única excepción documentada: `source/app/platform/win/WindowedMode.cpp` conserva `__asm` en `HookDirect3DDeviceReplacerSA` (extensión NOTSA del modo ventana, fuera de `source/game_sa`; decisión del propietario, opción A). Todas las clases convertidas están en sus dos archivos `.cpp` y `.h`, con `X87Math.cpp/.h` como único módulo compartido de helpers x87. Las secciones fechadas más abajo conservan las cifras del momento en que se escribieron. Documentos: [PROCESO.md](PROCESO.md) (política y técnicas), [PLAN_POR_LOTES.md](PLAN_POR_LOTES.md) (lo que falta, por dificultad), [FILES.md](FILES.md) (archivos pendientes), [AUDITORIA_ARCHIVOS_AUXILIARES.md](AUDITORIA_ARCHIVOS_AUXILIARES.md), [REVALIDACION_INTERIOR_CARCTRL.md](REVALIDACION_INTERIOR_CARCTRL.md) y [approvals.json](approvals.json) (evidencia por función).

Orden del usuario del 10/10/2026: eliminar ensamblador en línea y declaraciones naked de source/game_sa conservando resultados exactos del EXE, sin tolerancias numéricas.

El inventario inicial identificó 185 archivos con 965 apariciones de ensamblador y 312 declaraciones naked, tras excluir comentarios y cadenas. Son apariciones léxicas, no funciones. [inventory.csv](inventory.csv) y [summary.json](summary.json) se regeneran con `python -B contrib/audit_cpp_assembly.py`; el análisis no sustituye un parser C++ ni una comprobación de ramas del preprocesador.

## Conversiones comprobadas

[approvals.json](approvals.json) conserva 111 conversiones de funciones fuente con sus auditorías dentro del juego:

| Función | Entrada EXE | Pares propios |
| --- | --- | ---: |
| FindSequenceElement | 0x421770 | 2048 |
| ClearInterestingVehicleList | 0x423F00 | 256 |
| GetAnimIdForPed | 0x61B460 | 2048 |
| Office_FurnishCenter | 0x599A30 | 4096 |
| Shop_FurnishCeiling | 0x59A130 | 2048 |
| RwTexDictionaryFindHashNamedTexture | 0x734E50 | 2048 |
| FurnishOffice | 0x599AF0 | 2048 |
| Shop_AddShelfInfo | 0x59A140 | 4096 |
| CanSunbathe | 0x632140 | 2048 |
| Get2dEffectStore | 0x4C5A60 | 1024 |
| CloudsSinX87 / CloudsCosX87 / CloudsSinMulX87 | Helpers fuente, tres consumidores | 3072 compartidos |
| BirdsSinMul / BirdsCosMul | Helpers fuente, CBirds::Render | 512 compartidos |
| BrightSinMulX87 / BrightCosMulX87 / PlaneTrailsSinMulX87 / PlaneTrailsCosMulX87 | Helpers fuente, dos consumidores | 1536 compartidos |
| JpMulSinX87 / JpMulCosX87 | Helpers fuente, ProcessPed y RenderJetPack | 1536 compartidos |
| SinX87 / CosX87 de Swim | Helpers fuente, ProcessControlInput | 2048 compartidos |
| FollowPointRouteCosX87 | Helper fuente, CalcBlendRatio | 512 |
| ClimbSinMulX87 / ClimbCosMulX87 | Helpers fuente, seis consumidores | 4608 compartidos |
| 3dMarkersSinMulAdd | Helper fuente, dos consumidores | 4608 compartidos |
| BirdsSinMulAdd / BirdsCosMulAdd / BirdsNegSinDiv | Helpers fuente, tres consumidores | 2560 compartidos |
| FollowNodeRouteCosHalfX87 | Helper fuente, CalcBlendRatio | 1024 |
| SwimSubSinX87 | Helper fuente, SwimmingResistance | 1024 |
| SetUseD3DResourceBuffering | 0x730AC0 | 512 |
| Native_00424100 | 0x424100 | 2048 |
| ShadowExtendedSqrt (X87Math) | Helper fuente, StoreStaticShadow y StoreShadowForVehicle | 8192 compartidos |
| ShadowDistanceFade (X87Math) | Helper fuente, StoreStaticShadow y StoreShadowForVehicle | 8192 compartidos |
| IsPlayerInPositionForConversation | 0x43AC40 | 2414 |
| CConversationNode::ClearRecursively | 0x43A710 | 2048 |
| CAnimBlendNode::CalcTheta | 0x4D00E0 | 1024 |
| CAnimBlendSequence::RemoveQuaternionFlips | 0x4D1190 | 512 |
| CPedShelterAttractor::GetDisplacement | 0x5EF420 | 512 |
| CAttractorScanner::Clear | 0x5FFF90 | 2048 |
| Log10Times20 (CAECollisionAudioEntity) | helper, tres consumidores | 3072 |
| Log10Times20 (CAEWeaponAudioEntity) | helper, PlayGunSounds | 1024 |
| CAEWMADecoder::GetStreamPlayTimeMs | 0x502AF0 | 512 |
| CalculateRadioVolume355 (UpdateRadioVolumes) | helper, 0x4EA010 | 1024 |
| CAESoundManager::Service (fragmento fild/fmul) | 0x4F0000 | 1024 |
| CAEAudioUtility::AudioLog10 | 0x4D9E50 | 5264 |
| CBouncingPanel::ProcessPanel (pow) | 0x6F49A0 | 18384 |
| CCheckpoints::SetHeading | 0x722970 | 18176 |
| SinTimesTenX87 (CCoronas) | helper, 0x6FBAA0 | 16704 |
| CCutsceneMgr::FinishCutscene | 0x5B04D0 | 13184 |
| CDecision::Set (ProbabilityByte) | 0x600570 | 3072 |
| CEntryExitManager::AddOne (rotación) | AddOne | 7808 |
| CFileLoader::LoadObjectInstance (acos) | 0x538090 | 11968 |
| CGameLogic::UpdateSkip (GetHeadingMinusHalfPi) | 0x442480 | 13888 |
| CGamma::SetGamma | 0x747200 | 12416 |
| CInterestingEvents::ScanForNearbyEntities | 0x605A30 | 2048 |
| CLoadingScreen (ClockSeconds) | GetClockTime 0x590280 | 9328 |
| CMaths::InitMathsTables | 0x59AC90 | 32016 |
| CVehicleModelInfo::SetEnvMapCoeffCB | 0x4C88B0 | 256 |
| CPedDamageResponseCalculator::ComputeWillKillPed (caída) | 0x4B3210 | 6144 |
| CCustomCarEnvMapPipeline (FildMul) | helper | 10880 |
| CPlantMgr::CalculateWindBending | 0x5DB3D0 | 1024 |
| CRenderer (FpatanMinus) | helper | 18112 |
| CRunningScript (áreas anguladas) | 0x487F60, 0x488780, 0x4883F0 | 6144 |
| CSprite2d::OffsetTexCoordForBilinearFiltering | 0x728150 | 11136 |
| CTaskAllocatorKillThreatsBasic::ComputeClosestPeds | 0x69C850 | 14784 |
| CTaskComplexDestroyCarMelee::CalculateSearchPositionAndRanges | 0x6289F0 | 15488 |
| CTaskComplexFleePoint (posición de huida) | 0x65B460 | 12672 |
| CTaskSimpleAchieveHeading::SetUpIK | 0x667F20 | 18880 |
| CTaskSimpleCarDrive::UpdateBopping | 0x63C900 | 26624 |
| CTaskSimpleCarJumpOut::ProcessPed | 0x64DD60 | 19136 |
| CTaskSimpleChoking::ProcessPed | 0x620490 | 32256 |
| CTaskSimpleGunControl (rumbo) | helper | 18688 |
| CTaskSimpleIKChain::ProcessPed | 0x633C80 | 14720 |
| CTaskSimpleJump::Launch | 0x679B80 | 12608 |
| CTaskSimpleStealthKill::ProcessPed | 0x62E540 | 19520 |
| CTaskSimpleThrowControl::ProcessPed | 0x61F9F0 | 15232 |
| CTaskSimpleThrowProjectile::ProcessPed | 0x62AF50 | 15232 |
| CVisibilityPlugins::SetupVehicleVariables | 0x733160 | 18176 |
| CWanted::ReportCrimeNow | 0x562120 | 15616 |
| CWeaponInfo::GetTargetHeadRange | 0x743D50 | 8896 |
| CClouds::Update (suma de rotación) | 0x712FF0 | 8384 |
| BreakObject_c::Update (acos) | 0x59E220 | 10752 |
| CBulletInfo::Update (asin en grados) | 0x7360D0 | 13904 |
| CCurves::CalcCorrectedDist | 0x43C880 | 19456 |
| FxManager_c::CalcFrustumInfo (radio) | 0x4A9140 | 14336 |
| Fx_c::CreateMatFromVec | 0x49E950 | 15360 |
| CCarCtrl::FindGhostRoadHeight | 0x422370 | 2048 |
| CClothes::GetTextureDependency | 0x5A7EA0 | 8448 |
| CPedSaveStructure::Construct | 0x5D43D0 | 4096 |
| FxFrustumInfo_c::IsCollision | 0x4AA030 | 8192 |
| CGarages::FindGarageForObject | 0x44A240 | 4608 |
| CEventScriptCommand::Destructor | 0x4B0A50 | 8192 |
| CCollision::SphereVsEntity | 0x41A5A0 | 4608 |
| CBaseModelInfo::SetFlags (envoltorio cdecl) | 0x5B3AD0 | 13280 |
| CClumpModelInfo::SetFlags (envoltorio cdecl) | 0x5B3C30 | 12256 |
| CPedGroupIntelligence::ComputeScriptCommandTasks | 0x5F7800 | 2048 |
| CPedPool::GetRef | 0x4442D0 | 8192 |
| IKChain_c::ProcessPed (ángulo de rotación) | 0x617CA0 | 11264 |
| RwHelper::GetAnimHierarchyFromFrame | 0x734AB0 | 2048 |
| CTaskInteriorSitInChair::Constructor | 0x675C30 | 4096 |
| CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange | 0x6458A0 | 19520 |
| CTaskSimpleBeHit::ProcessPed (rumbo) | 0x620A20 | 16896 |
| CTaskSimpleDead::ProcessPed (asin) | 0x630600 | 16896 |
| CTaskSimpleGoToPointFine::SetTargetPos | 0x65F330 | 12224 |
| CPedIntelligence::IncrementAngerAtPlayer | 0x421050 | 8192 |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D | 0x4BC470 | 4096 |
| GetPlantDensity (argumento en EAX) | 0x5DC210 | 18384 |
| FindPlayerHeading | 0x56E450 | 13120 |

Cada corrida aprobada tiene cero diferencias, resultados no nulos, rutas y restauración verificadas. Estas conversiones vuelven a probar implementaciones catalogadas; no incrementan las 105 aprobaciones de la cohorte de dependencias.

El lote IntegerMinimal2 paso 8704 pares compartidos: 4608 en World4 y 4096 en World10. Ambos cuerpos completos coinciden con el EXE, sin relocaciones. Se comprobaron argumentos fisicos, EAX/ECX/EDX y banderas definidas; AF de XOR es indefinido y no se exige. Partidas, configuracion y EXE se verificaron por hash. El trabajo se detuvo por orden del usuario tras este lote el 11/10/2026. La lista completa de los 177 archivos pendientes esta en [FILES.md](FILES.md), con CSV agrupado en [files.csv](files.csv) y lineas individuales en [inventory.csv](inventory.csv).

El corte aprobado tras TrigCompound6 contiene 930 apariciones de ensamblador y 277 declaraciones naked en 177 archivos. Los tres helpers de nubes comparten una campaña de 3072 pares sobre sus consumidores, sin contar tres veces esos pares ni añadir entradas del EXE. La regresión de 20 métodos de nubes pasó otros 8384 pares. Los diez helpers adicionales comparten 5632 pares en seis consumidores; no se multiplican por helper. Consultar el inventario y el registro de conversiones por separado. La migración completa sigue pendiente.

FindSequenceElement usa aritmética modular de 32 bits, lectura del byte del flag y resto firmado; excluye división por cero y INT_MIN/-1 como el IDIV original. Dos intentos de ClearInterestingVehicleList se rechazaron por alterar ECX/EDX; la versión final escribe directamente los dos slots de vehículo y preserva esos registros en la compilación probada.

InteriorCpp.cpp se compila con símbolos /Zi y sin PCH/unity: /ZI insertaba una reserva de pila que cambiaba las banderas de la función vacía. El banco de interiores instrumenta DeskQuad y acredita el cuerpo convertido, sin validación transitiva de ese colaborador. La prueba de texturas usa diccionarios y colaboradores RW reales, sin stubs. Su corrida anterior todavía usaba ensamblador y no cuenta como conversión; la aprobada es 20261010-235425-798187.

Los cálculos x87 y las envolturas que preservan registros necesitan soluciones específicas. El núcleo entero experimental de conversión flotante pasó 128976 casos offline; todavía no tiene aprobación dentro de GTA. Las propuestas de memoria y otros lotes siguen pendientes hasta demostrar su equivalencia en el juego.

No mover ensamblador a otro archivo de producción, escribir opcodes en un arreglo ni delegar el cuerpo al EXE para fingir una conversión. Los instrumentos de prueba quedan separados del objetivo de producción. No se reescriben hashes ni pruebas históricas para atribuirles la fuente nueva.

## Estudio de generación de instrucciones

El 11/10 se compiló compiler-math-study.cpp con MSVC 19.51 x86, /O2 /Oi /arch:IA32 /fp:fast. La función C++ `float(sin(x))` generó FLD, FSIN, RET, y `float(sqrt(x))` generó FLD, FSQRT, RET, sin almacenamiento float32 intermedio. sqrtf no estaba disponible como intrínseca en este estudio. Esto permite investigar conversiones que conservan operaciones x87 mediante generación del compilador, con fuente íntegramente C++. No demuestra equivalencia de expresiones mayores ni prueba dentro del juego: hay que inspeccionar cada objeto y contrastar operaciones, ABI, precisión y estado. /fp:fast no se aplica globalmente ni justifica tolerancias.

El segundo lote de interiores pasó los 6144 pares de 20261011-000453-501774 después de rechazar dos corridas con 54 diferencias C1. Shelf se compila en una unidad separada con opciones que producen FILD/FSTP directos de los argumentos. Su banco compara CW, SW completo, tag, registros80 vivos y MXCSR, conserva los 108 bytes raw y verifica restauración física. FIP/FDP de código/datos reubicados se conservan como evidencia sin exigir que las direcciones del EXE y la DLL sean iguales. Alcance: PC24/nearest y colaboradores instrumentados, sin aprobación transitiva. La regresión de las dos funciones anteriores está en un banco separado.

La regresión CppInteriorRegression1 20261011-000903-597439 repitió 6144 pares de Office_FurnishCenter y Shop_FurnishCeiling sobre las opciones finales, cero diferencias y cero registros nulos, con restauración verificada. Conserva la evidencia anterior y añade referencia de regresión en approvals.json.

Get2dEffectStore pasó 1024 pares propios más 1024 de ScratchPad; la regresión de sus otros 28 métodos pasó 5264 pares. /ZI añadía SUB ESP que cambiaba flags; /Zi en ModelInfo.cpp conserva el PCH y evita esa reserva. Se verificó PUSH/MOV EBP, MOV EAX constante, POP EBP y RET, sin alteraciones de flags/ECX/EDX/NV/x87. No se atribuyen esas garantías integrales a los otros 28 métodos.

TrigSimple10 conserva las operaciones FLD/FSIN o FCOS/FMUL cuando corresponde, sin temporales ni llamadas CRT. Ocho cuerpos consumidores, incluidas las lambdas de render, mantienen instrucciones y relocations salvo los destinos autorizados de los helpers. Un fallo del lector COFF se corrigió: .bss tiene payload ausente, no bytes del encabezado. Sus inicializadores de referencias también se compararon. Las cuatro campañas pasaron 5632 pares exactos; 459 casos de aves emitieron flush después de generar geometría y 74 casos de jetpack con ThrustStop alcanzaron ambos frames de sus jetballs. La cobertura se infiere del flujo fuente y las trazas medidas. Estos bancos conservan contratos históricos de resultados, memoria y trazas; no añaden una medición completa de FP108/EFLAGS/registros a cada consumidor. Evidencias en [CppTrigCoffAudit](../CppTrigCoffAudit/README.md).

Memoria superó las ocho calibraciones diagnósticas tras corregir la copia del contexto en app_debug.cpp; sigue pendiente la campaña completa sin logger y MemoryMgr.cpp permanece sin convertir. TrigCompound6 ya fue integrado y revalidado: seis helpers, 9216 pares compartidos en cuatro campañas, cero diferencias y restauración verificada. La cobertura medida incluye 311 casos de actualización de aves, 459 casos de geometría renderizada y 24 casos de la rama SubSin de natación. Los consumidores conservan sus contratos históricos; no se atribuye medición integral de FP108 o registros a cada uno.

TrigExtra3 aprobado: FollowPointRoute pasó 512 pares y Climb 4608, cero diferencias, sin resultados nulos y restauración verificada. Quince cuerpos COFF conservan instrucciones y relocations salvo los tres destinos autorizados. El primer banco Climb alcanzaba entidades externas: se aislaron todas las cabezas de sectores 120x120 y repeat 16x16, con restauración y guardia de ámbito; la campaña final no alcanzó entidades externas. Se acreditaron 51 casos de cosine en rutas y ocho casos de seno/coseno en escalada mediante inferencia de flujo y trazas. Evidencias en CppTrigFollowPointRoute/runs y CppTrigClimb/runs.

## ShadowSqrt (11/10/2026)

ShadowExtendedSqrt pasó a source/game_sa/X87Math.cpp (módulo compartido de helpers x87, compilado con /O2 /Oi /arch:IA32 /fp:fast /Zi sin RTC/PCH/unity; antes ShadowSqrtCpp.cpp). El objeto real hace FLD32, FSQRT, FSTP64, FLD64, sin relocaciones en .text. Los cuerpos completos de StoreStaticShadow (1258 bytes) y StoreShadowForVehicle (3464 bytes) son idénticos a los anteriores salvo el destino de la llamada (compare_coff, caller-spec.json). Bancos: CppShadowSqrtStatic (corrida 20261011-125451-461566, 4096 pares) y CppShadowSqrtVehicle (corrida 20261011-125623-404352, 4096 pares), cero diferencias, cero registros nulos, estado, partidas y configuración restaurados. El conteo pasa a 927 apariciones de ensamblador y 277 declaraciones naked en 177 archivos.

Límites: el helper original (debug) dejaba EAX=CCCCCCCC y ECX=0 y el nuevo no; la equivalencia se apoya en que ninguno de los dos sitios de llamada lee esos registros ni los flags tras la llamada (ADD ESP,4 es la primera instrucción). NaN, negativos, caminos de excepción x87 y el contrato completo de registros no están cubiertos. ShadowExtendedProduct, ShadowDistanceFade y ShadowFadeInteger siguen en ensamblador y no quedan validados por estos bancos. Este lote no incrementa la cohorte de 499 dependencias ni afirma independencia del EXE.

Plan de lotes y clasificación por dificultad: [PLAN_POR_LOTES.md](PLAN_POR_LOTES.md) · [plan-clasificacion.csv](plan-clasificacion.csv).

## ShadowDistanceFade (11/10/2026)

ShadowDistanceFade pasó a source/game_sa/X87Math.cpp (antes ShadowMathCpp.cpp). Hace las mismas operaciones IEEE en el mismo orden de resultados que el ensamblador (1 - (sqrt(cuad) - d*0,75) * (4/d)), con almacenamiento FSTP64 y recarga; el compilador calcula 4/d antes de la raíz, así que el cuerpo no es idéntico byte a byte al del ensamblador. Las constantes 4,0 y 0,75 salen del .rdata del propio objeto. Los cuerpos de StoreStaticShadow y StoreShadowForVehicle son idénticos salvo el destino de la llamada (caller-spec-fade.json) y, tras la llamada, ninguno lee EAX, ECX, EDX ni banderas. Bancos CppShadowSqrtStatic (corrida 20261011-131717-551954) y CppShadowSqrtVehicle (corrida 20261011-131818-030789), 4096 pares cada uno, cero diferencias. La rama de atenuación aparece en las trazas de Vehicle (534 intensidades distintas); Static sólo registra el estado resumido. No cubre NaN, distancia cero ni caminos de excepción x87. El conteo pasa a 926 apariciones de ensamblador.

Corrección de alcance del lote 1: ShadowExtendedProduct también lo usa la función de polígonos (Shadows.cpp:975), y ShadowHeightDifference y ShadowHeightSum los usa CastShadowSectorList; los XY y de proyección los usan otras funciones. Esos helpers necesitan bancos derivados de ShadowXY355, ShadowGenerate355, ShadowXYZ355 y ShadowSectors355, no sólo los dos bancos de ShadowSqrt.

## Conversaciones (11/10/2026)

IsPlayerInPositionForConversation (0x43AC40) sustituye sus tres bloques x87 por funciones noinline del propio ConversationForPed.cpp, compilado completo con /O2, y ClearRecursively (0x43A710) deja de usar el envoltorio `naked` que preservaba EDX: se engancha directamente a la versión de ConversationNode.cpp, compilado completo con /O2, cuyo objeto (92 bytes, igual que el EXE) sólo escribe EAX, ECX y ESI. Bancos CppConversationForPed (corrida 20261011-143612-800417, 4702 pares: 2414 de posición y 2288 de Update) y CppConversationClear (corrida 20261011-141656-479908, 2048 pares; EDX preservado en todos y banderas 0x84), cero diferencias. Detalle, límites y hashes de los objetos en [CppConversationStudy](../CppConversationStudy/README.md). El conteo pasa a 922 apariciones de ensamblador y 276 declaraciones naked en 175 archivos.

## Consolidación sin archivos auxiliares duplicados (11/10/2026)

Siguiendo la auditoría ([AUDITORIA_ARCHIVOS_AUXILIARES.md](AUDITORIA_ARCHIVOS_AUXILIARES.md)) se eliminaron `TrigSimpleCpp`, `TrigCompound6Cpp`, `CloudsMathCpp`, `CloudsAccessCpp`, `CloudsWeatherCpp.h`, `ShadowSqrtCpp`, `ShadowMathCpp` e `IntegerMinimalCpp`:

- **`X87Math.cpp/.h`:** un único módulo compartido con los 14 helpers x87 (seno/coseno, combinaciones, comparaciones ordenadas, raíz y atenuación de sombras). Cada función conserva exactamente sus bytes y relocaciones (comparados con los objetos anteriores). Los tres helpers de Clouds (`CloudsSinX87`, `CloudsCosX87`, `CloudsSinMulX87`) eran duplicados byte a byte de los de `TrigSimple` y desaparecen.
- **`Clouds.cpp`:** usa `CWeather::` (mismas direcciones que las 10 variables duplicadas de `CloudsWeatherCpp.h`) y llama directamente a `FindPlayerPed`, `FindPlayerVehicle`, `FindPlayerCoors` y `g_fx` en lugar de los cuatro envoltorios; conserva sus opciones (`/arch:IA32`).
- **`D3DResourceSystem.cpp`:** contiene `SetUseD3DResourceBuffering` y se compila completo con `/O2 /Oi /Oy`; el cuerpo mide 10 bytes, idéntico al del EXE.
- **`Native_00424100`:** ahora vive en `CarCtrl.cpp` (3 bytes, idénticos al EXE), optimizada por `#pragma`; `NativeStubs.cpp` se eliminó. Ver [REVALIDACION_INTERIOR_CARCTRL.md](REVALIDACION_INTERIOR_CARCTRL.md).
- **Revalidación (primera tanda):** 16 bancos repetidos con los archivos nuevos, todos con cero diferencias y cero registros nulos, y estado, partidas y configuración restaurados: CppShadowSqrtStatic y Vehicle (4096 + 4096), CppCloudsMath3 (3072), CppCloudsRegression20 (8384), CppTrigBirdsRender (512), CppTrigBrightPlane (1536), CppTrigClimb (4608), CppTrigFollowPointRoute (512), CppTrigJetPack (1536), CppTrigSwim (2048), CppTrigCompound6Birds (2560), Markers (4608), Route (1024), Swim (1024), CppIntegerWorld4 (4608) y CppIntegerWorld10 (4096). Los registros de `approvals.json` apuntan a las corridas nuevas y conservan la anterior en `previous_run`.
- **`Interior_c`:** `InteriorCpp.cpp` e `InteriorShelfCpp.cpp` se fundieron en `Interior_c.cpp`; ver [REVALIDACION_INTERIOR_CARCTRL.md](REVALIDACION_INTERIOR_CARCTRL.md) para el detalle. Los dos fallos preexistentes (InteriorCommerce y `CStreaming::Shutdown`) eran huecos de la instrumentación de sus bancos y quedaron corregidos: ambos bancos aprueban.

## Animación y atractores (11/10/2026)

Se convirtieron cuatro funciones con ensamblador de cuatro clases ([detalle y objetos](../CppAnimAttractorStudy/README.md)): `CAnimBlendNode::CalcTheta`, `CAnimBlendSequence::RemoveQuaternionFlips`, `CPedShelterAttractor::GetDisplacement` y `CAttractorScanner::Clear`. Bancos completos repetidos sobre la fuente final: Anim16 (14080 pares), Render14 (6784), CTasks61c (28928) y DependenciesTaskPed5 (8192), todos aprobados con cero diferencias; las evidencias están en `CppAnimAttractorUnified`. Dos corridas intermedias fallaron y se corrigieron (ver el estudio). El conteo pasa a 918 apariciones de ensamblador y 274 declaraciones naked en 171 archivos.

## Audio: x87 en cinco archivos (11/10/2026)

Se eliminó el ensamblador de `AECollisionAudioEntity.cpp`, `AEWeaponAudioEntity.cpp`, `AEWMADecoder.cpp`, `AERadioTrackManager.cpp` y `AESoundManager.cpp` con cuatro helpers de `X87Math` ([detalle y objetos](../CppAudioX87Study/README.md)). La función `Log10Times20` estaba duplicada en los dos primeros archivos y quedó en un único `AudioLog10Times20Cpp`. Bancos completos repetidos: Audio17 (11088 pares), Audio20 (10176), Audio14 (10240) y RadioVolumes355 (1024), aprobados con cero diferencias; Audio11 fallaba sólo en `CAEPedlessSpeechAudioEntity::AddSayEvent` (4/1024), también con el árbol anterior; era un error de dirección (seis sitios leían la variable 0xB6B99C donde el EXE lee 0xB6B98C), corregido después y aprobado (13632 pares). Una primera corrida de RadioVolumes355 falló por el orden de evaluación de los argumentos y se corrigió. El conteo pasa a 913 apariciones de ensamblador y 274 declaraciones naked en 166 archivos.

## Audio: AudioLog10 y el error de dirección de voz (11/10/2026)

`CAEAudioUtility::AudioLog10` (el mismo `fldlg2 / fyl2x` que los helpers anteriores) pasó a `AudioLog10Cpp`, con CppModelInfoRegression (5264 pares, auditado), Small2 (32016) y AudioHw54 (9280) aprobados. Al investigar el fallo preexistente de `AddSayEvent` se encontró que la fuente leía `s_pVehicleAudioSettingsForRadio` (0xB6B99C) donde el EXE lee `s_pPlayerAttachedForRadio` (0xB6B98C) en el código de voz y en el escáner de policía; con seis sitios corregidos, Audio11 (13632 pares), PedSpeech58 (21760), AudioScannerPending (4096) y AudioWave2 (18496) aprueban. Además, `AddSayEvent` suma la distancia al cuadrado como el EXE, (z² + y²) + x² sin redondear. Detalle en [CppAudioX87Study](../CppAudioX87Study/README.md). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Nueve archivos con un solo bloque (12/10/2026)

Se eliminó el ensamblador de `BouncingPanel.cpp`, `Checkpoints.cpp`, `Coronas.cpp`, `CutsceneMgr.cpp`, `Decision.cpp`, `EntryExitManager.cpp`, `FileLoader.cpp`, `GameLogic.cpp` y `Gamma.cpp` con cinco helpers nuevos en `X87Math` (`PowScaleVector3Cpp`, `UnsignedMillisToSecondsCpp`, `HeadingMinusHalfPiCpp`, `GammaScaleCpp`, `ExeAcosCpp`), reutilizando los trigonométricos existentes en tres casos y C++ puro en `Decision.cpp` ([detalle y objetos](../CppNineFilesStudy/README.md)). Nueve bancos históricos completos repetidos, sin diferencias: World52 (18384 pares), Data8a (18176), World11 (16704), Cutscene35 (13184), DecisionPending (3072), World18 (7808) más FixBugs (10304), FileLoader47 (11968), GameLogic27 (13888) y Misc70 (12416). Una primera tanda se solapó con otra por error mío y quedó invalidada; las cifras son de la tanda única en serie. `ExeAcosCpp` duplica la fórmula de acos de `AnimCalcThetaCpp` y `PowScaleVector3Cpp` usa el `_CIpow` de la DLL, no el del EXE (límites en el detalle). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

Se eliminó el ensamblador de `AEAudioEnvironment.cpp`, `AESmoothFadeThread.cpp`, `AEGlobalWeaponAudioEntity.cpp`, `AEVorbisDecoder.cpp`, `AEWaveDecoder.cpp` y `AEAmbienceTrackManager.cpp` con cinco helpers nuevos en `X87Math` (`UnsignedTimesFloatCpp`, `SecondsToMsCpp`, `BytesToMsCpp`, `WaveCursorBytesCpp`, `SmoothFadeVolumeCpp`); bancos Small2, Sys10, AnimAudio6, Audio13, Core9, AudioCompletion y dependencias sin diferencias ([detalle y hashes](../CppAudioSixStudy/README.md)).

Se eliminó el ensamblador de `BulletTraces.cpp` con el helper nuevo `OneMinusUnsignedRatioCpp` en `X87Math` y `AudioLog10Times20Cpp`; bancos Fx43, Data7, Replay47 y WeaponFinal2 sin diferencias, con la interceptación de las entradas de la DLL añadida a Fx43 ([detalle y hashes](../CppBulletTracesStudy/README.md)).

Se eliminó el ensamblador de `Quaternion.cpp`, `Boat.cpp`, `Trailer.cpp` y `Event.cpp` con seis ayudantes nuevos en `X87Math` (`QuaternionFromAxisAngleCpp`, `Atan2FromCpp`, `PowScaleFloatCpp`, `TrailerSteerCpp`, `Log10Times10Cpp`, `SoundLevelIncrementCpp`); bancos DependenciesMathLeaf3, Boat23, Sys13 y Events52 sin diferencias, con la interceptación de las entradas de la DLL añadida a Events52 y Sys13 ([detalle y hashes](../CppFourFilesStudy/README.md)).

Se eliminó el ensamblador de `Fire.cpp`, `PlayerInfo.cpp`, `PointLights.cpp`, `Population.cpp` y `Replay.cpp` con seis ayudantes nuevos en `X87Math` (`FireBurnTimeCpp`, `IntFractionCpp`, `PlayerInfoAtanFromCpp`, `PointLightsInRangeXYCpp`, `PointLightsDistanceFadeCpp`, `ReplayLookAroundOffsetsCpp`); bancos Fire24, Fx43, Player28, DependenciesPointLights1 y 2, PopFinal4 y Replay47 sin diferencias ([detalle y hashes](../CppFiveFilesStudy/README.md)).

Se eliminó el ensamblador de `Ropes.cpp`, `ShotInfo.cpp`, `SimpleTransform.cpp`, `Sprite.cpp` y `EntitySeekPosCalculatorRadiusAngleOffset.cpp` con cinco ayudantes nuevos en `X87Math` (`RopesInterpolateCpp`, `ShotInfoDestroyTimeCpp`, `SimpleTransformInvertCpp`, `SpriteQuadCoeffsCpp`, `SpriteDimensionTermsCpp`); bancos Misc61a, Fx43, CEntA, World15 y Misc56 sin diferencias, con la interceptación de las entradas de la DLL de `CFormation::*` añadida a Misc61a ([detalle y hashes](../CppFiveFilesBStudy/README.md)).

Se eliminó el ensamblador de `common.cpp`, `WaterCreature_c.cpp`, `TaskComplexDestroyCarArmed.cpp` y `TimeCycle.cpp` (los cuatro archivos que quedaban del stash `lote-28-archivos-sin-terminar`) con cuatro ayudantes nuevos en `X87Math` (`CommonTransformPointCpp`, `CommonTransformVectorsCpp`, `DestroyCarDistanceCpp`, `TimeCycleDirnToSunCpp`, `TimeCycleVectorToSunCpp`); bancos DependenciesMathSprite2, World52, CTasks10a y Time20 sin diferencias; `Common::TransformVectors` con banco propio nuevo (CppTransformVectors1, 3072 pares) ([detalle y hashes](../CppFourFilesCStudy/README.md)).

Se eliminó el ensamblador de diecinueve archivos de nivel 1 (`3dMarkers`, `AEPedAudioEntity`, `AEMFDecoder`, `CarAI`, `ClothesBuilder`, `Cover`, `Door`, `Entity`, `EntityScanner`, `FireManager`, `ProcSurfaceInfo`, `rwplcore.h`, `TheScripts`, `AEAudioUtility`, `AEAudioHardware`, `IdleCam`, `TaskSimpleFightingControl`, `TaskSimplePlayerOnFoot` y `WeaponEffects`); 26 bancos sin diferencias ([detalle y hashes](../CppNineteenFilesStudy/README.md)).

Se eliminó el ensamblador de diecinueve archivos de nivel 2 (`Pickup`, `Object`, `QuadBike`, `GangWars`, `PedGeometryAnalyser`, `Timer`, `Weapon`, `Weather`, `PedGroupPlacer`, `AEWeatherAudioEntity`, `EventKnockOffBike`, `Hud`, `ProjectileInfo`, `CarEnterExit`, `PlayerPed`, `EntryExit`, `Train`, `Pickups` y `PostEffects`) y se convirtió una función de `RpAnimBlend`; los bancos aprobaron sin diferencias ([detalle y hashes](../CppTwentyFilesStudy/README.md)).

Se eliminó el ensamblador de otros cinco archivos de nivel 2 (`Font`, `TaskComplexWaitForPed`, `Birds`, `Placeable` y `TaskSimpleHoldEntity`); seis bancos sin diferencias ([detalle y hashes](../CppFiveLevel2Study/README.md)).

Se eliminó el ensamblador de cinco archivos más (`EventSource`, `General`, `PlayerRelationshipRecorder`, `BoneNode` y `TaskSimpleClimb`); los bancos aprobaron sin diferencias, `CTasks15` tras aislar en el banco las cabezas del mundo ([detalle y hashes](../CppFiveLevel3Study/README.md)).

Se eliminó el ensamblador de tres archivos más (`cHandlingDataMgr`, `PedGroups` y `TheZones`); cinco bancos sin diferencias ([detalle y hashes](../CppThreeLevel4Study/README.md)).

Se eliminó el ensamblador de `RpAnimBlend` y `Ped` (los seis puentes de velocidad y los emisores de partículas de pisada, con un banco nuevo propio); bancos sin diferencias ([detalle y hashes](../CppTwoLevel5Study/README.md)).

Se eliminó el ensamblador de los 25 archivos que quedaban (oleada 1: `AEVehicleAudioEntity`, `TaskSimpleJetPack`, `PathFind`, `TaskSimpleSwim`, `Bmx`, `PedIK`, `WaterLevel`, `MemoryMgr`, `Physical`, `Radar`, `Pad` y `Matrix`; oleada 2: `Automobile`, `TaskSimpleFight`, `Vehicle`, `DecisionMakerTypes`, `Garage`, `Formation`, `Bike`, `Camera`, `Plane`, `Interior_c`, `Shadows`, `Stats` y `Cam`), tres bloques sueltos (`CWorld::Process`, `CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed` y `CMenuManager::SaveStatsToFile`), `platform.cpp` (`rsCommandLine_Eax`, `rsPreInitCommandLine_Eax` y `psGetMemoryFunctions`) y `ExeViewWindow` de `app_game.cpp`; hay dos hooks nuevos (`CStats::UpdateSexAppealStat`, banco `DependenciesStatsSex16`, y `CPlayerPed::ControlButtonSprint`, banco `DependenciesTaskSprint16`) y un hook retirado (`CShadows::ShadowRenderTriangleCB`, 0x709CF0; `HookUnlockAudit` usa `retired.py`). `DependenciesMemory9` (exigía instrucciones idénticas al EXE) se retira y `DependenciesMemoryCpp16` es el banco de `CMemoryMgr::Malloc`, `Realloc`, `MallocAlign` y `FreeAlign`. Bancos de estrés nuevos para los ayudantes x87 (`CppAutomobileX87Stress1`, `CppBmxX87Stress1`, `CppCamX87Stress1`, `CppCameraX87Stress1`, `CppPlaneX87Stress1`) y `CppMatrixExact1`. La regresión final sigue en curso: el registro [approvals.json](approvals.json) sólo contiene corridas aprobadas y el estudio lista los bancos y las funciones que faltan ([detalle, bancos y objetos](../CppTwelveLevel6Study/README.md)). La excepción de `WindowedMode.cpp` se anota arriba y en [PROCESO.md](PROCESO.md).

## Diez archivos con un solo bloque (12/10/2026)

Se eliminó el ensamblador de `InterestingEvents.cpp`, `LoadingScreen.cpp`, `Maths.cpp`, `VehicleModelInfo.cpp`, `PedDamageResponseCalculator.cpp`, `CustomCarEnvMapPipeline.cpp`, `PlantMgr.cpp`, `Renderer.cpp`, `RunningScript.cpp` y `Sprite2d.cpp` ([detalle y objetos](../CppTenFilesStudy/README.md)). Siete helpers nuevos en `X87Math` (`Atan2MinusCpp` sustituye a `HeadingMinusHalfPiCpp` del lote anterior, sin duplicar), y `UnsignedMillisToSecondsCpp` y `TrigSimpleSinMulCpp` reutilizados. Bancos completos repetidos sin diferencias en las funciones convertidas: InterestingScan355 (2048 pares), Loading (9328), Small2 (32016), PedDamagePending (6144), EnvMap37 (10880), Renderer33 (18112), ScriptAngled355 (6144), Sprite2d33 (11136), DependenciesMathSprite2 (3072) y GameLogic27 (13888, tras la unificación). Tres bancos fallaban en funciones ajenas al lote (VehModelInfoFinal4, Plant19 y Game22), con los mismos recuentos con el árbol anterior; eran defectos de instrumentación de los bancos (puntos de entrada de la DLL sin interceptar y una comprobación de hash de `Game.cpp` que la propia sonda invalidaba) y quedaron corregidos sin retirar compuertas: VehModelInfoFinal4 16384 pares, Plant19 10688 y Game22 5824, cero diferencias. El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Nueve archivos con un solo bloque (12/10/2026)

Se eliminó el ensamblador de `BouncingPanel.cpp`, `Checkpoints.cpp`, `Coronas.cpp`, `CutsceneMgr.cpp`, `Decision.cpp`, `EntryExitManager.cpp`, `FileLoader.cpp`, `GameLogic.cpp` y `Gamma.cpp` con cinco helpers nuevos en `X87Math` (`PowScaleVector3Cpp`, `UnsignedMillisToSecondsCpp`, `HeadingMinusHalfPiCpp`, `GammaScaleCpp`, `ExeAcosCpp`), reutilizando los trigonométricos existentes en tres casos y C++ puro en `Decision.cpp` ([detalle y objetos](../CppNineFilesStudy/README.md)). Nueve bancos históricos completos repetidos, sin diferencias: World52 (18384 pares), Data8a (18176), World11 (16704), Cutscene35 (13184), DecisionPending (3072), World18 (7808) más FixBugs (10304), FileLoader47 (11968), GameLogic27 (13888) y Misc70 (12416). Una primera tanda se solapó con otra por error mío y quedó invalidada; las cifras son de la tanda única en serie. `ExeAcosCpp` duplica la fórmula de acos de `AnimCalcThetaCpp` y `PowScaleVector3Cpp` usa el `_CIpow` de la DLL, no el del EXE (límites en el detalle). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Diez archivos con un solo bloque (12/10/2026)

Se eliminó el ensamblador de `InterestingEvents.cpp`, `LoadingScreen.cpp`, `Maths.cpp`, `VehicleModelInfo.cpp`, `PedDamageResponseCalculator.cpp`, `CustomCarEnvMapPipeline.cpp`, `PlantMgr.cpp`, `Renderer.cpp`, `RunningScript.cpp` y `Sprite2d.cpp` ([detalle y objetos](../CppTenFilesStudy/README.md)). Siete helpers nuevos en `X87Math` (`Atan2MinusCpp` sustituye a `HeadingMinusHalfPiCpp` del lote anterior, sin duplicar), y `UnsignedMillisToSecondsCpp` y `TrigSimpleSinMulCpp` reutilizados. Bancos completos repetidos sin diferencias en las funciones convertidas: InterestingScan355 (2048 pares), Loading (9328), Small2 (32016), PedDamagePending (6144), EnvMap37 (10880), Renderer33 (18112), ScriptAngled355 (6144), Sprite2d33 (11136), DependenciesMathSprite2 (3072) y GameLogic27 (13888, tras la unificación). Dos bancos fallan en funciones ajenas al lote y **igual con el árbol anterior** (línea base propia): VehModelInfoFinal4 (`PreprocessHierarchy`, `ReduceMaterialsInVehicle`) y Plant19 (`CPlantMgr::Render`); en ambos las funciones convertidas son iguales. Game22 no compila su sonda (defecto del banco: lista `Game.cpp` entre las fuentes que la propia sonda modifica). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Trece archivos de tareas con un solo bloque (12/10/2026)

Se eliminó el ensamblador de trece archivos de `Tasks/` ([detalle y objetos](../CppThirteenFilesStudy/README.md)): cuatro helpers nuevos en `X87Math` (`TrigAtan2Cpp`, `DistanceSquaredZXYCpp`, `PowDecaySpeedCpp`, `IntRatioCpp`) y el resto con helpers existentes. Doce bancos históricos completos repetidos sin diferencias (entre 12608 y 32256 pares cada uno). CTasks6a y CTasks11 fallaban igual con el código anterior por no interceptar los puntos de entrada de la DLL (más una violación de acceso en `ComputeEventSourceType`, que ya es código real); corregidos sin retirar compuertas: 14784 y 12672 pares, cero diferencias. El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## pow del EXE frente al de la DLL (12/10/2026)

El límite abierto de `PowScaleVector3Cpp` y `PowDecaySpeedCpp` (usaban el `__CIpow` de la CRT de la DLL, no el `_CIpow` del EXE) se cerró con el banco nuevo [PowEquivalence](../PowEquivalence/README.md): el algoritmo (LIBM de Intel, rama SSE2) y todas sus tablas son idénticos en el EXE y en la DLL (76 de unos 80 bloques básicos iguales; los demás sólo se activan con entradas especiales). La comparación en el juego encontró diferencias de C99 (±1 elevado a NaN/±infinito y NaN elevado a ±0, donde el EXE devuelve NaN y la DLL 1) que `ExeCompatiblePowCpp` reproduce ahora. Valores idénticos bit a bit: rejilla de 28224 pares de especiales, los 2^32 exponentes float con la base de CarJumpOut, 536 870 912 casos por regiones (todas las combinaciones de signo y exponente binario de damp y paso) y 2^26 casos por función; STasks10 (19136 pares) y World52 (18384) repetidos. Las banderas de excepción coinciden salvo en el desbordamiento a ±infinito y en NaN elevado a ±0 (clases aceptadas y contadas por el banco; inobservables); la rama no SSE2 no puede elegirse porque la DLL entera requiere SSE2. Límite: el dominio completo de dos floats (2^64) se apoya en identidad de código y tablas más cobertura por regiones, no en una enumeración.

## Dieciséis archivos: doce convertidos, cuatro siguen naked (12/10/2026)

De los dieciséis archivos pedidos ([detalle, objetos y motivos](../CppSixteenFilesStudy/README.md)) se convirtieron doce: siete helpers nuevos en `X87Math` (acos y asin del CRT del EXE, sumas con `fild`, la fórmula de `CCurves` y el radio del frustum de Fx) y siete funciones en C++ puro (`Fx_c::CreateMatFromVec`, `CCarCtrl::FindGhostRoadHeight`, `CClothes::GetTextureDependency` directo, `CPedSaveStructure::Construct` con el residuo EDX:EAX del EXE, etc.). Cuatro se convirtieron, se probaron y se volvieron a su versión naked validada porque C++ no puede dejar los registros que el EXE deja y que sus llamadores o los bancos exigen: `FxFrustumInfo_c::IsCollision` (conserva ECX; rompió `FxManager_c::ShouldCreate`), `CGarages::FindGarageForObject` y `CEventScriptCommand::Destructor` (residuo exacto de ECX) y `CCollision::SphereVsEntity` (exige quitar las comprobaciones de ejecución de todo `Collision.cpp`, lo que cambió ColWorld59c). Los bancos BreakObj y Data23 fallaban igual con el código anterior por no interceptar las entradas de la DLL; corregidos sin retirar compuertas. El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Las cuatro naked restantes, ahora en C++ (12/10/2026)

`FxFrustumInfo_c::IsCollision`, `CGarages::FindGarageForObject`, `CEventScriptCommand::Destructor` y `CCollision::SphereVsEntity` pasaron a C++ **sin relajar ningún banco** ([detalle, técnica y objetos](../CppFourNakedStudy/README.md)): el residuo de ECX, EDX y EAX que deja el EXE se reproduce terminando cada función con una llamada a un ayudante trivial de `X87Math` (`ResidueRegsCpp` o `ResidueEcx::Get`) que recibe esos valores por la convención de llamada y no toca registros; `Destructor` recibe el EDX de entrada como parámetro `__fastcall`; las comprobaciones de ejecución se desactivan sólo en esas funciones. Bancos aprobados: DependenciesWorldLeaf3 (8192 pares), Fx25 (14336), DependenciesWorld4 (4608), DependenciesGarage2, Object48, DependenciesDecisionPed7 (8192; la primera corrida falló 1024/2048 sólo por EDX y se corrigió), GangWarsPending, GeoFinal1 y ColWorld59c (13568). La dependencia del código que genera el compilador se vigila al cargar la DLL (los ayudantes deben seguir siendo triviales), con `contrib/audit_register_residue.py` sobre los objetos y con los bancos; `Destructor` registra ahora a mano un marco de excepciones equivalente al del original y su desenrollado está probado (caso `seh_unwind` nuevo en DependenciesDecisionPed7, con control negativo). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Catorce archivos con un bloque y una naked: once convertidos (12/10/2026)

Se convirtieron a C++ once funciones ([detalle, técnica, objetos y y cómo se resolvieron las tres que dependían del EAX de entrada](../CppFourteenFilesStudy/README.md)), sin relajar ningún banco: `SetBaseModelInfoFlags`, `SetClumpModelInfoFlags`, `CPedGroupIntelligence::ComputeScriptCommandTasks`, `CPedPool::GetRef`, el ángulo de rotación de `IKChain_c`, `GetAnimHierarchyFromFrame`, `CTaskInteriorSitInChair::Constructor`, `IsVehicleInRange`, el rumbo de `CTaskSimpleBeHit`, el asin de `CTaskSimpleDead` y `CTaskSimpleGoToPointFine::SetTargetPos`. Nuevo helper `ExeAsinDoubleCpp`. Las tres que dependían del EAX de entrada (`IncrementAngerAtPlayer`, `SetMaxEntityDist2D`, `GetPlantDensity`) se convirtieron después con el ayudante `EntryEaxCpp` (ver abajo). El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.

## Las tres que dependían del EAX de entrada, ahora en C++ (12/10/2026)

`IncrementAngerAtPlayer` (0x421050), `SetMaxEntityDist2D` (0x4BC470) y `GetPlantDensity` (0x5DC210) se convirtieron **sin relajar ningún banco** ([técnica y detalle](../CppFourteenFilesStudy/README.md)): `EntryEaxCpp` (`X87Math`) es una `__fastcall` sin cuerpo que MSVC compila a `ret 0` y por tanto devuelve el EAX con el que se entró; la función que se reproduce la llama antes de ejecutar nada que escriba EAX. `SetMaxEntityDist2D` reproduce además la palabra de estado de `fnstsw ax` con `X87CompareStatusCpp`. Bancos aprobados: World52 (18384 pares), DependenciesTaskPed5 (8192), DependenciesWorld9 (4096), EvHA (17984) y EvHF (2048; el banco no interceptaba `CPedGroups::GetPedsGroup` de la DLL y fallaba igual antes del cambio). La técnica es comportamiento sin definir en el lenguaje: la vigilan una comprobación de bytes al cargar la DLL y el auditor de objetos, que además exige que nada escriba EAX antes de la lectura.

## FindPlayerHeading (12/10/2026)

`FindPlayerHeading` (0x56E450, `World_Helper.cpp`) pasó a C++ sin helpers nuevos: `atan2(-adelante.x, adelante.y)` con `TrigAtan2Cpp`, el rumbo compacto cuando la entidad no tiene matriz, y los mismos índices que el original (crudo para el vehículo remoto y el ped de reserva; resuelto con `PlayerInFocus` para el vehículo normal). Bancos aprobados: WldC (13120 pares) y RadarFinal3 (15040). Quedaba pendiente porque no estaba en ninguna de las listas de lotes anteriores. El conteo pasa a 849 apariciones de ensamblador y 246 declaraciones naked en 103 archivos.
