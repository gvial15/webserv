#include <string>
#include <vector>

class CGI {
	public:
		CGI(std::string path, std::vector<std::string> params);
		~CGI();

		// public methods

		// getters

		std::string	get_result() const { return result; }

	private:

		// private attribute

		std::string	result;
};