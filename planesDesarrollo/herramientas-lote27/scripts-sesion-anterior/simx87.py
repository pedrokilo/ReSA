import re,sys
lines=[l.split(': ',1)[1].strip() for l in open('rot.txt') if ': ' in l]
prog=[]
for l in lines:
    prog.append(l)
    if l.startswith('ret'): break
st=[]  # st[-1] = ST0 ; entries are expr strings with tag
mem={}  # esp offset -> expr
regs={}
cnt=[0]
def newv(prefix='t'):
    cnt[0]+=1; return f'{prefix}{cnt[0]}'
def rd(opnd):
    m=re.match(r"dword ptr \[(\w+)(?: \+ ((?:0x)?[0-9a-f]+))?\]",opnd)
    if not m: print("BAD",opnd); raise SystemExit
    base=m.group(1); off=int(m.group(2),0) if m.group(2) else 0
    return base,off
names={0x58:'x',0x5c:'y',0x60:'z'}
def load(opnd):
    b,o=rd(opnd)
    if b=='esp':
        return mem.get(o, names.get(o,f'?esp{o:x}'))
    if b=='ecx':
        return f'M[{o//4}]'  # matrix word index
def getst(i): return st[-1-i]
def setst(i,v): st[-1-i]=v
out=[]
for ln,ins in enumerate(prog):
    p=ins.split(' ',1); op=p[0]; a=p[1] if len(p)>1 else ''
    args=[s.strip() for s in a.split(',')] if a and 'ptr' not in a else None
    if op=='sub' or op=='add' or op=='ret': continue
    if op=='fld':
        if 'ptr' in a: st.append(load(a))
        else: st.append(getst(int(re.search(r'\((\d)\)',a).group(1))))
    elif op=='fcos': st[-1]=f'cos({st[-1]})'
    elif op=='fsin': st[-1]=f'sin({st[-1]})'
    elif op=='fchs': st[-1]=f'(-{st[-1]})'
    elif op in('fstp','fst'):
        b,o=rd(a) if 'ptr' in a else (None,None)
        if a.startswith('dword'):
            v=st[-1]
            if b=='esp':
                nm=newv('f'); mem[o]=nm; out.append(f'  // [esp+{o:x}] {nm} = float({v})')
            else: out.append(f'  OUT[{o//4}] = float({v})')
            if op=='fstp': st.pop()
        else:
            # fstp st(0) / st(n)
            i=int(re.search(r'\((\d)\)',a).group(1))
            if i==0: st.pop()
            else: setst(i,st[-1]); st.pop()
    elif op in('fmul','fadd','fsub','fsubr','fdiv'):
        sym={'fmul':'*','fadd':'+','fsub':'-','fsubr':'-'}[op]
        if 'ptr' in a:
            rhs=load(a); lhs=st[-1]
        else:
            m=re.match(r'st(?:\((\d)\))?(?:, ?st\((\d)\))?',a)
            t=a.replace(' ','')
            mm=re.match(r'st(,st\((\d)\))?$',t)
            mm2=re.match(r'st\((\d)\)$',t)
            # forms: "st(1)" means st0 = st0 op st(1)
            if mm2: rhs=getst(int(mm2.group(1))); lhs=st[-1]
            else: raise Exception(ins)
        e=f'({rhs} {sym} {lhs})' if op=='fsubr' else f'({lhs} {sym} {rhs})'
        st[-1]=e
    elif op in('fmulp','faddp','fsubp'):
        t=a.replace(' ','')
        i=int(re.search(r'\((\d)\)',t).group(1))
        sym={'fmulp':'*','faddp':'+','fsubp':'-'}[op]
        # st(i) = st(i) op st0 ; pop
        v=f'({getst(i)} {sym} {st[-1]})'
        setst(i,v); st.pop()
    elif op=='fxch':
        i=int(re.search(r'\((\d)\)',a).group(1))
        st[-1],st[-1-i]=st[-1-i],st[-1]
    elif op=='mov':
        d,s=[x.strip() for x in a.split(', ')]
        if d in('eax','edx'):
            b,o=rd(s); regs[d]=mem.get(o,'?')
        else:
            b,o=rd(d); out.append(f'  OUT[{o//4}] = {regs[s]}')
    else:
        print('UNHANDLED',ins)
    print(f'{ln:3d} {ins:40s} | depth={len(st)} ST0={st[-1] if st else None}'[:230])
print('\n'.join(out))
print('stack left',st)
