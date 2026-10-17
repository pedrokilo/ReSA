import subprocess,sys,os,re,json,time,shutil
from pathlib import Path
ROOT=Path(r'D:\Original\gta-reversed-master')
PY=str(ROOT/r'reversing\CVehicle\tooling\Scripts\python.exe')
if not os.path.exists(PY): PY=sys.executable
env=dict(os.environ,CMAKE_BUILD_PARALLEL_LEVEL='2',_CL_='-MP2')
OUT=ROOT/'reversing'/sys.argv[1]
OUT.mkdir(parents=True,exist_ok=True)
WRAP='''
import sys,runpy
from pathlib import Path
sys.path.insert(0, r"D:\Original\gta-reversed-master\reversing")
import runtime_runner
runtime_runner.GAME = Path("E:/JUEGOS/GTA San Andreas - CVehicle Test")
sys.argv = [sys.argv[1]]
runpy.run_path(sys.argv[0], run_name="__main__")
'''
def run(cmd,**k): return subprocess.run(cmd,cwd=str(ROOT),env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',**k)
res={}
def _modified(bank):
    return set(l[3:].strip().replace('"','') for l in subprocess.run(['git','status','--porcelain','--',f'reversing/{bank}'],cwd=str(ROOT),capture_output=True,text=True,encoding='utf-8',errors='replace').stdout.splitlines() if l[:2].strip()=='M')
PRE={b:_modified(b) for b in sys.argv[2:]}
for bank in sys.argv[2:]:
    r={'bank':bank}; t0=time.time()
    for script in ('build_probe.py','Diagnostics/verify_compiled_calls.py','run_local.py'):
        sp=str(ROOT/'reversing'/bank/script)
        # Paso opcional: algunos bancos (RequestedPadProcessPad) regeneran su prueba estatica del PE recien compilado entre la compilacion y la ejecucion;
        # run_local.py exige que compiled-runtime-proof.json coincida con el hash del ASI que acaba de producir build_probe.py.
        if script.startswith('Diagnostics/') and not os.path.exists(sp): continue
        p=run([PY,'-B',sp]) if script.startswith('Diagnostics/') else run([PY,'-B',os.environ['TEMP']+'/hist_wrap.py',sp])  # el verificador estatico no necesita hist_wrap (solo lee)
        r[Path(script).stem]=p.returncode
        if script.startswith('Diagnostics/'):
            if p.returncode: r['err']=(p.stdout+p.stderr)[-500:]; break
            continue
        if script=='build_probe.py' and p.returncode==0:
            # algunos bancos antiguos no instalan su sonda en _baseline: copiarla desde artifacts si falta
            for ext in ('asi','pdb'):
                for src in (ROOT/'reversing'/bank/'artifacts').glob('gta_reversed-*-probe.'+ext):
                    dst=Path(r'E:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline')/src.name
                    if not dst.exists() or dst.stat().st_mtime<src.stat().st_mtime: shutil.copy2(src,dst)
        out=p.stdout+p.stderr
        if script=='build_probe.py':
            if p.returncode: r['err']=out[-500:]; break
        else:
            r['head']=out.strip().splitlines()[0] if out.strip() else ''
            m=re.search(r'Informe: (.+)',out)
            if m:
                folder=Path(m.group(1).strip()); r['folder']=str(folder)
                dst=OUT/'runs'/f'{bank}-{folder.name}'; dst.mkdir(parents=True,exist_ok=True)
                for n in ('summary.json','RESULTADO.txt'):
                    if (folder/n).exists(): shutil.copy2(folder/n,dst/n)
                import gzip
                if (folder/'comparisons.jsonl').exists():
                    with open(folder/'comparisons.jsonl','rb') as f, gzip.GzipFile(filename='',mode='wb',fileobj=open(dst/'comparisons.jsonl.gz','wb'),mtime=0) as z: z.write(f.read())
                try:
                    s=json.load(open(folder/'summary.json',encoding='utf-8'))
                    for k in ('passed','all_equal','pairs','state_restored','saves_unchanged','configs_restored'): r[k]=s.get(k)
                except Exception as e: r['summary_err']=str(e)
            else: r['err']=out[-500:]
    # restaurar archivos de evidencia histórica que el banco reescribe
    after=set(l[3:].strip().replace('"','') for l in subprocess.run(['git','status','--porcelain','--',f'reversing/{bank}'],cwd=str(ROOT),capture_output=True,text=True,encoding='utf-8',errors='replace').stdout.splitlines() if l[:2].strip()=='M')
    for path in sorted(after-PRE.get(bank,set())):
        subprocess.run(['git','checkout','--',path],cwd=str(ROOT),capture_output=True)
    # limpiar lo regenerable (sonda .pdb de ~1,7 GB, informe crudo del clon) solo si la evidencia ya se copió al repo
    try:
        sys.path.insert(0,str(Path(__file__).resolve().parent))
        from limpiar_regenerables import limpiar_banco
        copiado=any((OUT/'runs').glob(f'{bank}-*/summary.json')) if (OUT/'runs').exists() else False
        r['limpieza_gb']=limpiar_banco(bank,r.get('folder') if copiado else None)
    except Exception as e: r['limpieza_err']=str(e)
    r['secs']=round(time.time()-t0); res[bank]=r; print(json.dumps(r),flush=True)
