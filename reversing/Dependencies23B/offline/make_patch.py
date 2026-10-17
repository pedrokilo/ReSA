import re, subprocess, shutil, os, sys
REPO = 'D:/Original/gta-reversed-master/'
rel = 'source/game_sa/Entity/Vehicle/Vehicle.cpp'
cur = open(REPO + rel, 'rb').read().decode()
crlf = '\r\n' in cur
txt = cur.replace('\r\n', '\n')
# ---- cuerpo nuevo
parts = ['parts/p0_header.inc', 'parts/p1_common.inc', 'parts/p2_route0.inc', 'parts/p3_wing.inc', 'parts/p4_rotor.inc', 'parts/p9_entry.inc']
body = ''.join(open(p, 'rb').read().decode().replace('\r\n', '\n') for p in parts)
body = body.replace('uint8(v.m_nTypeStatus >> 3)', 'uint8(v.GetStatus())')
body = body.replace('}  // namespace\n', '}  // namespace\n#undef M32\n\nstatic_assert(offsetof(CVehicle, m_pFlyingHandlingData) == 0x388 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nCreatedBy) == 0x4A4 &&\n              offsetof(CVehicle, m_vecMoveSpeed) == 0x44 && offsetof(CVehicle, m_vecTurnSpeed) == 0x50 && offsetof(CVehicle, m_fMass) == 0x8C && offsetof(CVehicle, m_fTurnMass) == 0x90 &&\n              offsetof(CVehicle, m_vecCentreOfMass) == 0xA4 && offsetof(CVehicle, m_matrix) == 0x14,\n              "FlyingControl lee estos desplazamientos del EXE");\n', 1)
# ---- sustituir el cuerpo antiguo
a = txt.index('// 0x6D85F0\nvoid CVehicle::FlyingControl(')
b = txt.index('// 0x6DAF00\n', a)
new = txt[:a] + body.rstrip('\n') + '\n\n' + txt[b:]
new = new.replace('    // RH_ScopedInstall(FlyingControl, 0x6D85F0);', '    RH_ScopedInstall(FlyingControl, 0x6D85F0);')
assert 'RH_ScopedInstall(FlyingControl, 0x6D85F0);' in new and '    // RH_ScopedInstall(FlyingControl' not in new
if crlf:
    new = new.replace('\n', '\r\n')
os.makedirs('pw/a/' + os.path.dirname(rel), exist_ok=True)
os.makedirs('pw/b/' + os.path.dirname(rel), exist_ok=True)
open('pw/a/' + rel, 'wb').write(cur.encode())
open('pw/b/' + rel, 'wb').write(new.encode())
print('crlf', crlf, 'bytes', len(cur), len(new))
