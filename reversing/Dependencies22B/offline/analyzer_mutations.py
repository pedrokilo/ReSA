"""Comprueba que run_local.py rechaza un registro alterado: toma un registro valido (p. ej. el que produce rehearsal.exe) y aplica mutaciones; cada una debe hacer fallar el analizador.
Uso: python analyzer_mutations.py <registro.jsonl>   (con runtime_runner.py y probe_case.py en el directorio padre de Dependencies22B, como en reversing/)"""
import copy
import importlib.util
import json
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('run_local_22b', HERE.parent / 'run_local.py')
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)


def load(path):
    return [json.loads(l) for l in Path(path).read_text(encoding='utf-8').splitlines()]


def first(rows, pred):
    return next(r for r in rows if r['event'] == 'comparison' and pred(r))


def run(rows):
    with tempfile.TemporaryDirectory() as d:
        p = Path(d) / 'comparisons.jsonl'
        p.write_text('\n'.join(json.dumps(r) for r in rows) + '\n', encoding='utf-8')
        try:
            if mod.analyze(p) is None:
                return 'analyze devolvio None (no acredita)'
        except Exception as e:  # noqa: BLE001
            return str(e)[:110]
    return None


def mutate(base, name, fn):
    rows = copy.deepcopy(base)
    fn(rows)
    reason = run(rows)
    print(('RECHAZADA  ' if reason else 'ACEPTADA (MAL) ') + name + (' -> ' + reason if reason else ''))
    return reason is not None


def corona(r):
    return next(e for e in r['original']['trace'] if isinstance(e, list) and e[0] == 'register_corona')


def both(r, fn):
    fn(r['original']); fn(r['candidate'])


def main():
    base = load(sys.argv[1])
    ok = True
    print('original ->', 'aceptado' if run(copy.deepcopy(base)) is None else 'RECHAZADO (MAL)')
    muts = []

    def m_id(rows):
        r = first(rows, lambda r: r['function'].endswith('HeadLightEffect') and any(isinstance(e, list) and e[0] == 'register_corona' for e in r['original']['trace']))
        both(r, lambda v: [e.__setitem__(1, e[1] + 1) for e in v['trace'] if isinstance(e, list) and e[0] == 'register_corona'])
    muts.append(('id de la corona desplazado en ambos lados', m_id))

    def m_diff(rows):
        r = first(rows, lambda r: True); r['candidate']['x87']['al'] ^= 1
    muts.append(('retorno distinto', m_diff))

    def m_ftw(rows):
        r = first(rows, lambda r: True); r['candidate']['x87']['ftw'] = 0xFF7F
    muts.append(('pila x87 no vacia', m_ftw))

    def m_gate(rows):
        r = first(rows, lambda r: True); r['original']['guards_preserved'] = False
    muts.append(('guarda de la arena', m_gate))

    def m_write(rows):
        r = first(rows, lambda r: True); both(r, lambda v: v['changes'].append([0, 'ff']))
    muts.append(('la funcion escribe memoria', m_write))

    def m_no_find(rows):
        for r in rows:
            if r['event'] == 'comparison':
                both(r, lambda v: v.__setitem__('trace', [e for e in v['trace'] if not (isinstance(e, list) and e[0] == 'find_player_vehicle')]))
    muts.append(('sin llamadas a FindPlayerVehicle', m_no_find))

    def m_no_panel(rows):
        for r in rows:
            if r['event'] == 'comparison':
                both(r, lambda v: v.__setitem__('trace', [e for e in v['trace'] if not (isinstance(e, list) and e[0] == 'get_panel_status')]))
    muts.append(('sin llamadas a GetPanelStatus', m_no_panel))

    def m_const(rows):
        r = first(rows, lambda r: any(isinstance(e, list) and e[0] == 'register_corona' and e[12] == 1 for e in r['original']['trace']))
        both(r, lambda v: [e.__setitem__(19, 0x3F000001) for e in v['trace'] if isinstance(e, list) and e[0] == 'register_corona' and e[12] == 1])
    muts.append(('constante pullTowardsCam alterada en ambos lados', m_const))

    def m_ret(rows):
        r = first(rows, lambda r: r['function'].endswith('TailLightEffect') and any(isinstance(e, list) and e[0] == 'register_corona' for e in r['original']['trace']))
        both(r, lambda v: v['x87'].__setitem__('al', v['x87']['al'] ^ 1))
    muts.append(('retorno de la cola contrario al modo en ambos lados', m_ret))

    def m_cov(rows):
        for r in rows:
            if r['event'] == 'comparison' and r['profile']['directed'] == 'threshold':
                r['profile']['directed'] = 'none'
    muts.append(('sin escenarios de umbral', m_cov))

    def m_pair(rows):
        rows.remove(first(rows, lambda r: True))
    muts.append(('un par menos', m_pair))

    def m_end(rows):
        rows[:] = [r for r in rows if r['event'] != 'complete']
    muts.append(('sin evento complete', m_end))

    def m_dis(rows):
        r = first(rows, lambda r: r['profile']['disabled_or_alarm'] != 0)
        both(r, lambda v: v['trace'].append(['register_corona'] + [0] * 22))
    muts.append(('coronas con disabledOrAlarm', m_dis))
    for name, fn in muts:
        ok &= mutate(base, name, fn)
    print('TODAS RECHAZADAS' if ok else 'ALGUNA ACEPTADA')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
