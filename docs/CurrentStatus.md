# Estado comprobado de ReSA · 8 de octubre de 2026

**Corte documental: 8 de octubre de 2026, después de vincular Context9, GeometryPlanes, seis mutaciones de CPad y PlaceGroup V2, hora de Ciudad de México.** Los contadores corresponden al catálogo sincronizado con 103 aprobaciones nuevas. Las sondas posteriores pendientes no se suman.

El catálogo contiene **771 clases, namespaces o grupos / 8446 entradas**, todas marcadas como reconstruidas. Hay **6839 entradas con evidencia histórica vinculada** y **1607 pendientes de validación**, distribuidas en **63 grupos**. El alcance solicitado inicial fue de 77 grupos y 1710 funciones; 103 identidades nuevas quedaron aprobadas para sus fixtures. Reconstrucción no significa validación ni independencia del ejecutable.

**Vigencia actual: 0 funciones acreditadas.** Las validaciones nuevas tienen `dependency_review=null`: son evidencia histórica aprobada, con límites explícitos. Las 52 de Interior_c mantienen sus resultados históricos; su huella de dependencias no coincide con el árbol actual. Esta documentación no cambia huellas de evidencia ni acredita dependencias.

El **snapshot de arranque histórico del 8 de octubre de 2026** seleccionó **694 grupos completos / 6136 funciones** y desactivó 2313 hooks del inventario de ejecución (incluye 3 extras NOTSA), sin discrepancias entre selección y estados. El ASI de ese arranque fue `f33a7f5ca0ebdb08f4f77d91c2f8142c4a2776761191c3e683eb352fc3438cae`. Es un snapshot fechado; no identifica la última instalación ni certifica comportamiento conjunto. Las sondas posteriores restauran la instalación que encontraron. La whitelist histórica de ese arranque permanece fija; las nuevas aprobaciones no se añadieron automáticamente. El argumento `--historical-hooks` es explícito y no implica validación vigente. [Registro de arranque y correspondencias](../reversing/RequestedValidation20261008/historical-launch/README.md).

## Ciento tres aprobaciones nuevas, con alcance limitado

| Función | Pares aprobados | Evidencia conservada | Alcance |
| --- | ---: | --- | --- |
| CDecision::From | 512 | [Resumen](../reversing/RequestedLeafValidation/LiveBatch/validation-summary.json) | Objetos separados y autoasignación; excluye solapamiento parcial. |
| CTaskSimpleGangDriveBy::GetTaskType | 768 | [Resumen](../reversing/RequestedLeafValidation/LiveBatch/validation-summary.json) | Entrada directa, tabla GTA y despacho virtual; 256 combinaciones de campos. |
| CPedDamageResponseCalculator::Constructor | 1024 | [Resumen](../reversing/RequestedMenuBatch/runs/constructor/20261008-111101-552593/enriched-summary.json) | Campos/padding y retorno this; no acredita cálculo de daño ni actores reales. |
| CInterestingEvents::InvalidateNonVisibleEvents | 384 | [Resumen](../reversing/RequestedMenuBatch/runs/visibility/20261008-112224-200531/enriched-summary.json) | Ocho eventos, cámaras y visibilidad; LOS y limpieza de referencias aislados con trazas. |
| CFormation::FindCoverPointsBehindBox | 4096 | [Resumen](../reversing/RequestedMenuBatch/runs/formation-expanded/20261008-113522-938438/enriched-summary.json) | Cajas no degeneradas, transformaciones rígidas y límites; comparación completa de salida. |
| CTaskComplexWanderStandard::GetWanderType | 1280 | [Resumen](../reversing/RequestedPreflightWander/runs/20261008-115205-491044/enriched-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskComplexGangJoinRespond::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGoToAttractor::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSmartFleePoint::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGangJoinRespond::Destructor | 192 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGoToAttractor::Destructor | 160 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSmartFleePoint::Destructor | 160 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexFollowLeaderInFormation::Destructor | 192 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexKillPedFromBoat::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexKillPedFromBoat::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexHitPedWithCar::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexHitPedWithCar::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSunbathe::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSunbathe::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexWanderStandard::Constructor | 2048 | [Resumen](../reversing/RequestedWanderRemaining/validation-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskComplexWanderStandard::ScanForStuff | 4096 | [Resumen](../reversing/RequestedWanderRemaining/validation-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskSimpleFightingControl::Constructor | 256 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskComplexProstituteSolicit::Constructor | 128 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskSimpleFightingControl::Destructor | 32 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskComplexProstituteSolicit::Destructor | 256 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached | 5120 | [Resumen](../reversing/RequestedGeometryCentre/validation-summary.json) | Centro de caja y transformaciones finitas en almacenamiento privado; no acredita actores reales. |
| CPad::CarGunJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::CollectPickupJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::ConversationNoJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::ConversationYesJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::CycleWeaponLeftJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::CycleWeaponRightJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::DuckJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::ExitVehicleJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAccelerate | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAccelerateJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAnaloguePadDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAnaloguePadLeft | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAnaloguePadRight | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetAnaloguePadUp | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetBrake | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetCarGunFired | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetCarGunLeftRight | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetCarGunUpDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetDuck | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetEnterTargeting | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetExitVehicle | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetForceCameraBehindPlayer | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetGroupControlBack | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetGroupControlForward | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetHandBrake | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetHorn | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetHydraulicJump | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetJump | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetLookBehindForCar | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetLookBehindForPed | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetLookLeft | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetMeleeAttack | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetPedWalkLeftRight- | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetPedWalkUpDown- | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetSprint | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetSteeringLeftRight | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetSteeringUpDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetTarget | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetTouchedTimeDelta | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GroupControlBackJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GroupControlForwardJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::HornJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::JumpJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::LastStationJustUp | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::MeleeAttackJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::NextStationJustUp | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::ShiftTargetLeftJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::ShiftTargetRightJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::SniperZoomIn | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::SniperZoomOut | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::SprintJustDown | 3876 | [Resumen](../reversing/RequestedPadSelectors/validation-summary-51.json) | Selector aislado: estado privado, ABI declarado y globales; no acredita dispositivos ni input real. |
| CPad::GetLookRight | 512 | [Resumen](../reversing/RequestedPadShoulderInteraction/validation-summary.json) | Producto de 512 combinaciones de hombros, modos, controles y presiones; sin input real. |
| CInterestingEvents::InvalidateEvent | 720 | [Resumen](../reversing/RequestedRemainingLeafPair/validation-summary-event720.json) | Ocho slots exactos/null/externo; bytes y cleanup trazado. Subset aprobado del banco mixto fallido; sin cadenas reales de referencias. |
| CPedDamageResponseCalculator::ComputeWillKillPed | 18432 | [Resumen](../reversing/RequestedRemainingLeafPairV2/validation-summary-damage18432.json) | Bytes completos y trazas contra EXE/modelo independiente; cheats 61/77/67 independientes, límites float finitos y colaboradores aislados. Sin actores reales ni vigencia de dependencias. |
| CTaskSimplePlayerOnFoot::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x006857C0, slots y cinco rutas. No acredita constructor/destructor ni actores. |
| CTaskSimpleFightingControl::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x0061DC90, slots y cinco rutas. No acredita constructor/destructor ni actores. |
| CTaskComplexPlayHandSignalAnim::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x0061B2E0, slots y cinco rutas. No acredita constructor/destructor ni actores. |
| CTaskComplexKillPedOnFootArmed::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x00621240, slots y cinco rutas. No acredita constructor/destructor ni actores. |
| CTaskComplexKillCriminal::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x0068BF20, slots y cinco rutas. No acredita constructor/destructor ni actores. |
| CTaskComplexProstituteSolicit::GetTaskType | 1280 | [Resumen](../reversing/RequestedTaskTypeSix/validation-summary.json) | Getter puro; entrada nativa 0x00661AE0, slots y cinco rutas. No acredita constructor/destructor ni actores. |

Los resúmenes incluyen el EXE y DLL ejecutados, fuentes/instrumentos congelados, comparaciones y comprobaciones de restauración. Los errores previos permanecen conservados. La aprobación de una función no acredita el resto de su clase; los bancos preparados o fallidos no suman validaciones. [Inventario por función y vigencia](ValidationInventory.md).
| CPad::AimWeaponLeftRight | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::AimWeaponUpDown | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::GetDisplayVitalStats | 15480 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::GetPedWalkLeftRight-ped | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::GetPedWalkUpDown-ped | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::GetWeapon | 15480 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::LookAroundLeftRight | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::LookAroundUpDown | 6048 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPad::WeaponJustDown | 15480 | [Resumen](../reversing/RequestedPadContext9/validation-summary-context9.json) | Selectores sobre imágenes ABI privadas y task tags; retornos y predicados compilados de bytes completos. Sin actores, entrada OS ni trazas call-for-call de helpers. |
| CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached | 2048 | [Resumen](../reversing/RequestedGeometryPlanesV2/validation-summary.json) | 1024 figuras finitas por buffers separados o alias completo; bytes, guards, PC53 dot-model y trazas. Normalise nativo compartido como dependencia; no actores ni aliases desplazados. |
| CPad::StopShaking | 2048 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPad::SetTouched | 2048 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPad::ClearMouseHistory | 256 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPad::ClearKeyBoardHistory | 256 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPad::Clear | 1024 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPad::Initialise | 256 | [Resumen](../reversing/RequestedPadMutationNext/validation-summary-six.json) | Pad privado 308 bytes y 2624 globales/guards, con máscaras de escritura originales; sin entrada OS ni arranque del juego. |
| CPedGroupPlacer::PlaceGroup | 1536 | [Resumen](../reversing/RequestedGroupPlacerCorrectedV2/validation-summary.json) | Seis enums válidos, 1536 fixtures y delegates aislados con args/trazas; AL crudo exacto y arena intacta. Sin colocación real, pools ni actores. |

## CPad: corpus y reparación de la interacción de hombros

Se vincularon **51 selectores × 3876 fixtures = 197676 pares**, con payload privado, globales, ABI declarado y restauración de entradas/tablas/flags. La comparación de bool usa el valor declarado en AL; los bits superiores indefinidos de EAX no se interpretan como diferencias. El corpus cubre modos 0..3, presiones y transiciones aisladas, sin acreditar combinaciones exhaustivas ni integración con dispositivos. [Resumen de 51 identidades](../reversing/RequestedPadSelectors/validation-summary-51.json).

`GetLookRight` se acreditó aparte con **512 pares**, incluyendo las 16 combinaciones old/new de ambos hombros, cuatro modos, controles activos/inactivos y presiones 0/1/128/255. Una tanda previa reprodujo **12 diferencias** en la combinación de liberación izquierda con hombro derecho mantenido; se conservó y se corrigió antes del replay aprobado. [Resumen final](../reversing/RequestedPadShoulderInteraction/validation-summary.json). Ambos cierres naturales fueron 0 y las comprobaciones de restauración se conservaron.

El primer corpus de CPad tuvo **590 diferencias** y un `passed=true` incorrecto porque el runner sobrescribía el rechazo del analizador. Ese veredicto quedó invalidado mediante auditoría adicional; los resúmenes originales permanecen intactos. El runner conserva ahora `passed=false`, conteos y salida natural. [Auditoría del falso veredicto](../reversing/RequestedPadSelectors/VERDICT_AUDIT.md).

Las 51 aprobaciones se compilaban contra la versión de `Pad.cpp` con seis reparaciones; `GetLookRight` se compiló después de la séptima. La huella completa de `Pad.cpp` cambió: las 51 mantienen evidencia histórica, aunque su archivo ya no coincide con la fuente de su compilación. Cada evidencia conserva su manifest real; no se actualizan hashes históricos ni se acredita vigencia.

`CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached` añadió **5120 pares** de geometría privada y límites explícitos. [Resumen aprobado](../reversing/RequestedGeometryCentre/validation-summary.json).

## Documentos congelados y evidencia preparada

El estado central y los derivados se actualizan con el catálogo. Los documentos que forman parte de manifests o snapshots conservan sus bytes y el alcance de su fecha; una descripción de banco preparado no se convierte en aprobación por actualizar documentación. [Lista de 15 excepciones con hashes y referencias](../reversing/RequestedValidation20261008/documentation-frozen-exceptions.json). Los bancos pendientes y replays aún no vinculados no se suman a este corte. El objetivo inicial sigue siendo 1710 identidades en 77 grupos; quedan 1607 en 63 grupos.

El banco mixto de eventos y daño completó 9936 pares y restauró entradas, tablas, flags y globales, con salida natural 0. Sólo los 720 de InvalidateEvent fueron aprobados. El primer ComputeWillKillPed tuvo 1560 diferencias: 1536 se explican por el índice de cheat incorrecto 77 frente al 61 nativo; 24 por redondear el remainder de caída antes de calcular daño. El modelo anterior también seguía 77; el diagnóstico lo conserva como error, y el V2 separado añade 61/77 independientes. No se acredita daño por corregir un modelo. [Replay original rechazado](../reversing/RequestedRemainingLeafPair/runs/20261008-131904-293285/enriched-summary.json) y [diagnóstico estático V2](../reversing/RequestedRemainingLeafPairV2/NATIVE_DIAG.md).

## Seis getters de tipo de tarea

Se añadieron seis GetTaskType, **1280 pares cada uno / 7680 total**, comparando entrada original/candidata, slots originales/DLL y despacho virtual sobre arenas privadas. Los cinco V0 del CSV conservan dirección0 como identidad; las direcciones nativas verificadas figuran como metadata auxiliar, sin sustituir claves del catálogo. ProstituteSolicit conserva su V0x00661AE0. REA no reconoció estos procedimientos; los bytes originales y slots se documentan con extracción PE y replay. [Resumen con correspondencias y límites](../reversing/RequestedTaskTypeSix/validation-summary.json).

Los constructores y stubs de animación/referencias preparan fixtures; sus contadores se verifican mediante predicados compilados, sin serializar cada llamada individual. Las arenas omiten destructores y no adquieren recursos reales. La aprobación acredita sólo el getter constante, no los constructores, destructores, actores, pools ni animación.

Las dos correcciones de ComputeWillKillPed (cheat nativo 61 y conservación del remainder x87 al guardar salud) fueron probadas en el **V2 de 19152 pares: 18432 de daño y 720 de eventos**, con cero diferencias frente al EXE y al modelo independiente, salida natural 0 y restauración completa. Sólo se añade la identidad de daño; InvalidateEvent ya estaba registrada. El producto de daño cubre tres tipos de ped, 64 combinaciones con cheats 61/77/67 independientes, ocho armas y doce límites finitos. Las llamadas de referencias, muerte forzada y voz son colaboradores aislados: esta aprobación histórica no acredita actores reales, esos helpers ni todas las entradas float/NaN/infinito. La revisión de dependencias sigue pendiente. [Resumen de daño aprobado](../reversing/RequestedRemainingLeafPairV2/validation-summary-damage18432.json). El corpus original fallido y su modelo incorrecto permanecen intactos.

## REA y dependencia del EXE

REA local admite el EXE x86 canónico de GTA SA y conserva la admisión del EXE específico de GTA IV. La conexión nueva verificó importación, bytes, pseudocódigo y ensamblado de CDecision::From y análisis de WinMain. La campaña estática resolvió direcciones para las **1710 identidades iniciales**, obtuvo **1395 dossiers** y conservó **315 fallos de búsqueda de procedimiento**. Esto no es aprobación dinámica ni eliminación de dependencias. [Resumen completo de campaña](../reversing/RequestedValidation20261008/rea-campaign-summary.json) y [reparación del proveedor](../reversing/RequestedValidation20261008/REA_REPAIR.md). GetTaskType de GangDriveBy conserva la limitación de reconocimiento en Ghidra: su cuerpo completo de seis bytes se acreditó con bytes y ensamblado, sin inventar pseudocódigo.

El objetivo standalone existe en CMake, pero WinMain sólo permite exportar hooks y termina fuera de ese modo. Persisten llamadas a direcciones del original y globales ligados a su memoria. No existe evidencia de un juego independiente terminado. Las 17765 entradas detectadas por Ghidra no son un censo semántico exacto; los 9326 cuerpos sin nombre cruzado no equivalen automáticamente a funciones necesarias pendientes ni a nuevas clases.

Esta actualización conserva las mediciones históricas de cada banco. Modificar documentación incluida en una revisión de dependencias puede retirar su vigencia sin convertir una prueba histórica en fallida.

## Pendientes por clase

Tabla generada desde `docs/hooks.csv`; cuenta `validated=0`, independientemente del bloqueo del hook.

El primer V2 de GeometryPlanes tuvo 1444 diferencias/2048 y se conserva rechazado. El replay corregido aprobó 2048 con buffers separados o alias exacto completo, modelo x87 PC53 y 8192 callbacks nativos auditados; Normalise nativo sigue siendo una dependencia, sin acreditación del helper ni de aliases desplazados, NaN/Inf o actores. [Fallo preservado](../reversing/RequestedGeometryPlanesV2/runs/20261008-140227-969549/enriched-summary.json) y [aprobación limitada](../reversing/RequestedGeometryPlanesV2/validation-summary.json).

Context9 conservó su baseline de 1744 diferencias y aprobó 82728 después de corregir la precisión de los dos LookAround y el gate FireWeapon de WeaponJustDown. Sus nueve identidades sólo acreditan campos privados de actor/tarea; los getters de helpers no tienen trazas de cantidad/orden. Las seis mutaciones de CPad añadieron 5888 con máscaras originales de escritura. PlaceGroup V2 añadió 1536 por seis enums válidos; compara AL crudo, incluyendo 128/252, y excluye los 24 bits altos no definidos del retorno. Todos preservaron salida natural 0 y restauración completa.

El banco AnimProjectile agotó 600 segundos tras 57138 filas completas de 60624; no produjo terminal/restauración natural ni aprobación parcial. El intento rechazado permanece intacto. La auditoría posterior de sus 720 casos de ReferenceAnims detectó 48 diferencias: con el flag del bloque de animación en 1, el EXE añade referencia y el candidato pide streaming. La fuente consulta otro global de carga; la corrección está propuesta y esa función sigue pendiente. [Diagnóstico reproducible](../reversing/RequestedAnimProjectilePair/Diagnostics/reference-loadstate-diagnostic.json). El retry mantiene la misma ASI 43c7b635 y los 12 inputs congelados; un wrapper separado amplía sólo timeout a 1200, aún sin aprobación en este corte. [Intento incompleto](../reversing/RequestedAnimProjectilePair/runs/20261008-142758-927336/enriched-summary.json) y [wrapper explícito](../reversing/RequestedAnimProjectilePair/run_extended.py).

El freeze relativo antiguo de TaskCtorDtor corresponde a preparación no ejecutada: su documento 822866 se preserva en un archivo histórico. El documento posterior cf755 no era entrada compilada del banco aprobado y su aviso actual puede actualizarse; ambas épocas quedan separadas en la lista de excepciones.

| Clase/grupo | Funciones sin validar |
| --- | ---: |
| CAEAudioHardware | 1 |
| CAEFrontendAudioEntity | 4 |
| CAEPedAudioEntity | 15 |
| CAEPoliceScannerAudioEntity | 13 |
| CAERadioTrackManager | 29 |
| CAEScriptAudioEntity | 8 |
| CAEStreamingChannel | 18 |
| CAEVehicleAudioEntity | 102 |
| CAudioEngine | 82 |
| CAutomobile | 99 |
| CBike | 22 |
| CCam | 14 |
| CCamera | 91 |
| CCarEnterExit | 27 |
| CCheat | 72 |
| CClothesBuilder | 15 |
| CCollision | 1 |
| CCustomBuildingRenderer | 3 |
| CFont | 32 |
| CGangWars | 32 |
| CGenericGameStorage | 5 |
| CGroupEventHandler | 30 |
| CHud | 30 |
| CIdleCam | 13 |
| CMonsterTruck | 5 |
| CPad | 8 |
| CPathFind | 54 |
| CPedGeometryAnalyser | 7 |
| CPickups | 35 |
| CPlane | 14 |
| CPlayerPed | 1 |
| CPools | 16 |
| CPopulation | 64 |
| CPostEffects | 35 |
| CProjectileInfo | 1 |
| CRadar | 63 |
| CRealTimeShadowManager | 5 |
| CRoadBlocks | 7 |
| CRope | 5 |
| CRunningScript | 24 |
| CShadows | 21 |
| CStreaming | 94 |
| CTaskComplexArrestPed | 4 |
| CTaskComplexKillCriminal | 8 |
| CTaskComplexKillPedOnFootArmed | 8 |
| CTaskComplexPlayHandSignalAnim | 2 |
| CTaskComplexProstituteSolicit | 4 |
| CTaskGangHasslePed | 2 |
| CTaskGangHassleVehicle | 2 |
| CTaskGoToVehicleAndLean | 2 |
| CTaskSimpleFightingControl | 3 |
| CTaskSimplePlayerOnFoot | 10 |
| CTaskUtilityLineUpPedWithCar | 2 |
| CTheScripts | 69 |
| CTrain | 20 |
| CVehicleModelInfo | 75 |
| CVisibilityPlugins | 84 |
| CWaterLevel | 25 |
| CWeapon | 35 |
| FxEmitterBP_c | 2 |
| FxSystem_c | 31 |
| InteriorGroup_c | 1 |
| Win | 1 |
