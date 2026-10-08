import importlib.util,json,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('restoration_under_test',ROOT/'restore_winmain.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
class FakePath:
    def __init__(self,errors,mismatch):self.errors=list(errors);self.mismatch=mismatch;self.writes=0;self.data=b'instrumented'
    def __str__(self):return 'fake-only-no-real-WinMain'
    def write_bytes(self,data):
        self.writes+=1
        if self.errors:raise OSError(self.errors.pop(0),'injected')
        self.data=b'wrong' if self.mismatch else data
    def read_bytes(self):return self.data
cases=[]
for name,errors,mismatch,count,error_type in [('three22_success',[22]*3,False,4,None),('four22_raises',[22]*4,False,4,OSError),('accessdenied_raises',[13],False,1,OSError),('byte_mismatch_raises',[],True,1,RuntimeError),('first_success',[],False,1,None)]:
    fake=FakePath(errors,mismatch);attempts=[];delays=[];raised=None
    try:module.restore_saved(fake,b'original',attempts,delays.append)
    except Exception as error:raised=error
    assert (raised is None if error_type is None else isinstance(raised,error_type))
    assert fake.writes==count and delays==[.25]*sum('retry_delay_seconds' in a for a in attempts)
    cases.append(dict(case=name,passed=True,writes=fake.writes,delays=delays,attempts=attempts,error_propagated=type(raised).__name__ if raised else None))
report=dict(cases=cases,helper_sha256=hashlib.sha256((ROOT/'restore_winmain.py').read_bytes()).hexdigest(),test_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),no_real_file_modified=True)
(ROOT/'Diagnostics/restoration-injected-tests.json').write_text(json.dumps(report,indent=2))
print('5 injected restoration cases passed')
