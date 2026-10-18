"""Comprueba la salida de harness.exe (EXE real en un bloque reubicado frente a la reconstruccion C++ del parche).

Para cada (funcion, caso): EXE == C++ (EAX, arena completo con guardas, traza de los dobles) y ambos == oraculo independiente de Python (fixture_oracle.py).
Los campos de EAX de las funciones void no se exigen (ningun llamador los lee). Las funciones CRT se comprueban contra la traza de sus colaboradores (los dobles del arnes):
_lock/_unlock = [1|2, 16 + indice], EnterCriticalSection/LeaveCriticalSection = [3|4, stream + 0x20], _nh_malloc = [size, _newmode], _getptd = valor del caso.

Uso:  python check_offline.py salida.jsonl
"""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import fixture_oracle as oracle

M32 = 0xFFFFFFFF
VOID = {'RwPlCore::_rwSListDestroyEndEntries', 'RwPlCore::_rwStringClose', 'CRT::CrtLockFile2', 'CRT::CrtUnlockFile2'}


def main(path):
    n = 0
    seen = {f: set() for f in oracle.FUNCTIONS}
    for line in Path(path).read_text().splitlines():
        r = json.loads(line)
        fn, c, delta, base = r['f'], r['c'], r['delta'], r['base']
        seen[fn].add(c)
        exe, cpp = r['exe'], r['cpp']
        if fn not in VOID and exe['eax'] != cpp['eax']:
            raise SystemExit('%s %d: EAX del EXE %#x != C++ %#x' % (fn, c, exe['eax'], cpp['eax']))
        if exe['arena'] != cpp['arena'] or exe['trace'] != cpp['trace']:
            raise SystemExit('%s %d: arena o traza distintos entre el EXE y C++' % (fn, c))
        want = oracle.expected(fn, c, base)
        name = fn.split('::')[1]
        f = oracle.fixture(fn, c, base)
        # --- expectativas del arnes para las funciones CRT (dobles) y ajustes por la reubicacion
        if name == 'CrtLockFile2' or name == 'CrtUnlockFile2':
            tag = (1, 3) if name == 'CrtLockFile2' else (2, 4)
            want['trace'] = [tag[0], f['index'] + 16] if f['real'] else [tag[1], 0x500020]
        elif name == 'CrtMalloc':
            want['eax'] = oracle.word(c, 77)
            want['trace'] = [f['size'], f['newmode']]
            del want['null']
            want.pop('align8', None)
        elif name == 'CrtErrno':
            want['eax'] = (oracle.word(c, 78) + 8) & M32
            for k in ('eax_minus_ptd', 'readback'):
                del want[k]
        elif name == '_rwStringStreamGetSize':
            want['trace'] = [(oracle.EMPTY_STRING + delta) & M32 if f['null'] else (base + 16) & M32]
        for k in ('lock_count', 'recursion', 'owner_is_this_thread', 'state'):
            want.pop(k, None)
        for side in ('exe', 'cpp'):
            got = r[side]
            if 'arena' in want and got['arena'] != want['arena']:
                raise SystemExit('%s %d (%s): arena distinto del oraculo' % (fn, c, side))
            if got['trace'] != want['trace']:
                raise SystemExit('%s %d (%s): traza %s != oraculo %s' % (fn, c, side, got['trace'], want['trace']))
            if 'eax' in want and fn not in VOID and got['eax'] != want['eax']:
                raise SystemExit('%s %d (%s): EAX %#x != oraculo %#x' % (fn, c, side, got['eax'], want['eax']))
        n += 1
    for fn, cases in seen.items():
        if cases != set(range(oracle.CASES)):
            raise SystemExit('cobertura incompleta de ' + fn)
    print('OK: %d pares (EXE == C++ == oraculo) en %d funciones' % (n, len(seen)))


if __name__ == '__main__':
    main(sys.argv[1])
