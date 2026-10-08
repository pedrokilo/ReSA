from pathlib import Path
import re
p=Path('source/game_sa/Entity/Vehicle/Bike.cpp');s=p.read_text();a=s.index('static void BikeMoveHandBone355');b=s.index('// 0x6BF230',a);chunk=s[a:b];chunk=re.sub(r'\bbx\b','backX',chunk);chunk=re.sub(r'\bdx\b','deltaX',chunk);chunk=re.sub(r'\bdy\b','deltaY',chunk);s=s[:a]+chunk+s[b:];p.write_text(s)
