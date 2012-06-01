

--[[
  �������� ���� ����� ��� �������� �� �������� � ��������������.
  �����������:
  ���������������� �������� ���������� ���� ��� ������ � �������������� ������� ������ ������ (����)
]]

function InitWindowEditor()
  assert(CreateXML~=nil, "ERROR: InitWindowEditor - CreateXML() - is nil")

  WindowEditor={}
  
  local StyleSets={} --������ ������ ����������� �� ������

  --------------------------------------------------------------------------------------------------
  -- ��������/���������� ������ ���� � ������
  --------------------------------------------------------------------------------------------------
  
  local function LoadStyleSetFromFile (StyleSetName)
    --��������� ����� ������ � ������ ��� � ���������
    assert(StyleSetName, "ERROR: WindowEditor.LoadStyleSetFromFile (StyleSetName) - FileName is nil")
    
    --�������� ������� ������ ������ � ����������� � ������� ������� ������
    local StyleSet={}
    StyleSets[StyleSetName]=StyleSet
    StyleSet.Name=StyleSetName
    
    local function LoadSimpleStyleSetByName(SetName)
      --�������� XML � ������� ������
      local xml=CreateXML()
      xml.Load("data/UI/Styles/"..SetName..".xml")
      xml.ForEachChildByName("Style", function(style_xml) 
        --����������� ������ ������ ������� �� XML
        local style_name=style_xml.ReadStringByName("Name")
        local style=StyleSet[style_name]
          if not(style) then 
            style=CreateWinStyle()
          end
        style.SetByXML(style_xml)
        StyleSet[style_name]=style
      end)
    end
    
    local SimpleSets=UnpackString(StyleSetName,", ", true)
    ForEachInTable(SimpleSets, LoadSimpleStyleSetByName)
  end
  
  WindowEditor.GetStyleSetByName=function (StyleSetName)
    assert(StyleSetName~=nil, "ERROR: WindowEditor.GetStyleSetByName(StyleSetName) - StyleSetName - is nil")
    local StyleSet=StyleSets[StyleSetName]
    if StyleSet==nil then
      LoadStyleSetFromFile(StyleSetName)
      --myprint("----- loaded styleset: <",StyleSetName,">")
    end
    StyleSet=StyleSets[StyleSetName]
    assert(StyleSet~=nil, "ERROR: WindowEditor.GetStyleSetByName("..StyleSetName..") - StyleSet is nil")
    return StyleSet
  end
  
  --------------------------------------------------------------------------------------------------
  -- �������� ����������� ������
  --------------------------------------------------------------------------------------------------
  
  local function AddCloseButtonToWindow (window, button)
    --���� ������ ������, �� ������� �������� ���� ����� ��������� � ���
    assert(window~=nil,"ERROR: WindowEditor.AddCloseButtonToWindow - window is nil")
    assert(WindowManager~=nil,"ERROR: WindowEditor.AddCloseButtonToWindow - WindowManager is nil")
    if button==nil then
      button=WindowEditor.CreateButton ("_CloseButton", 0.5, 0.5)
    end
    button.RegisterEventHandler("Pressed", WindowManager.GetFunctionForWindowClose(window))
    window.AddControl(button)
    button.Node:SetPosition(window.Rect.Width-button.Rect.Width,0,0)
  end
    
  --------------------------------------------------------------------------------------------------
  -- ���������� � �������� ����
  --------------------------------------------------------------------------------------------------
  
 
  --������ ������� ��� �������� ��������� �� ��������� ����
  local CreateControlFunctionsByType={}
        CreateControlFunctionsByType["Button"]=UI.MakeButton
        CreateControlFunctionsByType["Flag"]=UI.MakeFlag
        CreateControlFunctionsByType["Slider"]=UI.MakeSlider
        CreateControlFunctionsByType["InputBox"]=UI.MakeInputBox
        CreateControlFunctionsByType["TextBox"]=UI.MakeTextBox
        CreateControlFunctionsByType["StatusBar"]=UI.MakeStatusBar
  
  local function LoadControlFromXML (xml)
    assert(xml~=nil, "ERROR: WindowEditor.LoadControlFromXML (xml) - xml is nil")
    local control=CreateControl()
    control.SetControlByXML(xml)
    --�������� ����������� �������� �� ��� ����
    local F_Create=CreateControlFunctionsByType[control.ControlType]
    assert(F_Create, "ERROR: WindowEditor.LoadControlFromXML (xml) - Creation function for control type: "..control.ControlType.." not found")
    F_Create(control)
    --��������� ���������� �������� � ������������ � xml
    control.SetByXML(xml)
    return control
  end
  
  WindowEditor.LoadWindowFromXML=function (xml)
    assert(xml~=nil,"ERROR: WindowEditor.LoadWindowFromXML (xml) - xml is nil")
    local window=CreateControl()
    window.SetControlByXML(xml)
    UI.MakeWindow(window)
    local caption=xml.ReadStringByName("Caption")
    if caption then window.SetCaption(caption) end
    window.Movable=xml.ReadBooleanByName("Movable", true)
    window.AlwaysFocused=xml.ReadBooleanByName("AlwaysFocused", false)
    window.SelectControlsByKeys=xml.ReadBooleanByName("SelectControlsByKeys", true)
    window.AnimatedOpening=xml.ReadBooleanByName("AnimatedOpening", true)
    window.AlwaysOverWindow=xml.ReadStringByName("AlwaysOverWindow")
    
    --�������� ��������� ����
    xml.ForEachChildByName("Control", function(control_xml)
      local control=LoadControlFromXML(control_xml)
      window.AddControl(control)
    end)
    
    window.HandleEvent("LostFocus")
    return window
  end
  
  WindowEditor.LoadWindowFromFile=function (filename)
    assert(filename~=nil, "ERROR: WindowEditor.LoadWindowFromFile (filename) - filename is nil")
    local xml=CreateXML("data/UI/Windows/"..filename)
    return WindowEditor.LoadWindowFromXML(xml)
  end
end
