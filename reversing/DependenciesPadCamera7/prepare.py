import difflib,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def dis(a,b):return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',hex(a),hex(b)],cwd=ROOT,text=True,encoding='utf-8')
texts=[dis(0x5404A0,0x5404DC),dis(0x540E80,0x540F80),dis(0x540F80,0x541040),dis(0x50A850,0x50A870)]
(HERE/'disassembly.txt').write_text('\n'.join(texts),encoding='utf-8')
def asm(text,lo,hi):
    targets=set(re.findall(r'\b(?:j\w+) (00[0-9A-F]{6})\b',text));out=[]
    for line in text.splitlines():
        m=re.match(r'^([0-9A-F]{8}): (.*)',line)
        if not m or not lo<=int(m[1],16)<=hi:continue
        ins=m[2]
        if ins=='call 0050A850':ins='call dword ptr [MouseCameraModeBody]'
        elif ins=='jmp 00821B40':ins='jmp dword ptr [MouseCameraFtolLibrary]'
        else:
            assert not ins.startswith('call '),ins
            ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins)
        out.append('    '+('L'+m[1]+': ' if m[1] in targets else '')+ins)
    return '\n'.join(out)
files={};p='source/game_sa/Pad.h';s=(ROOT/p).read_text(encoding='utf-8');at=s.index('    static void InjectHooks');s=s[:at]+'    bool GetCameraCycleUp() const;\n    int16 GetMouseCameraStickHorizontal() const;\n    int16 GetMouseCameraStickVertical() const;\n'+s[at:];files[p]=s
p='source/game_sa/Pad.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('#include "Pad.h"','#include "Pad.h"\n#include "Cam.h"',1);at=s.index('    RH_ScopedCategory',s.index('void CPad::InjectHooks()'));end=s.index(';',at)+1;s=s[:end]+'\n    RH_ScopedInstall(GetCameraCycleUp, 0x5404A0);\n    RH_ScopedInstall(GetMouseCameraStickHorizontal, 0x540E80);\n    RH_ScopedInstall(GetMouseCameraStickVertical, 0x540F80);'+s[end:]
# A raw member symbol is mutable under RH OFF; the bank proves it resolves to DLL when candidate helper ON.
s+='\nstatic uintptr_t MouseCameraModeBody=std::bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam);\nstatic uintptr_t MouseCameraFtolLibrary=0x821B40;\n\n// 0x5404A0: native table 5404DC = B3,C1,B3,B3; all candidate targets local.\n__declspec(naked) bool CPad::GetCameraCycleUp() const {\n    __asm {\n        movzx eax,word ptr [ecx+10Ah]\n        cmp eax,3\n        ja invalidMode\n        cmp eax,1\n        je alternate\n        cmp word ptr [ecx+1Ah],0\n        je released\n        cmp word ptr [ecx+4Ah],0\n        jmp previous\n    alternate:\n        cmp word ptr [ecx+10h],0\n        je released\n        cmp word ptr [ecx+40h],0\n    previous:\n        jne released\n        mov eax,1\n        ret\n    released:\n        xor eax,eax\n        ret\n    invalidMode:\n        xor al,al\n        ret\n    }\n}\n'
for text,lo,hi,name in [(texts[1],0x540E80,0x540F7A,'GetMouseCameraStickHorizontal'),(texts[2],0x540F80,0x54103B,'GetMouseCameraStickVertical')]:s+='\n// '+hex(lo)+': exact native x87/integer branches, actual CRT tail call.\n__declspec(naked) int16 CPad::'+name+'() const {\n    __asm {\n'+asm(text,lo,hi)+'\n    }\n}\n'
files[p]=s
p='source/game_sa/Camera.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('return plugin::CallMethodAndReturn<bool, 0x5404A0, CPad*>(pad);','return pad->GetCameraCycleUp();',1)
s=s.replace('plugin::CallMethodAndReturn<int16, 0x540E80, CPad*>(CPad::GetPad(0))','CPad::GetPad(0)->GetMouseCameraStickHorizontal()').replace('plugin::CallMethodAndReturn<int16, 0x540F80, CPad*>(CPad::GetPad(0))','CPad::GetPad(0)->GetMouseCameraStickVertical()');files[p]=s
p='source/game_sa/Cam.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('reinterpret_cast<int16(__thiscall*)(CPad*)>(0x540E80)(CPad::GetPad(0))','CPad::GetPad(0)->GetMouseCameraStickHorizontal()').replace('reinterpret_cast<int16(__thiscall*)(CPad*)>(0x540F80)(CPad::GetPad(0))','CPad::GetPad(0)->GetMouseCameraStickVertical()')
start=s.index('int16 MouseCameraStick(const CPad* pad, bool vertical) {');end=s.index('\nbool TwoPlayerCameraVisible',start);s=s[:start]+'int16 MouseCameraStick(const CPad* pad, bool vertical) {\n    return vertical?pad->GetMouseCameraStickVertical():pad->GetMouseCameraStickHorizontal();\n}\n'+s[end:];files[p]=s
patch=''.join(''.join(difflib.unified_diff((ROOT/p).read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p)) for p,s in files.items())
(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
