p='contrib/inventory_implementations.py'
s=open(p,encoding='utf-8').read()
B=chr(92)
old="re.search(r'' + re.escape(macro[1]) + r'', f['body'])"
new="re.search(r'"+B+"b' + re.escape(macro[1]) + r'"+B+"b', f['body'])"
assert old in s
open(p,'w',encoding='utf-8').write(s.replace(old,new))
