LDFLAGS = -ldl -lffi

ffi: ffi.cpp
	$(CXX) ffi.cpp   -o ffi $(LDFLAGS)

clean:
	rm ffi
