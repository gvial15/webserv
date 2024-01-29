#include <string>
#include <vector>

class CGI {
	public:
		CGI(std::string path, std::vector<std::string> params, int output_fd);
		~CGI() {};

	private:

		// private class
		class ExecveException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: execve() failed";
			}
		};

		class ForkException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: fork() failed";
			}
		};
};