# Lista los usos de las 96 direcciones de S26A-S29A en el codigo propio (source/), sin comentarios de linea completa.
import csv,re,os
rows=list(csv.DictReader(open('docs/ExeIndependiente/raices.csv',encoding='utf-8')))
sel=sorted([r for r in rows if r['lote'] in('S26A','S27A','S28A','S29A')],key=lambda r:r['direccion'])
pat=re.compile(r'0x0*(7[4-6][0-9A-Fa-f]{4})\b')
hits=[]
for dp,dn,fn in os.walk('source'):
    for f in fn:
        if not f.endswith(('.cpp','.h','.hpp','.inc')): continue
        p=os.path.join(dp,f)
        for i,l in enumerate(open(p,encoding='utf-8',errors='replace').read().split('\n'),1):
            for m in pat.finditer(l): hits.append((int(m.group(1),16),p.replace(chr(92),'/'),i,l.strip()))
out=open('reversing/FnS26A29A/sitios.txt','w',encoding='utf-8')
for r in sel:
    a=int(r['direccion'],16)
    out.write("## %s %X\n"%(r['lote'],a))
    for x in hits:
        if x[0]==a and not x[3].startswith('//'): out.write("%s:%d: %s\n"%(x[1],x[2],x[3][:230]))
out.close()
