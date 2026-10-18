import re, pathlib
pend = open('CppTwelveLevel6Final/pendientes_regresion.txt').read().split()
watch = ['World.cpp', 'MenuManager.cpp', 'platform.cpp', 'app_game.cpp', 'TaskSimpleCarSetPedSlowDraggedOut.cpp']
extra = set()
for bp in pathlib.Path('.').glob('*/build_probe.py'):
    try:
        t = bp.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    if any(w in t for w in watch):
        extra.add(bp.parent.name)
for b in ['WldD', 'CTasks6b', 'MenuMgrA', 'MenuMgrB', 'MenuMgrC', 'MenuMgrD', 'Rs27', 'GameIdle', 'HookUnlockGame',
          'CppFormationNumeric', 'DependenciesFormationMemory8', 'PowEquivalence']:
    if pathlib.Path(b).is_dir():
        extra.add(b)
allq = list(dict.fromkeys(pend + sorted(extra)))
ok = [b for b in allq if pathlib.Path(b, 'run_local.py').exists() and pathlib.Path(b, 'build_probe.py').exists()]
skip = [b for b in allq if b not in ok]
open('CppTwelveLevel6Final/cola_final.txt', 'w').write(' '.join(ok))
print(len(pend), len(extra), len(allq), len(ok), 'omitidos', skip)
