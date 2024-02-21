#include "../Class/CGI.hpp"
#include <unistd.h>
#include <iostream>

CGI::CGI( Request & request, RequestConfig &config ){
	this->_scriptPath = request.getRequestElem().find("path")->second;
    this->_postData = request.getBody();
	this->_method = request.getRequestElem().find("method")->second;
    this->_contentType = request.getRequestElem().find("Content-Type")->second;
    // std::cout << this->_contentType
	this->_response = this->executeCgiScript();
}

void		CGI::setEnvp( std::vector<char *> &envp ) {
	std::vector<std::string> envVars;
	envVars.push_back("REQUEST_METHOD=" + this->_method);
	if (!this->_method.compare("POST")){
		envVars.push_back("CONTENT_LENGTH=" + std::to_string(_postData.length()));
		envVars.push_back("CONTENT_TYPE=" + this->_contentType);
	}   
	else{
		envVars.push_back("QUERY_STRING=" + this->_query);
	}

	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
		envp.push_back(const_cast<char*>(it->c_str()));
	}
	envp.push_back(NULL);
	for (int i = 0; envp[i]; i++){
		std::cerr << "ENVP" << envp[i] << std::endl;
	}
}

std::string	CGI::executeCgiScript( void ){
	std::cout << "EXECUTING ==== " << _scriptPath << " " << _postData << std::endl;
    int stdout_pipefd[2];
    int stdin_pipefd[2];
    pipe(stdout_pipefd); // Pipe for script's output
    pipe(stdin_pipefd);  // Pipe for script's input

    pid_t pid = fork();
	int		child_status;

    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        return "";
    }
    if (pid == 0) {
		this->_scriptPath.erase(0, 1);
		std::cout << this->_scriptPath << std::endl;

        close(stdout_pipefd[0]); // Close the read end of the output pipe
        close(stdin_pipefd[1]);  // Close the write end of the input pipe

        // Redirect stdout to the write end of the output pipe
        dup2(stdout_pipefd[1], STDOUT_FILENO);
        close(stdout_pipefd[1]);

        // Redirect stdin to the read end of the input pipe
        dup2(stdin_pipefd[0], STDIN_FILENO);
        close(stdin_pipefd[0]);

        // Prepare arguments array (argv)
        char* argv[] = {const_cast<char*>(_scriptPath.c_str()), NULL};
        
        // Set CGI environment variables
        std::vector<char*> envp;
		setEnvp( envp );

        // Execute the CGI script with execve
        execve(_scriptPath.c_str(), argv, envp.data());

        // If execve returns, it must have failed
        std::cerr << "Exec failed: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        close(stdout_pipefd[1]); // Close the write end of the output pipe
        close(stdin_pipefd[0]);  // Close the read end of the input pipe

        // Write POST data to the CGI script
        write(stdin_pipefd[1], _postData.c_str(), _postData.size());
        close(stdin_pipefd[1]);  // Close the pipe after writing

        // Read the script's output
        std::string output;
        char buffer[1024];
        ssize_t count;
        while ((count = read(stdout_pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, count);
        }
        close(stdout_pipefd[0]);

        // Wait for the child process to finish
        waitpid(pid, &child_status, 0);
		if (WIFEXITED(child_status)) {
        	this->_status = WEXITSTATUS(child_status);
    	}
        return output;
    }
}

std::string const &CGI::getScriptPath( void ) const{
	return (this->_scriptPath);
}

std::string const &CGI::getPostData( void ) const{
	return (this->_postData);
}

std::string const &CGI::getResponse( void ) const{
	return (this->_response);
}

int			const CGI::getStatus( void ) const {
	return this->_status;
}
