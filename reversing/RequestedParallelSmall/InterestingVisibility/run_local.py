import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
from probe_case import main
if __name__=='__main__':
    raise SystemExit(main(Path(__file__).parent,'interesting-visibility-pending','CInterestingEvents::InvalidateNonVisibleEvents',{'CInterestingEvents::InvalidateNonVisibleEvents':384}))
