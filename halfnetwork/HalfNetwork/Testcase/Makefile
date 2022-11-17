INC = -I/usr/src/ACE_wrappers -I/usr/src/ACE_wrappers/HalfNetwork
LIB = -lHalfNetwork -ldl -lpthread -lACE -L/usr/src/ACE_wrappers/lib
DEF = -DACE_AS_STATIC_LIBS
SRCS = main.cpp EchoBackServer.cpp
OBJSS = main.cpp EchoBack.o

TARGET = TestCase

all : $(TARGET)

$(TARGET) :
	g++ -o $(TARGET) $(SRCS) $(INC) $(LIB) $(DEF)

clean :
	rm -f $(OBJS) $(TARGET)

new :
	$(MAKE) clean
	$(MAKE)


