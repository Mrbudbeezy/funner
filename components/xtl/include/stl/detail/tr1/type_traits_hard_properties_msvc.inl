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

//����������� ������������ ����
template <class T> struct alignment_of:                      public integral_constant<size_t, __alignof (T)> {};
template <>        struct alignment_of<void>:                public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const void>:          public integral_constant<size_t, 0> {};
template <>        struct alignment_of<volatile void>:       public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const volatile void>: public integral_constant<size_t, 0> {};
