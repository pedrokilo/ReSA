"""Controles negativos del arnes sin juego: siembra un error en el C++ extraido (cand.inc), recompila y ejecuta; la funcion afectada debe dar diferencias.
Uso: python mutate.py   (requiere haber ejecutado extract.py/bld.bat antes; deja cand.inc intacto al terminar)"""
import json
import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
NLOH = os.environ.get('NLOH', str(HERE.parents[1] / 'CVehicle/conan-cache/p/nlohmd014ef7748f4b/p/include'))
VC = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
MUTATIONS = [
    ('RpClumpGetNumAtomics', 'RpWorld::RpClumpGetNumAtomics', '++numAtomics;', ''),
    ('AnisoSet escribe un DWORD', 'RpAnisot::RpAnisotTextureSetMaxAnisotropy', '*reinterpret_cast<RwInt8*>(reinterpret_cast<uintptr_t>(tex)', '*reinterpret_cast<RwInt32*>(reinterpret_cast<uintptr_t>(tex)'),
    ('TriMat con signo', 'RpWorld::RpGeometryTriangleGetMaterial', 'const RwUInt16 matIndex = triangle->matIndex;\n    if (matIndex == 0xFFFFu)', 'const short matIndex = (short)triangle->matIndex;\n    if (matIndex == -1)'),
    ('MatFind devuelve n-2', 'RpWorld::_rpMaterialListFindMaterialIndex', '- 1u);', '- 2u);'),
    ('IndexBuf usa > 0', 'RwCore::RwD3D9IndexBufferCreate', 'indexBuffer) >= 0;', 'indexBuffer) > 0;'),
    ('WorldReg intercambia size/id', 'RpWorld::RpWorldRegisterPlugin', '&s_worldTKList, size, pluginID,', '&s_worldTKList, pluginID, size,'),
    ('Vdecl trunca el tipo', 'RpWorld::_rpD3D9VertexDeclarationGetSize', 'return sizes[type];', 'return sizes[type & 0xFFFF];'),
    ('SecGet con el desplazamiento de geometria', 'RpWorld::RpD3D9WorldSectorGetUsageFlags', 'static_cast<uintptr_t>(s_sectorUsageFlagsOffset));\n}\n', 'static_cast<uintptr_t>(s_geometryUsageFlagsOffset));\n}\n'),
    ('AtomRights usa el registro del mundo', 'RpWorld::RpAtomicSetStreamRightsCallBack', '&s_atomicTKList, pluginID, rightsCB', '&s_worldTKList, pluginID, rightsCB'),
    ('MatInit no pone space a cero', 'RpWorld::_rpMaterialListInitialize', 'matList->space        = 0;', ''),
    ('MatGet con indice de 16 bits', 'RpWorld::_rpMaterialListGetMaterial', 'matList->materials[matIndex];', 'matList->materials[(short)matIndex];'),
    ('MaxSupported lee +0x6D', 'RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy', ' + offsetof(D3DCAPS9, MaxAnisotropy));', ' + offsetof(D3DCAPS9, MaxAnisotropy) + 1);'),
    ('Light usa el registro del mundo', 'RpWorld::RpLightRegisterPlugin', '&s_lightTKList, size,', '&s_worldTKList, size,'),
    ('AtomWorld lee desplazamiento de sector', 'RpWorld::RpAtomicGetWorld', 'static_cast<uintptr_t>(s_atomicWorldOffset));', 'static_cast<uintptr_t>(s_sectorUsageFlagsOffset));'),
]


def run(cmd):
    bat = HERE / 'mut_build.bat'
    lines = ['@echo off', 'call "%s" >nul 2>&1' % VC, 'cd /d "%s"' % HERE, cmd]
    bat.write_text('\r\n'.join(lines) + '\r\n', encoding='ascii')
    return subprocess.run([str(bat)], capture_output=True, text=True, encoding='utf-8', errors='replace')


def main():
    original = (HERE / 'cand.inc').read_text(encoding='utf-8')
    bad = 0
    try:
        for label, fn, old, new in MUTATIONS:
            if original.count(old) != 1:
                print('SIN APLICAR (%s): %d apariciones' % (label, original.count(old)))
                bad += 1
                continue
            (HERE / 'cand.inc').write_text(original.replace(old, new), encoding='utf-8', newline='\n')
            r = run(f'cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /I"{NLOH}" main.cpp /Fe:obj/mut.exe /Fdobj/mut.pdb /Foobj/mut_ /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO')
            if 'error' in (r.stdout + r.stderr).lower() and 'error C' in r.stdout:
                print('NO COMPILA (%s): %s' % (label, r.stdout[-400:]))
                bad += 1
                continue
            out = HERE / 'mut.jsonl'
            if out.exists():
                out.unlink()
            proc = subprocess.run([str(HERE / 'obj/mut.exe'), '1024', str(out)], cwd=HERE, capture_output=True, timeout=900)
            fails, summarized = {}, set()
            if out.exists():
                for line in out.read_text(encoding='utf-8').splitlines():
                    if '"function_summary"' in line:
                        j = json.loads(line)
                        summarized.add(j['function'])
                        if j['failures']:
                            fails[j['function']] = j['failures']
            crashed = fn not in summarized
            hit = fails.get(fn, 0)
            others = {k: v for k, v in fails.items() if k != fn}
            detected = (hit > 0 or crashed) and not others
            print('%-44s %-52s diferencias=%4d %s otras=%s %s' % (label, fn, hit, 'CAIDA(exit=%d)' % proc.returncode if crashed else '', others, 'OK' if detected else 'FALLO DEL CONTROL'))
            bad += 0 if detected else 1
    finally:
        (HERE / 'cand.inc').write_text(original, encoding='utf-8', newline='\n')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
