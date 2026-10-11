import difflib, re, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
def dis(start,end):
    return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',start,end],cwd=ROOT,text=True,encoding='utf-8')
def body(text,lo,hi):
    rows=[]
    for line in text.splitlines():
        m=re.match(r'^([0-9A-F]{8}): (.*)',line)
        if not m or not lo<=int(m[1],16)<=hi: continue
        ins=m[2].replace('ds:','')
        ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins)
        rows.append('    L'+m[1]+': '+ins)
    return '\n'.join(rows)
key=dis('0x541490','0x541A60'); fx=dis('0x4AA030','0x4AA0D0')
(HERE/'disassembly.txt').write_text(key+'\n'+fx,encoding='utf-8')
files={}
p='source/game_sa/Pad.cpp'; s=(ROOT/p).read_text(encoding='utf-8')
s=s.replace('    RH_ScopedInstall(Initialise, 0x541D90);','    RH_ScopedGlobalInstall(GetCurrentKeyPressed, 0x541490);\n    RH_ScopedInstall(Initialise, 0x541D90);',1)
s=s.replace('int GetCurrentKeyPressed(RsKeyCodes& keys) {\n    return plugin::CallAndReturn<int, 0x541490, RsKeyCodes&>(keys);\n}', '// Native EAX returns the output address; last rising key in EXE priority wins.\n__declspec(naked) int GetCurrentKeyPressed(RsKeyCodes&) {\n    __asm {\n'+body(key,0x541490,0x541A5A)+'\n    }\n}')
files[p]=s
p='source/game_sa/Fx/FxFrustumInfo.h';s=(ROOT/p).read_text(encoding='utf-8').replace('    bool IsCollision(', '    static void InjectHooks();\n\n    bool IsCollision(',1);files[p]=s
p='source/game_sa/Fx/FxFrustumInfo.cpp'; files[p]='#include "StdInc.h"\n#include "FxFrustumInfo.h"\n\nvoid FxFrustumInfo_c::InjectHooks() {\n    RH_ScopedClass(FxFrustumInfo_c);\n    RH_ScopedCategory("Fx");\n    RH_ScopedInstall(IsCollision, 0x4AA030);\n}\n\n// Circular order starts at the previously rejected plane; success does not change it.\n// Exact x87 comparisons retain unordered and equality branches from the EXE.\n__declspec(naked) bool FxFrustumInfo_c::IsCollision(FxSphere_c&) {\n    __asm {\n'+body(fx,0x4AA030,0x4AA0CA)+'\n    }\n}\n'
p='source/InjectHooksMain.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('        FxSphere_c::InjectHooks();','        FxSphere_c::InjectHooks();\n        FxFrustumInfo_c::InjectHooks();',1)
if '#include "FxFrustumInfo.h"' not in s:s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "FxFrustumInfo.h"',1)
files[p]=s
patch=''
for p,new in files.items():
    old=(ROOT/p).read_text(encoding='utf-8')
    patch+=''.join(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+p,tofile='b/'+p))
(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
