"""Primer caso con diferencias de un comparisons.jsonl del banco: python firstdiff.py <carpeta>"""
import json, sys
n = 0
for line in open(sys.argv[1] + '/comparisons.jsonl', encoding='utf-8'):
    r = json.loads(line)
    if r['event'] == 'comparison':
        n += 1
        if not r['equal']:
            o, c = r['original'], r['candidate']
            print('case', r['case'], 'family', r['input']['family'])
            for k in o:
                if o[k] != c[k]:
                    if k == 'observed':
                        for kk in o[k]:
                            if o[k][kk] != c[k][kk]:
                                a, b = o[k][kk], c[k][kk]
                                if kk == 'events':
                                    for i, (x, y) in enumerate(zip(a, b)):
                                        if x != y:
                                            print(' ev', i, x, '|', y)
                                            break
                                    print(' nev', len(a), len(b))
                                else:
                                    print(' obs', kk, str(a)[:400], '|', str(b)[:400])
                    else:
                        print(' ', k, o[k], c[k])
            print('fp', r['fp_difference'], 'sites', r['sites'][:30])
            break
print('rows', n)
