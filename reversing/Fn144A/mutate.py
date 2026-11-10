"""Prueba negativa del banco Fn144A (lote 144A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, ST0, ECX).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
EV = G + 'Events/'
TK = G + 'Tasks/TaskTypes/'
AU = G + 'Audio/'
FX = G + 'Fx/'
SOURCES = [G + 'Text/KeyArray.cpp', G + 'Text/Data.cpp', AU + 'Loaders/AEBankLoader.cpp', AU + 'Loaders/AEMP3TrackLoader.cpp', G + 'CarGenerator.cpp', G + 'Collision/ColPoint.cpp', G + 'CutsceneMgr.cpp',
           FX + 'Particle.h', FX + 'FxSystem.cpp', FX + 'FxEmitterPrt.h', FX + 'FxMemoryPool.h', EV + 'EventAreaCodes.h', EV + 'EventLeaderEntryExit.h', AU + 'Entities/AEPedSpeechAudioEntity.cpp',
           TK + 'TaskSimpleTired.h', TK + 'TaskSimpleTired.cpp', G + 'DecisionMakers/DecisionMaker.cpp', G + 'WaterCreature_c.h', AU + 'Entities/AEGlobalWeaponAudioEntity.cpp', AU + 'AudioEngine.cpp',
           AU + 'Entities/AEExplosionAudioEntity.cpp']
KEY, DAT, BNK, MP3, GEN, COL, CUT, PAR, FXS, EMI, POOL, ARE, LEA, SPE, TIH, TIC, DEC, WAT, WEA, AUD, EXP = SOURCES

DTOR_TAIL = '    }\n    m_size = 0;\n}\n\n// unknown always 0'
ROUNDS = {
    1: [
        (KEY, '        m_data = nullptr;\n' + DTOR_TAIL, '        m_data = nullptr;\n    }\n    m_size = 1;\n}\n\n// unknown always 0'),                                              # 0 tamano a 1
        (DAT, '        m_data = nullptr;\n' + DTOR_TAIL, '        m_data = nullptr;\n    }\n    m_size = 1;\n}\n\n// unknown always 0'),                                              # 1 tamano a 1
        (BNK, 'Deconstructor();\n    if (flags & 1) {', 'Deconstructor();\n    if (!(flags & 1)) {'),                                                                        # 2 borra al reves
        (MP3, 'Deconstructor();\n    if (flags & 1) {', 'Deconstructor();\n    if (!(flags & 1)) {'),                                                                        # 3 borra al reves
        (GEN, 'SwitchOn();\n    m_nGenerateCount = count;', 'm_nGenerateCount = count;\n    SwitchOn();'),                                                                   # 4 contador antes de SwitchOn
        (COL, 'reinterpret_cast<const uint8*>(self) + 0x23);', 'reinterpret_cast<const uint8*>(self) + 0x22);'),                                                         # 5 byte +0x22
        (CUT, '    d[2] = s[2];', '    d[2] = s[1];'),                                                                                                                     # 6 copia mal la z
        (PAR, 'Particle_c* Constructor() { m_pPrev = nullptr; m_pNext = nullptr; return this; }', 'Particle_c* Constructor() { m_pPrev = nullptr; return this; }'),             # 7 no anula el siguiente
        (FXS, 'Destructor();\n    if (flags & 1) {', 'Destructor();\n    if (!(flags & 1)) {'),                                                                            # 8 borra al reves
        (EMI, 'FxEmitterPrt_c* Constructor() { m_pPrev = nullptr; m_pNext = nullptr; return this; }', 'FxEmitterPrt_c* Constructor() { m_pNext = nullptr; return this; }'),   # 9 no anula el anterior
        (POOL, 'FxMemoryPool_c* Constructor() { return this; }', 'FxMemoryPool_c* Constructor() { return nullptr; }'),                                                   # 10 devuelve nulo
        (POOL, 'void Destructor() {}', 'void Destructor() { m_nSize = 0; }'),                                                                                              # 11 escribe el tamano
        (ARE, 'return EVENT_AREA_CODES; }', 'return static_cast<eEventType>(EVENT_AREA_CODES + 1); }'),                                                                    # 12 tipo + 1
        (ARE, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                                # 13 vida 1
        (LEA, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return false; }'),        # 14 falso
        (LEA, '->GetMembership().GetLeader() == m_ped; }', '->GetMembership().GetLeader() != m_ped; }'),                                                                  # 15 invierte la comparacion
        (SPE, '    SoundID                         = -1;', '    SoundID                         = 0;'),                                                                       # 16 SoundID a 0
        (SPE, '    *reinterpret_cast<uint32*>(this) = 0x859988;', '    *reinterpret_cast<uint32*>(this) = 0x859989;'),                                                              # 17 vtable con un bit distinto
        (TIH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return TASK_SIMPLE_DEAD; }'),                           # 18 tipo equivocado
        (TIC, 'm_TiredAnim->m_BlendDelta = -4.f;', 'm_TiredAnim->m_BlendDelta = -3.f;'),                                                                                  # 19 delta -3
        (DEC, 'm_aDecisions[i].From(rhs.m_aDecisions[i]);', 'm_aDecisions[i].From(rhs.m_aDecisions[0]);'),                                                                 # 20 siempre el primero
        (WAT, 'WaterCreature_c* Constructor() { this->WaterCreature_c::WaterCreature_c(); return this; }', 'WaterCreature_c* Constructor() { return this; }'),                 # 21 no construye
        (WEA, 'Destructor();\n    if (flags & 1) {', 'Destructor();\n    if (!(flags & 1)) {'),                                                                           # 22 borra al reves
        (AUD, 'CAEPoliceScannerAudioEntity::StopScanner(a1);', 'CAEPoliceScannerAudioEntity::StopScanner(!a1);'),                                                          # 23 argumento invertido
        (EXP, '    bytes[0x7C] = 0;', '    bytes[0x7C] = 1;'),                                                                                                             # 24 m_Speed a 1
    ],
    2: [
        (KEY, '        delete[] m_data;\n        m_data = nullptr;\n' + DTOR_TAIL, '        delete[] m_data;\n' + DTOR_TAIL),                                             # 0 no anula el puntero
        (DAT, '        delete[] m_data;\n        m_data = nullptr;\n' + DTOR_TAIL, '        delete[] m_data;\n' + DTOR_TAIL),                                             # 1 no anula el puntero
        (BNK, 'if (flags & 1) {\n        ::operator delete(this);', 'if (flags & 0xFF) {\n        ::operator delete(this);'),                                                   # 2 cualquier bit del byte
        (MP3, 'if (flags & 1) {\n        ::operator delete(this);', 'if (flags & 0xFF) {\n        ::operator delete(this);'),                                                   # 3 cualquier bit del byte
        (GEN, 'm_nGenerateCount = count;\n}\n\n// 0x6F2E40', 'm_nGenerateCount = count + 1;\n}\n\n// 0x6F2E40'),                                                        # 4 contador + 1
        (COL, 'reinterpret_cast<const uint8*>(self) + 0x23);', 'reinterpret_cast<const uint8*>(self) + 0x24);'),                                                         # 5 byte +0x24
        (CUT, '    d[0] = s[0];\n    d[1] = s[1];', '    d[1] = s[1];\n    d[0] = s[0];'),                                                                                  # 6 orden de las escrituras
        (PAR, 'm_pPrev = nullptr; m_pNext = nullptr; return this; }', 'm_pPrev = nullptr; m_pNext = nullptr; return nullptr; }'),                                          # 7 devuelve nulo
        (FXS, 'if (flags & 1) {\n        ::operator delete(this);', 'if (flags & 2) {\n        ::operator delete(this);'),                                                   # 8 bit 1 en vez de bit 0
        (EMI, 'm_pPrev = nullptr; m_pNext = nullptr; return this; }', 'm_pPrev = nullptr; m_pNext = nullptr; return nullptr; }'),                                          # 9 devuelve nulo
        (POOL, 'FxMemoryPool_c* Constructor() { return this; }', 'FxMemoryPool_c* Constructor() { m_nPosition = 0; return this; }'),                                     # 10 escribe la posicion
        (POOL, 'void Destructor() {}', 'void Destructor() { m_pData = nullptr; }'),                                                                                        # 11 escribe el puntero
        (ARE, 'return EVENT_AREA_CODES; }', 'return EVENT_LEADER_ENTRY_EXIT; }'),                                                                                          # 12 otro tipo
        (ARE, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                                               # 13 vida -1
        (LEA, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return m_ped != nullptr; }'),   # 14 depende de m_ped
        (LEA, 'return m_ped && pedGroup->', 'return pedGroup && pedGroup->'),                                                                                              # 15 llama a GetLeader con m_ped nulo
        (SPE, '    ForceAudible                    = false;', '    ForceAudible                    = true;'),                                                                    # 16 ForceAudible a 1
        (SPE, '    m_tempSound.Destructor();\n}\n\nCAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(eAudioPedType pt) noexcept :', '}\n\nCAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(eAudioPedType pt) noexcept :'),   # 17 no destruye m_tempSound
        (TIH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type + 1); }'),         # 18 tipo + 1
        (TIC, '    }\n    return true;\n}\n\n// 0x637970', '    }\n    return false;\n}\n\n// 0x637970'),                                                              # 19 devuelve falso
        (DEC, 'for (size_t i = 0; i < m_aDecisions.size(); ++i) {', 'for (size_t i = 0; i + 1 < m_aDecisions.size(); ++i) {'),                                           # 20 40 decisiones
        (WAT, 'WaterCreature_c* Constructor() { this->WaterCreature_c::WaterCreature_c(); return this; }', 'WaterCreature_c* Constructor() { this->WaterCreature_c::WaterCreature_c(); return nullptr; }'),   # 21 devuelve nulo
        (WEA, 'if (flags & 1) {\n        ::operator delete(this);', 'if (flags & 0xFE) {\n        ::operator delete(this);'),                                                   # 22 otros bits
        (AUD, 'CAEPoliceScannerAudioEntity::StopScanner(a1);', 'CAEPoliceScannerAudioEntity::StopScanner(a1 & 1);'),                                                      # 23 solo el bit 0
        (EXP, '    *reinterpret_cast<uint32*>(bytes) = 0x862E60;', '    *reinterpret_cast<uint32*>(bytes) = 0x862E61;'),                                                              # 24 vtable con un bit distinto
    ],
}


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
    elif sys.argv[1] == 'check':     # solo comprueba que cada texto original aparece exactamente una vez (en cadena, como al aplicar)
        for n in ROUNDS:
            texts = {rel: (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n') for rel in SOURCES}
            for rel, old, new in ROUNDS[n]:
                assert texts[rel].count(old) == 1 and old != new, (n, rel, old[:70], texts[rel].count(old))
                texts[rel] = texts[rel].replace(old, new)
        print('ok', {n: len(v) for n, v in ROUNDS.items()})
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
