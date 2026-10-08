"""Independent original-only oracle; complete-line partial logs are diagnostic only."""
import argparse,collections,gzip,hashlib,json,struct
from pathlib import Path
HERE=Path(__file__).resolve().parent
DURATIONS=[(0,100),(100,100),(200,100),(-1,100),(100,-1),(-32768,32767),(32767,-32768)]
def expected(row):
 seed=row['seed'];pad=bytearray((seed*73+i*29+(i>>3))&255 for i in range(308))
 for i in range(0x118,0x120):pad[i]&=1
 old,duration=DURATIONS[(seed//3200)%7];struct.pack_into('<h',pad,0x10C,old);struct.pack_into('<I',pad,0x12C,row['deadline']);pad[0x130]=row['stored_frequency']
 freq=row['frequency'];clock=row['clock'];gates=row['gates']
 if gates&1 and not gates&2:
  if not freq:struct.pack_into('<h',pad,0x10C,0);pad[0x110]=0
  elif clock>=row['deadline'] or freq>row['stored_frequency']:
   if duration>old:struct.pack_into('<h',pad,0x10C,duration);pad[0x110]=freq
   struct.pack_into('<I',pad,0x12C,(clock+row['delay'])&0xFFFFFFFF);pad[0x130]=freq
 return list(pad)
def main():
 p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--output',type=Path,required=True);args=p.parse_args();count=0;domains=collections.Counter();offsets=collections.Counter();other=collections.Counter();samples=[];terminal=False;seen=set()
 opener=gzip.open if args.log.suffix=='.gz' else open
 with opener(args.log,'rt',encoding='utf8') as stream:
  for line in stream:
   try:row=json.loads(line)
   except json.JSONDecodeError:break
   if row.get('event')=='complete':terminal=True
   if row.get('event')!='comparison':continue
   seed=row['seed'];assert seed not in seen;seen.add(seed);o=row['original']['pad'][16:324];c=row['candidate']['pad'][16:324];assert o==expected(row),('native oracle mismatch',seed)
   diff=[i for i,(a,b) in enumerate(zip(o,c)) if a!=b];contractkeys=('pad','controllers','globals','return_reference_exact','fp','scratch_physical');changed=[k for k in contractkeys if row['original'][k]!=row['candidate'][k]];assert bool(changed)==(not row['equal']);other.update(k for k in changed if k!='pad')
   if changed:
    domains[row['domain']]+=1;offsets.update(diff)
    if len(samples)<12:samples.append({'seed':seed,'offsets':diff,'duration':row['duration'],'frequency':row['frequency'],'stored_frequency':row['stored_frequency'],'clock':row['clock'],'deadline':row['deadline'],'delay':row['delay'],'gates':row['gates']})
   count+=1
 result={'diagnostic_only':True,'runtime_approved':False,'native_oracle_cases':count,'complete_event':terminal,'complete_corpus':seen==set(range(89600)),'differences':sum(domains.values()),'domains':dict(domains),'difference_offsets':dict(offsets),'other_payload_differences':dict(other),'samples':samples,'native_evidence_sha256':hashlib.sha256((HERE/'evidence/StartShake.exe.json').read_bytes()).hexdigest(),'proposed_source_changes':['NoShakeBeforeThis float -> uint32','NoShakeFreq char -> uint8','if(ShakeDur) -> if(time > ShakeDur)','remove float cast from uint32 clock+delay assignment'],'source_edited':False}
 args.output.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:result[k] for k in ('native_oracle_cases','complete_event','differences','runtime_approved')}))
if __name__=='__main__':main()
