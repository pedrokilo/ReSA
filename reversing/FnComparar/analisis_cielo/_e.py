import sys, json, glob, statistics
sys.path.insert(0,'D:/Original/wv-nubes/reversing/FnComparar')
import png_util as P, cielo_analizar as C
E='D:/Original/wv-nubes/reversing/FnComparar/ejecuciones/'
def L(d):
    r={}
    for ln in open(d+'/resultados.jsonl',encoding='utf-8'):
        j=json.loads(ln)
        if j.get('tipo')=='cielo' and j['cap'].get('valida'): r[j['escena']]=j
    return r
for modo,pref in (('orig','o_E'),('std','s_E')):
    for d in sorted(glob.glob(E+pref+'*')):
        R=L(d); cel={}
        for e,j in R.items():
            cel.setdefault(e.rsplit('_r',1)[0],[]).append((sum(C.med(P.leer(d+'/'+e+'.png')[2],C.MASK))/3, j['clima_mem']['LightningFlash'], j['clima_mem']['Rain'], j['orient_ok']))
        for k,v in cel.items():
            print(modo,d.split('/')[-1][:6],k[2:],'n',len(v),'lum',[round(x[0]) for x in v],'flash',[x[1] for x in v],'rain',[round(x[2],2) for x in v][:3],'ok',all(x[3] for x in v))
