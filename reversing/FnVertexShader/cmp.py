import sys,subprocess
ref=open(sys.argv[1],encoding='latin1',newline='').read()
inp=open(sys.argv[1]+".in").read()
out=subprocess.run(["./harness.exe"],input=inp.encode(),capture_output=True).stdout.decode('latin1')
A=ref.split("@@\n"); B=out.replace("\r\n","\n").split("@@\n")
print(len(A),len(B))
bad=0
for a,b in zip(A,B):
    if a!=b:
        bad+=1
        if bad<=3:
            import difflib
            print("".join(difflib.unified_diff(a.splitlines(1),b.splitlines(1),"exe","cpp")))
print("diferencias:",bad)
