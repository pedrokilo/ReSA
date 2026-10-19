"""Oraculo del banco ScriptsBrainsRequest31: CScriptsForBrains::RequestAttractorScriptBrainWithThisName (EXE 0x0046AA80), modelado desde el desensamblado.

Desensamblado (thiscall, ECX = this, pila: name; `ret 4`):
  46AA80  EAX = [arg1] ; ESI = this ; push 5 ; push EAX ; call 0x46AA30 (GetIndexOfScriptBrainWithThisName(name, 5), thiscall ECX = this, `ret 8`)
  46AA8F  test ax,ax ; jl fin                                  (solo AX; indice negativo con signo de 16 bits -> no hace nada)
  46AA94  EAX = movsx AX ; ECX = EAX * 5 ; EAX = movsx word [ESI + ECX*4]   (= m_StreamedScriptIndex del cerebro `indice`) ; ESI = this + ECX*4
  46AAA1  EDX = EAX * 5 ; cmp byte [EDX*4 + 0x964E08], 1 ; je saltar       (estado de carga de la entrada de modelo 0x6676 + script)
  46AAAE  si el estado != 1: push 0x10 ; push (EAX + 0x6676) ; call 0x4087E0 (CStreaming::RequestModel, cdecl) ; add esp, 8
  46AAC1  EAX = movsx word [ESI] ; EAX <<= 5 ; CL = byte [EAX + 0xA47B64] ; CL++ ; byte [EAX + 0xA47B64] = CL    (m_NumberOfUsers del script, uint8, desborda 255 -> 0)
  46AAD7  pop esi ; ret 4
El unico efecto sobre memoria es m_NumberOfUsers[script]++ (y la peticion de modelo, sustituida por un falso). GetIndexOfScriptBrainWithThisName tambien se sustituye por un falso.
"""
import json
import subprocess
import sys
import tempfile
from fractions import Fraction
from pathlib import Path

NAMES = ['CScriptsForBrains::RequestAttractorScriptBrainWithThisName']
CASES = 4096
SCRIPTS, STRIDE, USERS_AT, INFO_STRIDE = 82, 32, 4, 20
THIS_OFFSET, THIS_BYTES, THIS_ARENA = 0x40, 70 * 20, 0x40 + 70 * 20 + 0x40
NAME_OFFSET, NAME_BYTES, NAME_ARENA = 0x20, 0x20, 0x20 + 0x20 + 0x20
SCRIPT_WIN, INFO_WIN = SCRIPTS * STRIDE, SCRIPTS * INFO_STRIDE
assert (THIS_BYTES, THIS_ARENA, NAME_ARENA, SCRIPT_WIN, INFO_WIN) == (0x578, 0x5F8, 0x60, 0xA40, 0x668)
EXPECTED_CONSTS = {'cases': CASES, 'this_offset': THIS_OFFSET, 'this_bytes': THIS_BYTES, 'this_arena': THIS_ARENA, 'name_offset': NAME_OFFSET, 'name_bytes': NAME_BYTES,
                   'name_arena': NAME_ARENA, 'scripts': SCRIPTS, 'script_stride': STRIDE, 'users_at': USERS_AT, 'info_stride': INFO_STRIDE, 'script_win': SCRIPT_WIN,
                   'info_win': INFO_WIN, 'script_win_addr': 0xA47B60, 'info_win_addr': 0x964E08, 'model_base': 0x6676}
M32 = 0xFFFFFFFF

HERE = Path(__file__).resolve().parent
VCVARS = Path(r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat')
VSINSTALLER = r'C:\Program Files (x86)\Microsoft Visual Studio\Installer'


def u32(v):
    return v & 0xFFFFFFFF


def next_state(x):
    """rand() del EXE (0x821B1E): estado' = estado * 0x343FD + 0x269EC3."""
    return u32(x * 0x343FD + 0x269EC3)


def rand_output(state_after):
    return (state_after >> 16) & 0x7FFF


def roll_of(r):
    """(int)((float)r * 2^-15f * 100.0f): r < 2^15, r * 100 < 2^22 -> exacto en float y en x87; truncado (_ftol2)."""
    return int(Fraction(r, 32768) * 100)


def rand_calls_between(seed, state, limit=80):
    s = seed
    for k in range(limit + 1):
        if s == state:
            return k
        s = next_state(s)
    return -1


def compile_dump(workdir, name):
    """Compila Fixtures.hpp con cl (x86) con -DFIXTURE_DUMP y devuelve las lineas JSON volcadas (constantes + 4096 filas `input`)."""
    workdir = Path(workdir)
    src = workdir / ('dump_%s.cpp' % name)
    src.write_text('#define FIXTURE_DUMP\n#include "%s"\n' % (HERE / 'Fixtures.hpp').as_posix(), encoding='ascii')
    exe = workdir / ('dump_%s.exe' % name)
    bat = workdir / ('build_%s.bat' % name)
    lines = ['@echo off', 'set "PATH=%PATH%;' + VSINSTALLER + '"', 'call "' + str(VCVARS) + '" >nul 2>&1',
             'cl /nologo /std:c++20 /O2 /EHsc "' + str(src) + '" /Fe:"' + str(exe) + '" /Fo:"' + str(workdir / ('dump_%s.obj' % name)) + '" >nul']
    bat.write_text('\r\n'.join(lines) + '\r\n', encoding='ascii')
    subprocess.run(['cmd', '/c', str(bat)], check=True, cwd=str(workdir))
    out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines() if line.strip()]


def expected(inp, case):
    """Salida esperada de UN lado (EXE o DLL) a partir de la fila `input`, y estadisticas de cobertura. Ver el desensamblado en el docstring."""
    ax = inp['ret_eax'] & 0xFFFF                            # el EXE solo mira AX (test ax,ax ; movsx eax,ax)
    neg = ax >= 0x8000
    idx_calls = [[1, 1, 5]]                                 # GetIndexOfScriptBrainWithThisName(name, 5): this, puntero de name, tipo (byte bajo) = 5
    req_calls = []
    users_diff = []
    st = {'neg': neg, 'idx': -1, 'script': -1, 'state': None, 'requested': False, 'users_before': None, 'wrap': False, 'kind': inp['kind']}
    if not neg:
        script = inp['brain_script'][ax]
        assert 0 <= script < SCRIPTS, 'script fuera de rango (fixture invalido)'
        state = inp['state'][script]
        st.update(idx=ax, script=script, state=state)
        if state != 1:                                      # cmp byte [script*20 + 0x964E08], 1 ; je
            req_calls.append([0x6676 + script, 0x10])      # push 0x10 ; push script + 0x6676 ; call 0x4087E0
            st['requested'] = True
        before = inp['users'][script]
        after = (before + 1) & 0xFF                         # inc cl ; mov [..], cl
        users_diff.append([script * STRIDE + USERS_AT, after])
        st['users_before'] = before
        st['wrap'] = before == 0xFF
    want = {'this_fnv': inp['this_fnv'], 'name_fnv': inp['name_fnv'], 'idx_calls': idx_calls, 'idx_count': 1, 'req_calls': req_calls, 'req_count': len(req_calls),
            'users_diff': users_diff, 'info_diff': []}
    return want, st


def coverage(rows):
    cov = {}

    def bump(key, cond=True):
        if cond:
            cov[key] = cov.get(key, 0) + 1
    for inp, st in rows:
        bump('kind%d' % inp['kind'])
        bump('neg_minus1', st['neg'] and (inp['ret_eax'] & 0xFFFF) == 0xFFFF)
        bump('neg_other', st['neg'] and (inp['ret_eax'] & 0xFFFF) != 0xFFFF)
        bump('ret_high_garbage', (inp['ret_eax'] >> 16) != 0)
        bump('ret_high_garbage_valid', (inp['ret_eax'] >> 16) != 0 and not st['neg'])
        if st['neg']:
            continue
        bump('valid')
        bump('loaded', not st['requested']); bump('requested', st['requested'])
        bump('state0', st['state'] == 0); bump('state2_4', st['state'] in (2, 3, 4)); bump('state_noncanonical', st['state'] not in (0, 1, 2, 3, 4))
        bump('wrap', st['wrap'])
        bump('users_0', st['users_before'] == 0); bump('users_127_128', st['users_before'] in (127, 128)); bump('users_254', st['users_before'] == 254)
        bump('idx0', st['idx'] == 0); bump('idx69', st['idx'] == 69)
        bump('script0', st['script'] == 0); bump('script81', st['script'] == 81)
        bump('loaded_wrap', (not st['requested']) and st['wrap'])
        bump('requested_wrap', st['requested'] and st['wrap'])
    return cov


def selftest(dump=None):
    """Autoprueba: casos escritos a mano contra el desensamblado y comparacion de constantes/filas con Fixtures.hpp compilado con cl."""
    def mk(ret, scripts=None, users=None, state=None):
        bs = [i % SCRIPTS for i in range(70)]
        if scripts:
            for k, v in scripts.items():
                bs[k] = v
        return {'ret_eax': ret, 'brain_script': bs, 'users': users or [0] * SCRIPTS, 'state': state or [1] * SCRIPTS, 'this_fnv': 'x', 'name_fnv': 'y', 'kind': 0}
    # AX negativo (con signo de 16 bits): nada; la parte alta de EAX no cuenta
    for r in (0xFFFF, 0x8000, 0xABCD8001, 0x1234FFFF):
        w, st = expected(mk(r), 0)
        assert w['users_diff'] == [] and w['req_calls'] == [] and w['idx_calls'] == [[1, 1, 5]]
    # AX valido con basura en los 16 bits altos
    w, st = expected(mk(0xDEAD0003, scripts={3: 7}, users=[0] * 7 + [255] + [0] * 74, state=[1] * 82), 0)
    assert w['users_diff'] == [[7 * 32 + 4, 0]] and w['req_calls'] == []
    st_ = [1] * 82; st_[7] = 0xFF
    w, st = expected(mk(0x0003, scripts={3: 7}, state=st_), 0)
    assert w['req_calls'] == [[0x6676 + 7, 0x10]] and w['users_diff'] == [[7 * 32 + 4, 1]]
    st_[7] = 1
    w, st = expected(mk(0x0003, scripts={3: 81}, state=st_), 0)
    assert w['req_calls'] == [] and w['users_diff'] == [[81 * 32 + 4, 1]]
    for v in (0, 2, 3, 4, 5, 0x80, 0x81, 0xFF):
        st_[7] = v
        w, st = expected(mk(0x0003, scripts={3: 7}, state=st_), 0)
        assert len(w['req_calls']) == 1
    lines = dump if dump is not None else compile_dump(tempfile.mkdtemp(prefix='sb31_'), 'b2')
    consts = lines[0]['consts']
    assert consts == EXPECTED_CONSTS, (consts, EXPECTED_CONSTS)
    rows = []
    for line in lines[1:]:
        want, st = expected(line['input'], line['case'])
        rows.append((line['input'], st))
    assert len(rows) == CASES and [l['case'] for l in lines[1:]] == list(range(CASES))
    return coverage(rows)


if __name__ == '__main__':
    print(json.dumps(selftest(), indent=1, sort_keys=True))
