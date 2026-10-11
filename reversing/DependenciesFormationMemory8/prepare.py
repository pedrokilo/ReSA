import difflib,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def dis(a,b):return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',hex(a),hex(b)],cwd=ROOT,text=True,encoding='utf-8')
formation=dis(0x69B1B0,0x69B240);memory=dis(0x745510,0x745520);(HERE/'disassembly.txt').write_text(formation+'\n'+memory,encoding='utf-8')
targets=set(re.findall(r'\b(?:j\w+) (00[0-9A-F]{6})\b',formation));body=[]
for line in formation.splitlines():
    m=re.match(r'^([0-9A-F]{8}): (.*)',line)
    if not m or not 0x69B1B0<=int(m[1],16)<=0x69B230:continue
    ins=m[2];assert not ins.startswith('call '),ins;ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins);body.append('    '+('L'+m[1]+': ' if m[1] in targets else '')+ins)
files={};p='source/game_sa/Formation.h';s=(ROOT/p).read_text(encoding='utf-8');needle='    static void InjectHooks();';assert needle in s;s=s.replace(needle,needle+'\n    static int32 FindNearestFreeDestination(CVector,float*);',1);files[p]=s
p='source/game_sa/Formation.cpp';s=(ROOT/p).read_text(encoding='utf-8');needle='    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240);';assert needle in s;s=s.replace(needle,'    RH_ScopedGlobalInstall(FindNearestFreeDestination, 0x69B1B0);\n'+needle,1);s=s.replace('plugin::CallAndReturn<int32, 0x69B1B0, CVector, float*>(positions[i], &total)','FindNearestFreeDestination(positions[i], &total)');s+='\n// 0x69B1B0: native first-minimum, claimed mask and x87 distance/total writes.\n__declspec(naked) int32 CFormation::FindNearestFreeDestination(CVector,float*) {\n    __asm {\n'+'\n'.join(body)+'\n    }\n}\n';files[p]=s
p='source/app/platform/platform.h';s=(ROOT/p).read_text(encoding='utf-8');s+='\nRwMemoryFunctions* psGetMemoryFunctions();\n';files[p]=s
p='source/app/platform/platform.cpp';s=(ROOT/p).read_text(encoding='utf-8');needle='    RH_ScopedCategoryGlobal();';s=s.replace(needle,needle+'\n    RH_ScopedGlobalInstall(psGetMemoryFunctions, 0x745510);',1);old='RwMemoryFunctions* psGetMemoryFunctions() {\n    return plugin::CallAndReturn<RwMemoryFunctions*, 0x745510>();\n    // return &g_objMemFunctions;\n}';assert old in s;s=s.replace(old,'// Native getter returns the actual initialized EXE RW callback table.\n__declspec(naked) RwMemoryFunctions* psGetMemoryFunctions() {\n    __asm {\n        mov eax,8D6228h\n        ret\n    }\n}',1);files[p]=s
patch=''.join(''.join(difflib.unified_diff((ROOT/p).read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p)) for p,s in files.items());(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
