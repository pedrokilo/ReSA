# Traductor simbolico de x87 en linea recta (sin saltos) a C++ con double: cada `fstp dword` emite una sentencia (los operandos de memoria se leen en ese momento).
# Uso: python x87gen.py <archivo dis> <direccion funcion hex> ; registros: ecx -> a, eax -> b, edx -> out segun el mapa pasado en MAP.
import sys,re
def gen(lines,regs):
    st=[]; out=[]
    def mem(op):
        m=re.match(r'(dword|qword) ptr \[(\w+)(?: \+ (0x[0-9a-f]+|\d+))?\]',op)
        if not m: raise Exception('mem '+op)
        r=m.group(2); off=int(m.group(3),0) if m.group(3) else 0
        name=regs.get(r,r)
        if off%4: raise Exception('align '+op)
        return f'double({name}[{off//4}])'
    def sti(op):
        return int(re.match(r'st\((\d)\)',op).group(1)) if op.startswith('st(') else 0
    for l in lines:
        parts=l.split(' ',2); mn=parts[1]; op=parts[2] if len(parts)>2 else ''
        if mn=='fld':
            st.insert(0, st[sti(op)] if op.startswith('st') else mem(op))
        elif mn in('fmul','fadd','fsub','fsubr','fdiv','fdivr'):
            if op.startswith('dword'): b=mem(op); a=st[0]; tgt=0
            else:
                i=sti(op); a=st[0]; b=st[i]; tgt=0
            sym={'fmul':'*','fadd':'+','fsub':'-','fdiv':'/'}
            if mn=='fsubr': r=f'({b} - {a})'
            elif mn=='fdivr': r=f'({b} / {a})'
            else: r=f'({a} {sym[mn]} {b})'
            st[0]=r
        elif mn in('faddp','fmulp','fsubp','fsubrp'):
            i=sti(op); a=st[i]; b=st[0]
            sym={'faddp':'+','fmulp':'*','fsubp':'-'}
            st[i]= f'({a} {sym[mn]} {b})' if mn!='fsubrp' else f'({b} - {a})'
            st.pop(0)
        elif mn=='fstp':
            if op.startswith('dword'):
                m=re.match(r'dword ptr \[(\w+)(?: \+ (0x[0-9a-f]+|\d+))?\]',op); off=int(m.group(2),0) if m.group(2) else 0
                out.append(f'{regs.get(m.group(1),m.group(1))}[{off//4}] = float({st[0]});'); st.pop(0)
            else: raise Exception(l)
        elif mn=='fchs': st[0]=f'(-{st[0]})'
        elif mn in('mov','ret'): pass
        else: raise Exception(l)
    return out,st
if __name__=='__main__':
    f,addr=sys.argv[1],sys.argv[2].lower()
    txt=open(f).read().split('=== ')
    for blk in txt:
        if blk.startswith('0x'+addr.zfill(8)):
            lines=[l for l in blk.splitlines()[1:] if l.strip()]
            regs={'ecx':'a','eax':'b','edx':'out'}
            o,st=gen(lines,regs)
            print('\n'.join(o)); print('left',st)
