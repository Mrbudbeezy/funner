--[[
  ��������� ������� ��� ���� �������� �� �������
  ���� ������� ���������� false - �� ���������� ��������� ����� �� �����
]]

function ForEachInTable (t, func, ...)
  local index, value = next (t)
  while index do
    if func (value, ...)==false then
      return
    end
    index, value = next (t, index)
  end
end
