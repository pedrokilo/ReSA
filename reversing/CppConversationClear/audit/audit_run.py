"""Auditoria independiente de CppConversationClear; sin importar el runner ni ejecutar el juego."""
import argparse
import gzip
import hashlib
import json
import re
import shutil
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SELECTED = {'CConversationNode::ClearRecursively': 2048}
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
FLAGS_HYPOTHESIS = 0x84  # CF=OF=0, SF=1, ZF=0, PF=1: se COMPRUEBA, no se asume
GATES = ('called', 'fixture_acyclic', 'route_ok', 'stack_restored', 'registers_preserved', 'edx_preserved',
         'df_preserved', 'oracle_ok', 'guards_ok', 'snapshot_restored', 'fp_state_restored')
REQUIRED_SOURCES = {'source/game_sa/Conversations/ConversationNode.cpp', 'source/game_sa/Conversations/ConversationNode.h',
                    'source/game_sa/Conversations/ConversationNodeCpp.cpp', 'source/game_sa/Conversations/Conversations.h',
                    'source/game_sa/Conversations/ConversationForPed.cpp', 'source/CMakeLists.txt', 'CMakeLists.txt',
                    'source/InjectHooksMain.cpp', 'source/reversiblehooks/RootHookCategory.h',
                    'source/reversiblehooks/ReversibleHook/Base.h'}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('bank')
    parser.add_argument('run', type=Path)
    parser.add_argument('--archive', action='store_true')
    parser.add_argument('--require-registered', action='store_true')
    args = parser.parse_args()
    assert args.bank == 'CppConversationClear'
    bank = ROOT / 'reversing' / args.bank
    table = (bank / 'LiveComparison.hpp').read_text(encoding='utf-8')
    entries = re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)
    entries = [(c, f, n) for c, f, n in entries if c + '::' + f in SELECTED]
    expected = {c + '::' + f: int(n) for c, f, n in entries}
    assert expected == SELECTED and len(expected) == len(entries), 'Tabla fns[] distinta de la seleccion'
    total = sum(expected.values())
    summary = json.loads((args.run / 'summary.json').read_text(encoding='utf-8'))
    assert all(summary[k] is True for k in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'))
    assert summary['original_exe_sha256'] == EXE
    assert summary['pairs'] == total and summary['pairs_per_function'] == expected
    raw = (args.run / 'comparisons.jsonl').read_bytes()
    assert hashlib.sha256(raw).hexdigest() == summary['log_sha256']
    rows = [json.loads(line) for line in raw.decode('utf-8').splitlines()]
    pairs = [r for r in rows if r.get('event') == 'comparison']
    complete = [r for r in rows if r.get('event') == 'complete']
    assert len(pairs) == total and Counter(r['function'] for r in pairs) == expected
    assert {r['pair'] for r in pairs} == set(range(total)) and all(r['pair'] == r['case'] for r in pairs)
    assert len(complete) == 1 and complete[0]['state_restored'] is True and complete[0]['pairs'] == total
    assert complete[0]['fp_state_restored'] is True and complete[0]['hook_restored'] is True
    assert not any(r.get('event') in ('error', 'assertion', 'first_chance_av', 'first_chance_av_raw') for r in rows)
    begin = [r for r in rows if r.get('event') == 'begin']
    assert len(begin) == 1 and begin[0]['registered_hook'] is True and begin[0]['nodes_address_ok'] is True
    assert begin[0]['collaborators_stubbed'] is False and begin[0]['fp_state_restored'] is True
    assert begin[0]['nodes_address'] == 0x969570
    flags, eaxs, shapes, roots, reach = set(), set(), set(), set(), set()
    shared = 0
    for r in pairs:
        assert r.get('original') is not None and r.get('candidate') is not None, r['pair']
        assert r['accepted'] is True and r['equal'] is True and r['original'] == r['candidate'], r['pair']
        for side in ('original', 'candidate'):
            v = r[side]
            assert isinstance(v, dict), (r['pair'], side)
            for g in GATES:
                assert v.get(g) is True, (r['pair'], side, g)
            # EDX preservado en TODOS los pares; ESI/EDI/EBX son las semillas del arnes.
            assert v['edx'] == v['edx_seed'], (r['pair'], side, 'EDX')
            assert (v['esi'], v['edi'], v['ebx']) == (0x2468ACE0, 0x10293847, 0x13579BDF), (r['pair'], side)
            assert v['ecx_seed'] == 0x969570 + 0x18 * r['root'], (r['pair'], side)
            assert v['touched'] == v['reachable'] == r['reachable'] and 1 <= v['reachable'] <= 49, (r['pair'], side)
            assert v['nodes_hash_after'] == v['expected_hash'], (r['pair'], side)
            assert v['flags_df_before'] == v['flags_df_after'] == 0, (r['pair'], side)
        o = r['original']
        flags.add(o['flags_defined']); eaxs.add((o['eax_seed'], o['eax'])); shapes.add(r['shape']); roots.add(r['root'])
        reach.add(r['reachable']); shared += bool(r['shared'])
    for name, count in expected.items():
        assert {r['case'] for r in pairs if r['function'] == name} == set(range(count)), name
    assert shapes == set(range(16)) and {0, 49} <= roots and min(reach) == 1 and max(reach) == 49 and shared > 0
    assert any(r['original']['edx_seed'] == 0x969570 + 0x18 * ((r['root'] + 1 + (r['case'] // 2) % 49) % 50) for r in pairs if r['case'] & 1)
    assert flags == {FLAGS_HYPOTHESIS}, f'Banderas definidas observadas {sorted(map(hex, flags))}; la hipotesis 0x84 (SF|PF) NO se confirma'
    fsum = [r for r in rows if r.get('event') == 'function_summary']
    assert not args.require_registered or begin[0]['registered_hook'] is True
    assert len(fsum) == 1 and {r['function']: r['cases'] for r in fsum} == expected
    assert all(r['failures'] == 0 and r['complete_paths'] is True for r in fsum)
    manifest = json.loads((bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    assert summary['executed_probe_sha256'] == manifest['binaries']['asi']
    assert REQUIRED_SOURCES <= set(manifest['sources']), 'Faltan hashes de fuentes obligatorias'
    assert sha(ROOT / 'source/game_sa/Game.cpp') == manifest['game_cpp']
    for path, digest in manifest['sources'].items():
        assert sha(ROOT / path) == digest, path
    for path, digest in manifest['instrumentation'].items():
        assert sha(bank / path) == digest, path
    result = dict(bank=args.bank, run=args.run.name, functions=len(expected), pairs=total, pairs_per_function=expected,
                  differences=0, null_comparison_records=0, state_restored=True, saves_unchanged=True, configs_restored=True,
                  hook_routes_verified=True, registered_hook_verified=True, edx_preserved_all_pairs=True,
                  flags_defined_observed=sorted(flags), eax_after_values=sorted({e for _, e in eaxs}),
                  log_sha256=summary['log_sha256'], tested_sources=manifest['sources'], bank_sources=manifest['instrumentation'],
                  original_exe_sha256=EXE, executed_probe_sha256=summary['executed_probe_sha256'],
                  auditor_sha256=sha(Path(__file__)), route_field='route_ok',
                  scope='Acyclic fixtures only; raw EAX/ECX/EDX/ESI/EDI/EBX/EBP/ESP/flags at return; not a proof of the EXE body or of other callers')
    if args.archive:
        dest = bank / 'runs' / args.run.name
        assert not dest.exists()
        dest.mkdir(parents=True)
        for name in ('summary.json', 'RESULTADO.txt'):
            shutil.copy2(args.run / name, dest / name)
            assert sha(dest / name) == sha(args.run / name)
        with (dest / 'comparisons.jsonl.gz').open('wb') as stream:
            with gzip.GzipFile(filename='', mode='wb', fileobj=stream, mtime=0) as zipped:
                zipped.write(raw)
        with gzip.open(dest / 'comparisons.jsonl.gz', 'rb') as stream:
            assert hashlib.sha256(stream.read()).hexdigest() == summary['log_sha256']
        (dest / 'approval-audit.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
