/*
    bad_lexical_cast
*/

//������������
inline bad_lexical_cast::bad_lexical_cast ()
  : source (&typeid (void)), target (&typeid (void))
  {}

inline bad_lexical_cast::bad_lexical_cast (const std::type_info& source_type, const std::type_info& target_type)
  : source (&source_type), target (&target_type)
  {}

//�������� � ������� ����
inline const std::type_info& bad_lexical_cast::source_type () const
{
  return *source;
}

inline const std::type_info& bad_lexical_cast::target_type () const
{
  return *target;
}
