###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := LUALIB LUA_UTILS

#���� �1 - LUALIB
LUALIB.TYPE             := static-lib        #��� 楫�
LUALIB.NAME             := extern.lua        #��� ��室���� 䠩��
LUALIB.INCLUDE_DIRS     := include				   #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
LUALIB.SOURCE_DIRS      := sources           #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
LUALIB.LIB_DIRS         :=                   #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
LUALIB.LIBS             :=                   #���᮪ �ᯮ��㥬�� �� �������� ������⥪
LUALIB.COMPILER_DEFINES :=  								 #��।������ ᡮન
LUALIB.COMPILER_CFLAGS  := -TP               #����� ᡮન (������� �� ���䨣��樨)

#���� �2 - LUA_UTILS
LUA_UTILS.TYPE             := test-suite     	 #��� 楫�
LUA_UTILS.INCLUDE_DIRS     := include	sources  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
LUA_UTILS.SOURCE_DIRS      := utils          	 #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
LUA_UTILS.LIB_DIRS         :=                  #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
LUA_UTILS.LIBS             := extern.lua       #���᮪ �ᯮ��㥬�� �� �������� ������⥪
LUA_UTILS.COMPILER_DEFINES :=  							   #��।������ ᡮન
LUA_UTILS.COMPILER_CFLAGS  := -TP              #����� ᡮન (������� �� ���䨣��樨)
