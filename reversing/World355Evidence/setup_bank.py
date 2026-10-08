from pathlib import Path
p=Path('reversing/ProjectileCore355')
s=(p/'build_probe.py').read_text().replace("'source/game_sa/DecisionMakers/Decision.cpp', 'source/game_sa/DecisionMakers/Decision.h'","'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'").replace('decision-pending','projectile-core355').replace('DecisionPendingLive','ProjectileCore355Live');(p/'build_probe.py').write_text(s)
s=(p/'run_local.py').read_text().replace('decision-pending','projectile-core355').replace('Pendientes CDecision','ProjectileCore355');(p/'run_local.py').write_text(s)
