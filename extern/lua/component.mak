###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := EXTERN.LUALIB EXTERN.LUA_UTILS

#���� �1 - LUALIB
EXTERN.LUALIB.TYPE                 := static-lib        #��� 楫�
EXTERN.LUALIB.NAME                 := funner.extern.lua        #��� ��室���� 䠩��
EXTERN.LUALIB.INCLUDE_DIRS         := include				   #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
EXTERN.LUALIB.SOURCE_DIRS          := sources           #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
EXTERN.LUALIB.LIB_DIRS             :=                   #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
EXTERN.LUALIB.LIBS                 :=                   #���᮪ �ᯮ��㥬�� �� �������� ������⥪
EXTERN.LUALIB.COMPILER_DEFINES     :=  								 #��।������ ᡮન
EXTERN.LUALIB.msvc.COMPILER_CFLAGS := -TP               #����� ᡮન (������� �� ���䨣��樨)
EXTERN.LUALIB.g++.COMPILER_CFLAGS  := -x c++

#���� �2 - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                 := test-suite     	 #��� 楫�
EXTERN.LUA_UTILS.INCLUDE_DIRS         := include	sources  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
EXTERN.LUA_UTILS.SOURCE_DIRS          := utils          	 #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
EXTERN.LUA_UTILS.LIB_DIRS             :=                  #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
EXTERN.LUA_UTILS.LIBS                 := funner.extern.lua       #���᮪ �ᯮ��㥬�� �� �������� ������⥪
EXTERN.LUA_UTILS.COMPILER_DEFINES     :=  							   #��।������ ᡮન
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS := -TP              #����� ᡮન (������� �� ���䨣��樨)
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS  := -x c++
