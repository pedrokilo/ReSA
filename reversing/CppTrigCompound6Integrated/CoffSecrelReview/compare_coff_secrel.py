"""Read-only COFF comparison. No compiler, linker, Git or game execution."""
import argparse, hashlib, json, struct
from pathlib import Path

def parse(path):
    data=Path(path).read_bytes()
    u16=lambda p:struct.unpack_from('<H',data,p)[0]
    u32=lambda p:struct.unpack_from('<I',data,p)[0]
    big=data[:4]==b'\0\0\xff\xff'
    if big:
        if u16(4)<2 or u16(6)!=0x14c:raise ValueError('Unsupported bigobj version/machine')
        sections, symptr, count, start, width=u32(44),u32(48),u32(52),56,20
    else:
        if u16(0)!=0x14c or u16(16)!=0:raise ValueError('Require standalone i386 COFF object')
        sections,symptr,count,start,width=u16(2),u32(8),u32(12),20,18
    strings=symptr+count*width
    if strings+4>len(data):raise ValueError('Truncated string table')
    def name(raw):
        if raw[:4]==b'\0'*4:
            pos=strings+struct.unpack('<I',raw[4:8])[0]
            return data[pos:data.index(0,pos)].decode('utf-8','strict')
        return raw.rstrip(b'\0').decode('utf-8','strict')
    syms={};i=0
    while i<count:
        p=symptr+i*width
        if big:section,typ,storage,aux=struct.unpack_from('<iHBB',data,p+12)
        else:section,typ,storage,aux=struct.unpack_from('<hHBB',data,p+12)
        syms[i]={'name':name(data[p:p+8]),'value':u32(p+8),'section':section,'type':typ,'storage':storage}
        i+=1+aux
    secs={}
    for k in range(sections):
        p=start+k*40;size,raw,rel=u32(p+16),u32(p+20),u32(p+24);n=u16(p+32);flags=u32(p+36)
        skip=0
        if flags&0x1000000:
            if n!=0xffff:raise ValueError('Malformed overflow relocations')
            n=u32(rel)-1;skip=10
        reloc=[]
        for j in range(n):
            off,si,typ=struct.unpack_from('<IIH',data,rel+skip+j*10)
            if si not in syms:raise ValueError('Relocation to auxiliary/missing symbol')
            reloc.append((off,syms[si],typ))
        # Uninitialized COFF sections have no file payload. Offset zero is the
        # file header, not their data (and includes the compilation timestamp).
        if raw==0:
            if not flags&0x80 or reloc:raise ValueError('Missing initialized payload or unexpected BSS relocations')
            payload=bytes(size)
        else:
            if raw+size>len(data):raise ValueError('Truncated section payload')
            payload=data[raw:raw+size]
        secs[k+1]={'data':payload,'relocs':reloc}
    return data,syms,secs

def function(path,name,aliases):
    data,syms,secs=parse(path)
    hits=[s for s in syms.values() if s['name']==name and s['section']>0 and s['type']&0x20]
    if len(hits)!=1:raise ValueError('Require exactly one defined function symbol: '+name)
    sym=hits[0];sec=secs[sym['section']];begin=sym['value']
    ends=[s['value'] for s in syms.values() if s['section']==sym['section'] and s['type']&0x20 and s['value']>begin]
    end=min(ends,default=len(sec['data']));body=bytearray(sec['data'][begin:end]);rels=[];changes=[]
    for off,target,typ in sec['relocs']:
        if not begin<=off<end:continue
        pos=off-begin
        if typ not in (0x6,0x7,0xb,0x14):raise ValueError('Unsupported relocation type '+hex(typ))
        if pos+4>len(body):raise ValueError('Relocation crosses function boundary')
        targetname=target['name'];helper=targetname in aliases
        if helper:
            if typ!=0x14 or pos==0 or body[pos-1]!=0xe8:raise ValueError('Authorized helper must be E8 REL32 call')
            canonical=aliases[targetname];changes.append({'offset':pos,'helper':canonical,'symbol':targetname})
        else:canonical=targetname
        # Relocation addends are compared, not silently normalized.
        addend=bytes(body[pos:pos+4]).hex()
        # SECREL is a 32-bit section/TLS offset. Preserve its raw bytes too;
        # compare type, offset, addend, symbol and referent without masking it.
        if typ != 0xb: body[pos:pos+4]=b'\0'*4
        destination=None if helper else (target['value'],target['type'],target['storage'])
        referent=None
        if not helper and target['section']>0 and not target['type']&0x20:
            referent=hashlib.sha256(secs[target['section']]['data']).hexdigest()
        rels.append((pos,typ,canonical,addend,destination,referent))
    return {'bytes':bytes(body).hex(),'relocations':sorted(rels),'helpers':changes,'object_sha256':hashlib.sha256(data).hexdigest(),'size':len(body)}

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--spec',required=True);args=ap.parse_args();spec=json.loads(Path(args.spec).read_text())
    oldalias={};newalias={}
    for pair in spec['helpers']:
        oldalias[pair['old']]=pair['canonical'];newalias[pair['new']]=pair['canonical']
    result=[];ok=True
    for job in spec['consumers']:
        old=function(job['old_object'],job['symbol'],oldalias);new=function(job['new_object'],job['symbol'],newalias)
        equal=old['bytes']==new['bytes'] and old['relocations']==new['relocations']
        # Require every mapped helper relocation to match by offset and canonical identity.
        equal=equal and [(x['offset'],x['helper']) for x in old['helpers']]==[(x['offset'],x['helper']) for x in new['helpers']]
        ok=ok and equal
        result.append({'symbol':job['symbol'],'equal':equal,'old':old,'new':new})
    print(json.dumps({'all_equal':ok,'comparisons':result},indent=2));return 0 if ok else 1
if __name__=='__main__':raise SystemExit(main())
