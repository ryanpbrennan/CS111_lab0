#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void sigseg_handler(int sig)
{
	printf("segfault caught. exiting..\n");
	exit(4);
}

void seg_fault(int cf)
{
	char* s = NULL;

	if (cf)
	{
		signal(SIGSEGV, sigseg_handler);
	}

	s[0] = 0;
}

int main(int argc, char **argv)
{
	int c;
	int opt_index = 0;
	int catch_flag = 0;
	int seg_flag = 0;

	char* infile = NULL;
	char* outfile = NULL;

	static struct option long_opts[] =
	{
		{"input", 	    required_argument, 	    NULL, 		'i'},
		{"output", 	    required_argument, 	    NULL, 		'o'},
		{"segfault", 	no_argument, 	   		NULL, 		's'},
		{"catch", 	    no_argument, 	    	NULL, 		'c'},
		{0, 0, 0, 0}
	};
	
	while ((c = getopt_long(argc, argv, "sci:o:", long_opts, &opt_index)) != -1)
	{
		switch(c)
		{
			case 'i':
				infile = optarg;
				break;
			case 'o':
				outfile = optarg;
				break;
			case 's':
				seg_flag = 1;
				break;
			case 'c':
				catch_flag = 1;
				break;
			default:
				printf("unrecognized option. usage:\nlab0 [--input=FILE] [--output=FILE] [[--segfault] --catch]\n");
				exit(1);
		}	
	}


	if (seg_flag == 1)
	{

        seg_fault(catch_flag);

		/*char* s = NULL;

		if (catch_flag)
		{
			signal(SIGSEGV, sigseg_handler);
		}

		s[0] = 0;*/
	}

	if (infile != NULL)
	{
		int ifd = open(infile, O_RDONLY);

		if (ifd >= 0)
		{
			close(0);
			dup(ifd);
			close(ifd);
		}
		else
		{
			fprintf(stderr, "error opening %s: %s\n", infile, strerror(errno));
			exit(2);
		}
	}

	if (outfile != NULL)
	{
		int ofd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);

		if (ofd >= 0)
		{
			close(1);
			dup(ofd);
			close(ofd);
		}
		else
		{	
			fprintf(stderr, "error opening %s: %s\n", outfile, strerror(errno));
			exit(3);
		}
	}

	char* buf = malloc(sizeof(char));
	int b;

	while ((c = read(0, buf, 1)) > 0)
	{
		if ((b = write(1, buf, 1)) < 0)
		{
			fprintf(stderr, "error writing: %s.\n", strerror(errno));
			exit(5);
		}
	}

	if (c < 0)
	{
		fprintf(stderr, "error reading: %s\n", strerror(errno));
		exit(6);
	}

	free(buf);
	close(0);
	close(1);

	return 0;
}
