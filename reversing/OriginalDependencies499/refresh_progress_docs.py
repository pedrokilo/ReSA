"""Refresh cohort documentation from admitted runtime evidence only."""
import csv,json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
def load(p):return json.loads(p.read_text(encoding='utf-8'))
def main():
 progress=load(HERE/'progress.json');approved=load(HERE/'approved.json')
 rows=list(csv.DictReader((HERE/'progress.csv').open(encoding='utf-8-sig')))
 hooks=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
 banks={};details=[]
 for address,item in approved.items():
  summary=load(ROOT/item['validation_summary']);identity=item['identity'];count=summary['functions'][identity]
  details.append(f"| {identity} | {address} | {count} | [Evidencia](../{Path(item['validation_summary']).parent.name}/README.md) |")
  banks[item['validation_summary']]=summary
 total=sum(s['pairs'] for s in banks.values());passed=progress['runtime_passed'];pending=progress['game_runtime_pending']
 current=len(hooks);groups=len({r['class'] for r in hooks})
 text=f"""# Dependencias originales fuera del catálogo · cohorte de 499

Cohorte fija del 10 de octubre de 2026: destinos originales explícitos referenciados desde entradas catalogadas. `targets.csv` y `baseline.json` preservan el snapshot inicial y sus hashes. El inventario general puede cambiar al admitir funciones; esta cohorte siempre conserva sus 499 direcciones.

Tres subagentes revisaron los destinos por símbolos, contratos fuente y evidencia binaria. [progress.csv](progress.csv) conserva decisiones y admisiones; las 21 inciertas siguen explícitas en los informes de revisión. Clasificar una función no equivale a validarla.

| Clasificación | Total | Aprobadas en juego | Pendientes |
| --- | ---: | ---: | ---: |
| Funciones del juego | 235 | {passed} | {pending} |
| Bibliotecas/runtime/SDK | 243 | 0 | Seguimiento separado |
| Identidad o procedencia incierta | 21 | 0 | 21 |
| Total de la cohorte | 499 | {passed} | {499-passed} |

## Funciones admitidas

{passed} funciones, **{total} pares finales, cero diferencias y ningún resultado original/candidate nulo**, con rutas RH de producción apagado/activo comprobadas, restauración de estado, partidas y configuración. Se cuenta únicamente la corrida final de cada banco; las preparatorias no inflan el total. Catálogo principal actual: {current}/{current} funciones y {groups}/{groups} grupos con evidencia histórica; su 100% no incluye las dependencias pendientes de esta cohorte.

| Función | Dirección EXE | Pares finales | Banco y alcance |
| --- | --- | ---: | --- |
"""+'\n'.join(details)+"""

Cada banco documenta dirección/instrucción del EXE, reconstrucción o corrección, dominios de fixtures y límites. Se comparan retornos, memoria y registros pertinentes antes de admitir filas nuevas. La observación compuesta de DependenciesGarage2 no concedió aprobación al llamador FindGarageForObject. Su admisión posterior tiene banco propio en DependenciesWorld4, con 4.096 pares y prueba de la ruta activa del colaborador geométrico. Los servicios de bibliotecas aún empleados se declaran por banco.

`approved.json` enlaza evidencias; `verify_bank.py` comprueba cada par, resúmenes, rutas, restauración y huellas exactas; `register_bank.py` admite sólo direcciones clasificadas como juego dentro de esta cohorte. `dependency_review=null`: aprobación histórica de los fixtures, sin certificar independencia transitiva del EXE ni integración universal. Las huellas de corridas anteriores no se reescriben cuando otra función del mismo archivo cambia.

## Continuación

[work-queue.csv](work-queue.csv) muestra las entradas del juego todavía pendientes. Las bibliotecas siguen en el inventario completo; no se consideran reconstruidas por clasificarlas. Las inciertas necesitan resolver identidad/procedencia antes de diseñar pruebas. No registrar por nombre supuesto, cuerpo existente ni por pertenecer a un rango de direcciones.
"""
 (HERE/'README.md').write_text(text,encoding='utf-8')
 prepared={}
 for bank in ROOT.joinpath('reversing').glob('Dependencies*'):
  h=bank/'LiveComparison.hpp'
  if h.exists():
   for c,f,a,n in re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(0x[0-9A-Fa-f]+),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}',h.read_text(encoding='utf-8')):prepared[int(a,16)]={'identity':c+'::'+f,'bank':bank.name,'pairs_expected':n}
 queue=[]
 for r in rows:
  if r['category']=='game' and r['runtime_status']!='passed':
   d=prepared.get(int(r['address'],16),{});queue.append({'address':r['address'],'identity':d.get('identity',r['identity']),'status':'prepared_not_validated' if d else 'pending','bank':d.get('bank',''),'pairs_expected':d.get('pairs_expected',''),'source_snapshot':r['source']})
 assert len(queue)==pending
 with (HERE/'work-queue.csv').open('w',encoding='utf-8',newline='') as f:
  w=csv.DictWriter(f,fieldnames=['address','identity','status','bank','pairs_expected','source_snapshot']);w.writeheader();w.writerows(queue)
 status=ROOT/'docs/CurrentStatus.md';s=status.read_text(encoding='utf-8');start=s.index('## Dependencias fuera del catálogo:');end=s.index('<!-- RESA-HOOK-UNLOCK-FINAL:BEGIN -->',start)
 section=f"""## Dependencias fuera del catálogo: progreso comprobado

La cohorte inicial de 499 destinos se clasificó en 235 funciones del juego, 243 de bibliotecas y 21 inciertas. **{passed} funciones del juego admitidas después de {total} pares finales dentro del juego, cero diferencias, ningún resultado nulo y restauración comprobada.** Quedan {pending} entradas del juego y 21 inciertas; las bibliotecas se siguen aparte. [Inventario, bancos y alcance](../reversing/OriginalDependencies499/README.md).

El catálogo principal contiene **{current}/{current} funciones y {groups}/{groups} grupos con evidencia histórica**, sin pendientes dentro del catálogo. Ese porcentaje no incluye las dependencias pendientes de la cohorte ni acredita independencia del EXE. Las cifras del corte 453 que siguen abajo conservan su fecha histórica.

"""
 s=s[:start]+section+s[end:];s=re.sub(r'El catálogo de progreso contiene \*\*\d+/\d+ clases, namespaces o grupos y \d+/\d+ funciones',f'El catálogo de progreso contiene **{groups}/{groups} clases, namespaces o grupos y {current}/{current} funciones',s,count=1);status.write_text(s,encoding='utf-8')
 handoff=f"""# Continuidad: dependencias fuera del catálogo

Estado del 10 de octubre de 2026, rama main. Cohorte fija de 499: 235 juego, 243 bibliotecas y 21 inciertas. Admitidas {passed} funciones con {total} pares finales RH OFF/ON, cero diferencias, cero resultados nulos y restauración comprobada. Catálogo {current}/{current}, grupos {groups}/{groups}; evidencia histórica, sin certificar independencia del EXE. [Inventario](../OriginalDependencies499/README.md).

Quedan {pending} entradas del juego y 21 inciertas. Consultar progress.csv, work-queue.csv y los READMEs de bancos. Registrar sólo después de verificar JSONL, ABI, efectos y restauración dentro del juego completo. La clasificación estática no concede aprobación. Las bibliotecas tienen seguimiento separado. Las propuestas pendientes aparecen en la cola y no reciben flags de validación.

Compilaciones y GTA en serie, paralelismo 2. No editar fuentes durante la compilación ni modificar instrumentación aprobada sin repetir su banco. Usar sólo la copia CVehicle Test; nunca la instalación personal. EXE canónico SHA256: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. runtime_runner.py, los 28 documentos congelados y la whitelist permanecen intactos.

Cada banco conserva validation-summary.json y runs/corrida con summary.json, approval-audit.json, RESULTADO.txt y comparisons.jsonl.gz sin pérdida. verify_bank.py audita los pares; register_bank.py admite exclusivamente direcciones clasificadas como juego de esta cohorte. No se hizo commit ni push de estas tandas. Excluir backups, cachés, artifacts, dumps y docs/Reorganizacion de futuras publicaciones.
"""
 (ROOT/'reversing/_handoff/DEPENDENCIES499_CURRENT.md').write_text(handoff,encoding='utf-8')
 print(json.dumps({'cohort_passed':passed,'pending_game':pending,'approved_pairs':total,'catalogue':current,'groups':groups}))
if __name__=='__main__':main()
