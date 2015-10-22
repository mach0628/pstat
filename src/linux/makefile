######################################
# 
# Generic makefile 
# 
# by George Foot 
# email: george.foot@merton.ox.ac.uk 
# 
# Copyright (c) 1997 George Foot 
# All rights reserved. 
# 
# No warranty, no liability; 
# you use this at your own risk. 
# 
# You are free to modify and 
# distribute this without giving 
# credit to the original author. 
# 
######################################

### Customising
# �û��趨
#
# Adjust the following if necessary; EXECUTABLE is the target
# executable's filename, and LIBS is a list of libraries to link in
# (e.g. alleg, stdcx, iostr, etc). You can override these on make's
# command line of course, if you prefer to do it that way.
# 
# �����Ҫ����������Ķ����� EXECUTABLE ��Ŀ��Ŀ�ִ���ļ����� LIBS
# ��һ����Ҫ���ӵĳ�����б������� alleg, stdcx, iostr �ȵȣ�����Ȼ��
# ������ make �������и������ǣ���Ը���û���⡣
# 
CC=gcc
CXX=gcc
EXECUTABLE := pstat
LIBS :=  stdc++  pcap pthread
# ��������ɶ�̬���ӿ�
LINKFLAGS= 
# Now alter any implicit rules' variables if you like, e.g.:
#
# �������ı��κ�����Ķ������������еı���������

CFLAGS := -g -Wall
CXXFLAGS := $(CFLAGS) 


RM-F := rm -f

# You shouldn't need to change anything below this point.
#

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cc,$(MISSING_DEPS)))
CPPFLAGS += 

.PHONY : everything deps objs clean veryclean rebuild

everything : $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) *.o
	@$(RM-F) *.d
	@$(RM-F) $(EXECUTABLE)
veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(EXECUTABLE) : $(OBJS)
	$(CC) $(LINKFLAGS) -o $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))