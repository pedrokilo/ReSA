"""Read-only reproduction of the first720 native-reference fixture audit."""
import collections,gzip,itertools,json,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
RUN=ROOT/'reversing/RequestedAnimProjectilePair/runs/20261008-142758-927336'
def main():
    seen=set();differences=collections.Counter();native_model=True
    with gzip.open(RUN/'comparisons.jsonl.gz','rt',encoding='utf8') as stream:
        for line in itertools.islice(stream,720):
            x=json.loads(line);assert x['function']=='InteriorGroup_c::ReferenceAnims'
            key=(x['group'],x['referenced'],x['index'],x['loaded'],x['poison']);assert key not in seen;seen.add(key)
            for side in ['original','candidate']:assert x[side]['gate'] is True and x[side]['collaborators_valid'] is True
            native_model &= x['original']['bytes']==x['model_bytes'] and x['original']['trace']==x['model_trace']
            if not x['equal']:differences[(x['loaded'],x['referenced'])]+=1
    expected={(g,r,i,l,p) for g in range(3) for r in [0,1,-1] for i in [0,1,127,179] for l in range(5) for p in [0,85,170,255]}
    assert seen==expected and native_model and dict(differences)=={(1,0):48}
    summary=json.loads((RUN/'enriched-summary.json').read_text(encoding='utf8'));assert summary['passed'] is False and summary['error']=='Tiempo agotado.'
    print(json.dumps({'diagnostic_only':True,'native_model_equal':native_model,'cases':len(seen),'differences':48,'scope':'Original mixed run incomplete; no approval'}))
if __name__=='__main__':main()
