"""Segunda seleccion (confianza media): ANCLA cuya alineacion por orden (fuente vs direccion, dentro de cada constante, mismas cuentas n_cpp==n_exe) pone el NaN en el lado contrario."""
import csv,re
NOAREA=re.compile(r'Audio/|Hud\.cpp|Clouds|Mirrors|Weather|PlantMgr|Fx/|Interior/|Pipelines|3dMarker|Checkpoint|WaterLevel|X87Math|Lights|Shadows|Coronas|Skidmarks|Renderer|Radar|Frontend|MenuManager|Font|Sprite|TimeCycle|Stats|Fire|Particle|Pickups|Blip|Cheat|Replay|Script')
r=list(csv.DictReader(open('auditoria2_auto.csv',encoding='utf-8')))
sel=[]
for x in r:
    if not x['tipo'].startswith('ANCLA') or 'DIFIERE' not in (x['orden'] or ''): continue
    if x['veredicto_auto']=='FALSO_POSITIVO_NAN_EXPLICITO' or NOAREA.search(x['archivo']): continue
    nm=x['orden'].split()[-1].split('/')
    if nm[0]!=nm[1]: continue
    if x['mismo_largo']!='1': continue
    if re.search(r'ordenada|con NaN|NaN|isnan',x['codigo']): continue
    sel.append(x)
w=csv.writer(open('sel2.csv','w',newline='',encoding='utf-8')); w.writerow(['archivo','linea','occ','dir_exe_cmp'])
for x in sel:
    w.writerow([x['archivo'],x['linea'],x['occ'],x['orden'].split(':')[0]]); print(x['archivo'].split('game_sa/')[1],x['linea'],x['codigo'][:80],x['orden'])
