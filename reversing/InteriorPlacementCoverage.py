"""Read-only advisory coverage report for InteriorPlacement.
Reads comparisons/summary only; never validates manifests, changes gates or writes files.
"""
import argparse
import collections
import json
from pathlib import Path

def word(value, offset=0):
    if isinstance(value,list) and len(value)>=offset+4:
        return int.from_bytes(bytes(value[offset:offset+4]),'little',signed=True)
    return None

def audit(path):
    path=path/'comparisons.jsonl' if path.is_dir() else path
    functions={}; errors=[]; pairs=[]
    with path.open('r',encoding='utf-8-sig') as stream:
        for line_number,line in enumerate(stream,1):
            if not line.strip():continue
            try:record=json.loads(line)
            except ValueError as error:
                errors.append({'line':line_number,'error':str(error)});continue
            if record.get('event')!='comparison':continue
            name=record['function'];case=int(record['case']);original=record['original']
            counts=functions.setdefault(name,collections.Counter())
            counts['cases']+=1;counts['equal']+=record.get('equal') is True
            counts['unequal']+=record.get('equal') is not True
            counts['return_nonzero']+=original.get('result',0)!=0
            counts['return_nil']+=original.get('result',0)==0
            pairs.append(record.get('pair'))
            events=collections.Counter()
            for event in original.get('trace',[]):
                if not isinstance(event,list) or not event:continue
                tag=event[0];events[tag]+=1
                if tag=='allocate' and len(event)>1:counts[f'allocation_size_{event[1]}']+=1
            for tag,number in events.items():
                counts[f'trace_{tag}']+=number;counts[f'cases_{tag}']+=1
            counts['placement_retry_cases']+=events['place']>1
            if name.endswith('PlaceFurnitureOnWall'):counts['placement_100_attempt_cases']+=events['place']>=100
            if name.endswith('PlaceFurnitureInCorner'):counts['placement_20_attempt_cases']+=events['place']>=20
            counts['allocation_cases']+=events['allocate']>0
            # These classifications explicitly depend on the declared callback contract.
            counts['find_configured_negative_cases']+=events['find']>0 and case%4==1
            counts['find_configured_positive_cases']+=events['find']>0 and case%4!=1
            object_count=word(original.get('objects'))
            if object_count is not None:counts['observed_store_count_positive_cases']+=object_count>0
            counts['observed_remove_and_delete_cases']+=events['remove']>0 and events['delete']>0
            pool=original.get('pool');pool_count=word(pool,8);head=word(pool,0)
            if pool_count is not None:
                counts['observed_pool_after_count_negative_cases']+=pool_count<0
                counts['observed_pool_after_count_zero_cases']+=pool_count==0
                counts['observed_pool_after_count_positive_cases']+=pool_count>0
                counts['observed_pool_after_positive_count_null_head_cases']+=pool_count>0 and head==0
    summary_path=path.parent/'summary.json';summary=None
    if summary_path.is_file():
        try:summary=json.loads(summary_path.read_text(encoding='utf-8-sig'))
        except ValueError as error:errors.append({'summary_error':str(error)})
    return {
        'report':str(path),'mode':'advisory/read-only; no manifest verification or certification',
        'summary_reported_passed':summary.get('passed') if summary is not None else None,
        'comparison_rows':len(pairs),'pair_ids_contiguous_in_file':pairs==list(range(len(pairs))),
        'functions':{name:dict(sorted(counts.items())) for name,counts in functions.items()},
        'parse_errors':errors,
        'limits':[
            'Counts describe original outputs/traces; equality is preserved verbatim from the report.',
            'Pool classifications describe AFTER state only. No modulo8 inference of pool input is made; negative-count scenarios are represented by observed signed counts.',
            'Find sign uses the deterministic callback contract scenario%4==1 -> -1, otherwise scenario%3. Its return is not directly exported in the trace.',
            'Store creation is observed positive object count after a fixture initialized with count zero; remove/delete is observed by both trace events.',
            'This tool does not impose hard gates, certify coverage, verify source hashes/manifests, run GTA/builds or write any file.'
        ]
    }

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('reports',nargs='+',type=Path)
    args=parser.parse_args()
    print(json.dumps([audit(path) for path in args.reports],ensure_ascii=False,indent=2))
