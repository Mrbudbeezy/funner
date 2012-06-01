-- ���������� ������� � �������� -------------------------------------------------------------------

local function Control_SetCaption (control, caption_text)
  if caption_text then
    if not(control.Caption) then
      --[[ ���� �� ������� ������ ��� ������ �������
      control.Caption=Scene.TextLine.Create()
      control.Caption:BindToParent(control.ForeNode)]]
      control.Caption=CreateMultiTextLine()
      control.Caption.Node:BindToParent(control.ForeNode)
    end
    control.Caption.SetText(caption_text)
  else
    if control.Caption then
      control.Caption.Node:Unbind()
    end
    control.Caption=nil
  end
end

UI.Control_SetCaption = Control_SetCaption


-- �������� �� XML ------------------------------------------------------------------------------------
function UI.Control_SetByXML (control, xml)
  --������������� �� XML ����� ��������� ��� ��������� ���� �����.
  --��������� ������������ ��� �������� �������� ����������� ����
  assert(xml, "ERROR: SetControlByXML(control, xml) - xml is nil")
  assert(control, "ERROR: SetControlByXML(control, xml) - control is nil")
  
  --��������� ����� � ���� ��������
  control.Name=xml.ReadStringByName ("Name")
    if  not(control.Name) then 
      control.Name="unnamed" 
    end
  control.ControlType=xml.ReadStringByName("Type")
  
  -- �������� ������ �������� ��������
  control.xreslist=xml.ReadStringByName("xreslist")
  
  -- ��������� ������� � ���������
  control.SetWidthAndHeight(xml.ReadNumbersByName("Size"))
  control.Node.Position= xml.ReadVec3ByName("XY")
  local scale =xml.TryReadVec3ByName("Scale")
     if scale then control.Node.Scale=vec3 (scale.x, scale.y, 1) end
  
  -- ��������� ����� �����������
  control.Rect.SpecialMaterial=xml.ReadStringByName("SpecialMaterial")
  control.StyleSetName=xml.ReadStringByName("StyleSetName")
    control.Styles=WindowEditor.GetStyleSetByName(control.StyleSetName)
  
  control.Help=xml.ReadStringByName("Help")

  control.ArrowSelectable=xml.ReadBooleanByName("ArrowSelectable")

  -- ��������� �������
  Control_SetCaption(control, xml.ReadStringByName("Caption"))
  control.CaptionPosition=xml.TryReadVec3ByName("CaptionPosition")
  control.CaptionRelativePosition=xml.TryReadVec3ByName("CaptionRelativePosition")
  control.CaptionAlignment=xml.TryReadVec3ByName("CaptionAlignment")
  control.CaptionMaxLength=xml.ReadNumberByName("CaptionMaxLength")
  -- ������ �������������� ������� �� ������� ����� ����������� ����
  local AdditionalEvents=xml.ReadStringByName("AdditionalEvents")
    if AdditionalEvents then 
      AdditionalEvents=UnpackString(AdditionalEvents,", ", true)
      print(control.Name," AdditionalEvents: ", unpack(AdditionalEvents))
    else
      AdditionalEvents={}
    end
  
  --�������� �������� ��������� ������� ��������
  local Events={"BeforeOpened",
                "Opened",
                "GotFocus",
                "LostFocus",
                "MouseMoved",
                "CharCode",
                "KeyDown",
                "Pressed",
                "Released",
                "Pressed_2",
                "Released_2",
                "Redraw",
                "Changed",
                "BeforeClosed",
                "Closed",
                unpack(AdditionalEvents)}
  LoadObjectEventScriptsFromXML(control, UIControlScript, xml, Events, control) --��� ������� �������� �������������� ���������� ��� ������������ ����� �������

  --�������� ������ ����������� �������
  xml.ForEachChildByName("Position", function (position_xml)
    local pos = position_xml.ReadVec3ByName("XY")
    control.Positions[position_xml.ReadStringByName("Name")]=pos
  end)
  
  --�������� ������ �������������� ��������
  control.SpriteAnimations={}
  control.SpritesAnimating=false
  xml.ForEachChildByName("Sprite", function (sprite_xml)
    local sprite, PeriodicCall = ReadSpriteFromXML(sprite_xml)
    if sprite then
      control.Sprites[sprite.Name]=sprite
      local hidden=sprite_xml.ReadBooleanByName("Hidden")
      if not(hidden) then
        sprite:BindToParent(control.ForeNode)
      end
      if PeriodicCall then
        control.SpriteAnimations[sprite]=PeriodicCall
        PeriodicCall.Pause()
      end
    end
  end)
end


-- �������� � �������� �������� ---------------------------------------------------------------------
function UI.Control_LoadResources(control)
  assert(control,"ERROR: LoadResourcesForControl(control) - control is nil")
  local res=control.xreslist
  if res then
    ResourceManager.Load(res)
  end
end

function UI.Control_UnloadResources(control)
  assert(control,"ERROR: UnloadResourcesForControl(control) - control is nil")
  local res=control.xreslist
  if res then
    ResourceManager.Unload(res)
  end
end

-- ��������� � ���������� �������� ��������� �������� --------------------------------------------------
function UI.Control_AnimateSprites(control, animate)
  assert(control,"ERROR: AnimateSpritesInControl(control, animate) - control is nil")
  if not(animate==false) then
    animate=true
  end
  -- ���� �������� �������� ��� ��� ����
  if animate==control.SpritesAnimating then
    return
  end
  -- ���� ��� - �������� ��� ��������� �������� �� ���������� 
  ForEachInTable(control.SpriteAnimations, function (PeriodicCall)
    if animate==false then
      PeriodicCall.Pause()
    else
      PeriodicCall.Resume()
    end
  end)
  control.SpritesAnimating=animate
end

function UI.Control_HideSprite(control, SpriteName)
  assert(control, "ERROR: UI.Control_HideSprite(control, SpriteName) - control is nil")
  assert(SpriteName, "ERROR: UI.Control_HideSprite(control, SpriteName) - SpriteName is nil")
  local sprite=control.Sprites[SpriteName]
  assert(sprite, "ERROR: UI.Control_HideSprite( "..control.Name..", "..SpriteName..") - sprite not found")
  sprite:Unbind()
end

function UI.Control_ShowSprite(control, SpriteName)
  assert(control, "ERROR: UI.Control_ShowSprite(control, SpriteName) - control is nil")
  assert(SpriteName, "ERROR: UI.Control_ShowSpritee(control, SpriteName) - SpriteName is nil")
  local sprite=control.Sprites[SpriteName]
  assert(sprite, "ERROR: UI.Control_ShowSprite( "..control.Name..", "..SpriteName..") - sprite not found")
  sprite:BindToParent(control.ForeNode)
end
  
-- ����������� --------------------------------------------------------------------------------------------
function UI.Control_Redraw (control)
  -- ����� �������� ����� ����� ��������� �����, ������� ��� �������
  assert(control.Style, "ERROR: "..control.Name.." .Redraw() - control style is nil")
  --print("RedrawControl: ",control.Name)
  if control.Style.Rect then
    -- ����������� ��������������
    control.Rect.Style=control.Style.Rect
  end
  control.Rect.Redraw()
  
  if control.Style.Text then
    -- ����������� �������
    if control.Caption then
      --��������� ������� �������
      if control.CaptionPosition then
        local pos=control.CaptionPosition
        control.Caption.Node:SetPosition( pos.x, pos.y, pos.z)
      else
        local pos=control.CaptionRelativePosition
        if not(pos) then
          pos=vec3( 0, 0, 0)
        end
        control.Caption.Node:SetPosition(control.Rect.Width*pos.x,-control.Rect.Height*pos.y, pos.z)
      end
      --��������� ������������ �������
      local Alignment=control.CaptionAlignment
      local AlignmentX=Scene.TextLineAlignment.Left
      local AlignmentY=Scene.TextLineAlignment.Top
      if control.CaptionAlignment then
        AlignmentX=SelectArgument((Alignment.x*2+1), Scene.TextLineAlignment.Left, Scene.TextLineAlignment.Center, Scene.TextLineAlignment.Right)
        AlignmentY=SelectArgument((Alignment.y*2+1), Scene.TextLineAlignment.Top, Scene.TextLineAlignment.Center, Scene.TextLineAlignment.Bottom)
      end
      control.Caption.SetAlignment( AlignmentX, AlignmentY)
      control.Caption.SetMaxTextLength(control.CaptionMaxLength)
      control.Style.Text.ApplyToMultiTextLine(control.Caption)
    end
  else
    control.Caption.Redraw()
  end
  
  control.HandleEvent("Redraw")
end

--��� ��������� ����� ---------------------------------------------------------------------------


function UI.Control_SetStyle(control, StyleName)
  assert(control, "ERROR: Control_SetStyle(control, StyleName) - control is nil")
  if StyleName then
    local style=control.Styles[StyleName]
    if style then
      control.Style=style
      -- ����������� ������ ���������
      control.Redraw()
      --����� �����
      if (style.Sound) and (PlaySoundEffect) then
        PlaySoundEffect(style.Sound)
      end
      return true
    end
  end
  return false
end

function UI.Control_SetStyles(control, Styles)
  control.Styles=Styles
  control.SetStyle(control.Styles.Default)
end

function UI.Control_SetSpecialMaterial(control, Material)
  control.Rect.SpecialMaterial=Material
end
