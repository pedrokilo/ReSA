<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Asociación de bancos para las 1710 identidades validated=0

Identidades exactas/full-label: 57. Sólo candidatos por coocurrencia nombre/clase: 279. Sin banco encontrado: 1374. No son resultados de validación.

`pending-bank-map.json` conserva todas las 1710 identidades con candidatos, método de coincidencia, índice y pares cuando se pudieron asociar; `pending-without-bank.csv` enumera sólo las no encontradas.

Se revisaron headers inmediatos *.hpp/*.h de carpetas de reversing, descriptores fns y variantes con clase/nombre/dirección, etiquetas completas, pares_per_function/function_pairs y arrays names/labels/functions. Una función mencionada como colaborador no queda acreditada: los candidatos requieren lectura semántica. Los resúmenes indexados sólo pueden asociarse con orden de array confirmado; el mapa deja los pares desconocidos sin inventarlos.

Win::NOTSA_WinMain se encuentra en Probe.hpp de UnlockPlatform, índice 5 de names. Es selección de arranque y contador de llamadas; los pares indexados del resumen corresponden a 2,4,6. No hay pares de la entrada WinMain. Véase win-entry-audit.md.

Clases sin banco para al menos una identidad: CAEFrontendAudioEntity, CAEPedAudioEntity, CAEPoliceScannerAudioEntity, CAERadioTrackManager, CAEScriptAudioEntity, CAEStreamingChannel, CAEVehicleAudioEntity, CAudioEngine, CAutomobile, CBike, CCam, CCamera, CCarEnterExit, CCheat, CClothesBuilder, CCollision, CDecision, CFont, CFormation, CGangWars, CGenericGameStorage, CGroupEventHandler, CHud, CIdleCam, CInterestingEvents, CMonsterTruck, CPad, CPathFind, CPedDamageResponseCalculator, CPedGeometryAnalyser, CPedGroupPlacer, CPickups, CPlane, CPlayerPed, CPools, CPopulation, CPostEffects, CProjectileInfo, CRadar, CRealTimeShadowManager, CRoadBlocks, CRope, CRunningScript, CShadows, CStreaming, CTaskComplexKillCriminal, CTaskComplexKillPedOnFootArmed, CTaskComplexProstituteSolicit, CTaskSimpleFightingControl, CTaskSimplePlayerOnFoot, CTheScripts, CTrain, CVehicleModelInfo, CVisibilityPlugins, CWaterLevel, CWeapon, FxEmitterBP_c, FxSystem_c, InteriorGroup_c.
