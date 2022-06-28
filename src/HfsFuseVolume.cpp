#include "HfsFuseVolume.h"
#include "darling-dmg/src/HFSHighLevelVolume.h"

//#include "darling-dmg/src/main-fuse.h"
#include "darling-dmg/src/exceptions.h"
#include "darling-dmg/src/Reader.h"

#include <errno.h>
#include <string.h>
#ifdef __APPLE__
#  include <sys/xattr.h> // for ENOATTR
#elif defined(_MSC_VER)
#  define ENOATTR (93) // TODO better ?
#else
#  include <sys/xattr.h>
#  define ENOATTR ENODATA
#endif

#include <iostream>
#include <functional>

static int handle_exceptions(std::function<int()> func)
{
	try
	{
		return func();
	}
	catch (const file_not_found_error& e)
	{
//		std::cerr << "File not found: " << e.what() << std::endl;
		return -ENOENT;
	}
	catch (const function_not_implemented_error& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return -ENOSYS;
	}
	catch (const io_error& e)
	{
		std::cerr << "I/O error: " << e.what() << std::endl;
		return -EIO;
	}
	catch (const no_data_error& e)
	{
		std::cerr << "Non-existent data requested" << std::endl;
		return -ENODATA;
	}
	catch (const attribute_not_found_error& e)
	{
		//std::cerr << e.what() << std::endl;
		return -ENOATTR;
	}
	catch (const operation_not_permitted_error& e)
	{
		std::cerr << e.what() << std::endl;
		return -EPERM;
	}
	catch (const std::logic_error& e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		abort();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unknown error: " << e.what() << std::endl;
		return -EIO;
	}
}

int HfsFuseVolume::getattr(const char* path, struct FUSE_STAT* stat)
{
	return handle_exceptions([&]() -> int {
#ifndef DARLING
		*stat = hfsHighLevelVolume->stat(path);
#else
		struct FUSE_STAT st = g_volume->stat(path);
		bsd_stat_to_linux_stat(&st, reinterpret_cast<linux_stat*>(stat));
#endif
		return 0;
	});
}

int HfsFuseVolume::readlink(const char* path, char* buf, size_t size)
{
	if (size > INT32_MAX)
		return -EIO;

	return handle_exceptions([&]() -> int {

		std::shared_ptr<Reader> file;
		size_t rd;

		file = hfsHighLevelVolume->openFile(path);
		rd = file->read(buf, (int32_t)size-1, 0);
		
		buf[rd] = '\0';
		return 0;
	});
}

int HfsFuseVolume::open(const char* path, struct fuse_file_info* info)
{
	return handle_exceptions([&]() -> int {

		std::shared_ptr<Reader> file;
		std::shared_ptr<Reader>* fh;

		file = hfsHighLevelVolume->openFile(path);
		fh = new std::shared_ptr<Reader>(file);

		info->fh = uint64_t(fh);
		return 0;
	});
}

int HfsFuseVolume::read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info)
{
	(void)path;

	if (bytes > INT32_MAX)
		return -EIO;
	
	return handle_exceptions([&]() -> int {
		if (!info->fh)
			return -EIO;

		std::shared_ptr<Reader>& file = *(std::shared_ptr<Reader>*) info->fh;
		return file->read(buf, (int32_t)bytes, offset);
	});
}

int HfsFuseVolume::release(const char* path, struct fuse_file_info* info)
{
	(void)path;

	return handle_exceptions([&]() -> int {

		std::shared_ptr<Reader>* file = (std::shared_ptr<Reader>*) info->fh;
		delete file;
		info->fh = 0;
		
		return 0;
	});
}

//#ifdef DEBUG
//template class std::map<std::string, struct FUSE_STAT>;
//#endif

int HfsFuseVolume::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info)
{
	(void)offset;
	(void)info;
	return handle_exceptions([&]() -> int {
		std::map<std::string, struct FUSE_STAT> contents;

if ( strcmp(path, "/utf8_names") == 0 ) {
printf("");
}
		contents = hfsHighLevelVolume->listDirectory(path);

		for (auto it = contents.begin(); it != contents.end(); it++)
		{
			const char* cstr = it->first.c_str();
			if (filler(buf, cstr, &it->second, 0) != 0) // was NULL instead of it->second.
				return -ENOMEM;
		}

		return 0;
	});

}

#if defined(__APPLE__) && !defined(DARLING)
int HfsFuseVolume::getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position)
#else
int HfsFuseVolume::getxattr(const char* path, const char* name, char* value, size_t vlen)
#endif
{
	#ifdef __APPLE__
		if (position > 0) return -ENOSYS; // it's not supported... yet. I think it doesn't happen anymore since osx use less ressource fork
	#endif

	return handle_exceptions([&]() -> int {
		std::vector<uint8_t> data;

		data = hfsHighLevelVolume->getXattr(path, name);
		if (data.size() == 0)
			return 0;
		if (data.size() > INT32_MAX)
			return -EIO;

		if (value == nullptr)
			return (int32_t)data.size();
		
		if (vlen < data.size()) {
			// looks like high sierra return the maxmum bytes he can, but never ERANGE !
			memcpy(value, &data[0], vlen);
			return (int32_t)vlen;
			//return -ERANGE;
		}


		memcpy(value, &data[0], data.size());
		return (int32_t)data.size();
	});
}

int HfsFuseVolume::listxattr(const char* path, char* buffer, size_t size)
{
	return handle_exceptions([&]() -> int {
		std::vector<std::string> attrs;
		std::vector<char> output;

		attrs = hfsHighLevelVolume->listXattr(path);

		for (const std::string& str : attrs)
			output.insert(output.end(), str.c_str(), str.c_str() + str.length() + 1);
		if (output.size() > INT32_MAX)
			return -EIO;

		if (buffer == nullptr)
			return (int32_t)output.size();

		if (size < output.size())
			return -ERANGE;

		memcpy(buffer, &output[0], output.size());
		return (int32_t)output.size();
	});
}
