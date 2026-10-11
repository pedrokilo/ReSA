import difflib,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def dis(a,b):return subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/dis.sh',a,b],cwd=ROOT,text=True,encoding='utf-8')
def asm(text,lo,hi,calls):
    targets=set(re.findall(r'\b(?:j\w+) (00[0-9A-F]{6})\b',text));result=[]
    for line in text.splitlines():
        m=re.match(r'^([0-9A-F]{8}): (.*)',line)
        if not m or not lo<=int(m[1],16)<=hi:continue
        ins=m[2]
        if ins.startswith('call '):
            target=ins[5:];assert target in calls,(m[1],target);ins=calls[target]
        else:ins=re.sub(r'\b(00[0-9A-F]{6})(?=$|\s)',lambda x:'L'+x[1],ins)
        result.append('    '+('L'+m[1]+': ' if m[1] in targets else '')+ins)
    return '\n'.join(result)
fx=dis('0x49E950','0x49EA40');font=dis('0x719670','0x719750');cam=dis('0x420C40','0x420D40');radar=dis('0x585040','0x5853D0')
(HERE/'disassembly.txt').write_text(fx+'\n'+font+'\n'+cam+'\n'+radar,encoding='utf-8')
files={}
p='source/game_sa/Fx/Fx.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('    // RH_ScopedInstall(CreateMatFromVec, 0x49E950);','    RH_ScopedInstall(CreateMatFromVec, 0x49E950);',1)
start=s.index('// 0x49E950');end=s.index('// 0x49EA40',start)
new='// Actual RenderWare libraries; no game collaborator or substitute.\nstatic uintptr_t FxNormalizeLibrary=0x7ED9B0;\nstatic uintptr_t FxMatrixUpdateLibrary=0x7F18A0;\n\n// 0x49E950: preserve native write/alias order and x87 arithmetic.\n__declspec(naked) void Fx_c::CreateMatFromVec(RwMatrix*,const CVector*,const CVector*) {\n    __asm {\n'+asm(fx,0x49E950,0x49EA32,{'007ED9B0':'call dword ptr [FxNormalizeLibrary]','007F18A0':'call dword ptr [FxMatrixUpdateLibrary]'})+'\n    }\n}\n\n';s=s[:start]+new+s[end:];files[p]=s
p='source/game_sa/Font.h';s=(ROOT/p).read_text(encoding='utf-8').replace('static float GetScriptLetterSize(uint8 letterId);','float GetScriptLetterSize(uint8 letterId);',1);files[p]=s
p='source/game_sa/Font.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('    // Install("", "GetScriptLetterSize", 0x719670, &GetScriptLetterSize);','    RH_ScopedGlobalInstall(GetScriptLetterSize, 0x719670);',1)
old='float GetScriptLetterSize(uint8 letterId) {\n    return plugin::CallAndReturn<float, 0x719670, uint8>(letterId);\n}'
new='// RH mutable entry: bank enables helper and proves resolved DLL body.\nstatic uintptr_t ScriptSubFontBody=reinterpret_cast<uintptr_t>(&CFont::FindSubFontCharacter);\n// Native7192C0 preserves ECX/EDX; the new caller depends on CL across this call.\n// Adapt the reconstructed C++ helper ABI, without calling the original game helper.\n__declspec(naked) static void ScriptSubFontNativeAbi() {\n    __asm {\n        push ecx\n        push edx\n        push dword ptr [esp+16]\n        push dword ptr [esp+16]\n        call dword ptr [ScriptSubFontBody]\n        add esp,8\n        pop edx\n        pop ecx\n        ret\n    }\n}\n\n__declspec(naked) float GetScriptLetterSize(uint8) {\n    __asm {\n'+asm(font,0x719670,0x71974B,{'007192C0':'call ScriptSubFontNativeAbi'})+'\n    }\n}'
assert old in s;s=s.replace(old,new,1);files[p]=s
p='source/game_sa/Camera.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('    RH_ScopedInstall(GetArrPosForVehicleType, 0x50AF00);','    RH_ScopedOverloadedInstall(IsSphereVisible, "matrix", 0x420C40, bool(CCamera::*)(const CVector&,float,RwMatrix*));\n    RH_ScopedInstall(GetArrPosForVehicleType, 0x50AF00);',1)
old='bool CCamera::IsSphereVisible(const CVector& origin, float radius, RwMatrix* transformMatrix) {\n    return plugin::CallMethodAndReturn<bool, 0x420C40, CCamera*, const CVector&, float, RwMatrix*>(this, origin, radius, transformMatrix);\n}'
new='static uintptr_t CameraTransformPointsLibrary=0x7EDD90;\n// Native camera frustum comparisons; actual RW library transforms one copied point.\n__declspec(naked) bool CCamera::IsSphereVisible(const CVector&,float,RwMatrix*) {\n    __asm {\n'+asm(cam,0x420C40,0x420D3C,{'007EDD90':'call dword ptr [CameraTransformPointsLibrary]'})+'\n    }\n}'
assert old in s;s=s.replace(old,new,1);files[p]=s
p='source/game_sa/Radar.cpp';s=(ROOT/p).read_text(encoding='utf-8');s=s.replace('    // RH_ScopedInstall(ClipRadarPoly, 0x585040);','    RH_ScopedInstall(ClipRadarPoly, 0x585040);',1)
old='int32 CRadar::ClipRadarPoly(CVector2D* out, const CVector2D* in) {\n    return plugin::CallAndReturn<int32, 0x585040, CVector2D*, const CVector2D*>(out, in);\n}'
new='// The bank proves this mutable RH entry resolves to the reconstructed helper.\nstatic uintptr_t RadarBoxCollisionBody=reinterpret_cast<uintptr_t>(&LineRadarBoxCollision);\n\n__declspec(naked) int32 CRadar::ClipRadarPoly(CVector2D*,const CVector2D*) {\n    __asm {\n'+asm(radar,0x585040,0x5853C9,{'00584E00':'call dword ptr [RadarBoxCollisionBody]'})+'\n    }\n}'
assert old in s;s=s.replace(old,new,1);files[p]=s
patch=''.join(''.join(difflib.unified_diff((ROOT/p).read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p)) for p,s in files.items())
(HERE/'source-unapplied.patch').write_text(patch,encoding='utf-8',newline='\n')
