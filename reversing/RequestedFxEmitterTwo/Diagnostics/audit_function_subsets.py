"""Independent per-function audit of complete mixed-result replay. Never launches GTA."""
import collections,gzip,hashlib,importlib.util,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def audit(bank,run):
    manifest=json.loads((bank/'artifacts/manifest.json').read_text());errors=[]
    for label in ('tested_sources','bank_sources'):
        for path,wanted in manifest[label].items():
            if digest(ROOT/path)!=wanted:errors.append({'integrity':path})
    freeze=json.loads((bank/'prepared-freeze.json').read_text())
    for path,wanted in freeze['inputs'].items():
        if digest(ROOT/path)!=wanted:errors.append({'freeze':path})
    for ext in ('asi','pdb'):
        paths=list((bank/'artifacts').glob('gta_reversed-*.'+ext))
        if len(paths)!=1 or digest(paths[0])!=manifest[ext]:errors.append({'artifact':ext})
    spec=importlib.util.spec_from_file_location('independent_bank_model',bank/'model.py');model=importlib.util.module_from_spec(spec);spec.loader.exec_module(model)
    summary=json.loads((run/'enriched-summary.json').read_text())
    if summary.get('natural_exit') is not True or summary.get('exit_code')!=0:errors.append('natural exit')
    if summary.get('executed_probe_sha256')!=manifest['asi']:errors.append('executed ASI')
    for flag in ('saves_unchanged','configs_restored'):
        if summary.get(flag) is not True:errors.append(flag)
    for flag in ('restore_error','runner_cleanup_error'):
        if summary.get(flag):errors.append({flag:summary[flag]})
    raw=run/'comparisons.jsonl';zipped=run/'comparisons.jsonl.gz'
    if not raw.exists() and not zipped.exists() and (run/'actual-bank.jsonl').exists():raw=run/'actual-bank.jsonl'
    storage=summary.get('comparison_storage')
    if storage:
        if raw.exists() and digest(raw)!=storage['uncompressed_sha256']:errors.append('raw storage SHA')
        if not zipped.exists() or digest(zipped)!=storage['compressed_sha256']:errors.append('compressed storage SHA')
        if zipped.exists():
            decoded=hashlib.sha256()
            with gzip.open(zipped,'rb') as stream:
                for block in iter(lambda:stream.read(4*1024*1024),b''):decoded.update(block)
            if decoded.hexdigest()!=storage['uncompressed_sha256']:errors.append('gzip roundtrip')
    log=gzip.open(zipped,'rt',encoding='utf8') if not raw.exists() else raw.open(encoding='utf8')
    counts=collections.Counter();diffs=collections.Counter();models=collections.Counter();gates=collections.Counter();ids=set();pairs=set();begin=None;terminal=None;globaldiff=0
    arrest=bank.name=='RequestedArrestRemainingFour'
    with log:
        for line in log:
            row=json.loads(line);event=row.get('event')
            if event=='begin':
                if begin is not None:errors.append('duplicate begin')
                begin=row
            elif event=='error':errors.append(row)
            elif event=='complete':
                if terminal is not None:errors.append('duplicate terminal')
                terminal=row
            elif event=='comparison':
                name=row['function'].split('::')[1];fixture=row['fixture'];key=(name,fixture['index']);pair=row['pair']
                if key in ids or pair in pairs:errors.append({'duplicate':key})
                ids.add(key);pairs.add(pair);counts[name]+=1;model.validate_fixture(name,fixture)
                required=('vptr_role_exact','helper_gates','target_gate','private_immutable') if arrest else ('vptr_role_exact','helper_gates','target_gate','names_immutable','textures_immutable')
                independently_equal=row['original']['payload']==row['candidate']['payload'] and all(row[leg].get(k) is True for leg in ('original','candidate') for k in required)
                if row['equal'] is not independently_equal:errors.append({'serialized_equal_disagrees':pair})
                unequal=not independently_equal;diffs[name]+=unequal;globaldiff+=unequal
                for leg in ('original','candidate'):
                    value=row[leg];wanted=model.expected(name,fixture,value)
                    models[name]+=any(value['payload'].get(k)!=v for k,v in wanted.items())
                    table=('native_' if leg=='original' else 'candidate_')+('task_table' if name=='Destructor' else 'arrest_table') if arrest else ('native_vtable' if leg=='original' else 'candidate_vtable')
                    required=('vptr_role_exact','helper_gates','target_gate','private_immutable') if arrest else ('vptr_role_exact','helper_gates','target_gate','names_immutable','textures_immutable')
                    gates[name]+=not(begin and value['raw_vptr']==begin[table] and value['expected_vptr']==begin[table] and all(value.get(k) is True for k in required))
    if dict(counts)!=model.COUNTS or pairs!=set(range(sum(model.COUNTS.values()))) or any({i for n,i in ids if n==name}!=set(range(count)) for name,count in model.COUNTS.items()):errors.append('complete exact corpus')
    if not terminal or terminal.get('comparisons')!=sum(model.COUNTS.values()) or terminal.get('differences')!=globaldiff or terminal.get('all_equal')!=(globaldiff==0) or any(terminal.get(k) is not True for k in ('state_restored','fp_restored')):errors.append('terminal totals/restoration')
    stages=[json.loads(line) for line in (run/'harness.jsonl').read_text().splitlines()]
    if [x['event'] for x in stages]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:errors.append('harness sequence')
    end=stages[-1]
    if end.get('changed_ranges')!=[] or end.get('snapshot_ranges')!=17692 or any(end.get(k) is not True for k in ('state_restored','entries_restored','flags_restored','timer_restored','tables_restored')):errors.append('harness restoration')
    return dict(global_errors=errors,bank_passed=summary.get('passed'),functions={name:dict(comparisons=counts[name],differences=diffs[name],model_mismatches=models[name],gate_failures=gates[name],subset_eligible=not errors and not(diffs[name] or models[name] or gates[name])) for name in model.COUNTS},scope='Complete corpus and global restoration required; only exact independently modeled function subsets eligible. No registry writes.',manifest_sha256=digest(bank/'artifacts/manifest.json'))
if __name__=='__main__':
    bank=ROOT/'reversing'/sys.argv[1];run=Path(sys.argv[2]).resolve();result=audit(bank,run);print(json.dumps(result,indent=2));raise SystemExit(bool(result['global_errors']))
