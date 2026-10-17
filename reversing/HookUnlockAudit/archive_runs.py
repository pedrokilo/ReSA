"""Conserva evidencia exacta comprimida y verifica el resultado independiente del runner."""
import csv
import gzip
import hashlib
import json
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE))
from retired import EXPECTED_REVIEWS,RETIRED
RETAINED={('CAEVehicleAudioEntity','ProcessPlayerVehicleEngine'),('CAEVehicleAudioEntity','ProcessVehicle'),
          ('CAEVehicleAudioEntity','Service'),('Game','Render2dStuff'),('Game','Idle'),('Game','FrontendIdle')}
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
results=[]
for argument in sys.argv[1:]:
    path=Path(argument);summary=json.loads((path/'summary.json').read_text())
    payload=(path/'comparisons.jsonl').read_bytes()
    assert hashlib.sha256(payload).hexdigest()==summary['log_sha256']
    events=[json.loads(line) for line in payload.splitlines()]
    assert all(summary.get(key) is True for key in ('passed','all_equal','state_restored','saves_unchanged','configs_restored','posteffects_passed'))
    pairs=[e for e in events if e['event']=='comparison']
    assert len(pairs)==2048 and {e['pair'] for e in pairs}==set(range(2048))
    assert all(e['equal'] and e['accepted'] and e['original'] is not None and e['candidate'] is not None and e['original']==e['candidate'] for e in pairs)
    reviews=[e for e in events if e['event']=='catalog_lock_review']
    assert len(reviews)==EXPECTED_REVIEWS and len({(e['class'],e['fn_name']) for e in reviews})==EXPECTED_REVIEWS
    assert not any((e['class'],e['fn_name']) in RETIRED for e in reviews)
    frame=[e for e in events if e['event']=='live_frame']
    assert len(frame)==240 and all(e['route_ok'] and e['result']['globals_restored'] for e in frame)
    post=[e for e in events if e['event']=='posteffects_live_checks']+[e['checks'] for e in events if e['event']=='posteffects_live_frame']
    samples=[sample for e in post for sample in e['comparisons']]
    assert len(post)==241 and len(samples)==1446
    assert all(e['passed'] and e['state_restored'] for e in post)
    assert all(e['passed'] and e['route_ok'] and e['original'] is not None and e['candidate'] is not None and e['original']['states']==e['candidate']['states'] and e['candidate']['esp_delta']==0 for e in samples)
    assert not any(e['event'].startswith('first_chance_av') or e['event'] in ('error','assertion') for e in events)
    complete=[e for e in events if e['event']=='complete'];assert len(complete)==1 and complete[0]['state_restored']
    destination=HERE/'runs'/path.name;destination.mkdir(parents=True,exist_ok=True)
    (destination/'comparisons.jsonl.gz').write_bytes(gzip.compress(payload,mtime=0))
    assert gzip.decompress((destination/'comparisons.jsonl.gz').read_bytes())==payload
    (destination/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
    (destination/'RESULTADO.txt').write_text((path/'RESULTADO.txt').read_text(),encoding='utf-8',newline='\n')
    runtime=[e for e in events if e['event']=='runtime_hook']
    with (destination/'runtime-locks.csv').open('w',encoding='utf-8',newline='') as stream:
        writer=csv.DictWriter(stream,fieldnames=['class','fn_name','locked','hooked','reversed','type'],lineterminator='\n');writer.writeheader()
        writer.writerows({k:int(e[k]) if isinstance(e[k],bool) else e[k] for k in writer.fieldnames} for e in runtime)
    results.append(dict(run=path.name,pairs=2048,differences=0,live_frames=240,posteffects_samples=1446,
        reviewed_locks=EXPECTED_REVIEWS,retired_locks=len(RETIRED),runtime_locked=sum(e['locked'] for e in runtime),log_sha256=summary['log_sha256'],
        state_restored=True,saves_unchanged=True,configs_restored=True,
        probe_sha256=summary['executed_probe_sha256']))
last_path=Path(sys.argv[-1]);last_events=[json.loads(line) for line in (last_path/'comparisons.jsonl').read_bytes().splitlines()]
actual={(e['class'],e['fn_name']) for e in last_events if e['event']=='runtime_hook' and e['locked']}
assert actual==RETAINED
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
assert {(r['class'],r['fn_name']) for r in rows if r['locked']=='1'}==RETAINED
assert results[-1]['probe_sha256']==manifest['binaries']['asi']
for relative,expected in manifest['sources'].items():
    assert hashlib.sha256((ROOT/relative).read_bytes()).hexdigest()==expected,relative
(HERE/'build-manifest-final.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8',newline='\n')
report=dict(passed=True,real_unlocks=['CEntryExit::GetPositionRelativeToOutsideWorld','CPostEffects::ImmediateModeRenderStatesStore'],
    stale_catalog_flags_corrected=413,remaining_locked=['::'.join(k) for k in sorted(RETAINED)],runs=results,
    final_source_hashes=manifest['sources'],scope='Contrato de interiores, diez estados del driver RW vivo y 240 fotogramas por corrida. No certifica concurrencia de audio, todos los interiores, menús/foreground ni todos los drivers.')
(HERE/'unlock-summary.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(report,ensure_ascii=False,indent=2))
