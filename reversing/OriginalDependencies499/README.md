<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencias originales fuera del catálogo · cohorte de 499

Cohorte fija del 10 de octubre de 2026: destinos originales explícitos referenciados desde entradas catalogadas. `targets.csv` y `baseline.json` preservan el snapshot inicial y sus hashes. El inventario general puede cambiar al admitir funciones; esta cohorte siempre conserva sus 499 direcciones.

Tres subagentes revisaron los destinos por símbolos, contratos fuente y evidencia binaria. [progress.csv](progress.csv) conserva decisiones y admisiones; las 21 que el 10/10 quedaron inciertas se resolvieron el 17/10/2026 (PLAN-001): fichas con evidencias en [review-uncertain-21.md](review-uncertain-21.md), 20 como juego y 1 como biblioteca, reconstruidas en C++ y validadas con los bancos `Dependencies21A` a `Dependencies21G`. Clasificar una función no equivale a validarla.

| Clasificación | Total | Aprobadas en juego | Pendientes |
| --- | ---: | ---: | ---: |
| Funciones del juego | 255 | 131 | 124 |
| Bibliotecas/runtime/SDK | 244 | 0 (1 reconstruida y validada, `CrtPrintf`, contada como biblioteca) | Seguimiento separado |
| Identidad o procedencia incierta | 0 | 0 | 0 |
| Total de la cohorte | 499 | 131 | 368 |

Evolución: el 10/10/2026 la tabla era 235 del juego (105 aprobadas, 130 pendientes), 243 bibliotecas y 21 inciertas; el 17/10/2026 las 21 inciertas se resolvieron (20 juego, 1 biblioteca) y se conciliaron 6 funciones del juego que otros bancos ya habían reconstruido y validado (ver «Conciliación con bancos de otras tandas»), de modo que hoy son 131 aprobadas y 124 pendientes. El total sigue en 499 y `targets.csv` y `baseline.json` no cambian. Los 368 de la columna de pendientes son las 124 del juego y las 244 bibliotecas, de las que la reconstruida (`CrtPrintf`, `runtime_status=library_validated` en `progress.csv`) conserva su seguimiento aparte y no cuenta como aprobada en juego.

## Funciones admitidas

La tabla siguiente es la admisión del 10/10/2026 (la que registra `approved.json`); las 20 funciones del juego admitidas el 17/10/2026 se listan en la sección «Resolución de los 21 inciertos». 105 funciones, **243392 pares finales, cero diferencias y ningún resultado original/candidate nulo**, con rutas RH de producción apagado/activo comprobadas, restauración de estado, partidas y configuración. Se cuenta únicamente la corrida final de cada banco; las preparatorias no inflan el total. Catálogo principal actual: 8550/8550 funciones y 784/784 grupos con evidencia histórica; su 100% no incluye las dependencias pendientes de esta cohorte.

| Función | Dirección EXE | Pares finales | Banco y alcance |
| --- | --- | ---: | --- |
| CPedGroups::GetPedsGroup | 0x005F7E80 | 1024 | [Evidencia](../DependenciesPedGroups1/README.md) |
| CStats::GetFatAndMuscleModifier | 0x00559AF0 | 4096 | [Evidencia](../DependenciesStats1/README.md) |
| CPointLights::AddLight | 0x007000E0 | 2560 | [Evidencia](../DependenciesPointLights1/README.md) |
| CPedGroups::IsGroupLeader | 0x005F7E40 | 4096 | [Evidencia](../DependenciesPedGroups2/README.md) |
| CPedGroups::GetGroupId | 0x005F7EE0 | 256 | [Evidencia](../DependenciesPedGroups2/README.md) |
| CPedGroups::IsInPlayersGroup | 0x005F7F10 | 512 | [Evidencia](../DependenciesPedGroups2/README.md) |
| CPedGroups::AreInSameGroup | 0x005F7F40 | 8192 | [Evidencia](../DependenciesPedGroups2/README.md) |
| CCarCtrl::ClearInterestingVehicleList | 0x00423F00 | 256 | [Evidencia](../DependenciesCarCtrl2/README.md) |
| CCarCtrl::RegisterVehicleOfInterest | 0x00423DE0 | 2048 | [Evidencia](../DependenciesCarCtrl2/README.md) |
| CCarCtrl::FindSequenceElement | 0x00421770 | 2048 | [Evidencia](../DependenciesCarCtrl2/README.md) |
| CModelInfo::Get2dEffectStore | 0x004C5A60 | 1024 | [Evidencia](../DependenciesLeaf2/README.md) |
| CMemoryMgr::InitScratchPad | 0x0072F480 | 1024 | [Evidencia](../DependenciesLeaf2/README.md) |
| CPointLights::Init | 0x006FFB40 | 512 | [Evidencia](../DependenciesPointLights2/README.md) |
| CPointLights::RemoveLightsAffectingObject | 0x006FFFE0 | 512 | [Evidencia](../DependenciesPointLights2/README.md) |
| CTaskSimpleFight::GetComboAnimGroupID | 0x004ABDA0 | 2048 | [Evidencia](../DependenciesFightLeaf2/README.md) |
| CTaskSimpleFight::IsComboSet | 0x004ABDC0 | 2048 | [Evidencia](../DependenciesFightLeaf2/README.md) |
| CTaskSimpleFight::IsHitComboSet | 0x004ABDF0 | 2048 | [Evidencia](../DependenciesFightLeaf2/README.md) |
| CSprite2d::OffsetTexCoordForBilinearFiltering | 0x00728150 | 1024 | [Evidencia](../DependenciesMathSprite2/README.md) |
| Common::TransformPoint | 0x0054ECE0 | 1024 | [Evidencia](../DependenciesMathSprite2/README.md) |
| Common::TransformPoints | 0x0054EEF0 | 1024 | [Evidencia](../DependenciesMathSprite2/README.md) |
| CGarage::IsPointInsideGarage-point | 0x00448740 | 1024 | [Evidencia](../DependenciesGarage2/README.md) |
| CGarage::IsPointInsideGarage-radius | 0x004487D0 | 1280 | [Evidencia](../DependenciesGarage2/README.md) |
| CGarages::FindSafeHouseIndexForGarageType | 0x004489F0 | 256 | [Evidencia](../DependenciesGarage2/README.md) |
| CTaskSimpleFight::ControlFight | 0x0061C5E0 | 2048 | [Evidencia](../DependenciesTaskLeaf3/README.md) |
| CPedSaveStructure::Construct | 0x005D43D0 | 2048 | [Evidencia](../DependenciesTaskLeaf3/README.md) |
| CQuaternion::Set-matrix | 0x0059C3E0 | 1024 | [Evidencia](../DependenciesMathLeaf3/README.md) |
| CQuaternion::Set-axis-angle | 0x0059C600 | 1024 | [Evidencia](../DependenciesMathLeaf3/README.md) |
| CPad::GetCurrentKeyPressed | 0x00541490 | 4096 | [Evidencia](../DependenciesWorldLeaf3/README.md) |
| FxFrustumInfo_c::IsCollision | 0x004AA030 | 4096 | [Evidencia](../DependenciesWorldLeaf3/README.md) |
| CTaskInteriorSitInChair::Constructor | 0x00675C30 | 2048 | [Evidencia](../DependenciesTaskCtor4/README.md) |
| CTaskComplexWaitForPed::Constructor | 0x00683340 | 2048 | [Evidencia](../DependenciesTaskCtor4/README.md) |
| CCarCtrl::SwitchVehicleToRealPhysics | 0x00423FC0 | 1024 | [Evidencia](../DependenciesCarCtrl4/README.md) |
| CCarCtrl::UpdateCarCount | 0x00424000 | 1024 | [Evidencia](../DependenciesCarCtrl4/README.md) |
| D3DResourceSystem::SetUseD3DResourceBuffering | 0x00730AC0 | 512 | [Evidencia](../DependenciesWorld4/README.md) |
| CGarages::FindGarageForObject | 0x0044A240 | 4096 | [Evidencia](../DependenciesWorld4/README.md) |
| CAttractorScanner::Clear | 0x005FFF90 | 2048 | [Evidencia](../DependenciesTaskPed5/README.md) |
| CPedIntelligence::IncrementAngerAtPlayer | 0x00421050 | 2048 | [Evidencia](../DependenciesTaskPed5/README.md) |
| CTaskSimpleFight::Constructor | 0x0061C470 | 2048 | [Evidencia](../DependenciesTaskPed5/README.md) |
| CEventSource::ComputeEventSourceType | 0x004ABAC0 | 2048 | [Evidencia](../DependenciesTaskPed5/README.md) |
| CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes | 0x00424130 | 1024 | [Evidencia](../DependenciesVehicleLeaf5/README.md) |
| CPathFind::AddRandomOffsetToNodePosition | 0x0044DA30 | 2048 | [Evidencia](../DependenciesVehicleLeaf5/README.md) |
| CPhysical::ApplyTurnForceComponents | 0x0065DA70 | 1024 | [Evidencia](../DependenciesVehicleLeaf5/README.md) |
| CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded | 0x0046AAE0 | 1024 | [Evidencia](../DependenciesVehicleLeaf5/README.md) |
| Fx_c::CreateMatFromVec | 0x0049E950 | 1024 | [Evidencia](../DependenciesWorldFxFont5/README.md) |
| CFont::GetScriptLetterSize | 0x00719670 | 6144 | [Evidencia](../DependenciesWorldFxFont5/README.md) |
| CCamera::IsSphereVisible-matrix | 0x00420C40 | 4096 | [Evidencia](../DependenciesWorldFxFont5/README.md) |
| CRadar::ClipRadarPoly | 0x00585040 | 4096 | [Evidencia](../DependenciesWorldFxFont5/README.md) |
| CDecisionMakerTypes::RemoveDecisionMaker | 0x006043A0 | 2048 | [Evidencia](../DependenciesDecisionRelationship6/README.md) |
| CDecisionMakerTypes::AddDecisionMaker | 0x00607050 | 2048 | [Evidencia](../DependenciesDecisionRelationship6/README.md) |
| CDecisionMakerTypes::LoadEventIndices | 0x00600840 | 128 | [Evidencia](../DependenciesDecisionRelationship6/README.md) |
| CPlayerRelationshipRecorder::RecordRelationshipWithPlayer | 0x0061A1D0 | 2048 | [Evidencia](../DependenciesDecisionRelationship6/README.md) |
| CDecisionMakerTypes::MakeDecision-Ped | 0x00606E70 | 2048 | [Evidencia](../DependenciesDecisionPed7/README.md) |
| CDecisionMakerTypes::MakeDecision-Group | 0x00606F80 | 2048 | [Evidencia](../DependenciesDecisionPed7/README.md) |
| CPedPool::GetRef | 0x004442D0 | 2048 | [Evidencia](../DependenciesDecisionPed7/README.md) |
| CEventScriptCommand::Destructor | 0x004B0A50 | 2048 | [Evidencia](../DependenciesDecisionPed7/README.md) |
| CStats::PopulateFavoriteRadioStationList | 0x00558EC0 | 2048 | [Evidencia](../DependenciesStatsRadio7/README.md) |
| CStats::FindMostFavoriteRadioStation | 0x00558FA0 | 2048 | [Evidencia](../DependenciesStatsRadio7/README.md) |
| CCollision::CalculateTrianglePlanes-colModel | 0x00418580 | 1024 | [Evidencia](../DependenciesCollisionAdapters1/README.md) |
| CCollision::RemoveTrianglePlanes-colModel | 0x004185A0 | 1024 | [Evidencia](../DependenciesCollisionAdapters1/README.md) |
| CCam::CameraPathInterpolateVector | 0x005B2090 | 4096 | [Evidencia](../DependenciesWorldWaterPath6/README.md) |
| CCam::CameraPathInterpolateScalar | 0x005B2330 | 4096 | [Evidencia](../DependenciesWorldWaterPath6/README.md) |
| CWaterLevel::GetWaterLevelNoWavesForQuad | 0x006E5BB0 | 8192 | [Evidencia](../DependenciesWorldWaterPath6/README.md) |
| CWaterLevel::GetWaterLevelNoWavesForTriangle | 0x006E5E90 | 8192 | [Evidencia](../DependenciesWorldWaterPath6/README.md) |
| CPad::GetCameraCycleUp | 0x005404A0 | 4096 | [Evidencia](../DependenciesPadCamera7/README.md) |
| CPad::GetMouseCameraStickHorizontal | 0x00540E80 | 8192 | [Evidencia](../DependenciesPadCamera7/README.md) |
| CPad::GetMouseCameraStickVertical | 0x00540F80 | 8192 | [Evidencia](../DependenciesPadCamera7/README.md) |
| CFormation::FindNearestFreeDestination | 0x0069B1B0 | 8192 | [Evidencia](../DependenciesFormationMemory8/README.md) |
| Rs::psGetMemoryFunctions | 0x00745510 | 64 | [Evidencia](../DependenciesFormationMemory8/README.md) |
| CStats::FindCriminalRatingString | 0x0055A210 | 2048 | [Evidencia](../DependenciesStatsUi8/README.md) |
| CStats::UpdateStatsOnRespawn | 0x0055CFC0 | 2048 | [Evidencia](../DependenciesStatsUi8/README.md) |
| CStats::UpdateStatsAddToHealth | 0x0055D030 | 2048 | [Evidencia](../DependenciesStatsUi8/README.md) |
| CStats::CheckForStatsMessage | 0x00559760 | 2048 | [Evidencia](../DependenciesStatsUi8/README.md) |
| CWaterLevel::CalculateWavesOnlyForCoordinate2Native | 0x006E7210 | 2048 | [Evidencia](../DependenciesWorld9/README.md) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D | 0x004BC470 | 2048 | [Evidencia](../DependenciesWorld9/README.md) |
| CStats::UpdateStatsWhenDriving | 0x0055CAC0 | 2048 | [Evidencia](../DependenciesVehicleStats9/README.md) |
| CStats::UpdateStatsWhenFlying | 0x0055CC00 | 2048 | [Evidencia](../DependenciesVehicleStats9/README.md) |
| CMemoryMgr::Malloc-SizeHint | 0x0072F420 | 2048 | [Evidencia](../DependenciesMemory9/README.md) |
| CMemoryMgr::Realloc-MemorySizeHint | 0x0072F440 | 2048 | [Evidencia](../DependenciesMemory9/README.md) |
| CMemoryMgr::MallocAlign-SizeAlignHint | 0x0072F4C0 | 2048 | [Evidencia](../DependenciesMemory9/README.md) |
| CMemoryMgr::FreeAlign | 0x0072F4F0 | 2048 | [Evidencia](../DependenciesMemory9/README.md) |
| CStats::UpdateFatAndMuscleStats | 0x0055C470 | 2048 | [Evidencia](../DependenciesVehicleStats10/README.md) |
| CStats::UpdateStatsWhenWeaponHit | 0x0055CEB0 | 2048 | [Evidencia](../DependenciesVehicleStats10/README.md) |
| CStats::UpdateStatsWhenSwimming | 0x0055C990 | 2048 | [Evidencia](../DependenciesVehicleStats10/README.md) |
| CPedGroups::AddGroup | 0x005FB800 | 2048 | [Evidencia](../DependenciesTask10/README.md) |
| CPedGroups::Init | 0x005FB8C0 | 2048 | [Evidencia](../DependenciesTask10/README.md) |
| CPedGroups::CleanUpForShutDown | 0x005FB930 | 2048 | [Evidencia](../DependenciesTask10/README.md) |
| CDecisionMakerTypes::GetInstance | 0x004684F0 | 2048 | [Evidencia](../DependenciesTask11/README.md) |
| CTaskComplexSunbathe::CanSunbathe | 0x00632140 | 2048 | [Evidencia](../DependenciesTask11/README.md) |
| CStats::UpdateStatsWhenCycling | 0x0055C780 | 2048 | [Evidencia](../DependenciesVehicleStats11/README.md) |
| CVehicle::IsValidModForVehicle | 0x0049B010 | 2048 | [Evidencia](../DependenciesVehicleStats11/README.md) |
| CCam::GetBoatHeightForFirstPerson | 0x00509CA0 | 2048 | [Evidencia](../DependenciesWorld10/README.md) |
| CCarCtrl::Native_00424100 | 0x00424100 | 2048 | [Evidencia](../DependenciesWorld10/README.md) |
| CVehicle::operator new-anon | 0x006E2D50 | 2048 | [Evidencia](../DependenciesVehicle12/README.md) |
| CTaskSimpleFight::Destructor | 0x0061C530 | 2048 | [Evidencia](../DependenciesTask12/README.md) |
| RwHelper::RwTexDictionaryFindHashNamedTexture | 0x00734E50 | 2048 | [Evidencia](../DependenciesWorld11/README.md) |
| CPathFind::RemoveInteriorLinks | 0x0044DF60 | 2048 | [Evidencia](../DependenciesWorld13/README.md) |
| RwHelper::GetAnimHierarchyFromFrame | 0x00734AB0 | 2048 | [Evidencia](../DependenciesWorld12/README.md) |
| CPathFind::CalcRoadDensity | 0x0044EFC0 | 2048 | [Evidencia](../DependenciesPathDensity13/README.md) |
| CTheZones::SetCurrentZoneAsUnlocked | 0x00572800 | 2048 | [Evidencia](../DependenciesZones13/README.md) |
| CPathFind::FindNodeOrientationForCarPlacement | 0x00450320 | 2048 | [Evidencia](../DependenciesPathOrientation14/README.md) |
| CCarCtrl::FindGhostRoadHeight | 0x00422370 | 2048 | [Evidencia](../DependenciesWorld14/README.md) |
| CPedGroupIntelligence::ComputeScriptCommandTasks | 0x005F7800 | 2048 | [Evidencia](../DependenciesGroupTasks14/README.md) |
| CStats::LoadStatUpdateConditions | 0x00559860 | 2048 | [Evidencia](../DependenciesStatsFile15/README.md) |
| CGarage::IsEntityTouching3D | 0x00448EE0 | 2048 | [Evidencia](../DependenciesWorld15/README.md) |
| CGarage::EntityHasASphereWayOutsideGarage | 0x00449050 | 2048 | [Evidencia](../DependenciesGarageSphere15/README.md) |

Cada banco documenta dirección/instrucción del EXE, reconstrucción o corrección, dominios de fixtures y límites. Se comparan retornos, memoria y registros pertinentes antes de admitir filas nuevas. La observación compuesta de DependenciesGarage2 no concedió aprobación al llamador FindGarageForObject. Su admisión posterior tiene banco propio en DependenciesWorld4, con 4.096 pares y prueba de la ruta activa del colaborador geométrico. Los servicios de bibliotecas aún empleados se declaran por banco.

`approved.json` enlaza evidencias; `verify_bank.py` comprueba cada par, resúmenes, rutas, restauración y huellas exactas; `register_bank.py` admite sólo direcciones clasificadas como juego dentro de esta cohorte. `dependency_review=null`: aprobación histórica de los fixtures, sin certificar independencia transitiva del EXE ni integración universal. Las huellas de corridas anteriores no se reescriben cuando otra función del mismo archivo cambia.

## Resolución de los 21 inciertos (17/10/2026)

20 funciones del juego aprobadas con **43008 pares finales, cero diferencias y ningún resultado nulo**, y una biblioteca (CRT `printf`) con 4096 pares, sumando 47104 pares de los bancos `Dependencies21A` a `Dependencies21G`. Sus corridas finales están en `reversing/CppTwelveLevel6Final/runs/` y se registran en [approved-level6.json](approved-level6.json) (mismo formato que `approved.json`; bancos del esquema de nivel 6, sin `approval-audit.json`, por lo que `audit-cohort-current.py` y `refresh_progress_docs.py` siguen leyendo sólo `approved.json`). En `progress.csv` las 20 figuran con `category=game`, `implementation=implemented` y `runtime_status=passed`.

| Función | Dirección EXE | Pares finales | Banco |
| --- | --- | ---: | --- |
| CCamera::ProcessObbeCinemaCameraCar | 0x005267C0 | 2048 | [Evidencia](../Dependencies21B/README.md) |
| CCamera::ProcessObbeCinemaCameraTrain | 0x00526950 | 2048 | [Evidencia](../Dependencies21B/README.md) |
| CCamera::ProcessObbeCinemaCameraHeli | 0x00526AE0 | 2048 | [Evidencia](../Dependencies21B/README.md) |
| CCamera::ProcessObbeCinemaCameraPlane | 0x00526C80 | 2048 | [Evidencia](../Dependencies21B/README.md) |
| CCamera::ProcessObbeCinemaCameraBoat | 0x00526E20 | 2048 | [Evidencia](../Dependencies21B/README.md) |
| CTaskSimpleFight::FightSetUpCol | 0x0061D5F0 | 1536 | [Evidencia](../Dependencies21C/README.md) |
| CTaskSimpleFight::FightHitCar | 0x0061D0B0 | 1536 | [Evidencia](../Dependencies21C/README.md) |
| CTaskSimpleFight::FightStrike | 0x006240B0 | 1536 | [Evidencia](../Dependencies21C/README.md) |
| CTaskSimpleGangDriveBy::Constructor | 0x006217D0 | 1024 | [Evidencia](../Dependencies21D/README.md) |
| CTaskSimpleGangDriveBy::PlayerTarget | 0x00621960 | 2048 | [Evidencia](../Dependencies21D/README.md) |
| CTaskSimpleGangDriveBy::AITarget | 0x00627600 | 4096 | [Evidencia](../Dependencies21D/README.md) |
| CTaskSimpleGangDriveBy::AimGun | 0x00628350 | 4096 | [Evidencia](../Dependencies21D/README.md) |
| CPopulation::RandomIntInLimitRange | 0x00610DB0 | 1536 | [Evidencia](../Dependencies21E/README.md) |
| CZoneDef::GetCenter | 0x0072D6D0 | 1536 | [Evidencia](../Dependencies21E/README.md) |
| CTaskComplexEnterCar::GetCameraStickModifier | 0x0063A380 | 3072 | [Evidencia](../Dependencies21E/README.md) |
| D3DResourceSystem::TidyUpD3DTextures | 0x007305E0 | 2048 | [Evidencia](../Dependencies21F/README.md) |
| D3DResourceSystem::TidyUpD3DIndexBuffers | 0x00730740 | 2048 | [Evidencia](../Dependencies21F/README.md) |
| RwHelper::GetFirstTexture | 0x00734940 | 1024 | [Evidencia](../Dependencies21G/README.md) |
| RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial | 0x00734DE0 | 1536 | [Evidencia](../Dependencies21G/README.md) |
| CQuaternion::Slerp (halftheta) | 0x0059C300 | 4096 | [Evidencia](../Dependencies21G/README.md) |
| CRT::CrtPrintf (biblioteca, seguimiento separado) | 0x00821982 | 4096 | [Evidencia](../Dependencies21A/README.md) |

Los grupos D3D (confianza media-alta, procedencia histórica no observable en el binario) y los colaboradores sustituidos por falsos en los bancos se detallan en [review-uncertain-21.md](review-uncertain-21.md). `dependency_review=null`: no certifica independencia transitiva del EXE.

<!-- COHORT-RECONCILE:BEGIN -->
## Conciliación con bancos de otras tandas (17/10/2026)

6 funciones del juego de la cohorte, que el 10/10 figuraban como envoltorios pendientes, fueron reconstruidas y validadas después por bancos de otras tandas (regresión final de nivel 6 y bancos 22/23) y constan con `validated=1` en `docs/hooks.csv` y en `reversing/validation-registry.json`. `reconcile_validated.py` las admite en [approved-reconciled.json](approved-reconciled.json) sólo tras reverificar la evidencia: identidad única en catálogo y registro, SHA256 del resumen, `passed`/`all_equal`/`differences=0`/`state_restored`, `covered_functions` propio, resumen de la corrida registrada, log JSONL leído en streaming contra su SHA y recuento de pares de **la propia función** (los fixtures de otras funciones del mismo banco no dan crédito). Las direcciones que sólo coinciden por nombre no cuentan (`InteriorGroup_c::SetupHousePeds` 0x0046CED0 es otra dirección que la validada 0x005965E0 y sigue pendiente). La biblioteca `CrtPrintf` ya estaba como `library_validated`; se reverifica con la misma regla y no cambia de categoría.

| Función | Dirección EXE | Pares propios | Pares del banco | Corrida |
| --- | --- | ---: | ---: | --- |
| CPlayerPed::ControlButtonSprint | 0x0060A610 | 3848 | 3848 | `DependenciesTaskSprint16-20261016-204102-478023` |
| CVehicle::FlyingControl | 0x006D85F0 | 16896 | 16896 | `Dependencies23B-20261017-162634-919625` |
| CPointLights::GenerateLightsAffectingObject | 0x006FFBB0 | 2048 | 7680 | `Dependencies22A-20261017-121118-063873` |
| CStats::UpdateSexAppealStat | 0x0055BF20 | 4096 | 4096 | `DependenciesStatsSex16-20261016-224645-599852` |
| CVehicle::DoVehicleLights | 0x006E1A60 | 8192 | 8192 | `Dependencies23A-20261017-134505-935121` |
| CPointLights::RenderFogEffect | 0x007002D0 | 2048 | 7680 | `Dependencies22A-20261017-121118-063873` |
| CRT::CrtPrintf (biblioteca) | 0x00821982 | 4096 | 4096 | `Dependencies21A-20261017-014358-631804` |

Los pares propios son los de la función en su banco; no se suman a los 243392 + 43008 de las secciones anteriores. `audit-cohort-current.py` sigue auditando sólo `approved.json`; las seis filas se auditaron con `reconcile_validated.py`.
<!-- COHORT-RECONCILE:END -->

## Continuación

[work-queue.csv](work-queue.csv) muestra las entradas del juego todavía pendientes (124). Las bibliotecas siguen en el inventario completo; no se consideran reconstruidas por clasificarlas. Ya no quedan inciertas (resueltas el 17/10/2026). No registrar por nombre supuesto, cuerpo existente ni por pertenecer a un rango de direcciones.
