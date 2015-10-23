PROJECT = bin/mysql_unittest

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
CC=gcc -g -std=c++11
SHARED_FLAG = -fPIC -shared

# #mysql home
DB_HOME = /usr/include/mysql

OBJDIR = ./obj/mysql/
SRCS  = unittest
SRCS += .
SRCS += ../deps/log

LIB  = -L/usr/lib64/mysql -lmysqlclient
LIB += -lstdc++ -lpthread

INC  = -I.
INC += -I../deps
INC += -I./unittest
INC += -I$(DB_HOME)

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
	@echo "|      Finish compilation MySQL Demo         |"
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
