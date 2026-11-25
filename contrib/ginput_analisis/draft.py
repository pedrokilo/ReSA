import sys,re,json,struct; sys.path.insert(0,'.')
import sites as S
from names2 import label
from names import name as oldname
from summ import summ
import modes as M
from orig import orb
allsites=[]
for tag,(lo,hi) in (('inst',(0x1000a740,0x1000d29a)),('late1',(0x10008294,0x10008310)),('menu',(0x1000d2a0,0x1000d500))):
    for d in S.sites(lo,hi): d['tag']=tag; allsites.append(d)
def lab(a): return label(a) or oldname(a,0x2000) or ''
rows=[]
for d in allsites:
    st,en=d['start'],d['end']
    O=S.dtext(d['orig'],st); N=S.dtext(d['new'],st)
    tg=re.findall(r'0x(1000[0-9a-f]{4})',N)
    tgt=int(tg[0],16) if tg else None
    # replaced callee
    m=re.match(r'.*?call 0x([0-9a-f]+)',O)
    callee=int(m.group(1),16) if m and O.startswith('call') else None
    kind=None
    if N.startswith('jmp 0x1000') and en-st>=4 and orb(st,1)[0] not in (0xe8,): kind='JMP'
    rows.append(dict(addr=st,end=en,tag=d['tag'],O=O,N=N,tgt=tgt,callee=callee,cname=lab(callee) if callee else '',ctx=lab(st)))
rows.sort(key=lambda r:r['addr'])
json.dump(rows,open('../draft_rows.json','w'),indent=0)
for r in rows:
    mm=''
    if r['tgt']:
        try:
            x=M.modes(r['tgt'])
            if x: mm=" M0=%s M1=%s"%(','.join(x[0]),','.join(x[1]))
        except Exception as e: mm=' (err)'
    print("%06x %s | %s | callee=%s | tgt=%s%s"%(r['addr'],r['ctx'],r['N'][:60],r['cname'],hex(r['tgt']) if r['tgt'] else '',mm))
