INC = -I/usr/src/ACE_wrappers -I/usr/src/ACE_wrappers/HalfNetwork
LIB = -lHalfNetwork -ldl -lpthread -lACE -L/usr/src/ACE_wrappers/lib
DEF = -DACE_AS_STATIC_LIBS
SRCS = TestClient.cpp NetworkThread.cpp EchoBackChecker.cpp
OBJSS = TestClient.o NetworkThread.o EchoBackChecker.o

TARGET = TestClient

all : $(TARGET)

$(TARGET) :
	g++ -o $(TARGET) $(SRCS) $(INC) $(LIB) $(DEF)

clean :
	rm -f $(OBJS) $(TARGET)

new :
	$(MAKE) clean
	$(MAKE)


