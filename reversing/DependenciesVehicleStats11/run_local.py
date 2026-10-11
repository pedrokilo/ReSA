"""Comparacion completa y oraculos independientes; no ejecutado durante preparacion."""
import json,math,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CStats::UpdateStatsWhenCycling':2048,'CVehicle::IsValidModForVehicle':2048}
HELPERS={0x53FB70,0x588B50,0x558E40,0x6A0050,0x588BE0,0x559760,0x821B40,0x823820,0x55C470,0x55C180,0x55B980,0x4C8740,0x4C8750}
GATES=('stack_restored','nonvolatile_preserved','route_ok','helper_route_ok','argument0_unchanged','fixture_readonly','mod_globals_readonly','globals_restored','floating_state_restored')
COUNTERS=(0,1,999,1000,0x7fffffff,0x80000000,0xfffffffe,0xffffffff)
LIMITS=(0xbf800000,0,0x3f800000,0x3f7fffff,0x3f800001,0x49800000,0x4a800000,0x7f800000,0x7fc01234)
SPEEDS=(0,0x3d4ccccc,0x3d4ccccd,0x3d4cccce,0x3e4ccccc,0x3e4ccccd,0x3e4cccce,0x3f800000,0xbf800000,0x7fc01234,0x3e800000,0x3d800000,0x3c800000,0x3f000000,0x40000000,1)
PEDALS=(0,0xbf800000,0x3e4ccccd,0x7fc01234)
STEPS=(0.,.01,.5,1.,1.999,3.,8.,-1.)
def f32(v):return struct.unpack('<f',struct.pack('<f',v))[0]
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def floating(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def unsigned(v):return float(v) if v<0x80000000 else f32(float(v))
def ftol(v):return math.trunc(v)&0xffffffff if math.isfinite(v) else 0

def expected_input(fn,case,slots,pointer):
    p,v=case%16,case//16;i=dict(profile=p,value=v,arg1=pointer)
    if 'Cycling' in fn:
        i.update(arg0=(0xa55a5a00^(v<<8))|(p&1),pedal_bits=PEDALS[(p//2)%4],speed_bits=SPEEDS[v%16],contact=0 if p<8 else 1+v%2,
                 stamina_counter=COUNTERS[v%8],skill_counter=COUNTERS[(v//8)%8],stamina_limit_bits=LIMITS[(v//4)%9],skill_limit_bits=LIMITS[(v//7)%9],step_bits=bits(STEPS[(v//3)%8]),increment_bits=bits(float(v%3)*25.))
    else:
        group,count=p%4,0 if p==0 else 1+v%15
        flags=0x900 if p<4 else 0x4400 if 8<=p<=11 else 0x1100 if p==12 else 0x800 if p==13 else 0x4500 if p==14 else 0
        wh=0 if p==1 else count-1 if p==2 else -1;nh=0 if p in (4,12) else 8 if p in (5,13) else 17 if p in (6,14) else -1
        i.update(arg0=slots[1],flags=flags,group=group,count=count,vehicle_model=slots[0],candidate_model=slots[1],wheel_hit=wh,normal_hit=nh,
                 nitro_blocked=p==8,vehicle_type=0 if p==11 else 1,nitro_flag=p==10,
                 wheels=[slots[1] if n==wh else 1000+group*15+n for n in range(15)],upgrades=[slots[1] if n==nh else -1 for n in range(18)])
    return i

def effects(fn,i):
    paths=set()
    if 'Cycling' not in fn:
        f=i['flags'];kind=f&0x7c00
        if f&0x100 and kind==0x800:
            result=i['arg0'] in i['wheels'][:i['count']];paths.add('wheel_empty' if not i['count'] else 'wheel_hit' if result else 'wheel_miss')
        elif not f&0x100 and kind==0x4400:
            result=not i['nitro_blocked'] and (i['vehicle_type']!=1 or i['nitro_flag'])
            paths.add('nitro_blocked' if i['nitro_blocked'] else 'nitro_type_other' if i['vehicle_type']!=1 else 'nitro_flag_yes' if i['nitro_flag'] else 'nitro_flag_no')
        else:
            result=i['arg0'] in i['upgrades'];paths.add('normal_hit' if result else 'normal_miss')
        paths.add('mod_profile_'+str(i['profile']))
        return {'boolean_result':int(result),'argument1_after':i['arg1']},paths
    tick=ftol(f32(f32(floating(i['step_bits'])*floating(0x3ca3d70a))*1000.))
    active=abs(floating(i['pedal_bits']))>0 or bool(i['arg0']&255)
    stamina=i['stamina_counter'];skill=i['skill_counter'];arg1=i['arg1']
    if active:
        paths.add('fat_fast' if i['arg0']&255 else 'fat_normal')
        grant=f32(floating(i['stamina_limit_bits'])*1000.)<unsigned(stamina)
        paths.add('stamina_grant' if grant else 'stamina_accumulate');stamina=0 if grant else (stamina+tick)&0xffffffff
    else:paths.add('fat_skip')
    if f32(floating(i['skill_limit_bits'])*1000.)<unsigned(skill):skill=0;paths.add('skill_grant')
    else:
        speed=floating(i['speed_bits']);squared=f32(speed*speed)
        small=f32(floating(0x3d4ccccd)**2);large=f32(floating(0x3e4ccccd)**2)
        factor=3. if i['contact']<2 and squared>small else (1.5 if i['arg0']&255 else 1.) if squared>large else 0.
        paths.add('factor_'+str(factor));paths.add('contact_'+str(i['contact']))
        if factor:
            arg1=tick;amount=ftol(float(math.ceil(f32(unsigned(tick)*factor))));skill=(skill+amount)&0xffffffff
        for name,threshold in (('small',small),('large',large)):
            if math.isnan(squared):paths.add(name+'_unordered')
            else:paths.add(name+('_below' if squared<threshold else '_equal' if squared==threshold else '_above'))
    return {'stamina_after':stamina,'skill_after':skill,'argument1_after':arg1},paths
REQUIRED_CYCLING={'fat_skip','fat_normal','fat_fast','stamina_grant','stamina_accumulate','skill_grant','factor_0.0','factor_1.0','factor_1.5','factor_3.0','contact_0','contact_1','contact_2','small_below','small_equal','small_above','small_unordered','large_below','large_equal','large_above','large_unordered'}
REQUIRED_MOD={'wheel_empty','wheel_hit','wheel_miss','nitro_blocked','nitro_type_other','nitro_flag_yes','nitro_flag_no','normal_hit','normal_miss'}|{'mod_profile_'+str(p) for p in range(16)}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    starts=[r for r in rows if r['event']=='begin']
    if len(starts)!=1 or not all(starts[0].get(k) is True for k in ('registered_hook','helper_routes_ok','floating_state_restored','unused_model_slots_proven')):raise RuntimeError('Inicio incompleto')
    start=starts[0];slots=start['unused_model_slots'];layout=start['snapshot_layout']
    if len(slots)!=2 or len(set(slots))!=2 or not all(19000<=s<20000 for s in slots) or len(layout)!=24 or start['snapshot_blocks']!=24:raise RuntimeError('Slots/snapshot incompleto')
    helpers=start['helper_routes']
    if len(helpers)!=13 or {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] for h in helpers):raise RuntimeError('Rutas de colaboradores incompletas')
    for h in helpers:
        if h['registered'] and not h['reconstructed'] and h['enabled']:raise RuntimeError('Wrapper recursivo habilitado')
        if h['entry'] in {0x55C470,0x55C180,0x55B980,0x4C8740,0x4C8750} and not all(h[k] for k in ('registered','reconstructed','enabled')):raise RuntimeError('DLL colaboradora no demostrada')
    paths={fn:set() for fn in EXPECTED};cases={fn:set() for fn in EXPECTED};pairs=set();pointer=None
    for r in rows:
        if r['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if r['event']!='comparison':continue
        fn,case=r['function'],r['case']
        if r['pair'] in pairs or case in cases[fn]:raise RuntimeError('Identificadores duplicados')
        pairs.add(r['pair']);cases[fn].add(case)
        if r['original'] is None or r['candidate'] is None or r['original']!=r['candidate'] or not r.get('equal') or not r.get('accepted'):raise RuntimeError('Diferencia/NULL')
        for side in ('original','candidate'):
            out=r[side];i=out['input']
            if pointer is None:pointer=i['arg1']
            if pointer!=layout[-3]['address']+16 or i!=expected_input(fn,case,slots,pointer):raise RuntimeError('Fixture incorrecto')
            if not all(out.get(k) is True for k in GATES):raise RuntimeError('Gate fallido')
            if len(out['state'])!=24 or any(len(raw)!=2*d['bytes'] for raw,d in zip(out['state'],layout)):raise RuntimeError('Memoria incompleta')
            effects_expected,coverage=effects(fn,i);paths[fn]|=coverage
            if any(out[k]!=v for k,v in effects_expected.items()) or out['argument0_after']!=i['arg0']:raise RuntimeError('Oracle/ABI incorrecto')
            raw=bytes.fromhex(out['state'][0]);get=lambda a:struct.unpack_from('<I',raw,a-0xb78200)[0]
            if out['stamina_after']!=get(0xb794dc) or out['skill_after']!=get(0xb794e0):raise RuntimeError('Memoria y contador discordantes')
            for index,canary in ((-3,0xa5),(-2,0x5a),(-1,0xc3)):
                memory=bytes.fromhex(out['state'][index])
                if memory[:16]!=bytes([canary])*16 or memory[-16:]!=bytes([canary])*16:raise RuntimeError('Canary dañado')
    if pairs!=set(range(4096)) or cases!={fn:set(range(2048)) for fn in EXPECTED}:raise RuntimeError('Casos incompletos')
    for fn,required in zip(EXPECTED,(REQUIRED_CYCLING,REQUIRED_MOD)):
        if not required<=paths[fn]:raise RuntimeError('Ramas faltantes '+str(required-paths[fn]))
    final=[r for r in rows if r['event']=='complete']
    if len(final)!=1 or final[0]['pairs']!=4096 or not all(final[0].get(k) is True for k in ('state_restored','floating_state_restored','helper_route_ok')):raise RuntimeError('Final no restaurado')
    summary['complete_paths']={fn:sorted(paths[fn]) for fn in EXPECTED};summary['coverage_method']='serialized_fixture_and_counter_oracle_no_instruction_tracing'
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciesvehiclestats11',manifest['binaries']['asi'],analyze,'DependenciesVehicleStats11',timeout=1500))
