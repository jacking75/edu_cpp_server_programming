#include "lib_acl.h"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <stdio.h>
#include <string>
#include <iostream>

//////////////////////////////////////////////////////////////////////////

static void test_main(bool move) {
	acl::string s("hello world!\r\n"
		"you're welcome\r\n"
		"what's your name\r\n"
		"happy new year");
	acl::string line;

	while (true) {
		if (s.scan_line(line, true, NULL) == true) {
			printf(">>line: %s, rest len: %d\r\n",
				line.c_str(), (int) s.length());
			line.clear();

			if (move) {
				s.scan_move();
			}
		} else {
			if (s.empty()) {
				break;
			}

			printf(">>last: %s, len: %d\r\n",
				s.c_str(), (int) s.length());

			acl_assert(strlen(s.c_str()) == s.length());

			if (move) {
				s.scan_move();
			}

			printf("=======================================\r\n");
			printf(">>string len: %d, buf len: %d, buf: \r\n%s\r\n",
				(int) s.length(), (int) strlen((char*) s.buf()),
				(char*) s.buf());
			printf("=======================================\r\n");

			break;
		}
	}
}

static void test(void) {
	acl::string path("/data1/www/video/test/test.ts");
	const char* disk = "/data1/www";
	if (path.begin_with(disk)) {
		printf("ok\r\n");
	} else {
		printf("error\r\n");
	}

	disk = "/data2/www";
	if (path.begin_with(disk)) {
		printf("error\r\n");
	} else {
		printf("ok\r\n");
	}
}

static void usage(const char* procname) {
	printf("usage: %s -h [help] -m [move buf after scan]\r\n", procname);
}

int main(int argc, char *argv[]) {
	int   ch;
	bool  move = false;

	while ((ch = getopt(argc, argv, "hm")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'm':
			move = true;
			break;
		default:
			break;
		}
	}

	acl::string s1 = "hello world!";
	std::string s2 = s1;
	s2 += ", ";
	s2 += s1;

	printf("s1=%s, s2=%s\r\n", s1.c_str(), s2.c_str());
	std::cout << "s1=" << s1 << ", s2=" << s2 << std::endl;

	for (int i = 0 ; i < 2; i++) {
		if (s1 == s2) {
			printf("strings equal!\r\n");
		} else {
			printf("strings not equal!\r\n");
		}

		s2.clear();
		s2 += s1;
	}

	test();
	test_main(move);

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return (0);
}
