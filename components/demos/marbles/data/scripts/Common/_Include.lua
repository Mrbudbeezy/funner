CommonScriptsFolder="data/scripts/Common"

-- ����������� ����������� ������
dofile(CommonScriptsFolder.."/assert.lua")
dofile(CommonScriptsFolder.."/ConvertToFunction.lua")
dofile(CommonScriptsFolder.."/CreateMethodForObject.lua")
dofile(CommonScriptsFolder.."/TempGlobals.lua")
dofile(CommonScriptsFolder.."/GetScriptByName.lua")
dofile(CommonScriptsFolder.."/ResourceManager.lua")
      InitResourceManager()
--����������� ����� c �������� ���������
dofile(CommonScriptsFolder.."/String/_Include.lua")
dofile(CommonScriptsFolder.."/Draw/_Include.lua")
dofile(CommonScriptsFolder.."/Table/_Include.lua")
dofile(CommonScriptsFolder.."/Timing/_Include.lua")
dofile(CommonScriptsFolder.."/XML/_Include.lua")

--����������� ��������� ������
dofile(CommonScriptsFolder.."/AddEventManager.lua")
dofile(CommonScriptsFolder.."/Mouse.lua")
      InitMouse()
dofile(CommonScriptsFolder.."/Keyboard.lua")
      InitKeyboard()
dofile(CommonScriptsFolder.."/GameSettings.lua")
dofile(CommonScriptsFolder.."/List.lua")
