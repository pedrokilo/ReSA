from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text();s=s.replace("'PlaneBlow355']","'PlaneBlow355','PlaneDamage355']");p.write_text(s)
p=Path('reversing/VehicleAudio355Evidence/asm-coverage-local.json');s=p.read_text().replace('0x6B9250','0x6B9250');p.write_text(s)
