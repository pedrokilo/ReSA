import pathlib

W = pathlib.Path(r'C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad/w20')
c = pathlib.Path('source/game_sa/X87Math.cpp')
h = pathlib.Path('source/game_sa/X87Math.h')
ct = c.read_bytes().decode()
ht = h.read_bytes().decode()
nlc = '\r\n' if '\r\n' in ct else '\n'
nlh = '\r\n' if '\r\n' in ht else '\n'
ct = ct.replace('\r\n', '\n')
ht = ht.replace('\r\n', '\n')

order = ['matrix', 'pedik', 'vehaudio', 'jetpack', 'pathfind', 'swim', 'bmx', 'waterlevel', 'physical', 'radar', 'pad', 'memmgr']
ac = '\n// --- Lote de doce archivos (AEVehicleAudioEntity, TaskSimpleJetPack, PathFind, TaskSimpleSwim, Bmx, PedIK, WaterLevel, MemoryMgr, Physical, Radar, Pad, Matrix; 15/10/2026) ---\n'
ah = '\n// Lote de doce archivos (15/10/2026).\n'
for f in order:
    t = (W / f'{f}-x87.cpp.txt').read_text(encoding='utf-8').replace('\r\n', '\n')
    hh = (W / f'{f}-x87.h.txt').read_text(encoding='utf-8').replace('\r\n', '\n')
    if f == 'pedik':
        # RotateX/RotateY de PedIK duplican los de Matrix: se descartan (PedIK.cpp usa MatrixRotateXCpp/MatrixRotateYCpp)
        lines = t.split('\n')
        start = next(i for i, l in enumerate(lines) if l.startswith('// CPedIK::PitchForSlope'))
        t = '\n'.join(lines[start:])
        hl = hh.split('\n')
        hstart = next(i for i, l in enumerate(hl) if l.startswith('// CPedIK::PitchForSlope'))
        hh = '\n'.join(hl[hstart:])
    ac += '\n'.join(l for l in t.split('\n')).strip('\n') + '\n\n'
    ah += hh.strip('\n') + '\n'

# comprobacion de arranque del ayudante de PathFind
old = 'static bool CarEnterExitEntryEdxHelperIsTrivial();\nstatic bool ResidueHelpersAreTrivial() {'
assert old in ct
ct = ct.replace(old, 'static bool CarEnterExitEntryEdxHelperIsTrivial();\nstatic bool PathFindResidueFloatIsTrivial();\nstatic bool ResidueHelpersAreTrivial() {', 1)
old2 = '&& CarEnterExitEntryEdxHelperIsTrivial();'
assert old2 in ct
ct = ct.replace(old2, '&& CarEnterExitEntryEdxHelperIsTrivial() && PathFindResidueFloatIsTrivial();', 1)

c.write_bytes((ct.rstrip('\n') + '\n' + ac).replace('\n', nlc).encode())
h.write_bytes((ht.rstrip('\n') + '\n' + ah).replace('\n', nlh).encode())
print('ok')
