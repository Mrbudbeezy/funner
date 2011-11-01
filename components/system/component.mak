###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

TARGETS.android  += SYSTEMLIB.UTILS.ANDROID_LAUNCHER
TARGETS.tabletos += SYSTEMLIB.UTILS.TABLETOS_LAUNCHER

#���� �1 - System library sources
SYSTEMLIB.SOURCES.TYPE                              := static-lib
SYSTEMLIB.SOURCES.NAME                              := funner.system
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      += sources
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      += sources/platform
SYSTEMLIB.SOURCES.SOURCE_DIRS                       += sources/wrappers
SYSTEMLIB.SOURCES.SOURCE_DIRS                       += sources/platform/default
SYSTEMLIB.SOURCES.IMPORTS                           := compile.system
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS                 := sources/platform/windows sources/platform/windows/application sources/platform/windows/thread sources/platform/windows/non_unistd
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS                := sources/platform/pthread sources/platform/unistd sources/platform/message_queue
SYSTEMLIB.SOURCES.msvc.COMPILER_CFLAGS              := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS            := sources/platform/no_windows
SYSTEMLIB.SOURCES.android.SOURCE_DIRS               := sources/platform/android sources/platform/message_queue
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS                := sources/platform/carbon
SYSTEMLIB.SOURCES.carbon.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.iphone.SOURCE_DIRS                := sources/platform/iphone
SYSTEMLIB.SOURCES.iphone.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.x11.SOURCE_DIRS                   := sources/platform/x11
SYSTEMLIB.SOURCES.x11.IMPORTS                       := compile.media.image
SYSTEMLIB.SOURCES.bada_simulator.SOURCE_DIRS        := sources/platform/windows sources/platform/windows/thread sources/platform/windows/non_unistd sources/platform/bada/startup_simulator
#SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/pthread
SYSTEMLIB.SOURCES.bada.SOURCE_DIRS                  := sources/platform/bada
SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/no_threads sources/platform/bada/startup_device
SYSTEMLIB.SOURCES.beagleboard.IMPORTS               := compile.extern.beagleboard
SYSTEMLIB.SOURCES.meego.IMPORTS                     := compile.extern.meego
SYSTEMLIB.SOURCES.tabletos.SOURCE_DIRS              := sources/platform/tabletos

#���� �2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers tests/thread
SYSTEMLIB.TESTS.IMPORTS          := compile.system link.system link.common.zip_file_system
SYSTEMLIB.TESTS.x11.IMPORTS      := link.media.image.ani_cursor_loader

#���� �3 - Android launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.TYPE          := android-pak
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.NAME          := funner.application
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.DLL_DIRS       = $(PLATFORM_DIR)/arch-arm/usr/lib
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.DLLS          := android
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.SOURCE_DIRS   := utils/android_launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.MANIFEST_FILE := utils/android_launcher/AndroidManifest.xml
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.RES_DIR       := utils/android_launcher/res

#���� �4 - Tabletos launcher
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.TYPE          := tabletos-bar
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.NAME          := funner.application
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.SOURCE_DIRS   := utils/tabletos_launcher
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.LIBS          := socket EGL GLESv1_CM png freetype screen bps pps OpenAL asound
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.RES_DIR       := utils/tabletos_launcher/res
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.MANIFEST_FILE := utils/tabletos_launcher/res/bar-descriptor.xml
