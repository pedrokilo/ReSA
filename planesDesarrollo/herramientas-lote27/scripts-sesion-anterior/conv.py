import re
def sub(path, start_pat, end_pat, repl, extra_include=True):
    s=open(path,encoding='utf-8',newline='').read()
    nl='\r\n' if '\r\n' in s else '\n'
    s=s.replace('\r\n','\n')
    a=s.index(start_pat); b=s.index(end_pat,a)+len(end_pat)
    s=s[:a]+repl+s[b:]
    open(path,'w',encoding='utf-8',newline='').write(s.replace('\n',nl))

p='source/game_sa/Audio/AEAudioEnvironment.cpp'
# first block
sub(p,'            uint8 deltaXExt[10], deltaYExt[10], deltaZValue[10];\n            __asm {','                fstp cameraOffset\n            }\n',
'''            const float dx = playerPos->x - center->x, dy = playerPos->y - center->y, dz = playerPos->z - center->z;
            cameraOffset = std::sqrt(dx * dx + dy * dy + dz * dz) - half;
''')
s=open(p,encoding='utf-8',newline='').read()
nl='\r\n' if '\r\n' in s else '\n'
s=s.replace('\r\n','\n')
a=s.index('    // Preserve the original x87 intermediates: Y remains extended')
b=s.index('        fstp dword ptr [edx + 8]\n    }\n',a)+len('        fstp dword ptr [edx + 8]\n    }\n')
new='''    // Mismo orden de operaciones que el EXE (x87 a 24 bits: cada operación se redondea a float).
    const auto* m = reinterpret_cast<const float*>(matrix);
    const float dx = pos->x - (m[4] * cameraOffset + center->x);
    const float dy = pos->y - (m[5] * cameraOffset + center->y);
    const float dz = pos->z - (m[6] * cameraOffset + center->z);
    output->x = -((dy * m[1] + dz * m[2]) + dx * m[0]);
    output->y = (dx * m[4] + dy * m[5]) + dz * m[6];
    output->z = (dx * m[8] + dy * m[9]) + dz * m[10];
'''
s=s[:a]+new+s[b:]
open(p,'w',encoding='utf-8',newline='').write(s.replace('\n',nl))

p='source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp'
sub(p,'                const float sandstorm = CWeather::Sandstorm, decibels = 20.f, dbOffset = 6.f;\n                __asm {','                    fstp volume\n                }\n',
'''                volume = AudioLog10Times20Cpp(CWeather::Sandstorm) - 6.f;
''')
sub(p,'        const float scale = 0.5714285969734192f;\n        float attenuationDistance;\n        __asm {','            fstp attenuationDistance\n        }\n',
'''        const float scale = 0.5714285969734192f;
        const float dz = cameraZ - waterZ;
        const float attenuationDistance = std::sqrt(distance * distance + dz * dz) * scale;
''')
