import difflib,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def dis(a,b):return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',a,b],cwd=ROOT,text=True,encoding='utf-8')
def asm(text,lo,hi):
    targets=set(re.findall(r'\b(?:j\w+) (00[0-9A-F]{6})\b',text))
    result=[]
    for line in text.splitlines():
        m=re.match(r'^([0-9A-F]{8}): (.*)',line)
        if not m or not lo<=int(m[1],16)<=hi:continue
        ins=m[2]
        if ins=='call 004487D0':ins='call dword ptr [FindGarageRadiusBody]'
        else:ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins)
        result.append('    '+('L'+m[1]+': ' if m[1] in targets else '')+ins)
    return '\n'.join(result)
find=dis('0x44A240','0x44A3C0');setter=dis('0x730AC0','0x730AD0')
(HERE/'disassembly.txt').write_text(find+'\n'+setter,encoding='utf-8')
files={}
p='source/game_sa/Garages.cpp';s=(ROOT/p).read_text(encoding='utf-8')
s=s.replace('    // RH_ScopedInstall(FindGarageForObject, 0x44A240);','    RH_ScopedInstall(FindGarageForObject, 0x44A240);',1)
old='int16 CGarages::FindGarageForObject(CObject* obj) {\n    return plugin::CallAndReturn<int16, 0x44A240, CObject*>(obj);\n}'
new='// Method entry may be redirected by RH; the validation bank proves its active DLL route.\nstatic uintptr_t FindGarageRadiusBody = std::bit_cast<uintptr_t>(static_cast<bool(CGarage::*)(CVector,float)>(&CGarage::IsPointInsideGarage));\n\n// Native x87 order, first nearest candidate wins, placement and matrix positions.\n__declspec(naked) int16 CGarages::FindGarageForObject(CObject*) {\n    __asm {\n'+asm(find,0x44A240,0x44A3B1)+'\n    }\n}'
assert old in s;s=s.replace(old,new,1);files[p]=s
p='source/game_sa/RenderWare/D3DResourceSystem.cpp';s=(ROOT/p).read_text(encoding='utf-8')
s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n\nvoid D3DResourceSystem::InjectHooks() {\n    RH_ScopedClass(D3DResourceSystem);\n    RH_ScopedCategory("RenderWare");\n    RH_ScopedInstall(SetUseD3DResourceBuffering, 0x730AC0);\n}',1)
old='void D3DResourceSystem::SetUseD3DResourceBuffering(bool bUse) {\n    ZoneScoped;\n\n    plugin::Call<0x730AC0, bool>(bUse);\n}'
new='// Writes one byte and preserves EAX high bits, ECX and EDX as in the EXE.\n__declspec(naked) void D3DResourceSystem::SetUseD3DResourceBuffering(bool) {\n    __asm {\n'+asm(setter,0x730AC0,0x730AC9)+'\n    }\n}'
assert old in s;s=s.replace(old,new,1);files[p]=s
p='source/game_sa/RenderWare/D3DResourceSystem.h';s=(ROOT/p).read_text(encoding='utf-8').replace('    static void CancelBuffering();','    static void InjectHooks();\n    static void CancelBuffering();',1);files[p]=s
p='source/InjectHooksMain.cpp';s=(ROOT/p).read_text(encoding='utf-8')
if '#include "RenderWare/D3DResourceSystem.h"' not in s:s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "RenderWare/D3DResourceSystem.h"',1)
s=s.replace('    CPad::InjectHooks();','    CPad::InjectHooks();\n    D3DResourceSystem::InjectHooks();',1);files[p]=s
patch=''.join(''.join(difflib.unified_diff((ROOT/p).read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p)) for p,s in files.items())
(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
