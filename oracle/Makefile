PROJECT = bin/oracle_test

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
CC=gcc -g
SHARED_FLAG = -fPIC -shared

# #oracle home
# ORACLE_HOME=/home/oracle/oracleClient/instantclient_11_2
DB_HOME = $(ORACLE_HOME)

OBJDIR = ./obj/oracle/
SRCS  = test
SRCS += .
SRCS += ../deps/log

LIB  = -lpthread -lstdc++

ifeq ($(shell uname -s), SunOS)
LIB += -lsocket -lnsl -lintl
endif

LIB += -L$(DB_HOME)/lib/ -lclntsh

INC  = -I.
INC += -I../deps
INC += -I./test
INC += -I$(DB_HOME)/rdbms/public

CPP_SRCS = $(foreach d,$(SRCS),$(wildcard $(d)/*.cpp))
OBJS = $(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(CPP_SRCS)))

all : obj $(PROJECT)
obj :
	mkdir -p $(addprefix $(OBJDIR), $(SRCS))

$(PROJECT) : $(OBJS) $(MODULE)
	test -d ./bin || mkdir -p ./bin
	$(CC) -o $@ $(OBJS) $(LIB)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "|      Finish compilation Oracle Demo        |"
	@echo "+--------------------------------------------+"
	@echo "|      copyright(c) voipman@qq.com           |"
	@echo "+--------------------------------------------+"

clean:
	rm -rf $(OBJS)


$(OBJDIR)%.o : %.cpp
	$(CC) $(INC) -c $< -o $@
$(OBJDIR)%.o : %.c
	$(CC) $(INC) -c $< -o $@
$(OBJDIR)%.o : %.cc
	$(CC) $(INC) -c $< -o $@
