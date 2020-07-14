#include <termios.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>

struct termios term_init(int code);
void term_restore();
//void sig_int_handler();
void sig_pip_handler();

int main(int argc, char* argv[])
{
	int shell_flag = 0;
	int opt_index = 0;

	char c;
	char* shell_name = NULL;

	static struct termios restore_state;
	struct pollfd pollfds[2];

	int i;
	const int buf_size = 256;
	int data;
	char buf[buf_size];

	int to_shell[2];    // pipes
	int from_shell[2];

	pid_t ret; // pid of child process

	//signal(SIGINT, sig_int_handler);
	signal(SIGPIPE, sig_pip_handler);

	static struct option long_opts[] =
	{
		{"shell", 		required_argument, 		NULL, 		's'},
		{0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, "[--shell=PROG]", long_opts, &opt_index)) != -1)
	{
		switch(c)
		{
			case 's':
				shell_flag = 1;
				shell_name = optarg;
				break;
			default:
				printf("unrecognized option. usage:\nlab1a [--shell=PROG]");
				exit(1);
		}
	}


	term_init(0);
	
	atexit(term_restore);


	if (pipe(to_shell) < 0)
	{
		fprintf(stderr, "error creating pipe to shell: %s\n", strerror(errno));
		exit(1);
	}
		
	if (pipe(from_shell) < 0)
	{
		fprintf(stderr, "error creating pipe from shell: %s\n", strerror(errno));
		exit(1);
	}

	if (shell_flag)
	{
		ret = fork();
		if (ret < 0)
		{
			fprintf(stderr, "fork failed: %s\n", strerror(errno));
			exit(1);
		}
		else if (ret == 0) 		// child process
		{
			close(0);
			dup(to_shell[0]);
			close(to_shell[0]);

			close(1);
			dup(from_shell[1]);

			close(2);
			dup(from_shell[1]);

			close(from_shell[1]);
			close(to_shell[1]);
			close(from_shell[0]);

			if (execlp(shell_name, shell_name, NULL) < 0)
			{
				fprintf(stderr, "error executing shell %s:%s\n", shell_name, strerror(errno));
				exit(1);
			}
		}
		else 	// parent process
		{
			pollfds[0].fd = 0;
			pollfds[0].events = POLLIN | POLLHUP | POLLERR;
			pollfds[1].fd = from_shell[0];
			pollfds[1].events = POLLIN | POLLHUP | POLLERR;
			
			close(to_shell[0]);
			close(from_shell[1]);

			while (1)
			{
				if (poll(pollfds, 2, -1) < 0)
				{
					fprintf(stderr, "poll failed: %s\n", strerror(errno));
					exit(1);
				}

				if (pollfds[0].revents & POLLIN)
				{
				// ready to read from stdin

					if ((data = read(0, buf, buf_size)) < 0)
					{
						fprintf(stderr, "read from stdin failed\n");
						exit(1);
					}

					// read from keyboard
					for (i = 0; i < data; i++)
					{
						if (buf[i] == 0x4)
						{
							close(to_shell[1]);
							break;
						}
						else if (buf[i] == 0x3)
						{
							kill(ret, SIGINT);
							break;
						}
						else if (buf[i] == '\r' || buf[i] == '\n')
						{
							write(1, "\r\n", 2);
							write(to_shell[1], "\n", 1);
						}
						else
						{
							write(1, &buf[i], 1);
							write(to_shell[1], &buf[i], 1);
						}
					} // end for
				} // end if

				if (pollfds[1].revents & POLLIN)
				{	
				// ready to read from shell

					if ((data = read(from_shell[0], buf, buf_size)) < 0)
					{
						fprintf(stderr, "read from shell failed\n");
						exit(1);
					}

					for (i = 0; i < data; i++)
					{
						if (buf[i] == '\n')
						{
							write(1, "\r\n", 2);
						}
						else
						{
							write(1, &buf[i], 1);
						}
					} // end for
				} // end if

				if (pollfds[1].revents & (POLLHUP | POLLERR))
				{
					return 0;
				}
			} // end while
		} // end else (parent process)
	} // end if (shell_flag)

	
	/*while ((data = read(0, buf, buf_size)) >= 0)
	{
		for (i = 0; i < data; i++)
		{
			if (buf[i] == 0x4)
			{
				write(1, "^D", 2);
				exit(0);
			}
			else if (buf[i] == '\r' || buf[i] == '\n')
			{
				write(1, "\r\n", 1);	
			}
			else
			{	
				write(1, &buf[i], 1);
			}
		}
	}*/


	return 0;
}

struct termios term_init(int code) // 0: entry, 1: exit
{
	static struct termios term_initial_state;
	static struct termios term_restore_state;

	if (code == 0)
	{
		int tm = tcgetattr(0,  &term_initial_state);
    
		if (tm < 0)
		{
			fprintf(stderr, "terminal state retrieval error: %s\n", strerror(errno));
			exit(1);
		}
		else
		{
			term_restore_state = term_initial_state;
			term_initial_state.c_iflag = ISTRIP;
			term_initial_state.c_oflag = 0;
			term_initial_state.c_lflag = 0;
			tcsetattr(0, TCSANOW, &term_initial_state);
		}
		return term_initial_state;
	}
	else if (code == 1)
	{
		return term_restore_state;
	}
   	else
	{
		fprintf(stderr, "error: unexpected entry code to term_init.\n");
		exit(1);
	}
}

void term_restore()
{
	struct termios restore_state = term_init(1);
	int rs = tcsetattr(0, TCSANOW, &restore_state);

	if (rs < 0)
	{
		fprintf(stderr, "error restoring terminal state: %s\n", strerror(errno));
		exit(1);
	}

	return;
}

/*void sig_int_handler()
{
	printf("encountered interupt\n");
	exit(1);
}*/

void sig_pip_handler()
{
	printf("broken pipe\n");
	exit(1);
}
