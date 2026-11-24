import sys, json, glob, statistics
sys.path.insert(0,'D:/Original/wv-nubes/reversing/FnComparar')
import png_util as P, cielo_analizar as C
E='D:/Original/wv-nubes/reversing/FnComparar/ejecuciones/'
def L(d):
    r={}
    for ln in open(d+'/resultados.jsonl',encoding='utf-8'):
        j=json.loads(ln)
        if j.get('tipo')=='cielo': r[j['escena']]=j
    return r
o=glob.glob(E+'o_D1-*')[0]; s=glob.glob(E+'s_D1-*')[0]; Ro=L(o); Rs=L(s)
grp={}
for e in sorted(Ro):
    k=e.rsplit('_r',1)[0]; grp.setdefault(k,[]).append(e)
for k,es in grp.items():
    mo=[];ms=[]
    for e in es:
        mo.append(sum(C.med(P.leer(o+'/'+e+'.png')[2],C.MASK))/3); ms.append(sum(C.med(P.leer(s+'/'+e+'.png')[2],C.MASK))/3)
    to=bytes.fromhex(Ro[es[0]]['mem']['timecycle']); ts=bytes.fromhex(Rs[es[0]]['mem']['timecycle'])
    dif=[(i,to[i],ts[i]) for i in range(len(to)) if to[i]!=ts[i]]
    print(k[2:],'orig',[round(x,1) for x in mo],'std',[round(x,1) for x in ms],'media o %.1f s %.1f'%(statistics.mean(mo),statistics.mean(ms)),'sd o %.2f s %.2f'%(statistics.pstdev(mo),statistics.pstdev(ms)))
    print('    timecycle bytes distintos (idx,o,s):',dif[:30])
