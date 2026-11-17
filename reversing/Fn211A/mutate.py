"""Prueba negativa del banco Fn211A (lote 211A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` o el puntero de salida desalineado, o un valor de capacidades de D3D9 imposible (MaxTextureBlendStages = 0x80000000 o MaxSimultaneousTextures = 0xFFFFFFFF). Las seis instancias del destructor del pool, las tres del constructor y las
de New y Delete comparten cuerpo (plantilla o ayudante): una mutacion en el cuerpo las hace fallar a todas.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
NL = chr(10)
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALO = '(reinterpret_cast<uintptr_t>(out) & 1)'
AUTO = 'source/game_sa/Entity/Vehicle/Automobile.cpp'
ENV = 'source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp'
MAT = 'source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline_EnvMat.cpp'
SOURCES = sorted({AUTO, ENV, MAT})


def round1():
    out = []
    out.append((AUTO, '        wcd->m_nNumSpheres = 4;', '        wcd->m_nNumSpheres = ' + UNALT + ' ? 5 : 4;'))                                                       # 0 SetUpWheelColModel
    out.append((AUTO, 'm_matrix->GetForward());' + NL + NL + '        m_nFakePhysics = 0;', 'm_matrix->GetForward());' + NL + NL + '        m_nFakePhysics = ' + UNALT + ' ? 1 : 0;'))   # 1 BurstTyre
    out.append((AUTO, 'firstExhaustFxSystem->SetConstTime(1, std::fabs(power));', 'firstExhaustFxSystem->SetConstTime(1, std::fabs(power) + (' + UNALT + ' ? 1.0f : 0.0f));'))   # 2 DoNitroEffect
    out.append((AUTO, 'AutomobileGetRideAnimDataExe(CAutomobile* /*self*/, void* /*edx*/) { return nullptr; }',
                'AutomobileGetRideAnimDataExe(CAutomobile* self, void* /*edx*/) { return ' + UNAL + ' ? reinterpret_cast<CRideAnimData*>(1) : nullptr; }'))      # 3 GetRideAnimData
    out.append((AUTO, 'AutomobileFlagToDestroyWhenNextProcessedExe(CAutomobile* /*self*/, void* /*edx*/) {}',
                'AutomobileFlagToDestroyWhenNextProcessedExe(CAutomobile* self, void* /*edx*/) { if (' + UNAL + ') *reinterpret_cast<volatile uint32*>(self) = 0; }'))   # 4 FlagToDestroyWhenNextProcessed
    out.append((AUTO, '    std::ranges::fill(m_aCarNodes, nullptr);' + NL + '    CClumpModelInfo::FillFrameArray(reinterpret_cast<RpClump*>(GetRwObject()), m_aCarNodes.data());',
                '    std::ranges::fill(m_aCarNodes, nullptr);' + NL + '    if (' + UNALT + ') m_aCarNodes[0] = reinterpret_cast<RwFrame*>(1);' + NL + '    CClumpModelInfo::FillFrameArray(reinterpret_cast<RpClump*>(GetRwObject()), m_aCarNodes.data());'))    # 5 SetModelIndex
    out.append((AUTO, '    m_pDustParticle->SetConstTime(true, m_heliDustFxTimeConst);', '    m_pDustParticle->SetConstTime(' + UNALT + ' ? false : true, m_heliDustFxTimeConst);'))   # 6 DoHeliDustEffect
    out.append((AUTO, '    self->Destructor();' + NL + '    if (flags & 1) {' + NL + '        CVehicle::operator delete(self);',
                '    self->Destructor();' + NL + '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        CVehicle::operator delete(self);'))                  # 7 ScalarDeletingDestructor
    out.append((ENV, '    const auto caps = (D3DCAPS9*)RwD3D9GetCaps();', '    const auto caps = (D3DCAPS9*)RwD3D9GetCaps();' + NL + '    if (caps->MaxTextureBlendStages == 0x80000000u) return 1u;'))     # 8 IsEnvironmentMappingSupported
    out.append((MAT, '    raw[0] = ToFixed8(in->Scale.x);', '    raw[0] = ToFixed8(in->Scale.x) + (' + UNALO + ' ? 1 : 0);'))                                                  # 9 EnvMatDataFromBuffer
    out.append((MAT, '    out->Scale.x            = FromFixed8(raw[0]);', '    out->Scale.x            = FromFixed8(raw[0]) + (' + UNALO + ' ? 1.0f : 0.0f);'))  # 10 EnvMatDataToBuffer
    out.append((ENV, '    self->LastFreeSlot    = -1;', '    self->LastFreeSlot    = ' + UNAL + ' ? 0 : -1;'))                                                           # 11-13 constructores
    out.append((ENV, '        self->Size         = 0;', '        self->Size         = ' + UNAL + ' ? 1 : 0;'))                                                       # 14-19 destructores
    out.append((ENV, '    EnvMapPoolDestroy(self);' + NL + '    if (flags & 1) {', '    EnvMapPoolDestroy(self);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))                            # 20 destructor con borrado
    out.append((ENV, '    return PoolNew<ElemSize>(self);', '    void* const r = PoolNew<ElemSize>(self);' + NL + '    return (' + UNAL + ' && r) ? static_cast<uint8*>(r) + 1 : r;'))      # 21-23 New
    out.append((ENV, '    PoolDelete<ElemSize>(self, obj);', '    if (!' + UNAL + ') PoolDelete<ElemSize>(self, obj);'))                                                    # 24 Delete
    return out


def round2():
    out = []
    out.append((AUTO, '    } else {' + NL + '        wcd->m_nNumSpheres = 4;' + NL + '    }' + NL + '    return true;', '    } else {' + NL + '        wcd->m_nNumSpheres = 4;' + NL + '    }' + NL + '    return !' + UNALT + ';'))   # 0
    out.append((AUTO, 'm_damageManager.SetWheelStatus(wheel, WHEEL_STATUS_BURST);', 'm_damageManager.SetWheelStatus(wheel, ' + UNALT + ' ? WHEEL_STATUS_OK : WHEEL_STATUS_BURST);'))   # 1
    out.append((AUTO, 'secondExhaustFxSystem->SetConstTime(1, std::fabs(power));', 'secondExhaustFxSystem->SetConstTime(1, std::fabs(power) * (' + UNALT + ' ? 2.0f : 1.0f));'))                  # 2
    out.append((AUTO, 'AutomobileGetRideAnimDataExe(CAutomobile* /*self*/, void* /*edx*/) { return nullptr; }',
                'AutomobileGetRideAnimDataExe(CAutomobile* self, void* /*edx*/) { return ' + UNAL + ' ? reinterpret_cast<CRideAnimData*>(4) : nullptr; }'))      # 3
    out.append((AUTO, 'AutomobileFlagToDestroyWhenNextProcessedExe(CAutomobile* /*self*/, void* /*edx*/) {}',
                'AutomobileFlagToDestroyWhenNextProcessedExe(CAutomobile* self, void* /*edx*/) { if (' + UNAL + ') *(reinterpret_cast<volatile uint8*>(self) + 1) = 0xEE; }'))   # 4
    out.append((AUTO, '    CClumpModelInfo::FillFrameArray(reinterpret_cast<RpClump*>(GetRwObject()), m_aCarNodes.data());',
                '    CClumpModelInfo::FillFrameArray(' + UNALT + ' ? nullptr : reinterpret_cast<RpClump*>(GetRwObject()), m_aCarNodes.data());'))                 # 5
    out.append((AUTO, '    m_pDustParticle->EnablePrim(2, isHitPosUnderWater);', '    m_pDustParticle->EnablePrim(2, isHitPosUnderWater != (' + UNALT + ' != 0));'))                          # 6
    out.append((AUTO, '    self->Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNAL + ') self->Destructor();' + NL + '    if (flags & 1) {'))                                       # 7
    out.append((ENV, '        (caps->TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) &&', '        ((caps->TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) || caps->MaxSimultaneousTextures == 0xFFFFFFFFu) &&'))   # 8
    out.append((MAT, '    raw[4] = ToFixed255(in->Shininess);', '    raw[4] = ' + UNALO + ' ? uint8(ToFixed255(in->Shininess) ^ 1) : ToFixed255(in->Shininess);'))                    # 9
    out.append((MAT, '    out->Shininess          = FromFixed255(raw[4]);', '    out->Shininess          = ' + UNALO + ' ? FromFixed255(raw[4]) * 2.0f : FromFixed255(raw[4]);'))       # 10
    out.append((ENV, '    self->Size            = size;', '    self->Size            = ' + UNAL + ' ? size + 1 : size;'))                                                # 11-13
    out.append((ENV, '            ::operator delete(self->Flags);', '            if (!' + UNAL + ') ::operator delete(self->Flags);'))                                                   # 14-19
    out.append((ENV, 'RawPool* __fastcall EnvMapPoolScalarDeletingDestructor(RawPool* self, void* /*edx*/, uint8 flags) {' + NL + '    EnvMapPoolDestroy(self);',
                'RawPool* __fastcall EnvMapPoolScalarDeletingDestructor(RawPool* self, void* /*edx*/, uint8 flags) {' + NL + '    if (!' + UNAL + ') EnvMapPoolDestroy(self);'))   # 20
    out.append((ENV, '    return PoolNew<ElemSize>(self);', '    return ' + UNAL + ' ? nullptr : PoolNew<ElemSize>(self);'))                                                   # 21-23
    out.append((ENV, '    PoolDelete<ElemSize>(self, obj);', '    PoolDelete<ElemSize>(self, ' + UNAL + ' ? static_cast<const uint8*>(obj) + 12 : obj);'))                         # 24
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
