import re,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_case import main
here=Path(__file__).parent
source=(here/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED={f'{m[0]}::{m[1]}':int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}',source)}
if __name__=='__main__':sys.exit(main(here,'interesting-scan355','InterestingScan355',EXPECTED,timeout=1500))
