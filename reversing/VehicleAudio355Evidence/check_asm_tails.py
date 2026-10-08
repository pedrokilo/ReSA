from pathlib import Path
import re
for p in Path('reversing/VehicleAudio355Evidence').glob('*Full.txt'):
 lines=p.read_text().splitlines();a=re.findall(r'^([0-9A-F]{8}):',p.read_text(),re.M);print(p.name,len(lines),a[0] if a else '',a[-1] if a else '')
