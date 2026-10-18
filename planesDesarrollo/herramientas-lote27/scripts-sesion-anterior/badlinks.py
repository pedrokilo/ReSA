import re
from pathlib import Path

R = Path(r'D:\Original\gta-reversed-master').resolve()
pat = re.compile(r'(?<!\\)\]\(([^)]+)\)')
for page in [R / 'docs/ImplementacionesDelJuego.md', *(R / 'docs/implementaciones').glob('detalle-*.md')]:
    for link in pat.findall(page.read_text(encoding='utf-8')):
        path, _, frag = link.partition('#')
        t = (page.parent / path).resolve()
        if frag.startswith('L') and frag[1:].isdigit() and t.is_file():
            n = len(t.read_text(encoding='utf-8-sig').splitlines())
            if int(frag[1:]) > n:
                print(page.name, link[:140], n)
