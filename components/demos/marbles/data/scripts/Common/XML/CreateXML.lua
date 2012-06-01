

function CreateXML(filename)
  --��� ����� ����������� ���� ����� ��������� xml �� �����
  local xml={}
  
  xml.Node=Common.StringNode.Create()
  
  --------------------------------------------------------------------------------------------------
  -- ���������� � ��������
  
  xml.Save=CreateMethodForObject(XML_Save, xml)
  
  xml.Load=CreateMethodForObject(XML_Load, xml)
    
  --------------------------------------------------------------------------------------------------
  --��������� �����
  xml.SetName=CreateMethodForObject(XML_SetName, xml)
   
  xml.GetName=CreateMethodForObject(XML_GetName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- ������ ������
  xml.WriteStringWithName=CreateMethodForObject(XML_WriteStringWithName, xml)
  
  xml.TryWriteStringWithName=CreateMethodForObject(XML_WriteStringWithName, xml)
    
  xml.WriteStringsWithNames=CreateMethodForObject(XML_WriteStringsWithNames, xml)
  
  xml.WriteBooleanWithName=CreateMethodForObject(XML_WriteBooleanWithName, xml)
  
  xml.TryWriteBooleanWithName=CreateMethodForObject(XML_WriteBooleanWithName, xml)
  
  xml.WriteNumbersWithName=CreateMethodForObject(XML_WriteNumbersWithName, xml)
  
  xml.WriteVec3WithName=CreateMethodForObject(XML_WriteVec3WithName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- ������ ������
  xml.ReadStringByName=CreateMethodForObject(XML_ReadStringByName, xml)
    
  xml.ReadStringsByNames=CreateMethodForObject(XML_ReadStringsByNames, xml)
  
  xml.ReadBooleanByName=CreateMethodForObject(XML_ReadBooleanByName, xml)

  xml.ReadNumberByName=CreateMethodForObject(XML_ReadNumberByName, xml)
  
  xml.ReadNumbersByNames=CreateMethodForObject(XML_ReadNumbersByNames, xml)
  
  xml.ReadNumbersByName=CreateMethodForObject(XML_ReadNumbersByName, xml)
    
  xml.ReadVec3ByName=CreateMethodForObject(XML_ReadVec3ByName, xml)
  
  xml.TryReadVec3ByName=CreateMethodForObject(XML_ReadVec3ByName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- ������ � ���������
  
  xml.AddChild=CreateMethodForObject(XML_AddChild, xml)
  
  xml.GetChild=CreateMethodForObject(XML_GetChild, xml)
  
  xml.ForEachChild=CreateMethodForObject(XML_ForEachChild, xml)
  
  xml.ForEachChildByName=CreateMethodForObject(XML_ForEachChildByName, xml)

  --------------------------------------------------------------------------------------------------
  -- �������������
  --------------------------------------------------------------------------------------------------
  xml.SetName("noname")
  
  --���� ���� ������� ��� ����� - ��������� xml
  if filename then
    xml.Load(filename)
  end
  
  return xml
end

