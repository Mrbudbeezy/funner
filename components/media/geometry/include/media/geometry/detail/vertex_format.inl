namespace detail
{

//��������� ������� �������
template <class Vertex> struct VertexFormatHolder
{
  VertexFormatHolder () { get_vertex_format (static_cast<const Vertex*> (0), format); }

  VertexFormat format;
};

}

/*
    ��������� ������� ������
*/

template <class Vertex>
inline const VertexFormat& get_vertex_format ()
{
  return common::Singleton<detail::VertexFormatHolder<Vertex> >::Instance ()->format;
}
