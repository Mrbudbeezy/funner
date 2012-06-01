--[[
������������� � �������� ������������ ��� �������� ����, ������, ��� ������ ����������� 
����������� ��������� ����������.
�������� ������:
.Update() - ��������� ������� ���, ���� �� ��� ������� (� ������� ���������� �������������)
.SetStyle(style) - ������������� ����� �����
.BindToParent(parent) - ������������� � ��������
.Unbind() - ������������
.HasXY(x,y) - �������� �� ������� ��������� x,y � ������������ �������������� (��� ����� ������)
]]


function CreateWinRect (width, height, SpecialMaterial) 
  assert(width~=nil, "ERROR: CreateWinRect () - width is nil")
  assert(height~=nil, "ERROR: CreateWinRect () - height is nil")

	local Rect={}
  
  Rect.Node=Scene.Node.Create()
  Rect.Width=width
  Rect.Height=height
  Rect.Style=nil -- ������� ��������� ����� ��������������.
  Rect.SpecialMaterial=SpecialMaterial -- ���� ����� ����. ��������, �� ����� ����� �������� ������ ����.
  
  -- �������� �������� ������ � �������
  Rect.Sprite=Scene.Sprite.Create()
    Rect.Sprite:BindToParent (Rect.Node)
    Rect.Sprite:SetPivotPosition (-0.5, 0.5, 0) -- � ����� ������� ����
  
  -----------------------------------------------------------------------------------------------------------------
  -- �������� �������
  -----------------------------------------------------------------------------------------------------------------
  
  Rect.Redraw = function ()
    -- ���������� ����� ����� ��������� �������� ����
    Rect.Sprite:SetScale (Rect.Width, Rect.Height, 1) -- ���������� �������
    local style=Rect.Style 
      if style==nil then return end --�����, ���� �� ����� ����� ��������������
    --���������� ���������
    if Rect.SpecialMaterial then 
      Rect.Sprite.Material=Rect.SpecialMaterial
    else
      if style.Material then
        Rect.Sprite.Material=style.Material
      end
    end
    --��������� ����� � ������������ �� ������
    if style.Color then
      Rect.Sprite.Color=style.Color
    end

    return Rect
  end 
  
  Rect.SetStyle = function (style)
    assert (style~=nil, "ERROR: br.SetStyle(style) - style is nil")
    Rect.Style=style
    Rect.Redraw()
  end
  
  Rect.BindToParent = function (parent)
    assert(parent~=nil, "ERROR: br.BindToParent(parent)- parent is nil")
    Rect.Node:BindToParent(parent)
  end
  
  Rect.Unbind = function ()
    Rect.Node:Unbind()
  end
  
  Rect.Position = function ()
    return Rect.Node.Position
  end
  
  Rect.GetCenterPosition =function ()
  local scale=Rect.Node.Scale
    return (Rect.Node.Position+vec3(Rect.Width*scale.x*0.5, -Rect.Height*scale.y*0.5, 0))
  end
  
  Rect.WorldPosition = function ()
    return Rect.Node.WorldPosition
  end  
  
  Rect.SetWidthAndHeight=function (width, height)
    assert(height,"ERROR: br.SetWidthAndHeight (width, height) - height is nil")
    Rect.Width=width
    Rect.Height=height
    Rect.Redraw()
  end
  
  Rect.HasXY =function (x, y)
    if (x==nil) or (y==nil) then
      return false
    end
    local pos=Rect.Node.Position
    if (pos.x<x) and (pos.y>y) then
      local scale=Rect.Node.Scale
      if ((x-pos.x)<(Rect.Width*scale.x)) and ((pos.y-y)<(Rect.Height*scale.y)) then
        return true
      end
    end
    return false
  end
  
  Rect.HasWorldXY =function (x, y)
    if (x==nil) or (y==nil) then
      return false
    end
    local pos=br.Node.WorldPosition
    if (pos.x<x) and (pos.y>y) then
      if ((x-pos.x)<Rect.Width) and ((pos.y-y)<Rect.Height) then
        return true
      end
    end
    return false
  end
  
  Rect.Redraw()
  
	return Rect
end

