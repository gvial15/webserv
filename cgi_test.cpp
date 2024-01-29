#include <unistd.h>
#include <iostream>

int	main(int argc, char **argv) {
	sleep(10);
	std::cout << "CGI TEST\n";
	int i = 0;
	while (argv[++i])
		std::cout << argv[i] << "\n";
	return 1;
}
