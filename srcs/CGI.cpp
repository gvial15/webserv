#include "../Class/CGI.hpp"
#include <unistd.h>
#include <iostream>

CGI::CGI(std::string path, std::vector<std::string> params, int output_fd) {
    const char	*argv[params.size() + 2];
	pid_t		pid;
	int			i;

	pid = fork();

    if (pid == -1)
		throw ForkException();
	// Child process
    if (pid == 0) {
        dup2(output_fd, STDOUT_FILENO);
        // Prepare argv[] for execve
		argv[0] = path.c_str();
		i = 0;
		while (++i < params.size())
			argv[i] = params[i].c_str();
		argv[params.size() + 1] = nullptr;
        // Execute the program
        execve(argv[0], const_cast<char * const *>(argv), nullptr);
		// execve exception
		throw ExecveException();
	}
};
