"""Oraculo del banco Fn237A (interfaz que usan run_local.py, fixture_selfcheck.py y neg_summary.py): delega en emu_oracle.py (emulacion del codigo ORIGINAL del EXE con Unicorn)."""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import bank_spec as S
import emu_oracle as E

NAMES = tuple(S.NAMES)
ENTRIES = tuple(S.ENTRIES)
DUMMY_ARENAS = [0x30000000, 0x30010000, 0x30020000, 0x30030000]
EVENT_NAMES = [c['name'] for c in S.COLLABS]
_KEYS_FILE = HERE / 'coverage.json'


def _keys():
    if _KEYS_FILE.exists():
        return json.loads(_KEYS_FILE.read_text(encoding='utf-8'))
    return [[], []]


KEYS = _keys()


def expected(inp):
    inp = dict(inp)
    inp.setdefault('arena', DUMMY_ARENAS)
    out = E.expected(inp)
    if out['fail']:
        raise RuntimeError('El oraculo (emulacion del EXE) fallo en el caso fn=%s n=%s cmd=%s: %s' % (inp.get('fn'), inp.get('n'), inp['cmd'], out['fail']))
    marks = ['cmd:%d' % inp['cmd']] + sorted({'ev:' + EVENT_NAMES[e[0]] for e in out['events']})
    k = inp['cmd'] - S.BASES[inp['fn']]
    if not (0 <= k < 100):
        marks.append('oor')
    out['marks'] = marks
    return out


def check_side(inp, obs, exp, side_row=None):
    """None si lo observado en un lado coincide con el oraculo; si no, (campo, detalle)"""
    if not (exp['esp_ok'] and exp['nonvolatile_ok'] and exp['seh_ok']):
        return ('abi_oraculo', 'la emulacion no restauro pila/registros/SEH')
    if obs['ret'] != exp['ret']:
        return ('ret', (obs['ret'], exp['ret']))
    if obs['events'] != exp['events']:
        return ('events', {'observado': obs['events'], 'esperado': exp['events']})
    if obs['gchanges'] != exp['gchanges']:
        return ('gchanges', {'observado': obs['gchanges'], 'esperado': exp['gchanges']})
    for a in S.ARENA_NAMES:
        if obs['changes'][a] != exp['changes'][a]:
            return ('changes_' + a, {'observado': obs['changes'][a], 'esperado': exp['changes'][a]})
    return None


def check_row(row, exp):
    for side in ('original', 'candidate'):
        bad = check_side(row['input'], row[side]['observed'], exp, row[side])
        if bad:
            return (side + ':' + bad[0], bad[1])
    return None
