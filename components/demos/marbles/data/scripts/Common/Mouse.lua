--[[
������� ���������� ������ "Mouse"
� ����� ������������ ����� ���������� ��� ������� "cursor"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
��������� ������:

Mouse.RegisterEventHandler(event, handler) - ����������� ����������� ������� �����

Mouse.UnregisterEventHandler(event, handler) - ������ �����������

Mouse.XY (ViewPort, UseCameraView) 
  ��������� ��������� ����. �� ��������� ������������ ���������� � ����.
  ���� ������� ViewPort - ���������� �� �������� � ��������� 0..100. ���� ������ �� ��������� - ���������� ��� ���
  ���� UseCameraVieW==true - ������������ ���������� � ������ ���������� ��������������� ������ �����
  
Mouse.SetXY (x, y, ViewPort, UseCameraView)
  ��������� ��������� ����.
  ���� ������� ViewPort - ��������������� � �����
  ���� UseCameraView==true - ��������������� � ����������� ��������������� ������ ����� �����
  
]]

--[[
  --���� ��� �������� ���������� ��������� ����� ������� �� ��������� �����
  --� ����� ������ ���� ����� ����������� ������ ����������
  
  function ChangeMouseDriving()
    if Mouse.DrivedByInput then
      Mouse.DrivedByInput=false
      print("-- Mouse: OFF")
    else
      Mouse.DrivedByInput=true
      print("-- Mouse: ON")
    end
    DelayedAction(2, ChangeMouseDriving)
  end
  
  ChangeMouseDriving()
]]


function InitMouse()
  Mouse={}
    
  --------------------------------------------------------------------------------------------------
  -- ��������� �������
  --------------------------------------------------------------------------------------------------
  
  local handler=Input.CreateEventHandler()
  Engine.InputEventHandlers.Register("Input", handler)

  assert(AddEventManager~=nil, "ERROR: InitMouse() - AddEventManager() is nil")
  AddEventManager(Mouse)
  
  local MovementTimer=Common.Timer.Create() --���� ������ �����, ����� ������� � ���������� �� �������������� ������� �����
        MovementTimer:Reset()
  
  Mouse.DrivedByInput=true --���� ����� ����������� ����� ���������, ����������� �� ���������� �����
  
  Mouse.HandleInputEvent=function ( Event, ...)
    if (Event=="Moved") then
      if (MovementTimer.SecondsEllapsed<0.01) then
        return
      else
        MovementTimer:Reset()
      end
    end
    if Mouse.DrivedByInput then
      Mouse.HandleEvent( Event, ...)
    end
  end
  
  
  --------------------------------------------------------------------------------------------------
  -- �������� �������
  --------------------------------------------------------------------------------------------------
  
  local cursor=Input.Cursor.Create()
  Engine.Cursors.Register("cursor", cursor)  
  
  Mouse.XY=function (ViewPort, UseCameraView, InWorldCoordinates)
    local Pos=cursor.Position
    if ViewPort==nil then
      --������ �������� ��������� ����
      return Pos.x, Pos.y
    else
      --������ ��������� ���� ������ ��������
      local x, y = Pos.x*100, Pos.y*100
      local area=ViewPort.Area
      x=x-area.Left
      y=y-area.Top
      x=x/area.Width
      y=y/area.Height
      if UseCameraView then
        --������ ��������� ���� ������ ������
        if ViewPort.Camera==nil then 
          print("ViewPort.Camera==nil")
          return
        end
        local camera=ViewPort.Camera
        x=x*(camera.Right-camera.Left)+camera.Left
        y=y*(camera.Bottom-camera.Top)+camera.Top
        if InWorldCoordinates then
          --������ ������� ��������� ����
          x=x+camera.WorldPosition.x
          y=y+camera.WorldPosition.y
        end
      end
      return x, y
    end
  end

  
  
  Mouse.SetXY=function (x, y, ViewPort, UseCameraView)
    if ViewPort==nil then
      --��������� �������� ��������� ����
      cursor:SetPosition (x, y)
    else
      if UseCameraView then
        --������ ������� ��������� � ������ ���������� ������ ��������
        assert(ViewPort.Camera~=nil,"Mouse.SetXY (x, y, ViewPort, UseCameraView) - no Camera to use it")
        x=(x-ViewPort.Camera.Left)/(ViewPort.Camera.Right-ViewPort.Camera.Left)
        y=(y-ViewPort.Camera.Top)/(ViewPort.Camera.Bottom-ViewPort.Camera.Top)
      end
      --������ ������� ��������� �� ������ � ������ ��������� �������� �� ���
      x=(x*ViewPort.Area.Width+ViewPort.Area.Left)*0.01
      y=(y*ViewPort.Area.Height+ViewPort.Area.Top)*0.01
      cursor:SetPosition (x, y)
    end
    Mouse.HandleEvent('Moved')
  end

  
  
  Mouse.Hide=function ()
    cursor:Hide()
  end
  
  Mouse.Show=function ()
    cursor:Show()
  end
  
  --------------------------------------------------------------------------------------------------
  -- �������������� ����
  --------------------------------------------------------------------------------------------------
  
  Mouse.MoveToPosition=function (x,y, MovementTime)
    --���������� ������ ���� � �������� ������� �� �������� �����
    assert(MovementTime, "ERROR: Mouse.MoveToPosition(x,y,MovementTime) - Delay is nil")
    local startX,startY=Mouse.XY()
    local Movement={}
    local MovementTimer=Common.Timer.Create()
    local function MoveMouse()
      local t=MovementTimer.SecondsEllapsed/MovementTime
      if t>=1 then
        Mouse.SetXY(x,y)
        return false
      end
      Mouse.SetXY(startX*(1-t)+x*t, startY*(1-t)+y*t)
    end
    Movement=StartPeriodicCall(0.04, MoveMouse)
  end
  
end

