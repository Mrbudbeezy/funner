--[[
 ��������������� ���������� ������ � �������
 ���� ��� ������� - ������ ���������� ��
 ���� ��� ������ - ��������� ������
]]

function ConvertToFunction(obj)
  if type(obj)=="function" then return obj end
  if type(obj)=="string" then return loadstring(obj) end
  return nil
end
