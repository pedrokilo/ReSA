"""Read a paused GTA process: minidump and symbolized x86 thread stacks.

Suspends one thread at a time only while reading its context; always resumes it.
Does not dismiss dialogs, change game memory, or terminate the game.
"""
import ctypes as c
from ctypes import wintypes as w
import json
from pathlib import Path
import struct
import sys

pid=int(sys.argv[1]); out=Path(sys.argv[2]); out.mkdir(parents=True,exist_ok=True)
k=c.WinDLL('kernel32',use_last_error=True)
d=c.WinDLL(r'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow\VsTest\x64\dbghelp.dll',use_last_error=True)
def bind(dll,name,args,result):
    f=getattr(dll,name);f.argtypes=args;f.restype=result;return f
OpenProcess=bind(k,'OpenProcess',[w.DWORD,w.BOOL,w.DWORD],w.HANDLE)
OpenThread=bind(k,'OpenThread',[w.DWORD,w.BOOL,w.DWORD],w.HANDLE)
Close=bind(k,'CloseHandle',[w.HANDLE],w.BOOL)
Suspend=bind(k,'SuspendThread',[w.HANDLE],w.DWORD)
Resume=bind(k,'ResumeThread',[w.HANDLE],w.DWORD)
Context=bind(k,'Wow64GetThreadContext',[w.HANDLE,c.c_void_p],w.BOOL)
Snapshot=bind(k,'CreateToolhelp32Snapshot',[w.DWORD,w.DWORD],w.HANDLE)
class Thread(c.Structure):
    _fields_=[('size',w.DWORD),('usage',w.DWORD),('tid',w.DWORD),('pid',w.DWORD),('priority',w.LONG),('delta',w.LONG),('flags',w.DWORD)]
First=bind(k,'Thread32First',[w.HANDLE,c.POINTER(Thread)],w.BOOL)
Next=bind(k,'Thread32Next',[w.HANDLE,c.POINTER(Thread)],w.BOOL)
Init=bind(d,'SymInitialize',[w.HANDLE,c.c_char_p,w.BOOL],w.BOOL)
bind(d,'SymSetOptions',[w.DWORD],w.DWORD)(0x10|0x2|0x4|0x200|0x2000)
Walk=bind(d,'StackWalk64',[w.DWORD,w.HANDLE,w.HANDLE,c.c_void_p,c.c_void_p,c.c_void_p,c.c_void_p,c.c_void_p,c.c_void_p],w.BOOL)
Symbol=bind(d,'SymFromAddr',[w.HANDLE,c.c_uint64,c.POINTER(c.c_uint64),c.c_void_p],w.BOOL)
class Line(c.Structure):
    _fields_=[('size',w.DWORD),('key',c.c_void_p),('number',w.DWORD),('file',c.c_char_p),('address',c.c_uint64)]
GetLine=bind(d,'SymGetLineFromAddr64',[w.HANDLE,c.c_uint64,c.POINTER(w.DWORD),c.POINTER(Line)],w.BOOL)
process=OpenProcess(0x410,False,pid)
if not process: raise c.WinError(c.get_last_error())
try:
    import msvcrt
    with (out/'process.dmp').open('wb') as f:
        Dump=bind(d,'MiniDumpWriteDump',[w.HANDLE,w.DWORD,w.HANDLE,w.DWORD,c.c_void_p,c.c_void_p,c.c_void_p],w.BOOL)
        if not Dump(process,pid,msvcrt.get_osfhandle(f.fileno()),0x1000,None,None,None):
            print('Minidump error',c.get_last_error())
    if not Init(process,rb'E:\JUEGOS\GTA San Andreas - CVehicle Test\scripts;E:\Original\gta-reversed-master\bin\Debug',True):
        raise c.WinError(c.get_last_error())
    snapshot=Snapshot(4,0);entry=Thread();entry.size=c.sizeof(entry);threads=[]
    try:
        ok=First(snapshot,c.byref(entry))
        while ok:
            if entry.pid==pid:threads.append(entry.tid)
            ok=Next(snapshot,c.byref(entry))
    finally:Close(snapshot)
    records=[]
    for tid in threads:
        thread=OpenThread(0x004a,False,tid)
        if not thread:continue
        suspended=False
        try:
            suspended=Suspend(thread)!=0xffffffff
            if not suspended:continue
            ctx=c.create_string_buffer(716);struct.pack_into('<I',ctx,0,0x10007)
            if not Context(thread,ctx):raise c.WinError(c.get_last_error())
            frame=c.create_string_buffer(4096)
            for offset,value in [(0,184),(32,180),(48,196)]:
                struct.pack_into('<Q',frame,offset,struct.unpack_from('<I',ctx,value)[0]);struct.pack_into('<I',frame,offset+12,3)
            stack=[];seen=set()
            for _ in range(90):
                if not Walk(0x14c,process,thread,frame,ctx,None,d.SymFunctionTableAccess64,d.SymGetModuleBase64,None):break
                pc=struct.unpack_from('<Q',frame,0)[0];fp=struct.unpack_from('<Q',frame,32)[0]
                if not pc or (pc,fp) in seen:break
                seen.add((pc,fp));sym=c.create_string_buffer(4096)
                struct.pack_into('<I',sym,0,88);struct.pack_into('<I',sym,80,3900)
                disp=c.c_uint64();name='?'
                if Symbol(process,pc,c.byref(disp),sym):name=c.string_at(c.addressof(sym)+84).decode(errors='replace')
                line=Line();line.size=c.sizeof(line);ld=w.DWORD()
                source=''
                if GetLine(process,pc,c.byref(ld),c.byref(line)):
                    source=f'{line.file.decode(errors="replace")}:{line.number}'
                stack.append(dict(pc=hex(pc),frame=hex(fp),symbol=name,source=source))
            records.append(dict(tid=tid,stack=stack))
        finally:
            if suspended:Resume(thread)
            Close(thread)
    (out/'stacks.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
    for record in records:
        print('THREAD',record['tid'])
        for frame in record['stack']:print(frame['pc'],frame['symbol'],frame['source'])
finally:
    bind(d,'SymCleanup',[w.HANDLE],w.BOOL)(process)
    Close(process)
