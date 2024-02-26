#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

int running;
int sigwinch_received;

const char *prompt = "rltest$ ";

static void
sighandler(int sig)
{
	sigwinch_received = 1;
}

static void
cb_linehandler(char *line)
{
	if (line == NULL || strcmp(line, "exit") == 0) {
		if (line == 0)
			printf("\n");
		printf("exit\n");

		rl_callback_handler_remove();

		running = 0;
	} else {
		if (*line) {
			add_history(line);
		}
		printf("input line: %s\n", line);
		free(line);
	}
}

int main(int argc, char *argv[])
{
	fd_set fds;
	int r;

	rl_callback_handler_install(prompt, cb_linehandler);

	running = 1;

	while (running) {
		FD_ZERO(&fds);
		FD_SET(fileno(rl_instream), &fds);

		r = select(FD_SETSIZE, &fds, NULL, NULL, NULL);
		if (r < 0 && errno != EINTR) {
			perror("rltest: select");
			rl_callback_handler_remove();
			break;
		}
		if (sigwinch_received) {
			rl_resize_terminal();
			sigwinch_received = 0;
		}
		if (r < 0)
			continue;

		if (FD_ISSET(fileno(rl_instream), &fds))
			rl_callback_read_char();
	}
	printf("rltest: Event loop has exited\n");

	return EXIT_SUCCESS;
}
