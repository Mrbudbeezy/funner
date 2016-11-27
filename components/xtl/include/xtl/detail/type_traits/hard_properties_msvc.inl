/*
    Проверка дополнительных сложных свойств типа с учётом возможностей компилятора MSVC
*/

//определение является ли T классом
template <class T> struct is_class: public bool_constant<__is_class (T)> {};

//определение является ли T перечислением
template <class T> struct is_enum: public bool_constant<__is_enum (T)> {};

//определение является ли T полиморфным классом
template <class T> struct is_polymorphic: public bool_constant<__is_polymorphic (T)> {};

//определение является ли тип T абстрактным
template <class T> struct is_abstract: public bool_constant<__is_abstract (T)> {};
