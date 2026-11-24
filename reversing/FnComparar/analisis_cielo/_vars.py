import json,glob,sys
def L(d):
    r={}
    for ln in open(d+'/resultados.jsonl',encoding='utf-8'):
        j=json.loads(ln)
        if j.get('tipo')=='cielo': r[j['escena']]=j
    return r
dirs={k:glob.glob('D:/Original/wv-nubes/reversing/FnComparar/ejecuciones/%s-*'%k)[0] for k in ('o_C1','o_C2','s_C1','s_C2')}
R={k:L(v) for k,v in dirs.items()}
keys=['CloudCoverage','Foggyness','ExtraSunnyness','Rain','WetRoads','Wind','Rainbow','SunGlare','InterpolationValue','Old','New','Forced','LightningFlash']
nk=['VolFader','VolUsedNum','VolDensity','VolMaxDist','CloudRot','IndivRot','vc_usados']
for esc in sorted(R['o_C1']):
    if 'pi+0.70' not in esc or ('c04' not in esc and 'c07' not in esc and 'c00' not in esc): continue
    print(esc[2:])
    for k in R:
        j=R[k][esc]; print('  ',k,{x:j['clima_mem'].get(x) for x in keys}, {x:j['nubes'].get(x) for x in nk})
