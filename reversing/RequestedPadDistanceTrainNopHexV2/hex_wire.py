"""Full lossless wire, strict shapes; decodes to the unchanged revision2 oracle objects."""
import json,re
from hex_asset import strict_loads,exact,byte_string
def spans(value):
    if type(value) is not list or len(value)!=4:raise ValueError('Wrong global span count')
    return [byte_string(v,n) for v,n in zip(value,(0xD78,33,33,0x268))]
BYTE_FIELDS={'pad':0x134,'camera':0xD78,'matrix':0x54,'vehicle':0x5A0}
LEG=set('gate helper_patch_exact collaborator_valid pad camera matrix vehicle globals trace guards fp'.split())
TOP=set('event function pair case bank equal geometry mode gates vehicle_role duration_role old_duration request_duration request_frequency camera_bits point_bits matrix_address initial_pad initial_camera initial_matrix initial_vehicle initial_globals model_pad model_trace model_sum_bits model_magnitude_bits original candidate'.split())
TERMINAL=set('event pairs differences state_restored entries_restored collaborator_entries_restored globals_restored flags_restored fp_restored fp_status_before fp_status_after fp_control_before fp_control_after mxcsr_before mxcsr_after x87_environment_restored x87_environment_before x87_environment_after scope'.split())
def trace(value):
    if type(value) is not list:raise ValueError('Wrong trace type')
    for item in value:
        exact(item,{'event','player','remote','role','arguments_valid'})
        if item['event']!='find_player_vehicle' or type(item['player']) is not int or type(item['role']) is not int or type(item['remote']) is not bool or type(item['arguments_valid']) is not bool:raise ValueError('Wrong trace fields')
    return value
def decode_leg(value):
    exact(value,LEG)
    if any(type(value[k]) is not bool for k in ('gate','helper_patch_exact','collaborator_valid','guards')):raise ValueError('Wrong leg boolean')
    exact(value['fp'],{'control','exceptions','mxcsr'})
    if any(type(v) is not int for v in value['fp'].values()):raise ValueError('Wrong FP integer')
    return dict(value,**{k:byte_string(value[k],n) for k,n in BYTE_FIELDS.items()},globals=spans(value['globals']),trace=trace(value['trace']))
def decode_row(value):
    if type(value) is not dict:raise ValueError('Wire row must be object')
    if value.get('event') in ('complete','error'):
        exact(value,TERMINAL)
        for k in TERMINAL-{'event','scope','x87_environment_before','x87_environment_after'}:
            integer=k in ('pairs','differences') or k.endswith('_before') or k.endswith('_after')
            if type(value[k]) is not (int if integer else bool):raise ValueError('Wrong terminal field type')
        if type(value['scope']) is not str:raise ValueError('Wrong scope type')
        byte_string(value['x87_environment_before'],28);byte_string(value['x87_environment_after'],28)
        return value
    exact(value,TOP)
    if value['event']!='comparison' or type(value['function']) is not str or type(value['equal']) is not bool:raise ValueError('Wrong row event/type')
    ints=TOP-{'event','function','equal','camera_bits','point_bits','initial_pad','initial_camera','initial_matrix','initial_vehicle','initial_globals','model_pad','model_trace','original','candidate'}
    if any(type(value[k]) is not int for k in ints):raise ValueError('Wrong metadata integer')
    for k in ('camera_bits','point_bits'):
        if type(value[k]) is not list or len(value[k])!=3 or any(type(v) is not int or not 0<=v<2**32 for v in value[k]):raise ValueError('Wrong input float bits')
    result=dict(value,original=decode_leg(value['original']),candidate=decode_leg(value['candidate']),initial_globals=spans(value['initial_globals']),model_pad=byte_string(value['model_pad'],0x134),model_trace=trace(value['model_trace']))
    for k,n in BYTE_FIELDS.items():result['initial_'+k]=byte_string(value['initial_'+k],n)
    return result
def encode_comparison_for_test(value):
    result=dict(value)
    for side in ('original','candidate'):
        leg=dict(value[side])
        for k in BYTE_FIELDS:leg[k]=bytes(leg[k]).hex()
        leg['globals']=[bytes(v).hex() for v in leg['globals']];result[side]=leg
    for k in BYTE_FIELDS:result['initial_'+k]=bytes(value['initial_'+k]).hex()
    result['initial_globals']=[bytes(v).hex() for v in value['initial_globals']];result['model_pad']=bytes(value['model_pad']).hex()
    return result
