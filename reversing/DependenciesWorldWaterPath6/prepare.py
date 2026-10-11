import difflib,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def dis(a,b):return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',hex(a),hex(b)],cwd=ROOT,text=True,encoding='utf-8')
def asm(text,lo,hi):
    targets=set(re.findall(r'\b(?:j\w+) (00[0-9A-F]{6})\b',text));out=[]
    for line in text.splitlines():
        m=re.match(r'^([0-9A-F]{8}): (.*)',line)
        if not m or not lo<=int(m[1],16)<=hi:continue
        ins=m[2]
        if ins.startswith('call '):
            assert ins=='call 00821B40',ins
            ins='call dword ptr [CameraPathFtolLibrary]'
        else:ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins)
        ins=ins.replace('mul eax,edx','mul edx')
        out.append('    '+('L'+m[1]+': ' if m[1] in targets else '')+ins)
    return '\n'.join(out)
spec=[(0x5B2090,0x5B2330,0x5B2321,'CameraPathInterpolateVector','void CameraPathInterpolateVector(CVector*,const float*,float,int32*)'),(0x5B2330,0x5B24A0,0x5B249A,'CameraPathInterpolateScalar','void CameraPathInterpolateScalar(float*,const float*,float,int32*)'),(0x6E5BB0,0x6E5E90,0x6E5E85,'GetWaterLevelNoWavesForQuad','bool CWaterLevel::GetWaterLevelNoWavesForQuad(const CWaterQuad*,float,float,float,float*,float*,float*)'),(0x6E5E90,0x6E61B0,0x6E61A7,'GetWaterLevelNoWavesForTriangle','bool CWaterLevel::GetWaterLevelNoWavesForTriangle(const CWaterTriangle*,float,float,float,float*,float*,float*)')]
texts=[dis(a,b) for a,b,_,_,_ in spec];(HERE/'disassembly.txt').write_text('\n'.join(texts),encoding='utf-8')
files={}
p='source/game_sa/Cam.h';s=(ROOT/p).read_text(encoding='utf-8');s+='\n// Descriptive aliases for native cutscene spline interpolation helpers.\nvoid CameraPathInterpolateVector(CVector*,const float*,float,int32*);\nvoid CameraPathInterpolateScalar(float*,const float*,float,int32*);\n';files[p]=s
p='source/game_sa/Cam.cpp';s=(ROOT/p).read_text(encoding='utf-8');needle='    RH_ScopedCategory("Camera");';assert needle in s;s=s.replace(needle,needle+'\n    RH_ScopedGlobalInstall(CameraPathInterpolateVector, 0x5B2090);\n    RH_ScopedGlobalInstall(CameraPathInterpolateScalar, 0x5B2330);',1)
s=s.replace('plugin::Call<0x5B2090>','CameraPathInterpolateVector').replace('plugin::Call<0x5B2330>','CameraPathInterpolateScalar')
s+='\n// Actual EXE CRT conversion library (ST0 input).\nstatic uintptr_t CameraPathFtolLibrary=0x821B40;\n'
for i in range(2):a,b,end,name,decl=spec[i];s+='\n// '+hex(a)+': exact native spline arithmetic and index writes.\n__declspec(naked) '+decl+' {\n    __asm {\n'+asm(texts[i],a,end)+'\n    }\n}\n'
files[p]=s
p='source/game_sa/WaterLevel.h';s=(ROOT/p).read_text(encoding='utf-8');needle='    static bool GetWaterLevelNoWaves(CVector';at=s.index(needle);s=s[:at]+'    static bool GetWaterLevelNoWavesForQuad(const CWaterQuad*,float,float,float,float*,float*,float*);\n    static bool GetWaterLevelNoWavesForTriangle(const CWaterTriangle*,float,float,float,float*,float*,float*);\n'+s[at:];files[p]=s
p='source/game_sa/WaterLevel.cpp';s=(ROOT/p).read_text(encoding='utf-8');needle='    RH_ScopedGlobalInstall(GetWaterLevelNoWaves, 0x6E8580);';assert needle in s;s=s.replace(needle,needle+'\n    RH_ScopedInstall(GetWaterLevelNoWavesForQuad, 0x6E5BB0);\n    RH_ScopedInstall(GetWaterLevelNoWavesForTriangle, 0x6E5E90);',1)
s=s.replace('plugin::CallAndReturn<bool, 0x6E5BB0>','GetWaterLevelNoWavesForQuad').replace('plugin::CallAndReturn<bool, 0x6E5E90>','GetWaterLevelNoWavesForTriangle')
for i in range(2,4):a,b,end,name,decl=spec[i];s+='\n// '+hex(a)+': reads real loaded water atlas; no game collaborator calls.\n__declspec(naked) '+decl+' {\n    __asm {\n'+asm(texts[i],a,end)+'\n    }\n}\n'
files[p]=s
patch=''.join(''.join(difflib.unified_diff((ROOT/p).read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p)) for p,s in files.items())
(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
