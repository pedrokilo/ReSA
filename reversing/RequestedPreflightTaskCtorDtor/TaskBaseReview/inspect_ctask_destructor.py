"""Read-only PE/Capstone inspection; never executes the ASI or starts GTA."""
import hashlib,json,struct,sys
from pathlib import Path
import capstone
from capstone.x86 import X86_OP_IMM,X86_OP_MEM
asi=Path(sys.argv[1]).resolve();out=Path(sys.argv[2]).resolve();out.mkdir(parents=True,exist_ok=True)
b=asi.read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0];opt=pe+24
assert struct.unpack_from('<H',b,opt)[0]==0x10b,'Expected PE32'
image=struct.unpack_from('<I',b,opt+28)[0];sections=opt+struct.unpack_from('<H',b,pe+20)[0]
ss=[struct.unpack_from('<4I',b,sections+i*40+8) for i in range(struct.unpack_from('<H',b,pe+6)[0])]
def offset(rva):
    for vs,va,rs,ro in ss:
        if va<=rva<va+max(vs,rs):return ro+rva-va
    raise ValueError(hex(rva))
def read(va,n):return b[offset(va-image):offset(va-image)+n]
def cstring(rva):
    at=offset(rva);return b[at:b.index(b'\x00',at)].decode('ascii')
exp=offset(struct.unpack_from('<I',b,opt+96)[0]);nf,nn,funcs,names,ords=struct.unpack_from('<5I',b,exp+20)
exports={}
for i in range(nn):
    name=cstring(struct.unpack_from('<I',b,offset(names)+i*4)[0]);idx=struct.unpack_from('<H',b,offset(ords)+i*2)[0]
    exports[name]=image+struct.unpack_from('<I',b,offset(funcs)+idx*4)[0]
vt=exports['??_7CTask@@6B@'];scalar=struct.unpack('<I',read(vt,4))[0]
def resolve(va):
    chain=[]
    for i in range(16):
        raw=read(va,5)
        if raw[0]!=0xe9:return va,chain
        dst=va+5+struct.unpack_from('<i',raw,1)[0];chain.append({'from':hex(va),'to':hex(dst)});va=dst
    raise ValueError('Unbounded jump chain')
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);md.detail=True
body,chain=resolve(scalar);ordinary=exports['??1CTask@@UAE@XZ'];ordinaryBody,ordinaryChain=resolve(ordinary);queue=[('ctask-deleting-vtable-thunk',body),('ctask-ordinary-destructor',ordinaryBody)];seen=set();reports=[]
while queue:
    label,address=queue.pop(0)
    if address in seen:continue
    seen.add(address);instructions=[];calls=[];stores=[]
    for ins in md.disasm(read(address,1024),address):
        instructions.append(f'{ins.address:08X}: {ins.bytes.hex():24} {ins.mnemonic} {ins.op_str}')
        if ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==vt:
            stores.append({'address':hex(ins.address),'memory_destination':ins.op_str.split(',')[0]})
        if ins.mnemonic=='call' and len(ins.operands)==1 and ins.operands[0].type==X86_OP_IMM:
            callee=ins.operands[0].imm&0xffffffff
            if image<=callee<image+struct.unpack_from('<I',b,opt+56)[0]:
                target,hops=resolve(callee);calls.append({'instruction':hex(ins.address),'entry':hex(callee),'body':hex(target),'thunks':hops})
                if label=='ctask-deleting-vtable-thunk':queue.append(('callee-'+hex(target),target))
        if ins.mnemonic.startswith('ret'):break
    (out/(label+'.asm.txt')).write_text('\n'.join(instructions)+'\n',encoding='utf-8')
    reports.append({'label':label,'body':hex(address),'stores_exact_ctask_vtable':stores,'calls':calls,'linear_disassembly_stops_at_first_ret':True})
summary={'asi_path':str(asi),'asi_sha256':hashlib.sha256(b).hexdigest(),'image_base':hex(image),'ctask_vtable_export':hex(vt),'slot0_entry':hex(scalar),'slot0_body':hex(body),'slot0_thunks':chain,'ordinary_destructor_export':hex(ordinary),'ordinary_destructor_body':hex(ordinaryBody),'ordinary_destructor_thunks':ordinaryChain,'procedures':reports,'limitation':'Static linear decoding. Store destination and call order must be assessed with this-flow; MSVC deleting thunk may support flags1/2/4; inspect exact flag branches, not assume EXE ABI; body includes free branch. No runtime or branch replay.'}
(out/'ctask-destructor-pe-review.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
print(json.dumps({'asi_sha256':summary['asi_sha256'],'ctask_vtable':hex(vt),'store_procedures':[(r['label'],r['stores_exact_ctask_vtable']) for r in reports if r['stores_exact_ctask_vtable']]},indent=2))
