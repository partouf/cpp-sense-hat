SAMPLES := samples/cylon samples/blank samples/sensors
LIB := lib/libsense-hat.so
CXXFLAGS := -Isrc -Os
PICFLAGS := -fPIC
SO_LDFLAGS := -shared -rdynamic

all: $(SAMPLES)

samples/blank: src/blank.cpp src/sense-hat.h $(LIB)
	 $(CXX) $(CXXFLAGS) -o samples/blank src/blank.cpp -Llib -lsense-hat

samples/cylon: src/cylon.cpp src/sense-hat.h $(LIB)
	$(CXX) $(CXXFLAGS) -o samples/cylon src/cylon.cpp -Llib -lsense-hat

samples/sensors: src/sensors.cpp src/sense-hat.h $(LIB)
	$(CXX) $(CXXFLAGS) -o samples/sensors src/sensors.cpp -Llib -lsense-hat

$(LIB): src/sense-hat.o
	$(CXX) $(SO_LDFLAGS) -o $@ $< -lRTIMULib

src/sense-hat.so: src/sense-hat.cpp src/sense-hat.h src/sense-hat-linux.cpp src/sense-hat-linux.h
	$(CXX) $(CXXFLAGS) $(PICFLAGS) $(SO_LDFLAGS) -o $@ src/sense-hat.cpp src/sense-hat-linux.cpp

clean:
	rm -f $(SAMPLES)
	rm -f $(LIB)
