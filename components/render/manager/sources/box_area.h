///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����� (��� ����������� ������� ���������� �������� ����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
class BoxAreaImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BoxAreaImpl ();
    BoxAreaImpl (const render::manager::Box&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                        SetBox (const render::manager::Box& Box);
    const render::manager::Box& Box    () const { return box; }
        
  private:
    render::manager::Box box;
};
