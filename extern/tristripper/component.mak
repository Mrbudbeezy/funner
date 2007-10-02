###################################################################################################
#������ �����
###################################################################################################
TARGETS := TRISTRIPPER TRISTRIPPER-TESTS

#���� �1 - TRISTRIPPER
TRISTRIPPER.TYPE                    := static-lib     #��� ����
TRISTRIPPER.NAME                    := libtristripper #��� ��������� �����
TRISTRIPPER.INCLUDE_DIRS            := include        #������������� ���� � ��������� c ������������� �������
TRISTRIPPER.SOURCE_DIRS             := src            #������������� ���� � ��������� c ��������� �������
TRISTRIPPER.LIB_DIRS                :=                #������������� ���� � ��������� � ������������
TRISTRIPPER.LIBS                    :=                #������ ������������ ��� �������� ���������
TRISTRIPPER.COMPILER_DEFINES        :=  					    #����������� ������
TRISTRIPPER.COMPILER_CFLAGS         :=                #����� ������ (������� �� ������������)

#���� �2 - TRISTRIPPER-TESTS
TRISTRIPPER-TESTS.TYPE              := application    #��� ����
TRISTRIPPER-TESTS.NAME              := test           #��� ��������� �����
TRISTRIPPER-TESTS.INCLUDE_DIRS      := include        #������������� ���� � ��������� c ������������� �������
TRISTRIPPER-TESTS.SOURCE_DIRS       := test           #������������� ���� � ��������� c ��������� �������
TRISTRIPPER-TESTS.LIB_DIRS          :=                #������������� ���� � ��������� � ������������
TRISTRIPPER-TESTS.LIBS              := libtristripper #������ ������������ ��� �������� ���������
TRISTRIPPER-TESTS.COMPILER_DEFINES  := WIN32 					    #����������� ������
TRISTRIPPER-TESTS.COMPILER_CFLAGS   :=                #����� ������ (������� �� ������������)
