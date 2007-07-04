namespace common
{

//implementation forwards
class ParseContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParseDispatch
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������� ������� ��� � ��� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Parse (const char*   file_name,
                       ParseContext& context,
                       const char*   format);
    static void Parse (const char*   name,
                       const char*   buf,
                       size_t        size,
                       ParseContext& conext,
                       const char*   format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void ParseWXF (char* buf,ParseContext& context);
    static void ParseXML (char* buf,ParseContext& context);
};

}
