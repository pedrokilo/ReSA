"""Independent gates for Stats UI routes, ABI, restoration and special title fixtures."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CStats::FindCriminalRatingString':2048,'CStats::UpdateStatsOnRespawn':2048,
          'CStats::UpdateStatsAddToHealth':2048,'CStats::CheckForStatsMessage':2048}
HELPERS={0x53FB70,0x588B50,0x558E40,0x6A0050,0x588BE0,0x559080,0x821B40,0x55C180,0x55B980}
GATES=('stack_restored','nonvolatile_preserved','route_ok','return_nonnull','fixture_contract',
       'globals_restored','helper_route_ok','floating_state_restored')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1 or not all(begin[0].get(k) is True for k in ('registered_hook','helper_routes_ok','floating_state_restored')):
        raise RuntimeError('Inicio sin rutas/restauracion demostradas')
    helpers=begin[0]['helper_routes']
    if len(helpers)!=9 or {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] for h in helpers):
        raise RuntimeError('Faltan colaboradores reales demostrados')
    if any(h['registered'] and not h['reconstructed'] and h['enabled'] for h in helpers):
        raise RuntimeError('Wrapper original habilitado en DLL')
    cases={fn:set() for fn in EXPECTED};pairs=set();flight=set();money=set();arrests=set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if row['event']!='comparison':continue
        fn=row['function']
        if row['pair'] in pairs or row['case'] in cases[fn]:raise RuntimeError('Caso/par repetido')
        pairs.add(row['pair']);cases[fn].add(row['case'])
        for side in ('original','candidate'):
            result=row[side]
            if result is None or not all(result.get(k) is True for k in GATES):raise RuntimeError('Resultado nulo o gate fallido')
        result=row['original']
        if fn=='CStats::FindCriminalRatingString':
            if not result['text'] or not result['eax'] or result['eax']==0xC1AEB8:raise RuntimeError('GXT invalido')
            rating=result['rating']
            if 375000<=rating<500000:flight.add(result['flight_ms'])
            if rating>=1000000:money.add(result['display_money'])
            if 1150<=rating<1300:arrests.add(result['arrests'])
    if pairs!=set(range(8192)) or cases!={fn:set(range(2048)) for fn in EXPECTED}:raise RuntimeError('Cobertura incompleta')
    if flight!={0,36000000,39600000} or money!={9999999,10000000,10000001} or arrests!={0,1001}:
        raise RuntimeError('Faltan rutas especiales de tiempo de vuelo, dinero o arrestos')
    complete=[r for r in rows if r['event']=='complete']
    if len(complete)!=1 or complete[0]['pairs']!=8192 or not all(complete[0].get(k) is True for k in ('state_restored','floating_state_restored','helper_route_ok')):
        raise RuntimeError('Final sin restauracion exacta')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesstatsui8',manifest['binaries']['asi'],analyze,'DependenciesStatsUi8',timeout=1500))
