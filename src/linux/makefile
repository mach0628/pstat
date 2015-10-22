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
# 用户设定
#
# Adjust the following if necessary; EXECUTABLE is the target
# executable's filename, and LIBS is a list of libraries to link in
# (e.g. alleg, stdcx, iostr, etc). You can override these on make's
# command line of course, if you prefer to do it that way.
# 
# 如果需要，调整下面的东西。 EXECUTABLE 是目标的可执行文件名， LIBS
# 是一个需要连接的程序包列表（例如 alleg, stdcx, iostr 等等）。当然你
# 可以在 make 的命令行覆盖它们，你愿意就没问题。
# 
CC=gcc
CXX=gcc
EXECUTABLE := pstat
LIBS :=  stdc++  pcap pthread
# 如果是生成动态连接库
LINKFLAGS= 
# Now alter any implicit rules' variables if you like, e.g.:
#
# 现在来改变任何你想改动的隐含规则中的变量，例如

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
