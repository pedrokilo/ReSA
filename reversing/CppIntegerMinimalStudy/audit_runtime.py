"""Independent offline admission; no runner imports or game execution."""
import argparse
import hashlib
import json
from pathlib import Path

W4 = {'D3DResourceSystem::SetUseD3DResourceBuffering': 512, 'CGarages::FindGarageForObject': 4096}
W10 = {'CCam::GetBoatHeightForFirstPerson': 2048, 'CCarCtrl::Native_00424100': 2048}
SEEDS = (0, 0xFF, 0x13579BDF, 0x2468ACE0, 0xFFFFFFFF, 0x80000000, 0x01020304, 0x7FFFFFFF)

def need(condition, label):
    if not condition:
        raise ValueError(label)

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def runner_audit(summary_path, comparisons, kind, asi_digest, game):
    summary = json.loads(Path(summary_path).read_text(encoding='utf-8'))
    expected = W4 if kind == 4 else W10
    need(all(summary.get(k) is True for k in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored')), 'runner restoration')
    need(summary.get('pairs') == sum(expected.values()) and summary.get('pairs_per_function') == expected, 'runner full counts')
    need(summary.get('log_sha256') == sha(comparisons) and summary.get('executed_probe_sha256') == asi_digest, 'runner log/ASI linkage')
    need(summary.get('original_exe_sha256') == '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b', 'canonical EXE hash')
    need(sha(Path(game)/'gta_sa.exe') == summary['original_exe_sha256'], 'current EXE hash')
    saves, configs = summary.get('saves_before'), summary.get('config_hashes_before')
    need(isinstance(saves, dict) and bool(saves) and isinstance(configs, dict) and bool(configs), 'restoration hash inventory')
    for path, digest in saves.items(): need(sha(path) == digest, 'save restoration hash: '+path)
    for path, digest in configs.items(): need(sha(Path(game)/path) == digest, 'config restoration hash: '+path)
    return {'runner_summary_sha256': sha(summary_path), 'saves_verified': saves, 'configs_verified': configs,
            'original_exe_sha256': summary['original_exe_sha256']}

def audit(path, kind):
    rows = [json.loads(x) for x in Path(path).read_text(encoding='utf-8').splitlines()]
    expected = W4 if kind == 4 else W10
    total = sum(expected.values())
    begin = [r for r in rows if r.get('event') == 'begin']
    need(len(begin) == 1 and begin[0].get('registered_hook') is True, 'begin/hook')
    b = begin[0]
    if kind == 4:
        need(b.get('layout') is True and b.get('geometry_collaborator_modes') == 'original_OFF_candidate_ON' and b.get('radius_probe_module') is True and b.get('radius_resolved_body', 0) > 0, 'geometry routes')
    else:
        need(all(b.get(k) is True for k in ('helper_registered', 'helper_route_ok', 'fp_state_restored')), 'handling route')
        need(b.get('collaborators_stubbed') is False and b.get('real_vehicle_count', 0) > 0 and b.get('player_area') == 0 and b.get('fixture_configuration') == 'FastLoader.SaveGameToLoad=0', 'real exterior fixture')
        need(b.get('helper_entry') == 0x6F5300 and b.get('helper_body', 0) > 0 and len(b.get('helper_on_bytes', [])) == len(b.get('helper_off_bytes', [])) == 32, 'helper byte proof')
    seen = {n: set() for n in expected}
    pairs = set()
    for r in rows:
        need(r.get('event') != 'error' and not r.get('event', '').startswith('first_chance'), 'runtime error')
        if r.get('event') != 'comparison':
            continue
        n, case, pair = r['function'], r['case'], r['pair']
        need(n in expected and type(case) is int and 0 <= case < expected[n] and case not in seen[n] and pair not in pairs, 'case identity')
        offset = 0 if n == next(iter(expected)) else expected[next(iter(expected))]
        need(type(pair) is int and pair == offset + case, 'pair mapping')
        seen[n].add(case); pairs.add(pair)
        need(r.get('equal') is True and r.get('accepted') is True and isinstance(r.get('original'), dict) and isinstance(r.get('candidate'), dict) and r['original'] == r['candidate'], 'null/different pair')
        for side in ('original', 'candidate'):
            v = r[side]
            gates = ('route_ok', 'helper_route_ok', 'stack_restored', 'registers_preserved', 'guards_ok')
            gates += ('fp_preserved', 'fixture_unchanged') if kind == 4 else ('contract_ok', 'readonly_ok', 'fp_state_restored')
            need(all(v.get(k) is True for k in gates), 'nested gate')
            if n == next(iter(W4)):
                seed = (0xA153EC41 ^ case * 2654435761) & 0xFFFFFFFF
                need(v['eax'] == ((seed & 0xFFFFFF00) | (case & 1)) and v['ecx'] == 0x41315926 and v['edx'] == 0x81928374, 'setter raw GPR')
                need(v['flags_before'] == v['flags_after'] and v.get('flags_preserved') is True and v.get('setter_exact_ok') is True, 'setter flags')
                before, after = v['buffering_before'], v['buffering_bytes']
                need(len(before) == len(after) == 17 and all(type(x) is int and 0 <= x <= 255 for x in before + after), 'byte neighborhood')
                expected_bytes = before.copy(); expected_bytes[8] = case & 1
                need(after == expected_bytes and v['eax_seed'] == seed and v['argument_before'] == v['argument_after'] == case & 1, 'physical setter argument/neighbors')
            elif n == 'CGarages::FindGarageForObject':
                need(v.get('return_range_ok') is True and -1 <= v['return_index'] < v['counter'] and v['counter'] == case % 51 and v['matrix_position'] == bool(case & 1), 'garage fixture')
            elif n == next(iter(W10)):
                need(type(v['target_present']) is bool and v['target_present'] == bool(case & 1) and v['return_bits'] == int(v['target_present']) and v['height_bits'] == v['expected_height_bits'], 'camera real/null fixture')
            else:
                seed = SEEDS[case % 8]
                need(v['eax_seed'] == seed and v['return_bits'] == seed & 0xFFFFFF00 and v['al'] == 0 and v['ecx_after'] == 0x41315926 and v['edx_after'] == 0x81928374, 'constant raw GPR')
                need(v['x_after'] == v['x_bits'] and v['y_after'] == v['y_bits'], 'physical float arguments')
                need(v['flags_after'] & 0x8C5 == 0x44 and v['flags_before'] & 0x400 == v['flags_after'] & 0x400, 'XOR defined flags/DF')
    need(pairs == set(range(total)) and all(seen[n] == set(range(count)) for n, count in expected.items()), 'full coverage')
    summaries = [r for r in rows if r.get('event') == 'function_summary']
    need(len(summaries) == len(expected) and {r['function'] for r in summaries} == set(expected), 'summary identity')
    for r in summaries:
        need(r['cases'] == expected[r['function']] and r['failures'] == 0, 'summary count')
        if kind == 10: need(r.get('complete_paths') is True, 'complete paths')
    end = [r for r in rows if r.get('event') == 'complete']
    need(len(end) == 1 and end[0].get('pairs') == total and end[0].get('state_restored') is True, 'final restore')
    if kind == 10: need(end[0].get('fp_state_restored') is True and end[0].get('helper_route_ok') is True, 'final FP/hook restore')
    return {'functions': len(expected), 'pairs': total, 'differences': 0, 'null_passes': 0, 'state_restored': True, 'comparisons_sha256': sha(path)}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('comparisons'); ap.add_argument('--kind', type=int, choices=(4, 10), required=True)
    ap.add_argument('--manifest', required=True); ap.add_argument('--asi', required=True)
    ap.add_argument('--repo', required=True); ap.add_argument('--bank', required=True)
    ap.add_argument('--runner-summary', required=True); ap.add_argument('--game', required=True)
    a = ap.parse_args(); m = json.loads(Path(a.manifest).read_text(encoding='utf-8'))
    need(sha(a.asi) == m['binaries']['asi'], 'ASI hash')
    required_sources = {('source/game_sa/RenderWare/D3DResourceSystem.cpp' if a.kind == 4 else 'source/game_sa/CarCtrl.cpp'), 'source/CMakeLists.txt', 'docs/hooks.csv',
                        'source/StdInc.h', 'source/game_sa/CarCtrl.h',
                        'source/game_sa/RenderWare/D3DResourceSystem.h', 'source/InjectHooksMain.cpp',
                        'reversing/OriginalDependencies499/HookRoute.hpp'}
    need(required_sources <= set(m['sources']), 'compile/catalog dependency missing from manifest')
    for path, digest in m['sources'].items(): need(sha(Path(a.repo) / path) == digest, 'source hash: ' + path)
    for path, digest in m['instrumentation'].items(): need(sha(Path(a.bank) / path) == digest, 'instrument hash: ' + path)
    result = audit(a.comparisons, a.kind); result['manifest_sha256'] = sha(a.manifest); result['asi_sha256'] = sha(a.asi)
    result.update(runner_audit(a.runner_summary,a.comparisons,a.kind,result['asi_sha256'],a.game))
    print(json.dumps(result, indent=2))

if __name__ == '__main__': main()
