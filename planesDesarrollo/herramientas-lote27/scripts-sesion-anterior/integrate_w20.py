import pathlib

W = pathlib.Path(r'C:\Users\pedro\AppData\Local\Temp\claude\D--Original-gta-reversed-master\c66a4e30-afba-4db2-93b7-ac1baf6b4246\scratchpad\w20')
cpp_path = pathlib.Path('source/game_sa/X87Math.cpp')
h_path = pathlib.Path('source/game_sa/X87Math.h')
c = cpp_path.read_bytes().decode('utf-8')
nl = '\r\n' if '\r\n' in c else '\n'
c = c.replace('\r\n', '\n')
hh = h_path.read_bytes().decode('utf-8').replace('\r\n', '\n')


def rep(text, old, new):
    assert old in text, old[:70]
    return text.replace(old, new, 1)


# 1) intrínsecos nuevos (log para PostEffects, tan para Weapon)
c = rep(c, 'extern "C" double __cdecl pow(double, double);\n',
        'extern "C" double __cdecl pow(double, double);\nextern "C" double __cdecl log(double);\nextern "C" double __cdecl tan(double);\n')
c = rep(c, '#pragma intrinsic(sin, cos, sqrt, atan2, log10, pow)', '#pragma intrinsic(sin, cos, sqrt, atan2, log10, pow, log, tan)')

# 2) comprobaciones de carga de los dos ayudantes de residuo nuevos (se definen al final del archivo)
c = rep(c, 'static bool ResidueHelpersAreTrivial() {',
        'static bool PlayerPedResidueFloatIsTrivial();\nstatic bool CarEnterExitEntryEdxHelperIsTrivial();\nstatic bool ResidueHelpersAreTrivial() {')
c = rep(c, ' && EntryEaxHelperIsTrivial();', ' && EntryEaxHelperIsTrivial() && PlayerPedResidueFloatIsTrivial() && CarEnterExitEntryEdxHelperIsTrivial();')

# 3) ayudantes
order = ['playerped', 'carenterexit', 'pickup', 'entryexit', 'pedgroupplacer', 'weather', 'quadbike', 'pickups', 'rpanimblend',
         'posteffects', 'gangwars', 'hud', 'timer', 'train', 'projinfo', 'weapon', 'knockoff', 'pedgeom', 'object', 'weatheraudio']
add_c = '\n// --- Lote de veinte archivos de nivel 2 (14/10/2026) ---\n'
add_h = '\n// Lote de veinte archivos de nivel 2 (14/10/2026).\n'
used = []
for f in order:
    t = (W / f'{f}-x87.cpp.txt').read_text(encoding='utf-8').replace('\r\n', '\n')
    if '__cdecl' not in t and '__fastcall' not in t:
        continue
    lines = t.split('\n')
    if f == 'posteffects':
        start = next(i for i, l in enumerate(lines) if l.startswith('// --- Lote de PostEffects'))
        lines = lines[start:]
    if f == 'weapon':
        blank = next(i for i, l in enumerate(lines) if l.strip() == '')
        lines = lines[blank + 1:]
    lines = [l for l in lines if not l.startswith('extern "C"')]
    add_c += '\n'.join(lines).strip('\n') + '\n\n'
    add_h += (W / f'{f}-x87.h.txt').read_text(encoding='utf-8').replace('\r\n', '\n').strip('\n') + '\n'
    used.append(f)
if not c.endswith('\n'):
    c += '\n'
if not hh.endswith('\n'):
    hh += '\n'
cpp_path.write_bytes((c + add_c).replace('\n', nl).encode('utf-8'))
h_path.write_bytes((hh + add_h).replace('\n', nl).encode('utf-8'))
print('ayudantes fusionados de:', used)
