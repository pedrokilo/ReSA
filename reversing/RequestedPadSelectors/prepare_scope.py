"""Read-only extraction of exact CPad identities and fresh REA full bodies."""
import csv, hashlib, json
from pathlib import Path
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
CAMPAIGN = ROOT/'reversing/RequestedValidation20261008'
RAW = CAMPAIGN/'artifacts/rea-campaign/2026-10-08T17-00-41-571Z'
EXCLUDED = set('Clear ClearKeyBoardHistory ClearMouseHistory Initialise ProcessPCSpecificStuff ProcessPad ReconcileTwoControllersInput SetTouched StartShake StartShake_Distance StartShake_Train StopShaking Update UpdateMouse'.split())
CONTEXT = set('GetPedWalkLeftRight-ped GetPedWalkUpDown-ped AimWeaponLeftRight AimWeaponUpDown LookAroundLeftRight LookAroundUpDown GetDisplayVitalStats WeaponJustDown GetWeapon'.split())
def digest(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    rows = [r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')) if r['class']=='CPad' and r['fn_name'] not in EXCLUDED]
    assert len(rows)==61 and all(r['validated']=='0' for r in rows)
    results = {r['function']:r for r in json.loads((CAMPAIGN/'rea-campaign-summary.json').read_text())['results'] if r['class']=='CPad'}
    evidence = HERE/'evidence'; evidence.mkdir(exist_ok=True)
    output=[]
    for row in rows:
        result=results[row['fn_name']]
        info={**row,'phase':'context' if row['fn_name'] in CONTEXT else 'pure','rea':result}
        if result['ok']:
            raw=RAW/result['path']; info['raw_sha256']=digest(raw)
            payload=json.loads(raw.read_text(encoding='utf-8'))
            structured=payload.get('structuredContent') or json.loads(payload['content'][0]['text'])
            body=structured['result']
            exact={k:body.get(k) for k in ('procedure','pseudocode','assembly','jump_tables','references','callees')}
            target=evidence/(row['fn_name']+'.json'); target.write_text(json.dumps(exact,indent=2),encoding='utf-8')
            info['full_body_evidence']=target.relative_to(ROOT).as_posix()
            info['full_body_sha256']=digest(target)
        output.append(info)
    (HERE/'scope.json').write_text(json.dumps({'static_only':True,'selected':61,'pure':52,'context':9,'rea_ok':sum(r['rea']['ok'] for r in output),'sources':{p:digest(ROOT/p) for p in ['source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/ControllerState.h']},'entries':output},indent=2),encoding='utf-8')
    shorts=set('GetCarGunLeftRight GetCarGunUpDown GetSteeringLeftRight GetCarGunFired CarGunJustDown GetSteeringUpDown GetPedWalkLeftRight- GetPedWalkUpDown- GetHandBrake GetBrake GetAccelerate'.split())
    calls=[]
    for row in output:
        if row['phase']!='pure':continue
        name=row['fn_name'];typ='int16' if name in shorts else 'uint8' if name in ('GetMeleeAttack','MeleeAttackJustDown') else 'uint32' if name=='GetTouchedTimeDelta' else 'bool'
        calls.append(f'    Compare<{typ}>(log,"{name}",{row["address"]},{str(name.startswith("GetAnaloguePad")).lower()},{str(name in ("GetMeleeAttack","MeleeAttackJustDown")).lower()},fixtures);')
    assert len(calls)==52
    update='''#pragma once
#include "PadBank.hpp"
namespace RequestedPadSelectorsLive {
inline void Update() {
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pad-selectors-runtime.jsonl");
    std::vector<Range> globals;
    for(auto [address,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB73458,sizeof(CPad::Pads)},{0xB736F0,14},{0xB7CB84,4}})globals.push_back({reinterpret_cast<void*>(address),ReversingProbe::Bytes(reinterpret_cast<void*>(address),size)});
    const auto fixtures=Fixtures();
    log<<json({{"event","start"},{"functions",52},{"fixtures_per_function",fixtures.size()}}).dump()<<'\\n';
CALLS
    for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());
    bool restored=true;for(const auto& range:globals)restored &= range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
    log<<json({{"event",restored?"complete":"error"},{"state_restored",restored},{"functions",52},{"comparisons",52*fixtures.size()}}).dump()<<'\\n';
}
}
'''.replace('CALLS','\n'.join(calls))
    (HERE/'PadDispatch.hpp').write_text(update,encoding='utf-8')
    print('Prepared exact identities:',len(output),'fresh full REA bodies:',sum(r['rea']['ok'] for r in output))
if __name__=='__main__': main()
