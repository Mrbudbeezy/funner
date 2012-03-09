#include "shared.h"

using namespace media::animation;

class TestSpline
{
  public:
    typedef float result_type;

    void operator () (float time, float& value)
    {
      value = time;
    }
};

const char* get_type_name (const std::type_info& type)
{
  if (type == typeid (void))
    return "void";
  else if (type == typeid (float))
    return "float";
  else if (type == typeid (TestSpline))
    return "class TestSpline";
  else
    return type.name ();
}

void print (const Channel& channel)
{
  printf ("Channel parameter name is '%s'\n", channel.ParameterName ());
  printf ("Channel track type is '%s'\n", get_type_name (channel.TrackType ()));
  printf ("Channel value type is '%s'\n", get_type_name (channel.ValueType ()));

  if (channel.ValueType () != typeid (float))
    return;

  try
  {
    static const float TEST_TIME = 0.1f;

    float value = channel.Eval<float> (TEST_TIME);

    printf ("Channel eval for %.2f is %.2f, eval for return value is %.2f\n", TEST_TIME, channel.Eval<float> (TEST_TIME), value);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of channel_test:\n");
  
  try
  {
    Channel channel;

    print (channel);

    channel.SetParameterName ("position");

    TestSpline spline;

    channel.SetTrack (spline);

    print (channel);

    Channel copy_test (channel), clone_test (channel.Clone ());

    copy_test = clone_test;
    copy_test.Swap (clone_test);
    swap (copy_test, clone_test);

    TestSpline* channel_track = channel.Track<TestSpline> ();

    float channel_track_value;

    (*channel_track) (0.1f, channel_track_value);

    printf ("Channel track eval = %.2f\n", channel_track_value);

    Evaluator<float> evaluator = channel.Channel::Evaluator<float> ();

    printf ("Evaluator eval = %.2f\n", evaluator (0.1f));
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
