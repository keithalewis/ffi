SRCS = ffi.cpp
TSTS = ffi.t.cpp ffi_parse.t.cpp ffi_traits.t.cpp ffi_dl.t.cpp

CXXFLAGS = -g -std=gnu++17
CFLAGS = -g
LDLIBS = -ldl -lffi -lgdbm -lm

ffi: $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LDLIBS)

ffi.t: $(TSTS)
	$(CXX) $(CXXFLAGS) -o $@ $(TSTS) $(LDLIBS)

test: ffi.t
	./ffi.t

clean:
	@rm ffi ffi.t

lac: lac.c lac_dbm.c lac_ffi.c lac_parse.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

lac.t: lac.t.c lac_dbm.c lac_dbm.t.c lac_ffi.c lac_ffi.t.c lac_parse.c lac_parse.t.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

lac.o: lac.c lac.h lac_ffi.h lac_parse.h

lac.t.o: lac.t.c lac_ffi.h lac_parse.h
