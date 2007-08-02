namespace detail
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��� ���������� �������� ����������������� ���� ������ � ����� lua
//////////////////////////////////////////////////////////////////////////////////////////////////
struct IUserData
{
  virtual ~IUserData () {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���� ���
//////////////////////////////////////////////////////////////////////////////////////////////////
class Stack
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
//////////////////////////////////////////////////////////////////////////////////////////////////    
    Stack (lua_State*);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������� ����� � ����
//////////////////////////////////////////////////////////////////////////////////////////////////
    void Push (char);
    void Push (signed char);
    void Push (unsigned char);
    void Push (short);
    void Push (unsigned short);
    void Push (int);
    void Push (unsigned int);
    void Push (long);
    void Push (unsigned long);
    void Push (float);
    void Push (double);
    void Push (long double);
    void Push (const char*);
    void Push (void*);
    void PushFunction (const char*);

//////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
    void Pop (size_t items_count);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � ���� ����������������� ���� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void Push (const T&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> T Get (int index) const;

  private:
    template <class T> struct UserDataImpl;

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � ���� ����������������� ���� ������
//////////////////////////////////////////////////////////////////////////////////////////////////  
    void* NewUserData (size_t size);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ����� ��������� ������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
    double      GetDouble  (int index) const;
    int         GetInteger (int index) const;
    void*       GetPointer (int index) const;
    const char* GetString  (int index) const;

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ����� �������� ����������������� ���� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    IUserData* GetUserData (int index) const;

  private:
    lua_State* state;    
};

/*
    ��������� ��� ������� ����������������� ���� ������
*/

template <class T> struct Stack::UserDataImpl: IUserData
{
  UserDataImpl (const T& in_value) : value (in_value) {}

  T value;
};
  
/*
    ��������� � ���� ����������������� ���� ������
*/

template <class T>
inline void Stack::Push (const T& value)
{
  new (NewUserData (sizeof (UserDataImpl<T>))) UserDataImpl<T> (value);
}

/*
    ��������� �� ����� ����������������� ���� ������
*/

template <class T>
inline T Stack::Get (int index) const
{
  UserDataImpl<T>* object = dynamic_cast<UserDataImpl<T>*> (GetUserData (index));

  if (!object) ////!!!!!
    Raise <Exception> ("Stack::Get", "Stack item has other type than asked."); //wrong type

  return object->value;
}

/*
    ���������� ��������� � ���� ����� ������ ���������� � �������
*/

inline void Stack::Push (char value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (signed char value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (unsigned char value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (short value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (unsigned short value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (unsigned int value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (long value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (unsigned long value)
{
  Push (static_cast<int> (value));
}

inline void Stack::Push (float value)
{
  Push (static_cast<double> (value));
}

inline void Stack::Push (long double value)
{
  Push (static_cast<double> (value));
}

/*
    ������������� ��������� ��������� ������� �����
*/

template <> inline char Stack::Get<char> (int index) const
{
  return static_cast<char> (GetInteger (index));
}

template <> inline signed char Stack::Get<signed char> (int index) const
{
  return static_cast<signed char> (GetInteger (index));
}

template <> inline unsigned char Stack::Get<unsigned char> (int index) const
{
  return static_cast<unsigned char> (GetInteger (index));
}

template <> inline short Stack::Get<short> (int index) const
{
  return static_cast<short> (GetInteger (index));
}

template <> inline unsigned short Stack::Get<unsigned short> (int index) const
{
  return static_cast<unsigned short> (GetInteger (index));
}

template <> inline int Stack::Get<int> (int index) const
{
  return GetInteger (index);
}

template <> inline unsigned int Stack::Get<unsigned int> (int index) const
{
  return static_cast<unsigned int> (GetInteger (index));
}

template <> inline long Stack::Get<long> (int index) const
{
  return static_cast<long> (GetInteger (index));
}

template <> inline unsigned long Stack::Get<unsigned long> (int index) const
{
  return static_cast<unsigned long> (GetInteger (index));
}

template <> inline double Stack::Get<double> (int index) const
{
  return GetDouble (index);
}

template <> inline float Stack::Get<float> (int index) const
{
  return static_cast<float> (GetDouble (index));
}

template <> inline long double Stack::Get<long double> (int index) const
{
  return static_cast<long double> (GetDouble (index));
}

template <> inline void* Stack::Get<void*> (int index) const
{
  return GetPointer (index);
}

template <> inline const char* Stack::Get<const char*> (int index) const
{
  return GetString (index);
}

}
