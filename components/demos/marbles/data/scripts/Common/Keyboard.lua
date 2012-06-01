--[[
������� ���������� ������ "Keyboard"


��������� ������: ---------------------------------------------------------------

Keyboard.RegisterEventHandler(event, handler) - ����������� ����������� �������

Keyboard.UnregisterEventHandler(event, handler) - ������ �����������

]]


function InitKeyboard()
  Keyboard={}
    
  --------------------------------------------------------------------------------------------------
  -- ��������� �������
  --------------------------------------------------------------------------------------------------
  
  local handler=Input.CreateEventHandler()
  Engine.InputEventHandlers.Register("Input", handler)
  
  assert(AddEventManager~=nil, "ERROR: InitKeyboard() - AddEventManager() is nil")
  AddEventManager(Keyboard)
  
end

