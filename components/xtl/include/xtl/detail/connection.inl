namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� ॠ����樨 ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��뫮�
///////////////////////////////////////////////////////////////////////////////////////////////////
    void addref () { ref_count++; }
    
    void release ()
    {
      if (!--ref_count)
        delete this;
    }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ 䠪� ������ ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool connected () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����஢���� ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void block   () = 0;
    virtual void unblock () = 0;
    virtual bool blocked () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void disconnect () = 0;

  protected:
    connection_impl () : ref_count (1) {}

    virtual ~connection_impl () {}

  private:
    size_t ref_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���⮥ ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class empty_connection_impl: public connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� �����⢥����� ������� ���⮣� ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static connection_impl* instance_ptr () {
      static char buffer [sizeof (empty_connection_impl)];
      static connection_impl* empty = new (buffer) empty_connection_impl;

      return empty;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ ���ﭨ� ᮥ������� / ࠧ�� ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected  () { return false; }
    void disconnect () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����஢�� ᮥ�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void block   () {}
    void unblock () {}    
    bool blocked () { return true; }
};

}

/*
    connection
*/

inline connection::connection ()
  : impl (detail::empty_connection_impl::instance_ptr ())
{
  impl->addref ();
}

inline connection::connection (detail::connection_impl* in_impl)
  : impl (in_impl)
{
  impl->addref ();
}
  
inline connection::connection (const connection& c)
  : impl (c.impl)
{
  impl->addref ();  
}

inline connection::~connection ()
{
  impl->release ();
}

inline connection& connection::operator = (const connection& c)
{
  connection (c).swap (*this);

  return *this;
}

inline bool connection::connected () const
{
  return impl->connected ();
}

inline void connection::disconnect ()
{
  impl->disconnect ();

  connection ().swap (*this);
}

inline void connection::block ()
{
  impl->block ();
}

inline void connection::unblock ()
{
  impl->unblock ();
}

inline bool connection::blocked () const
{
  return impl->blocked ();
}

inline bool connection::operator == (const connection& c) const
{
  return impl == c.impl;
}

inline bool connection::operator != (const connection& c) const
{
  return !(*this == c);
}

inline void connection::swap (connection& c)
{
  detail::connection_impl* tmp = impl;
  impl                         = c.impl;
  c.impl                       = tmp;
}

inline void swap (connection& a, connection& b)
{
  a.swap (b);
}

/*
    auto_conection
*/

inline auto_connection::auto_connection (const connection& c)
  : connection (c)
  {}

inline auto_connection::~auto_connection ()
{
  disconnect ();
}
   
inline auto_connection& auto_connection::operator = (const connection& c)
{
  auto_connection (c).swap (*this);
  
  return *this;
}
