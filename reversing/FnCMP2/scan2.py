"""FnCMP2 - escaner de comparaciones float planas (con o sin literal) C++ <-> EXE.
Por funcion con direccion EXE conocida: extrae las comparaciones `< > <= >= == !=` con algun operando float (heuristica de tipos),
extrae las comparaciones x87 del EXE (fcom/fcomp/fucom + fnstsw + test/sahf + jcc), alinea ambas secuencias (LCS con compatibilidad de
particion {mayor,menor,igual}, y constante si la hay) y compara el lado del NaN. Salida: auditoria2_auto.csv (una fila por comparacion C++).
Uso: python -I scan2.py   (solo lectura del EXE; no lo ejecuta)"""
import re,os,csv,struct,sys,functools,collections
HERE=os.path.dirname(os.path.abspath(__file__))
import importlib.util
spec=importlib.util.spec_from_file_location('fcmp',os.path.join(HERE,'fcmp.py'))
F=importlib.util.module_from_spec(spec); spec.loader.exec_module(F)
ROOT=os.path.abspath(os.path.join(HERE,'..','..')).replace('\\','/')+'/'
SRC=ROOT+'source/game_sa'
CASES=['GT','LT','EQ','NaN']
hooks={}
for r in csv.DictReader(open(ROOT+'docs/hooks.csv',encoding='utf-8')):
    try: a=int(r['address'],16)
    except: continue
    if a: hooks[(r['class'],r['fn_name'])]=a
# ---------- tipos float ----------
FL=collections.Counter(); IN=collections.Counter()
rx_fdecl=re.compile(r'\b(?:float|double)\s+(?:\*\s*)?(\w+)')
rx_idecl=re.compile(r'\b(?:u?int(?:8|16|32|64)?(?:_t)?|short|long|char|bool|size_t|eModelID|eWeaponType|eVehicleType|auto)\s+(\w+)\s*[;=,\[)]')
for dp,dn,fn in os.walk(SRC):
    for f in fn:
        if f.endswith('.h'):
            t=open(os.path.join(dp,f),encoding='utf-8',errors='replace').read()
            for m in rx_fdecl.finditer(t): FL[m.group(1)]+=1
            for m in rx_idecl.finditer(t): IN[m.group(1)]+=1
for n in ('x','y','z','w'): FL[n]+=1000
FLOATSET={n for n in FL if FL[n]>=IN.get(n,0)}
FLOATFUN=set('Magnitude Magnitude2D SquaredMagnitude SquaredMagnitude2D Dot DotProduct DotProduct2D Distance Distance2D DistanceSquared Distance2DSquared sqrt sqrtf fabs fabsf sin cos atan2 acos asin Length GetLength cosf sinf'.split())
rx_flit=re.compile(r'(?<![\w.])(\d+\.\d*(?:[eE][-+]?\d+)?[fF]?|\d+[fF]|\.\d+[fF]?)(?![\w.])')
rx_id=re.compile(r'[A-Za-z_]\w*')
def lit_of(s):
    s=s.strip()
    m=re.fullmatch(r'\(?\s*([-+]?)\s*(\d+\.\d*(?:[eE][-+]?\d+)?[fF]?|\d+[fF]|\.\d+[fF]?)\s*\)?',s)
    if m:
        try: return struct.unpack('<f',struct.pack('<f',float(m.group(2).rstrip('fF'))*(-1 if m.group(1)=='-' else 1)))[0]
        except: return None
    return None
def is_float(expr,local_f,local_i):
    if rx_flit.search(expr): return True
    ids=rx_id.findall(expr)
    for t in ids:
        if t in local_i: continue
        if t in local_f or t in FLOATFUN: return True
    for t in ids:
        if t in local_i: continue
        if t in FLOATSET and not t.startswith('m_n') and not t.startswith('m_ub'):
            return True
    return False
# ---------- parseo C++ ----------
def mask(code):
    code=re.sub(r'"(\\.|[^"\\])*"','""',code)
    code=re.sub(r"'(\\.|[^'\\])'","' '",code)
    code=code.replace('->','..').replace('<<','  ').replace('>>','  ')
    code=re.sub(r'\w<[\w:\*]+(?:,\s*[\w:\*]+)*>',lambda m:re.sub(r'[<>]','_',m.group(0)),code)
    return code
rx_op=re.compile(r'(<=|>=|==|!=|<|>)')
def operands(code,pos,oplen):
    i=pos-1; d=0
    while i>=0:
        c=code[i]
        if c in ')]': d+=1
        elif c in '([':
            if d==0: break
            d-=1
        elif d==0:
            if c in ',;?{}': break
            if c==':' and not(i>0 and code[i-1]==':') and not(i+1<len(code) and code[i+1]==':'): break
            if code[max(0,i-1):i+1] in('&&','||'): break
            if c=='=' and code[max(0,i-1):i+1] not in('==','<=','>=','!='): break
        i-=1
    L=code[i+1:pos]
    left_open=i
    j=pos+oplen; d=0
    while j<len(code):
        c=code[j]
        if c in '([': d+=1
        elif c in ')]':
            if d==0: break
            d-=1
        elif d==0:
            if c in ',;?{}': break
            if c==':' and not(code[j+1:j+2]==':') and not(j>0 and code[j-1]==':'): break
            if code[j:j+2] in('&&','||'): break
            if code[j:j+2] in('<=','>=','==','!=') or (c in '<>' and code[j+1:j+2]!='>'): break
        j+=1
    R=code[pos+oplen:j]
    neg=False
    if left_open>=0 and code[left_open]=='(' and j<len(code) and code[j]==')':
        k=left_open-1
        while k>=0 and code[k]==' ': k-=1
        if k>=0 and code[k]=='!': neg=True
    return L.strip(),R.strip(),neg,(left_open,j)
def parse_functions(path):
    L=open(path,encoding='utf-8',errors='replace').read().split('\n')
    inst={}
    for l in L:
        for m in re.finditer(r'RH_Scoped\w*Install\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+)',l): inst[m.group(1)]=int(m.group(2),16)
    funcs=[]; i=0; n=len(L)
    while i<n:
        s=L[i]
        if s and not s[0].isspace() and not s.startswith(('//','#','}','{','namespace','using','template','struct','class','enum','static_assert','extern')) and '(' in s and not s.rstrip().endswith(';'):
            j=i
            while j<n and '{' not in L[j] and j<i+8: j+=1
            if j<n and '{' in L[j]:
                k=j+1
                while k<n and not L[k].startswith('}'): k+=1
                sig=' '.join(x.strip() for x in L[i:j+1])
                funcs.append((i,j,min(k,n-1),sig,' '.join(L[max(0,i-2):i+1])))
                i=k+1; continue
        i+=1
    return L,inst,funcs
def addr_of(sig,ctx,inst):
    m=re.search(r'(\w+)::(~?\w+)\s*\(',sig)
    a=None
    if m: a=hooks.get((m.group(1),m.group(2))) or inst.get(m.group(2))
    else:
        m2=re.search(r'(\w+)\s*\(',sig)
        if m2: a=inst.get(m2.group(1))
    if not a:
        c=re.search(r'//\s*(0x[0-9A-Fa-f]{6})',ctx)
        if c: a=int(c.group(1),16)
    return a,(m.group(0)[:-1].strip() if m else sig[:40])
def cpp_compares(L,j,k,sig):
    txt='\n'.join(L[j:k+1])
    lf=set(); li=set()
    for m in rx_fdecl.finditer(sig+'\n'+txt): lf.add(m.group(1))
    for m in rx_idecl.finditer(sig+'\n'+txt):
        if m.group(1) not in lf: li.add(m.group(1))
    out=[]
    inblock=False
    for ln in range(j,k+1):
        raw=L[ln]
        code=raw
        if inblock:
            if '*/' in code: code=code.split('*/',1)[1]; inblock=False
            else: continue
        code=re.sub(r'/\*.*?\*/','',code)
        if '/*' in code: code=code.split('/*')[0]; inblock=True
        code=code.split('//')[0]
        if ln==j: code=code[code.find('{'):] if '{' in code else code
        c=mask(code)
        for m in rx_op.finditer(c):
            op=m.group(1); pos=m.start()
            Lx,Rx,neg,span=operands(c,pos,len(op))
            if not Lx or not Rx: continue
            if not (is_float(Lx,lf,li) or is_float(Rx,lf,li)): continue
            if re.search(r'\bnullptr\b|\bNULL\b|\btrue\b|\bfalse\b',Lx+Rx): continue
            out.append(dict(line=ln+1,op=op,neg=neg,L=Lx,R=Rx,code=raw.strip()[:110],pos=pos,span=span,nocc=sum(1 for q in out if q['line']==ln+1),same=(ln!=j and '/*' not in raw and len(c)==len(raw.split('//')[0]))))
    return out
def tset(op,neg):
    s={'>':{'GT'},'>=':{'GT','EQ'},'<':{'LT'},'<=':{'LT','EQ'},'==':{'EQ'},'!=':{'GT','LT','NaN'}}[op]
    s=set(s)
    if neg: s=set(CASES)-s
    return frozenset(s)
# ---------- EXE ----------
@functools.lru_cache(None)
def an(addr,mb):
    try: return F.analyze(addr,mb)
    except Exception as e: return []
def exe_list(addr,mb=0x6000):
    out=[]
    for a,pre,desc,outs,prev in an(addr,mb):
        if not pre or not outs: continue
        J=frozenset(o.split(':')[0] for o in outs if o.endswith('SALTA'))
        Fl=frozenset(CASES)-J
        const=None; kind=None
        if pre.mnemonic=='ftst': const=0.0; kind='mem'
        m=re.search(r'\[(0x[0-9a-f]+)\]',pre.op_str)
        if const is None and m and 'dword' in pre.op_str and pre.mnemonic.startswith('fcom'):
            try:
                const=struct.unpack('<f',F.rd(int(m.group(1),16),4))[0]; kind='mem'
            except: pass
        if const is None:
            for q in reversed([q for q in prev if q.address<pre.address]):
                if q.mnemonic=='fld' and 'dword ptr [0x' in q.op_str:
                    mm=re.search(r'\[(0x[0-9a-f]+)\]',q.op_str)
                    try: const=struct.unpack('<f',F.rd(int(mm.group(1),16),4))[0]; kind='st0'
                    except: pass
                    break
                if q.mnemonic=='fld1': const=1.0; kind='st0'; break
                if q.mnemonic=='fldz': const=0.0; kind='st0'; break
                if q.mnemonic in('fld','fxch','fcom','fcomp','fstp'): break
        out.append(dict(addr=a,J=J,Fl=Fl,const=const,kind=kind,desc=desc))
    out.sort(key=lambda e:e['addr'])
    return out
SW={'GT':'LT','LT':'GT','EQ':'EQ','NaN':'NaN'}
def sw(s): return frozenset(SW[c] for c in s)
def match(cmp,e):
    """None: incompatible; True/False: el lado del NaN coincide (cmp['T'] = casos con condicion verdadera)."""
    T=cmp['T']
    best=None
    for swap in (False,True):
        if cmp['lit'] is not None:
            if e['const'] is None or abs(e['const']-cmp['lit'])>1e-6*max(1,abs(cmp['lit'])): return None
            if (e['kind']=='st0')!=swap: continue
        Tx=(sw(T) if swap else T)
        for X in (e['J'],e['Fl']):
            if (X-{'NaN'})==(Tx-{'NaN'}):
                ok=(('NaN' in X)==('NaN' in Tx))
                if best is None or (ok and not best): best=ok
    return best

def ctype(Tn):
    """tipo de particion canonico (var-relativo): LT|GE, GT|LE, EQ|NE"""
    Tn=frozenset(Tn)
    if Tn in (frozenset({'LT'}),frozenset({'GT','EQ'})): return 'LT|GE'
    if Tn in (frozenset({'GT'}),frozenset({'LT','EQ'})): return 'GT|LE'
    return 'EQ|NE'
def cside(T):
    """lado donde cae el NaN: True si NaN esta en el lado que contiene LT (LT|GE), GT (GT|LE), EQ (EQ|NE)"""
    Tn=frozenset(T)-{'NaN'}; nan='NaN' in T
    key={frozenset({'LT'}):'a',frozenset({'GT','EQ'}):'b',frozenset({'GT'}):'a',frozenset({'LT','EQ'}):'b',frozenset({'EQ'}):'a',frozenset({'GT','LT'}):'b'}.get(Tn)
    return key if nan else ('b' if key=='a' else 'a')   # lado en el que cae el NaN
def eside(e,swap):
    J=e['J']; Fl=e['Fl']
    if swap: J=sw(J); Fl=sw(Fl)
    Xn=J-{'NaN'}
    t=ctype(Xn)
    first=(Xn in (frozenset({'LT'}),frozenset({'GT'}),frozenset({'EQ'})))
    nanJ='NaN' in J
    # lado 'a' es el estricto/unico (LT, GT, EQ)
    if first: return t,('a' if nanJ else 'b')
    return t,('b' if nanJ else 'a')
def ex_key(e):
    sh=(e['kind']=='st0')
    return eside(e,sh) if e['const'] is not None else eside(e,False)

def order_align(cs,ex):
    """Alineacion por orden (codigo fuente vs direccion) dentro de cada constante: devuelve {indice_cpp: (addr_exe, mismo_lado, n_cpp, n_exe)}"""
    out={}
    groups=collections.defaultdict(list)
    for ci,c in enumerate(cs):
        if c['lit'] is not None: groups[round(c['lit'],5)].append(ci)
    for k,cis in groups.items():
        es=[e for e in ex if e['const'] is not None and abs(round(e['const'],5)-k)<1e-4]
        cis=sorted(cis,key=lambda i:(cs[i]['line'],cs[i]['pos']))
        n,m=len(cis),len(es)
        if not m: continue
        ct=[(ctype(cs[i]['T']-{'NaN'}),cside(cs[i]['T'])) for i in cis]
        et=[ex_key(e) for e in es]
        D=[[0]*(m+1) for _ in range(n+1)]
        for i in range(n-1,-1,-1):
            for j in range(m-1,-1,-1):
                w=(2+(1 if ct[i][1]==et[j][1] else 0)) if ct[i][0]==et[j][0] else -99
                D[i][j]=max(D[i+1][j],D[i][j+1],D[i+1][j+1]+w)
        i=j=0
        while i<n and j<m:
            w=(2+(1 if ct[i][1]==et[j][1] else 0)) if ct[i][0]==et[j][0] else -99
            if w>0 and D[i][j]==D[i+1][j+1]+w: out[cis[i]]=(es[j]['addr'],ct[i][1]==et[j][1],n,m); i+=1; j+=1
            elif D[i][j]==D[i+1][j]: i+=1
            else: j+=1
    return out
def classify(cs,ex):
    """Emparejamiento sin orden para las ancladas (misma constante, mismo lado de NaN) y LCS para las no ancladas.
    Devuelve {indice_cpp: (veredicto, indice_exe|None, confianza)}."""
    used=set(); res={}
    for ci,c in enumerate(cs):
        if c['lit'] is None: continue
        for ej,e in enumerate(ex):
            if ej not in used and match(c,e) is True: used.add(ej); res[ci]=('IGUAL',ej,'alta'); break
    for ci,c in enumerate(cs):
        if c['lit'] is None or ci in res: continue
        diff=[ej for ej,e in enumerate(ex) if ej not in used and match(c,e) is False]
        if diff:
            res[ci]=('DIFERENTE_CAND',diff[0],'alta' if len(diff)==1 else 'media'); used.add(diff[0])
    rest=[ci for ci,c in enumerate(cs) if ci not in res and c['lit'] is None]
    free=[ej for ej in range(len(ex)) if ej not in used]
    n,m=len(rest),len(free)
    D=[[0]*(m+1) for _ in range(n+1)]
    ok=lambda i,j: match(cs[rest[i]],ex[free[j]]) is True
    for i in range(n-1,-1,-1):
        for j in range(m-1,-1,-1):
            D[i][j]=max(D[i+1][j],D[i][j+1],D[i+1][j+1]+(1 if ok(i,j) else -99))
    i=j=0
    while i<n and j<m:
        if ok(i,j) and D[i][j]==D[i+1][j+1]+1: res[rest[i]]=('IGUAL',free[j],'media'); used.add(free[j]); i+=1; j+=1
        elif D[i][j]==D[i+1][j]: i+=1
        else: j+=1
    for ci,c in enumerate(cs):
        if ci in res: continue
        diff=[ej for ej,e in enumerate(ex) if ej not in used and match(c,e) is False]
        if diff:
            res[ci]=('DIFERENTE_CAND',diff[0],'alta' if c['lit'] is not None and len(diff)==1 else 'baja')
        else: res[ci]=('INDETERMINADO',None,'')
    return res
WR=r'm_vecMoveSpeed|m_vecTurnSpeed|SetPosition|m_matrix|m_vecPos|ApplyMoveSpeed|ApplyTurnSpeed|SetVelocity|m_placement|GetMatrix|\.pos\b|m_fHeading|m_fCurrentRotation|GetPosition\(\)\s*[+\-]?='
def bstat(bc,be,c):
    t=ctype(c['T']-{'NaN'}); key=(round(c['lit'],6) if c['lit'] is not None else None,t)
    a=bc[key]; b=be[key]
    mine=cside(c['T'])
    est='igual' if sorted(a)==sorted(b) else ('disjunto' if b and not (set(a)&set(b)) else ('elem' if b and mine not in b else 'difiere'))
    return '%d/%d %s'%(len(a),len(b),est)
def main():
    rows=[]; stats=collections.Counter()
    for dp,dn,fn in os.walk(SRC):
        for f in sorted(fn):
            if not f.endswith('.cpp') or 'RenderWare' in dp: continue
            p=os.path.join(dp,f); L,inst,funcs=parse_functions(p)
            rel=os.path.relpath(p,ROOT).replace('\\','/')
            for (i,j,k,sig,ctx) in funcs:
                cs=cpp_compares(L,j,k,sig)
                if not cs: continue
                addr,name=addr_of(sig,ctx,inst)
                if not addr:
                    for c in cs: rows.append((rel,c['line'],name,'',c['code'],'SIN_DIRECCION','','','','','','','','',''))
                    continue
                body='\n'.join(L[j:k+1])
                writes=len(re.findall(WR,body))
                ex=exe_list(addr)
                for c in cs:
                    c['T']=tset(c['op'],c['neg'])
                    lit=lit_of(c['R']); l2=lit_of(c['L'])
                    if lit is not None and l2 is None: c['lit']=lit
                    elif l2 is not None and lit is None: c['lit']=l2; c['T']=sw(c['T'])
                    else: c['lit']=None
                for c in cs:
                    c['lit0']=c['lit']
                    if c['lit'] is not None and not any(e['const'] is not None and abs(e['const']-c['lit'])<=1e-6*max(1,abs(c['lit'])) for e in ex):
                        c['lit']=None; c['fb']=True   # la constante no se ve junto a ninguna comparacion del EXE: se trata como sin ancla
                R=classify(cs,ex)
                OA=order_align(cs,ex)
                litset=[c['lit'] for c in cs if c['lit'] is not None]
                def kclaim(e): return e['const'] is not None and any(abs(e['const']-k)<=1e-6*max(1,abs(k)) for k in litset)
                bc=collections.defaultdict(list); be=collections.defaultdict(list)
                for c in cs:
                    t=ctype(c['T']-{'NaN'}); key=(c['lit'] if c['lit'] is not None else None,t)
                    bc[key if c['lit'] is None else (round(c['lit'],6),t)].append(cside(c['T']))
                for e in ex:
                    t,sd=ex_key(e)
                    if kclaim(e):
                        k=[k for k in litset if abs(e['const']-k)<=1e-6*max(1,abs(k))][0]; be[(round(k,6),t)].append(sd)
                    else: be[(None,t)].append(sd)
                for ci,c in enumerate(cs):
                    anch='ANCLA' if c['lit'] is not None else ('ORDEN_CONST' if c.get('fb') else 'ORDEN')
                    v,ej,conf=R[ci]
                    det='%x:%s'%(ex[ej]['addr'],ex[ej]['desc']) if ej is not None else '%d cmp EXE'%len(ex)
                    if v=='DIFERENTE_CAND' and re.search(r'isnan|isfinite|([A-Za-z_]\w*)\s*!=\s*\1\b',L[c['line']-1]): v='FALSO_POSITIVO_NAN_EXPLICITO'
                    stats[v]+=1
                    nan_cpp='NaN->true' if 'NaN' in c['T'] else 'NaN->false'
                    nk=sum(1 for q in cs if c['lit'] is not None and q['lit'] is not None and abs(q['lit']-c['lit'])<1e-6) if c['lit'] is not None else ''
                    ne=sum(1 for e in ex if c['lit'] is not None and e['const'] is not None and abs(e['const']-c['lit'])<1e-6) if c['lit'] is not None else ''
                    rows.append((rel,c['line'],name,hex(addr),c['code'],v,anch+' '+nan_cpp+' w%d conf=%s'%(writes,conf),det,c['op'],int(c['neg']),c['nocc'],int(c['same']),nk,ne,bstat(bc,be,c),('%x:%s %d/%d'%(OA[ci][0],'igual' if OA[ci][1] else 'DIFIERE',OA[ci][2],OA[ci][3])) if ci in OA else '',('%g'%c['lit']) if c['lit'] is not None else ''))
    with open(os.path.join(HERE,'auditoria2_auto.csv'),'w',newline='',encoding='utf-8') as fh:
        w=csv.writer(fh); w.writerow(['archivo','linea','funcion','dir_exe','codigo','veredicto_auto','tipo','detalle_exe','op','neg','occ','mismo_largo','n_cpp_const','n_exe_const','cubo','orden','const'])
        w.writerows(rows)
    print(stats,len(rows))
if __name__=='__main__': main()
