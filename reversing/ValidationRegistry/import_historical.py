"""Import a reviewed batch of explicit function/evidence mappings, without current credit."""
import csv
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
from validation_registry import KEY, digest


def main():
    registry = ROOT / 'reversing/validation-registry.json'
    data = json.loads(registry.read_text(encoding='utf-8'))
    rows = list(csv.DictReader((ROOT / 'docs/hooks.csv').open(encoding='utf-8-sig')))
    known = {tuple(r['hook'][k] for k in KEY) for r in data['functions']}
    specs = []
    for folder in ('CBaseModelInfo', 'CClouds', 'CConversations', 'CMenuSystem',
                   'CTaskComplexWander', 'CTaskSimpleJetPack', 'CTaskSimpleUseGun'):
        d = json.loads((ROOT / f'reversing/{folder}/validation-summary.json').read_text())
        cls, fn = d['function'].split('::')
        specs.append((folder, cls, fn, d['pairs'], d['address']))
    for folder, cls, functions in (
        ('FxManager', 'FxManager_c', {'DestroyAllFxSystems':180, 'Update':540}),
        ('CPedList', 'CPedList', dict.fromkeys(('BuildListFromGroup_NotInCar_NoLeader',
          'BuildListOfPedsOfPedType','RemovePedsAttackingPedType','RemovePedsThatDontListenToPlayer'),1024)),
        ('CClothes','CClothes',{'GetTextureDependency':4096}),
        ('CScripted2dEffects','CScripted2dEffects',{'Init':256}),
        ('CTaskComplexUseClosestFreeScriptedAttractorSprint','CTaskComplexUseClosestFreeScriptedAttractorSprint',{'Clone':2048,'GetTaskType':2048}),
        ('CAESound','CAESound',{'operator=':1024}),
    ):
        specs.extend((folder,cls,fn,pairs,None) for fn,pairs in functions.items())
    d = json.loads((ROOT/'reversing/CPed/validation-summary.json').read_text())
    specs.extend(('CPed','CPed',fn,n,None) for fn,n in d['function_pairs'].items())
    for folder in ('CPlayerInfo','FurnitureSubGroup_c'):
        d = json.loads((ROOT/f'reversing/{folder}/validation-summary.json').read_text())
        specs.extend((folder,folder,fn,d['pairs_per_function'][str(i)],None)
                     for i,fn in enumerate(d['functions']))
    added = 0
    for folder,cls,fn,pairs,address in specs:
        if (cls, fn) == ('CPed', 'MakeTyresMuddySectorList'):
            fn = 'MakeTyresMuddySectorList<CPtrListSingleLink<CPhysical*>>'
        matches = [r for r in rows if r['class']==cls and r['fn_name']==fn
                   and (address is None or int(r['address'],16)==int(address,16)
                        or (r['type']=='V' and int(r['address'],16)==0))]
        if len(matches)!=1:
            raise ValueError(f'Ambiguous mapping: {cls}::{fn}: {matches}')
        row = matches[0]
        key = tuple(row[k] for k in KEY)
        if key in known:
            continue
        path = ROOT/f'reversing/{folder}/validation-summary.json'
        summary = json.loads(path.read_text(encoding='utf-8'))
        if summary.get('passed') is not True:
            raise ValueError(f'Unapproved summary: {path}')
        limits = summary.get('limitations') or summary.get('scope') or 'Alcance limitado al banco; consultar el README. Vigencia pendiente de revision.'
        if isinstance(limits,list):
            limits = ' '.join(limits)
        data['functions'].append(dict(
            hook={k:row[k] for k in KEY},
            evidence=dict(path=path.relative_to(ROOT).as_posix(), sha256=digest(path),
                          passed_path=['passed'], pairs=pairs, original_address=address),
            tested_sources={}, source_provenance=None, dependency_review=None,
            scope=limits,
            review_reference=f'reversing/{folder}/README.md'))
        known.add(key)
        added += 1
    registry.write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(f'Added {added} historical entries; no current validations granted.')


if __name__ == '__main__':
    main()
