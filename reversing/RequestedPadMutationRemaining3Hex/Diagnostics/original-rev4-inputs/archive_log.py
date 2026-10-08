"""Lossless archive support for the future context runner; never touches live GTA logs."""
import gzip,hashlib,json
from pathlib import Path
def sha(path):
    digest=hashlib.sha256()
    with Path(path).open('rb') as stream:
        while block:=stream.read(1024*1024):digest.update(block)
    return digest.hexdigest()
def archive_preserved_log(path):
    raw=Path(path);info={'raw_sha256':sha(raw),'raw_bytes':raw.stat().st_size}
    if info['raw_bytes']<=90*1024*1024:return info
    target=raw.with_suffix(raw.suffix+'.gz')
    with raw.open('rb') as source,target.open('wb') as output,gzip.GzipFile(fileobj=output,mode='wb',mtime=0) as zipped:
        while block:=source.read(1024*1024):zipped.write(block)
    decoded=hashlib.sha256()
    with gzip.open(target,'rb') as source:
        while block:=source.read(1024*1024):decoded.update(block)
    assert decoded.hexdigest()==info['raw_sha256'],'Compressed archive failed roundtrip integrity'
    info.update(gzip_file=target.name,gzip_sha256=sha(target),lossless_verified=True)
    raw.with_name('log-archive.json').write_text(json.dumps(info,indent=2)+'\n',encoding='utf-8')
    raw.unlink()
    return info
