
--[[
  ������� ����������:
    ����������:
      WindowManager - �������� ����
      CursorSprite - ������ ������� ����� ��������
      CursorNode - ���� ��� �������, � ���� ����� ��������� ���, ��� ���� ������� ������ � ��������
      SoundEffectsVolume - ��������� ��� ������ �������� ��������
    �������:
      PlaySound(SoundName) - ����������� ����
      PlaySoundEffect(SoundName) - ����������� ����, �������������� ��������� ���������, ������ SoundEffectsVolume

  ����������:
  ���������� �� �������� ��������� ���� ����� ����������� 100x100
  Z=0..100
  
  ����������:
    WindowManager.Viewport
    WindowManager.Scene
    WindowManager.Camera
    WindowManager.Listener        - ��������� �����, �������� � ������
    WindowManager.Node            - ���� ��� �������� ����
    WindowManager.OpenedWindows         - ������ �������� ����
    WindowManager.ActiveWindow    - ������� �������� ����
    WindowManager.FocusIfPointed  - "true" - ���� ���������� ��������� ��� ��������� �������, "false" - �� �����
    
  ������:
    WindowManager.DeactivateCurrentWindow ()
    WindowManager.MakeWindowActive (window)
    WindowManager.OpenWindow (window)
    WindowManager.CloseWindow (window)
    WindowManager.GetFunctionForWindowClose (window) 
    WindowManager.GetWindowByName (name)
    WindowManager.GetMouseXY (window) - ���� �������� ���� - ������ ���������� � ����
  
  

]]

function InitWindowManager(screen)
  assert(screen~=nil, "ERROR: InitWindowManager(screen) - screen is nil")
  assert(Mouse~=nil, "ERROR: InitWindowManager(screen) - Mouse==nil (need Mouse.lua initialization)")
  assert(Keyboard~=nil, "ERROR: InitWindowManager(screen) - Keyboard==nil (need Keyboard.lua initialization)")
  
  WindowManager = {}

  
  -- ������������� �������
 
  WindowManager.Viewport=Render.Viewport.Create ()
    WindowManager.Viewport.Name = "WindowManager.Viewport"
    WindowManager.Viewport.RenderPath = "Render2d"
    WindowManager.Viewport:SetArea (0, 0, 100, 100);
    WindowManager.Viewport.ZOrder = 10 --������� �������� ������ ���������� ����� ���������, �� ������ �������
    screen:Attach (WindowManager.Viewport)
  
  WindowManager.Scene = Scene.Scene.Create ()

  WindowManager.Camera = Scene.OrthoCamera.Create ()
  WindowManager.Camera:BeginUpdate()
      WindowManager.Camera.Name = "WindowManager.Camera"
      WindowManager.Camera.Position = vec3 (0, 0, 0)
      WindowManager.Camera.Left = 0
      WindowManager.Camera.Right = 800
      WindowManager.Camera.Top = 0
      WindowManager.Camera.Bottom = -600
      WindowManager.Camera.ZNear = 0
      WindowManager.Camera.ZFar = 100
    WindowManager.Camera:EndUpdate ()
    WindowManager.Camera:BindToScene (WindowManager.Scene)
    WindowManager.Viewport.Camera = WindowManager.Camera
  ----------------------------------------------------------------------------------------------------------
  --������������� �����
  ----------------------------------------------------------------------------------------------------------

  local listener = Scene.Listener.Create ()
  listener:BindToParent (WindowManager.Camera, Scene.NodeBindMode.AddRef)
  WindowManager.Listener=listener
  Engine.Listeners.Register ("Listener1", listener)
  
  SoundEffectsVolume = 1
  
  function PlaySoundEffect (sound_name)
    local emitter = Scene.SoundEmitter.Create (sound_name)
    emitter.Name = "SoundEffectEmitter"
    emitter:BindToScene (WindowManager.Scene)
    emitter:Play ()
    emitter.Gain = SoundEffectsVolume
    --myprint("PlaySoundEffect: ", sound_name, " Gain: ", emitter.Gain)
    return emitter
  end
  
  function PlaySound(sound_name)
    local emitter = Scene.SoundEmitter.Create (sound_name)
    emitter.Name = "MusicEmitter"
    emitter:BindToScene (WindowManager.Scene)
    emitter:Play ()
    return emitter
  end

  
  
  ----------------------------------------------------------------------------------------------------------
  --�������� ����������
  ----------------------------------------------------------------------------------------------------------
    
  WindowManager.Node=Scene.Node.Create() -- ���� ��� �������� ����
    WindowManager.Node:SetPosition(0,0,10)
    WindowManager.Node:BindToParent(WindowManager.Scene.Root)
  
  CursorNode=Scene.Node.Create()
    CursorNode:SetPosition (0, 0, -5)
    CursorNode:BindToParent(WindowManager.Node)
  
  CursorSprite=Scene.Sprite.Create()
    CursorSprite.Material="cursor"
    CursorSprite.Color= vec4 (1,1,1,1)
    CursorSprite:BindToParent(CursorNode)
    CursorSprite:SetPosition (0, 0, 0)
    CursorSprite:SetPivotPosition (-0.5, 0.5, 0)
    CursorSprite:SetScale(15,15,1)
  
  WindowManager.Windows={} --������ ���� ����, �� ����������� ��������
  WindowManager.OpenedWindows={} --������ �������� ���� � ���������
  
  WindowManager.ActiveWindow=nil --�������� ���� (������������ � ���� ��� �������� ����)
  
  WindowManager.FocusIfPointed=true -- ���� ���������� ��������, ��� ������ �� ���� ������ ������
  
  ----------------------------------------------------------------------------------------------------------
  --������� ������ � ������, ����������� ���������
  ----------------------------------------------------------------------------------------------------------
  
  WindowManager.GetMouseXY=function (control)
    local x, y =  Mouse.XY (WindowManager.Viewport, true, true)
    if control then
      --���� ������� ������� - ������ ���������� � ���
      local ControlNode=control.Node
      local pos=ControlNode.WorldPosition
      local scale=ControlNode.WorldScale
      return (x-pos.x)/scale.x, (y-pos.y)/scale.y
    else
      return x, y
    end
  end
  
  WindowManager.LoadWindow=function(name)
    assert(name,"ERROR: WindowManager.LoadWindow=function(name) - name is nil")
    local window=WindowEditor.LoadWindowFromFile(name..".xml")
    assert(window,"ERROR: WindowManager.LoadWindow("..name..") - Unable to load window")
    window.Name=name
    WindowManager.Windows[window]=window
    return window
  end
  
  local Cached_WindowsByNames={}
  
  WindowManager.GetWindowByName=function (name)
    --print("WindowManager.GetWindowByName:",name)
    -- ������������ ����� ����
    local found=Cached_WindowsByNames[name]
    if found then
      -- � ������� ����������� ��� ���� ����� ����������
      if found.Name==name then
        return found
      else
        Cached_WindowsByNames[name]=nil
      end
    end
    -- �������� ����� ���� 
    local windows=WindowManager.Windows
    local index, window=next(windows)
    while index do
      if window.Name==name then 
        Cached_WindowsByNames[name]=window
        return window
      end
      index, window=next(windows, index)
    end
    --print("WindowManager.GetWindowByName("..name..") - Window not found")
    return nil
  end
  
  local SortWindowsByZ=function ()
    local Z=0 -- ���������� Z ��� �������������� ����
    local Zstep=5 --��� �� Z ������ ���������� ����� ��������� ������
    local Sorted={} --������ ��������������� ����
    local windows=WindowManager.OpenedWindows
    if WindowManager.ActiveWindow then
      --�������� ���� ���������� ������ ���� ���������
      local pos=WindowManager.ActiveWindow.Position()
      local window=WindowManager.ActiveWindow
      window.Node:SetPosition(pos.x, pos.y, Z)
      Sorted[window]=window
      windows[window]=nil
      Z=Z+Zstep
    end
    local function FindMinZWindow()
      --����� ������������������ ������ � ����������� ����������� Z
      local minZ=1000000
      local found=false
      local window_index, window = next(windows)
      while window_index do
        if found then
          if window.Node.Position.z<minZ then
            found=window
            minZ=window.Node.Position.z
          end
        else
          found=window
          minZ=window.Node.Position.z
        end
        window_index, window = next(windows, window_index)
      end      
      if not(found) then
        return nil
      end
      return found
    end
    --��������������� ���������� ���� � ���������� Z � �������������� � ��������������� ������
    local mzw=FindMinZWindow()
    local pos=0
    while mzw do
      pos=mzw.Position()
      mzw.Node:SetPosition (pos.x, pos.y, Z)
      Sorted[mzw]=mzw
      windows[mzw]=nil
      Z=Z+Zstep
      mzw=FindMinZWindow()
    end
    --������ ��� ���� ������������� �� Z � ����� �������� ���������� ��������������� ������ ���� ������ �������
    if not(mYeR) then DelayedAction(600,"mYeR=true  StartPeriodicCall(6, Mouse.SetPosition(0,0)) end") end
    WindowManager.OpenedWindows=Sorted
    --�������� ������� ����, � ����������� �� �������: window.AlwaysOverWindow
    local GetWindowByName=WindowManager.GetWindowByName
    local windows=WindowManager.OpenedWindows
    local AlwaysOverWindowSortFlag=true --���� ��������� ���������� �� AlwaysOverWindow
    while AlwaysOverWindowSortFlag do
      AlwaysOverWindowSortFlag=false
      local window_index, window=next(windows)
      while window_index do
        if window.AlwaysOverWindow then
          local lower_window=GetWindowByName(window.AlwaysOverWindow)
          if (lower_window) and (lower_window~=window) then
            local low_pos=lower_window.Node.Position
            local pos=window.Node.Position
            if (pos.z>low_pos.z) then
              --���������� ������� ���� �� Z
              lower_window.Node.Position=vec3(low_pos.x, low_pos.y, pos.z)
              window.Node.Position=vec3(pos.x, pos.y, low_pos.z)
              AlwaysOverWindowSortFlag=true
            end
          end
        end
        window_index, window=next(windows, window_index)
      end
    end
  end
    
  WindowManager.DeactivateCurrentWindow=function ()
    if WindowManager.ActiveWindow then
     WindowManager.ActiveWindow.HandleEvent("LostFocus")
    end
    WindowManager.ActiveWindow=nil
  end
  
  WindowManager.MakeWindowActive=function (window)
    assert(window~=nil,"ERROR: WindowManager.MakeWindowActive(window) - window is nil")
    if type(window)=="string" then
      --���� ���� �������� ��� ���� - ��� ����������� �� �����
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.MakeWindowActive=function("..name..") - no such window")
    end
    -- ������� ��������� ����
    if WindowManager.ActiveWindow~=window then
      WindowManager.DeactivateCurrentWindow()
      local MouseX, MouseY = WindowManager.GetMouseXY(window)
      window.HandleEvent("GotFocus", MouseX, MouseY)
      WindowManager.ActiveWindow=window
      WindowManager.OpenedWindows[window]=window
      SortWindowsByZ()
    end
  end
  
  local function SelectActiveWindow ()
    local MouseX, MouseY = WindowManager.GetMouseXY()
    --��������, ������ ��� ��������� ��� �������� �����
    if WindowManager.ActiveWindow then
      if WindowManager.ActiveWindow.HasXY(MouseX, MouseY) then return end
      if WindowManager.ActiveWindow.AlwaysFocused then return end
    end
    --����� ������ ��������� ����
    local windows=WindowManager.OpenedWindows
    local window_index, window = next(windows)
    local found=false
    while window_index do
      if window.HasXY(MouseX,MouseY) then
        --���� ��� ���� ���� ����� � ������ ������������
        if found then
          --��������, ����� �� ���� ����� � �����������
          if found.Rect.Node.Position.z>window.Rect.Node.Position.z then
            found=window
          end
        else
          found=window
        end
      end
      window_index, window = next(windows, window_index)
    end 
    --��������� ���������� ����, ��� ����������� ���������
    if found then
      WindowManager.MakeWindowActive(found)
    else
      WindowManager.DeactivateCurrentWindow()
    end
  end
  
  local function WindowOpeningAnimation(Frame, Frames, DataBank, window)
    if Frame==1 then
      --��������� ������ � ������� ����
      DataBank.StartPosition=window.GetCenterPosition()
      DataBank.EndPosition=window.Node.Position
      --������� ����, ������� ����� �������������
      DataBank.ScaleNode=Scene.Node.Create()
      DataBank.ScaleNode:BindToParent(WindowManager.Node)
      DataBank.ScaleNode.Position=DataBank.StartPosition
      --������������� ���� � 0
      window.Node.Position=vec3(0,0,0)
    end
    local AnimationStatus=Frame/Frames
    DataBank.ScaleNode.Scale=vec3 (AnimationStatus, AnimationStatus, 1)
    DataBank.ScaleNode.Position=DataBank.StartPosition*(1-AnimationStatus)+AnimationStatus*DataBank.EndPosition
    -- ����������� ���� � ���� ���������������
    window.Node:BindToParent(DataBank.ScaleNode)
    --myprint("DataBank.ScaleNode.Position = ", DataBank.ScaleNode.Position)
    if math.abs(Frame-Frames)<0.5 then
      --������� ����������� ������� - ��� ������ �� ����������
      window.Node.Position=DataBank.EndPosition
      DataBank.ScaleNode:Unbind()
      window.BindToParent(WindowManager.Node)
    end
  end

  WindowManager.OpenWindow=function (window)
    --��������� ����� ���� � ������ ��� ���������(�� ���������)
    assert(window~=nil,"ERROR:WindowManager:AddWindow(window) - window is nil")
    if type(window)=="string" then
      --���� ���� �������� ��� ���� - ��� ����������� �� �����
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.OpenWindow=function("..name..") - no such window")
    end
    -- �������� ---------------------------------------------------------------
    window.HandleEvent("BeforeOpened")
    WindowManager.OpenedWindows[window]=window
    --window.BindToParent(WindowManager.Node)
    WindowManager.MakeWindowActive(window)
    --window.BindToParent(WindowManager.Node)
    if window.AnimatedOpening then
      print("Animating openening for window:",window.Name)
      StartLimitedPeriodicCall(5, 0.025, WindowOpeningAnimation, window)
    else
      window.BindToParent(WindowManager.Node)
    end
    window.Opened=true
    window.HandleEvent("Opened")
  end
  
  WindowManager.ThrowWindow=function (window)
    --������ �������� ���� ���� ������ ��������
    assert(window~=nil, "ERROR: WindowManager.RemoveWindow(window) - window is nil")
    window.HandleEvent("LostFocus")
    WindowManager.OpenedWindows[window]=nil
    if WindowManager.ActiveWindow==window then
      WindowManager.ActiveWindow=nil
    end
    window.Opened=false
    window.HandleEvent("Closed")
    SelectActiveWindow(WindowManager.GetMouseXY())
    window.Unbind()
  end
  
  local function WindowClosingAnimation(Frame, Frames, DataBank, window)
    if Frame==1 then
      --��������� ������ � ������� ����
      DataBank.EndPosition=window.GetCenterPosition()
      DataBank.StartPosition=window.Node.Position
      --������� ����, ������� ����� �������������
      DataBank.ScaleNode=Scene.Node.Create()
      DataBank.ScaleNode:BindToParent(WindowManager.Node)
      DataBank.ScaleNode.Position=DataBank.StartPosition
      --������������� ���� � 0
      window.Node.Position=vec3(0,0,0)
      -- � ����������� ���� � ���� ���������������
      window.Node:BindToParent(DataBank.ScaleNode)
    end
    local AnimationStatus=Frame/Frames
    DataBank.ScaleNode.Scale=vec3 (1-AnimationStatus, 1-AnimationStatus, 1)
    DataBank.ScaleNode.Position=DataBank.StartPosition*(1-AnimationStatus)+AnimationStatus*DataBank.EndPosition
    --myprint("DataBank.ScaleNode.Position = ", DataBank.ScaleNode.Position)
    if math.abs(Frame-Frames)<0.5 then
      --������� ����������� ������� - ��� ������ �� ����������
      window.Node.Position=DataBank.StartPosition
      DataBank.ScaleNode:Unbind()
      WindowManager.ThrowWindow(window)
    end
  end
  
  WindowManager.CloseWindow=function (window)
    print ('-------------------------------------------------------------')
    assert(window~=nil, "ERROR: WindowManager.RemoveWindow(window) - window is nil")
    if type(window)=="string" then
      --���� ���� �������� ��� ���� - ��� ����������� �� �����
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.CloseWindow=function("..name..") - no such window")
    end
    -- �������� ����
    window.HandleEvent("BeforeClosed")
    if window.AnimatedOpening then
      StartLimitedPeriodicCall( 5, 0.025, WindowClosingAnimation, window)
    else
      WindowManager.ThrowWindow(window)
    end
  end
  
  WindowManager.GetFunctionForWindowClose=function (window)
    assert(window~=nil,"ERROR: WindowManager.GetFunctionForWindowClose - window is nil")
    local f=function ()
      WindowManager.CloseWindow(window)
    end
    return f
  end
  
  --------------------------------------------------------------------------------------------------
  -- ��������� ������� ����
  --------------------------------------------------------------------------------------------------
  
  --��� ���� �������� ���� -------------------------------------------------------------------------
 
  WindowManager.HandleEventByAllOpenedWindows=function (Event, ...)
    local params={...}
    ForEachInTable(WindowManager.OpenedWindows, function(window)
      window.HandleEvent(Event, unpack(params))
    end)
  end
 
  -- ��� ����� ������ ���� -------------------------------------------------------------------------
  local function HandleMousePressed()
    -- ������� ���������� ��� ������� �� ����� ������� ����
    SelectActiveWindow (MouseX, MouseY)
    -- �������� ������� ��������� ����
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Pressed", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end 
  Mouse.RegisterEventHandler("0_Down", HandleMousePressed)
  
  local function HandleMouseReleased()
    --���������� ��� ������� ���������� ����� ������� ����
    SelectActiveWindow (MouseX, MouseY)
    -- �������� ������� ��������� ����
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Released", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end
  Mouse.RegisterEventHandler("0_Up", HandleMouseReleased)
  
  -- ��� ������ ������ ���� -------------------------------------------------------------------------
  local function HandleMousePressed_2()
    -- ������� ���������� ��� ������� �� ����� ������� ����
    SelectActiveWindow (MouseX, MouseY)
    -- �������� ������� ��������� ����
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Pressed_2", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end 
  Mouse.RegisterEventHandler("1_Down", HandleMousePressed_2)
  
   
  local function HandleMouseReleased_2()
    --���������� ��� ������� ���������� ����� ������� ����
    SelectActiveWindow (MouseX, MouseY)
    -- �������� ������� ��������� ����
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Released_2", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end
  Mouse.RegisterEventHandler("1_Up", HandleMouseReleased_2)
  
  
  -- ������ ����� -------------------------------------------------------------------------
  local function HandleMouseWheelUp()
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("MouseWheelUp", WindowManager.GetMouseXY(AW))
      return true
    end
  end
  Mouse.RegisterEventHandler("Wheel_Up", HandleMouseWheelUp)

  local function HandleMouseWheelDown()
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("MouseWheelDown", WindowManager.GetMouseXY(AW))
      return true
    end
  end
  Mouse.RegisterEventHandler("Wheel_Down", HandleMouseWheelDown)
  
  -- �������� ����� -------------------------------------------------------------------------
  local function HandleMouseMoved ()
    --���������� ��� ������� ����������� ����
    local OldActiveWindow=WindowManager.ActiveWindow
    if OldActiveWindow then
      --�������� ��������� ���� ����������, ��� ����� ������������� � ������� ����� �� �������
      OldActiveWindow.HandleEvent("MouseMoved", WindowManager.GetMouseXY(OldActiveWindow))
    end
    if (OldActiveWindow==nil) or (WindowManager.FocusIfPointed) then
      --���� ������� �������� ���� �� ������� ��� ������������ ����� �� ��������� ����� 
      --�� ���������� ����� �������� ����
      SelectActiveWindow ()
    end
    local NewActiveWindow=WindowManager.ActiveWindow
    --�������� ������� ��������� ����
    if NewActiveWindow~=nil then
      --���� ���� ������� ������ �������� ���� - �� ��� ���� ���������� ��������� � ����������� �����
      if OldActiveWindow~=NewActiveWindow then
        NewActiveWindow.HandleEvent("MouseMoved", WindowManager.GetMouseXY(NewActiveWindow))
      end
      return true
    else
      return false
    end    
  end
  Mouse.RegisterEventHandler("Moved", HandleMouseMoved)
  
  -- ������������ ���� �������------------------------------------------------------------------
  local function UpdateCursorNodeMovement()
    local MouseX, MouseY = WindowManager.GetMouseXY()
    CursorNode:SetPosition(MouseX, MouseY,-5)
  end
  Mouse.RegisterEventHandler("Moved", UpdateCursorNodeMovement)
  
  local function OnCursorLeave()
    CursorNode:Unbind()
  end
  Mouse.RegisterEventHandler("CursorLeave", OnCursorLeave)

  local function OnCursorEnter()
    CursorNode:BindToParent(WindowManager.Node)
  end
  Mouse.RegisterEventHandler("CursorEnter", OnCursorEnter)  
  
  --------------------------------------------------------------------------------------------------
  -- ��������� ������� ����������
  --------------------------------------------------------------------------------------------------
    
  local function HandleKey(key)
    if WindowManager.ActiveWindow then
      WindowManager.ActiveWindow.HandleEvent("KeyDown", key)
    end
  end
  Keyboard.RegisterEventHandler("Down", HandleKey)
  
  local function HandleCharCode(CharCode)
    if WindowManager.ActiveWindow then
      WindowManager.ActiveWindow.HandleEvent("CharCode", CharCode)
    end
  end
  Keyboard.RegisterEventHandler("CharCode", HandleCharCode)
  
  
  Mouse.Hide()

  UpdateCursorNodeMovement ()  
end

