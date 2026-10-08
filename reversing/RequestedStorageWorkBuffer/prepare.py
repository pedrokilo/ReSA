import json,shutil
from pathlib import Path
HERE=Path(__file__).resolve().parent;OLD=HERE.with_name('RequestedPoolsSixGettersV2')
for name in ('PreflightHarness.hpp','VTableRanges.hpp','build_probe.py','restore_winmain.py'):
    s=(OLD/name).read_text().replace('RequestedPreflightPoolsSixGettersLive','RequestedPreflightStorageWorkBufferLive').replace('RequestedPoolsSixGettersV2Live','RequestedStorageWorkBufferLive').replace('RequestedPoolsSixGettersV2','RequestedStorageWorkBuffer').replace('requested-pools-six-getters-v2','requested-storage-work-buffer').replace('preflight-pools-six-getters-v2','preflight-storage-work-buffer').replace('gta_reversed-pools-six-getters-v2','gta_reversed-storage-work-buffer')
    if name=='build_probe.py':
        a=s.index('TESTED=');b=s.index('\n',a);s=s[:a]+"TESTED=['source/game_sa/GenericGameStorage.cpp','source/game_sa/GenericGameStorage.h']"+s[b:]
    (HERE/name).write_text(s)
(HERE/'Diagnostics').mkdir(exist_ok=True)
(HERE/'corpus-plan.json').write_text(json.dumps(dict(functions={'SaveDataToWorkBuffer-org':'0x005D1270','LoadDataFromWorkBuffer-org':'0x005D1300'},pairs_each=1024,sizes=[-1,0,1,2,3,4,7,16],positions=[0,1,2,3,16,51168,51176,51184],prior_failed=[False,True],patterns=8,buffer_capacity=51200,domain='Nonoverlapping caller/private work buffer; in-buffer copies only; no refill/flush, file I/O or variable-capacity credit. Exact rawAL bool return; upperEAX unspecified.',full_payload=['guarded51200byteWorkBuffer','guarded256byteCaller','globalsC16EE8..C16EFFF','globals8D2BD0..8D2BEF'],harness_ranges=17692),indent=2))
