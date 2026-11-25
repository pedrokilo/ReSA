import sys,re,json,collections; sys.path.insert(0,'.')
from addrs import *
from names2 import label
from names import name as oldname
import sites as S
allsites=[]
for lo,hi in ((0x1000a740,0x1000d29a),(0x10008294,0x10008310),(0x1000d2a0,0x1000d900)):
    allsites+=S.sites(lo,hi)
patched=set()
for d in allsites:
    for a in range(d['start'],d['end']+1): patched.add(a)
inst_ranges=[(0x1000a740,0x1000d29a),(0x10008294,0x10008310),(0x1000d2a0,0x1000d500),(0x1000a600,0x1000a660),(0x1000a660,0x1000a740)]
def in_inst(a): return any(lo<=a<=hi for lo,hi in inst_ranges)
def lab(a):
    l=label(a) or oldname(a,0x2000) or ''
    return l
out=[]
for A in sorted(addrs|{0x843d10,0x845410}):
    kinds=collections.Counter(); where=collections.defaultdict(list)
    for i in uses.get(A,[]):
        if in_inst(i.address): kinds['instalador']+=1; continue
        mn=i.mnemonic; ops=i.operands
        k=None
        if mn in('call','jmp') and ops[0].type==X86_OP_IMM: k='llamada_directa'
        elif mn=='push': k='push_inmediato'
        elif mn=='mov' and ops[0].type==X86_OP_MEM and (ops[0].mem.disp&0xffffffff)==A: k='escritura'
        elif mn=='mov' and ops[0].type==X86_OP_MEM and ops[1].type==X86_OP_IMM and ops[1].imm&0xffffffff==A: k='guarda_puntero'
        elif mn=='mov' and ops[0].type==X86_OP_REG and ops[1].type==X86_OP_IMM: k='puntero_a_registro'
        elif mn in('cmp','test'): k='comparacion'
        elif mn in('mov','movzx','movsx','fld','fild','fmul','fadd','fsub','fdiv','add','sub','and','or','xor','imul') : k='lectura'
        else: k='otro:'+mn
        kinds[k]+=1
        fo=fof(i.address)
        where[k].append('%x@%x'%(i.address,fo))
    out.append((A,dict(kinds),{k:v[:4] for k,v in where.items()}))
json.dump([(hex(a),k,w) for a,k,w in out],open('../classify_raw.json','w'))
c=collections.Counter()
for a,k,w in out:
    key=tuple(sorted(k))
    c[key]+=1
for k,v in c.most_common(): print(v,k)
