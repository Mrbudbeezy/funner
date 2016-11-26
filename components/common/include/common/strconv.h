#ifndef COMMONLIB_COMMON_STRING_CONVERTER_HEADER
#define COMMONLIB_COMMON_STRING_CONVERTER_HEADER

#include <stddef.h>
#include <xtl/functional_fwd>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс преобразователя строк
///////////////////////////////////////////////////////////////////////////////////////////////////
class IStringConverter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IStringConverter () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Convert (const void*& source_buffer_ptr,
                          size_t&      source_buffer_size,
                          void*&       destination_buffer_ptr,
                          size_t&      destination_buffer_size) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конвертер строк
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    StringConverter  (const char* source_encoding, const char* destination_encoding);
    StringConverter  (const StringConverter& conv);
    ~StringConverter ();

    StringConverter& operator = (const StringConverter& conv);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (const void*& source_buffer_ptr,
                  size_t&      source_buffer_size,
                  void*&       destination_buffer_ptr,
                  size_t&      destination_buffer_size) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (StringConverter&);

  private:
    IStringConverter* converter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (StringConverter&, StringConverter&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления конвертацией строк
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverterSystem
{
  public:
    typedef xtl::function<IStringConverter* (const char* source_encoding, const char* destination_encoding)> ConverterFn;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистраци конвертеров строк
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterConverter       (const char* source_encoding,
                                         const char* destination_encoding,
                                         const ConverterFn& converter);
    static void UnregisterConverter     (const char* source_encoding, const char* destination_encoding);
    static void UnregisterAllConverters ();
    static bool IsConverterRegistered   (const char* source_encoding,
                                         const char* destination_encoding);
};

}

#endif
