from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/read_damage_constants.py');s=p.read_text().replace('for addr in [','for addr in [0x871360,');s += '\nfor i in range(64):\n v=struct.unpack_from("<I",b,raw+0x871360-0x400000-va+i*4)[0]\n if v in [0x6B8EC0,0x6BEA10,0x6B89B0]: print(i,hex(v))\n';Path('reversing/VehicleAudio355Evidence/read_bike_slots.py').write_text(s)
