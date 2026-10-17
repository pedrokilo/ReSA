"""Simulador simbolico de la pila x87 sobre un camino recto del desensamblado.
uso: python x87sym.py <dis> "ini-fin,ini-fin,..." [macro=fichero ...]
Cada rango (direcciones hex inclusive) se toma del .dis; las llamadas 'call 0x59c890' etc. se sustituyen por la lista de instrucciones de macros.
Imprime las instrucciones x87 con el estado y al final los registros 'muertos' por debajo de la cima (slot k = T-k)."""
import sys,re
dis={}
order=[]
for l in open(sys.argv[1]):
    m=re.match(r'([0-9a-f]+): (.*)',l.strip())
    if m:
        a=int(m.group(1),16); dis[a]=m.group(2); order.append(a)
def ins(a): return dis[a]
ranges=[]
for r in sys.argv[2].split(','):
    lo,hi=(r.split('-')+[r])[:2]
    ranges.append((int(lo,16),int(hi,16)))
macros={}
for arg in sys.argv[3:]:
    k,v=arg.split('=',1)
    macros[int(k,16)]=open(v).read().splitlines()
stack=[]            # lista de expr (indice 0 = slot 1)
slots={}            # slot -> expr (ultimo escrito)
log=[]
def push(e):
    stack.append(e); slots[len(stack)]=e
def st(i): return stack[-1-i]
def setst(i,e):
    stack[-1-i]=e; slots[len(stack)-i]=e
def pop(): return stack.pop()
def mem(op):
    m=re.search(r'\[(.*?)\]',op); return m.group(1) if m else op
def run(line):
    mn,_,op=line.partition(' ')
    op=op.strip()
    if mn=='fld':
        if op.startswith('st('): push(st(int(op[3:-1])))
        else: push(mem(op))
    elif mn in('fldz',): push('0')
    elif mn=='fld1': push('1')
    elif mn=='fild': push('int(%s)'%mem(op))
    elif mn in('fst','fstp'):
        if op.startswith('st('):
            i=int(op[3:-1]); 
            if mn=='fstp':
                v=st(0); pop(); 
                if i>0: setst(i-1,v)
            else: setst(i,st(0))
        else:
            if mn=='fstp': pop()
    elif mn in('fadd','fsub','fsubr','fmul','fdiv','fdivr'):
        sym={'fadd':'+','fsub':'-','fmul':'*','fdiv':'/','fsubr':'-','fdivr':'/'}[mn]
        rev=mn in('fsubr','fdivr')
        ops=[o.strip() for o in op.split(',')]
        if len(ops)==2 and ops[0].startswith('st(') and ops[1].startswith('st('):
            d=int(ops[0][3:-1]); s=int(ops[1][3:-1]); a=st(d); b=st(s)
            setst(d,'(%s%s%s)'%((b,sym,a) if rev else (a,sym,b)))
        elif op.startswith('st('):
            s=int(op[3:-1]); a=st(0); b=st(s); setst(0,'(%s%s%s)'%((b,sym,a) if rev else (a,sym,b)))
        else:
            a=st(0); b=mem(op); setst(0,'(%s%s%s)'%((b,sym,a) if rev else (a,sym,b)))
    elif mn in('faddp','fsubp','fsubrp','fmulp','fdivp','fdivrp'):
        sym={'faddp':'+','fsubp':'-','fsubrp':'-','fmulp':'*','fdivp':'/','fdivrp':'/'}[mn]
        rev=mn in('fsubrp','fdivrp')
        i=1
        if op.startswith('st('): i=int(op[3:-1])
        a=st(i); b=st(0); setst(i,'(%s%s%s)'%((b,sym,a) if rev else (a,sym,b))); pop()
    elif mn in('fcomp','fcom','ficomp','ficom'):
        if op.startswith('st('): o='st%s'%op[3:-1]; 
        else: o=mem(op)
        log.append('   %s cmp %s vs %s'%(mn,st(0),o))
        if mn.endswith('p'): pop()
    elif mn in('fcompp',):
        pop(); pop()
    elif mn=='fxch':
        i=1 if not op else int(op[3:-1]); a=st(0); b=st(i); setst(0,b); setst(i,a)
    elif mn in('fchs','fabs','fsqrt','fsin','fcos','frndint'):
        setst(0,'%s(%s)'%(mn[1:],st(0)))
    elif mn=='fpatan':
        y=st(1); x=st(0); pop(); setst(0,'atan2(%s,%s)'%(y,x))
    elif mn in('fnstsw','fnstcw','fldcw','wait'): pass
    else: raise Exception('x87 no soportado: '+line)
def feed(line):
    mn=line.split(' ')[0]
    if mn.startswith('f') and mn not in ('fs:',):
        run(line); 
        if mn in('fcomp','fcom'): pass
        log.append('%-28s depth=%d top=%s'%(line,len(stack),st(0) if stack else '-'))
for lo,hi in ranges:
    for a in order:
        if lo<=a<=hi:
            l=ins(a)
            if l.startswith('call 0x'):
                t=int(l.split('0x')[1],16)
                if t in macros:
                    for m in macros[t]: feed(m.split(': ',1)[-1].strip())
                continue
            feed(l)
print('\n'.join(log))
print('depth final',len(stack))
dead=sorted(k for k in slots if k>len(stack))
for k in dead: print('slot T-%d = %s'%(k,slots[k]))
for k in range(1,len(stack)+1): print('live slot %d = %s'%(k,slots[k]))
