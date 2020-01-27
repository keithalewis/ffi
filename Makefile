SRCS = ffi.cpp
TSTS = ffi.t.cpp ffi_parse.t.cpp ffi_traits.t.cpp ffi_dl.t.cpp

CXXFLAGS = -g -std=gnu++17
CFLAGS = -g
LDLIBS = -ldl -lffi

ffi: $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LDLIBS)

ffi.t: $(TSTS)
	$(CXX) $(CXXFLAGS) -o $@ $(TSTS) $(LDLIBS)

test: ffi.t
	./ffi.t

clean:
	@rm ffi ffi.t

ffi: ffi.cpp ffi_cif.h ffi_traits.h ffi_parse.h

ffi.t: ffi.t.cpp ffi_parse.t.cpp ffi_parse.h ffi_dl.t.cpp ffi_dl.h ffi_parse.t.cpp ffi_parse.h
