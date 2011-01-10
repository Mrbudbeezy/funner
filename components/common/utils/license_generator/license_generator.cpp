#include <cstdio>

#include <stl/stack>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>

#include <common/command_line.h>
#include <common/license_manager.h>
#include <common/property_map.h>
#include <common/string.h>
#include <common/xml_writer.h>

#include <license_manager_shared.h>

using namespace common;

const char* APPLICATION_NAME = "license-generator";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

struct Params;

//�����
struct Option
{
  CommandLine::SwitchHandler handler;       //���������� �����
  const char*                name;          //��� �������
  char                       short_name;    //�������� ���
  const char*                argument_name; //��� ���������
  const char*                tip;           //���������
};

typedef stl::vector<CheckFile> CheckFileArray;

//��������� �������
struct Params
{
  const Option*  options;            //������ �����
  size_t         options_count;      //���������� �����
  stl::string    result_license;     //���� ������������ ��������
  time_t         since_date;         //���� ������ �������� ��������
  time_t         till_date;          //���� ��������� �������� ��������
  StringArray    allowed_components; //������ ����������� �����������
  CheckFileArray check_files;        //������ ������ ��� �����������
  PropertyMap    properties;         //��������
  bool           force;              //������������� ������ ������������� �������� �������
  bool           silent;             //���������� ��������������
  bool           print_help;         //�������� �� �������
};

void parse_date (const char* date_string, time_t& result_date)
{
  static const char* METHOD_NAME = "parse_date";

  if (!rematch (date_string, "[:digit:][:digit:][:digit:][:digit:]-[:digit:][:digit:]-[:digit:][:digit:]"))
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid date string '%s' format, must be yyyy-mm-dd", date_string);

  tm tm_date;

  memset (&tm_date, 0, sizeof (tm_date));

  tm_date.tm_mday = atoi (date_string + 8);
  tm_date.tm_mon  = atoi (date_string + 5) - 1;
  tm_date.tm_year = atoi (date_string) - 1900;

  time_t date = mktime (&tm_date);

  if (date == -1)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't convert date string '%s'", date_string);

  result_date = date;
}

//��������� ��������� �� ���������
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", APPLICATION_NAME);
  printf ("  OPTIONS:\n");

  for (size_t i = 0; i < params.options_count; i++)
  {
    const Option& option = params.options [i];

    stl::string prefix = common::format (option.argument_name ? "    --%s=<%s>" : "    --%s", option.name, option.argument_name);

    if (option.short_name)
      prefix += common::format (option.argument_name ? ", -%c <%s>" : ", -%c", option.short_name, option.argument_name);

    printf ("%s ", prefix.c_str ());

    if (prefix.size () < HELP_STRING_PREFIX_LENGTH)
      for (size_t i=0, count=HELP_STRING_PREFIX_LENGTH-prefix.size (); i<count; i++)
        printf (" ");

    printf ("%s\n", option.tip);
  }

  params.print_help = true;
}

//��������� ������� �������� ��������
void command_line_license_period (const char* period, Params& params)
{
  static const char* METHOD_NAME = "command_line_license_period";

  if (params.till_date)
    throw xtl::format_operation_exception (METHOD_NAME, "license validity period is already setted");

  if (!rematch (period, "[:digit:]-[:digit:][:digit:]"))
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid period parameter '%s' format, must be y-mm", period);

  size_t years_period   = atoi (period),
         monthes_period = atoi (period + 2);

  if (monthes_period > 11)
    throw xtl::format_operation_exception (METHOD_NAME, "Period's month part can't be greater than 11");

  if (!params.since_date)
    time (&params.since_date);

  tm* since_time = gmtime (&params.since_date);

  since_time->tm_year += years_period;
  since_time->tm_mon  += monthes_period;

  if (since_time->tm_mon > 11)
  {
    since_time->tm_year++;
    since_time->tm_mon -= 12;
  }

  params.till_date = mktime (since_time);
}

//��������� ������� �������� ��������
void command_line_till_date (const char* date_string, Params& params)
{
  static const char* METHOD_NAME = "command_line_till_date";

  if (params.till_date)
    throw xtl::format_operation_exception (METHOD_NAME, "license validity period is already setted");

  parse_date (date_string, params.till_date);
}

//��������� ������� �������� ��������
void command_line_since_date (const char* date_string, Params& params)
{
  static const char* METHOD_NAME = "command_line_since_date";

  if (params.since_date)
    throw xtl::format_operation_exception (METHOD_NAME, "license since date is already setted");

  parse_date (date_string, params.since_date);
}

//��������� ������� �������� ��������
void command_line_result_license (const char* file_name, Params& params)
{
  if (!params.result_license.empty ())
    throw xtl::format_operation_exception ("command_line_result_license", "Duplicate result license switch");

  params.result_license = file_name;
}

//��������� ������ ����������� �����������
void command_line_allowed_components (const char* components_list, Params& params)
{
  params.allowed_components += split (components_list);
}

//��������� ������ ������ ��� �����������
void command_line_check_files (const char* check_files_list, Params& params)
{
  common::StringArray strings = split (check_files_list);
  
  for (size_t i=0; i<strings.Size (); i++)
    params.check_files.push_back (CheckFile (strings [i]));
}

//��������� ������ ������������� ������
void command_line_force (const char*, Params& params)
{
  params.force = true;
}

//��������� ������ ������
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//������������ ������ �� ����
stl::string date_to_string (time_t date)
{
  tm* tm_date = gmtime (&date);

  return format ("%04d-%02d-%02d", tm_date->tm_year + 1900, tm_date->tm_mon + 1, tm_date->tm_mday);
}

//�������� ������������ �����
void validate (Params& params)
{
  static const char* METHOD_NAME = "validate";

  if (params.result_license.empty ())
    throw xtl::format_operation_exception (METHOD_NAME, "Result license path not setted");

  if (!params.since_date)
    time (&params.since_date);

  if (!params.till_date)
    throw xtl::format_operation_exception (METHOD_NAME, "Incomplete license validity period specification, till date not setted");

  if (!params.force && difftime (params.till_date, params.since_date) < 0)
    throw xtl::format_operation_exception (METHOD_NAME, "Till date is earlier than since date");

  if (!params.silent && params.allowed_components.IsEmpty ())
    printf ("Warning: allowed components list is empty\n");
    
  if (!params.silent && params.check_files.empty ())
    printf ("Warning: check files list is empty\n");

  if (!params.force)
  {
    time_t current_time;

    time (&current_time);

    if (difftime (params.till_date, current_time) < 0)
      throw xtl::format_operation_exception (METHOD_NAME, "Till date already passed");
  }
}

//���������� ������� �� ������ ������ �������
void add_node_properties (Params& params, ParseNode node, const char* base_name)
{
  stl::string node_full_name = base_name;

  if (!node_full_name.empty ())
    node_full_name.append (".");

  node_full_name.append (node.Name ());

  if (node.AttributesCount ())
  {
    if (params.properties.IsPresent (node_full_name.c_str ()))
      throw xtl::format_operation_exception ("add_node_property", "Duplicate property '%s'", node_full_name.c_str ());

    stl::string property_value;

    for (size_t i = 0, count = node.AttributesCount (); i < count; i++)
      property_value += node.Attribute (i);

    params.properties.SetProperty (node_full_name.c_str (), property_value.c_str ());
  }

  for (ParseNode child = node.First (); child; child = child.Next ())
    add_node_properties (params, child, node_full_name.c_str ());
}

typedef xtl::shared_ptr<XmlWriter::Scope> ScopePtr;
typedef stl::stack<ScopePtr>              ScopeStack;

//��������� ��������
void generate_license (Params& params)
{
  unsigned char hash [16];

  calculate_license_hash (params.check_files, params.properties, hash);

    //������� �������� �����������
  params.properties.RemoveProperty ("AllowedComponents");

  stl::string hex_hash_string;

  for (size_t i = 0; i < 16; i++)
    hex_hash_string += common::format ("%02x", hash [i]);

  XmlWriter writer (params.result_license.c_str ());

  XmlWriter::Scope license_scope (writer, "License");

  writer.WriteAttribute ("LicenseHash", hex_hash_string.c_str ());

  for (size_t i = 0, count = params.properties.Size (); i < count; i++)
  {
    StringArray property_name_components = split (params.properties.PropertyName (i), ".");
    ScopeStack  scope_stack;

    for (size_t j = 0, components_count = property_name_components.Size (); j < components_count - 1; j++)
      scope_stack.push (ScopePtr (new XmlWriter::Scope (writer, property_name_components [j])));

    writer.WriteAttribute (property_name_components [property_name_components.Size () - 1], params.properties.GetString (i));

    while (!scope_stack.empty ())
      scope_stack.pop ();
  }

  if (!params.check_files.empty ())
  {
    XmlWriter::Scope check_files_scope (writer, "CheckFiles");

    for (size_t i = 0, count = params.check_files.size (); i < count; i++)
    {
      XmlWriter::Scope file_scope (writer, "File");

      writer.WriteAttribute ("Path", params.check_files [i].name);
    }
  }

  if (!params.allowed_components.IsEmpty ())
  {
    XmlWriter::Scope allowed_components_scope (writer, "AllowedComponents");

    for (size_t i = 0, count = params.allowed_components.Size (); i < count; i++)
    {
      XmlWriter::Scope component_scope (writer, "Component");

      writer.WriteAttribute ("Wildcard", params.allowed_components [i]);
    }
  }
}

int main (int argc, const char *argv[])
{
  try
  {
      //�������������
    Params params;

    static Option options [] = {
      {xtl::bind (&command_line_result_license,     _1, xtl::ref (params)), "output",      'o', "output",         "result license"},
      {xtl::bind (&command_line_allowed_components, _1, xtl::ref (params)), "components",  'c', "component-list", "allowed components list"},
      {xtl::bind (&command_line_check_files,        _1, xtl::ref (params)), "check-files", 'f', "file-list",      "check-files list"},
      {xtl::bind (&command_line_license_period,     _1, xtl::ref (params)), "period",      'p', "period",         "license validity period ('y-mm')"},
      {xtl::bind (&command_line_since_date,         _1, xtl::ref (params)), "since-date",  's', "date",           "since date"},
      {xtl::bind (&command_line_till_date,          _1, xtl::ref (params)), "till-date",   't', "date",           "till date"},
      {xtl::bind (&command_line_force,              _1, xtl::ref (params)), "force",       0,   0,                "force (ignore errors)"},
      {xtl::bind (&command_line_silent,             _1, xtl::ref (params)), "silent",      0,   0,                "silent mode"},
      {xtl::bind (&command_line_help,               _1, xtl::ref (params)), "help",        '?', 0,                "print help message"},
    };

    static const size_t options_count = sizeof (options) / sizeof (*options);

    params.options       = options;
    params.options_count = options_count;
    params.force         = false;
    params.silent        = false;
    params.print_help    = false;
    params.since_date    = 0;
    params.till_date     = 0;

    CommandLine command_line;

    for (size_t i = 0; i < params.options_count; i++)
      command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

      //������ ��������� ������
    command_line.Process (argc, argv);

      // --help ������ �������� ��������� ������
    if (params.print_help)
      return 0;

    if (command_line.ParamsCount () > 1)
      throw xtl::format_operation_exception ("", "No more than one input file allowed");

      //�������� ������������ �����
    validate (params);

      //���������� ������ ������� �������� � ������ ����������� � ��������
    stl::string since_date_string = date_to_string (params.since_date),
                till_date_string  = date_to_string (params.till_date),
                allowed_components_string;

    for (size_t i = 0, count = params.allowed_components.Size (); i < count; i++)
      allowed_components_string += params.allowed_components [i];
      
    params.properties.SetProperty ("SinceDate",         since_date_string.c_str ());
    params.properties.SetProperty ("TillDate",          till_date_string.c_str ());
    params.properties.SetProperty ("AllowedComponents", allowed_components_string.c_str ());

    if (command_line.ParamsCount ())
    {
      const char* source_xml = command_line.Param (0);

      Parser    p (source_xml, "xml");
      ParseLog  log = p.Log ();
      ParseNode root = p.Root ().First ("License");

      if (!root)
        log.Error (p.Root (), "No root 'License' tag");

      for (size_t i = 0; i < log.MessagesCount (); i++)
        switch (log.MessageType (i))
        {
          case ParseLogMessageType_Error:
          case ParseLogMessageType_FatalError:
            throw xtl::format_operation_exception ("", log.Message (i));
          default:
            break;
        }

      for (ParseNode node = root.First (); node; node = node.Next ())
        add_node_properties (params, node, "");
    }

      //��������� ��������
    generate_license (params);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
