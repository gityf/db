PROJECT = liboradb.so

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
CC = gcc -g
SHARED_FLAG = -fPIC -shared
CC += $(SHARED_FLAG)

# #oracle home
DB_HOME = $(ORACLE_HOME)

OBJDIR = ../obj/oracle/tcl/
SRCS  = .. .
SRCS += ../../deps/log

LIB  = -lpthread -lstdc++

ifeq ($(shell uname -s), SunOS)
LIB += -lsocket -lnsl -lintl
endif

LIB += -L$(DB_HOME)/lib/ -lclntsh -ltcl8.5
# LIB += -L$(DB_HOME)/libclntsh.so.11.1 -ltcl8.5

INC  = -I.
INC += -I../../deps
INC += -I..
INC += -I$(DB_HOME)/rdbms/public
INC += -I$(DB_HOME)/sdk/include

CPP_SRCS = $(foreach d,$(SRCS),$(wildcard $(d)/*.cpp))
OBJS = $(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(CPP_SRCS)))

all : obj $(PROJECT)
obj :
	mkdir -p $(addprefix $(OBJDIR), $(SRCS))

$(PROJECT) : $(OBJS) $(MODULE)
	test -d ./bin || mkdir -p ./bin
	$(CC) $(SHARED_FLAG) -o $@ $(OBJS) $(LIB)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "| Finish compilation Tcl Oracle Client Demo  |"
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
