from pathlib import Path
import re,json
evidence=Path('reversing/VehicleAudio355Evidence/Bike_ControlFull.txt')
instructions=[]
for line in evidence.read_text().splitlines():
 m=re.fullmatch(r'([0-9A-F]{8}): (.+)',line)
 if m and 0x6B9250<=int(m[1],16)<=0x6BC92C:instructions.append((int(m[1],16),m[2]))
addresses={address for address,_ in instructions}
assert instructions[0][0]==0x6B9250 and instructions[-1]==(0x6BC92C,'ret')
assert len(addresses)==len(instructions)
calls=sorted({int(m[1],16) for _,instruction in instructions if (m:=re.fullmatch(r'call ([0-9A-F]{8})',instruction))})
assert all(not 0x6B9250<=address<=0x6BC92C for address in calls)
targets={int(m[1],16) for _,instruction in instructions if (m:=re.fullmatch(r'j\w+ ([0-9A-F]{8})',instruction))}
assert targets<=addresses
out=['// Native transcription from complete canonical ASM; all internal branches relocated.',
 '// The original SEH handler is a canonical dependency. Exceptional unwinding is not certified.',
 'static const uintptr_t BikeControl355Calls[] = {']
out += [f'    0x{address:08X}, // dependency {index}' for index,address in enumerate(calls)]
out += ['};','__declspec(naked) void CBike::ProcessControl() {','    __asm {']
for address,instruction in instructions:
 if address in targets:out.append(f'    bike_control_{address:08X}:')
 if m:=re.fullmatch(r'call ([0-9A-F]{8})',instruction):
  instruction=f'call dword ptr [BikeControl355Calls+{calls.index(int(m[1],16))*4}]'
 elif m:=re.fullmatch(r'(j\w+) ([0-9A-F]{8})',instruction):
  instruction=f'{m[1]} bike_control_{int(m[2],16):08X}'
 out.append('        '+instruction)
out+=['    }','}']
Path('reversing/VehicleAudio355Evidence/Bike_ControlNativeDraft.txt').write_text('\n'.join(out)+'\n')
Path('reversing/VehicleAudio355Evidence/bike-control-dependencies.json').write_text(json.dumps([{'index':index,'address':f'0x{address:08X}'} for index,address in enumerate(calls)],indent=2))
print(len(instructions),'native instructions',len(targets),'relocated labels',len(calls),'external dependencies')
