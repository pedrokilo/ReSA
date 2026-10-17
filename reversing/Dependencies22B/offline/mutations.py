"""Sensibilidad del ensayo: aplica mutaciones al codigo de la DLL (glue_head.txt, glue_tail.txt, x87src/X87Math.cpp) en una copia temporal, recompila y ejecuta rehearsal.exe con los
mismos pares del banco; cada mutacion debe producir diferencias (las marcadas 'equivalente' no cambian el comportamiento observable y se dejan para documentarlo).
Uso: python mutations.py [casos por funcion = 4096] [prefijos de mutacion...]     (requiere haber generado glue_*.txt y x87src con extract_glue.py / make_x87src.py)"""
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
# (nombre, archivo, patron, reemplazo, equivalente)
MUTATIONS = [
    ('M01 limite 0.9 -> 0.91', 'glue_head.txt', '(isTrain ? 0.85f : 0.9f)', '(isTrain ? 0.85f : 0.91f)', False),
    ('M02 limite del tren 0.85 -> 0.9', 'glue_head.txt', '(isTrain ? 0.85f : 0.9f)', '(isTrain ? 0.9f : 0.9f)', False),
    ('M03 distancia < 40 -> <= 40', 'glue_head.txt', 'magnitude < 40.0f', 'magnitude <= 40.0f', False),
    ('M04 pull de la corona 2', 'glue_head.txt', 'sqrtDot, false, 0.5f, false, 15.0f, false, false', 'sqrtDot, false, 0.6f, false, 15.0f, false, false', False),
    ('M05 tamano de la linea de luz del tren', 'glue_head.txt', 'isTrain ? 0.3f : 0.075f', 'isTrain ? 0.3f : 0.07f', False),
    ('M06 colores halogenos', 'glue_head.txt', 'halogen ? 0xC3 : 0x8C', 'halogen ? 0xC4 : 0x8C', False),
    ('M07 id +4 de la linea de luz', 'glue_head.txt', 'id + 4, this', 'id + 2, this', False),
    ('M08 espejo por !=0', 'glue_head.txt', 'isRight == 0, reinterpret_cast', 'isRight != 1, reinterpret_cast', False),
    ('M09 dot > 0 -> >= 0 (cabeza)', 'glue_head.txt', 'if (!(dot > 0.0f)) {', 'if (dot < 0.0f) {', False),
    ('M10 tren: tranvia por subtipo', 'glue_head.txt', 'm_nVehicleSubType == VEHICLE_TYPE_TRAIN && m_nModelIndex != MODEL_TRAM', 'm_nVehicleSubType == VEHICLE_TYPE_TRAIN', False),
    ('M11 camara siempre en placement', 'glue_head.txt', '&TheCamera.GetPosition().x', '&TheCamera.m_placement.m_vPosn.x', False),
    ('M12 cola: brake >= 0', 'glue_tail.txt', 'm_BrakePedal > 0.0f', 'm_BrakePedal >= 0.0f', False),
    ('M13 cola: sin comprobar el freno de mano', 'glue_tail.txt', '!vehicleFlags.bIsHandbrakeOn && ', '', False),
    ('M14 cola: id +8 -> +4', 'glue_tail.txt', 'static_cast<uint32>(lightId) * 2 + 8, this', 'static_cast<uint32>(lightId) * 2 + 4, this', False),
    ('M15 cola: Stallion sin Sabre', 'glue_tail.txt', 'm_nModelIndex == MODEL_STALLION || m_nModelIndex == MODEL_SABRE', 'm_nModelIndex == MODEL_STALLION', False),
    ('M16 cola: panel 6 -> 5', 'glue_tail.txt', 'GetPanelStatus(REAR_BUMPER)', 'GetPanelStatus(FRONT_BUMPER)', False),
    ('M17 cola: dummy 2*id+1 -> 2*id', 'glue_tail.txt', 'static_cast<eVehicleDummy>(2 * lightId + 1)', 'static_cast<eVehicleDummy>(2 * lightId)', False),
    ('M18 cola: retorno siempre true', 'glue_tail.txt', 'return mode != 0;', 'return true;', False),
    ('M19 x87: orden del producto escalar (cabeza)', 'x87src/X87Math.cpp', '*dot = float((tz + ty) + tx);\n    offset[0] = v[0];\n    offset[1] = v[1];\n    offset[2] = v[2];\n}\n\n// CVehicle::DoTailLightEffect 0x6E1820',
     '*dot = float((tx + ty) + tz);\n    offset[0] = v[0];\n    offset[1] = v[1];\n    offset[2] = v[2];\n}\n\n// CVehicle::DoTailLightEffect 0x6E1820', False),
    ('M20 x87: producto del transform en float', 'x87src/X87Math.cpp', 'const double xa = JpFixMemOp(kJpFixMul, double(m[8]), v + 2);', 'const double xa = float(JpFixMemOp(kJpFixMul, double(m[8]), v + 2));', False),
    ('M21 x87: suma de cuadrados en otro orden', 'x87src/X87Math.cpp', 'const double sum = (x * x + y * y) + z * z;', 'const double sum = (z * z + y * y) + x * x;', False),
    ('M22 x87: NormaliseAndMag con sum < 0', 'x87src/X87Math.cpp', 'if (sum <= 0.0) {', 'if (sum < 0.0) {', False),
    ('M23 x87: recip en float', 'x87src/X87Math.cpp', 'const double recip = 1.0 / root;', 'const double recip = float(1.0 / root);', False),
    ('M24 x87: magnitud = sqrt', 'x87src/X87Math.cpp', 'return float(1.0 / recip);', 'return float(root);', False),
    ('M25 x87: espejo = -x', 'x87src/X87Math.cpp', 'v[0] = float(JpFixMemOp(kJpFixSubr, twice, v + 0));', 'v[0] = -v[0]; (void)twice;', False),
    ('M26 x87: seno en float', 'x87src/X87Math.cpp', 'x1 = JetPackNegSinExtCpp(heading) * kJpFixHeight;', 'x1 = float(JetPackNegSinExtCpp(heading)) * kJpFixHeight;', False),
    ('M27 x87: producto escalar de la cola sin negar', 'x87src/X87Math.cpp', 'JpFixMemOp(kJpFixMul, -double(matrix[6]), d + 2)', 'JpFixMemOp(kJpFixMul, double(matrix[6]), d + 2)', False),
    ('M28 x87: tren sin tope 1.0 (cabeza)', 'x87src/X87Math.cpp', '        a = a + a;\n        if (1.0 < a) {\n            a = 1.0;\n        }', '        a = a + a;', False),
    ('M29 x87: modo 2 usa 128', 'x87src/X87Math.cpp', 'a * 96.0, 0', 'a * 128.0, 0', False),
    ('M30 x87: tamano del tren x3', 'x87src/X87Math.cpp', 'sz = float(double(sz) * 4.0);', 'sz = float(double(sz) * 3.0);', False),
    ('M31 x87: _ftol2 por cast a int con saturacion (equivalente: /arch:IA32 compila (int)x como _ftol2)', 'x87src/X87Math.cpp', '*red = Ftol2RegsDblCpp(190.0 * a, 0).eax;', '*red = (unsigned)(int)(190.0 * a);', True),
    ('M32 x87: rango en float (equivalente: 150 * subnormal es exacto)', 'x87src/X87Math.cpp', 'return float(double(lodMultiplier) * kVehHeadTail150);', 'return lodMultiplier * 150.0f;', True),
]


def main():
    args = sys.argv[1:]
    cases = int(args[0]) if args and args[0].isdigit() else 4096
    prefixes = [a for a in args if not a.isdigit()]
    nlohmann = os.environ.get('NLOHMANN', '')
    results = []
    for name, rel, old, new, equivalent in MUTATIONS:
        if prefixes and not any(name.startswith(p) for p in prefixes):
            continue
        with tempfile.TemporaryDirectory() as tmp:
            work = Path(tmp) / 'offline'
            shutil.copytree(HERE, work, ignore=shutil.ignore_patterns('obj', '*.jsonl', 'long*.out'))
            shutil.copy2(HERE.parent / 'Fixtures.hpp', Path(tmp) / 'Fixtures.hpp')
            target = work / rel
            text = target.read_bytes().decode('utf-8').replace('\r\n', '\n')
            # en X87Math.cpp solo se muta el bloque anexado (VehHeadTail*): hay patrones repetidos en funciones anteriores del archivo
            start = text.index('lote g22b, prefijo VehHeadTail') if rel.startswith('x87src') else 0
            if old not in text[start:]:
                print(name, '-> PATRON NO ENCONTRADO')
                continue
            target.write_text(text[:start] + text[start:].replace(old, new, 1), encoding='utf-8', newline='\n')
            env = dict(os.environ, NLOHMANN=nlohmann)
            build = subprocess.run(['cmd', '/c', str(work / 'build.bat')], capture_output=True, text=True, cwd=work, env=env)
            if not (work / 'obj' / 'rehearsal.exe').exists():
                print(name, '-> NO COMPILA\n', build.stdout[-600:])
                continue
            run = subprocess.run([str(work / 'obj' / 'rehearsal.exe'), str(cases), '0', '-'], capture_output=True, text=True, cwd=work)
            found = re.findall(r'(Head|Tail|CVehicle::\w+): (\d+) casos \(desde \d+\), (\d+) diferencias', run.stdout)
            summary = ' | '.join('%s: %s' % (n.split('::')[-1], d) for n, c, d in found)
            total = sum(int(d) for _, _, d in found)
            verdict = 'DETECTADA' if total else ('equivalente, sin diferencias' if equivalent else 'NO DETECTADA')
            print('%-70s %s  [%s]' % (name, summary, verdict), flush=True)
            results.append((name, total, equivalent))
    undetected = [n for n, t, e in results if not t and not e]
    print('Sin detectar (no equivalentes): %s' % (undetected or 'ninguna'))
    return 1 if undetected else 0


if __name__ == '__main__':
    sys.exit(main())
