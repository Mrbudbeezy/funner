#include "shared.h"

using namespace common;

/*
    Описание реализации параметров шифрования
*/

struct FileCryptoParameters::Impl: public xtl::reference_counter
{
  stl::string                      read_method;
  stl::string                      write_method;
  unsigned short                   key_bits;
  xtl::uninitialized_storage<char> key;
  
  Impl (const char* in_read_method, const char* in_write_method, const void* in_key, unsigned short in_key_bits)
    : read_method (in_read_method)
    , write_method (in_write_method)
    , key_bits (in_key_bits)
  {
    size_t key_size = key_bits / CHAR_BIT;
    
    if (key_bits % CHAR_BIT)
      key_size++;

    key.resize (key_size);

    if (key_size)
      memcpy (key.data (), in_key, key_size);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

FileCryptoParameters::FileCryptoParameters (const char* read_method, const char* write_method, const void* key, unsigned short key_bits)
{
  static const char* METHOD_NAME = "common::FileFileCryptoParameters";
  
  if (!read_method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "read_method");
    
  if (!write_method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "write_method");

  if (!key && key_bits)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key");
    
  impl = new Impl (read_method, write_method, key, key_bits);
}

FileCryptoParameters::FileCryptoParameters (const char* read_method,const char* write_method,const char* key_string)
{
  static const char* METHOD_NAME = "common::FileFileCryptoParameters";
  
  if (!read_method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "read_method");
    
  if (!write_method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "write_method");

  if (!key_string)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_string");
    
  size_t key_length = strlen (key_string);
  
  if (key_length % 2)
    throw xtl::make_argument_exception (METHOD_NAME, "key_string", key_string, "Key string must have even value length");

  if (key_length * CHAR_BIT > USHRT_MAX)
    throw xtl::make_argument_exception (METHOD_NAME, "key_string", key_string, "Key string too long");

  xtl::uninitialized_storage<char> key (key_length / 2);

  compress_buffer (key_length, key_string, key.data ());

  impl = new Impl (read_method, write_method, key.data (), (unsigned short)key.size () * CHAR_BIT);
}

FileCryptoParameters::FileCryptoParameters (const FileCryptoParameters& params)
  : impl (params.impl)
{
  addref (impl);
}

FileCryptoParameters::~FileCryptoParameters ()
{
  release (impl);
}

FileCryptoParameters& FileCryptoParameters::operator = (const FileCryptoParameters& params)
{
  addref (params.impl);  
  release (impl);
  
  impl = params.impl;

  return *this;
}

/*
    Параметры
*/

//метод шифрования при чтении из файла
const char* FileCryptoParameters::ReadMethod () const
{
  return impl->read_method.c_str ();
}

//метод шифрования при записи в файл
const char* FileCryptoParameters::WriteMethod () const
{
  return impl->write_method.c_str ();
}

//указатель на буфер, содержащий ключ шифрования
const void* FileCryptoParameters::Key () const
{
  static char empty_key = 0;

  return impl->key.size () ? impl->key.data () : &empty_key;
}

//количество битов в ключе
unsigned short FileCryptoParameters::KeyBits () const
{
  return impl->key_bits;
}
