------------------------------------------------------------------------------------------------------------------------------------
-- ��������������� ������� ��� �������� ��������
------------------------------------------------------------------------------------------------------------------------------------
local function AddStyleChangingHandler(control, Event, StyleName)
  assert(control, "ERROR:  AddStyleChangingHandler(control, Event, StyleName) - control is nil")
  assert(Event, "ERROR: AddStyleChangingHandler(control, Event, StyleName) - Event is nil")
  local function ChangeStyleByEvent()
    control.SetStyle(StyleName)
  end
  control.RegisterEventHandler(Event, ChangeStyleByEvent)
end

------------------------------------------------------------------------------------------------------------------------------------
-- �������� ��������
------------------------------------------------------------------------------------------------------------------------------------


function CreateControl ()
  local control={}
  
  control.Name=name
  
  --- ����������� ��������� �������� -------------------------------------------------------
  
  control.Rect=CreateWinRect(10, 10)
  
  control.Node=control.Rect.Node
  
  control.ForeNode=Scene.Node.Create() --������������ ��� �������� ���� ���� ������ ��������
    control.ForeNode:BindToParent(control.Rect.Node)
    control.ForeNode:SetPosition(0,0,-1)
    control.ForeNode:SetScale(1,1, (1/128))
    
  --- ������� ��� �������� ------------------------------------------------------------------
  
  control.StyleSetName="Control"
  
  control.Styles={}
  
  control.Style=nil
    
  --- ��������� �������� --------------------------------------------------------------------
    
  control.ControlType="Control"
  
  control.ArrowSelectable=true
  
  control.ParentFocused=false
  
  control.Focused=false
  
  --- �������������� ������ -----------------------------------------------------------------
  
  control.Positions={}--������ ����������� �������
  
  control.Sprites={}  --������ ����������� ��������
  
  --------------------------------------------------------------------------------------------------
  -- ��������� �������
  --------------------------------------------------------------------------------------------------
  
  assert(AddEventManager~=nil, "ERROR: CreateControl () - AddEventManager() is nil")
  AddEventManager(control)
 
  --------------------------------------------------------------------------------------------------
  -- �������� �������
  --------------------------------------------------------------------------------------------------
            
  control.SetWidthAndHeight =control.Rect.SetWidthAndHeight
  control.Position          =control.Rect.Position
  control.GetCenterPosition    =control.Rect.GetCenterPosition
  control.WorldPosition     =control.Rect.WorldPosition
  control.BindToParent =control.Rect.BindToParent
  control.Unbind       =control.Rect.Unbind
  control.SetScale     =control.Rect.SetScale
  control.HasXY        =control.Rect.HasXY
  control.HasWorldXY   =control.Rect.HasWorldXY

  
  control.SetStyle=CreateMethodForObject(UI.Control_SetStyle, control)
  
  control.SetStyles=CreateMethodForObject(UI.Control_SetStyles, control)
  
  control.SetSpecialMaterial=CreateMethodForObject(UI.Control_SetSpecialMaterial, control)
  
  control.ShowSprite=CreateMethodForObject(UI.Control_ShowSprite, control)
  
  control.HideSprite=CreateMethodForObject(UI.Control_HideSprite, control)
  
  --------------------------------------------------------------------------------------------------
  -- ���������� �������
  --------------------------------------------------------------------------------------------------
  
  control.Redraw=CreateMethodForObject(UI.Control_Redraw, control)
  
  control.SetCaption=CreateMethodForObject(UI.Control_SetCaption, control)
  
  --------------------------------------------------------------------------------------------------
  -- ������ � XML (���������� � ��������)
  --------------------------------------------------------------------------------------------------  
  
  control.SetControlByXML=CreateMethodForObject(UI.Control_SetByXML, control)
  
  --------------------------------------------------------------------------------------------------
  -- ����������� �������
  --------------------------------------------------------------------------------------------------
  
  -- ��������� ����� �� �������� -----------------------------------------------------------
  AddStyleChangingHandler(control, "GotFocus", "Focused")
  AddStyleChangingHandler(control, "LostFocus", "Default")
  AddStyleChangingHandler(control, "ParentGotFocus", "Default")
  AddStyleChangingHandler(control, "ParentLostFocus","Default")
  AddStyleChangingHandler(control, "Pressed", "Pressed")
  AddStyleChangingHandler(control, "Released", "Focused")
  
  -- ��������� � ���������� �������� ��������� �������� -------------------------------------
  control.RegisterEventHandler("GotFocus", function () 
    control.Focused=true 
    UI.Control_AnimateSprites(control, true)
  end)
  
  control.RegisterEventHandler("LostFocus",function () 
    control.Focused=false 
    UI.Control_AnimateSprites(control, false)
  end)
  
  control.RegisterEventHandler("ParentGotFocus", function () 
    control.ParentFocused=true 
    UI.Control_AnimateSprites(control, true)
  end)
  
  control.RegisterEventHandler("ParentLostFocus",function () 
    control.ParentFocused=false 
    UI.Control_AnimateSprites(control, false)
  end)
  
  -- �������������� �������� � �������� �������� ---------------------------------------------
  control.RegisterEventHandler("BeforeOpened",UI.Control_LoadResources, control)
  control.RegisterEventHandler("Closed", UI.Control_UnloadResources, control )
  
  return control
end

