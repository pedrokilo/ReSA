"""Move the two pending C++ bodies into their independently compiled unit."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
path = ROOT / 'source/game_sa/Interior/Interior_c.cpp'
text = path.read_text(encoding='utf-8')
start = text.index('// 0x599A30\n// Integer layout')
end = text.index('// 0x599AF0', start)
office = text[start:end]
text = text[:start] + text[end:]
start = text.index('// 0x59A130\nvoid Interior_c::Shop_FurnishCeiling()')
end = text.index('// 0x59A140', start)
ceiling = text[start:end]
text = text[:start] + text[end:]
path.write_text(text, encoding='utf-8')
(path.parent / 'InteriorCpp.cpp').write_text(
    '#include "StdInc.h"\n#include "Interior_c.h"\n\n' + office + ceiling,
    encoding='utf-8')
path = ROOT / 'reversing/CppInteriorProposal/build_probe.py'
text = path.read_text(encoding='utf-8')
text = text.replace("'Interior/Interior_c.cpp',", "'Interior/Interior_c.cpp','Interior/InteriorCpp.cpp',")
text = text.replace("['source/InjectHooksMain.cpp'", "['source/CMakeLists.txt','source/InjectHooksMain.cpp'")
path.write_text(text, encoding='utf-8')
