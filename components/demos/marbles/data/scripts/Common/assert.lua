--[[
  ��� ������� ������������ ��� ������� error(message)
  ���������� �������� �� ��, ��� � assert(value, message)
  ������, � ������� �� assert, ��������� ���������� ���� �� �������� ��������� ������
]]
local AssertionPrefixString=""

function AssertionPrefix(str)
  if str then
    AssertionPrefixString=str
  else
    AssertionPrefixString=""
  end
end

function assert(value, str)
  if value then
    return
  end
  error(AssertionPrefixString..str)
end

