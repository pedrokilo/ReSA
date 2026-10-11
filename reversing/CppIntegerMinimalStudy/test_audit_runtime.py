"""Synthetic positive fixtures and independent evidence rejection tests."""
import copy
import json
import tempfile
from pathlib import Path
from audit_runtime import audit, W4, W10, SEEDS

def fixture(kind):
    expected = W4 if kind == 4 else W10
    begin = {'event': 'begin', 'registered_hook': True, 'layout': True,
             'geometry_collaborator_modes': 'original_OFF_candidate_ON', 'radius_probe_module': True,
             'radius_resolved_body': 1, 'helper_registered': True, 'helper_route_ok': True,
             'fp_state_restored': True, 'collaborators_stubbed': False, 'real_vehicle_count': 1,
             'player_area': 0, 'fixture_configuration': 'FastLoader.SaveGameToLoad=0',
             'helper_entry': 0x6F5300, 'helper_body': 1, 'helper_on_bytes': [1]*32, 'helper_off_bytes': [2]*32}
    rows = [begin]; offset = 0
    for n, count in expected.items():
        for case in range(count):
            v = {k: True for k in ('route_ok', 'helper_route_ok', 'stack_restored', 'registers_preserved',
                                  'guards_ok', 'fp_preserved', 'fixture_unchanged', 'contract_ok', 'readonly_ok', 'fp_state_restored')}
            if n == next(iter(W4)):
                seed = (0xA153EC41 ^ case * 2654435761) & 0xFFFFFFFF
                before = [7]*17; after = before.copy(); after[8] = case & 1
                v.update(eax=(seed & 0xFFFFFF00)|(case&1), eax_seed=seed, ecx=0x41315926, edx=0x81928374,
                         flags_before=0x44, flags_after=0x44, flags_preserved=True, setter_exact_ok=True,
                         buffering_before=before, buffering_bytes=after, argument_before=case&1, argument_after=case&1)
            elif n == 'CGarages::FindGarageForObject':
                v.update(return_range_ok=True, return_index=-1, counter=case%51, matrix_position=bool(case&1))
            elif n == next(iter(W10)):
                v.update(target_present=bool(case&1), return_bits=case&1, height_bits=123, expected_height_bits=123)
            else:
                seed = SEEDS[case%8]
                v.update(eax_seed=seed, return_bits=seed&0xFFFFFF00, al=0, ecx_after=0x41315926, edx_after=0x81928374,
                         x_bits=123, x_after=123, y_bits=456, y_after=456, flags_before=0, flags_after=0x44)
            rows.append({'event':'comparison', 'function':n, 'case':case, 'pair':offset+case,
                         'equal':True, 'accepted':True, 'original':v, 'candidate':copy.deepcopy(v)})
        offset += count
        rows.append({'event':'function_summary', 'function':n, 'cases':count, 'failures':0, 'complete_paths':True})
    rows.append({'event':'complete', 'pairs':offset, 'state_restored':True, 'fp_state_restored':True, 'helper_route_ok':True})
    return rows

def main():
    rejected = 0
    with tempfile.TemporaryDirectory() as td:
        p = Path(td)/'comparisons.jsonl'
        for kind in (4,10):
            rows = fixture(kind)
            def run(r):
                p.write_text('\n'.join(json.dumps(x) for x in r), encoding='utf-8'); return audit(p,kind)
            assert run(rows)['differences'] == 0
            # Corrupt both equal payloads to ensure independent recomputation, not only parity.
            mutations = [('null', None), ('case', None), ('restore', None), ('gpr', None), ('flags', None), ('argument', None), ('missing', None)]
            for name,_ in mutations:
                r = copy.deepcopy(rows)
                target = next(x for x in r if x.get('event')=='comparison' and ('SetUse' in x['function'] if kind==4 else 'Native_' in x['function']))
                if name=='null': target['original']=target['candidate']=None
                elif name=='case': target['case']=99999
                elif name=='restore': r[-1]['state_restored']=False
                else:
                    for side in ('original','candidate'):
                        v=target[side]
                        if name=='gpr': v['ecx' if kind==4 else 'ecx_after']=0
                        elif name=='flags': v['flags_after']=0
                        elif name=='argument': v['argument_after' if kind==4 else 'x_after']=999
                        else: del v['flags_before']
                try: run(r)
                except (ValueError,KeyError,TypeError): rejected+=1
                else: raise AssertionError('Accepted '+name)
    print('2 positive complete campaigns; 14 corrupted campaigns rejected.')

if __name__=='__main__': main()
