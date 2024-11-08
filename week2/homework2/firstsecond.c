#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn(const char * program, char ** arg_list) {
	pid_t child_pid = fork();
	if (child_pid != 0)
		return child_pid;
	else {
		execvp (program, arg_list);
		perror("exec failed");
		return 1;
	}
}

int main() {
	char *arg_list1[] = { "konsole", "-e", "./second", "&" };
	spawn("konsole", arg_list1);

	char *arg_list2[] = { "konsole", "-e", "./first", NULL };
	spawn("konsole", arg_list2);
	printf ("Main program exiting...\n");
	return 0;
}