"""Mutaciones del cuerpo de la DLL (PlaySound y el ayudante x87) para comprobar que el ensayo sin juego la detecta.
Uso: python mutations.py <raiz del repositorio> <directorio de superposicion con game_sa/ ya parcheado> [ruta del gta-sa.exe] [indices separados por comas]
Requiere haber ejecutado antes build.bat (con la misma raiz y superposicion): reutiliza obj\\hmain.obj, obj\\Hw_h.obj, obj\\X87Math.obj y proj.rsp. Cada mutacion se aplica a una COPIA de la superposicion, se
recompila solo el objeto afectado, se enlaza y se ejecuta h.exe con 2048 casos; se espera al menos una diferencia ('TOTAL ... N diferencias', N > 0).
Las mutaciones marcadas 'equivalente' no cambian el comportamiento observable (el falso del colaborador enmascara a 16 bits) y NO deben detectarse."""
import os, re, shutil, subprocess, sys, tempfile
from pathlib import Path

here = Path(__file__).resolve().parent
root = Path(sys.argv[1]).resolve()
overlay = Path(sys.argv[2]).resolve()
exe = sys.argv[3] if len(sys.argv) > 3 else r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
only = set(sys.argv[4].split(',')) if len(sys.argv) > 4 else None
HW = 'game_sa/Audio/Hardware/AEAudioHardware.cpp'
X87 = 'game_sa/X87Math.cpp'
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
# (nombre, fichero, antes, despues, equivalente)
MUTATIONS = [
    ('canal <= 0 en lugar de < 0', HW, 'if (channel < 0) {', 'if (channel <= 0) {', False),
    ('ranura > cuenta en lugar de >=', HW, 'if (channelSlot >= reinterpret_cast', 'if (channelSlot > reinterpret_cast', False),
    ('indice >= 64 en lugar de > 64', HW, 'if (channelIndex > MAX_NUM_AUDIO_CHANNELS) {', 'if (channelIndex >= MAX_NUM_AUDIO_CHANNELS) {', False),
    ('indice sin sumar el canal', HW, 'static_cast<uint16>(channelSlot + channel)', 'static_cast<uint16>(channelSlot)', False),
    ('comprobacion del indice con la suma de 32 bits', HW, 'if (channelIndex > MAX_NUM_AUDIO_CHANNELS) {', 'if (static_cast<uint32>(channelSlot + channel) > MAX_NUM_AUDIO_CHANNELS) {', False),
    ('porcentaje con posicion >= 0', HW, '(flags & 8) && position > 0', '(flags & 8) && position >= 0', False),
    ('porcentaje con el bit 4', HW, '(flags & 8) && position > 0', '(flags & 0x10) && position > 0', False),
    ('onStreet invertido', HW, 'SetNotInRoom(!(flags & 0x200))', 'SetNotInRoom((flags & 0x200) != 0)', False),
    ('sin SetNotInRoom', HW, 'if (m_n3dEffectsQueryResult) {\n        (*channelSlotPtr)->SetNotInRoom', 'if (false) {\n        (*channelSlotPtr)->SetNotInRoom', False),
    ('tabla de flags con el indice de 16 bits', HW, '[channel + channelSlot] = flags;', '[channelIndex] = flags;', False),
    ('m_nFlags sin extension de signo', HW, 'static_cast<uint32>(static_cast<int32>(flags)); // movsx', 'static_cast<uint32>(static_cast<uint16>(flags)); // movsx', False),
    ('Play con flags de 7 bits', HW, 'static_cast<int8>(flags), speed)', 'static_cast<int8>(flags & 0x7F), speed)', False),
    ('posicion negativa sin poner a cero', HW, 'playPosition < 0 ? 0 : playPosition', 'playPosition', False),
    ('recorte sin signo', HW, 'static_cast<int32>(static_cast<int16>(position)) > length', 'static_cast<uint32>(static_cast<uint16>(position)) > static_cast<uint32>(length)', False),
    ('bucle de SetAudioBuffer como uint16 (equivalente: mismos 16 bits)', HW, 'static_cast<int16>(loopOffset), sampleRate', 'static_cast<uint16>(loopOffset), sampleRate', True),
    ('floor hacia cero', X87, 'if (negative && (mantissa & ((1ull << shift) - 1ull)) != 0) {', 'if (false) {', False),
    ('menor que 1 negativo da 0', X87, '(negative && (bits << 1) != 0) ? -1ll : 0ll', '0ll', False),
    ('constante 0.01 en doble', X87, 'double(0.01f)', '0.01', False),
    ('producto en float', X87, 'float(double(percent) * double(0.01f) * double(length))', 'float(percent) * 0.01f * float(length)', False),
]
bad = 0
for i, (name, rel, before, after, equivalent) in enumerate(MUTATIONS):
    if only and str(i) not in only:
        continue
    tmp = Path(tempfile.mkdtemp(prefix='g26a_mut_'))
    shutil.copytree(overlay, tmp / 'ov')
    p = tmp / 'ov' / rel
    text = p.read_bytes().decode('utf-8')
    if text.count(before) != 1:
        print('%2d NO APLICABLE (%d apariciones): %s' % (i, text.count(before), name), flush=True); bad += 1; shutil.rmtree(tmp, ignore_errors=True); continue
    p.write_bytes(text.replace(before, after).encode('utf-8'))
    out = tmp / 'obj'; out.mkdir()
    ov = '/I"%s" /I"%s"' % (tmp / 'ov', tmp / 'ov' / 'game_sa')
    lines = ['@echo off', 'call "%s" >nul 2>&1' % VCVARS, 'cd /d "%s"' % here]
    if rel == HW:
        subprocess.run([sys.executable, str(here / 'prepare.py'), str(root), str(tmp / 'ov'), str(out)], check=True, capture_output=True)
        lines.append('cl %s @proj.rsp /Fo"%s\\Hw_h.obj" "%s\\Hw_h.cpp" > "%s\\hw.log" 2>&1 || exit /b 1' % (ov, out, out, out))
        objs = '"%s\\hmain.obj" "%s\\Hw_h.obj" "%s\\X87Math.obj"' % (here / 'obj', out, here / 'obj')
    else:
        lines.append('cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MTd /DNOMINMAX %s /I"%s\\source\\game_sa" "%s" /Fo"%s\\X87Math.obj" /Fd"%s\\x.pdb" > "%s\\x87.log" 2>&1 || exit /b 1'
                     % (ov, root, tmp / 'ov' / X87, out, out, out))
        objs = '"%s\\hmain.obj" "%s\\Hw_h.obj" "%s\\X87Math.obj"' % (here / 'obj', here / 'obj', out)
    lines.append('link /nologo /OUT:"%s\\h.exe" %s /OPT:REF /FORCE:UNRESOLVED /DEBUG /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO user32.lib kernel32.lib > "%s\\link.log" 2>&1' % (out, objs, out))
    lines.append('if not exist "%s\\h.exe" exit /b 1' % out)
    lines.append('"%s\\h.exe" "%s" 2048' % (out, exe))
    bat = tmp / 'run.bat'; bat.write_text('\r\n'.join(lines) + '\r\n', encoding='ascii')
    res = subprocess.run(['cmd.exe', '/c', str(bat)], capture_output=True, text=True, cwd=here)
    m = re.search(r'TOTAL (\d+) pares, (\d+) diferencias', res.stdout)
    n = int(m.group(2)) if m else -1
    ok = (n != 0) != equivalent and n >= 0
    print('%2d %-6s %-92s diferencias=%d' % (i, 'OK' if ok else 'FALLO', name, n), flush=True); bad += not ok
    shutil.rmtree(tmp, ignore_errors=True)
print('mutaciones sin el resultado esperado:', bad, flush=True)
sys.exit(bad != 0)
