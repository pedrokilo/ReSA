"""Primary PE byte evidence only; no executable load or invocation."""
import hashlib,json,struct
from pathlib import Path
import capstone
HERE=Path(__file__).resolve().parent
EXE=Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
data=EXE.read_bytes(); pe=struct.unpack_from('<I',data,0x3c)[0];opt=pe+24
image=struct.unpack_from('<I',data,opt+28)[0]; sections=opt+struct.unpack_from('<H',data,pe+20)[0]
ss=[struct.unpack_from('<4I',data,sections+i*40+8) for i in range(struct.unpack_from('<H',data,pe+6)[0])]
def read(va,n):
    for vs,base,size,raw in ss:
        if base<=va-image<base+size:return data[raw+va-image-base:raw+va-image-base+n]
    raise ValueError(hex(va))
scope=json.loads((HERE/'scope.json').read_text()); md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for row in scope['entries']:
    address=int(row['address'],16)
    if row['rea']['ok']:
        dossier=json.loads((HERE.parents[1]/row['full_body_evidence']).read_text())
        ranges=dossier['procedure']['body']['ranges']; observed=True
    else:
        # Failed Ghidra function creation: bounded catalogue window, explicitly not a function-body claim.
        ranges=[{'start':hex(address),'end':hex(address+0x2f)}];observed=False
    bodies=[]
    for span in ranges:
        start=int(span['start'],16);end=int(span['end'],16);blob=read(start,end-start+1)
        bodies.append({'start':hex(start),'end':hex(end),'bytes':blob.hex(),'sha256':hashlib.sha256(blob).hexdigest(),'assembly':[f'{i.address:08X}: {i.bytes.hex()} {i.mnemonic} {i.op_str}' for i in md.disasm(blob,start)]})
    (HERE/'evidence'/(row['fn_name']+'.exe-bytes.json')).write_text(json.dumps({'exe':str(EXE),'exe_sha256':hashlib.sha256(data).hexdigest(),'catalogue_address':row['address'],'body_ranges_observed_by_rea':observed,'bounded_window_only':not observed,'ranges':bodies},indent=2),encoding='utf-8')
print('Extracted primary PE bytes for',len(scope['entries']),'identities')
for address,name in ((0x53edc0,'IsLeftShoulder1Pressed'),(0x53ef40,'IsTrianglePressed')):
    blob=read(address,64)
    (HERE/'evidence'/(name+'.collaborator-window.json')).write_text(json.dumps({'scope':'Read-only native input helper, not validation target; bounded64-byte listing requires explicit branch review.','address':hex(address),'exe_sha256':hashlib.sha256(data).hexdigest(),'bytes':blob.hex(),'assembly':[f'{i.address:08X}: {i.bytes.hex()} {i.mnemonic} {i.op_str}' for i in md.disasm(blob,address)]},indent=2),encoding='utf-8')
