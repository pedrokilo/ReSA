# Biseccion automatica del agua oscura con ddmin.planificar y prueba_agua.py (criterio: objeto 2877 con modelo RW cargado).
import json, os, subprocess, sys, time
import ddmin
aqui=os.path.dirname(os.path.abspath(__file__))
est=os.path.join(aqui,'estado_agua_fn.json')
if not os.path.exists(est):
    st=json.load(open(os.path.join(aqui,'estado_ropero.json'),encoding='utf-8'))
    st['rondas']=[r for r in st['rondas'] if r['n_apagados']==0]
    st['culpables_rutas']=[]; st.pop('terminado',None)
    json.dump(st,open(est,'w',encoding='utf-8'),indent=1,ensure_ascii=False)
st=json.load(open(est,encoding='utf-8'))
ANDAMIO=('Global/CMenuManager','Global/CGenericGameStorage','Global/C_PcSave','Global/CFileMgr','Global/CLoadingScreen','Global/CGame')
U=st['U']; Uidx=list(range(len(U)))
print('grupos',len(U),'apagables',len(Uidx),flush=True)
while True:
    resp=[(r['apagado_idx'],r['resultado']) for r in st['rondas'] if r['fase']==st['fase'] and r['resultado'] in ('E','O')]
    plan=ddmin.planificar(Uidx,resp)
    if plan[0]=='fin':
        print('FIN',plan[2],[U[i] for i in plan[1]],flush=True); st['terminado']=True; st['culpables_rutas']=[U[i] for i in plan[1]]
        json.dump(st,open(est,'w',encoding='utf-8'),indent=1,ensure_ascii=False); break
    conj=plan[1]; n=len(st['rondas'])+1
    ruta=os.path.join(aqui,'rondas','x%03d.unhook.txt'%n); af=os.path.join(aqui,'rondas','x%03d.args'%n)
    open(ruta,'w',encoding='utf-8',newline='\n').write('\n'.join(U[i] for i in conj)+'\n')
    open(af,'w',newline='').write('--unhook-file='+ruta.replace(chr(92),'/'))
    print('ronda',n,'apagando',len(conj),'-',plan[2],flush=True)
    for intento in range(2):
        rc=subprocess.run([sys.executable,'-B',os.path.join(aqui,'prueba_agua.py'),af,'x%03d'%n],cwd=aqui,capture_output=True,text=True).returncode
        if rc in (0,1): break
        print('  fallo de arranque, reintento',intento+1,flush=True)
    else: rc=1; print('  sin arranque: se anota como E (sin resultado fiable)',flush=True)
    res='O' if rc==0 else 'E'
    print('  ->',res,flush=True)
    st['rondas'].append({'n':n,'fase':st['fase'],'apagado_idx':conj,'n_apagados':len(conj),'motivo':plan[2],'resultado':res,'hora':time.strftime('%Y-%m-%dT%H:%M:%S'),'duracion':'auto','decision':'auto'})
    json.dump(st,open(est,'w',encoding='utf-8'),indent=1,ensure_ascii=False)
