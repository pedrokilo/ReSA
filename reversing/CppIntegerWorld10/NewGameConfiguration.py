"""Local isolated FastLoader fixture; never edits runtime_runner.py or saves."""
import hashlib,re

def run_new_game(runner,prefix,asi,analyze,title,timeout):
 targets=[runner.GAME/'gta-reversed.ini',runner.GAME/'scripts/gta-reversed.ini']
 snapshots={p:p.read_bytes()for p in targets}
 digests={p:hashlib.sha256(b).hexdigest()for p,b in snapshots.items()}
 original_configuration=runner.probe_configuration
 allowed_inputs={hashlib.sha256(b).hexdigest()for b in snapshots.values()}
 calls=0
 def configuration(data):
  nonlocal calls
  if hashlib.sha256(data).hexdigest()not in allowed_inputs:raise RuntimeError('Configuration backup differs from pre-run snapshot')
  result=original_configuration(data)
  result,n=re.subn(rb'(?m)^SaveGameToLoad\s*=\s*2\s*$',b'SaveGameToLoad = 0',result)
  if n!=1:raise RuntimeError('Expected exactly one isolated FastLoader save slot')
  calls+=1
  return result
 # No INI is written here: run_case first takes its normal backup, then asks
 # this callback for the temporary bytes and performs its own guarded write.
 runner.probe_configuration=configuration
 code=None
 try:
  code=runner.run_case(prefix,asi,analyze,title,timeout=timeout)
 finally:
  runner.probe_configuration=original_configuration
  restoration_needed=[]
  for target,data in snapshots.items():
   if not target.is_file()or hashlib.sha256(target.read_bytes()).hexdigest()!=digests[target]:
    restoration_needed.append(str(target));target.write_bytes(data)
  if any(not p.is_file()or hashlib.sha256(p.read_bytes()).hexdigest()!=h for p,h in digests.items()):raise RuntimeError('Outer INI restoration failed')
  if restoration_needed:raise RuntimeError('Runner did not restore INI; outer guard restored '+', '.join(restoration_needed))
 if calls!=2:raise RuntimeError('Both isolated INI configurations were not generated')
 return code
