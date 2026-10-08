from pathlib import Path
p=Path('reversing/PlaneGeneration355/LiveComparison.hpp');s=p.read_text().replace('int16(e->m_TaskId)','int16(static_cast<eTaskType>(e->m_TaskId))').replace('int16(e->m_FacialExpressionType)','int16(static_cast<eFacialExpression>(e->m_FacialExpressionType))');p.write_text(s)
p=Path('reversing/PlaneGeneration355/build_probe.py');s=p.read_text();ns={};exec(s[:s.index("if __name__")],{'__file__':str(p.resolve()),'__name__':'dependency_check'},ns);print('missing:',[x for x in ns['SOURCES'] if not Path(x).is_file()])
