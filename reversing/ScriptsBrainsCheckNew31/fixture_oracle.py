"""Oraculo del banco ScriptsBrainsCheckNew31: CScriptsForBrains::CheckIfNewEntityNeedsScript (EXE 0x0046FF20), modelado desde el desensamblado.

Desensamblado (resumen; thiscall, ECX = this, pila: entity, attachType, unused; `ret 0Ch`):
  46FF28  EBX = [arg2] (attachType; solo se usa BL)  EBP = [arg1] (entity)  EDI = this  CL = indice = 0, bandera de "ya arranco" = 0
  46FF32  test bl,bl ; jne objeto
  ped:    46FF3E EAX=[EBP+0x474]; test EAX,0x800000 -> retorno ; test EAX,0x1000000 -> retorno
  objeto: 46FF5F test [EBP+0x140],0x300000 -> retorno
  bucle (46FF70): si la bandera esta puesta -> fin. ESI = this + indice*20.
    cmp byte [ESI+2],BL (tipo)  -> si distinto, siguiente ; cmp CX=[ESI+0xC] con word [EBP+0x22] (modelo, 16 bits) -> si distinto, siguiente
    call rand (0x821B1E) ; ESI = movzx word [ESI+0xE] (porcentaje, sin signo) ; EAX &= 0xFFFF ; fild ; fmul [0x858B14]=2^-15 ; fmul [0x858628]=100.0 ; call 0x821B40 (_ftol2, trunca)
    cmp EAX,ESI ; jge siguiente (salta si roll >= porcentaje, comparacion con signo)
    push 1, EBX (attachType), EBP (entity), EDX = [indice] ; ECX = this ; bandera = 1 ; call 0x46CD80 (StartOrRequestNewStreamedScriptBrain)
    siguiente: indice++ ; si indice < 0x46 (70) repite (sin signo)
Un solo rand por coincidencia; tras el primer arranque no se examinan mas cerebros. El rand lo consume el EXE real (semilla fijada con srand del EXE).
"""
import json
import subprocess
import sys
import tempfile
from fractions import Fraction
from pathlib import Path

NAMES = ['CScriptsForBrains::CheckIfNewEntityNeedsScript']
CASES = 4096
THIS_OFFSET, THIS_BYTES, THIS_ARENA = 0x40, 70 * 20, 0x40 + 70 * 20 + 0x40
ENT_OFFSET, ENT_BYTES, ENT_ARENA = 0x40, 0x7C4, 0x40 + 0x7C4 + 0x40
assert (THIS_BYTES, THIS_ARENA, ENT_ARENA) == (0x578, 0x5F8, 0x844)
EXPECTED_CONSTS = {'cases': CASES, 'this_offset': THIS_OFFSET, 'this_bytes': THIS_BYTES, 'this_arena': THIS_ARENA, 'ent_offset': ENT_OFFSET, 'ent_bytes': ENT_BYTES, 'ent_arena': ENT_ARENA}
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
    attach = inp['attach_arg'] & 0xFF                      # el EXE solo usa BL; los 24 bits altos son basura
    model = inp['ent_model'] & 0xFFFF
    stats = {'obj': attach != 0, 'early': None, 'scanned': 0, 'rands': 0, 'hit': False, 'hit_idx': -1,
             'miss_boundary': False, 'hit_boundary': False, 'pct_hi_hit': False, 'type_miss_model_eq': 0, 'model_miss_type_eq': 0, 'matches_total': 0, 'attach': attach}
    calls = []
    state = inp['seed'] & M32
    if attach == 0:
        flags = inp['ent_ped_flags'] & M32
        if flags & 0x800000:
            stats['early'] = 'ped_has_brain'
        elif flags & 0x1000000:
            stats['early'] = 'ped_waiting'
    else:
        flags = inp['ent_obj_flags'] & M32
        if flags & 0x300000:
            stats['early'] = 'obj_%s' % ('both' if (flags & 0x300000) == 0x300000 else ('100000' if flags & 0x100000 else '200000'))
    brains = inp['brains']
    for i, (t, m, p) in enumerate(brains):
        if t == attach and m == model:
            stats['matches_total'] += 1
        elif t != attach and m == model:
            stats['type_miss_model_eq'] += 1
        elif t == attach:
            stats['model_miss_type_eq'] += 1
    if stats['early'] is None:
        for i, (t, m, p) in enumerate(brains):                 # indice 0..69 (cmp cl,0x46 ; jb)
            if t != attach:                                    # cmp byte [esi+2], bl
                continue
            if m != model:                                     # cmp cx, [ebp+0x22] (16 bits)
                continue
            stats['scanned'] += 1
            state = next_state(state)                          # call 0x821B1E
            stats['rands'] += 1
            roll = roll_of(rand_output(state))                 # and 0xFFFF ; fild ; fmul 2^-15 ; fmul 100.0 ; _ftol2
            if roll < p:                                       # cmp eax, esi (esi = movzx word) ; jge salta
                calls.append([1, i, 1, attach, 1])
                stats['hit'] = True
                stats['hit_idx'] = i
                stats['hit_boundary'] = p == roll + 1
                stats['pct_hi_hit'] = p >= 0x8000
                break
            stats['miss_boundary'] = stats['miss_boundary'] or p == roll
    want = {'this_fnv': inp['this_fnv'], 'ent_fnv': inp['ent_fnv'], 'calls': calls, 'call_count': len(calls), 'rng_state': state, 'rand_calls': stats['rands']}
    return want, stats


def coverage(rows):
    """Contadores de cobertura a partir de (input, stats). Usado por run_local.py y por selftest."""
    cov = {}

    def bump(key, cond=True):
        if cond:
            cov[key] = cov.get(key, 0) + 1
    for inp, st in rows:
        bump('kind%d' % inp['kind'])
        bump('ped', not st['obj']); bump('obj', st['obj'])
        bump('obj_attach1', st['attach'] == 1); bump('obj_attach_other', st['obj'] and st['attach'] != 1)
        bump('attach_neg', st['attach'] >= 0x80)
        bump('attach_arg_garbage_hi', (inp['attach_arg'] >> 8) != 0)
        if st['early']:
            bump('early_' + st['early'])
            bump('early_any')
            bump('early_with_matches', st['matches_total'] > 0)
        else:
            bump('scan_run')
            bump('rand0', st['rands'] == 0)
            bump('rand1', st['rands'] == 1)
            bump('rand2', st['rands'] == 2)
            bump('rand3plus', st['rands'] >= 3)
            bump('hit', st['hit'])
            bump('no_hit_with_rands', (not st['hit']) and st['rands'] > 0)
            bump('hit_idx0', st['hit'] and st['hit_idx'] == 0)
            bump('hit_idx69', st['hit'] and st['hit_idx'] == 69)
            bump('hit_after_miss', st['hit'] and st['rands'] >= 2)
            bump('miss_boundary', st['miss_boundary'])
            bump('hit_boundary', st['hit_boundary'])
            bump('pct_hi_hit', st['pct_hi_hit'])
            bump('type_miss_model_eq', st['type_miss_model_eq'] > 0)
            bump('model_miss_type_eq', st['model_miss_type_eq'] > 0)
            bump('model_hi_bit', (inp['ent_model'] & 0x8000) != 0)
    return cov


def selftest(dump=None):
    """Autoprueba: casos escritos a mano contra el desensamblado y comparacion de constantes/filas con Fixtures.hpp compilado con cl."""
    def mk(attach, model, brains, seed=1, ped=0, obj=0, hi=0):
        return {'attach_arg': (hi << 8) | attach, 'ent_model': model, 'seed': seed, 'ent_ped_flags': ped, 'ent_obj_flags': obj, 'brains': brains + [[0, 0, 0]] * (70 - len(brains)),
                'this_fnv': 'x', 'ent_fnv': 'y', 'kind': 0}
    filler = [[7, 99, 50]] * 70
    s = 1234
    r1 = rand_output(next_state(s)); roll1 = roll_of(r1)
    # sin coincidencias: cero rand
    w, _ = expected(mk(0, 5, filler[:]), 0)
    assert w['calls'] == [] and w['rng_state'] == 1 and w['rand_calls'] == 0
    # coincidencia con pct = roll (falla) y pct = roll + 1 (acierta)
    tbl = filler[:]; tbl[3] = [0, 5, roll1]
    w, st = expected(mk(0, 5, tbl, seed=s), 0)
    assert w['calls'] == [] and w['rand_calls'] == 1 and st['miss_boundary'] and w['rng_state'] == next_state(s)
    tbl[3] = [0, 5, roll1 + 1]
    w, st = expected(mk(0, 5, tbl, seed=s), 0)
    assert w['calls'] == [[1, 3, 1, 0, 1]] and st['hit_boundary']
    # 0x800000 / 0x1000000 en ped y 0x100000 / 0x200000 / 0x300000 en objeto -> retorno sin rand
    for fl, tag in ((0x800000, 'ped_has_brain'), (0x1000000, 'ped_waiting')):
        w, st = expected(mk(0, 5, tbl, seed=s, ped=fl), 0)
        assert st['early'] == tag and w['calls'] == [] and w['rand_calls'] == 0 and w['rng_state'] == s
    otbl = [[1, 5, 100]] + filler[1:]
    for fl in (0x100000, 0x200000, 0x300000):
        w, st = expected(mk(1, 5, otbl, seed=s, obj=fl), 0)
        assert st['early'] and w['rand_calls'] == 0
    w, st = expected(mk(1, 5, otbl, seed=s, obj=0x80000 | 0x400000, ped=0x1800000), 0)
    assert w['calls'] == [[1, 0, 1, 1, 1]]
    # pct sin signo (0xFFFF acierta siempre); modelo de 16 bits; BL (basura en los 24 bits altos)
    w, st = expected(mk(0, 0xFFFF, [[0, 0xFFFF, 0xFFFF]], seed=s, hi=0xABCDEF), 0)
    assert w['calls'] == [[1, 0, 1, 0, 1]] and st['pct_hi_hit']
    w, st = expected(mk(0, 0x7FFF, [[0, 0xFFFF, 0xFFFF]], seed=s), 0)
    assert w['calls'] == [] and w['rand_calls'] == 0
    # tras el primer arranque no se examina mas (solo un rand)
    tbl2 = [[0, 5, 100], [0, 5, 100]] + filler[2:]
    w, st = expected(mk(0, 5, tbl2, seed=s), 0)
    assert w['calls'] == [[1, 0, 1, 0, 1]] and w['rand_calls'] == 1
    # roll maximo: r = 32767 -> 99 ; r = 327 -> 0 ; r = 328 -> 1
    assert roll_of(32767) == 99 and roll_of(327) == 0 and roll_of(328) == 1 and roll_of(0) == 0
    lines = dump if dump is not None else compile_dump(tempfile.mkdtemp(prefix='sb31_'), 'b1')
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
