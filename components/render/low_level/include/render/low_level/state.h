#ifndef RENDER_LOW_LEVEL_STATE_HEADER
#define RENDER_LOW_LEVEL_STATE_HEADER

#include <render/low_level/object.h>
#include <render/low_level/common.h>

namespace render
{

namespace low_level
{

/*
===================================================================================================
    Input stage
===================================================================================================
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
///Семантика атрибута вершины
///////////////////////////////////////////////////////////////////////////////////////////////////
enum VertexAttributeSemantic
{
  VertexAttributeSemantic_Position,  //положение
  VertexAttributeSemantic_Normal,    //нормаль
  VertexAttributeSemantic_Color,     //цвет
  VertexAttributeSemantic_TexCoord0, //каналы текстурных координат
  VertexAttributeSemantic_TexCoord1,
  VertexAttributeSemantic_TexCoord2,
  VertexAttributeSemantic_TexCoord3,
  VertexAttributeSemantic_TexCoord4,
  VertexAttributeSemantic_TexCoord5,
  VertexAttributeSemantic_TexCoord6,  
  VertexAttributeSemantic_TexCoord7,
  
  VertexAttributeSemantic_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Типы входных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InputDataType
{
  InputDataType_Byte,       //однобайтовое целое знаковое число
  InputDataType_UByte,      //однобайтовое целое беззнаковое число
  InputDataType_Short,      //двубайтовое целое знаковое число
  InputDataType_UShort,     //двубайтовое целое беззнаковое число
  InputDataType_Int,        //четырёхбайтовое целое знаковое число
  InputDataType_UInt,       //четырёхбайтовое целое беззнаковое число
  InputDataType_ByteNorm,   //однобайтовое целое знаковое число (нормированное)
  InputDataType_UByteNorm,  //однобайтовое целое беззнаковое число (нормированное)
  InputDataType_ShortNorm,  //двубайтовое целое знаковое число (нормированное)
  InputDataType_UShortNorm, //двубайтовое целое беззнаковое число (нормированное)
  InputDataType_IntNorm,    //четырёхбайтовое целое знаковое число (нормированное)
  InputDataType_UIntNorm,   //четырёхбайтовое целое беззнаковое число (нормированное)
  InputDataType_Float,      //четырёхбайтовое вещественное число

  InputDataType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Форматы входных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InputDataFormat
{
  InputDataFormat_Value,                           //одинарное значение
  InputDataFormat_Vector1 = InputDataFormat_Value, 
  InputDataFormat_Vector2,                         //вектор из двух элементов
  InputDataFormat_Vector3,                         //вектор из трёх элементов
  InputDataFormat_Vector4,                         //вектор из четырёх элементов
  
  InputDataFormat_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершинный атрибут
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexAttribute
{
  const char*      semantic;                //семантика вершинного атрибута
  InputDataFormat  format;                  //формат данных
  InputDataType    type;                    //тип элементов
  unsigned int     slot;                    //номер слота с вершинным буфером
  unsigned int     offset;                  //смещение от начала вершинного буфера
  unsigned int     stride;                  //шаг в вершинном буфере
  unsigned int     instance_data_step_rate; //количество отрисованных инстансов после которого произойдет смещение на 1
  bool             is_per_instance;         //является ли атрибут инстанцируемым
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор входного потока
///////////////////////////////////////////////////////////////////////////////////////////////////
struct InputLayoutDesc
{
  unsigned int           vertex_attributes_count; //количество вершинных атрибутов
  const VertexAttribute* vertex_attributes;       //вершинные атрибуты
  InputDataType          index_type;              //тип индексов в вершинном буфере
  unsigned int           index_buffer_offset;     //смещение в индексном буфере
};

/*
===================================================================================================
    Shader stage
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Типы шейдерных параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ProgramParameterType
{
  ProgramParameterType_Int,         //целое число
  ProgramParameterType_Float,       //вещественное число
  ProgramParameterType_Int2,        //вектор из 2-х целых чисел
  ProgramParameterType_Float2,      //вектор из 2-х вещественных чисел
  ProgramParameterType_Int3,        //вектор из 3-х целых чисел
  ProgramParameterType_Float3,      //вектор из 3-х вещественных чисел
  ProgramParameterType_Int4,        //вектор из 4-х целых чисел
  ProgramParameterType_Float4,      //вектор из 4-х вещественных чисел
  ProgramParameterType_Float2x2,    //матрица 2x2 вещественных чисел
  ProgramParameterType_Float3x3,    //матрица 3x3 вещественных чисел
  ProgramParameterType_Float4x4,    //матрица 4x4 вещественных чисел

  ProgramParameterType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметр шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameter
{
  const char*          name;   //имя константы
  ProgramParameterType type;   //тип константы
  unsigned int         slot;   //номер слота с константым буфером
  unsigned int         count;  //количество элементов массива
  unsigned int         offset; //смещение относительно начала константного буфера
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор расположения параметров шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParametersLayoutDesc
{
  unsigned int            parameters_count; //количество параметров шейдера
  const ProgramParameter* parameters;       //параметры шейдера
};

/*
===================================================================================================
    Shader stage (sampling)
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Типы фильтров
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TexMinFilter
{
  TexMinFilter_Default,         //фильтрация выбирается в соответствии с предпочтениями устройства визуализации
  TexMinFilter_Point,           //точечная фильтрация
  TexMinFilter_Linear,          //линейная фильтрация
  TexMinFilter_PointMipPoint,   //точечная фильтрация по ближайшему мипу
  TexMinFilter_LinearMipPoint,  //линейная фильтрация по ближайшему мипу
  TexMinFilter_PointMipLinear,  //точечная фильтрация с линейной интерполяцией мипов
  TexMinFilter_LinearMipLinear, //трилинейная фильтрация
  
  TexMinFilter_Num
};

enum TexMagFilter
{
  TexMagFilter_Default, //фильтрация выбирается в соответствии с предпочтениями устройства визуализации
  TexMagFilter_Point,   //точечная фильтрация
  TexMagFilter_Linear,  //линейная фильтрация

  TexMagFilter_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режимы свёртки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TexcoordWrap
{
  TexcoordWrap_Repeat,        //повторение
  TexcoordWrap_Mirror,        //зеркальное отражение
  TexcoordWrap_Clamp,         //отсечение
  TexcoordWrap_ClampToBorder, //отсечение по цвету границы
  
  TexcoordWrap_Num,
  
  TexcoordWrap_Default = TexcoordWrap_Repeat
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор сэмплирования
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SamplerDesc
{
  TexMinFilter min_filter;           //фильтрация, используемая если размер текселя меньше размера пикселя
  TexMagFilter mag_filter;           //фильтрация, используемая если размер текселя больше размера пикселя
  TexcoordWrap wrap_u;               //свёртка по координате U
  TexcoordWrap wrap_v;               //свёртка по координате V
  TexcoordWrap wrap_w;               //свёртка по координате W
  CompareMode  comparision_function; //функция сравнения для depth подключенных текстур
  float        mip_lod_bias;         //смещение от вычисленного mip-уровня
  float        min_lod;              //минимальный mip-уровень
  float        max_lod;              //максимальный mip-уровень
  unsigned int max_anisotropy;       //максимальный уровень анизотропии
  float        border_color [4];     //цвет бордюра
};

/*
===================================================================================================
    Rasterizer stage
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим заливки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FillMode
{
  FillMode_Wireframe, //каркасная модель заливки
  FillMode_Solid,     //сплошная модель заливки
  
  FillMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим отсечения граней
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CullMode
{
  CullMode_None,  //без отсечения
  CullMode_Front, //отсекать передние грани
  CullMode_Back,  //отсекать задние грани
  
  CullMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор уровня растеризации
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizerDesc
{
  FillMode fill_mode;               //режим заливки
  CullMode cull_mode;               //режим отсечения
  bool     front_counter_clockwise; //режим определения передних граней (по часовой стрелке - true, против - false)
  int      depth_bias;              //значение прибавляемое к каждому пикселу 
  bool     scissor_enable;          //включено ли scissor-отсечение
  bool     multisample_enable;      //включено ли мультисэмплирование
  bool     antialiased_line_enable; //включен ли антиалиасинг линий
};

/*
===================================================================================================
    Output stage (depth-stencil)
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции, выполняемые над элементами буфера трафарета
///////////////////////////////////////////////////////////////////////////////////////////////////
enum StencilOperation
{
  StencilOperation_Keep,      //значене элемента не изменяется
  StencilOperation_Zero,      //значение элемента устанавливается равным нулю
  StencilOperation_Replace,   //значение элемента заменяется значением stencil_reference
  StencilOperation_Increment, //значение увеличивается на 1 и обрезается по верхнему пределу
  StencilOperation_Decrement, //значение уменьшается на 1 и обрезается по нижнему пределу
  StencilOperation_Invert,    //значение инвертируется
  
  StencilOperation_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор буфера трафарета
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StencilDesc
{
  CompareMode      stencil_compare_mode;   //режим сравнения значений в stencil-test
  StencilOperation stencil_fail_operation; //операция, выполняемая над элементом буфера трафарета в случае провала stencil-test
  StencilOperation depth_fail_operation;   //операция, выполняемая над элементом буфера трафарета в случае провала depth-test
  StencilOperation stencil_pass_operation; //операция, выполняемая над элементом буфера трафарета в случае прохождения stencil-test
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Виды граней
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FaceMode
{
  FaceMode_Front, //передняя грань
  FaceMode_Back,  //задняя грань
  
  FaceMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор уровня попиксельного отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DepthStencilDesc
{
  bool          depth_test_enable;      //включён ли тест глубины
  bool          depth_write_enable;     //включена ли запись в буфер глубины
  bool          stencil_test_enable;    //включён ли тест трафарета
  CompareMode   depth_compare_mode;     //режим сравнения в тесте глубины
  unsigned char stencil_read_mask;      //маска, накладываемая на значения буфера трафарета при чтении
  unsigned char stencil_write_mask;     //маска, накладываемая на значения буфера трафарета при записи
  StencilDesc   stencil_desc [FaceMode_Num]; //дескрипторы буфера трафарета для различных видов граней
};

/*
===================================================================================================
    Output stage (blending)
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вид операции смешивания цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BlendOperation
{
  BlendOperation_Add,                //arg1 + arg2
  BlendOperation_Subtraction,        //arg1 - arg2
  BlendOperation_ReverseSubtraction, //arg2 - arg1
  BlendOperation_Min,                //min (arg1, arg2)
  BlendOperation_Max,                //max (arg1, arg2)
  
  BlendOperation_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аргумент функции смешивания цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BlendArgument
{
  BlendArgument_Zero,                    //0
  BlendArgument_One,                     //1
  BlendArgument_SourceColor,             //цвет источника
  BlendArgument_SourceAlpha,             //альфа источника
  BlendArgument_InverseSourceColor,      //1 - цвет источника
  BlendArgument_InverseSourceAlpha,      //1 - альфа источника
  BlendArgument_DestinationColor,        //цвет приёмника
  BlendArgument_DestinationAlpha,        //альфа приёмника
  BlendArgument_InverseDestinationColor, //1 - цвет приёмника
  BlendArgument_InverseDestinationAlpha, //1 - альфа приёмника
  
  BlendArgument_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги маски записи цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ColorWriteFlag
{
  ColorWriteFlag_Red   = 1, //необходимо ли записывать красную компоненту цвета
  ColorWriteFlag_Green = 2, //необходимо ли записывать зелёную компоненту цвета
  ColorWriteFlag_Blue  = 4, //необходимо ли записывать синюю компоненту цвета
  ColorWriteFlag_Alpha = 8, //необходимо ли записывать альфа компоненту цвета
  ColorWriteFlag_All   = ColorWriteFlag_Red | ColorWriteFlag_Green | ColorWriteFlag_Blue | ColorWriteFlag_Alpha
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор уровня смешивания цветов цели рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderTargetBlendDesc
{
  bool           blend_enable;                      //включено ли смешивание цветов
  BlendOperation blend_color_operation;             //вид операции смешивания цветов
  BlendArgument  blend_color_source_argument;       //аргумент функции смешивания цветов, выбранный из источника цвета
  BlendArgument  blend_color_destination_argument;  //аргумент функции смешивания цветов, выбранный из приёмника цвета
  BlendOperation blend_alpha_operation;             //вид операции смешивания альфа компонент
  BlendArgument  blend_alpha_source_argument;       //аргумент функции смешивания альфа компонент, выбранный из источника цвета
  BlendArgument  blend_alpha_destination_argument;  //аргумент функции смешивания альфа компонент, выбранный из приёмника цвета
  unsigned char  color_write_mask;                  //маска записи цветов
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор уровня смешивания цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BlendDesc
{
  bool                  sample_alpha_to_coverage;                         //работа с альфа при мультисэмплинге
  bool                  independent_blend_enable;                         //включено ли независимое смешивание цветов для целей рендеринга
  RenderTargetBlendDesc render_target [DEVICE_RENDER_TARGET_SLOTS_COUNT]; //настройки смешивания цветов целей рендеринга
};

/*
===================================================================================================
    Классы состояний
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Шаблонный класс состояния устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Desc> class IStateTemplate: virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение/получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (Desc&) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Шаблонный класс состояния устройства отрисовки без возможности запроса дескриптора состояния
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Desc> class ILayoutTemplate: virtual public IObject {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Классы состояний устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef ILayoutTemplate<InputLayoutDesc>             IInputLayout;
typedef ILayoutTemplate<ProgramParametersLayoutDesc> IProgramParametersLayout;
typedef IStateTemplate<SamplerDesc>                  ISamplerState;
typedef IStateTemplate<RasterizerDesc>               IRasterizerState;
typedef IStateTemplate<DepthStencilDesc>             IDepthStencilState;
typedef IStateTemplate<BlendDesc>                    IBlendState;

}

}

#endif
