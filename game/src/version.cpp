#include <stdio.h>

void WriteVersion()
{
#ifndef __WIN32__
	FILE* fp = fopen("VERSION.txt", "w");

	if (fp)
	{
		fprintf(fp, "__GAME_VERSION__: %s\n", __GAME_VERSION__);
		fprintf(fp, "%s@%s:%s\n", "emF3YXRv", __HOSTNAME__, __PWD__);
		fclose(fp);
	}
#endif
}
//martysama0134's ceqyqttoaf71vasf9t71218
