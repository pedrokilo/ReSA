# Anota un desensamblado con el nombre propio de cada `call 0xDIR` (o push/jmp/mov de direccion de funcion): de RH_ScopedGlobalInstall, de comentarios `// 0xDIR` de los .h y de docs/hooks.csv.
import csv,re,sys,glob
names={}
for r in csv.DictReader(open('docs/hooks.csv',encoding='utf-8')):
    try: names[int(r['address'],16)]=r['fn_name']
    except: pass
for f in glob.glob('source/**/*.cpp',recursive=True)+glob.glob('source/**/*.h',recursive=True):
    t=open(f,encoding='utf-8',errors='replace').read()
    for m in re.finditer(r'RH_ScopedGlobalInstall\(\s*(\w+)\s*,\s*0x([0-9A-Fa-f]{6,8})\)',t): names[int(m.group(2),16)]=m.group(1)
    if f.endswith('.h'):
        for m in re.finditer(r'(\w+)\s*\([^;{}]*\)\s*;\s*//\s*0x([0-9A-Fa-f]{6,8})\b',t): names.setdefault(int(m.group(2),16),m.group(1))
for l in open(sys.argv[1],encoding='utf-8'):
    l=l.rstrip('\n')
    m=re.search(r'0x([0-9a-f]{6,7})$',l)
    if m and int(m.group(1),16) in names: l+='    ; '+names[int(m.group(1),16)]
    print(l)
