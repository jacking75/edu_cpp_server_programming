INC = -I/usr/src/ACE_wrappers
CFLAGS = -g $(INC) 

SRCS = $(shell ls *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEF = -DACE_AS_STATIC_LIBS -Wall -ggdb

TARGET = libHalfNetwork.a

all : $(TARGET)

$(TARGET) : 
	g++ -c $(SRCS) $(INC) $(DEF)
	ar rcv $(TARGET) $(OBJS)
	ranlib $(TARGET)
	
clean :
	rm -rf $(OBJS) $(TARGET)

new : 
	$(MAKE) clean 
	$(MAKE) 

