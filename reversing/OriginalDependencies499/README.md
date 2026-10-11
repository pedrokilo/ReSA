<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencias originales fuera del catálogo · cohorte de 499

Cohorte fija del 10 de octubre de 2026: destinos originales explícitos referenciados desde entradas catalogadas. `targets.csv` y `baseline.json` preservan el snapshot inicial y sus hashes. El inventario general puede cambiar al admitir funciones; esta cohorte siempre conserva sus 499 direcciones.

Tres subagentes revisaron los destinos por símbolos, contratos fuente y evidencia binaria. [progress.csv](progress.csv) conserva decisiones y admisiones; las 21 inciertas siguen explícitas en los informes de revisión. Clasificar una función no equivale a validarla.

| Clasificación | Total | Aprobadas en juego | Pendientes |
| --- | ---: | ---: | ---: |
| Funciones del juego | 235 | 104 | 131 |
| Bibliotecas/runtime/SDK | 243 | 0 | Seguimiento separado |
| Identidad o procedencia incierta | 21 | 0 | 21 |
| Total de la cohorte | 499 | 104 | 395 |

## Funciones admitidas

104 funciones, **241344 pares finales, cero diferencias y ningún resultado original/candidate nulo**, con rutas RH de producción apagado/activo comprobadas, restauración de estado, partidas y configuración. Se cuenta únicamente la corrida final de cada banco; las preparatorias no inflan el total. Catálogo principal actual: 8549/8549 funciones y 784/784 grupos con evidencia histórica; su 100% no incluye las dependencias pendientes de esta cohorte.

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

Cada banco documenta dirección/instrucción del EXE, reconstrucción o corrección, dominios de fixtures y límites. Se comparan retornos, memoria y registros pertinentes antes de admitir filas nuevas. La observación compuesta de DependenciesGarage2 no concedió aprobación al llamador FindGarageForObject. Su admisión posterior tiene banco propio en DependenciesWorld4, con 4.096 pares y prueba de la ruta activa del colaborador geométrico. Los servicios de bibliotecas aún empleados se declaran por banco.

`approved.json` enlaza evidencias; `verify_bank.py` comprueba cada par, resúmenes, rutas, restauración y huellas exactas; `register_bank.py` admite sólo direcciones clasificadas como juego dentro de esta cohorte. `dependency_review=null`: aprobación histórica de los fixtures, sin certificar independencia transitiva del EXE ni integración universal. Las huellas de corridas anteriores no se reescriben cuando otra función del mismo archivo cambia.

## Continuación

[work-queue.csv](work-queue.csv) muestra las entradas del juego todavía pendientes. Las bibliotecas siguen en el inventario completo; no se consideran reconstruidas por clasificarlas. Las inciertas necesitan resolver identidad/procedencia antes de diseñar pruebas. No registrar por nombre supuesto, cuerpo existente ni por pertenecer a un rango de direcciones.
