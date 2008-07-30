///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ���������� CLR-����� � const char*
///////////////////////////////////////////////////////////////////////////////////////////////////
class AutoString
{
  public:
///������������
    AutoString () : str (0) { }

    AutoString (System::String^ from)
    {
      str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi (from);
    }

///����������    
    ~AutoString ()
    {
      if (str)
        System::Runtime::InteropServices::Marshal::FreeHGlobal ((System::IntPtr)str);
    }

///������
    const char* Data ()
    {
      return str ? str : "";
    }
    
///�����
    void Swap (AutoString& s)
    {
      stl::swap (str, s.str);
    }

  private:
    char* str;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
inline const char* get_string (System::String^ from, AutoString& buffer)
{
  AutoString (from).Swap (buffer);
  
  return buffer.Data ();
}
