-- ������������� �������
local function InitGraphics ()
  -- �������� ������
  
  local screen = Scene.Screen.Create ()
  
  screen:DisableBackground ()
  
  -- �������� ������ � ���� � config.xml

  Engine.Screens.Register ("Screen1", screen)
  
  -- ���������� �� ����� ������� ������

  local viewport = Scene.Viewport.Create ()
  
  viewport.Technique       = "Render2d"
  viewport.ZOrder          = 0
  viewport.BackgroundColor = vec4 (0, 1, 0, 1)  

  viewport:SetArea (0, 0, 100, 100)
  viewport:EnableBackground ()

  screen:Attach (viewport)
  
  local methods = {}
  
  methods.SetCamera = function (camera) viewport.Camera = camera end
  
  return methods
end

-- ������������� �����  
local function InitInput () 
  local this = {}  
  
  local mouse_x = 0
  local mouse_y = 0

  -- �����������
  
  this.OnWindowClosed = function ()
    print ("Window closed")
    
    System.Application.Exit (0)
  end
  
  this.OnCursorX = function (value)      
    mouse_x = value
  end
  
  this.OnCursorY = function (value)
    mouse_y = value
  end  
  
  this.OnMouseDown = function () 
    print (string.format ("left button down (x=%g, y=%g)", mouse_x, mouse_y))
  end
  
  this.OnMouseUp = function ()
    print (string.format ("left button up (x=%g, y=%g)", mouse_x, mouse_y))
  end  
  
  this.OnCursorEnterWindow = function ()
    print ("cursor in window")
  end
  
  this.OnCursorLeaveWindow = function ()
    print ("cursor out of window")
  end  

  -- ����������� ����������� ������� �����. CreateEventHandler ��� ���������� ������������ ��� ������� ����� � ������
  
  local function EventsDispatch (command)
    -- ���������� �������
    
    local status, value = pcall (loadstring (string.format ("return function (this) %s end", command)))
    local fn
    
    if status then
      fn = value
    else
      print (string.format ("input error: %s", value))

      return
    end

    -- ������ �������

    status, value = pcall (fn, this)
    
    if not status then
      print (string.format ("input error: %s", value))      
    end
  end

  Engine.InputEventHandlers.Register ("game", Input.CreateEventHandler (EventsDispatch))  

  return {}
end

-- �������� ��������
local function InitResources ()
  local resource_group = Engine.ResourceManager.Group.Create ()
  
  resource_group:Add ("data/materials.xmtl")

  local resource_binding = Engine.ResourceManager.CreateBinding (resource_group)

  resource_binding:Load ()
  
  return {}
end

-- �������� �������� �����
local function InitScene (gfx)
  -- �������� ����� � ������

  local scene  = Scene.Scene.Create ()
  local camera = Scene.OrthoCamera.Create ()
  
  scene.Root.Scale = vec3 (1, -1, 1)
  
  camera.Left   = 0
  camera.Right  = 100
  camera.Top    = -100
  camera.Bottom = 0
  
  camera:BindToScene (scene)
  
  -- ��������� �������� ������
  
  gfx.SetCamera (camera)
  
  -- �������� �������
  
  local sprite = Scene.Sprite.Create ()
  
  sprite.Position = vec3 (50, 50, 0)
  sprite.Scale    = vec3 (50, 50, 1)
  sprite.Material = "material1"

  sprite:BindToScene (scene)  
  
  return {}
end

-- ������� �������
local function Main ()
  local gfx_system      = InitGraphics () -- ������ ��������, ����� ������� ������ �� ������ ��������� �������
  local input_system    = InitInput ()  
  local scene_system    = InitScene (gfx_system)
  local resource_system = InitResources ()
end

-- ����� Main
Main ()
