#ifndef MEDIA_RMS_SHARED_HEADER
#define MEDIA_RMS_SHARED_HEADER

#include <stl/algorithm>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/list>
#include <stl/vector>
#include <stl/auto_ptr.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <common/file.h>
#include <common/parser.h>
#include <common/string.h>
#include <common/strlib.h>

#include <media/rms/server.h>

namespace media
{

namespace rms
{

/*///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ����� �� �������� ������ �������� � ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IResourceManagerCallback
{
  public:
    virtual void AddResources      (size_t count, const char** resource_names) = 0;
    virtual void ReleaseResources  (size_t count, const char** resource_names) = 0;
    virtual void PrefetchResources (size_t count, const char** resource_names) = 0;
    virtual void LoadResources     (size_t count, const char** resource_names) = 0;
    virtual void UnloadResources   (size_t count, const char** resource_names) = 0;

  protected:
    virtual ~IResourceManagerCallback () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������� �������� � ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceManagerBindingCallback
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceManagerBindingCallback  (IResourceManagerCallback*);
    ResourceManagerBindingCallback  (const ResourceManagerBindingCallback&);
    ~ResourceManagerBindingCallback ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddResources      (size_t count, const char** resource_names);
    void ReleaseResources  (size_t count, const char** resource_names);
    void PrefetchResources (size_t count, const char** resource_names);
    void LoadResources     (size_t count, const char** resource_names);
    void UnloadResources   (size_t count, const char** resource_names);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DisconnectFromResourceManager ();

  private:    
    ResourceManagerBindingCallback& operator = (const ResourceManagerBindingCallback&); //no impl

  private:
    struct Impl;
    Impl* impl;
};*/

}

}

#endif
