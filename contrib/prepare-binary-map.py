"""Prepara entradas candidatas para el analizador sin modificar el EXE."""
import argparse,csv,json,sys
from pathlib import Path
sys.dont_write_bytecode=True
from pe_inventory import PE,hx,rtti_map

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
    p.add_argument('--exe',type=Path,default=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'));p.add_argument('--output',type=Path)
    a=p.parse_args();root=a.root.resolve();pe=PE(a.exe);out=a.output or root/'build/binary-map';out.mkdir(parents=True,exist_ok=True)
    def rows(name):
        with (root/'docs/MemoryMap'/name).open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
    seeds=set()
    for r in rows('functions.csv')+rows('registrations.csv'):
        if r.get('va') and pe.code(int(r['va'],16)):seeds.add(int(r['va'],16))
    for t in rows('vtables.csv'):
        try:count=int(t['slots_declared'],0)
        except ValueError:continue
        for index in range(min(count,512)):
            words=pe.words(int(t['va'],16)+4*index,1)
            if words and pe.code(words[0]):seeds.add(words[0])
    _,_,slots=rtti_map(pe,{})
    seeds.update(int(s['function_va'],16) for s in slots)
    (out/'seeds.txt').write_text('\n'.join(hx(s) for s in sorted(seeds))+'\n')
    print(f'{len(seeds)} entradas candidatas preparadas; EXE intacto.')
if __name__=='__main__':main()
