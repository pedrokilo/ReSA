"""Compare 784 independent original/candidate colour grids inside GTA."""
import itertools
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = '31f0abcaad1fd61e590e4aa26b3da26c01ae224f1d4883a18d26349b129dee00'

def fixture_palettes():
    """Reproduce test inputs only; expected grids always come from GTA's original."""
    result=[]
    for mode in range(5):
        palette=[]
        for i in range(128):
            rgb=([0,0,0],[255,255,255],[i*2]*3,[(i%8)*32,((i//8)%8)*32,(i//64)*128],[14 if i%2 else 128]*3)[mode]
            palette.extend(rgb+[i])
        result.append(palette)
    for channel,base,delta in itertools.product(range(3),(0,14,15,128,240,255),(-16,-15,-14,0,14,15,16)):
        palette=[base,base,base,73]*128
        palette[4+channel]=min(255,max(0,base+delta))
        result.append(palette)
    random=0xC0FFEE
    for seed in range(64):
        palette=[]
        for _ in range(512):
            random=(random*1664525+1013904223)&0xFFFFFFFF
            palette.append(random>>24)
        result.append(palette)
    return result

def analyze(path):
    if not path.exists(): return None
    data = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in data.splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('error','assertion') for r in rows): raise RuntimeError('Error registrado dentro de GTA.')
    if not any(r['event']=='complete' for r in rows): return None
    pairs = [r for r in rows if r['event']=='comparison']
    palettes = [r for r in rows if r['event']=='palette']
    ready = [r for r in rows if r['event']=='player_ready']
    completed = [r for r in rows if r['event']=='complete']
    if len(ready)!=1 or ready[0]['palettes']!=196 or not ready[0]['independent_inputs']: raise RuntimeError('Fixture incompleta.')
    if len(palettes)!=196 or {r['id'] for r in palettes}!=set(range(196)) or any(len(r['rgba'])!=512 or any(type(x)!=int or not 0<=x<=255 for x in r['rgba']) for r in palettes): raise RuntimeError('Paletas incompletas.')
    by_id={r['id']:r['rgba'] for r in palettes}
    if any(by_id[i]!=palette for i,palette in enumerate(fixture_palettes(),1)): raise RuntimeError('Entradas del banco modificadas.')
    expected = set(itertools.product(range(196),range(2),(0,165)))
    if len(pairs)!=784 or {r['pair'] for r in pairs}!=set(range(784)) or {(r['palette'],r['menu'],r['poison']) for r in pairs}!=expected: raise RuntimeError('Casos incompletos o duplicados.')
    for r in pairs:
        if not all(r[k] for k in ('equal','full_storage_equal','bounded_writes','inputs_unchanged','independent_inputs')) or r['original_candidate_calls']!=0 or r['candidate_calls']!=1: raise RuntimeError('Estado divergente en caso '+str(r['pair']))
        if r['original']!=r['candidate'] or len(r['original'])!=64 or any(type(x)!=int or not 0<=x<128 for x in r['original']): raise RuntimeError('Cuadricula diferente.')
    if len(completed)!=1 or completed[0]['pairs']!=784 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
    fallback_entries=sum(r['original'].count(127) for r in pairs)
    max_unique=max(len(set(r['original'])) for r in pairs)
    if fallback_entries==0 or max_unique!=64: raise RuntimeError('Falta cobertura de rechazo o seleccion.')
    return dict(pairs=784,palettes=196,all_equal=True,grid_entries_compared=784*64,independent_inputs=True,bounded_writes=True,inputs_unchanged=True,candidate_calls=784,state_restored=True,fallback_entries=fallback_entries,max_unique_colours=max_unique,log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else: sys.exit(run_case('menusystem',PROBE_HASH,analyze,'CMenuSystem::FillGridWithCarColours'))
