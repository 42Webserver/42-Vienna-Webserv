#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int	main(int argc, char** argv)
{
	if (argc < 2)
		return (42);

	char *python_path = "/usr/bin/python3";

    char *script_path = "./cgi-bin/main.py";

    char *args[] = {python_path, script_path, "NULL", "string", NULL};

    char *env[] = {NULL};

    if (execve(python_path, args, env) == -1) {
        perror("execve failed");
        return (1);
    }
}
