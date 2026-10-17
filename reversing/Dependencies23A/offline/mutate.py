"""Mutaciones del codigo de DoVehicleLights y de los ayudantes x87: cada una debe ser detectada por el ensayo (rehearsal.exe contra los bytes del EXE).
uso: python mutate.py [casos = 4000] [indices separados por coma]. Restaura siempre los textos originales."""
import subprocess, sys, pathlib, shutil
HERE = pathlib.Path(__file__).resolve().parent
G = HERE.parent
FILES = {'x': G.parent / 'g23a-x87.cpp.txt', 'dvl': G / 'src_dvl.txt', 'head': G / 'src_head.txt'}
MUT = [
 ('x', 'AbandonedFar sin plantar |dx|', '    PointLightsPlant1(ax);\n    return far;', '    return far;'),
 ('x', 'AbandonedFar plantando la suma en T-3', '    PointLightsPlant2(sum);\n    PointLightsPlant1(ax);\n    return far;', '    PointLightsPlant3(sum);\n    PointLightsPlant1(ax);\n    return far;'),
 ('x', 'PropRise sin plantar el objetivo', '    PointLightsPlant2(kVehLightsPopUp);\n', ''),
 ('x', 'PropFall sin plantar el resultado', '    PointLightsPlant1(result);\n    *prop = float(result);', '    *prop = float(result);'),
 ('x', 'PropFall max con el orden de argumentos contrario', '(kVehLightsZero > loaded) ? 0.0 : double(loaded)', '(loaded > kVehLightsZero) ? double(loaded) : 0.0'),
 ('x', 'Slow con T-4 y T-3 cambiados', '    PointLightsPlant4(y2);\n    PointLightsPlant3(sum);', '    PointLightsPlant4(sum);\n    PointLightsPlant3(y2);'),
 ('x', 'TrailPoint sin plantar y', '    PointLightsPlant3(yr);\n', ''),
 ('x', 'Brake con >=', 'return double(*brake) > kVehLightsZero;', 'return double(*brake) >= kVehLightsZero;'),
 ('x', 'Negate sin el cambio de signo de z', 'out[2] = float(-double(forward[2]));', 'out[2] = float(double(forward[2]));'),
 ('x', 'DummyZero solo dos componentes', 'for (int i = 0; i < 3; ++i) {\n        volatile double component', 'for (int i = 0; i < 2; ++i) {\n        volatile double component'),
 ('x', 'Geometry sin plantar T-5', '    PointLightsPlant5(stale[0]);\n', ''),
 ('x', 'Geometry sin plantar T-3', '    PointLightsPlant3(stale[2]);\n', ''),
 ('x', 'Positive con >=', 'return double(*dot) > kVehLightsZero;', 'return double(*dot) >= kVehLightsZero;'),
 ('x', 'Limit sin plantar la raiz', '    PointLightsPlant2(root);\n', ''),
 ('x', 'Near con <=', 'return double(*magnitude) < kVehLightsForty;', 'return double(*magnitude) <= kVehLightsForty;'),
 ('x', 'Corona sin plantar la primera R', '    PointLightsPlant3(rounded1);\n', ''),
 ('x', 'Corona con el orden de producto de la funcion en las copias en linea', 'if (inlinedCopy) {   // 0x6E21D0', 'if (!inlinedCopy) {   // 0x6E21D0'),
 ('x', 'Corona sin el recorte a 1.0 del tren', '(kVehLightsOneV < twice) ? double(kVehLightsOneV) : double(twice)', 'double(twice)'),
 ('dvl', 'okRearRight con REAR_RIGHT (el original usa REAR_LEFT)', 'bool okRearRight  = IsLightOk(LIGHT_REAR_LEFT);', 'bool okRearRight  = IsLightOk(LIGHT_REAR_RIGHT);'),
 ('dvl', 'sin borrar la mitad baja de m_renderLights', '    m_nRenderLightsFlags &= 0xF0;\n', ''),
 ('dvl', 'alarma que no anula forceLightsOn', 'forceLightsOn = (CTimer::GetTimeInMS() & 0x100) != 0;', 'forceLightsOn = forceLightsOn || (CTimer::GetTimeInMS() & 0x100) != 0;'),
 ('dvl', 'niebla invertida', 'VehLightsSlowCpp(&m_vecMoveSpeed.x) ? 1 : 0', 'VehLightsSlowCpp(&m_vecMoveSpeed.x) ? 0 : 1'),
 ('dvl', 'AddLight trasero con GetPosition en vez del punto', 'CVector{ trailPoint[0], trailPoint[1], trailPoint[2] }', 'GetPosition()'),
 ('dvl', 'reflejos tambien en trenes', '    if (!IsSubTrain()) {\n        if (alarmDark) {', '    if (true) {\n        if (alarmDark) {'),
]
n = int(sys.argv[1]) if len(sys.argv) > 1 else 4000
only = set(int(x) for x in sys.argv[2].split(',')) if len(sys.argv) > 2 else None
orig = {k: p.read_text(encoding='utf-8') for k, p in FILES.items()}
def restore():
    for k, p in FILES.items():
        p.write_text(orig[k], encoding='utf-8', newline='\n')
undetected = []
try:
    for i, (key, name, old, new) in enumerate(MUT):
        if only and i not in only:
            continue
        text = orig[key]
        assert text.count(old) == 1, (i, name, text.count(old))
        FILES[key].write_text(text.replace(old, new), encoding='utf-8', newline='\n')
        r = subprocess.run(['sh', str(G / 'rebuild.sh')], capture_output=True, text=True)
        if r.stdout.strip():
            print('%2d %-70s ERROR DE COMPILACION %s' % (i, name, r.stdout.strip()[:200])); restore(); continue
        import os
        extra = ['0', 'head'] if os.environ.get('HEADMODE') else []
        out = subprocess.run([str(HERE / 'obj/rehearsal.exe'), str(n), '0', '-'] + extra, capture_output=True, text=True)
        line = [l for l in out.stdout.splitlines() if 'diferencias' in l]
        fails = int(line[0].split(',')[1].split()[0]) if line else -1
        print('%2d %-70s %s' % (i, name, 'detectada (%d diferencias)' % fails if fails else 'NO DETECTADA'))
        if not fails:
            undetected.append(name)
        restore()
finally:
    restore()
    subprocess.run(['sh', str(G / 'rebuild.sh')], capture_output=True, text=True)
print('no detectadas:', undetected)
