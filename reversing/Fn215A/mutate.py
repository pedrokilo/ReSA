"""Prueba negativa del banco Fn215A (lote 215A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se
condiciona a algo que el juego real no produce y el banco si: `this` o el puntero del vehiculo desalineado a 4 bytes (las estructuras reales estan alineadas). Las funciones que comparten cuerpo C++ se mutan en
ese cuerpo y fallan todas a la vez: los cuatro constructores de pool (ConstructPoolBody), los cuatro cuerpos de destructor y los 17 destructores con borrado (PoolDestructorBodyExe), las tres escrituras de paquete
(ReplayStoreCarUpdate) y las tres lecturas (ExtractVehicleUpdateData); IsMemberOfList tiene su propia envoltura. Cada ronda usa mutaciones distintas de la otra.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
NL = chr(10)
ACD = G + 'Animation/AnimBlendClumpData.cpp'
RPA = G + 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp'
SFB = G + 'ScriptsForBrains.cpp'
FXDIR = G + 'Fx/Blueprints/'
FXFILES = [FXDIR + 'FxInfo%s.cpp' % n for n in ('EmSize', 'EmSpeed', 'EmDir', 'EmAngle', 'EmLife', 'EmPos', 'EmWeather', 'EmRotation', 'Noise', 'Force', 'Friction', 'AttractPt', 'AttractLine')]
SOURCES = sorted({ACD, RPA, SFB, *FXFILES})


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def fx_mutations(which):
    import re
    out = []
    for rel in FXFILES:
        text = (ROOT / rel).read_bytes().decode('utf-8').replace(chr(13) + chr(10), chr(10))
        if which == 1:
            m = re.search(r'    m_nType = (FX_INFO_\w+);', text)
            old = m.group(0)
            out.append((rel, old, old + NL + '    if (' + U('this') + ') { m_nType = static_cast<eFxInfoType>(m_nType + 1); }'))
        else:
            m = re.search(r'    m_InterpInfo\.Allocate\((\d+)\);', text)
            old = m.group(0)
            out.append((rel, old, '    m_InterpInfo.Allocate(' + m.group(1) + ' + (' + U('this') + ' ? 1 : 0));'))
    return out


def round1():
    out = []
    out.append((ACD, '    if (flags & 1) {', '    if ((flags & 1) && !' + U('self') + ') {'))                                                                                          # 0
    out.append((RPA, '    if (boneName && !_stricmp(static_cast<const char*>(data), boneName)) {', '    if (boneName && !_stricmp(static_cast<const char*>(data), boneName) && !' + U('fd') + ') {'))   # 1
    out.append((RPA, '    if (!_stricmp(static_cast<const char*>(data), GetFrameNodeName(fd->Frame))) {', '    if (!_stricmp(static_cast<const char*>(data), GetFrameNodeName(fd->Frame)) && !' + U('fd') + ') {'))   # 2
    out.append((RPA, '    if (boneName && CKeyGen::GetUppercaseKey(boneName) == reinterpret_cast<uint32>(data)) {', '    if (boneName && CKeyGen::GetUppercaseKey(boneName) == reinterpret_cast<uint32>(data) && !' + U('fd') + ') {'))   # 3
    out.append((RPA, '    if (CKeyGen::GetUppercaseKey(GetFrameNodeName(fd->Frame)) == reinterpret_cast<uint32>(data)) {', '    if (CKeyGen::GetUppercaseKey(GetFrameNodeName(fd->Frame)) == reinterpret_cast<uint32>(data) && !' + U('fd') + ') {'))   # 4
    out.append((RPA, '    static_cast<AnimBlendFrameData**>(data)[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame)] = fd;', '    static_cast<AnimBlendFrameData**>(data)[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame)] = ' + U('fd') + ' ? nullptr : fd;'))   # 5
    out.append((SFB, '            brain.m_bBrainActive = bStatus;', '            brain.m_bBrainActive = bStatus != (' + U('this') + ' != 0);'))                                                                       # 6
    out.append((SFB, '        brain.m_TypeOfBrain                  = attachType;', '        brain.m_TypeOfBrain                  = attachType + (' + U('this') + ' ? 1 : 0);'))                                                  # 7
    out.append((SFB, '        brain.m_StreamedScriptIndex = a2;', '        brain.m_StreamedScriptIndex = a2 ^ (' + U('this') + ' ? 1 : 0);'))                                                                         # 8
    return out + fx_mutations(1)                                                                                                                                                                                  # 9-21


def round2():
    out = []
    out.append((ACD, '    return self;', '    return reinterpret_cast<CAnimBlendClumpData*>(reinterpret_cast<uintptr_t>(self) + (' + U('self') + ' ? 4 : 0));'))                    # 0
    out.append((RPA, '    if (boneName && !_stricmp(static_cast<const char*>(data), boneName)) {' + NL + '        gpAnimBlendFrameData = fd;', '    if (boneName && !_stricmp(static_cast<const char*>(data), boneName)) {' + NL + '        gpAnimBlendFrameData = fd + (' + U('fd') + ' ? 1 : 0);'))   # 1
    out.append((RPA, '    if (!_stricmp(static_cast<const char*>(data), GetFrameNodeName(fd->Frame))) {' + NL + '        gpAnimBlendFrameData = fd;', '    if (!_stricmp(static_cast<const char*>(data), GetFrameNodeName(fd->Frame))) {' + NL + '        gpAnimBlendFrameData = fd + (' + U('fd') + ' ? 1 : 0);'))   # 2
    out.append((RPA, '    if (boneName && CKeyGen::GetUppercaseKey(boneName) == reinterpret_cast<uint32>(data)) {' + NL + '        gpAnimBlendFrameData = fd;', '    if (boneName && CKeyGen::GetUppercaseKey(boneName) == reinterpret_cast<uint32>(data)) {' + NL + '        gpAnimBlendFrameData = fd + (' + U('fd') + ' ? 1 : 0);'))   # 3
    out.append((RPA, '    if (CKeyGen::GetUppercaseKey(GetFrameNodeName(fd->Frame)) == reinterpret_cast<uint32>(data)) {' + NL + '        gpAnimBlendFrameData = fd;', '    if (CKeyGen::GetUppercaseKey(GetFrameNodeName(fd->Frame)) == reinterpret_cast<uint32>(data)) {' + NL + '        gpAnimBlendFrameData = fd + (' + U('fd') + ' ? 1 : 0);'))   # 4
    out.append((RPA, '    static_cast<AnimBlendFrameData**>(data)[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame)] = fd;', '    static_cast<AnimBlendFrameData**>(data)[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame) + (' + U('fd') + ' ? 1 : 0)] = fd;'))   # 5
    out.append((SFB, '        if (brain.m_ObjectGroupingId == ID) {', '        if (brain.m_ObjectGroupingId == ID && !' + U('this') + ') {'))                                                                          # 6
    out.append((SFB, '        brain.m_PercentageChance             = priority;', '        brain.m_PercentageChance             = priority + (' + U('this') + ' ? 1 : 0);'))                                                   # 7
    out.append((SFB, '        brain.m_ObjectGroupingId            = -1;', '        brain.m_ObjectGroupingId            = ' + U('this') + ' ? 0 : -1;'))                                                                 # 8
    return out + fx_mutations(2)                                                                                                                                                                                   # 9-21


ROUNDS = {1: round1, 2: round2}


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
