namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection_impl: private reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void addref () { increment (); }
    
    void release ()
    {
      if (decrement ())
        delete this;
    }
    
    using reference_counter::use_count;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool connected () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void block   () = 0;
    virtual void unblock () = 0;
    virtual bool blocked () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void disconnect () = 0;

  protected:
    virtual ~connection_impl () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class empty_connection_impl: public connection_impl
{
  public:
    empty_connection_impl () {}
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ���������� / ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected  () { return false; }
    void disconnect () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
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
  : impl (&singleton_default<detail::empty_connection_impl, false>::instance ())
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
