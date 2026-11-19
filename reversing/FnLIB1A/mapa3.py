import sys, math
sys.argv=['x']
exec(open('mapa2.py').read().split("changed=True")[0])
CRT={'___local_stdio_printf_options','__vsnprintf_l','__vsprintf_l','__vfprintf_l','_sprintf','_fprintf','_vsprintf','_gmtime'}
# --- orden de archivos en el EXE (por anclas) y fuente por archivo (orden de definicion)
files=collections.OrderedDict()
for (fl,n),f in src.items():
    if n in CRT and fl not in('gzio',): continue
    files.setdefault(fl,[]).append((fl,n))
def anchor_pos():
    a=collections.defaultdict(list)
    for e,(s,m) in match.items(): a[s[0]].append(e)
    return {f:sorted(v) for f,v in a.items()}
ap=anchor_pos()
print({f:(hex(v[0]),hex(v[-1]),len(v)) for f,v in ap.items()})

for va,r in exe.items(): r['x87']=int(r.get('x87') or 0)
def sim(e,s):
    re_,rs=exe[e],src[s]
    if re_['size']>8000: return 0.0
    r=re_['size']/max(rs['size'],1)
    v=math.exp(-(math.log(r)**2)/(2*0.6**2))
    # x87
    if (re_['x87']>0)!=(rs['x87']>0): v*=0.5
    # llamadas
    nc_e=int(re_['ncalls']); nc_s=sum(1 for c in rs['calls'] if c not in ('__chkstk','@__security_check_cookie@4'))
    v*=math.exp(-abs(nc_e-nc_s)/ (3+max(nc_e,nc_s)))
    # cadenas
    a,b=re_['strset'],rs['strset']
    if a or b:
        j=len(a&b)/max(len(a|b),1)
        v*=(0.3+0.7*j) if (a and b) else 0.45
    # consistencia de llamados ya emparejados
    ok=bad=0
    for c in re_['int_calls']:
        if c in match:
            if match[c][0] in rs['int_calls']: ok+=1
            else: bad+=1
    v*=(1+0.3*ok)*(0.8**bad)
    return v
def dp(exes,srcs,th=0.15,skip_e=0.12,skip_s=0.05):
    n,m=len(exes),len(srcs)
    sc=[[0.0]*(m+1) for _ in range(n+1)]; bt=[[None]*(m+1) for _ in range(n+1)]
    for i in range(1,n+1): sc[i][0]=sc[i-1][0]-skip_e; bt[i][0]='e'
    for j in range(1,m+1): sc[0][j]=sc[0][j-1]-skip_s; bt[0][j]='s'
    for i in range(1,n+1):
        for j in range(1,m+1):
            best=(sc[i-1][j]-skip_e,'e'); 
            t=sc[i][j-1]-skip_s
            if t>best[0]: best=(t,'s')
            x=sim(exes[i-1],srcs[j-1])
            if x>=th:
                t=sc[i-1][j-1]+x
                if t>best[0]: best=(t,'m')
            sc[i][j],bt[i][j]=best
    i,j=n,m; out=[]
    while i>0 or j>0:
        b=bt[i][j]
        if b=='m': out.append((exes[i-1],srcs[j-1],sim(exes[i-1],srcs[j-1]))); i-=1; j-=1
        elif b=='e': i-=1
        else: j-=1
    return out[::-1]
# secuencia ordenada de EXE y anclas
allE=sorted(e for e in exe if 0x7CFE00<=e<0x7EB5C0)
anch=sorted(e for e in match if match[e][1].startswith('cadenas'))
seg=[]
fixed=dict((e,match[e][0]) for e in anch)
bounds=[None]+anch+[None]
dpm={}
for a,b in zip(bounds,bounds[1:]):
    lo=a if a else 0x7CFE00-1; hi=b if b else 0x7EB5C0
    exes=[e for e in allE if lo<e<hi]
    if not exes: continue
    sa=match[a][0] if a else None; sb=match[b][0] if b else None
    def tail(s):
        l=files[s[0]]; return l[l.index(s)+1:]
    def head(s):
        l=files[s[0]]; return l[:l.index(s)]
    if sa and sb and sa[0]==sb[0]:
        l=files[sa[0]]; srcs=l[l.index(sa)+1:l.index(sb)] if l.index(sa)<l.index(sb) else []
    elif sa and sb: srcs=tail(sa)+head(sb)
    elif sa: srcs=tail(sa)
    elif sb: srcs=head(sb)
    else: srcs=[]
    srcs=[s for s in srcs if s not in sm or sm[s]==None]
    if not srcs: continue
    for e,s,x in dp(exes,srcs):
        dpm[e]=(s,x)
print('dp',len(dpm))
import pprint
for e in sorted(dpm):
    s,x=dpm[e]; print(hex(e),exe[e]['size'],s[1],src[s]['size'],round(x,2))
