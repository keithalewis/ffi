// ffi_dl.h - Dynamic load library
#pragma once
#include <dlfcn.h>
#include <memory>

namespace ffi {

	// dynamic link library
	class dl {
		std::shared_ptr<void> lib;
	public:
		dl(const char* lib, int flags = RTLD_LAZY)
			: lib(dlopen(lib, flags), [](void* p) { if (p) dlclose(p); })
		{ 
			if (nullptr == this->lib.get()) {
				std::runtime_error(dlerror());
			}
		}
		operator void*() 
		{
			return lib.get();
		}
		void* sym(const char* name) const
		{
			return dlsym(lib.get(), name);
		}
	};

};
