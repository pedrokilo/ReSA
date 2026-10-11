"""Strict independent admission gates. No approval before a complete runtime log."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={
    'CMemoryMgr::Malloc-SizeHint':2048,
    'CMemoryMgr::Realloc-MemorySizeHint':2048,
    'CMemoryMgr::MallocAlign-SizeAlignHint':2048,
    'CMemoryMgr::FreeAlign':2048,
}
GATES=('argument_after_matches','stack_restored','nonvolatile_preserved','route_ok','cand_body_contract',
       'helper_route_ok','fixture_contract','return_contract','alignment_ok',
       'backpointer_ok','resources_balanced','canaries_preserved','floating_state_restored')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1 or begin[0].get('registered_hook') is not True or begin[0].get('helper_routes_ok') is not True or begin[0].get('candidate_body_contract') is not True or begin[0].get('crt_entries_modified') is not False:
        raise RuntimeError('Missing registered routes or native CRT proof')
    helpers=begin[0]['helper_routes']
    if {h['entry'] for h in helpers}!={0x824257,0x824269,0x82413F} or len(helpers)!=3 or not all(h['route_ok'] and h['body']==h['entry'] for h in helpers):
        raise RuntimeError('Wrong CRT collaborators')
    proofs=begin[0].get('body_proofs',[])
    contracts=((0x72F420,14,5,0x824257),(0x72F440,19,10,0x824269),
               (0x72F4C0,35,12,0x824257),(0x72F4F0,16,11,0x82413F))
    if len(proofs)!=4:raise RuntimeError('Missing compiled argument-transfer proof')
    for proof,(entry,length,offset,crt) in zip(proofs,contracts):
        if (proof['entry'],proof['length'],proof['branch_opcode_offset'],proof['crt'])!=(entry,length,offset,crt) or proof.get('proven') is not True:
            raise RuntimeError('Invalid compiled body contract')
        original=bytes(proof['original']);candidate=bytes(proof['candidate']);bridge=bytes(proof['bridge_bytes'])
        if len(original)!=length or len(candidate)!=length or len(bridge)!=6:raise RuntimeError('Truncated instruction proof')
        opcode=0xE9 if entry==0x72F4F0 else 0xE8
        if original[offset]!=opcode or candidate[offset]!=opcode:raise RuntimeError('Wrong branch opcode')
        if original[:offset+1]!=candidate[:offset+1] or original[offset+5:]!=candidate[offset+5:]:raise RuntimeError('Compiled instructions differ')
        if entry+offset+5+int.from_bytes(original[offset+1:offset+5],'little',signed=True)!=crt:raise RuntimeError('Original CRT branch mismatch')
        if proof['candidate_body']+offset+5+int.from_bytes(candidate[offset+1:offset+5],'little',signed=True)!=proof['branch']:raise RuntimeError('Candidate bridge branch mismatch')
        if bridge!=b'\x68'+crt.to_bytes(4,'little')+b'\xC3':raise RuntimeError('Bridge changes argument transfer')
        if not proof['candidate_module'] or proof['candidate_module']==0x400000 or proof['candidate_module']!=proof['bridge_module']:raise RuntimeError('Bridge/body module mismatch')
    cases={fn:set() for fn in EXPECTED};pairs=set();realloc_profiles=set();alignments=set();sizes=set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Invalid access')
        if row['event']!='comparison':continue
        fn=row['function'];case=row['case']
        if row.get('equal') is not True or row.get('accepted') is not True or row['original']!=row['candidate']:raise RuntimeError('Pair not explicitly accepted')
        if row['pair'] in pairs or case in cases[fn]:raise RuntimeError('Duplicate fixture or pair')
        pairs.add(row['pair']);cases[fn].add(case)
        for side in ('original','candidate'):
            r=row[side]
            if not isinstance(r,dict) or not all(r.get(k) is True for k in GATES):raise RuntimeError('NULL result or failed runtime gate')
            if r['allocations']!=r['frees']:raise RuntimeError('Unbalanced owned resource')
            size=r['payload_size'];old=r['old_size']
            if fn.endswith('Malloc-SizeHint'):
                if r['requested_size']!=size or len(r['defined_payload'])!=size:raise RuntimeError('Malloc defined extent mismatch')
                sizes.add(size)
            elif fn.endswith('Realloc-MemorySizeHint'):
                kept=min(old,size)
                if r['requested_size']!=size or r['prefix_preserved_bytes']!=kept or len(r['defined_payload'])!=kept:raise RuntimeError('Realloc prefix mismatch')
                if r['return_kind']!=('null_after_free' if old and size==0 else 'owned_block'):raise RuntimeError('Realloc NULL contract mismatch')
                realloc_profiles.add(('null' if old==0 else 'owned','zero' if size==0 else 'shrink' if old>size else 'same' if old==size else 'grow'))
            else:
                align=r['alignment'];alignments.add(align)
                if align<4 or align&(align-1) or r['requested_size']!=size+align:raise RuntimeError('Invalid alignment extent')
                if fn.endswith('MallocAlign-SizeAlignHint') and len(r['defined_payload'])!=size:raise RuntimeError('Aligned payload mismatch')
                if fn.endswith('FreeAlign') and r['return_kind']!='void':raise RuntimeError('Free return incorrectly interpreted')
    if pairs!=set(range(8192)) or any(v!=set(range(2048)) for v in cases.values()):raise RuntimeError('Incomplete coverage')
    if sizes!={0,1,2,3,4,7,8,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,2047,2048,2049,4096}:raise RuntimeError('Missing size boundaries')
    if alignments!={4,8,16,32,64,128,256}:raise RuntimeError('Missing valid alignments')
    if not {('null','zero'),('null','grow'),('owned','zero'),('owned','shrink'),('owned','same'),('owned','grow')}<=realloc_profiles:raise RuntimeError('Missing legal realloc branches')
    final=[r for r in rows if r['event']=='complete']
    if len(final)!=1 or final[0]['pairs']!=8192 or not all(final[0].get(k) is True for k in ('state_restored','helper_route_ok','floating_state_restored')):raise RuntimeError('Final restoration missing')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciesmemory9',manifest['binaries']['asi'],analyze,'DependenciesMemory9',timeout=1500))
