#include "../Class/CGI.hpp"
#include <string>
#include <unistd.h>
#include <iostream>

std::string CGI::vector_to_string(std::vector<std::string> vector) {
    std::string string;

    for (size_t i = 0; i < vector.size(); i++)
        string = string + vector[i];
    return (string);
}

CGI::CGI( Request & request, RequestConfig &config) {
    (void) config;
	this->_scriptPath = request.getRequestElem().find("path")->second;
    this->_postData = vector_to_string(request.getBody());
	this->_method = request.getRequestElem().find("method")->second;
    this->_contentType = request.getRequestElem().find("Content-Type")->second;
    this->_query = request.getQuery();
    this->_status = 0;

	this->_response = this->executeCgiScript();
}

void        CGI::childProcess(int *stdout_pipefd, int *stdin_pipefd){
    this->_scriptPath.erase(0, 1);
		std::cout << this->_scriptPath << std::endl;

        close(stdout_pipefd[0]);
        close(stdin_pipefd[1]);
        dup2(stdout_pipefd[1], STDOUT_FILENO);
        close(stdout_pipefd[1]);
        dup2(stdin_pipefd[0], STDIN_FILENO);
        close(stdin_pipefd[0]);

        // Prepare arguments array (argv)
        char* argv[] = {const_cast<char*>(_scriptPath.c_str()), NULL};
        
        // Set CGI environment variables
        std::vector<char*> envp;
        std::vector<std::string> envVars;
        if (!this->_method.compare("POST")){
            envVars.push_back("REQUEST_METHOD=" + this->_method);
            envVars.push_back("CONTENT_LENGTH=" + std::to_string(_postData.length()));
            envVars.push_back("CONTENT_TYPE=" + this->_contentType);
        }   
        else {
            envVars.push_back("QUERY_STRING=" + this->_query);
        }

        for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
            envp.push_back(const_cast<char*>(it->c_str()));
        }
        envp.push_back(NULL);
        // Execute the CGI script with execve
        execve(_scriptPath.c_str(), argv, envp.data());
        std::cerr << "Exec failed: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
}

std::string	CGI::executeCgiScript( void ){
	std::cout << "EXECUTING ==== " << _scriptPath << " " << _postData << std::endl;
    int stdout_pipefd[2];
    int stdin_pipefd[2];
    if (pipe(stdout_pipefd) == -1 || pipe(stdin_pipefd) == -1){
        std::cerr << "Pipe failed" << std::endl;
        this->_status = 500;
        return "";
    }

    pid_t pid = fork();
	int		child_status;

    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        this->_status = 500;
        return "";
    }
    if (pid == 0) {
        // child process
        this->childProcess(stdout_pipefd, stdin_pipefd);
        return ("");
    } else {
        // Parent process

        close(stdout_pipefd[1]);
        close(stdin_pipefd[0]);

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
            int exit_status = WEXITSTATUS(child_status);
        	if(exit_status == 400)
                this->_status = 400;
            else if (exit_status)
                this->_status = 400;
    	}
        return output;
    }
}

std::string const &CGI::getScriptPath( void ) const {
	return (this->_scriptPath);
}

std::string const &CGI::getPostData( void ) const {
	return (this->_postData);
}

std::string const &CGI::getResponse( void ) const {
	return (this->_response);
}

int CGI::getStatus( void ) const {
	return this->_status;
}

