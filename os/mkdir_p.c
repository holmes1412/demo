#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/param.h>
#else
#define MAXPATHLEN 4096
#include <direct.h>
#endif

int mkdir_p(const char *name, mode_t mode)
{
	int ret = mkdir(name, mode);
	if (ret == 0 || errno != ENOENT)
		return ret;

	size_t len = strlen(name);
	if (len > MAXPATHLEN)
	{
		errno = ENAMETOOLONG;
		return -1;
	}

	char path[MAXPATHLEN + 1] = {};
	memcpy(path, name, len);
	
	if (name[len - 1] != '/')
	{
		path[len] = '/';
		len++;
	}

	int i;
	int has_valid = 0;

	for (i = 0; i < len; i++)
	{
		if (path[i] != '/' && path[i] != '.')
		{
			has_valid = 1;
			continue;
		}

		if (path[i] == '/' && has_valid == 1)
		{
			path[i] = '\0';
			printf("mkdir %s\n", path);
			if (mkdir(path, mode) != 0 && errno != EEXIST)
				return -1;

			path[i] = '/';
			has_valid = 0;
		}
	}

	return 0;
}

int main(int argc, const char *argv[])
{
	if (argc != 2)
		return -1;

	if (mkdir_p(argv[1], 0755) != 0)
		printf("errno=%d\n", errno);

	return 0;
}

