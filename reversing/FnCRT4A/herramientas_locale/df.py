# Desensambla una funcion del EXE hasta la siguiente entrada del mapa de funciones (docs/MemoryMap/binary_functions.csv).
import sys,csv,bisect,os
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from desens import dis,iat
import re
IAT=iat()
def ann(l):
    return re.sub(r'\[0x(8[0-9a-f]{5})\]',lambda m:'[%s]'%IAT.get(int(m.group(1),16),'0x'+m.group(1)) if int(m.group(1),16) in IAT else m.group(0),l)
root=os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..'))
vas=sorted(int(r['va'],16) for r in csv.DictReader(open(os.path.join(root,'docs/MemoryMap/binary_functions.csv'),encoding='utf-8')))
for s in sys.argv[1:]:
    a=int(s,16); i=bisect.bisect_right(vas,a); e=vas[i] if i<len(vas) else a+512
    print(f"=== {a:x}..{e:x} ({e-a})"); print("\n".join(dis(a,e-a)))
