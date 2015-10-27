PROJECT = bin/sqlite3_unittest

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
CC=gcc -g 
SHARED_FLAG = -fPIC -shared

# #sqlite3 home
DB_HOME = .

OBJDIR = ./obj/sqlite3/
SRCS  = unittest
SRCS += .
SRCS += ../deps/log

LIB += -lstdc++ -lpthread -ldl

INC  = -I.
INC += -I../deps
INC += -I./unittest
INC += -I$(DB_HOME)

CPP_SRCS = $(foreach d,$(SRCS),$(wildcard $(d)/*.cpp))
C_SRCS = $(foreach d,.,$(wildcard $(d)/*.c))
OBJS = $(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(CPP_SRCS)))
OBJS += $(addprefix $(OBJDIR), $(patsubst %.c, %.o, $(C_SRCS)))
all : obj $(PROJECT)
obj :
	mkdir -p $(addprefix $(OBJDIR), $(SRCS))

$(PROJECT) : $(OBJS) $(MODULE)
	test -d ./bin || mkdir -p ./bin
	$(CC) -o $@ $(OBJS) $(LIB)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "|      Finish compilation Sqlite3 Demo       |"
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
