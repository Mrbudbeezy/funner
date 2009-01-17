#ifndef MODELER_VISUALIZER_SHARED_HEADER
#define MODELER_VISUALIZER_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <stl/auto_ptr.h>
#include <stl/hash_map>
#include <stl/list>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/implicit_cast.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/file.h>
#include <common/parse_iterator.h>
#include <common/parser.h>
#include <common/strlib.h>
#include <common/var_registry.h>

#include <syslib/application.h>
#include <syslib/timer.h>
#include <syslib/window.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <media/rms/manager.h>

#include <render/low_level/device.h>
#include <render/low_level/utils.h>

#include <render/scene_render.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

#include <script/shell.h>

#include <tools/ui/custom_window_system.h>

class RenderableModel
{
  public:
    RenderableModel (render::low_level::IDevice& device, const char* file_name);
    ~RenderableModel ();

    void Draw (render::low_level::IDevice& device);

  private:
     RenderableModel (const RenderableModel&);
     RenderableModel& operator = (const RenderableModel&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

//�������� ���������������� �������� ����
class MyChildWindow: public tools::ui::ICustomChildWindow, public xtl::reference_counter
{
  public:
///�����������
    MyChildWindow ();

///��������� ��������� ����
    void SetPosition (size_t x, size_t y);

///��������� �������� ����
    void SetSize (size_t width, size_t height);

///��������� ������������� ����
    void SetParent (const void* parent_window_handle);

///���������� ���������� ����
    void Show (bool state);

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    engine::SubsystemManager          manager; //�������� ���������
    scene_graph::OrthoCamera::Pointer camera;
    syslib::Window                    window;
};

//�������� ������ ����������
class MyApplicationServer: public tools::ui::IApplicationServer, public xtl::reference_counter
{
  private:
    typedef xtl::function <void (const char*)> WaitFileHandler;

  public:
    MyApplicationServer ();

    void ExecuteCommand (const char* command);

    void SetProperty (const char* name, const stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::SetProperty");
    }

    void GetProperty (const char* name, stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::GetProperty");
    }

    bool IsPropertyPresent (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }

    bool IsPropertyReadOnly (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }

    void RegisterPropertyListener (const char* name_wc_mask, tools::ui::IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::RegisterPropertyListener");
    }

    void UnregisterPropertyListener (const char* name_wc_mask, tools::ui::IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterPropertyListener");
    }

    void UnregisterAllPropertyListeners (const char* name_wc_mask)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");
    }

    void UnregisterAllPropertyListeners ()
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");
    }

    tools::ui::ICustomChildWindow* CreateChildWindow (const char* init_string)
    {
      return new MyChildWindow ();
    }

    void OpenProjectPath (const char* path);

    size_t CalculatingTrajectoriesCount ();

    void CleanBatchCalculationList ();
    void AddBatchCalculation       (float nu1, float nu2, float nu3);
    void RunBatchCalculation       (size_t lod, float point_equal_epsilon);

    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    void LoadAllCalculatedTrajectories ();

    void UnloadEnvelope ();
    void UnloadModels ();
    void Cleanup ();

    void LoadTrajectory (const char* file_name);
    bool LoadTrajectoryIfExist (float nu1, float nu2, float nu3, float epsilon);

    void OnNewXmeshTrajectory (const char* desc_trajectory_file_name);
    void CalculateTrajectory (double nu1, double nu2, double nu3, size_t lod);

    void LoadEnvelope ();

    void OnNewXmeshEnvelope (const char* desc_envelope_file_name);
    void CalculateEnvelope (size_t lod);

    void OnNewCondorBatchTrajectory (const char* desc_trajectory_file_name);
    void OnNewTrajectoriesCoords (const char* desc_file_name, size_t lod);
    void CalculateTrajectories (size_t trajectories_count, size_t lod);

    void CheckNewFiles   ();
    void WaitForFile     (const char* file_name, const WaitFileHandler& handler);
//    void StopFileWaiting (const char* file_name);

    void LoadModel (const char* path);
    void SaveModel ();

    void CreateCondorBinaries ();

    void Benchmark ();

  private:
    typedef xtl::shared_ptr<script::Environment> ShellEnvironmentPtr;

    struct WaitedFile
    {
      stl::string     file_name;
      size_t          current_file_size;
      WaitFileHandler handler;

      WaitedFile (const char* in_file_name, const WaitFileHandler& in_handler)
        : file_name (in_file_name), current_file_size (1), handler (in_handler)
        {}
    };

    struct VisualModelResource
    {
      scene_graph::VisualModel::Pointer visual_model;
      media::rms::Binding               resource_binding;
    };

    typedef stl::list<WaitedFile> WaitedFiles;
    typedef stl::hash_map<stl::hash_key<const char*>, VisualModelResource> VisualModels;
    typedef stl::hash_map<stl::hash_key<const char*>, stl::string>         CondorTrajectoriesNames;
    typedef stl::hash_map<math::vec3f, stl::string>                        CalculatingTrajectoriesNuMap;
    typedef stl::hash_map<stl::hash_key<const char*>, math::vec3f>         CalculatingTrajectoriesNameMap;
    typedef stl::vector<math::vec3f>                                       BatchCalculations;

  private:
      //��������� ����������
    struct Configuration
    {
      stl::string                       win32_plugin_path;
      stl::string                       osx_plugin_path;
      stl::string                       linux_plugin_path;
      stl::string                       condor_path;                           //���� � bin ����� condor'�
      bool                              use_condor;                            //������������ �� condor
      stl::string                       author;                                //��� ������������ ���������
      bool                              visualize_new_calculations;            //��������� �� ���������� ����� ����� ��������
      size_t                            processors_count_for_calculation;      //������������ ���������� ����������� ��� �������� �� ��������� ����������
      common::VarRegistry               configuration_registry;                //������ ��������
      xtl::connection                   visualize_new_calculations_connection;

      Configuration ();

      void Load (const char* configuration_file_name);

      void OnChangeVisualizeNewCalculationsMode ();
    };

  private:
    ShellEnvironmentPtr               shell_environment;                    //��������� ���������� �����
    script::Shell                     shell;                                //���������� �������������
    scene_graph::Scene                scene;
    scene_graph::OrthoCamera::Pointer camera;
    render::Screen                    screen;
    Configuration                     configuration;                        //���������
    stl::string                       project_path;                         //���� � ������� ������������ �����
    stl::string                       condor_trajectory_requirements;       //condor ���������� ��� ������� �������� ����������
    syslib::Timer                     wait_files_timer;                     //������ �������� ������� ����� ������
    stl::string                       working_directory;                    //�����, ���������� ����������
    VisualModelResource               envelope;
    bool                              calculating_envelope;                 //��� �� ������ ������� ���������
    VisualModels                      trajectories;
    media::rms::ResourceManager       resource_manager;
    WaitedFiles                       waited_files;                         //��������� �����
    CondorTrajectoriesNames           condor_trajectories_names;            //�����, � ������� ����� ������������� ������������ condor'�� ����������
    size_t                            trajectory_vertex_per_second;         //������������������ ������� ���������� ��� �������� ����������
    CalculatingTrajectoriesNuMap      calculating_trajectories_nu_map;      //�������������� � ������ ������ ����������
    CalculatingTrajectoriesNameMap    calculating_trajectories_name_map;    //�������������� � ������ ������ ����������
    bool                              calculating_trajectories_coord;       //��� �� ������ ������� ��������� ����� ��� ��������� �������� ����������
    BatchCalculations                 batch_calculations;                   //������� ��� ��������� �������
};

//�������� ������
const char* get_spawn_error_name ();

namespace math
{

size_t hash (const math::vec3f& vector);

}

#endif
