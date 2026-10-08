"""Independent native descending-history oracle; preserves baseline, no source edits."""
import collections,hashlib,json,shutil,struct
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];RUN=HERE/'runs/update/20261008-151219-621228'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest'];domains=collections.Counter();offsets=collections.Counter();count=0
 assert report['passed'] is False and report['differences']==2560 and report['natural_exit'] is True and report['exit_code']==0
 assert all(report[k] is True for k in ('bank_restored','harness_restored','floating_environment_restored','saves_unchanged','configs_restored','layout_file_restored'))
 for line in (RUN/'comparisons.jsonl').open():
  row=json.loads(line)
  if row.get('event')!='comparison':continue
  seed=row['seed'];o=row['original'];c=row['candidate'];original=o['pad'][16:324];candidate=c['pad'][16:324]
  assert row['equal'] is False and original[:0x60]==candidate[:0x60] and original[0x74:]==candidate[0x74:]
  assert all(o[k]==c[k] for k in ('controllers','globals','scratch_physical','return_reference_exact','fp'))
  initial=[i*101+seed for i in range(10)]
  assert list(struct.unpack('<10h',bytes(original[0x60:0x74])))==[initial[0]]+initial[:9]
  assert list(struct.unpack('<10h',bytes(candidate[0x60:0x74])))==initial[1:]+[initial[0]]
  offsets.update(i for i,(a,b) in enumerate(zip(original,candidate)) if a!=b);domains[row['domain']]+=1;count+=1
 assert count==2560 and domains=={'normal':1920,'byte-counter-stress':640}
 archive=HERE/'baseline-archive/update'/manifest['asi'];archive.mkdir(parents=True,exist_ok=True)
 for p,h in manifest['tested_sources'].items():
  src=ROOT/p;assert sha(src)==h;dst=archive/'sources'/p;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
 for p in ('manifest.json','layout-proof.json','build.log'):
  src=HERE/'artifacts/update'/p
  if src.exists():shutil.copy2(src,archive/p)
 for p in ('enriched-summary.json','summary.json','harness.jsonl','log-archive.json'):
  shutil.copy2(RUN/p,archive/p)
 summary={'passed':False,'differences':count,'domains':dict(domains),'pad_difference_offsets':dict(offsets),'all_other_payloads_exact':True,'native_history_oracle_cases':count,'native_evidence_sha256':sha(HERE/'evidence/Update.exe.json'),'raw_sha256':sha(RUN/'comparisons.jsonl'),'compiled_manifest':manifest,'proposed_fix':'std::copy_backward(begin(SteeringLeftRightBuffer),end(SteeringLeftRightBuffer)-1,end(SteeringLeftRightBuffer)); preserves element0 and shifts elements0..8 into1..9, as native541D2D loop.','source_edited':False,'registry_written':False}
 (HERE/'update-baseline-audit.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps({'audited':count,'source_edited':False,'registry_written':False}))
if __name__=='__main__':main()
