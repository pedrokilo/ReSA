"""Strict lossless byte transport; canonical objects retain every legacy byte and field."""
import re,json
def unique_pairs(pairs):
    result={}
    for key,value in pairs:
        if key in result:raise ValueError('Duplicate JSON transport field')
        result[key]=value
    return result
def strict_loads(line):
    def reject_constant(value):raise ValueError('Nonfinite JSON transport constant')
    return json.loads(line,object_pairs_hook=unique_pairs,parse_constant=reject_constant)
COMPARISON=set('event function address seed domain alias clock deadline delay duration frequency stored_frequency gates pad_id switch_exact guards_unchanged inactive_scratch_unchanged return_reference_exact globals_unchanged helper_entries_original equal original candidate scratch_seed_bytes'.split())
LEG=set('pad controllers globals return_reference_exact fp scratch_physical scratch_guards_unchanged inactive_scratch_unchanged'.split())
TOP_BOOLS=set('switch_exact guards_unchanged inactive_scratch_unchanged return_reference_exact globals_unchanged helper_entries_original equal'.split())
TOP_INTS=set('address seed alias clock deadline delay duration frequency stored_frequency gates pad_id'.split())
TERMINAL=set('event comparisons state_restored floating_environment_restored switch_exact guards_unchanged inactive_scratch_unchanged return_reference_exact globals_unchanged helper_entries_original phased_chunk_pairs phased_original_buffer_estimate_bytes phased_original_buffer_bound'.split())
PROOF=set('artifact_sha256 pdb_sha256 function_rva function_bytes scratch_rva scratch_bytes encoded_operands actual_module_base mapped_body_exact artifact_hash_exact explicit_highlow_relocations'.split())
def exact(value,keys):
    if type(value) is not dict or set(value)!=keys:raise ValueError('Unexpected transport object shape')
def byte_string(value,length):
    if type(value) is not str or re.fullmatch('[0-9a-f]{'+str(length*2)+'}',value) is None:raise ValueError('Malformed lowercase byte transport or length')
    return list(bytes.fromhex(value))
def spans(value,lengths):
    if type(value) is not list or len(value)!=len(lengths):raise ValueError('Wrong byte span count')
    return [byte_string(span,length) for span,length in zip(value,lengths)]
def decode_leg(value):
    exact(value,LEG)
    if any(type(value[k]) is not bool for k in ('return_reference_exact','scratch_guards_unchanged','inactive_scratch_unchanged')):raise ValueError('Wrong leg bool')
    exact(value['fp'],{'rounding','exceptions','mxcsr'})
    if any(type(v) is not int for v in value['fp'].values()):raise ValueError('Wrong FP transport type')
    return dict(value,pad=byte_string(value['pad'],340),controllers=spans(value['controllers'],[80]*3),globals=spans(value['globals'],[16,33,33]),scratch_physical=spans(value['scratch_physical'],[80]*2))
def decode_row(value):
    if type(value) is not dict:raise ValueError('Transport row must be object')
    event=value.get('event')
    if event=='start':
        exact(value,{'event','bank','function','address','cases','wire_encoding'})
        if value['wire_encoding']!='lowercasehex-v1':raise ValueError('Unsupported byte encoding')
        if type(value['address']) is not int or type(value['cases']) is not int or type(value['bank']) is not str or type(value['function']) is not str:raise ValueError('Wrong start type')
        return {k:v for k,v in value.items() if k!='wire_encoding'}
    if event=='current_artifact_proof':
        exact(value,{'event','proof'});exact(value['proof'],PROOF)
        if any(type(value['proof'][k]) is not bool for k in ('mapped_body_exact','artifact_hash_exact')):raise ValueError('Wrong proof bool')
        if any(type(value['proof'][k]) is not int for k in PROOF-{'artifact_sha256','pdb_sha256','mapped_body_exact','artifact_hash_exact'}):raise ValueError('Wrong proof integer')
        if any(type(value['proof'][k]) is not str or re.fullmatch('[0-9a-f]{64}',value['proof'][k]) is None for k in ('artifact_sha256','pdb_sha256')):raise ValueError('Wrong proof digest')
        return value
    if event=='comparison':
        exact(value,COMPARISON)
        if any(type(value[k]) is not bool for k in TOP_BOOLS) or any(type(value[k]) is not int for k in TOP_INTS):raise ValueError('Wrong comparison metadata type')
        if type(value['function']) is not str or type(value['domain']) is not str:raise ValueError('Wrong comparison strings')
        return dict(value,original=decode_leg(value['original']),candidate=decode_leg(value['candidate']),scratch_seed_bytes=spans(value['scratch_seed_bytes'],[48,48]))
    if event=='complete':
        exact(value,TERMINAL)
        for key in TERMINAL-{'event'}:
            expected=int if key in ('comparisons','phased_chunk_pairs','phased_original_buffer_estimate_bytes') else bool
            if type(value[key]) is not expected:raise ValueError('Wrong terminal type')
        return value
    raise ValueError('Unexpected error or transport event')
def encode_comparison_for_test(value):
    """Independent test transport encoder; native code uses integer nibble conversion."""
    result=dict(value)
    for side in ('original','candidate'):
        leg=dict(value[side]);leg['pad']=bytes(leg['pad']).hex()
        for key in ('controllers','globals','scratch_physical'):leg[key]=[bytes(v).hex() for v in leg[key]]
        result[side]=leg
    result['scratch_seed_bytes']=[bytes(v).hex() for v in value['scratch_seed_bytes']]
    return result
