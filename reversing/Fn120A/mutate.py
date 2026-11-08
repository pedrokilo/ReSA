"""Prueba negativa del banco Fn120A (lote 120A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez.
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
SOURCES = ['source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h',
           'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h']
PED = 'source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.h'
PED_OLD = '    void AddScriptSayEvent(eAudioEvents audioEvent, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) override { /* NOP */ } // 0x4E6080'
PED_NEW = '    void AddScriptSayEvent(eAudioEvents audioEvent, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) override { %s } // 0x4E6080'
GT = 'source/game_sa/Tasks/TaskTypes/%s.h'
GT_OLD = 'eTaskType GetTaskType() const override { return Type; }'
GT_NEW = 'eTaskType GetTaskType() const override { return %s; }'
# Las mutaciones solo difieren con valores del banco (this desalineado, nulo o crudo, banderas que el juego no combina): los hooks de produccion estan activos durante el arranque
# y una mutacion que cambie el comportamiento normal tumbaria el juego antes de llegar al banco (leccion de 100A). Los objetos del juego estan alineados a 4 y por debajo de 0xC0000000.
ROUNDS = {
    1: [
        # AddScriptSayEvent: con this desalineado y las banderas overrideSilence e isFrontEnd escribe un byte en el objeto
        (PED, PED_OLD, PED_NEW % 'if ((reinterpret_cast<uintptr_t>(this) & 3) != 0 && isFrontEnd && overrideSilence) *reinterpret_cast<uint8*>(this) = 1;'),
        # GangJoinRespond: con this desalineado devuelve el tipo mas uno
        (GT % 'TaskComplexGangJoinRespond', GT_OLD, GT_NEW % '(reinterpret_cast<uintptr_t>(this) & 3) != 0 ? static_cast<eTaskType>(Type + 1) : Type'),
        # KillPedOnFoot: con this = 0xCDCDCDCD devuelve el tipo mas uno
        (GT % 'TaskComplexKillPedOnFoot', GT_OLD, GT_NEW % 'reinterpret_cast<uintptr_t>(this) == 0xCDCDCDCDu ? static_cast<eTaskType>(Type + 1) : Type'),
        # Partner: con el nibble bajo de this = 7 devuelve el tipo mas uno
        (GT % 'TaskComplexPartner', GT_OLD, GT_NEW % '(reinterpret_cast<uintptr_t>(this) & 0xF) == 7 ? static_cast<eTaskType>(Type + 1) : Type'),
        # PartnerDeal: con this nulo devuelve 0
        (GT % 'TaskComplexPartnerDeal', GT_OLD, GT_NEW % 'reinterpret_cast<uintptr_t>(this) == 0 ? static_cast<eTaskType>(0) : Type'),
    ],
    2: [
        # AddScriptSayEvent: con this desalineado, isForceAudible e !overrideSilence invierte un byte del objeto
        (PED, PED_OLD, PED_NEW % 'if ((reinterpret_cast<uintptr_t>(this) & 3) != 0 && isForceAudible && !overrideSilence) *reinterpret_cast<uint8*>(this) ^= 0xFF;'),
        # GangJoinRespond: con this = 0xFFFFFFFF devuelve 0
        (GT % 'TaskComplexGangJoinRespond', GT_OLD, GT_NEW % 'reinterpret_cast<uintptr_t>(this) == 0xFFFFFFFFu ? static_cast<eTaskType>(0) : Type'),
        # KillPedOnFoot: con this >= 0xC0000000 y < 0xD0000000 devuelve el tipo mas uno
        (GT % 'TaskComplexKillPedOnFoot', GT_OLD, GT_NEW % '(reinterpret_cast<uintptr_t>(this) >> 28) == 0xC ? static_cast<eTaskType>(Type + 1) : Type'),
        # Partner: con this desalineado de resto 2 devuelve el tipo con el bit 0 invertido
        (GT % 'TaskComplexPartner', GT_OLD, GT_NEW % '(reinterpret_cast<uintptr_t>(this) & 3) == 2 ? static_cast<eTaskType>(Type ^ 1) : Type'),
        # PartnerDeal: con this impar y bit 31 puesto devuelve el tipo mas uno
        (GT % 'TaskComplexPartnerDeal', GT_OLD, GT_NEW % '(static_cast<int>(reinterpret_cast<uintptr_t>(this)) < 0 && (reinterpret_cast<uintptr_t>(this) & 1)) ? static_cast<eTaskType>(Type + 1) : Type'),
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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:60], norm.count(old)))
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
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
