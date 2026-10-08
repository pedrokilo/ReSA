"""Read current ASI/PDB metadata and body; never load the probe module."""
import ctypes as c, comtypes, comtypes.client as cc, hashlib, json, struct
from comtypes.server import IClassFactory
from pathlib import Path
import capstone
DLL=r'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\msdia140.dll'

def check(asi_path,pdb_path,manifest,output):
    asi=Path(asi_path).read_bytes()
    assert hashlib.sha256(asi).hexdigest()==manifest['asi']
    with Path(pdb_path).open('rb') as f:assert hashlib.file_digest(f,'sha256').hexdigest()==manifest['pdb']
    m=cc.GetModule(DLL);dll=c.WinDLL(DLL);get=dll.DllGetClassObject
    get.argtypes=[c.POINTER(comtypes.GUID),c.POINTER(comtypes.GUID),c.POINTER(c.c_void_p)];get.restype=c.HRESULT
    ptr=c.c_void_p();guid=comtypes.GUID('{e6756135-1e65-4d17-8576-610761398c3c}')
    assert get(c.byref(guid),c.byref(IClassFactory._iid_),c.byref(ptr))==0
    source=c.cast(ptr,c.POINTER(IClassFactory)).CreateInstance(interface=m.IDiaDataSource)
    source.loadDataFromPdb(str(Path(pdb_path).resolve()));session=source.openSession()
    pe=struct.unpack_from('<I',asi,60)[0];opt=pe+24
    assert struct.unpack_from('<H',asi,pe+4)[0]==0x14c and struct.unpack_from('<H',asi,opt)[0]==0x10b
    count=struct.unpack_from('<H',asi,pe+6)[0];opsize=struct.unpack_from('<H',asi,pe+20)[0];image=struct.unpack_from('<I',asi,opt+28)[0]
    sections=[struct.unpack_from('<IIII',asi,opt+opsize+i*40+8) for i in range(count)]
    def read(rva,size):
        for vs,va,rs,off in sections:
            if va<=rva and rva+size<=va+rs:return asi[off+rva-va:off+rva-va+size]
        raise ValueError(('Unmapped current body',rva,size))
    functions=[];decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
    for unknown in session.globalScope.findChildren(5,None,0):
        symbol=unknown.QueryInterface(m.IDiaSymbol)
        if str(symbol.name)!='CPad::StartShake_Train':continue
        rva=int(symbol.relativeVirtualAddress);length=int(symbol.length);body=read(rva,length)
        instructions=[dict(rva=i.address-image,mnemonic=i.mnemonic,operands=i.op_str) for i in decoder.disasm(body,image+rva)]
        returns=[i['operands'] for i in instructions if i['mnemonic']=='ret']
        assert returns and all(int(x,0)==8 for x in returns),('Train compiled stack ABI must be RET8',returns)
        functions.append(dict(name=symbol.name,rva=rva,length=length,body_sha256=hashlib.sha256(body).hexdigest(),bytes_hex=body.hex(),instructions=instructions,return_operands=returns))
    assert len(functions)==1,('Train current artifact symbol ambiguous/missing',len(functions))
    result=dict(asi_sha256=manifest['asi'],pdb_sha256=manifest['pdb'],reader='VS DIA metadata only; ASI never loaded',native_ret='0x53FB3D RET8',functions=functions)
    output=Path(output);output.write_text(json.dumps(result,indent=2)+'\n')
    return hashlib.sha256(output.read_bytes()).hexdigest()
