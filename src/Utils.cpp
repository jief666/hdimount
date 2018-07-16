//
//  Utils.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 16/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#include "Utils.hpp"

#include <string>
#include <iostream>

#if defined(__linux__) || defined(__APPLE__)
#include <termios.h>
#endif

std::string stprintf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buf[3000];
	vsnprintf(buf, sizeof(buf)-1, format, ap);
	va_end(ap);
	return buf;
}

bool GetPassword(std::string* pwPtr)
{
#if defined(__linux__) || defined(__APPLE__)
	struct termios told, tnew;
	FILE *stream = stdin;

	/* Turn echoing off and fail if we can’t. */
	if (tcgetattr (fileno (stream), &told) != 0)
		return false;
	tnew = told;
	tnew.c_lflag &= ~ECHO;
	if (tcsetattr (fileno (stream), TCSAFLUSH, &tnew) != 0)
		return false;

	/* Read the password. */
	std::getline(std::cin, *pwPtr);

	/* Restore terminal. */
	(void) tcsetattr (fileno (stream), TCSAFLUSH, &told);

	std::cout << std::endl;

	return true;
#else
	std::getline(std::cin, pw);
	std::cout << std::endl;

	return true;
#endif
}
