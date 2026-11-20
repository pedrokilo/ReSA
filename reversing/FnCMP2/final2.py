"""Genera auditoria2.csv, auditoria2_A.csv, bancos_a_repetir2.csv y las cifras del README a partir de:
 pre_head.csv  = escaneo de scan2.py sobre el arbol de la rama cmp1 (antes de los parches de FnCMP2)
 post.csv      = escaneo sobre el arbol actual (despues de los parches)
Una comparacion cuenta como parcheada si estaba DIFERENTE_CAND antes y ya no lo esta despues (misma archivo/linea/ocurrencia)."""
import csv,re,json,collections,os,struct
HERE=os.path.dirname(os.path.abspath(__file__))
ROOT=os.path.abspath(os.path.join(HERE,'..','..'))
rd=lambda f:list(csv.DictReader(open(os.path.join(HERE,f),encoding='utf-8')))
pre=rd('pre_head2.csv'); post=rd('post2.csv')
key=lambda x:(x['archivo'],x['linea'],x['occ'],x['op'])
postk={}
for x in post: postk[(x['archivo'],x['linea'],x['occ'])]=x
NOAREA=re.compile(r'Audio/|Hud\.cpp|Clouds|Mirrors|Weather|PlantMgr|Fx/|Interior/|Pipelines|3dMarker|Checkpoint|WaterLevel|X87Math|Lights|Shadows|Coronas|Skidmarks|Renderer|Radar|Frontend|MenuManager|Font|Sprite|TimeCycle|Stats|Fire|Particle|Pickups|Blip|Cheat|Replay|Script')
RIESGO=[(r'Entity/Vehicle|Physical|Placeable|Automobile',5),(r'Entity/Ped|Tasks/|PedIntelligence|PlayerInfo|Events/',4),(r'Cam|IdleCam|Camera',4),(r'Collision|World\.cpp|Streaming',4),(r'CarCtrl|CarAI|CarGenerator|Population|GangWars|Train|Rope|Garage|Door|Weapon|ProjectileInfo|Timer|RoadBlocks|Boat|Bike',3)]
def riesgo(x):
    s=0
    for p,w in RIESGO:
        if re.search(p,x['archivo']): s=max(s,w)
    m=re.search(r' w(\d+)',x['tipo'] or '')
    wr=int(m.group(1)) if m else 0
    if NOAREA.search(x['archivo']): return 0
    return s*10+min(wr,20)
rows=[]; patched=[]
for x in pre:
    v=x['veredicto_auto']
    if v=='SIN_DIRECCION': fin='SIN_DIRECCION'; det='funcion sin direccion EXE conocida'
    elif v=='IGUAL': fin='IGUAL'; det=('anclada por constante' if x['tipo'].startswith('ANCLA') else 'emparejada por orden/tipo')
    elif v=='FALSO_POSITIVO_NAN_EXPLICITO': fin='FALSO_POSITIVO'; det='el C++ ya trata el NaN de forma explicita (isnan / x!=x)'
    elif v=='INDETERMINADO': fin='INDETERMINADO'; det='ninguna comparacion x87 del EXE de la misma particion queda libre'
    else:
        p=postk.get((x['archivo'],x['linea'],x['occ']))
        if p is not None and p['veredicto_auto']!='DIFERENTE_CAND':
            fin='DIFERENTE'; det='parcheada (NaN como el EXE) '+(x['detalle_exe'].split(':')[0]); patched.append(x)
        else:
            est=x['cubo'].split()[1] if x['cubo'] else ''
            if x['tipo'].startswith('ANCLA') and est in('disjunto','elem') and NOAREA.search(x['archivo']):
                fin='DIFERENTE'; det='sin parchear: fuera de las zonas de riesgo (audio/HUD/render); confirmado por constante'
            elif x['tipo'].startswith('ANCLA') and est in('disjunto','elem'):
                fin='DIFERENTE'; det='sin parchear: linea no automatizable (varias comparaciones/comentario de bloque); confirmado por constante'
            else:
                fin='INDETERMINADO'; det='candidato de baja confianza (atribucion ambigua o sin ancla); no se parchea'
    rows.append(dict(archivo=x['archivo'],linea=x['linea'],funcion=x['funcion'],dir_exe=x['dir_exe'],codigo=x['codigo'],veredicto=fin,motivo=det,
                     tipo=(x['tipo'] or '').split(' w')[0],riesgo=riesgo(x),cubo=x['cubo'],orden=x['orden'],const=x['const'],origen='B'))
# A: los 162 POSIBLE_DIFERENTE_NAN de FnCMP1
A=[x for x in csv.DictReader(open(os.path.join(HERE,'..','FnCMP1','escaneo.csv'),encoding='utf-8')) if x['resultado']=='POSIBLE_DIFERENTE_NAN']
def norm(s): return re.sub(r'\s+','',s)[:30]
byf=collections.defaultdict(list)
for r in rows: byf[r['archivo']].append(r)
Arows=[]; cnt=collections.Counter()
known={('CarAI.cpp','302'),('Train.cpp','1221'),('Train.cpp','1230'),('GangWars.cpp','1090'),('GangWars.cpp','1107'),('GangWars.cpp','1149')}
for a in A:
    pr=a['predicado'].split()[-1].rstrip('fF')
    try: kv=float(pr)
    except: kv=None
    cand=[r for r in byf[a['archivo']] if abs(int(r['linea'])-int(a['linea']))<=8 and norm(r['codigo'])==norm(a['codigo'])]
    exact=[r for r in cand if r['const'] and kv is not None and abs(float(r['const'])-kv)<1e-4]
    use=exact or cand
    if any(a['archivo'].endswith(f) and a['linea']==l for f,l in known): v='IGUAL'; why='ya confirmada a mano en FnCMP1 (auditoria.csv) y parcheada en cmp1'
    elif not use: v='INDETERMINADO'; why='sin fila equivalente en el escaner (linea movida o funcion sin direccion)'
    else:
        vs=[r['veredicto'] for r in use]
        v='DIFERENTE' if 'DIFERENTE' in vs else ('FALSO_POSITIVO' if 'FALSO_POSITIVO' in vs else ('IGUAL' if 'IGUAL' in vs else 'INDETERMINADO'))
        sel=[r for r in use if r['veredicto']==v][0]; why=sel['motivo']
        if v=='IGUAL': why='el escaner empareja la comparacion con una del EXE con el mismo lado de NaN ('+sel['tipo']+', cubo '+sel['cubo']+')'
    cnt[v]+=1
    Arows.append(dict(archivo=a['archivo'],linea=a['linea'],funcion=a['funcion'],dir_exe=a['dir_exe'],codigo=a['codigo'],predicado=a['predicado'],veredicto=v,motivo=why))
fn=['archivo','linea','funcion','dir_exe','codigo','predicado','veredicto','motivo']
w=csv.DictWriter(open(os.path.join(HERE,'auditoria2_A.csv'),'w',newline='',encoding='utf-8'),fieldnames=fn); w.writeheader(); w.writerows(Arows)
print('A',len(A),cnt)
rows.sort(key=lambda r:(r['veredicto']!='DIFERENTE',-r['riesgo'],r['archivo'],int(r['linea'])))
fn=['archivo','linea','funcion','dir_exe','codigo','veredicto','motivo','tipo','riesgo','cubo','orden','const','origen']
w=csv.DictWriter(open(os.path.join(HERE,'auditoria2.csv'),'w',newline='',encoding='utf-8'),fieldnames=fn); w.writeheader(); w.writerows(rows)
c=collections.Counter(r['veredicto'] for r in rows); print('B',len(rows),c)
c2=collections.Counter((r['veredicto'],r['riesgo']>0) for r in rows); print(c2)
# bancos a repetir: funciones con comparaciones parcheadas + las parcheadas a mano
manual={('source/game_sa/Entity/Vehicle/Automobile.cpp','0x6ad690','2345/2346/2353 (a mano)')}
reg={}
for i in range(1,7):
    pth=os.path.join(ROOT,'reversing','validation-registry','parte-%03d.json'%i)
    if not os.path.exists(pth): continue
    d=json.load(open(pth,encoding='utf-8'))
    for f in d['functions']:
        h=f['hook']
        try: a=int(h['address'],16)
        except: a=0
        if a: reg.setdefault(a,[]).append((i,f))
        reg.setdefault((h['class'],h['fn_name']),[]).append((i,f))
byfn=collections.OrderedDict()
for x in patched:
    byfn.setdefault((x['archivo'],x['dir_exe'],x['funcion']),[]).append(x['linea'])
for f,a,l in manual:
    byfn.setdefault((f,a,'CAutomobile::ProcessControlInputs'),[]).append('2345,2346,2353')
out=[]
for (f,a,name),lines in byfn.items():
    addr=int(a,16); mm=re.match(r'(\w+)::(~?\w+)',name); e=reg.get(addr) or (reg.get((mm.group(1),mm.group(2))) if mm else None)
    if e:
        part,fe=e[0]; ev=fe.get('evidence',{}); pairs=ev.get('pairs',''); evp=ev.get('path','')
        banco='reversing/validation-registry/parte-%03d.json'%part
    else: banco=''; pairs=''; evp=''
    out.append(dict(funcion=name,direccion='0x%08X'%addr,archivo_parcheado=f,lineas=' '.join(sorted(set(lines),key=int)) if all(s.isdigit() for s in lines) else ','.join(lines),
                    banco_registro_validacion=banco,pares=pairs,evidencia_banco=evp,nota='repetir con entradas NaN en los operandos de las comparaciones parcheadas' if banco else 'sin banco en el registro: revisar docs/hooks.csv (funcion no validada o interna)'))
w=csv.DictWriter(open(os.path.join(HERE,'bancos_a_repetir2.csv'),'w',newline='',encoding='utf-8'),fieldnames=list(out[0].keys())); w.writeheader(); w.writerows(out)
print('bancos',len(out),'con banco',sum(1 for o in out if o['banco_registro_validacion']))
