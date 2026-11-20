"""Aplica el parche minimo de NaN a las comparaciones listadas en un CSV (archivo,linea,occ,dir_exe_cmp).
`a OP b` -> `!(a OP' b)` (OP' = complemento ordenado) cuando el EXE manda el NaN al lado verdadero; `!(a OP b)` -> `(a OP' b)` cuando lo manda al falso.
Uso: python -I patch2.py seleccion.csv   (modifica source/ del worktree; no compila)"""
import sys,csv,os,re,importlib.util,collections
HERE=os.path.dirname(os.path.abspath(__file__))
spec=importlib.util.spec_from_file_location('scan2',os.path.join(HERE,'scan2.py')); S=importlib.util.module_from_spec(spec); spec.loader.exec_module(S)
CO={'<':'>=','>':'<=','<=':'>','>=':'<'}
sel=collections.defaultdict(list)
for r in csv.DictReader(open(sys.argv[1],encoding='utf-8')): sel[r['archivo']].append(r)
done=0; skipped=[]
for rel,rows in sel.items():
    p=S.ROOT+rel; raw=open(p,encoding='utf-8',newline='').read()
    nl='\r\n' if '\r\n' in raw else '\n'
    L,inst,funcs=S.parse_functions(p)
    L=[x.rstrip('\r') for x in L]
    edits=collections.defaultdict(list)
    for (i,j,k,sig,ctx) in funcs:
        cs=S.cpp_compares(L,j,k,sig)
        for r in rows:
            ln=int(r['linea'])
            if not (j<=ln-1<=k): continue
            for c in cs:
                if c['line']==ln and c['nocc']==int(r['occ']):
                    edits[ln].append((c,r))
    for ln,lst in edits.items():
        line=L[ln-1]
        for c,r in sorted(lst,key=lambda t:-t[0]['pos']):
            if not c['same'] or c['op'] not in CO: skipped.append((rel,ln,'no automatico')); continue
            code=S.mask(line.split('//')[0]); pos=c['pos']; op=c['op']
            Lx,Rx,neg,(lo,jj)=S.operands(code,pos,len(op))
            ls=lo+1
            while code[ls]==' ': ls+=1
            mret=re.match(r'return\s*',code[ls:])
            if mret: ls+=mret.end()
            re_=jj
            while code[re_-1]==' ': re_-=1
            Ltxt=line[ls:pos].rstrip(); Rtxt=line[pos+len(op):re_].strip()
            if neg:
                k2=lo-1
                while code[k2]==' ': k2-=1
                assert code[k2]=='!'
                new='(%s %s %s)'%(Ltxt,CO[op],Rtxt); a,b=k2,jj+1
            else:
                new='!(%s %s %s)'%(Ltxt,CO[op],Rtxt); a,b=ls,re_
            line=line[:a]+new+line[b:]; done+=1
        addrs=sorted(set(r['dir_exe_cmp'] for c,r in lst))
        line=line+' // NaN como el EXE (%s): fcom + test ah + jcc'%','.join('0x'+a for a in addrs)
        L[ln-1]=line
    open(p,'w',encoding='utf-8',newline='').write(nl.join(L))
print('parcheadas',done,'omitidas',skipped)
