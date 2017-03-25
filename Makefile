LIB := lib/libsense-hat.a
LINKOBJ := src/sense-hat-linux.o src/sense-hat.o
CXXFLAGS := -Isrc -std=c++11
PICFLAGS := -fPIC
SO_LDFLAGS := -shared -rdynamic -std=c++14
LINKLIB := -Llib -lsense-hat
RTIMU := -lRTIMULib

all: $(LIB)

$(LIB): $(LINKOBJ)
	ar r $(LIB) $(LINKOBJ)
	ranlib $(LIB)

src/sense-hat.o: src/sense-hat.cpp
	$(CXX) -c $(CXXFLAGS) -o src/sense-hat.o src/sense-hat.cpp

src/sense-hat-linux.o: src/sense-hat-linux.cpp
	$(CXX) -c $(CXXFLAGS) -o src/sense-hat-linux.o src/sense-hat-linux.cpp

clean:
	rm -f $(SAMPLES)
	rm -f $(LIB)
