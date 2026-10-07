"""Mapa estático de ReSA: catálogo, código y evidencias existentes. No ejecuta GTA."""
import argparse, base64, csv, gzip, hashlib, importlib.util, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path
sys.dont_write_bytecode=True

def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def load(path):
    with path.open(encoding='utf-8-sig',newline='') as f: return list(csv.DictReader(f))
def clean(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*',lambda m: ''.join('\n' if c=='\n' else ' ' for c in m[0]),text,flags=re.S)
def split_args(text):
    parts=[]; start=0; depth=0; quoted=False; escaped=False
    for i,c in enumerate(text):
        if quoted:
            if c=='"' and not escaped: quoted=False
            escaped=c=='\\' and not escaped
            continue
        if c=='"': quoted=True
        elif c in '(<[{': depth+=1
        elif c in ')>]}': depth-=1
        elif c==',' and depth==0: parts.append(text[start:i].strip()); start=i+1
    parts.append(text[start:].strip()); return parts

def scan(root):
    declarations=[]; functions=[]; vtables=[]; registrations=[]; globals_=[]; files=[]
    scopes=re.compile(r'\bnamespace\s+([\w:]+)\s*\{|\b(class|struct)\s+(?:NOTSA_EXPORT_VTABLE\s+|alignas\([^)]*\)\s+)*([A-Za-z_]\w*)\s*(?:final\s*)?(?::([^;{}]*))?\s*\{|[{}]')
    heads=re.compile(r'^[ \t]*(?:[\w:<>,*&~]+[ \t]+)*?((?:[A-Za-z_]\w*::)*(?:operator\s*(?:new(?:\[\])?|delete(?:\[\])?|[^\s(]+)|~?[A-Za-z_]\w*))[ \t]*\(([^;{}]*?)\)[ \t]*(?:(?:const|override|final|noexcept|NOTSA_EXPORT_VTABLE)[ \t]*)*(?:=[ \t]*(?:0|default|delete)[ \t]*)?(?:\s*:\s*[^;{]+)?([;{])',re.M)
    scope_hook=re.compile(r'RH_Scoped(VirtualClass|Class|NamespaceName|Namespace)\s*\(([^;\n]*)\)')
    install=re.compile(r'RH_Scoped(\w*Install)\s*\(([^;\n]*)\)')
    for path in sorted((root/'source').rglob('*')):
        if path.suffix not in ('.h','.hpp','.cpp','.c'): continue
        rel=path.relative_to(root).as_posix(); source_bytes=path.read_bytes(); raw=source_bytes.decode('utf-8-sig',errors='replace'); text=clean(raw)
        files.append(dict(file=rel,sha256=hashlib.sha256(source_bytes).hexdigest()))
        events=[(m.start(),'scope',m) for m in scopes.finditer(text)]
        events.extend((m.start(),'function',m) for m in heads.finditer(text))
        stack=[]
        for pos,kind,m in sorted(events,key=lambda x:(x[0],x[1])):
            names=[s[1] for s in stack if s[0] in ('namespace','class')]
            line=text.count('\n',0,pos)+1
            if kind=='scope':
                if m[0]=='}':
                    if stack: stack.pop()
                elif m[1]: stack.append(('namespace',m[1]))
                elif m[3]:
                    name='::'.join(names+[m[3]])
                    declarations.append(dict(name=name,kind=m[2],bases=(m[4] or '').strip(),file=rel,line=line,status='source_type_definition'))
                    stack.append(('class',m[3]))
                else: stack.append(('block',''))
            else:
                name=m[1]
                if name in ('if','for','while','switch','return','catch','assert','static_assert') or name.startswith(('RH_','VALIDATE_')) or name.isupper(): continue
                # Local statements are excluded unless the name is explicitly qualified.
                if stack and stack[-1][0]=='block': continue
                qualified=name if '::' in name else '::'.join(names+[name])
                functions.append(dict(name=qualified,signature=m[0].strip()[:700],file=rel,line=line,
                    kind='definition_candidate' if m[3]=='{' else 'declaration_candidate',status='source_function_candidate',address=''))
        owners=[]
        for m in scope_hook.finditer(text):
            args=split_args(m[2]); owner=args[0].strip('"') if args else ''
            owners.append((m.start(),owner))
            if m[1]=='VirtualClass' and len(args)>=3 and re.fullmatch(r'0x[\da-fA-F]+',args[1]):
                vtables.append(dict(owner=owner,va=f'0x{int(args[1],16):08X}',rva=f'0x{int(args[1],16)-0x400000:08X}',slots_declared=args[2],file=rel,line=text.count('\n',0,m.start())+1,status='source_vtable_reference'))
        for m in install.finditer(text):
            args=split_args(m[2]); scope=next((o for p,o in reversed(owners) if p<m.start()),'')
            addr=next((a for a in args[1:] if re.fullmatch(r'0x[\da-fA-F]+',a)),None)
            if not scope or not addr or not args: continue
            name=args[0]; overload='Overloaded' in m[1] or 'Overload' in m[1]
            has_label=overload and len(args)>1 and args[1].startswith('"')
            label=args[1].strip('"') if has_label else ''
            registrations.append(dict(owner=scope,name=name+('-'+label if has_label else ''),va=f'0x{int(addr,16):08X}',
                macro='RH_Scoped'+m[1],file=rel,line=text.count('\n',0,m.start())+1,status='source_hook_reference'))
        for m in re.finditer(r'\b(StaticRef|StaticArray)\s*<[^;\n]*?>\s*\(\s*(0x[\da-fA-F]+)',text):
            line=text.count('\n',0,m.start())+1; source=raw.splitlines()[line-1].strip()
            variable=re.search(r'(\w+)\s*=\s*(?:StaticRef|StaticArray)',source)
            globals_.append(dict(name=variable[1] if variable else 'reference',va=f'0x{int(m[2],16):08X}',
                rva=f'0x{int(m[2],16)-0x400000:08X}',file=rel,line=line,expression=source[:400],status='source_global_reference'))
    return declarations,functions,vtables,registrations,globals_,files

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument('--output',type=Path)
    p.add_argument('--template',type=Path); args=p.parse_args(); root=args.root.resolve()
    out=args.output or root/'docs/MemoryMap';out.mkdir(parents=True,exist_ok=True)
    hooks=load(root/'docs/hooks.csv'); extras=load(root/'docs/hooks_notsa.csv')
    spec=importlib.util.spec_from_file_location('resa_validation_registry',root/'contrib/validation_registry.py'); module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    validation=module.evaluate(root,root/'docs/hooks.csv')
    key=lambda r:tuple(r[k] for k in module.KEY)
    registry=json.loads((root/'reversing/validation-registry.json').read_text(encoding='utf-8'))
    evidence={key(r['hook']):r for r in registry['functions']}
    states={key(r['hook']):r for r in validation['functions']}
    types,sourcefuncs,tables,registrations,globals_,files=scan(root)
    reg_index=defaultdict(list)
    for r in registrations: reg_index[(r['owner'],r['name'])].append(r)
    owners=defaultdict(list)
    for row in hooks:
        state=states.get(key(row),{}); record=evidence.get(key(row),{})
        ev=record.get('evidence',{})
        row.update(name=row['class']+'::'+row['fn_name'],evidence_ok=bool(state.get('evidence_ok')),current=bool(state.get('current')),
            source_match=bool(state.get('source_match')),evidence_path=ev.get('path',''),scope=state.get('scope',''),
            evidence_status='current' if state.get('current') else 'historical' if state.get('evidence_ok') else 'unlinked',
            status='catalogue_entry',va=row['address'] if int(row['address'],16) else '',rva='',source_locations='',source_address_candidates='')
        candidates=reg_index[(row['class'],row['fn_name'])]
        row['source_locations']=';'.join(f"{r['file']}:{r['line']}" for r in candidates)
        row['source_address_candidates']=';'.join(sorted({r['va'] for r in candidates}))
        if row['va']: row['rva']=f"0x{int(row['va'],16)-0x400000:08X}"
        # Zero addresses stay unresolved in the catalogue; candidates remain separate.
        owners[row['class']].append(row)
    classes=[]
    for name,entries in sorted(owners.items()):
        decls=[d for d in types if d['name']==name]
        vt=[t for t in tables if t['owner']==name]
        classes.append(dict(name=name,kind=decls[0]['kind'] if decls else 'catalogue_group',
            total=len(entries),reversed=sum(r['reversed']=='1' for r in entries),locked=sum(r['locked']=='1' for r in entries),
            validated_flag=sum(r['validated']=='1' for r in entries),evidence_ok=sum(r['evidence_ok'] for r in entries),current=sum(r['current'] for r in entries),
            vtables=';'.join(dict.fromkeys(t['va'] for t in vt)),bases=';'.join(dict.fromkeys(d['bases'] for d in decls)),
            source_locations=';'.join(f"{d['file']}:{d['line']}" for d in decls),status='catalogue_class_or_group'))
    changed_files=[r['file'] for r in files if not (root/r['file']).is_file() or sha(root/r['file'])!=r['sha256']]
    summary=dict(image_base='0x00400000',catalogue_entries=len(hooks),catalogue_owners=len(classes),
        reversed=sum(r['reversed']=='1' for r in hooks),pending=sum(r['reversed']=='0' for r in hooks),
        validated_flags=sum(r['validated']=='1' for r in hooks),approved_evidence=validation['documented'],current_validation=validation['current'],
        source_matches=validation['source_matches'],source_type_definitions=len(types),source_unique_type_names=len({t['name'] for t in types}),
        source_function_candidates=len(sourcefuncs),source_vtable_references=len(tables),source_global_references=len(globals_),source_hook_references=len(registrations),
        source_files=len(files),notsa_entries=len(extras),addressed_catalogue_entries=sum(bool(r['va']) for r in hooks),
        catalogue_zero_addresses=sum(not r['va'] for r in hooks),hooks_sha256=sha(root/'docs/hooks.csv'),registry_sha256=sha(root/'reversing/validation-registry.json'),
        source_manifest_sha256=hashlib.sha256(json.dumps(files,sort_keys=True).encode()).hexdigest(),
        source_files_changed_during_generation=changed_files,binary_verified=False,exhaustive_cpp_parse=False)
    dataset=dict(summary=summary,classes=classes,functions=hooks,types=types,source_functions=sourcefuncs,vtables=tables,globals=globals_,registrations=registrations,notsa=extras)
    for name,entries in dataset.items():
        if name=='summary': continue
        with (out/(name+'.csv')).open('w',encoding='utf-8',newline='') as f:
            if entries:
                writer=csv.DictWriter(f,fieldnames=list(entries[0]),extrasaction='ignore');writer.writeheader();writer.writerows(entries)
    (out/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    (out/'source_manifest.json').write_text(json.dumps(files,indent=2)+'\n',encoding='utf-8')
    previous=out/'index.html'
    if previous.is_file():
        packed_old=re.search(r"atob\('([^']+)'\)",previous.read_text(encoding='utf-8'))
        if packed_old:
            old=json.loads(gzip.decompress(base64.b64decode(packed_old[1])))
            dataset.update({key:value for key,value in old.items() if key.startswith('binary_')})
            if 'binary_summary' in old:
                summary.update(binary_inventory_available=True,binary_verified=True)
                (out/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    template=args.template or root/'reversing/MemoryMap/viewer.template.html'
    packed=base64.b64encode(gzip.compress(json.dumps(dataset,ensure_ascii=False,separators=(',',':')).encode(),mtime=0)).decode()
    (out/'index.html').write_text(template.read_text(encoding='utf-8').replace('__DATA__',packed),encoding='utf-8')
    print(json.dumps(summary,ensure_ascii=False,indent=2))
if __name__=='__main__': main()
