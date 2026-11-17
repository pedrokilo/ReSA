import json, sys
sys.path.insert(0, 'D:/Original/wv-6/reversing/Fn225A')
import fixture_oracle as o
path = sys.argv[1]; maxn = int(sys.argv[2]) if len(sys.argv) > 2 else 3
shown = 0
for line in open(path, encoding='utf-8'):
    if '"event":"comparison"' not in line: continue
    r = json.loads(line)
    if r['original'] != r['candidate']:
        if shown >= maxn: break
        shown += 1
        print('==', r['function'], 'caso', r['case'])
        for k in r['original']:
            if r['original'][k] != r['candidate'][k]:
                if k == 'observed':
                    for kk in r['original'][k]:
                        if r['original'][k][kk] != r['candidate'][k][kk]:
                            print(' observed.%s EXE=%s\n               DLL=%s' % (kk, str(r['original'][k][kk])[:700], str(r['candidate'][k][kk])[:700]))
                else:
                    print(' ', k, 'EXE=', r['original'][k], 'DLL=', r['candidate'][k])
        e = o.expected(r['input'])
        print(' oraculo marks', e['marks'])
        print(' bad', o.check_row(r))
