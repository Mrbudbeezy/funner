///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Control> class ControlRegistry
{
  public:
    typedef xtl::intrusive_ptr<Control> ControlPtr;

///����������� ��������
    void Register (const char* id, const ControlPtr& control)
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ControlRegistry<T>::Register";

      if (!id)
        throw xtl::make_null_argument_exception (METHOD_NAME, "id");

      ControlMap::iterator iter = controls.find (id);

      if (iter != controls.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "id", id, "Control has already registered");

      controls.insert_pair (id, control);
    }
    
///������ ����������� ��������
    void Unregister (const char* id)
    {
      if (!id)
        return;

      controls.erase (id);
    }
    
///����� ��������
    Control* Find (const char* id) const
    {
      if (!id)
        return 0;
        
      ControlMap::const_iterator iter = controls.find (id);
      
      if (iter == controls.end ())
        return 0;
        
      return iter->second.get ();
    }
    
    ControlPtr Item (const char* id) const
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ControlRegistry<T>::Item";

      if (!id)
        throw xtl::make_argument_exception (METHOD_NAME, "id");

      ControlMap::const_iterator iter = controls.find (id);

      if (iter == controls.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "id", id, "No control with this name");

      return iter->second.get ();
    }
    
///���������� ���������
    size_t Size () { return controls.size (); }

///����������� ������
    static void RegisterInvokers (script::Environment& environment, const char* library_name)
    {
      using namespace script;

      InvokerRegistry& lib = environment.CreateLibrary (library_name);

        //����������� �������

      lib.Register ("get_ItemsCount", make_invoker (&ControlRegistry::Size));

        //����������� �������

      lib.Register ("Find", make_invoker (&ControlRegistry::Find));
      lib.Register ("Item", make_invoker (&ControlRegistry::Item));

        //����������� ����� ������

      environment.RegisterType<ControlRegistry> (library_name);
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ControlPtr> ControlMap;

  private:
    ControlMap controls; //��������
};
