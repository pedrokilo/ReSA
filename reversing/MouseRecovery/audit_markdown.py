"""Inventory project Markdown and check local inline links without reading caches."""
import hashlib
import json
import re
import subprocess
from pathlib import Path
from urllib.parse import unquote

ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent

def main():
    names=sorted(set(subprocess.check_output(
        ['git','ls-files','--cached','--others','--exclude-standard','--','*.md'],cwd=ROOT,text=True).splitlines()))
    records=[];broken=[];checked=0
    for name in names:
        path=ROOT/name
        raw=path.read_bytes();source=raw.decode('utf-8-sig')
        # Code examples may deliberately contain placeholder links.
        prose=re.sub(r'```.*?```','',source,flags=re.S)
        links=[]
        for match in re.finditer(r'!?\[[^\]\n]*\]\((<[^>]+>|[^)\n]+)\)',prose):
            target=match.group(1).strip()
            if target.startswith('<'):target=target[1:-1]
            else:target=re.split(r'\s+["\']',target,maxsplit=1)[0]
            if re.match(r'^(?:https?://|mailto:|app:)',target,re.I):continue
            filepart,_,fragment=target.partition('#')
            if not filepart:
                continue  # In-page fragments are not filesystem links.
            filepart=unquote(filepart).replace('\\','/')
            dest=Path(filepart) if re.match(r'^[A-Za-z]:/',filepart) else path.parent/filepart
            exists=dest.exists();checked+=1
            links.append(dict(target=target,exists=exists))
            if not exists:broken.append(dict(document=name,target=target))
        records.append(dict(path=name,sha256=hashlib.sha256(raw).hexdigest(),bytes=len(raw),
                            lines=len(source.splitlines()),local_links=links))
    report=dict(markdown_files=len(records),local_file_links_checked=checked,broken=broken,documents=records,
                scope='Git tracked and untracked nonignored Markdown; local file targets, not remote URLs or heading fragments')
    out=HERE/'artifacts/docs-audit.json';out.parent.mkdir(exist_ok=True)
    out.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='documents'},ensure_ascii=True))
    return int(bool(broken))

if __name__=='__main__':raise SystemExit(main())
