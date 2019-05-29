
#include <stdio.h>
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>


#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <string>
#include <algorithm>

#include "../src/Utils.h"
#include "../src_tests_diskimages/io.h"

using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
	return rtrim(ltrim(s));
}

int main(int argc, char **argv)
{
	if ( argc != 2 ) {
		printf("Usage: egenerate_version <top project dir>\n");
		return 1;
	}
	string dir = argv[1];
	try
	{
		string output = execAndGetOutput_T("git rev-parse --short  HEAD");
		output = trim(output);
		printf("Commit short sha is '%s'\n", output.c_str());

		if ( !file_or_dir_exists(dir+"/version.txt") ) {
			printf("File '%s/version.txt' is not found. First arg is project top dir.\n", dir.c_str());
			return 1;
		}
		string versionTxt = read_all_at_path_T(dir+"/version.txt", "Cannot find version.txt");
		versionTxt = trim(versionTxt);
		printf("File '%s/version.txt' contains '%s'\n", dir.c_str(), versionTxt.c_str());
		
		string version = stringPrintf("%s__%s", versionTxt.c_str(), output.c_str());

		string newheader = stringPrintf("/* Generated file, do not edit */\n#define HDIMOUNT_VERSION \"%s\"\n", version.c_str());

		if ( !file_or_dir_exists(dir+"/src/Version.h") ) {
			printf("File '%s/src/Version.h' doesn't exit. First arg is project top dir.\n", dir.c_str());
			return 1;
		}
		string header = read_all_at_path_T(dir+"/src/Version.h", "Cannot find 'src/Version.h'");

		if ( header != newheader ) {
			char tmp[101];
			int nb = sscanf(header.c_str(), "/* Generated file, do not edit */\n#define HDIMOUNT_VERSION \"%50s\"\n", tmp);
			if ( nb != 1 )
			{
				printf("New version is '%s'. Generate '%s/src/Version.h'\n", version.c_str(), dir.c_str());
				write_at_path_T(dir+"/src/Version.h", newheader, "cannot write");
			}
			else
			{
				if ( strlen(tmp) > 0  &&  tmp[strlen(tmp)-1] == '\"' )
					tmp[strlen(tmp)-1] = 0;
				
				printf("Old version was '%s', new version is '%s'. Generate '%s/src/Version.h'\n", tmp, version.c_str(), dir.c_str());
				write_at_path_T(dir+"/src/Version.h", newheader, "cannot write");
			}
		}else{
			printf("Version didn't change. It's '%s'. '%s/src/Version.h' wasn't touched\n", version.c_str(), dir.c_str());
		}
	}
    catch(const string& s)
    {
        fprintf(stderr, "%s\n", s.c_str());
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Exeception type '%s', what '%s'\n", typeid(e).name(), e.what());
    }
    catch(...)
    {
        fprintf(stderr, "Unknow exception\n");
    }
}

