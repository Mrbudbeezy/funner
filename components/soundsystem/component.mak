###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SOUNDSYSTEM_COMPONENTS SOUNDSYSTEM_TESTS

#���� �1 - �����
SOUNDSYSTEM_COMPONENTS.TYPE       := package
SOUNDSYSTEM_COMPONENTS.COMPONENTS := manager openaldevice sgplayer

#���� �2 - �����
SOUNDSYSTEM_TESTS.TYPE       := package
SOUNDSYSTEM_TESTS.COMPONENTS := manager/tests openaldevice/tests sgplayer/tests
