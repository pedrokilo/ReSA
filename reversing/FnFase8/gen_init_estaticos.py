import csv,struct,sys,collections,re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
d=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',d,60)[0]
ns,=struct.unpack_from('<H',d,pe+6); opt,=struct.unpack_from('<H',d,pe+20)
secs=[]
for i in range(ns):
    p=pe+24+opt+40*i
    vs,rva,rs,rp=struct.unpack_from('<IIII',d,p+8)
    secs.append((0x400000+rva,min(vs,rs),rp))
def rd(a,n):
    for va,sz,rp in secs:
        if va<=a<va+sz: return d[rp+a-va:rp+a-va+n]
    return b''
md=Cs(CS_ARCH_X86,CS_MODE_32)
r=list(csv.DictReader(open(r'D:\Original\wv-fase8b\docs\CodigoVivoExe\fase8-inicializadores.csv',encoding='utf-8')))
pend=[x for x in r if x['estado']=='pendiente-DatosExe' or (x['clase'].startswith('llamada') )]
pat=collections.OrderedDict()
for x in pend:
    a=int(x['va'],16); n=int(x['bytes'])
    ins=[(i.address,i.mnemonic,i.op_str) for i in md.disasm(rd(a,n),a)]
    key=' ; '.join(m+(' '+o if m in('call',) and False else '') for _,m,o in ins)
    pat.setdefault(tuple(m for _,m,_ in ins),[]).append((x['va'],ins,x['clase']))
for k,v in pat.items():
    print(len(v),k, v[0][2]);
    for a,m,o in v[0][1]: print('    %x %s %s'%(a,m,o))
print("=======")
import re
out=[]; unh=[]
for x in r:
    if x['estado']!='pendiente-DatosExe': continue
    a=int(x['va'],16); n=int(x['bytes'])
    ins=[(i.address,i.mnemonic,i.op_str) for i in md.disasm(rd(a,n),a)]
    ms=tuple(m for _,m,_ in ins)
    h=lambda s:int(s,16) if s.startswith('0x') else int(s)
    if ms==('mov','call','push','call','pop','ret') and ins[0][2].startswith('ecx, '):
        out.append(('CTOR',h(ins[0][2][5:]),h(ins[1][2]),0,0,a))
    elif ms==('push','push','push','push','push','call','push','call','pop','ret'):
        out.append(('VEC',h(ins[4][2]),h(ins[3][2]),h(ins[2][2]),h(ins[1][2]),a))  # this,size,count,ctor
        if h(ins[5][2])!=0x821d30: unh.append((a,'vec?'))
        out[-1]=('VEC',h(ins[4][2]),h(ins[3][2]),h(ins[2][2]),h(ins[1][2]),a)
    elif ms==('push','call','pop','ret'): pass
    elif ms==('mov','jmp') and ins[0][2].startswith('ecx, '):
        out.append(('CTOR',h(ins[0][2][5:]),h(ins[1][2]),0,0,a))
    elif ms==('mov','mov','ret') and 'dword ptr [0x' in ins[0][2] and ins[1][2].startswith('dword ptr [0x'):
        s=re.search(r'\[(0x[0-9a-f]+)\]',ins[0][2]).group(1); dd=re.search(r'\[(0x[0-9a-f]+)\]',ins[1][2]).group(1)
        out.append(('COPY32',h(dd),h(s),0,0,a))
    else: unh.append((a,ms,[ (m,o) for _,m,o in ins]))
print(len(out),len(unh))
for u in unh: print(hex(u[0]),u[1:] )
import json
json.dump(out,open(r'C:\Users\pedro\AppData\Local\Temp\ini.json','w'))
print("=======GEN")
from unicorn import *
from unicorn.x86_const import *
def mk():
    uc=Uc(UC_ARCH_X86,UC_MODE_32)
    for i in range(ns):
        p=pe+24+opt+40*i
        vsz,rva,rs,rp=struct.unpack_from('<IIII',d,p+8)
        va=0x400000+rva; sz=(max(vsz,rs)+0xFFF)&~0xFFF
        uc.mem_map(va,sz)
        uc.mem_write(va,d[rp:rp+min(rs,vsz)])
    uc.mem_map(0x10000000,0x100000)  # pila
    uc.mem_map(0x20000000,0x1000)
    uc.mem_write(0x20000000,b'\xf4'*16)
    return uc
def emular(this,ctor,args):
    uc=mk()
    esp=0x10080000
    for a in reversed(args):
        esp-=4; uc.mem_write(esp,struct.pack('<I',a))
    esp-=4; uc.mem_write(esp,struct.pack('<I',0x20000000))
    uc.reg_write(UC_X86_REG_ESP,esp); uc.reg_write(UC_X86_REG_ECX,this)
    writes=[]
    def hw(uc,access,addr,size,value,ud):
        if not (0x10000000<=addr<0x10100000): writes.append((addr,size,value&((1<<(8*size))-1)))
    uc.hook_add(UC_HOOK_MEM_WRITE,hw)
    pend=[]
    def hv(uc,addr,size,ud):   # __ehvec_ctor (0x821D30): (this, size, count, ctor, dtor), cdecl: se emula elemento a elemento y se retorna
        e=uc.reg_read(UC_X86_REG_ESP)
        pend.append(struct.unpack('<IIIII',uc.mem_read(e,20)))
        uc.emu_stop()
    uc.hook_add(UC_HOOK_CODE,hv,begin=0x821D30,end=0x821D30)
    pc=ctor
    while True:
        try: uc.emu_start(pc,0x20000000,count=100000)
        except Exception as ex: raise Exception('%s eip=%x esp=%x pc=%x'%(ex,uc.reg_read(UC_X86_REG_EIP),uc.reg_read(UC_X86_REG_ESP),pc))
        if not pend: break
        ret,t,sz,cnt,ct=pend.pop()
        for i in range(cnt): writes.extend(emular(t+i*sz,ct,[]))
        e=uc.reg_read(UC_X86_REG_ESP)
        uc.reg_write(UC_X86_REG_EAX,t); uc.reg_write(UC_X86_REG_ESP,e+4+20)  # stdcall: la rutina saca sus 5 argumentos
        pc=ret
    return writes

rows=[]
def H(v): return '0x%X'%v
for x in r:
    if x['estado']!='pendiente-DatosExe': continue
    a=int(x['va'],16); n=int(x['bytes'])
    ins=[(i.address,i.mnemonic,i.op_str) for i in md.disasm(rd(a,n),a)]
    ms=tuple(m for _,m,_ in ins)
    h=lambda s:int(s,16) if s.startswith('0x') else int(s)
    ops=[o for _,_,o in ins]
    if ms==('mov','call','push','call','pop','ret') and ops[0].startswith('ecx, '):
        rows.append((a,'kCtor0',h(ops[0][5:]),h(ops[1]),0,0,0,0))
    elif ms==('push','push','push','push','push','call','push','call','pop','ret'):
        rows.append((a,'kVec',h(ops[4]),h(ops[3]),h(ops[2]),h(ops[1]),0,0))
    elif ms==('push','call','pop','ret'): pass
    elif ms==('mov','jmp') and ops[0].startswith('ecx, '):
        rows.append((a,'kCtor0',h(ops[0][5:]),h(ops[1]),0,0,0,0))
    elif ms==('mov','mov','ret') and ops[0].startswith('dword ptr [0x'):
        s=re.search(r'\[(0x[0-9a-f]+)\]',ops[0]).group(1); dd=re.search(r'\[(0x[0-9a-f]+)\]',ops[1]).group(1)
        rows.append((a,'kCopy32',h(dd),h(s),0,0,0,0))
    elif ms==('mov','mov','ret') and ops[0].startswith('ax, word ptr'):
        s=re.search(r'\[(0x[0-9a-f]+)\]',ops[0]).group(1); dd=re.search(r'\[(0x[0-9a-f]+)\]',ops[1]).group(1)
        rows.append((a,'kCopy16',h(dd),h(s),0,0,0,0))
    elif ms==('mov','mov','lea','mov','add','dec','jne','ret') and 'word ptr [eax], 0xffff' in ops[3]:
        rows.append((a,'kFill16',h(ops[0][5:]),h(ops[1][5:]),0xFFFF,0,0,0))
    elif a==0x84d190:
        rows.append((a,'kCopy32',0x8cc0b8,0x8cbf74,0,0,0,0)); rows.append((a,'kCopy32',0x8cc0c0,0x8cbf78,0,0,0,0))
    elif a==0x84d750: rows.append((a,'kIndex24',0xb6fe88,0x8ccb80,0x8cc8e0,0,0,0))
    elif a==0x84cfc0: rows.append((a,'kFdivr16',0xb6ba54,0x8cbc8c,0xb6ba4c,0,0,0))
    elif a==0x84e2e0: rows.append((a,'kCtor1',0xb74240,0x59aed0,0x3f800000,0,0,0))
    elif a in(0x84e7b0,): rows.append((a,'kCtor4',0xb7cb10,0x7170c0,0x46,0x46,0x46,0xff))
    elif a==0x850910: rows.append((a,'kCtor4',0xc03a44,0x7170c0,0xff,0xff,0xff,0xff))
    elif a==0x8543d0: rows.append((a,'kVec',0xc2ba08,0xe0,0xd2,0x6d0450,0,0))
    else: pass # ceros (bss ya a cero), atexit sin objeto, asignaciones dependientes de datos de ejecucion
rows.sort()
HK=set(int(x['address'],16) for x in csv.DictReader(open(r'D:\Original\wv-fase8b\docs\hooks.csv',encoding='utf-8')))
nuevo=[]
for row in rows:
    va,k,a,b,c,dd,e,f=row
    if k in('kCtor0','kCtor1','kCtor4','kVec'):
        ctor=b if k!='kVec' else dd
        if ctor not in HK:
            if k=='kVec': items=[(a+i*b,[]) for i in range(c)]
            elif k=='kCtor0': items=[(a,[])]
            elif k=='kCtor1': items=[(a,[c])]
            else: items=[(a,[c,dd,e,f][::1])]
            tmp=[]
            try:
                for this,args in items:
                    for (ad,sz,v) in emular(this,ctor,args):
                        if v==0 and 0x8E4000<=ad<0xC9E000: continue
                        tmp.append((va,'kWrite%d'%(sz*8),ad,v,0,0,0,0))
                nuevo.extend(tmp)
                continue
            except Exception as ex:
                print('EMULACION FALLA',hex(va),hex(ctor),ex)
    nuevo.append(row)
rows=nuevo
print(len(rows))
with open(r'D:\Original\wv-fase8b\source\app\platform\win\StandaloneInit.inc','w',encoding='utf-8',newline='\n') as f:
    f.write('// GENERADO (fase 8, segunda ronda; reversing/FnFase8/gen_init_estaticos.py): inicializadores estaticos del EXE original que DatosExe todavia no define (fase8-inicializadores.csv, estado pendiente-DatosExe).\n')
    f.write('// Cada fila = una rutina de la tabla _initterm del original (va), decodificada del archivo: constructores de objetos globales (se llama al C++ con gancho), arrays de relleno 0xFFFF, copias de valores iniciales.\n')
    f.write('// Se omiten las que solo registran el destructor (atexit), las que ponen a cero memoria de .bss y las que dependen de datos de ejecucion. {va, tipo, a, b, c, d, e, f}\n')
    for row in rows:
        f.write('{ 0x%08X, %s, %s, %s, %s, %s, %s, %s },\n'%((row[0],row[1])+tuple(H(v) for v in row[2:])))
