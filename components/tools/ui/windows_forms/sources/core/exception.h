//������ ����� �᪫�祭��, �࠭᫨஢����� �� .NET Framework
struct DotNetException: public xtl::message_exception_base
{
  DotNetException (const char* source, System::Exception^ exception) :
    message_exception_base (source, AutoString (exception->Message).Data ()) {}
};
