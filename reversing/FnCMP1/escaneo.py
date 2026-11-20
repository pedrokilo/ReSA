"""Cribado automatico: para cada comparacion de float contra un literal en una funcion con direccion conocida,
busca en el EXE las comparaciones x87 contra esa misma constante y comprueba si alguna reproduce el conjunto de
casos {>,<,==,NaN} que el C++ pretende. Si hay constantes iguales pero ninguna encaja -> POSIBLE DIFERENTE."""
import re,os,csv,struct,sys,functools
sys.path.insert(0,os.path.dirname(__file__))
import importlib.util
spec=importlib.util.spec_from_file_location('fcmp',os.path.join(os.path.dirname(__file__),'fcmp.py'))
F=importlib.util.module_from_spec(spec); spec.loader.exec_module(F)
root='D:/Original/wv-int/'
src=root+'source/game_sa'
hooks={}
for r in csv.DictReader(open(root+'docs/hooks.csv')):
    try: a=int(r['address'],16)
    except: continue
    if a: hooks[(r['class'],r['fn_name'])]=a
lit=r'(-?\d+\.\d*(?:[eE][-+]?\d+)?[fF]?|-?\d+[fF]|-?\.\d+[fF]?)'
ident=r'[A-Za-z_][\w\.\->\(\)\[\]:\*]*'
rx_r=re.compile(r'(!\(\s*)?(\(?'+ident+r'\)?)\s*(<=|>=|<|>)\s*'+lit+r'(?![\w\.])')
rx_l=re.compile(r'(!\(\s*)?'+lit+r'\s*(<=|>=|<|>)\s*('+ident+r')')
CASES=['GT','LT','EQ','NaN']
def P(op,neg):
    s={'>':{'GT'},'>=':{'GT','EQ'},'<':{'LT'},'<=':{'LT','EQ'}}[op]
    if neg: s=set(CASES)-s
    return frozenset(s)
SWAP={'GT':'LT','LT':'GT','EQ':'EQ','NaN':'NaN'}
@functools.lru_cache(None)
def an(addr):
    try: return F.analyze(addr,0x3000)
    except Exception as e: return []
def f32(s):
    s=s.rstrip('fF'); return struct.unpack('<f',struct.pack('<f',float(s)))[0]
def exe_cmps(addr):
    out=[]
    for a,pre,desc,outs,prev in an(addr):
        if not pre or not outs: continue
        J=frozenset(o.split(':')[0] for o in outs if o.endswith('SALTA'))
        Fl=frozenset(CASES)-J
        m=re.search(r'\[(0x[0-9a-f]+)\]',pre.op_str)
        if m and 'dword' in pre.op_str:
            try: v=struct.unpack('<f',F.rd(int(m.group(1),16),4))[0]; out.append((a,v,J,Fl,desc,'mem'))
            except: pass
        # constante cargada en st0 antes de comparar con una variable
        for q in reversed(prev):
            if q.mnemonic=='fld' and 'dword ptr [0x' in q.op_str:
                mm=re.search(r'\[(0x[0-9a-f]+)\]',q.op_str)
                try: v=struct.unpack('<f',F.rd(int(mm.group(1),16),4))[0]; out.append((a,v,J,Fl,desc,'st0'))
                except: pass
                break
            if q.mnemonic=='fld1': out.append((a,1.0,J,Fl,desc,'st0')); break
            if q.mnemonic=='fldz': out.append((a,0.0,J,Fl,desc,'st0')); break
            if q.mnemonic in ('fld','fxch','fcom','fcomp','fstp'): break
    return out
def functions(path):
    L=open(path,encoding='utf-8',errors='replace').read().split('\n')
    inst={}
    for l in L:
        for m in re.finditer(r'RH_Scoped\w*Install\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+)',l):
            inst[m.group(1)]=int(m.group(2),16)
    return L,inst
def enclosing(L,i):
    j=i
    while j>=0:
        s=L[j]
        if s and not s[0].isspace() and not s.startswith(('//','#','}','{','namespace','using','template')) and '(' in s and not s.startswith(('if','else','for','while','return','switch')):
            return j
        j-=1
    return None
res=[]
for dp,dn,fn in os.walk(src):
    for f in fn:
        if not f.endswith('.cpp') or 'RenderWare' in dp: continue
        p=os.path.join(dp,f); L,inst=functions(p)
        for i,l in enumerate(L):
            if l.lstrip().startswith('//'): continue
            code=l.split('//')[0]
            hits=[]
            for m in rx_r.finditer(code):
                hits.append((bool(m.group(1)),m.group(3),m.group(4),False,m.group(2)))
            for m in rx_l.finditer(code):
                op={'<':'>','>':'<','<=':'>=','>=':'<='}[m.group(3)]
                hits.append((bool(m.group(1)),op,m.group(2),False,m.group(4)))
            if not hits: continue
            j=enclosing(L,i)
            if j is None: continue
            sig=L[j]
            m=re.search(r'(\w+)::(~?\w+)\s*\(',sig)
            addr=None
            if m:
                addr=hooks.get((m.group(1),m.group(2))) or inst.get(m.group(2))
            else:
                m2=re.search(r'(\w+)\s*\(',sig)
                if m2: addr=inst.get(m2.group(1))
            if not addr: continue
            for neg,op,k,_,expr in hits:
                try: kv=f32(k)
                except: continue
                ps=P(op,neg)
                cands=[c for c in exe_cmps(addr) if abs(c[1]-kv)<=1e-6*max(1,abs(kv)) ]
                if not cands: verdict='SIN_ANCLA'; det=''
                else:
                    gtIn='GT' in ps; ltIn='LT' in ps
                    good=[]; sepOk=0
                    for a,v,J,Fl,desc,kind in cands:
                        pp=ps if kind=='mem' else frozenset(SWAP[c] for c in ps)
                        gI='GT' in pp; lI='LT' in pp
                        if gI==lI: good.append(a); continue
                        want='GT' if gI else 'LT'; other='LT' if gI else 'GT'
                        B=J if want in J else Fl
                        if other in B: continue
                        sepOk+=1
                        if ('NaN' in B)==('NaN' in pp): good.append(a)
                    if good: verdict='OK'
                    elif sepOk: verdict='POSIBLE_DIFERENTE_NAN'
                    else: verdict='SIN_PATRON'
                    det=';'.join('%x:%s:%s'%(c[0],c[5],c[4]) for c in cands[:4])
                res.append((os.path.relpath(p,root).replace(chr(92),'/'),i+1,sig.strip()[:60],hex(addr),code.strip()[:90],('!' if neg else '')+op+' '+k,verdict,det))
w=csv.writer(open(os.path.join(os.path.dirname(__file__),'escaneo.csv'),'w',newline='',encoding='utf-8'))
w.writerow(['archivo','linea','funcion','dir_exe','codigo','predicado','resultado','compares_exe_con_esa_constante'])
for r in res: w.writerow(r)
from collections import Counter
print(Counter(r[6] for r in res))
