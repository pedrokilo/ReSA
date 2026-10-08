"""Read VS DIA metadata only. Never loads or executes the GTA probe module."""
import ctypes as c,comtypes,comtypes.client as cc,json,hashlib,argparse,struct,capstone
from comtypes.server import IClassFactory
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
DLL=r'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\msdia140.dll'
def main():
 m=cc.GetModule(DLL);dll=c.WinDLL(DLL);get=dll.DllGetClassObject;get.argtypes=[c.POINTER(comtypes.GUID),c.POINTER(comtypes.GUID),c.POINTER(c.c_void_p)];get.restype=c.HRESULT
 ptr=c.c_void_p();guid=comtypes.GUID('{e6756135-1e65-4d17-8576-610761398c3c}');assert get(c.byref(guid),c.byref(IClassFactory._iid_),c.byref(ptr))==0
 factory=c.cast(ptr,c.POINTER(IClassFactory));source=factory.CreateInstance(interface=m.IDiaDataSource)
 parser=argparse.ArgumentParser();parser.add_argument('--manifest',type=Path,required=True);parser.add_argument('--asi',type=Path,required=True);parser.add_argument('--pdb',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
 manifest=json.loads(args.manifest.read_text());asi=args.asi.read_bytes();assert hashlib.sha256(asi).hexdigest()==manifest['asi']
 with args.pdb.open('rb') as handle:assert hashlib.file_digest(handle,'sha256').hexdigest()==manifest['pdb']
 source.loadDataFromPdb(str(args.pdb.resolve()));session=source.openSession();globalScope=session.globalScope
 pe=struct.unpack_from('<I',asi,60)[0];count=struct.unpack_from('<H',asi,pe+6)[0];opt=pe+24;opsize=struct.unpack_from('<H',asi,pe+20)[0];image=struct.unpack_from('<I',asi,opt+28)[0];imageSize=struct.unpack_from('<I',asi,opt+56)[0]
 sections=[struct.unpack_from('<IIII',asi,opt+opsize+i*40+8) for i in range(count)]
 def read(rva,size):
  for vs,va,rs,off in sections:
   if va<=rva<va+max(vs,rs):return asi[off+rva-va:off+rva-va+size]
  raise ValueError(rva)
 relocRva,relocSize=struct.unpack_from('<II',asi,opt+96+5*8);relocData=read(relocRva,relocSize);relocations=[];cursor=0
 while cursor<len(relocData):
  page,length=struct.unpack_from('<II',relocData,cursor)
  if length==0:break
  assert length>=8
  for offset in range(cursor+8,cursor+length,2):
   entry=struct.unpack_from('<H',relocData,offset)[0];kind=entry>>12
   if kind==3:relocations.append(page+(entry&0xFFF))
   else:assert kind==0,'Unsupported PE relocation'
  cursor+=length
 decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);decoder.detail=True
 result={'reader':'VisualStudioDIA140 metadata only','asi_sha256':manifest['asi'],'pdb_sha256':manifest['pdb'],'image_base':image,'image_size':imageSize,'functions':[],'scratch':[],'relocation_type':'PE32 HIGHLOW; explicit DWORD offsets, no blind RVA rebasing'}
 # Function-local statics are descendants of the function scope, not immediate globals.
 for unknown in globalScope.findChildren(5,None,0):
  func=unknown.QueryInterface(m.IDiaSymbol)
  if 'ReconcileTwoControllersInput' not in str(func.name):continue
  item={'name':func.name,'rva':func.relativeVirtualAddress,'length':func.length};result['functions'].append(item)
  def children(scope):
   for unknown in scope.findChildren(0,None,0):
    sym=unknown.QueryInterface(m.IDiaSymbol)
    if 'reconciledState' in str(sym.name):result['scratch'].append({'name':sym.name,'rva':sym.relativeVirtualAddress,'length':sym.type.length,'tag':sym.symTag,'locationType':sym.locationType,'parent':dict(item)})
    if sym.symTag==6:children(sym) # lexical blocks
  children(func)
  body=read(item['rva'],item['length']);item['bytes_hex']=body.hex();item['body_sha256']=hashlib.sha256(body).hexdigest();item['highlow_relocations']=[r-item['rva'] for r in relocations if item['rva']<=r<item['rva']+item['length']]
  item['calls']=[]
  for ins in decoder.disasm(body,image+item['rva']):
   if ins.mnemonic!='call' or not ins.op_str.startswith('0x'):continue
   destination=int(ins.op_str,16)-image;initial=destination;thunk=read(destination,5)
   if thunk[0]==0xE9:destination=destination+5+int.from_bytes(thunk[1:5],'little',signed=True)
   try:symbol=session.findSymbolByRVA(destination,0);name=symbol.name
   except Exception:name=None
   item['calls'].append({'call_rva':ins.address-image,'destination_rva':destination,'initial_rva':initial,'symbol':name})
   if name in ('std::max<short>','std::min<short>','_RTC_CheckEsp','CControllerState::Clear'):
    helper=session.findSymbolByRVA(destination,5);helperRva=helper.relativeVirtualAddress;helperSize=helper.length;helperBody=read(helperRva,helperSize)
    item.setdefault('helper_bodies',{})[name]={'rva':helperRva,'length':helperSize,'bytes_hex':helperBody.hex(),'sha256':hashlib.sha256(helperBody).hexdigest(),'highlow_relocations':[r-helperRva for r in relocations if helperRva<=r<helperRva+helperSize],'instructions':[{'rva':i.address-image,'mnemonic':i.mnemonic,'operands':i.op_str} for i in decoder.disasm(helperBody,image+helperRva)]}
 for function in result['functions']:
  function['scratch_operands']=[];scratch=result['scratch'][0]
  for ins in decoder.disasm(bytes.fromhex(function['bytes_hex']),image+function['rva']):
   for operand in ins.operands:
    absolute=None;field_offset=None;field_size=None
    if operand.type==capstone.x86_const.X86_OP_MEM and operand.mem.base==0 and operand.mem.index==0:absolute=operand.mem.disp&0xFFFFFFFF;field_offset=ins.disp_offset;field_size=ins.disp_size
    elif operand.type==capstone.x86_const.X86_OP_IMM:absolute=operand.imm&0xFFFFFFFF;field_offset=ins.imm_offset;field_size=ins.imm_size
    if absolute is not None and image+scratch['rva']<=absolute<image+scratch['rva']+48:
     body_offset=ins.address-image-function['rva']+field_offset
     assert field_size==4 and body_offset in function['highlow_relocations'],'Scratch operand lacks exact HIGHLOW mapping'
     function['scratch_operands'].append({'instruction_rva':ins.address-image,'body_encoded_operand_offset':body_offset,'scratch_offset':absolute-image-scratch['rva'],'operand_size':operand.size,'opcode':ins.mnemonic})
  assert function['scratch_operands'] and any(x['scratch_offset']==0 for x in function['scratch_operands'])
 assert len(result['functions'])==1 and len(result['scratch'])==1 and result['scratch'][0]['length']==48
 scratch=result['scratch'][0];assert 16<=scratch['rva'] and scratch['rva']+48+16<=imageSize
 result['validation']='Static metadata/current artifact proof only. Runtime must confirm actual module identity, PE bounds, matching relocated body bytes and actual scratch operands before seeding; never use this file with another ASI.'
 args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'asi_sha256':result['asi_sha256'],'scratch_rva':result['scratch'][0]['rva'],'scratch_size':result['scratch'][0]['length'],'encoded_scratch_operands':len(result['functions'][0]['scratch_operands']),'calls':result['functions'][0]['calls']}))
if __name__=='__main__':main()
