"""Profundidad maxima de la pila x87 por funcion de un listado .cod de MSVC (/FAs). uso: python codepth.py x.cod regex_de_funciones"""
import sys,re
text=open(sys.argv[1],encoding='latin1').read()
want=re.compile(sys.argv[2])
RET1=re.compile(r'(JpFixMemOp|FormationRootCpp|ExtCpp|NormaliseMagCpp|VehHeadTailDirectionCpp|VehHeadTailXx)')
def delta(m,ops):
    if m in('fld','fldz','fld1','fild','fldpi'): return 1
    if m in('fstp','fistp','fisttp','fcomp','fucomp','fiscomp','ficomp'): return -1
    if m in('fcompp','fucompp'): return -2
    if m in('faddp','fmulp','fsubp','fsubrp','fdivp','fdivrp'): return -1
    if m=='fpatan': return -1
    return 0
for pm in re.finditer(r'^(\S+)\s+PROC.*?^\1\s+ENDP',text,re.S|re.M):
    name=pm.group(1)
    if not want.search(name): continue
    body=pm.group(0).splitlines()
    ins=[];labels={}
    for l in body[1:]:
        l=l.split(';')[0].rstrip()
        if not l.strip(): continue
        if not l.startswith('\t') and l.rstrip().endswith(':'): labels[l.strip()[:-1]]=len(ins); continue
        parts=l.strip().split(None,1)
        ins.append((parts[0],parts[1] if len(parts)>1 else ''))
    seen={};stack=[(0,0)];maxd=0;rets=set()
    while stack:
        i,d=stack.pop()
        while i<len(ins):
            if seen.get((i,d)): break
            seen[(i,d)]=1
            m,o=ins[i]
            if m.startswith('f'): 
                d+=delta(m,o)
            if m=='call':
                if RET1.search(o): d+=1
                if '__ftol2' in o: maxd=max(maxd,d+1); d-=1
            if d<0: print('  UNDERFLOW',name,i,m,o)
            maxd=max(maxd,d)
            if m=='ret': rets.add(d); break
            if m=='jmp': i=labels.get(o.replace('SHORT ','').strip(),len(ins)); continue
            if m.startswith('j'): stack.append((labels.get(o.replace('SHORT ','').strip(),len(ins)),d))
            i+=1
    print('%-40s max=%d ret=%s'%(name,maxd,sorted(rets)))
