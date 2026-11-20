"""Selecciona del auditoria2_auto.csv las comparaciones DIFERENTE de confianza alta para parchear automaticamente:
ANCLA (comparacion contra literal) con cubo 1/1 (una sola comparacion C++ y una sola del EXE con esa constante y ese tipo de particion),
linea sin comentarios de bloque, en zonas donde un NaN llega al mundo (se excluyen audio, HUD, nubes, espejos, clima, plantas, Fx, interiores, pipelines, marcadores, agua de render)."""
import csv,re,sys
NOAREA=re.compile(r'Audio/|Hud\.cpp|Clouds|Mirrors|Weather|PlantMgr|Fx/|Interior/|Pipelines|3dMarker|Checkpoint|WaterLevel|X87Math|Lights|Shadows|Coronas|Skidmarks|Renderer|Radar|Frontend|MenuManager|Font|Sprite|TimeCycle|Stats|Fire|Particle|Pickups|Blip|Cheat|Replay|Script')
r=list(csv.DictReader(open('auditoria2_auto.csv',encoding='utf-8')))
sel=[];skip=[]
for x in r:
    if x['veredicto_auto']!='DIFERENTE_CAND' or not x['tipo'].startswith('ANCLA'): continue
    n,m=x['cubo'].split()[0].split('/')
    if not (x['cubo'].split()[1] in ('disjunto','elem') and int(m)>=1): continue
    if x['mismo_largo']!='1': skip.append((x['archivo'],x['linea'],'no automatico')); continue
    if NOAREA.search(x['archivo']): skip.append((x['archivo'],x['linea'],'fuera de zona')); continue
    sel.append(x)
w=csv.writer(open('sel1.csv','w',newline='',encoding='utf-8')); w.writerow(['archivo','linea','occ','dir_exe_cmp'])
for x in sel: w.writerow([x['archivo'],x['linea'],x['occ'],x['detalle_exe'].split(':')[0]])
print(len(sel),'seleccionadas',len(skip),'omitidas')
