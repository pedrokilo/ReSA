"""Read-only lexical classification; never admits conversions."""
import csv,json,re,hashlib
from pathlib import Path
from collections import Counter,defaultdict
ROOT=Path(__file__).resolve().parents[2]
OUT=Path(__file__).resolve().parent
LITERALS=re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'')
TOKENS=re.compile(r'\b(?:__asm|_asm|asm)\b|__declspec\s*\(\s*naked\s*\)')
def body_end(code,start):
 depth=1;i=start+1
 while i<len(code) and depth:
  depth+=(code[i]=='{')-(code[i]=='}');i+=1
 return i

def classify(text):
 lower=text.lower(); words=re.findall(r'\b[a-z][a-z0-9]*\b',lower)
 calls=bool(re.search(r'\bcall\b',lower)); fp=bool(re.search(r'\bf(?:ld|stp?|addp?|subr?p?|mulp?|divr?p?|sin|cos|sqrt|patan|yl2x|2xm1|scale|comp?p?|ucomp?p?|nstsw|ild|istp?)\b',lower))
 if re.search(r'push\s+(?:0x[0-9a-f]+|0*[0-9a-f]{5,}h)\s*\}?\s*(?:__asm\s*\{\s*)?ret',lower) and not fp:
  return 'absolute_game_or_crt_delegate'
 if 'fs:' in lower or 'exceptionhandler' in lower or 'exceptioninfo' in lower:return 'seh_or_unwind_special_abi'
 if not calls and ('fsin' in words or 'fcos' in words) and not any(x in words for x in ['fpatan','fyl2x','f2xm1','fsqrt','fcomp','fcompp','fucompp']):
  if any(x in words for x in ['fadd','faddp','fsub','fsubp','fsubr','fsubrp','fdiv','fdivp','fdivr','fdivrp','fstp']):return 'sin_cos_composite_or_store'
  return 'sin_cos_simple_or_mul'
 if 'fsqrt' in words:return 'fsqrt_with_context'
 if 'fild' in words or 'fistp' in words or 'fist' in words:return 'integer_float_conversion_context'
 if any(x in words for x in ['fpatan','fyl2x','f2xm1','fscale']):return 'other_transcendental_context'
 if fp:return 'other_x87_arithmetic_or_compare'
 if not calls and re.search(r'\bmov\s+eax\s*,\s*(?:0x[0-9a-f]+|[0-9a-f]+h|\d+)\b',lower) and len(words)<20:return 'constant_return_candidate'
 if calls and len(words)<70:return 'caller_register_bridge_candidate'
 if len(words)>250:return 'large_native_body_integer_or_mixed'
 return 'integer_control_or_register_contract'

rows=[];snapshots={}
for path in sorted((ROOT/'source/game_sa').rglob('*')):
 if path.suffix not in {'.cpp','.h','.hpp','.inc'} or not path.is_file():continue
 raw=path.read_bytes();text=raw.decode('utf-8-sig',errors='replace');code=LITERALS.sub(lambda m: ''.join('\n' if c=='\n' else ' ' for c in m[0]),text)
 matches=list(TOKENS.finditer(code))
 if not matches:continue
 file=path.relative_to(ROOT).as_posix();snapshots[file]=hashlib.sha256(raw).hexdigest()
 naked=[]
 for match in matches:
  if match[0].startswith('__declspec'):
   start=code.find('{',match.end());semi=code.find(';',match.end())
   if start>=0 and (semi<0 or start<semi):naked.append((match.start(),start,body_end(code,start)))
 for match in matches:
  kind='naked' if match[0].startswith('__declspec') else 'inline_assembly';owner=next((r for r in naked if r[0]<=match.start()<r[2]),None)
  if owner:
   begin,start,end=owner;context=code[start:end];signature=' '.join(code[begin:start].split());unit=file+':'+str(code.count('\n',0,begin)+1)
  else:
   start=match.end()
   while start<len(code) and code[start].isspace():start+=1
   end=body_end(code,start) if start<len(code) and code[start]=='{' else code.find('\n',start)
   if end<0:end=len(code)
   context=code[start:end];signature='non-naked asm block';unit=file+':block:'+str(code.count('\n',0,match.start())+1)
  rows.append(dict(file=file,line=code.count('\n',0,match.start())+1,kind=kind,family=classify(context),unit=unit,signature=signature,context_lines=context.count('\n')+1,is_large_native_context=len(re.findall(r'\b[a-zA-Z][a-zA-Z0-9]*\b',context))>250,context_sha256=hashlib.sha256(context.encode()).hexdigest(),context=context))
counts=Counter(r['family'] for r in rows);bykind={family:dict(Counter(r['kind'] for r in rows if r['family']==family)) for family in sorted(counts)}
units={r['unit']:r for r in rows};uc=Counter(r['family'] for r in units.values())
historical=list(csv.DictReader((ROOT/'reversing/CppOnlyMigration/inventory.csv').open(encoding='utf-8')))
now={(r['file'],r['line'],r['kind']) for r in rows};before={(r['file'],int(r['line']),r['kind']) for r in historical}
result=dict(scope='Live source lexical occurrences; naked body shared across its asm tokens, non-naked blocks classified separately. Not function counts or approval.',lexical_counts=dict(Counter(r['kind'] for r in rows)),family_occurrences=dict(counts),family_by_kind=bykind,context_units=dict(uc),historical_inventory_rows=len(historical),live_added_rows=len(now-before),live_removed_rows=len(before-now),source_hashes=snapshots,entries=rows)
(OUT/'inventory-family.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:v for k,v in result.items() if k not in ['entries','source_hashes']},ensure_ascii=False,indent=2))
