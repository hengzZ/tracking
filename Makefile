TARGET=demo
CXX=g++

CXXFLAGS=-O3 -Wall \
	 -std=c++11 \
	 -I/usr/local/include 
LIBS=-L/usr/local/lib \
     -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui


ODIR=obj
SRCS=$(wildcard ./*.cpp)
OBJS=$(patsubst %.cpp, $(ODIR)/%.o, $(notdir $(SRCS)))


all: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(ODIR)/%.o: ./%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)


dirs:
	mkdir -p $(ODIR)

.PHONY:clean

clean:
	rm -rf $(TARGET) $(ODIR)
