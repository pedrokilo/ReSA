p='contrib/inventory_implementations.py'
s=open(p,encoding='utf-8').read()
a=s.index("DEFINE = re.compile(")
b=s.index("\n",s.index("re.M)",a))
B=chr(92)
line="DEFINE = re.compile(r'^[ "+B+"t]*#[ "+B+"t]*define[ "+B+"t]+("+B+"w+)(?:[^"+B+"n]*"+B+B+B+"n)*[^"+B+"n]*', re.M)"
s=s[:a]+line+s[b:]
open(p,'w',encoding='utf-8').write(s)
