set "HFS=C:\Program Files\Side Effects Software\Houdini 15.5.632"
set "PATH=%HFS%\bin;%PATH%"

hcustom.exe -i ../../publish SOP_principalcurvature.C -I eigen -I libigl\include
rem add -g to compile with debug info

PAUSE
