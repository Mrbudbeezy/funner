/*
    �������� �������������� ������� ������� ���� � ������ ������������ ����������� MSVC
*/

//����������� �������� �� T �������
template <class T> struct is_class: public bool_constant<__is_class (T)> {};

//����������� �������� �� T �������������
template <class T> struct is_enum: public bool_constant<__is_enum (T)> {};

//����������� �������� �� T ����������� �������
template <class T> struct is_polymorphic: public bool_constant<__is_polymorphic (T)> {};

//����������� �������� �� ��� T �����������
template <class T> struct is_abstract: public bool_constant<__is_abstract (T)> {};

//����������� �������� �� T POD-�����
template <class T> struct is_pod: public bool_constant<__is_pod (T)> {};

//����������� ������������ ����
template <class T> struct alignment_of: public integral_constant<size_t, __align_of (T)> {};
