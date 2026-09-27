"""Compare from the front-end: skip videos temporarily, never load saves."""
import configparser, json, shutil, sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
import runtime_runner as runner
shared_configuration=runner.probe_configuration
def menu_configuration(data):
    return shared_configuration(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(path):
    if not runner.terminal_event(path): return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') for r in rows): raise RuntimeError('Fallo en el motor')
    pairs=[r for r in rows if r['event']=='comparison']
    assert len(pairs)==24 and {(r['case'],r['route']) for r in pairs}=={(i,j) for i in range(8) for j in range(3)}
    assert all(r['equal'] and r['original']==r['candidate']==1201 and r['candidate_calls']==1 and r['original_candidate_calls']==0 and all(r[k] for k in ('inputs_unchanged','original_bytes','original_tables','candidate_tables')) for r in pairs)
    assert rows[-1]['event']=='complete' and rows[-1]['state_restored'] and rows[-1]['other_hooks_unchanged']
    return dict(pairs=24,all_equal=True,hook_state=rows[0],state_restored=True,log_sha256=runner.digest(path),scope='Frontend motor; direct entry and both vtables; no gameplay or save loading')
if __name__=='__main__':
    manifest=json.loads((HERE/'artifacts/probe/manifest.json').read_text())
    for relative in ('gta-reversed.ini','scripts/gta-reversed.ini'):
        cfg=configparser.ConfigParser();cfg.read(runner.GAME/relative)
        assert not cfg.getboolean('FastLoader','Enable') and cfg.getint('FastLoader','SaveGameToLoad')==-2
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/probe/gta_reversed-gang-leader.{ext}'
        assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-gang-leader-probe.{ext}'
        if target.exists() and runner.digest(target)!=manifest[ext]:
            archive=target.with_name(target.name+'.'+runner.digest(target))
            if not archive.exists(): shutil.copy2(target,archive)
            assert runner.digest(archive)==runner.digest(target)
        shutil.copy2(source,target)
    runner.probe_configuration=menu_configuration
    status=runner.run_case('gang-leader',manifest['asi'],analyze,'GangLeader::GetTaskType',timeout=180,stop_before_analysis=True,unhook='')
    if status==0:
        path=max((runner.GAME/'resultados-gang-leader').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns)
        summary=json.loads(path.read_text())
        summary.update(run_directory=str(path.parent),manifest=manifest,original_exe_sha256=runner.EXE_HASH,runner_sha256=runner.digest(Path(__file__)),temporary_configuration='FastLoader=true; SaveGameToLoad=-2; no hook exclusions',original_address='0x0065DF20',original_bytes='b8 b1 04 00 00 c3',vtable_address='0x0086F8FC',vtable_index=4)
        (HERE/'validation-summary.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
    sys.exit(status)
