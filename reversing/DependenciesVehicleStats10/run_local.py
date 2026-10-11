"""Rutas reales y efectos definidos de grasa/musculo, armas y natacion."""
import json
import math
import struct
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CStats::UpdateFatAndMuscleStats':2048,'CStats::UpdateStatsWhenWeaponHit':2048,'CStats::UpdateStatsWhenSwimming':2048}
HELPERS={0x53FB70,0x588B50,0x558E40,0x6A0050,0x588BE0,0x559760,0x821B40,0x55C180,0x55B980,0x743CD0,0x441390}
GATES=('stack_restored','nonvolatile_preserved','route_ok','helper_route_ok','nested_helper_route_ok',
       'arguments_unchanged','globals_restored','floating_state_restored')
COUNTERS=(0,1,999,1000,0x7fffffff,0x80000000,0xfffffffe,0xffffffff)
LIMITS=(0xbf800000,0,0x3f800000,0x3f7fffff,0x3f800001,0x49800000,0x4a800000,0x7f800000,0x7fc01234)
SCALARS=(0xbf800000,0,0x3f800000,0x41c80000,0x447a0000,0x7fc01234)
SKILLS=(0,0x3f800000,0x4479ffff,0x447a0000,0x447a0001,0xbf800000,0x7fc01234,0x7f800000)
STEPS=(0.,.01,.5,1.,1.999,3.,8.,-1.)

def f32(v):return struct.unpack('<f',struct.pack('<f',v))[0]
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def floating(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def unsigned_x87(v):return float(v) if v<0x80000000 else f32(float(v))
def ftol_low(v):return 0 if not math.isfinite(v) else math.trunc(v)&0xffffffff
def step_ms(i):return ftol_low(f32(f32(floating(i['step_bits'])*floating(0x3ca3d70a))*1000.))
def grants(counter,limit):return f32(floating(limit)*1000.) < unsigned_x87(counter)

def expected_input(fn,case):
    p,v=case%16,case//16
    i=dict(profile=p,value=v,fat_counter=COUNTERS[v%8],max_health_counter=COUNTERS[(v//8)%8],
           fat_limit_bits=LIMITS[(v//4)%9],max_health_limit_bits=LIMITS[(v//7)%9],
           fat_bits=SCALARS[(v//11+p//4)%6],muscle_bits=SCALARS[(v//13)%6],
           fat_loss_bits=SCALARS[(v//3)%6],muscle_loss_bits=SCALARS[(v//5)%6],
           muscle_increment_bits=bits(float(v%3)*25.),max_health_increment_bits=bits(float((v//3)%3)*25.),max_health_bits=SKILLS[(p//2+v//13)%8],flag=p%4,step_bits=bits(STEPS[(v//3)%8]))
    a,b=COUNTERS[(v//3)%8],0
    if 'WeaponHit' in fn:
        a=22+v%11;slot=6 if a==32 else a-22
        i.update(weapon=a,slot=slot,stat=69+slot,skill_bits=SKILLS[(v//11)%8],weapon_counter=COUNTERS[(v//7)%8],
                 weapon_limit_bits=LIMITS[(v//9)%9],last_slot=slot if p&2 else (slot+1)%10,
                 weapon_increment_bits=bits(float(v%3)*25.),coop=bool(p&1))
    if 'Swimming' in fn:
        a=(0xa55a5a00^(v<<8))|(p&1);b=(0x5aa5a500^(v<<8))|((p>>1)&1)
        i.update(stamina_counter=COUNTERS[(v//8)%8],lung_counter=COUNTERS[(v//16)%8],
                 stamina_limit_bits=LIMITS[(v//4)%9],lung_limit_bits=LIMITS[(v//7)%9],
                 normal_rate_bits=SCALARS[(v//3)%6],fast_rate_bits=SCALARS[(v//5)%6],swim_increment_bits=bits(float(v%3)*25.))
    i.update(arg0=a,arg1=b)
    return i

def effects(fn,i):
    out={};paths=set()
    if 'WeaponHit' in fn:
        counter,last=i['weapon_counter'],i['last_slot']
        if i['coop']:paths.add('coop_skip')
        elif not floating(i['skill_bits'])<1000.:paths.add('skill_skip')
        elif last!=i['slot']:counter,last=0,i['slot'];paths.add('weapon_changed')
        elif unsigned_x87(counter)>floating(i['weapon_limit_bits']):counter=0;paths.add('weapon_display')
        else:counter=(counter+1)&0xffffffff;paths.add('weapon_accumulate')
        return dict(weapon_counter_after=counter,last_slot_after=last),paths
    if 'Swimming' in fn:
        fast=bool((i['arg0']&255) or (i['arg1']&255));paths.add('fast_rate' if fast else 'normal_rate')
        arg=ftol_low(floating(i['fast_rate_bits'] if fast else i['normal_rate_bits']))
    else:arg=i['arg0']
    fg=grants(i['fat_counter'],i['fat_limit_bits']);mg=grants(i['max_health_counter'],i['max_health_limit_bits'])
    paths.add('fat_grant' if fg else 'fat_accumulate');paths.add('max_health_grant' if mg else 'max_health_accumulate')
    out['fat_counter_after']=0 if fg else (i['fat_counter']+((step_ms(i)*arg)&0xffffffff)//10)&0xffffffff
    out['max_health_counter_after']=0 if mg else (i['max_health_counter']+step_ms(i))&0xffffffff
    if fg:
        fat=floating(i['fat_bits']);loss=floating(i['fat_loss_bits'] if fat>0. else i['muscle_loss_bits'])
        if fat>0.:
            paths.add('fat_positive');paths.add('flag_one' if i['flag']==1 else 'flag_other')
        else:paths.add('fat_nonpositive_or_unordered')
        paths.add('loss_positive' if loss>0. else 'loss_nonpositive_or_unordered')
        if loss>0.:
            original=fat if fat>0. else floating(i['muscle_bits']);difference=f32(original-loss)
            paths.add('loss_above_zero' if difference>0. else 'loss_zero_or_clamped')
    if 'Swimming' in fn:
        sg=grants(i['stamina_counter'],i['stamina_limit_bits'])
        out['stamina_counter_after']=0 if sg else (i['stamina_counter']+step_ms(i))&0xffffffff
        paths.add('stamina_grant' if sg else 'stamina_accumulate')
        if not i['arg0']&255:out['lung_counter_after']=i['lung_counter'];paths.add('lung_skip')
        else:
            lg=grants(i['lung_counter'],i['lung_limit_bits']);paths.add('lung_grant' if lg else 'lung_accumulate')
            out['lung_counter_after']=0 if lg else (i['lung_counter']+step_ms(i))&0xffffffff
    return out,paths

REQUIRED_COMMON={'fat_grant','fat_accumulate','max_health_grant','max_health_accumulate','fat_positive','fat_nonpositive_or_unordered',
                 'flag_one','flag_other','loss_positive','loss_nonpositive_or_unordered','loss_above_zero','loss_zero_or_clamped'}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    starts=[r for r in rows if r['event']=='begin']
    if len(starts)!=1 or not all(starts[0].get(k) is True for k in ('registered_hook','helper_routes_ok','floating_state_restored')):
        raise RuntimeError('Inicio incompleto')
    helpers=starts[0]['helper_routes']
    if len(helpers)!=11 or {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] for h in helpers):raise RuntimeError('Rutas incompletas')
    for h in helpers:
        if h['registered'] and not h['reconstructed'] and h['enabled']:raise RuntimeError('Wrapper original habilitado')
        if h['entry'] in {0x55C180,0x55B980,0x743CD0,0x441390} and not all(h[k] for k in ('registered','reconstructed','enabled')):raise RuntimeError('Colaborador directo sin DLL real')
    nested=[r for r in rows if r['event']=='nested_helper_begin']
    if len(nested)!=1 or nested[0]['route']['entry']!=0x55C470 or not nested[0]['route']['helper_route_ok']:raise RuntimeError('FatMuscle anidado no demostrado')
    cases={fn:set() for fn in EXPECTED};pairs=set();paths={fn:set() for fn in EXPECTED};weapons=set();swimming=set()
    for r in rows:
        if r['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if r['event']!='comparison':continue
        fn,case=r['function'],r['case']
        if r['pair'] in pairs or case in cases[fn]:raise RuntimeError('Identificadores repetidos')
        pairs.add(r['pair']);cases[fn].add(case)
        if r['original']!=r['candidate'] or not r.get('equal') or not r.get('accepted'):raise RuntimeError('Diferencia o rechazo')
        for side in ('original','candidate'):
            v=r[side]
            if v is None or not all(v.get(k) is True for k in GATES):raise RuntimeError('Resultado nulo/gate fallido')
            if v['input']!=expected_input(fn,case) or len(v['state'])!=17:raise RuntimeError('Fixture/snapshot incompleto')
            output,coverage=effects(fn,v['input']);paths[fn]|=coverage
            if any(v[k]!=value for k,value in output.items()):raise RuntimeError('Efecto definido de contador incorrecto')
            if 'Swimming' in fn:
                h=v['nested_helper']
                if h['entry']!=0x55C470 or not h['helper_route_ok'] or not h['shared_reconstructed_collaborator']:raise RuntimeError('Nested DLL real ausente')
                swimming.add((v['input']['arg0']&255,v['input']['arg1']&255))
            if 'WeaponHit' in fn:weapons.add(v['input']['weapon'])
    if pairs!=set(range(6144)) or cases!={fn:set(range(2048)) for fn in EXPECTED}:raise RuntimeError('Cobertura incompleta')
    for fn in EXPECTED:
        required={'coop_skip','skill_skip','weapon_changed','weapon_display','weapon_accumulate'} if 'WeaponHit' in fn else REQUIRED_COMMON
        if 'Swimming' in fn:required=required|{'fast_rate','normal_rate','stamina_grant','stamina_accumulate','lung_skip','lung_grant','lung_accumulate'}
        if not required<=paths[fn]:raise RuntimeError('Faltan rutas '+str(required-paths[fn]))
    if weapons!=set(range(22,33)) or swimming!={(0,0),(0,1),(1,0),(1,1)}:raise RuntimeError('Dominios incompletos')
    finals=[r for r in rows if r['event']=='complete']
    if len(finals)!=1 or finals[0]['pairs']!=6144 or not all(finals[0].get(k) is True for k in ('state_restored','floating_state_restored','helper_route_ok','nested_helper_restored')):raise RuntimeError('Final no restaurado')
    summary['complete_paths']={fn:sorted(paths[fn]) for fn in EXPECTED}
    summary['coverage_method']='serialized_inputs_and_counter_effects; no_instruction_branch_tracing'
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesvehiclestats10',manifest['binaries']['asi'],analyze,'DependenciesVehicleStats10',timeout=1500))
