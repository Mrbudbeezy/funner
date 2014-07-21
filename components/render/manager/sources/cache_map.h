///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CacheMapDefaultRemovePred
{
  template <class T> bool operator () (const T&) const { return true; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ����������� (��������� ����������� - LRU)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key, class Value, class RemovePred = CacheMapDefaultRemovePred>
class CacheMap: public Cache
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CacheMap (const CacheManagerPtr&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    ();
    bool   IsEmpty ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Value* Find (const Key&);
    Value& Get  (const Key&);
    Value& operator [] (const Key& key) { return Get (key); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add    (const Key& key, const Value& value);
    void Remove (const Key& key);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> void ForEach (Fn fn);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushCache ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Value& AddCore (const Key& key, const Value& value);
  
  private:
    struct Item;
  
    typedef stl::hash_map<Key, Item>     ItemMap;    
    typedef stl::list<typename ItemMap::iterator> ItemList;
    
    struct Item: public Value
    {
      FrameId                     last_use_frame; //���� ���������� ������������� ��������
      FrameTime                   last_use_time;  //����� ���������� ������������� ��������
      typename ItemList::iterator position;       //��������� � ������ ���������      
    };        

  private:
    ItemMap    item_map;
    ItemList   item_list;
    RemovePred remove_pred;
};

/*
    ����������
*/

/*
    �����������
*/

template <class Key, class Value, class RemovePred>
CacheMap<Key, Value, RemovePred>::CacheMap (const CacheManagerPtr& manager)
  : Cache (manager)
{
}

/*
    ���������� ��������� / �������� �� �������
*/

template <class Key, class Value, class RemovePred>
size_t CacheMap<Key, Value, RemovePred>::Size ()
{
  return item_map.size ();
}

template <class Key, class Value, class RemovePred>
bool CacheMap<Key, Value, RemovePred>::IsEmpty ()
{
  return item_map.empty ();
}

/*
    ����� ��������
*/

template <class Key, class Value, class RemovePred>
Value* CacheMap<Key, Value, RemovePred>::Find (const Key& key)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter == item_map.end ())
    return 0;
    
  iter->second.last_use_time  = CurrentTime ();
  iter->second.last_use_frame = CurrentFrame ();
  
  item_list.splice (item_list.begin (), item_list, iter->second.position);
  
  return &iter->second;
}

template <class Key, class Value, class RemovePred>
Value& CacheMap<Key, Value, RemovePred>::Get (const Key& key)
{
  if (Value* result = Find (key))
    return *result;
    
  return AddCore (key, Value ());
}

/*
    ������� � �������� ��������
*/

template <class Key, class Value, class RemovePred>
Value& CacheMap<Key, Value, RemovePred>::AddCore (const Key& key, const Value& value)
{
  Item item;
  
  item.last_use_time  = CurrentTime ();
  item.last_use_frame = CurrentFrame ();
  
  static_cast<Value&> (item) = value;
  
  stl::pair<typename ItemMap::iterator, bool> result = item_map.insert_pair (key, item);
  
  if (!result.second)
    throw xtl::make_argument_exception ("render::manager::CacheMap<Key, Value, RemovePred>::Add", "Internal error: item with specified key/value has been already inserted");    
    
  try
  {
    item_list.push_front (result.first);
  
    result.first->second.position = item_list.begin ();
    
    return result.first->second;
  }
  catch (...)
  {
    item_map.erase (result.first);
    throw;
  }
}

template <class Key, class Value, class RemovePred>
void CacheMap<Key, Value, RemovePred>::Add (const Key& key, const Value& value)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter != item_map.end ())
    throw xtl::make_argument_exception ("render::manager::CacheMap<Key, Value, RemovePred>::Add", "Item with specified key/value has been already inserted");
    
  AddCore (key, value);
}

template <class Key, class Value, class RemovePred>
void CacheMap<Key, Value, RemovePred>::Remove (const Key& key)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter == item_map.end ())
    return;
  
  item_list.erase (iter->second.position);
  item_map.erase (iter);
}

/*
    �������
*/

template <class Key, class Value, class RemovePred>
void CacheMap<Key, Value, RemovePred>::Clear ()
{
  item_map.clear ();
  item_list.clear ();
}

/*
    ���������
*/

template <class Key, class Value, class RemovePred> template <class Fn>
void CacheMap<Key, Value, RemovePred>::ForEach (Fn fn)
{
  for (typename ItemMap::iterator iter=item_map.begin (); iter!=item_map.end (); ++iter)
    fn (iter->first, iter->second);
}

/*
    ����� �������������� ��������� ������� �� �������������� � ����� min_frame ��� ������� min_time
*/

template <class Key, class Value, class RemovePred>
void CacheMap<Key, Value, RemovePred>::FlushCache ()
{
  FrameTime current_time  = CurrentTime (), time_delay = TimeDelay ();
  FrameId   current_frame = CurrentFrame (), frame_delay = FrameDelay ();

  typename ItemList::iterator iter = item_list.end ();

  while (!item_list.empty () && iter != item_list.begin ())
  {
    --iter;

    Item& item = (*iter)->second;
    
    if (current_time - item.last_use_time <= time_delay && current_frame - item.last_use_frame <= frame_delay)
      return;  

    if (remove_pred (item))
    {
      typename ItemList::iterator next = iter;

      ++next;
        
      item_map.erase (*iter);
      item_list.erase (iter);

      iter = next;
    }
  }
}
