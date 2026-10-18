"""Genera un registro sintetico con las mismas claves que LiveComparison.hpp y comprueba que run_local.py lo acepta (y rechaza 13 mutaciones).

Los campos se derivan del oraculo (fixture_oracle.py), asi que esto prueba la mecanica del analizador (cobertura, compuertas, comparacion con el oraculo, restauracion), no el EXE.
Uso: python synth_log.py
"""
import copy
import json
import shutil
import sys
from pathlib import Path

W = Path(__file__).resolve().parent
SANDBOX = W / 'sandbox'
if SANDBOX.exists():
    shutil.rmtree(SANDBOX)
(SANDBOX / 'Dependencies25E').mkdir(parents=True)
REV = Path(r'D:\Original\gta-reversed-master\reversing')
for name in ('probe_case.py', 'runtime_runner.py'):
    shutil.copy(REV / name, SANDBOX / name)
for name in ('Fixtures.hpp', 'fixture_oracle.py', 'run_local.py'):
    shutil.copy(W.parent / name, SANDBOX / 'Dependencies25E' / name)
sys.path.insert(0, str(SANDBOX))
sys.path.insert(0, str(SANDBOX / 'Dependencies25E'))
import fixture_oracle as o
import run_local as rl

FPHEX = 'ab' * 108
BASES = {fn: 0x0BC10010 + 0x1000 * i for i, fn in enumerate(o.FUNCTIONS)}


def side(fn, c):
    base = BASES[fn] if fn.split('::')[1] in ('_rwFrameListDeinitialize', 'RwD3D9RasterIsCompressed', '_rwPluginRegistryGetPluginOffset', '_rwStringStreamGetSize', '_rwSListDestroyEndEntries',
                                              '_rwSListGetNumEntries', '_rwSListGetEntry', '_rwSListGetBegin', '_rwSListGetEnd', '_rwStringClose', 'CrtAtexit') else 0
    want = o.expected(fn, c, base)
    out = {'input': want['input'], 'base': base, 'state': want.get('state', ['00000000']), 'arena': want.get('arena', ''), 'post_floating_state': FPHEX, 'post_mxcsr': 0x1F80,
           'stack_restored': True, 'nonvolatile_preserved': True, 'route_ok': True, 'guards_ok': True, 'globals_restored': True, 'floating_state_restored': True, 'trace': want['trace']}
    if 'eax' in want and fn.split('::')[1] not in ('_rwSListDestroyEndEntries', '_rwStringClose'):
        out['eax'] = want['eax']
    for k in ('null', 'align8', 'lock_count', 'recursion', 'owner_is_this_thread', 'eax_minus_ptd', 'readback'):
        if k in want:
            out[k] = want[k]
    if fn.endswith('CrtLockFile2'):
        out['lock_count'] = -2
    if fn.endswith('CrtUnlockFile2'):
        out['lock_count'] = -1
    if fn.endswith('CrtMalloc'):
        out['align16'] = 0
    if fn.endswith('CrtLockFile2') or fn.endswith('CrtUnlockFile2'):
        out['thread'] = 1234
    return out


def build_rows():
    rows = [{'event': 'waiting_for_player'}]
    rows.append({'event': 'begin', 'registered_hook': True, 'functions': 16, 'pairs': 16 * 1024, 'cases': dict(o.EXPECTED), 'compare_dead_x87_registers': True, 'collaborators_stubbed': True,
                 'cs_unlocked_lock_count': -1, 'cs_locked_lock_count': -2, 'bodies': {fn: 0x10001000 + i * 16 for i, fn in enumerate(o.FUNCTIONS)}, 'asi_module': 0x10000000, 'original_module': 0x400000})
    pair = 0
    for fn in o.FUNCTIONS:
        for c in range(1024):
            s = side(fn, c)
            rows.append({'event': 'comparison', 'function': fn, 'case': c, 'pair': pair, 'original': copy.deepcopy(s), 'candidate': copy.deepcopy(s), 'fp_difference': [],
                         'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'candidate_residue': {'eax': 1, 'ecx': 5, 'edx': 6}, 'equal': True, 'accepted': True})
            pair += 1
        rows.append({'event': 'function_summary', 'function': fn, 'cases': 1024, 'failures': 0, 'state_restored': True})
    rows.append({'event': 'global_state_final', 'state_restored': True})
    rows.append({'event': 'complete', 'pairs': pair, 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    return rows


def write(rows, name):
    p = SANDBOX / name
    p.write_text(''.join(json.dumps(r) + '\n' for r in rows), encoding='utf-8')
    return p


def expect_ok(rows, label):
    try:
        result = rl.analyze(write(rows, 'log.jsonl'))
    except Exception as e:
        raise SystemExit('FALLO: el registro valido fue rechazado (%s): %s' % (label, e))
    print('aceptado:', label, '-', result['pairs'], 'pares')


def expect_reject(mutate, label):
    rows = build_rows()
    mutate(rows)
    try:
        rl.analyze(write(rows, 'mut.jsonl'))
    except Exception as e:
        print('rechazado:', label, '->', str(e)[:90])
        return
    raise SystemExit('FALLO: la mutacion no fue rechazada: ' + label)


def row_of(rows, fn, case):
    return next(r for r in rows if r['event'] == 'comparison' and r['function'] == fn and r['case'] == case)


def both(r, key, value):
    r['original'][key] = value
    r['candidate'][key] = value


expect_ok(build_rows(), 'registro valido')
expect_reject(lambda rows: row_of(rows, o.FUNCTIONS[0], 5)['candidate'].__setitem__('eax', 7), 'candidato con otro EAX')
expect_reject(lambda rows: both(row_of(rows, o.FUNCTIONS[3], 9), 'eax', 123), 'EAX distinto del oraculo en ambos lados')
expect_reject(lambda rows: row_of(rows, o.FUNCTIONS[1], 3)['original'].__setitem__('stack_restored', False), 'ESP no restaurado')
expect_reject(lambda rows: both(row_of(rows, o.FUNCTIONS[2], 77), 'route_ok', False), 'ruta RH incorrecta')
expect_reject(lambda rows: row_of(rows, o.FUNCTIONS[5], 1).__setitem__('fp_difference', ['st0(dead)']), 'estado x87 distinto')
expect_reject(lambda rows: both(row_of(rows, o.FUNCTIONS[4], 40), 'arena', 'a5' * 80), 'arena distinto del oraculo')
expect_reject(lambda rows: both(row_of(rows, o.FUNCTIONS[10], 17), 'trace', [1, 2]), 'traza de colaborador distinta del oraculo')
expect_reject(lambda rows: next(r for r in rows if r['event'] == 'begin').__setitem__('registered_hook', False), 'hook no registrado')
expect_reject(lambda rows: next(r for r in rows if r['event'] == 'begin').__setitem__('compare_dead_x87_registers', False), 'compuerta x87 relajada')
expect_reject(lambda rows: row_of(rows, o.FUNCTIONS[12], 650)['original']['input'].__setitem__('index', 21), 'fixture distinta de la del oraculo')
expect_reject(lambda rows: both(row_of(rows, o.FUNCTIONS[13], 10), 'recursion', 1), 'cerrojo sin liberar tras unlock')
expect_reject(lambda rows: rows.remove(row_of(rows, o.FUNCTIONS[14], 1000)), 'caso omitido')
expect_reject(lambda rows: next(r for r in rows if r['event'] == 'complete').__setitem__('entries_intact', False), 'entradas parcheadas sin restaurar')
print('synth_log: OK')
