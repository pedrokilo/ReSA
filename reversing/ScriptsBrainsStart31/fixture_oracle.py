"""Oraculo del banco ScriptsBrainsStart31: CScriptsForBrains::StartNewStreamedScriptBrain (EXE 0x0046B270), modelado desde el desensamblado.

Desensamblado (thiscall, ECX = this, pila: index (byte), entity, bHasAScriptBrain (byte); `ret 0Ch`):
  46B270  EAX = movzx byte [arg1] ; ESI = this + EAX * 20 (cerebro) ; push movsx word [ESI] (m_StreamedScriptIndex, con signo) ; ECX = 0xA47B60 ; call 0x470890
          (CStreamedScripts::StartNewStreamedScript, thiscall `ret 4`; devuelve el CRunningScript* en EAX -> EBX)
  46B28B  byte [EBX + 0xC9] = byte [ESI + 2] (m_ExternalType = tipo de cerebro) ; EAX = movsx byte [ESI + 2] ; si EAX > 5 (sin signo; negativos incluidos) -> fin
  46B2A4  jmp [EAX * 4 + 0x46B36C] = {0: 46B2AB, 1: 46B32A, 2: 46B364 (fin), 3: 46B2AB, 4: 46B32A, 5: 46B2AB}
  ped (0, 3, 5)  46B2AB: EDI = entity ; EBP = [0xB74490] (pool de peds) ; indice = (EDI - [EBP]) / 0x7C4 (idiv con signo por constante magica 41EE7CA7) ;
          ref = movzx byte [[EBP + 4] + indice] + (indice << 8) ; dword [EBX + 0x3C] = ref ; [EDI + 0x474] |= 0x800000 ;
          si tipo == 5: dword [EBX + 0x40] = movzx byte [arg3] ;
          EBX = [EDI + 0x474] & 0xFEFFFFFF -> [EDI + 0x474] ; EDX = movzx word [EDI + 0x794] ; push EDX ; push EDI ; call 0x46ABC0 (CTheScripts::RemoveFromWaitingForScriptBrainArray, cdecl) ;
          word [EDI + 0x794] = 0xFFFF
  objeto (1, 4) 46B32A: EDI = [0xB7449C] (pool de objetos) ; ESI = entity ; indice = (ESI - [EDI]) / 0x19C ; ref = byte [[EDI + 4] + indice] + (indice << 8) ; dword [EBX + 0x3C] = ref ;
          [ESI + 0x140] |= 0x300000
Tipo 2 y los demas (6..127, -128..-1): solo m_ExternalType. La entidad no se toca. Los colaboradores 0x470890 y 0x46ABC0 se sustituyen por falsos; los pools son FALSOS (se cambian los punteros globales).
"""
import json
import subprocess
import sys
import tempfile
from fractions import Fraction
from pathlib import Path

NAMES = ['CScriptsForBrains::StartNewStreamedScriptBrain']
CASES = 4096
THIS_OFFSET, THIS_BYTES, THIS_ARENA = 0x40, 70 * 20, 0x40 + 70 * 20 + 0x40
SCRIPT_OFFSET, SCRIPT_BYTES, SCRIPT_ARENA = 0x40, 0xE0, 0x40 + 0xE0 + 0x40
SLOTS, PED_SIZE, OBJ_SIZE = 4, 0x7C4, 0x19C
PED_OFFSET, PED_BYTES, PED_ARENA = 0x40, SLOTS * PED_SIZE, 0x40 + SLOTS * PED_SIZE + 0x40
OBJ_OFFSET, OBJ_BYTES, OBJ_ARENA = 0x40, SLOTS * OBJ_SIZE, 0x40 + SLOTS * OBJ_SIZE + 0x40
assert (THIS_ARENA, SCRIPT_ARENA, PED_BYTES, PED_ARENA, OBJ_BYTES, OBJ_ARENA) == (0x5F8, 0x160, 0x1F10, 0x1F90, 0x670, 0x6F0)
SCR_VAR0, SCR_VAR1, SCR_EXT, PED_FLAGS_AT, PED_LOAD_AT, OBJ_FLAGS_AT = 0x3C, 0x40, 0xC9, 0x474, 0x794, 0x140
EXPECTED_CONSTS = {'cases': CASES, 'this_offset': THIS_OFFSET, 'this_bytes': THIS_BYTES, 'this_arena': THIS_ARENA, 'script_offset': SCRIPT_OFFSET, 'script_bytes': SCRIPT_BYTES,
                   'script_arena': SCRIPT_ARENA, 'slots': SLOTS, 'ped_size': PED_SIZE, 'obj_size': OBJ_SIZE, 'ped_offset': PED_OFFSET, 'ped_bytes': PED_BYTES, 'ped_arena': PED_ARENA,
                   'obj_offset': OBJ_OFFSET, 'obj_bytes': OBJ_BYTES, 'obj_arena': OBJ_ARENA}
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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def put(buf, off, size, value):
    """Aplica `value` (little endian) sobre el diccionario {desplazamiento: byte}."""
    for i in range(size):
        buf[off + i] = (value >> (8 * i)) & 0xFF


def diff_from(old_fields, new):
    """Lista ordenada [desplazamiento, byte_nuevo] solo donde el byte cambia. old_fields: {off: byte_antes}."""
    return [[o, b] for o, b in sorted(new.items()) if old_fields[o] != b]


def expected(inp, case):
    """Salida esperada de UN lado (EXE o DLL) a partir de la fila `input`, y estadisticas de cobertura. Ver el desensamblado en el docstring."""
    t = inp['type'] & 0xFF
    ts = t - 256 if t >= 128 else t
    slot = inp['ent_slot']
    calls = [['new', 1, s16(inp['script_idx'])]]           # push movsx word [esi] ; ECX = 0xA47B60
    # estado "antes" de los campos tocados (relativo al inicio de cada arena)
    scr_old, scr_new = {}, {}
    put(scr_old, SCRIPT_OFFSET + SCR_VAR0, 4, inp['script_var0']); put(scr_old, SCRIPT_OFFSET + SCR_VAR1, 4, inp['script_var1']); put(scr_old, SCRIPT_OFFSET + SCR_EXT, 1, inp['script_ext'])
    put(scr_new, SCRIPT_OFFSET + SCR_EXT, 1, t)                                        # byte [ebx + 0xC9] = tipo
    ped_old, ped_new, obj_old, obj_new = {}, {}, {}, {}
    pbase = PED_OFFSET + slot * PED_SIZE
    obase = OBJ_OFFSET + slot * OBJ_SIZE
    put(ped_old, pbase + PED_FLAGS_AT, 4, inp['ped_flags']); put(ped_old, pbase + PED_LOAD_AT, 2, inp['ped_load'])
    put(obj_old, obase + OBJ_FLAGS_AT, 4, inp['obj_flags'])
    st = {'type': ts, 'path': 'none', 'slot': slot, 'ref': None, 'has': inp['has_arg'] & 0xFF, 'load': inp['ped_load'] & 0xFFFF, 'ent_pool': inp['ent_pool'], 'flags_before': None}
    if 0 <= ts <= 5 and ts in (0, 3, 5):
        assert inp['ent_pool'] == 1, 'fixture invalido: entidad de ped fuera del pool de peds'
        ref = (slot << 8) + inp['ped_state'][slot]                                  # indice = (ped - base) / 0x7C4 ; ref = byte de estado + (indice << 8)
        put(scr_new, SCRIPT_OFFSET + SCR_VAR0, 4, ref)
        flags = (inp['ped_flags'] | 0x800000) & M32                                 # or [edi+0x474], 0x800000
        if ts == 5:
            put(scr_new, SCRIPT_OFFSET + SCR_VAR1, 4, inp['has_arg'] & 0xFF)       # movzx ecx, byte [arg3]
        load = inp['ped_load'] & 0xFFFF                                             # movzx edx, word [edi+0x794] (leido tras el or, antes de limpiar)
        flags &= 0xFEFFFFFF                                                         # and ebx, 0xFEFFFFFF
        calls.append(['rem', 1, load])                                              # push edx ; push edi ; call 0x46ABC0
        put(ped_new, pbase + PED_FLAGS_AT, 4, flags)
        put(ped_new, pbase + PED_LOAD_AT, 2, 0xFFFF)                               # word [edi+0x794] = 0xFFFF
        st.update(path='ped', ref=ref, flags_before=inp['ped_flags'])
    elif 0 <= ts <= 5 and ts in (1, 4):
        assert inp['ent_pool'] == 2, 'fixture invalido: objeto fuera del pool de objetos'
        ref = (slot << 8) + inp['obj_state'][slot]
        put(scr_new, SCRIPT_OFFSET + SCR_VAR0, 4, ref)
        put(obj_new, obase + OBJ_FLAGS_AT, 4, (inp['obj_flags'] | 0x300000) & M32)  # or [esi+0x140], 0x300000
        st.update(path='obj', ref=ref, flags_before=inp['obj_flags'])
    elif ts == 2:
        st['path'] = 'type2'
    else:
        st['path'] = 'out_of_range'                                                 # EAX > 5 sin signo (incluye negativos): salta al final
    # para los campos sin cambio, el "nuevo" valor es el viejo (el diff solo incluye bytes distintos)
    for old, new in ((scr_old, scr_new), (ped_old, ped_new), (obj_old, obj_new)):
        for o in old:
            new.setdefault(o, old[o])
    want = {'this_fnv': inp['this_fnv'], 'calls': calls, 'call_count': len(calls), 'pools_ok': True,
            'script_diff': diff_from(scr_old, scr_new), 'ped_diff': diff_from(ped_old, ped_new), 'obj_diff': diff_from(obj_old, obj_new)}
    return want, st


def coverage(rows):
    cov = {}

    def bump(key, cond=True):
        if cond:
            cov[key] = cov.get(key, 0) + 1
    for inp, st in rows:
        bump('kind%d' % inp['kind'])
        bump('path_' + st['path'])
        ts = st['type']
        bump('type_%d' % ts, ts in (0, 1, 2, 3, 4, 5, -1, 6, 7, 127, -128, 100))
        bump('type_neg', ts < 0); bump('type_gt5', ts > 5)
        bump('index_arg_garbage', (inp['index_arg'] >> 8) != 0); bump('has_arg_garbage', (inp['has_arg'] >> 8) != 0)
        bump('script_idx_neg', s16(inp['script_idx']) < 0); bump('script_idx_ge82', s16(inp['script_idx']) >= 82)
        bump('ent_bad_ptr_unused', st['path'] in ('type2', 'out_of_range') and inp['ent_pool'] in (0, 3))
        bump('ent_valid_unused', st['path'] in ('type2', 'out_of_range') and inp['ent_pool'] in (1, 2))
        if st['path'] == 'ped':
            bump('ped_slot%d' % st['slot'])
            bump('ped_type5_has1', ts == 5 and st['has'] == 1); bump('ped_type5_has0', ts == 5 and st['has'] == 0)
            bump('ped_type0', ts == 0); bump('ped_type3', ts == 3)
            fb = st['flags_before']
            bump('ped_flag800000_set', fb & 0x800000 != 0); bump('ped_flag1000000_set', fb & 0x1000000 != 0); bump('ped_flags_clear', fb & 0x1800000 == 0); bump('ped_flags_all', fb == 0xFFFFFFFF)
            bump('ped_load_ffff', st['load'] == 0xFFFF); bump('ped_load_0', st['load'] == 0); bump('ped_load_8000', st['load'] == 0x8000)
            bump('ped_state_empty_bit', (st['ref'] & 0x80) != 0); bump('ped_state_ref0', (st['ref'] & 0xFF) == 0)
        elif st['path'] == 'obj':
            bump('obj_slot%d' % st['slot'])
            bump('obj_type1', ts == 1); bump('obj_type4', ts == 4)
            fb = st['flags_before']
            bump('obj_flags_clear', fb & 0x300000 == 0); bump('obj_flags_set', fb & 0x300000 == 0x300000); bump('obj_flags_all', fb == 0xFFFFFFFF)
            bump('obj_state_empty_bit', (st['ref'] & 0x80) != 0)
    return cov


def selftest(dump=None):
    """Autoprueba: casos escritos a mano contra el desensamblado y comparacion de constantes/filas con Fixtures.hpp compilado con cl."""
    def mk(t, pool, slot=1, pflags=0, pload=0x1234, oflags=0, has=1, sidx=5, pst=(0x11, 0x22, 0x83, 0x44), ost=(1, 2, 3, 0x85)):
        return {'type': t, 'ent_pool': pool, 'ent_slot': slot, 'ped_flags': pflags, 'ped_load': pload, 'obj_flags': oflags, 'has_arg': has, 'script_idx': sidx & 0xFFFF,
                'script_var0': 0x01020304, 'script_var1': 0x0A0B0C0D, 'script_ext': 0x55, 'ped_state': list(pst), 'obj_state': list(ost), 'this_fnv': 'x', 'kind': 0}
    # tipo 5: ref = (slot << 8) + estado, local 1 = bHasAScriptBrain, flags |= 0x800000 y se limpia 0x1000000, remove(ped, cerebro a cargar), 0x794 = 0xFFFF
    w, st = expected(mk(5, 1, slot=2, pflags=0x1000000, pload=0x1234), 0)
    assert w['calls'] == [['new', 1, 5], ['rem', 1, 0x1234]]
    sd = dict(w['script_diff'])
    assert sd[0x40 + 0xC9] == 5 and sd[0x40 + 0x3C] == 0x83 and sd[0x40 + 0x3D] == 0x02 and sd[0x40 + 0x40] == 1 and sd[0x40 + 0x41] == 0 and sd[0x40 + 0x3E] == 0
    pd = dict(w['ped_diff']); base = 0x40 + 2 * 0x7C4
    assert pd[base + 0x474 + 2] == 0x80 and pd[base + 0x474 + 3] == 0x00 and pd[base + 0x794] == 0xFF and pd[base + 0x795] == 0xFF
    assert w['obj_diff'] == []
    # tipo 0 y 3: no tocan la variable local 1; flags sin 0x1000000 -> solo cambia 0x800000
    for t in (0, 3):
        w, st = expected(mk(t, 1, slot=0, pflags=0x1800000 ^ 0x1000000, pload=0xFFFF), 0)
        assert [o for o, b in w['script_diff'] if o in range(0x40 + 0x40, 0x40 + 0x44)] == [] and w['calls'][1] == ['rem', 1, 0xFFFF]
        assert w['ped_diff'] == [] or all(o < 0x40 + 4 * 0x7C4 for o, b in w['ped_diff'])
    # objeto (1, 4): ref con el pool de objetos, flags |= 0x300000, sin llamada a remove
    for t in (1, 4):
        w, st = expected(mk(t, 2, slot=3, oflags=0x100000), 0)
        assert w['calls'] == [['new', 1, 5]] and w['ped_diff'] == []
        assert dict(w['script_diff'])[0x40 + 0x3C] == 0x85 and (0x40 + 0x3D) not in dict(w['script_diff'])   # el byte 0x3D ya valia 3
        assert dict(w['obj_diff'])[0x40 + 3 * 0x19C + 0x140 + 2] == 0x30
    # tipo 2, 6, 127, -128 y -1: solo el byte 0xC9
    for t in (2, 6, 127, 128, 255):
        w, st = expected(mk(t, 0), 0)
        assert w['calls'] == [['new', 1, 5]] and w['ped_diff'] == [] and w['obj_diff'] == [] and w['script_diff'] == [[0x40 + 0xC9, t]]
    # indice de script con signo: 0xFFFF -> -1
    w, st = expected(mk(2, 0, sidx=-1), 0)
    assert w['calls'] == [['new', 1, -1]]
    lines = dump if dump is not None else compile_dump(tempfile.mkdtemp(prefix='sb31_'), 'b3')
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
