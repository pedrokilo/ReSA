import json, os, pathlib, hashlib

R = 'reversing/CppTwentyFilesUnified/runs'
S = 'reversing/CppTwentyFilesStudy/README.md'


def last(bank):
    return sorted(d for d in os.listdir(R) if d.startswith(bank + '-'))[-1]


def summ(bank):
    d = last(bank)
    s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
    assert s['passed'], bank
    return d, s


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')
G = 'source/game_sa/'
total = 0


def entry(addr, ident, n, src, bank, scope, extra=()):
    global total
    assert addr not in a, addr
    d, s = summ(bank)
    e = {
        'identity': ident, 'converted_source_functions': n, 'source': G + src,
        'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': d.split('-', 1)[1], 'pairs': s['pairs'],
        'own_pairs': s['pairs'], 'audit': f'{R}\\{d}/summary.json', 'differences': 0, 'null_comparison_records': 0,
        'state_restored': True, 'helper_codegen': S,
    }
    if extra:
        e['additional_banks'] = []
        for b in extra:
            dd, ss = summ(b)
            e['additional_banks'].append({'bank': b, 'run': dd.split('-', 1)[1], 'pairs': ss['pairs'], 'summary': f'{R}\\{dd}/summary.json'})
    e['scope'] = scope + tail
    a[addr] = e
    total += n


entry('0x00454C70', 'CPickup::IsVisible (PickupIsVisibleKeepEdx) y PickupRevenueIncrement', 2, 'Pickup.cpp', 'PickWeap1',
      'Puente de residuo de registros con ResidueRegsCpp (verificado por contrib/audit_register_residue.py) e incremento de ingresos con UnsignedTimesFloatCpp.', ('PickupsPending',))
entry('0x005A07D0', 'CObject::ProcessSamSiteBehaviour, SetMatrixForTrainCrossing y ayudantes (ObjMulByCrtPow, ObjTrainCrossingAngle, ObjDoorAngleDiff)', 5, 'Entity/Object/Object.cpp', 'Object48',
      'Ayudantes Object* de X87Math bajo float_control(precise); la potencia de la CRT queda en un ayudante aparte.')
entry('0x006CE370', 'CQuadBike (QuadDeltaStoredDamp, QuadDeltaFullDamp, QuadPowTimes, QuadLeanAngle, QuadAtan2)', 5, 'Entity/Vehicle/QuadBike.cpp', 'Misc61b',
      'Ayudantes QuadBike* de X87Math; atan2 y pow en funciones aparte sin precise.')
entry('0x00443DB0', 'CGangWars::CalculateTimeTillNextAttack, GangWaveRandomOffset, GangDefenderOffset y GangWarTimeDelta', 4, 'GangWars.cpp', 'GangEvent1',
      'Ayudantes GangWars* de X87Math; enteros sin signo como fild + 2^32.', ('GangWarsPending',))
entry('0x005F1500', 'CPedGeometryAnalyser::ComputeEntityDirs, GeometryPlaneOffset y las dos funciones de longitud de ruta', 4, 'PedGeometryAnalyser.cpp', 'GeometryCore355',
      'Ayudantes PedGeom* de X87Math.', ('GeometryRoutes355', 'RequestedGeometryPlanesV2'))
entry('0x005618D0', 'CTimer::UpdateVariables y ayudantes X87DivByU32, X87AddU32Ftol, X87MulU32Ftol y X87U64ToFloat', 5, 'Timer.cpp', 'Sys13',
      'Ayudantes Timer* de X87Math con enteros sin signo y 64 bits como el EXE.')
entry('0x0073C710', 'CWeapon (AsinDegreesX87, SetUpPelletCol, DoDriveByAutoAiming, FireAreaEffect, FireFromCar, InstantHitSwayAccumulate355, InstantHitOscillation, Fire)', 8, 'Weapon.cpp', 'WeaponInstant355',
      'Ayudantes Weapon* de X87Math (incl. WeaponTanCpp, WeaponLessThanTanCpp, WeaponSwayAccumulateCpp y WeaponHeadingDegreesCpp).', ('WeaponDrive355', 'WeaponFinal2', 'PickWeap1'))
entry('0x0072B850', 'CWeather (SinMulAddX87, CosMulAddX87, HalfMinusHalfCosX87, NegSinX87, CosX87)', 5, 'Weather.cpp', 'Fx43',
      'Ayudantes Weather* de X87Math (seno/coseno sin redondear donde el EXE los deja en ST0).')
entry('0x005FD810', 'CPedGroupPlacer::PlaceGroup (conmutador naked) y PlaceCircularGroup', 2, 'PedGroupPlacer.cpp', 'GroupPlacerPending',
      'PlaceGroup pasa a C++ plano (noinline en PlaceChatGroup/PlaceRandomGroup); círculo con PedGroupPlacerCircleCpp.', ('RequestedGroupPlacerCorrectedV2',))
entry('0x00506800', 'CAEWeatherAudioEntity::AddAudioEvent, UpdateParameters y Service', 3, 'Audio/Entities/AEWeatherAudioEntity.cpp', 'AudioCompletion',
      'Ayudantes WeatherAudio* de X87Math.')
entry('0x004B4AC0', 'CEventKnockOffBike::SetPedSafePosition y ayudantes X87Mul, X87Mul4, X87MulAdd y X87Combine', 5, 'Events/EventKnockOffBike.cpp', 'Data8b',
      'Ayudantes KnockOff* de X87Math. El banco Data8b necesitó dos interceptaciones nuevas en la DLL (CCamera::IsSphereVisible y CPathFind::FindNodeOrientationForCarPlacement) con el mismo Fake que en el EXE; ninguna compuerta se relajó.')
entry('0x0058A330', 'CHud::DrawRadar y DrawSubtitles (SubtitleScaleY, SubtitleCentreVitals y SubtitleScaleX)', 5, 'Hud.cpp', 'HudCamFinal2',
      'Ayudantes Hud* de X87Math.', ('HudSubtitles355',))
entry('0x00737C80', 'CProjectileInfo (ProjectileCreatorHeading, ProjectileThrownVelocity, ProjectileDamping, RemoveBackwardProjectileSteering, AddProjectileSteering, LimitProjectileSpeed)', 6, 'ProjectileInfo.cpp', 'ProjectileAdd355',
      'Ayudantes ProjInfo* de X87Math.', ('ProjectileUpdate355',))
entry('0x0064F1E0', 'CCarEnterExit::ComputePassengerIndexFromCarDoor (puente KeepEdx), GetNearestCarDoor e IsPlayerToQuitCarEnter', 3, 'CarEnterExit.cpp', 'CarEnterWave4',
      'Puente de residuo con #pragma runtime_checks off, safebuffers y CarEnterExitEntryEdxCpp (verificado por audit_register_residue.py).', ('CarQuit355', 'MiscWave2'))
entry('0x0060A820', 'CPlayerPed::GetButtonSprintResults (puente KeepEdx) y ayudantes PlayerPedATanMinus y PlayerPedAtan2', 3, 'Entity/Ped/PlayerPed.cpp', 'PlayerPed51',
      'Puente de residuo con PlayerPedResidueFloatCpp y safebuffers (verificado por audit_register_residue.py).')
entry('0x0043EAF0', 'CEntryExit::GetPositionRelativeToOutsideWorld (puente KeepEdx), FindValidTeleportPoint e IsInArea', 3, 'EntryExit.cpp', 'World11',
      'Puente de residuo con EntryEaxCpp (verificado por audit_register_residue.py).', ('HookUnlockAudit',))
entry('0x006F6CC0', 'CTrain (TrainProjection355, TrainLighting355, TrainLightingBlend355, TrainRailDistance355, ScaleByExePow, ScaleSixByExePow, HeadingMinusOld)', 7, 'Entity/Vehicle/Train.cpp', 'TrainWave3',
      'Ayudantes Train* de X87Math.', ('TrainPosition355',))
entry('0x00458970', 'CPickups (CreatePickupCoorsCloseToCoors, CreateSomeMoney, PickupGlowByte, DoPickUpEffects)', 4, 'Pickups.cpp', 'PickupsPending',
      'Ayudantes Pickups* de X87Math.')
entry('0x004D1610', 'RpAnimBlend NormaliseQuatExe (AnimQuaternionNormaliseCpp)', 1, 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp', 'RpAnim43',
      'Solo se convirtió NormaliseQuatExe; los seis bloques que quedan en el archivo son puentes al EXE con argumento en EAX y no se tocaron.')
entry('0x00704150', 'CPostEffects (RippleWave, RippleFinalU, FogSinCoord, FogCosCoord, InfraredVisionStoreAndSetLightsForHeatObjects)', 6, 'PostEffects.cpp', 'FxPost1',
      'Ayudantes PostFx* de X87Math (log y tan como intrínsecos).')

json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones fuente convertidas en este lote:', total)

D = pathlib.Path('reversing/CppTwentyFilesStudy/artifacts')
rows = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(D.glob('*.obj'))}
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {summ(b)[1]["pairs"]} |' for b in banks)
txt = f'''# Veinte archivos de nivel 2 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó el ensamblador de `Pickup.cpp`, `Entity/Object/Object.cpp`, `Entity/Vehicle/QuadBike.cpp`, `GangWars.cpp`, `PedGeometryAnalyser.cpp`, `Timer.cpp`, `Weapon.cpp`, `Weather.cpp`, `PedGroupPlacer.cpp`, `Audio/Entities/AEWeatherAudioEntity.cpp`, `Events/EventKnockOffBike.cpp`, `Hud.cpp`, `ProjectileInfo.cpp`, `CarEnterExit.cpp`, `Entity/Ped/PlayerPed.cpp`, `EntryExit.cpp`, `Entity/Vehicle/Train.cpp`, `Pickups.cpp` y `PostEffects.cpp` ({total} funciones fuente), y se convirtió `NormaliseQuatExe` de `Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp`. Un subagente por archivo preparó el análisis; compilación y bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

Cada función se comparó con el desensamblado del EXE y se convirtió a float puro en el mismo orden (con `float_control(precise)` donde `/fp:fast` podía reasociar) o a ayudantes nuevos de `X87Math` (los transcendentales, en funciones aparte sin `precise`). Los antiguos envoltorios `naked` que reproducían el residuo de registros pasan a C++ con `ResidueRegsCpp`, `EntryEaxCpp`, `CarEnterExitEntryEdxCpp` y `PlayerPedResidueFloatCpp`, con `#pragma runtime_checks("", off)` y `safebuffers` donde hacía falta; `contrib/audit_register_residue.py` los verifica sobre el código objeto (se ampliaron sus objetos y patrones, sin relajarlo).

## Qué se encontró y cómo se resolvió

- `Data8b` falló al principio: la DLL llamaba directamente a `CCamera::IsSphereVisible` y a `CPathFind::FindNodeOrientationForCarPlacement` y el banco solo interceptaba las direcciones del EXE. Se añadieron en `reversing/Data8b/LiveComparison.hpp` las interceptaciones en la entrada de la DLL con el mismo Fake; no se quitó ninguna compuerta. Quedó en 17024 pares sin diferencias.
- `Small2` y `RpAnim43` dieron un fallo intermitente («GTA no responde») y pasaron al repetir; las corridas fallidas se conservan como evidencia.
- El auditor de residuo marcó correctamente `__RTC_CheckEsp` en el puente de `PlayerPed`; se corrigió el código, no el auditor.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
{btab}

## Límites

- `RpAnimBlend.cpp` conserva seis bloques de ensamblador: son puentes al EXE con el argumento en EAX (código compilado pero desactivado con `USE_COPY_PASTE_FRAME_UPDATE=0`) y dependen del EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de diecinueve archivos de nivel 2 (`Pickup`, `Object`, `QuadBike`, `GangWars`, `PedGeometryAnalyser`, `Timer`, `Weapon`, `Weather`, `PedGroupPlacer`, `AEWeatherAudioEntity`, `EventKnockOffBike`, `Hud`, `ProjectileInfo`, `CarEnterExit`, `PlayerPed`, `EntryExit`, `Train`, `Pickups` y `PostEffects`) y se convirtió una función de `RpAnimBlend`; "
       "los bancos aprobaron sin diferencias ([detalle y hashes](../CppTwentyFilesStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de diecinueve archivos de nivel 1')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
