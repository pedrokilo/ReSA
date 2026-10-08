"""All 13952 synthetic canonical rows round-trip; no build/game/approval."""
import copy,hashlib,json
from pathlib import Path
import prepare_check as legacy
from hex_wire import encode_comparison_for_test,decode_row,strict_loads
def main():
    count=0
    for bank,key in enumerate(legacy.o.KEYS):
        for i in range(legacy.o.EXPECTED[key]):
            original=legacy.row_for(bank,i);wire=encode_comparison_for_test(original)
            decoded=decode_row(strict_loads(json.dumps(wire)));assert decoded==original
            assert legacy.o.expected_row(decoded)==legacy.o.expected_row(original);count+=1
    accepted=encode_comparison_for_test(legacy.row_for(2,32));rejected=[]
    def require_reject(name,value):
        try:decode_row(value)
        except (ValueError,TypeError):rejected.append(name)
        else:raise AssertionError('Malformed wire accepted: '+name)
    changes={
      'uppercase_hex':lambda r:r.__setitem__('initial_pad',r['initial_pad'].upper()),
      'short_camera':lambda r:r.__setitem__('initial_camera',r['initial_camera'][:-2]),
      'odd_matrix':lambda r:r.__setitem__('initial_matrix',r['initial_matrix']+'0'),
      'missing_vehicle':lambda r:r.pop('initial_vehicle'),
      'extra_field':lambda r:r.__setitem__('unexpected',0),
      'deduplicated_camera_global':lambda r:r['initial_globals'].pop(0),
      'legacy_array_in_wire':lambda r:r.__setitem__('initial_pad',[0]*0x134),
      'bool_as_integer':lambda r:r['original'].__setitem__('guards',1),
      'float_as_integer':lambda r:r.__setitem__('case',32.),
      'extra_FP_field':lambda r:r['candidate']['fp'].__setitem__('extra',0),
      'extra_trace_field':lambda r:r['model_trace'][0].__setitem__('extra',0),
    }
    for name,mutate in changes.items():
        value=copy.deepcopy(accepted);mutate(value);require_reject(name,value)
    for name,text in (('duplicate_json','{"event":"comparison","event":"error"}'),('nonfinite_json','{"value":NaN}')):
        try:strict_loads(text)
        except ValueError:rejected.append(name)
        else:raise AssertionError('Malformed JSON accepted')
    result=dict(synthetic_only=True,no_runtime_approval=True,canonical_parity_rows=count,same_oracle_rows=count,negative_codec_checks=rejected,legacy_semantic_rejections=7)
    (Path(__file__).resolve().parent/'transport-checks.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
