###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.SOURCES PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA

#���� - sources
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.SOURCES.TYPE        := static-lib
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.SOURCES.NAME        := funner.push_notifications.google_push_notifications
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.SOURCES.SOURCE_DIRS := sources
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.SOURCES.IMPORTS			:= compile.push_notifications.core compile.system

#���� - java
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA.TYPE        := android-jar
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA.NAME        := funner.google_push_notifications
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA.SOURCE_DIRS := java_sources
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA.JARS        := funner.application
PUSH_NOTIFICATIONS.GOOGLE_PUSH_NOTIFICATIONS.JAVA.IMPORTS     := compile.extern.gcm
