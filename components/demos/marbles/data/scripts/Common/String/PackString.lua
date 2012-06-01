function PackString(terminal, ...)
  --[[
  ������� ������ �� ���������� ����������� ����������, �������� �� ����������
  ]]
  terminal=tostring(terminal)
  local params={...}
  local value=nil
  local index=nil
  index, value = next(params)
  local s=""
  while index do
    if s~="" then
      --���� ������ ��� �� ������ - ������� �������� ����� ��������� ���������
      s=s..terminal
    end
    if type(value)=="string" then
      s=s..value
    else
      s=s..tostring(value)
    end
    index, value = next(params, index)
  end
  return s
end