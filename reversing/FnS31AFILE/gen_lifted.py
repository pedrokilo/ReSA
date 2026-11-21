# Genera los cuerpos transcritos literalmente (lift.py) de 0x75D240, 0x75C9D0 y 0x75DAE0 con su cabecera de funcion de C++.
# Uso: python gen_lifted.py > lifted_gen.txt   (desde la raiz del repositorio; lee el EXE en solo lectura)
import subprocess, sys, re

BIAS = 0x100


def lift(addr, size, nargs, calls, callreg=None):
    cmd = [sys.executable, 'reversing/FnS31AFILE/lift.py', addr, str(size), 'x', str(nargs)] + calls
    r = subprocess.run(cmd, capture_output=True, text=True, cwd='.', env={**__import__('os').environ})
    if r.returncode:
        sys.stderr.write(r.stderr + r.stdout)
        raise SystemExit(1)
    return r.stdout


def emit(name, rettype, params, addr, size, calls, comment, retcast, subs=()):
    body = lift(addr, size, len(params), calls)
    for a, b in subs:
        body = body.replace(a, b)
    body = body.replace('RET_VALUE', retcast)
    print(comment)
    print(f'{rettype} {name}({", ".join(t + " " + n for t, n in params)}) {{')
    print('    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;')
    print('    bool cf = false, zf = false, sf = false, of = false;')
    print('    alignas(4) uint8_t fr[0x180] = {};')
    for i, (t, n) in enumerate(params):
        print(f'    *reinterpret_cast<uint32_t*>(fr + {BIAS + 4 + 4 * i}) = static_cast<uint32_t>((uintptr_t)({n}));')
    print('    (void)cf; (void)zf; (void)sf; (void)of;')
    for l in body.splitlines():
        print('    ' + l)
    print('}')
    print()


emit('TriStripNeighbours', 'void',
     [('RpTriStripPolygon*', 'poly'), ('RpTriStripPolygon**', 'current'), ('RpTriStripPolygon**', 'next'), ('RpTriStripPolygon**', 'afterNext'),
      ('RxVertexIndex*', 'vertex0'), ('RxVertexIndex*', 'vertex1'), ('RxVertexIndex*', 'vertex2')],
     '75D240', 640, [], '// 0x75D240', 'void()')
emit('TriStripMeshCreate', 'RpTriStripMesh*',
     [('RpBuildMeshTriangle**', 'triangles'), ('RwUInt32', 'numTriangles')],
     '75C660', 880, [], '// 0x75C660', 'reinterpret_cast<RpTriStripMesh*>(static_cast<uintptr_t>(eax))')
emit('TriStripEmit', 'void',
     [('RpTriStripMesh*', 'stripMesh'), ('RpMesh*', 'record'), ('RpTriStripData*', 'stripData')],
     '75C9D0', 2160, ['0x75d240=T_TriStripNeighbours/7'], '// 0x75C9D0', 'void()')
emit('RpGeometrySortByMaterial', 'RpGeometry*',
     [('const RpGeometry*', 'geometry'), ('RpGeometrySortByMaterialCallBack', 'callback')],
     '75DAE0', 1858, ['0x74bfd0=T_GeometryPluginFirst/0', '0x74c200=T_MorphTargetCalcBoundingSphere/2', '0x74c4d0=T_GeometryAddMorphTarget/1',
                      '0x74c7d0=T_GeometryLock/2', '0x74c800=T_GeometryUnlock/1', '0x74ca90=T_GeometryCreate/3', '0x74ccc0=T_GeometryDestroy/1',
                      '0x74e1f0=T_MaterialListCopy/2', '0x8247e0=T_Qsort/4'], '// 0x75DAE0', 'reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax))',
     [('0x75e230u', 'static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&GeometrySortCompare))')])
emit('RpBuildMeshGenerateTriStrip', 'RpMeshHeader*',
     [('RpBuildMesh*', 'buildMesh'), ('void*', 'data')],
     '75C380', 731, ['0x8247e0=T_Qsort/4', '0x75c660=T_TriStripMeshCreate/2', '0x75c9d0=T_TriStripEmit/3'], '// 0x75C380',
     'reinterpret_cast<RpMeshHeader*>(static_cast<uintptr_t>(eax))',
     [('0x759640u', 'static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&BuildMeshTriangleCompare))')])
