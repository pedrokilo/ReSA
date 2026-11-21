# Quita del archivo las envolturas de 3 lineas `sig {\n    ...0xADDR)(...);\n}` de las direcciones dadas y deja un comentario.
# Uso: python strip_wrappers.py <archivo.cpp> <lote> ADDR[=Nombre] ...   (ADDR en hex sin 0x)
import sys,re
path,lote=sys.argv[1],sys.argv[2]
lines=open(path,encoding='utf-8').read().split('\n')
for a in sys.argv[3:]:
    addr=a.split('=')[0].upper()
    pat=re.compile(r'\)0x'+addr+r'\)\(',re.I)
    hits=[i for i,l in enumerate(lines) if pat.search(l)]
    ok=False
    for i in hits:
        if lines[i-1].rstrip().endswith('{') and lines[i+1].strip()=='}':
            name=re.search(r'(\w+)\(',lines[i-1]).group(1)
            lines[i-1:i+2]=['// 0x%s (%s): reconstruida al final del archivo (lote %s).'%(addr,name,lote)]
            ok=True;break
    print(addr,'OK' if ok else 'NO ENCONTRADA (%d coincidencias)'%len(hits))
open(path,'w',encoding='utf-8',newline='\n').write('\n'.join(lines))
