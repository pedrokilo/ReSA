import json, os, pathlib, hashlib

R = 'reversing/CppNineteenFilesUnified2/runs'
S = 'reversing/CppNineteenFilesStudy/README.md'


def last(bank):
    return sorted(d for d in os.listdir(R) if d.startswith(bank + '-'))[-1]


def summ(bank):
    d = last(bank)
    s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
    assert s['passed'], bank
    return d, s


def ref(bank):
    d, s = summ(bank)
    return d.split('-', 1)[1], f'{R}\\{d}/summary.json'


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')
G = 'source/game_sa/'
total = 0


def entry(addr, ident, n, src, bank, own, scope, extra=()):
    global total
    d, s = summ(bank)
    r, au = ref(bank)
    e = {
        'identity': ident, 'converted_source_functions': n, 'source': G + src,
        'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': r, 'pairs': s['pairs'],
        'own_pairs': own, 'audit': au, 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
        'helper_codegen': S,
    }
    if extra:
        e['additional_banks'] = []
        for b in extra:
            dd, ss = summ(b)
            e['additional_banks'].append({'bank': b, 'run': dd.split('-', 1)[1], 'pairs': ss['pairs'],
                                          'summary': f'{R}\\{dd}/summary.json'})
    e['scope'] = scope + tail
    a[addr] = e
    total += n


# 3dMarkers
entry('0x00725120', 'C3dMarkers::PlaceMarker (ComputeStdSize y distancia al jugador)', 2, '3dMarkers.cpp', 'Markers', 4096,
      'Float puro bajo float_control(precise) en ComputeStdSize (sz - sz·0,3 y sz - ((25 - dist)·sz)·0,015…) y sqrt(dx²+dy²) en float.')
# AEPedAudioEntity
entry('0x004E0EE0', 'CAEPedAudioEntity::UpdateJetPack', 1, 'Audio/Entities/AEPedAudioEntity.cpp', 'AudioWave2', 1024,
      'PedAudioJetPackSinCpp: fsin sin redondear, seno guardado como float, fchs solo si es negativo, ·k + 1.')
entry('0x004E1CC0', 'CAEPedAudioEntity::HandlePedHit', 1, 'Audio/Entities/AEPedAudioEntity.cpp', 'AudioPedPending', 2048,
      'AudioLog10Times20Cpp (ya existía).')
entry('0x004E2EE0', 'CAEPedAudioEntity::Service (PedShirtLogarithmicVolume)', 1, 'Audio/Entities/AEPedAudioEntity.cpp', 'AudioPedPending', 1024,
      'AudioLog10Times20Cpp con el umbral 1e-5 y -5 de 0x4D9E50; corrige el tratamiento de NaN y el redondeo previo al producto del código anterior.')
# AEMFDecoder
entry('0x004E7CB0', 'CAEMFDecoder::Initialise (duración)', 1, 'Audio/Loaders/AEMFDecoder.cpp', 'MFDec', 1024,
      'IntTimesFloatCpp(longitud, k) y truncado; las llamadas a QuickTime (plugin::Call) no se tocaron.')
entry('0x004E7940', 'CAEMFDecoder::GetStreamPlayTimeMs', 1, 'Audio/Loaders/AEMFDecoder.cpp', 'MFDec', 512,
      'MfDecoderPlayTimeMsCpp: fild · fdiv por el divisor · _ftol2 con un solo redondeo a 24 bits.', ('Sys10',))
entry('0x004E78E0', 'CAEMFDecoder::SetCursor', 1, 'Audio/Loaders/AEMFDecoder.cpp', 'MFDec', 512,
      'MfDecoderCursorToPlayTimeCpp = UnsignedTimesFloatCpp(pos, multiplicador) truncado: entero sin signo como fild + 2^32 y un solo redondeo.')
# CarAI
entry('0x0041CD00', 'CarAI::EntitiesGoHeadOn (HeadOnDot)', 1, 'CarAI.cpp', 'CarAI25', 1024,
      'CarAIHeadOnDotNoMatrixCpp: (dz·0 + cos·dy) + (-sin)·dx con seno y coseno sin redondear y float_control(precise).')
entry('0x0041CCB0', 'CarAI::ExeForward (vía UpdateCarAI)', 1, 'CarAI.cpp', 'CarAI25', 0,
      '-TrigSimpleSinCpp / TrigSimpleCosCpp en la rama SIN matriz, que es código muerto: UpdateCarAI (la única que llama a ExeForward, en 0x41FE98) lee antes m_matrix->GetUp().z sin comprobar nulo (0x41FDE7), así que con m_matrix nulo el EXE se cae antes de llegar. Solo se ejerce la rama con matriz (copia del vector) dentro de UpdateCarAI (CarAI25, 2048 pares); la rama convertida no se puede alcanzar con ningún fixture.')
entry('0x0041CC70', 'CarAI::ExeRight (vía UpdateCarAI)', 1, 'CarAI.cpp', 'CarAI25', 0,
      'TrigSimpleSinCpp / TrigSimpleCosCpp en la rama SIN matriz, que es código muerto: UpdateCarAI (la única que llama a ExeRight, en 0x420310) lee antes m_matrix->GetUp().z sin comprobar nulo (0x41FDE7), así que con m_matrix nulo el EXE se cae antes. Solo se ejerce la rama con matriz dentro de UpdateCarAI (CarAI25, 2048 pares); la rama convertida no se puede alcanzar con ningún fixture.')
# ClothesBuilder
entry('0x005A4940', 'CClothesBuilder::BlendGeometry (3 marcos: ClothesBlendTriple y ClothesNormalizeWeights)', 2, 'ClothesBuilder.cpp', 'ClothesGeometry355', 2048,
      'Float puro en el mismo orden bajo float_control(precise); normalización 1/suma y producto por peso.', ('ClothesArray355',))
entry('0x005A4F10', 'CClothesBuilder::BlendGeometry (2 marcos: ClothesBlendPair)', 1, 'ClothesBuilder.cpp', 'ClothesGeometry355', 2048,
      'Float puro bajo float_control(precise).')
# Cover
entry('0x006992B0', 'CCover::FindAndReserveCoverPoint (CoverDirDot)', 1, 'Cover.cpp', 'World16', 1024,
      'CoverDirDotCpp bajo float_control(precise): a = dirIndex·paso, coseno redondeado, -sin·dx sin redondear y dz·0 conservado.')
entry('0x00698D40', 'CCover::FindDirFromVector', 1, 'Cover.cpp', 'World16', 512,
      'CoverDirFromVectorCpp: atan2(-x, y)·escala con un solo redondeo y truncado de 32 bits como _ftol2.')
entry('0x00698D60', 'CCover::FindVectorFromDir', 1, 'Cover.cpp', 'World16', 512,
      'IntTimesFloatCpp, -TrigSimpleSinCpp y TrigSimpleCosCpp.')
# Door
entry('0x006F4040', 'CDoor::Process (seno, coseno y amortiguado)', 3, 'Door.cpp', 'AnimAudio6', 1024,
      'TrigSimpleSinCpp / TrigSimpleCosCpp y PowScaleFloatCpp (ayudantes ya existentes).')
# Entity
entry('0x00534E90', 'CEntity::ModifyMatrixForTreeInWind (EntityA_SinTimes)', 1, 'Entity/Entity.cpp', 'CEntA', 1024,
      'TrigSimpleSinMulCpp (ya existía).')
entry('0x00533790', 'CEntity::CreateEffects (EntityGroupC_HeadingDegreesPlus)', 1, 'Entity/Entity.cpp', 'CEntC', 512,
      'EntityHeadingDegreesPlusCpp: atan2·grados + base con el fpatan sin redondear; sin matriz, float puro.')
entry('0x006FC7A0', 'CEntity::ProcessLightsForEntity (dos fiadd)', 1, 'Entity/Entity.cpp', 'CEntC', 1024,
      'FloatPlusIntCpp(producto en float, entero) (ya existía).')
# EntityScanner
entry('0x005FFA20', 'CEntityScanner::ScanForEntitiesInRange', 1, 'EntityScanner.cpp', 'ScannerDragFx', 4096,
      'InterestingSectorCpp (sectores), DistanceSquaredZXYCpp ((dz²+dx²)+dy²) y rango² en float; comparaciones ordenadas.')
# FireManager
entry('0x0053AF00', 'CFireManager::Update (FireSinMul, FireCosMul y el bloque de rumbo)', 3, 'FireManager.cpp', 'Fire24', 1024,
      'TrigSimpleSinMulCpp / TrigSimpleCosMulCpp y -TrigSimpleSinCpp / TrigSimpleCosCpp (ya existían).')
# ProcSurfaceInfo
entry('0x005A32D0', 'ProcSurfaceInfo_c::AddObject (producto por fila, coseno/seno y desplazamiento de z)', 2, 'Plant/ProcSurfaceInfo.cpp', 'Misc8', 1024,
      'ProcSurfaceScaleRowCpp y ProcSurfaceOffsetZCpp bajo float_control(precise); TrigSimpleCosCpp / TrigSimpleSinCpp.')
# rwplcore.h
entry('0x00703650', 'RwFastRealToUInt32 y RwInt32FromReal (rwplcore.h; usados por CPostEffects::ColourFilter)', 2, 'RenderWare/rw/rwplcore.h', 'FxPost1', 256,
      'RwPlCoreFastRealToUInt32Cpp: redondeo al par con la palabra de control por defecto, 0x80000000 fuera de rango; RwInt32FromReal como truncado en C++. Para los llamadores reales (uint8) es la identidad.')
entry('0x00703520', 'CPostEffects::SetFilterMainColour (usa RwFastRealToUInt32)', 0, 'PostEffects.cpp', 'FxPost1', 256,
      'Cubre el mismo cambio de rwplcore.h.')
# TheScripts
entry('0x004939F0', 'CTheScripts::ProcessAllSearchLights (ExeAtan2)', 1, 'Scripts/TheScripts.cpp', 'ScriptsFinal3', 512,
      'TrigAtan2Cpp(-dir.x, dir.y) y TrigAtan2Cpp(dir.z, sqrt(...)) (ya existía).')
entry('0x00464980', 'CTheScripts::DrawScriptSpritesAndRectangles (MONOCOLOR_ANGLED)', 1, 'Scripts/TheScripts.cpp', 'ScriptsFinal3', 512,
      'TheScriptsAngledRectTermsCpp: coseno sin redondear, seno guardado como float y cuatro productos a 24 bits.')
entry('0x00485C20', 'CTheScripts::ScriptDebugCircle2D', 1, 'Scripts/TheScripts.cpp', 'ScriptsFinal3', 256,
      'IntTimesFloatCpp, TrigCompoundSinMulAdd y TrigCompoundCosMulAdd (ya existían).')
# AEAudioUtility
entry('0x004D9E80', 'CAEAudioUtility::GetCurrentTimeInMS', 1, 'Audio/AEAudioUtility.cpp', 'Small2', 512,
      'AudioUtilTimeMsCpp: (contador/frecuencia)·1000 - inicio con las dos mitades del inicio como el EXE.')
entry('0x004D9EF0', 'CAEAudioUtility::ConvertFromBytesToMS', 1, 'Audio/AEAudioUtility.cpp', 'Small2', 1024,
      'AudioUtilBytesToMsQuotientCpp con enteros sin signo como fild + 2^32.')
entry('0x004D9F40', 'CAEAudioUtility::ConvertFromMSToBytes', 1, 'Audio/AEAudioUtility.cpp', 'Small2', 1024,
      'AudioUtilMsToBytesProductCpp: (P·0,002)·ms.')
entry('0x005B97F0', 'CAEAudioUtility::StaticInitialise', 1, 'Audio/AEAudioUtility.cpp', 'Small2', 32,
      'AudioUtilLogInverseCpp (10/denominador) y AudioLog10Cpp.')
# AEAudioHardware
entry('0x004D8990', 'CAEAudioHardware::RescaleChannelVolumes (ExePowGain, ExeAddFadeNearEnd y ExeGainToDecibels)', 3, 'Audio/Hardware/AEAudioHardware.cpp', 'AudioHw54', 1024,
      'AudioHwPowGainCpp, AudioHwFadeNearEndCpp (float_control(precise)) y AudioLog10Times20Cpp.')
# IdleCam
entry('0x00517BF0', 'CIdleCam::ProcessFOVZoom', 1, 'IdleCam.cpp', 'IdleAttractorPending', 1024,
      'IdleCamFovLerpCpp: interpolación con el fsin sin redondear en una función aparte (sin precise).')
entry('0x005179E0', 'CIdleCam::ProcessSlerp', 1, 'IdleCam.cpp', 'IdleCamWave4', 1024,
      'IdleCamSlerpCpp con cuatro llamadas a GetATanOfXY (la cuarta sin redondear) y el límite de t sin redondear.')
entry('0x0050E760', 'CIdleCam::FinaliseIdleCamera', 1, 'IdleCam.cpp', 'IdleCamWave4', 640,
      'TimeCycleDirnToSunCpp (ya existía) y TrigSimpleSinCpp / TrigSimpleCosCpp.')
# TaskSimpleFightingControl
entry('0x00624B50', 'CTaskSimpleFightingControl::CalcMoveCommand', 1, 'Tasks/TaskTypes/TaskSimpleFightingControl.cpp', 'TaskFinal2', 1024,
      'Atan2MinusCpp (ya existía) y FightCtlElapsedCpp: (paso·0,02)·1000 truncado a 64 bits con float_control(precise).')
entry('0x0062A0A0', 'CTaskSimpleFightingControl::ProcessPed', 1, 'Tasks/TaskTypes/TaskSimpleFightingControl.cpp', 'FightingControlPending', 1024,
      'FightCtlShouldBlockCpp y FightCtlAttackDelayCpp (comparación ordenada y retardo con enteros sin signo como fild + 2^32).')
# TaskSimplePlayerOnFoot
entry('0x006859A0', 'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon (Atan2X87 y SinPlusX87)', 2, 'Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp', 'FootWave4', 1024,
      'TrigAtan2Cpp y OnFootSinPlusCpp (float(sin + z) con un solo redondeo).')
entry('0x00687530', 'CTaskSimplePlayerOnFoot::PlayerControlFighter (FighterDirection y FighterProject)', 2, 'Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp', 'PlayerFighterPending', 4096,
      '-TrigSimpleSinCpp / TrigSimpleCosCpp y OnFootFighterProjectCpp (rama cooperativa con seno y coseno redondeados; si no, sin redondear; dz·0 conservado).')
entry('0x00687C20', 'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon (usa los mismos ayudantes)', 0, 'Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp', 'FootWave4', 1024,
      'Cubre los mismos cambios de TaskSimplePlayerOnFoot.cpp.')
entry('0x00687F30', 'CTaskSimplePlayerOnFoot::PlayerControlDucked (usa los mismos ayudantes)', 0, 'Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp', 'FootWave4', 1024,
      'Cubre los mismos cambios de TaskSimplePlayerOnFoot.cpp.')
# WeaponEffects
entry('0x00742CF0', 'CWeaponEffects::Render (SinMulX87, CosMulX87, MulDoubleAddX87 y AngleSinCosX87)', 4, 'WeaponEffects.cpp', 'Ped9', 1024,
      'TrigSimpleSinMulCpp / TrigSimpleCosMulCpp, WeaponFxMulDoubleAddCpp y WeaponFxAngleSinCosCpp (atan2 sin redondear; 1 - cos con el coseno sin redondear).')

json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones fuente convertidas en este lote:', total)

# README del estudio
D = pathlib.Path('reversing/CppNineteenFilesStudy/artifacts')
rows = {}
for p in sorted(D.glob('*.obj')):
    rows[p.name] = hashlib.sha256(p.read_bytes()).hexdigest()
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.rsplit('-', 2)[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {summ(b)[1]["pairs"]} |' for b in banks)
txt = f'''# Diecinueve archivos de nivel 1 sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `3dMarkers.cpp`, `Audio/Entities/AEPedAudioEntity.cpp`, `Audio/Loaders/AEMFDecoder.cpp`, `CarAI.cpp`, `ClothesBuilder.cpp`, `Cover.cpp`, `Door.cpp`, `Entity/Entity.cpp`, `EntityScanner.cpp`, `FireManager.cpp`, `Plant/ProcSurfaceInfo.cpp`, `RenderWare/rw/rwplcore.h`, `Scripts/TheScripts.cpp`, `Audio/AEAudioUtility.cpp`, `Audio/Hardware/AEAudioHardware.cpp`, `IdleCam.cpp`, `Tasks/TaskTypes/TaskSimpleFightingControl.cpp`, `Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp` y `WeaponEffects.cpp` ({total} funciones fuente). Un subagente por archivo preparó el análisis (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los 26 bancos aprobaron a la primera, sin tocar ninguno; tras simplificar los ayudantes de `AEMFDecoder` (sin `fma`/`nextafter` ni `<cmath>`) se repitieron los 26 con el código final.

## Qué se hizo

Cada función se comparó con el desensamblado del EXE y se convirtió a float puro en el mismo orden (con `float_control(precise)` cuando `/fp:fast` podía reasociar), a ayudantes ya existentes de `X87Math` o a ayudantes nuevos en `X87Math` (los nombres están en `approvals.json`). `rwplcore.h` tenía dos macros con ensamblador (`RwFastRealToUInt32Inline` con `__asm` y `int32fromreal` con `_asm`; el recuento léxico cuenta las tres apariciones) y se convirtieron las dos.

Correcciones sobre el código anterior que encontró el análisis:
- `AEPedAudioEntity::Service`: el volumen logarítmico daba `log10(NaN)` en lugar de −5 con NaN y redondeaba el logaritmo a float antes del producto.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
{btab}

## Límites

- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes, NaN en la mayoría de los fixtures).
- `CarAI::ExeForward` y `ExeRight`: su rama sin matriz (la que tenía el ensamblador) es código muerto, porque `UpdateCarAI` lee `m_matrix->GetUp().z` sin comprobar nulo antes de llamarlas y el EXE se cae con `m_matrix` nulo; ningún fixture puede alcanzarla. Se convirtió igualmente para eliminar el ensamblador, y CarAI25 ejerce la rama con matriz.
- Código máquina: se listó la secuencia x87 de los 98 ayudantes de `X87Math.obj` con `reversing/CppOnlyMigration/herramientas/coff_x87_listar.py` (resultado en `x87-helpers-sequences.txt`). Los ayudantes con seno, coseno, `fpatan` o `fyl2x` salen en línea (sin `fsincos`, sin llamadas a la CRT para sin/cos/atan2/log10) y sin derrames de 64 bits entre el transcendental y su uso; los únicos `fstp`/`fld` de 64 bits intermedios son argumentos de `ExeCompatiblePowCpp` o valores ya redondeados a 24 bits. Eso comprueba la forma de las secuencias, no su igualdad con el EXE: esa la dan los bancos.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de diecinueve archivos de nivel 1 (`3dMarkers`, `AEPedAudioEntity`, `AEMFDecoder`, `CarAI`, `ClothesBuilder`, `Cover`, `Door`, `Entity`, `EntityScanner`, `FireManager`, `ProcSurfaceInfo`, `rwplcore.h`, `TheScripts`, `AEAudioUtility`, `AEAudioHardware`, `IdleCam`, `TaskSimpleFightingControl`, `TaskSimplePlayerOnFoot` y `WeaponEffects`); "
       "26 bancos sin diferencias ([detalle y hashes](../CppNineteenFilesStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `common.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
