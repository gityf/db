PROJECT = bin/tcbdb_unittest

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
CC=gcc -g
SHARED_FLAG = -fPIC -shared

# #leveldb home
DB_HOME = ../deps/tokyocabinet-1.4.47/inbin/

OBJDIR = ./obj/leveldb/
SRCS  = unittest
SRCS += .
SRCS += ../deps/log

LIB  = $(DB_HOME)/lib/libtokyocabinet.a
LIB += -lstdc++ -lpthread -lm -lz -lbz2

INC  = -I.
INC += -I../deps
INC += -I./unittest
INC += -I$(DB_HOME)/include

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
	@echo "|      Finish compilation tcbdb Demo         |"
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
