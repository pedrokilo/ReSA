# Lista inicios de funcion probables (prologos MSVC8) en el CRT del EXE y marca los que no tienen nombre en docs/hooks.csv (solo lectura).
import sys,csv,re,struct
sys.path.insert(0,'.')
from desens import rd
names={}
for r in csv.DictReader(open('../../docs/hooks.csv',encoding='utf-8')):
    try: names[int(r['address'],16)]=r['fn_name']
    except: pass
for f in __import__('glob').glob('../../source/game_sa/Crt/*.cpp'):
    for m in re.finditer(r'RH_ScopedGlobalInstall\(\s*([\w:]+)\s*,\s*0x([0-9A-Fa-f]+)\)',open(f,encoding='utf-8').read()): names[int(m.group(2),16)]=m.group(1)
lo,hi=int(sys.argv[1],16),int(sys.argv[2],16)
d=rd(lo,hi-lo)
for i in range(len(d)-8):
    a=lo+i
    if d[i:i+5]==b'\x8b\xff\x55\x8b\xec' or (d[i:i+3]==b'\x55\x8b\xec' and (i==0 or d[i-1] in (0xCC,0xC3,0x90) or d[i-1]==0xC2 or True) and d[i-1]!=0xFF) or (d[i]==0x6A and d[i+2]==0x68 and d[i+7]==0xE8):
        print(hex(a),names.get(a,'?'))
