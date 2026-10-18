import re,subprocess
s=open('/tmp/x87_stash.cpp',encoding='utf-8').read() if False else subprocess.check_output(['git','show','stash@{0}:source/game_sa/X87Math.cpp']).decode('utf-8')
i=s.index('// --- Lote de veintiocho')
tail=s[i:]
def block(name):
    m=re.search(r'(//[^\n]*\n)*__declspec\(noinline\)[^\n]*\b'+name+r'\(.*?\n\}\n',tail,re.S)
    return m.group(0)
head=tail[:tail.index('__declspec')]
out=head+''.join('\n'+block(n) for n in ['UnsignedTimesFloatCpp','SecondsToMsCpp','BytesToMsCpp','WaveCursorBytesCpp','SmoothFadeVolumeCpp'])
out=out.replace('Lote de veintiocho archivos','Lote de seis archivos de audio')
open('source/game_sa/X87Math.cpp','a',encoding='utf-8',newline='').write('\n'+out if not out.startswith('\n') else out)
h='''
// Lote de seis archivos de audio (12/10/2026).
float __cdecl UnsignedTimesFloatCpp(unsigned int value, float factor);
long __cdecl SecondsToMsCpp(double seconds);
long __cdecl BytesToMsCpp(unsigned int bytes, unsigned int bytesPerSecond);
int __cdecl WaveCursorBytesCpp(unsigned int position, unsigned int bytesPerSecond);
float __cdecl SmoothFadeVolumeCpp(float diff, float start, unsigned int elapsed, int fadeTime);
'''
open('source/game_sa/X87Math.h','a',encoding='utf-8',newline='').write(h)
