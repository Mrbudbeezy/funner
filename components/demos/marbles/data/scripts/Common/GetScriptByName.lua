
function GetScriptByName(ScriptStore, name, UseWrapper)
  assert(name, "ERROR: GetScriptByName(ScriptStore, name, UseWrapper) - name is nil")
  assert(type(ScriptStore)=="table", "ERROR: GetScriptByName(ScriptStore, name, UseWrapper) - ScriptStore is not a table")
  local script=ScriptStore[name]
  --�������� ������� ��������� �������� � ������������ ������ � �� ���� ��� �� ��� �������� � ������� ��������, �� ������� ������ � ���������
  local function ScriptWrapper(...)
    if script then
      script(...)
    else
      --���� ������ �� ��� ��� �� ��� ������� - ����� ������� �������� ���
      script=ScriptStore[name]
      if script then
        script(...)
      else
        --���� ������ �� ���������� ������� - ������� ��������� � ���, ��� ����� ������ �� ����������
        myprint("SCRIPT WARNING: Unknown script: ", name)
      end
    end
  end
  if UseWrapper then
    return ScriptWrapper
  else
    if script then
      --����������� ��� ����������� ������ ��� ��������
      return script
    else
      --���� ������ �� �������� - ���������� ������������ ��������
      myprint("SCRIPT WARNING: ", name," - used ScriptWrapper!")
      return ScriptWrapper
    end
  end
end
