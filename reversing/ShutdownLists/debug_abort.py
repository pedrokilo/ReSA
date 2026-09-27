"""Diagnostic one-shot CRT breakpoints in the runner-owned process only."""
import ctypes as c
from ctypes import wintypes as w
import json,struct,subprocess,sys,time
from pathlib import Path
HERE=Path(__file__).resolve().parent
pid=int(sys.argv[1]);folder=Path(sys.argv[2]);folder.mkdir(exist_ok=True)
active=json.loads((HERE/'artifacts/active-run.json').read_text());assert active['pid']==pid
k=c.WinDLL('kernel32',use_last_error=True)
d=c.WinDLL(r'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow\VsTest\x64\dbghelp.dll',use_last_error=True)
def bind(lib,name,args,ret):
 f=getattr(lib,name);f.argtypes=args;f.restype=ret;return f
Open=bind(k,'OpenProcess',[w.DWORD,w.BOOL,w.DWORD],w.HANDLE)
Close=bind(k,'CloseHandle',[w.HANDLE],w.BOOL)
Read=bind(k,'ReadProcessMemory',[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.c_void_p],w.BOOL)
Write=bind(k,'WriteProcessMemory',[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.c_void_p],w.BOOL)
Protect=bind(k,'VirtualProtectEx',[w.HANDLE,c.c_void_p,c.c_size_t,w.DWORD,c.POINTER(w.DWORD)],w.BOOL)
Flush=bind(k,'FlushInstructionCache',[w.HANDLE,c.c_void_p,c.c_size_t],w.BOOL)
Attach=bind(k,'DebugActiveProcess',[w.DWORD],w.BOOL)
Detach=bind(k,'DebugActiveProcessStop',[w.DWORD],w.BOOL)
Kill=bind(k,'DebugSetProcessKillOnExit',[w.BOOL],w.BOOL)
Wait=bind(k,'WaitForDebugEvent',[c.c_void_p,w.DWORD],w.BOOL)
Continue=bind(k,'ContinueDebugEvent',[w.DWORD,w.DWORD,w.DWORD],w.BOOL)
OpenThread=bind(k,'OpenThread',[w.DWORD,w.BOOL,w.DWORD],w.HANDLE)
GetContext=bind(k,'Wow64GetThreadContext',[w.HANDLE,c.c_void_p],w.BOOL)
SetContext=bind(k,'Wow64SetThreadContext',[w.HANDLE,c.c_void_p],w.BOOL)
Init=bind(d,'SymInitialize',[w.HANDLE,c.c_char_p,w.BOOL],w.BOOL)
Cleanup=bind(d,'SymCleanup',[w.HANDLE],w.BOOL)
Refresh=bind(d,'SymRefreshModuleList',[w.HANDLE],w.BOOL)
Symbol=bind(d,'SymFromName',[w.HANDLE,c.c_char_p,c.c_void_p],w.BOOL)
bind(d,'SymSetOptions',[w.DWORD],w.DWORD)(0x10|0x2|0x4|0x200|0x2000)
process=Open(0x1f0fff,False,pid);assert process
path=c.create_unicode_buffer(32768);size=w.DWORD(len(path))
Query=bind(k,'QueryFullProcessImageNameW',[w.HANDLE,w.DWORD,w.LPWSTR,c.POINTER(w.DWORD)],w.BOOL)
assert Query(process,0,path,c.byref(size)) and Path(path.value).resolve()==Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe').resolve()
points={};attached=False;initialized=False;events=[]
def export32(module_name,function):
 class Module(c.Structure):
  _fields_=[('size',w.DWORD),('id',w.DWORD),('pid',w.DWORD),('global_uses',w.DWORD),('uses',w.DWORD),('base',c.c_void_p),('length',w.DWORD),('handle',w.HANDLE),('name',w.WCHAR*256),('path',w.WCHAR*260)]
 snapshot=bind(k,'CreateToolhelp32Snapshot',[w.DWORD,w.DWORD],w.HANDLE)(0x18,pid)
 first=bind(k,'Module32FirstW',[w.HANDLE,c.POINTER(Module)],w.BOOL);next_=bind(k,'Module32NextW',[w.HANDLE,c.POINTER(Module)],w.BOOL)
 item=Module();item.size=c.sizeof(item);ok=first(snapshot,c.byref(item));base=None
 try:
  while ok:
   if item.name.lower()==module_name and item.base<0x100000000:base=item.base;break
   ok=next_(snapshot,c.byref(item))
 finally:Close(snapshot)
 assert base
 def read(at,size):
  b=c.create_string_buffer(size);assert Read(process,at,b,size,None);return b.raw
 pe=struct.unpack_from('<I',read(base,64),60)[0]
 rva=struct.unpack_from('<I',read(base+pe,128),120)[0];export=struct.unpack('<IIHHIIIIIII',read(base+rva,40))
 functions=struct.unpack('<'+'I'*export[6],read(base+export[8],4*export[6]))
 names=struct.unpack('<'+'I'*export[7],read(base+export[9],4*export[7]))
 ordinals=struct.unpack('<'+'H'*export[7],read(base+export[10],2*export[7]))
 for name,ordinal in zip(names,ordinals):
  if read(base+name,len(function)+1).split(b'\0')[0]==function.encode():return base+functions[ordinal]
 raise RuntimeError(function)
def patch(address,data):
 old=w.DWORD();ignored=w.DWORD();assert Protect(process,address,len(data),0x40,c.byref(old))
 try:assert Write(process,address,data,len(data),None);Flush(process,address,len(data))
 finally:Protect(process,address,len(data),old.value,c.byref(ignored))
try:
 assert Init(process,rb'E:\JUEGOS\GTA San Andreas - CVehicle Test\scripts',True);initialized=True
 deadline=time.monotonic()+240
 while time.monotonic()<deadline:
  Refresh(process)
  names=['gta_reversed!'+n for n in ('_wassert','abort','_invoke_watson','_invalid_parameter_noinfo_noreturn','_exit','exit')]+['kernel32!ExitProcess']
  for name in names:
   info=c.create_string_buffer(4096);struct.pack_into('<I',info,0,88);struct.pack_into('<I',info,80,3900)
   if Symbol(process,name.encode(),info):
    address=struct.unpack_from('<Q',info,56)[0];points[address]={'symbol':name}
  if points:break
  time.sleep(.25)
 assert points,'CRT symbols not found'
 # Verified original EXE CRT exit entry (push 0; push 0; ...).
 points[0x823CAE]={'symbol':'exe-exit'}
 points[export32('ntdll.dll','NtTerminateProcess')]={'symbol':'ntdll32-NtTerminateProcess'}
 assert Attach(pid);attached=True;Kill(False)
 event=c.create_string_buffer(176);armed=False;hits=0;stepping={}
 while time.monotonic()<deadline:
  if not Wait(event,250):continue
  code,owner,tid=struct.unpack_from('<III',event,0);status=0x10002
  if code==1:
   exception=struct.unpack_from('<I',event,16)[0];address=struct.unpack_from('<Q',event,32)[0]
   breakpoint=exception in (0x80000003,0x4000001f)
   status=0x10002 if breakpoint else 0x80010001
   if exception in (0x80000004,0x4000001e) and tid in stepping:
    at=stepping.pop(tid);patch(at,b'\xcc')
    thread=OpenThread(0x1a,False,tid);ctx=c.create_string_buffer(716);struct.pack_into('<I',ctx,0,0x10007)
    try:
     assert GetContext(thread,ctx);struct.pack_into('<I',ctx,192,struct.unpack_from('<I',ctx,192)[0]&~0x100);assert SetContext(thread,ctx)
    finally:Close(thread)
    status=0x10002
   elif breakpoint and not armed:
    for at,item in points.items():
     original=c.create_string_buffer(1);assert Read(process,at,original,1,None);item['original']=original.raw
     patch(at,b'\xcc')
    armed=True;print(json.dumps({hex(at):item['symbol'] for at,item in points.items()}),flush=True)
   elif breakpoint and address in points:
    item=points[address];patch(address,item['original'])
    thread=OpenThread(0x1a,False,tid);ctx=c.create_string_buffer(716);struct.pack_into('<I',ctx,0,0x10007)
    try:
     assert GetContext(thread,ctx);struct.pack_into('<I',ctx,184,address)
     if item['symbol']=='ntdll32-NtTerminateProcess':
      struct.pack_into('<I',ctx,192,struct.unpack_from('<I',ctx,192)[0]|0x100);stepping[tid]=address
     assert SetContext(thread,ctx)
     sp=struct.unpack_from('<I',ctx,196)[0];raw=c.create_string_buffer(512)
     if Read(process,sp,raw,512,None):
      (folder/f'stack-{hits}.json').write_text(json.dumps({'eip':hex(address),'esp':hex(sp),'words':[hex(n) for n in struct.unpack('<128I',raw.raw)]},indent=2))
    finally:Close(thread)
    hit=folder/f'hit-{hits}-{item["symbol"].replace("!","-")}';hits+=1
    with (folder/f'{hit.name}.txt').open('w') as out:
     subprocess.run([sys.executable,str(HERE.parent/'capture_live_stack.py'),str(pid),str(hit)],stdout=out,stderr=subprocess.STDOUT,timeout=45)
    events.append({'symbol':item['symbol'],'address':hex(address),'tid':tid,'capture':str(hit)})
    print(json.dumps(events[-1]),flush=True)
    if tid not in stepping:del points[address]
   elif exception not in (0x80000003,0x406d1388):
    events.append({'exception':hex(exception),'address':hex(address)})
    if exception==0x87a:
     hit=folder/f'exception-{len(events)}'
     with (folder/f'{hit.name}.txt').open('w') as out:
      subprocess.run([sys.executable,str(HERE.parent/'capture_live_stack.py'),str(pid),str(hit)],stdout=out,stderr=subprocess.STDOUT,timeout=45)
  Continue(owner,tid,status)
  if code==5:attached=False;break
finally:
 if attached:
  for at,item in points.items():
   if 'original' in item:
    try:patch(at,item['original'])
    except Exception:pass
  Detach(pid)
 if initialized:Cleanup(process)
 Close(process)
 (folder/'debug-events.json').write_text(json.dumps(events,indent=2))
