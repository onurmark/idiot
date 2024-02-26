#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

extern char *xmalloc PARAMS((size_t));

int com_list PARAMS((char *));
int com_view PARAMS((char *));
int com_rename PARAMS((char *));
int com_stat PARAMS((char *));
int com_pwd PARAMS((char *));
int com_delete PARAMS((char *));
int com_help PARAMS((char *));
int com_cd PARAMS((char *));
int com_quit PARAMS((char *));

typedef struct {
	char *name;
	rl_icpfunc_t *func;
	char *doc;
} COMMAND;

COMMAND commands[] = {
  { "cd", com_cd, "Change to directory DIR" },
  { "delete", com_delete, "Delete FILE" },
  { "help", com_help, "Display this text" },
  { "?", com_help, "Synonym for `help'" },
  { "list", com_list, "List files in DIR" },
  { "ls", com_list, "Synonym for `list'" },
  { "pwd", com_pwd, "Print the current working directory" },
  { "quit", com_quit, "Quit using Fileman" },
  { "rename", com_rename, "Rename FILE to NEWNAME" },
  { "stat", com_stat, "Print out statistics on FILE" },
  { "view", com_view, "View the contents of FILE" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};

/* forward declarations */
char *stripwhite(char *);
COMMAND *find_command(char *);

char *progname;

int done;

char *
dupstr(char *s)
{
	char *r;
	
	r = xmalloc(strlen(s) + 1);
	strcpy(r, s);
	return (r);
}

static char *
command_generator(const char *text, int state)
{
	static int list_index, len;
	char *name;

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = commands[list_index].name) != NULL) {
		list_index++;

		if (strncmp(name, text, len) == 0) {
			return dupstr(name);
		}
	}

	return (char *)NULL;
}

static char **
fileman_completion(const char *text, int start, int end)
{
	char **matches;

	// printf("text: %s, start: %d, end:%d\n", text, start, end);

	matches = (char **)NULL;

	if (start == 0) {
		matches = rl_completion_matches(text, command_generator);
	}

	return matches;
}

static void
fileman_display_matches_hook(char **matches, int num_matches, int max_length)
{
	int i, j;

	printf("\n");

	for (i = 0; i < num_matches; i++) {
		// printf("Matches[%d]: %s\n", i, matches[i]);

		for (j = 0; commands[j].name; j++) {
			if (!matches[i] || (strcmp(matches[i], commands[j].name) == 0)) {
				printf("%s\t\t%s.\n", commands[j].name, commands[j].doc);
			}
		}
	}
}

static void
initialize_readline(void)
{
	rl_readline_name = "FileMan";

	rl_attempted_completion_function = fileman_completion;
	rl_completion_display_matches_hook = fileman_display_matches_hook;
}

void too_dangerous(char *caller)
{
	fprintf(stderr,
			"%s: Too dangerous for me distribute. Write it yourself.\n",
			caller);
}

int
valid_argument(char *caller, char *arg)
{
	if (!arg || !*arg) {
		fprintf(stderr, "%s: Argument required.\n", caller);
		return 0;
	}

	return 1;
}

static char syscom[1024];

int com_list(char *arg)
{
	if (!arg) {
		arg = "";
	}

	sprintf(syscom, "ls -FClg %s", arg);
	return system(syscom);
}

int com_view(char *arg)
{
	if (!valid_argument("view", arg)) {
		return 1;
	}

	sprintf(syscom, "less %s", arg);

	return (system(syscom));
}

int com_rename(char *arg)
{
	too_dangerous("rename");
	return 1;
}

int com_stat(char *arg)
{
	struct stat finfo;

	if (!valid_argument("stat", arg)) {
		return 1;
	}

	if (stat(arg, &finfo) == -1) {
		perror(arg);
		return 1;
	}

	printf("Statistics for '%s':\n", arg);

	printf("%s has %lu link%s, and is %ld byte%s in length.\n",
			arg,
			finfo.st_nlink,
			(finfo.st_nlink == 1) ? "" : "s",
			finfo.st_size,
			(finfo.st_size == 1) ? "" : "s");
	printf("Inode Last Change at: %s", ctime(&finfo.st_ctime));
	printf("      Last Change at: %s", ctime(&finfo.st_atime));
	printf("    Last modified at: %s", ctime(&finfo.st_mtime));

	return 0;
}

int com_delete(char *arg)
{
	too_dangerous("delete");
	return 1;
}

int com_help(char *arg)
{
	register int i;
	int printed = 0;

	for (i = 0; commands[i].name; i++) {
		if (!*arg || strcmp(arg, commands[i].name) == 0) {
			printf("%s\t\t%s.\n", commands[i].name, commands[i].doc);
			printed++;
		}
	}

	if (!printed) {
		if (arg != NULL) {
			printf("No commands match '%s'. Possibilities are:\n", arg);
		}

		for (i = 0; commands[i].name; i++) {
			if (printed == 6) {
				printed = 0;
				printf("\n");
			}

			printf("%s\t", commands[i].name);
			printed++;
		}

		if (printed) {
			printf("\n");
		}
	}

	return 0;
}

int com_pwd(char *ignore)
{
	char dir[1024], *s;

	s = getcwd(dir, sizeof(dir) - 1);
	if (s == 0) {
		printf("Error getting pwd: %s\n", dir);
		return 1;
	}

	printf("Current directory is %s\n", dir);
	return 0;
}

int com_quit(char *arg)
{
	done = 1;
	return 0;
}

int com_cd(char *arg)
{
	if (chdir(arg) == -1) {
		perror(arg);
		return 1;
	}

	com_pwd("");
	return 0;
}

COMMAND *
find_command(char *name)
{
	register int i;

	for (i = 0; commands[i].name; i++) {
		if (strcmp(name, commands[i].name) == 0) {
			return &commands[i];
		}
	}

	return (COMMAND *)NULL;
}

static int
execute_line(char *line)
{
	register int i;
	COMMAND *command;
	char *word;

	i = 0;
	while (line[i] && whitespace(line[i])) {
		i++;
	}
	word = line + i;

	while (line[1] && !whitespace(line[i])) {
		i++;
	}

	if (line[i]) {
		line[i++] = '\0';
	}

	command = find_command(word);

	if (!command) {
		fprintf(stderr, "%s: No such command for FileMan.\n", word);
		return -1;
	}

	while (whitespace(line[i])) {
		i++;
	}

	word = line + i;

	return (*(command->func))(word);
}

char *
stripwhite(char *string)
{
	register char *s, *t;

	for (s = string; whitespace(*s); s++)
		;

	if (*s == 0) {
		return s;
	}

	t = s + strlen(s) - 1;
	while (t > s && whitespace(*t))
		t--;

	*++t = '\0';

	return s;
}

int main(int argc, char *argv[])
{
	char *line, *s;

	progname = argv[0];

	initialize_readline();

	for (; done == 0; ) {
		line = readline("FileMan: ");
		if (!line)
			break;

		s = stripwhite(line);

		if (*s) {
			add_history(s);
			printf("'%s'", s);
			execute_line(s);
		}

		free(line);
	}

	return EXIT_SUCCESS;
}
