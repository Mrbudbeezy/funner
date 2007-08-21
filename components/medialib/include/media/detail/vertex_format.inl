namespace detail
{

//�࠭���� �ଠ� ���設�
template <class Vertex> struct VertexFormatHolder
{
  VertexFormatHolder () { get_format (static_cast<const Vertex*> (0), format); }

  VertexFormat format;
};

}

/*
    ����祭�� �ଠ� ���設
*/

template <class Vertex>
inline const VertexFormat& get_vertex_format ()
{
  return common::Singleton<detail::VertexFormatHolder<Vertex> >::Instance ().format;
}
