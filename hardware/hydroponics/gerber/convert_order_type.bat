set %n=hydroponics

del %n%.GTL
del %n%.GBL
del %n%.GTS
del %n%.GBS
del %n%.GTO
del %n%.GBO
del %n%.TXT
del %n%-NPTH.TXT
del %n%.GML
del %n%.zip

ren %n%-F_Cu.gbr %n%.GTL
ren %n%-B_Cu.gbr %n%.GBL
ren %n%-F_Mask.gbr %n%.GTS
ren %n%-B_Mask.gbr %n%.GBS
ren %n%-F_Silkscreen.gbr %n%.GTO
ren %n%-B_Silkscreen.gbr %n%.GBO
ren %n%-PTH.drl %n%.TXT
ren %n%-NPTH.drl %n%-NPTH.TXT
ren %n%-Edge_Cuts.gbr %n%.GML

7z a %n%.zip %n%.GTL %n%.GBL %n%.GTS %n%.GBS %n%.GTO %n%.GBO %n%.TXT %n%-NPTH.TXT %n%.GML