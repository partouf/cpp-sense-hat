LIB := lib/libsense-hat.a
LINKOBJ := src/sense-hat-linux.o
CXXFLAGS := -Isrc -std=c++11
PICFLAGS := -fPIC
SO_LDFLAGS := -shared -rdynamic -std=c++11
LINKLIB := -Llib -lsense-hat
RTIMU := -lRTIMULib

all: $(LIB)

samples/blank: src/blank.cpp $(LIB)
	$(CXX) -Os $(CXXFLAGS) -o samples/blank src/blank.cpp $(LINKLIB)

samples/cylon: src/cylon.cpp $(LIB)
	$(CXX) -Os $(CXXFLAGS) -o samples/cylon src/cylon.cpp $(LINKLIB)

samples/sensors: src/sensors.cpp $(LIB)
	$(CXX) -Os $(CXXFLAGS) -o samples/sensors src/sensors.cpp $(LINKLIB)

$(LIB): $(LINKOBJ)
	ar r $(LIB) $(LINKOBJ)
	ranlib $(LIB)

src/sense-hat-linux.o: src/sense-hat-linux.cpp
	$(CXX) -c $(CXXFLAGS) -o src/sense-hat-linux.o src/sense-hat-linux.cpp

clean:
	rm -f $(SAMPLES)
	rm -f $(LIB)
