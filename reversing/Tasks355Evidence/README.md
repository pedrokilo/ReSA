<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tasks, geometry, FX and HUD: batch 355

Native bodies reconstructed from REA pseudo and canonical EXE assembly. Unapproved hooks remain false. Root owns build, sequential GTA runs and global inventory updates. banks.json contains exact identities, planned counts and actual approved reports. Ready is distinct from approved.

Original scope: 99 functions in 12 classes, 14 banks. Additional coordinated scope: 3 FxEmitter functions in 2 banks and 7 HUD functions in 7 banks. Total prepared identities before CCam: 109. DrawSubtitles belongs to root; its requested x87 coordinate repair is included in Hud.cpp.

Approved reports currently recorded locally: GangJoin 1792, Boat 2048, HitCar 3328, FxParticle 1024, FxRender/HeatHaze 3072 (20261007-222532-935246). Other banks await root runtime or corrections. Each bank README specifies isolated dependencies and limits. Canonical assembly supplements use private dis_complete.sh because common dis.sh truncates disassembly at 2000 lines. REA indexes remain private.

New HUD banks: HudSuccess355, HudWanted355, HudTimers355, HudVitals355, HudCrosshairs355, HudHelp355 and HudPlayerInfo355, each 1024 contiguous pairs. Complete source dependencies checked; all fixtures snapshot and restore touched globals and compare traces and object state.

Additional CCam scope coordinated with world355: 10 large processing functions, under reconstruction. All ten are prepared: nine with 1024 pairs each: CamFirstPerson355, CamTwoCars355, CamTwoPlayer355, CamShooting355, CamMouse355, CamM16355, CamAim355 CamFollowPed355 and CamFollowCar355. CamProcess355 adds 8192 pairs for Process; no CCam approval is claimed. Supplemental uncatalogued camera/pad helpers have private REA indexes and complete assembly in CCamExtra_*.txt.


Coordinated Bike scope: ProcessAI (0x6BC930, vtable871360 slot66 confirmed in canonical EXE) and ProcessRiderAnims (0x6B7280), native and prepared in BikeAI355/BikeRider355 with 1024 pairs each. Private REA index tasks355-bike-pseudo/index.json recovered both missing pseudocodes. Hooks remain false pending runtime.

2026-10-08: root approved final HUD group (Subtitles plus seven local banks), 1024 pairs each. Local task approvals added: Wander3072 report20261008-011104-463648; HandSignal2560 report20261008-011231-755625; VehicleLean3840 report20261008-011309-582333; SmartFlee4864 report20261008-011551-091985. Sunbathe and FormationFollow require fixture repeats; HassleVehicle and GangFollower require source repeats after the documented fixes.

Additional coordinated scope: BikePreRender355 (0x6BD090, vtable871360 slot17), native and prepared1024 pairs; not approved yet. WaterRender355 audited on root request: exact resident x87 shift helper, semantic CRenPar fields excluding two undefined padding bytes, and visible/invisible coverage fixtures; repeat pending. These notes do not update global hook status or CSV registries.
