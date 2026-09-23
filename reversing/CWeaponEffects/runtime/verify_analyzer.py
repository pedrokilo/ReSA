"""Reject missing or altered in-game evidence; uses the local comparison log."""
import copy
import json
import tempfile
from pathlib import Path
import run_local

def verify():
    rows=[json.loads(line) for line in (Path(__file__).parent/'engine-comparisons.jsonl').read_text().splitlines()]
    with tempfile.TemporaryDirectory() as folder:
        path=Path(folder)/'case.jsonl'
        def write(data):path.write_text(''.join(json.dumps(row)+'\n' for row in data),encoding='utf-8')
        write(rows)
        assert run_local.analyze(path)['pairs']==301
        rejected=[]
        for case in ('empty','incomplete','duplicate','geometry','color','hook','missing_target','assertion'):
            data=copy.deepcopy(rows)
            if case=='empty':data=[]
            elif case=='incomplete':data=[r for r in data if r['event']!='complete']
            elif case=='duplicate':data.append(next(r for r in data if r['event']=='comparison'))
            elif case in ('geometry','color'):
                row=next(r for r in data if r.get('group')=='triangles')
                command=next(c for c in row['draw_candidate'] if c['kind']=='triangle')
                command['numbers' if case=='geometry' else 'exact'][0]+=1
            elif case=='hook':next(r for r in data if r['event']=='comparison')['original_candidate_calls']=1
            elif case=='missing_target':
                row=next(r for r in data if r.get('group')=='coop_lock')
                for key in ('draw_original','draw_candidate'):
                    index=next(i for i,c in enumerate(row[key]) if c['kind']=='sprite');del row[key][index]
            else:data.append({'event':'assertion'})
            write(data)
            try:accepted=run_local.analyze(path) is not None
            except (RuntimeError,ValueError,TypeError):accepted=False
            assert not accepted,case
            rejected.append(case)
    return {'valid_log_accepted':True,'rejected':rejected}

if __name__=='__main__':print(json.dumps(verify(),indent=2))
