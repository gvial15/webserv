#include "../Class/CGI.hpp"
#include <unistd.h>
#include <iostream>
#include <fcntl.h>


std::string CGI::vector_to_string(std::vector<std::string> vector) {
    std::string string;

    for (size_t i = 0; i < vector.size(); i++)
        string = string + vector[i];
    return (string);
}

CGI::CGI( Request & request, RequestConfig &config ){
	this->_scriptPath = config.getPath();
    this->_postData = vector_to_string(request.getBody());
	this->_method = request.getRequestElem().find("method")->second;
    this->_contentType = request.getRequestElem().find("Content-Type")->second;
    this->_query = request.getQuery();
    this->_pathInfo = request.getPathInfo();
    this->_status = 0;
    this->_timeout = 1000000;

	this->_response = this->executeCgiScript();
}

void        CGI::splitScriptPath( void ){
    size_t lastSlashPos = this->_scriptPath.find_last_of('/');

    if (lastSlashPos != std::string::npos) {
        this->_execLocation = this->_scriptPath.substr(0, lastSlashPos + 1); // Includes the trailing slash
        this->_script = this->_scriptPath.substr(lastSlashPos + 1);
    } else {
        // Script is in the root directory
        this->_execLocation = "./"; // Current directory
        this->_script = this->_scriptPath;
    }
}

void        CGI::childProcess(int *stdout_pipefd, int *stdin_pipefd){
    this->splitScriptPath();
    if (this->_execLocation[0] == '/')
        this->_execLocation.erase(0,1);
    chdir(this->_execLocation.c_str());

    close(stdout_pipefd[0]);
    close(stdin_pipefd[1]);
    dup2(stdout_pipefd[1], STDOUT_FILENO);
    close(stdout_pipefd[1]);
    dup2(stdin_pipefd[0], STDIN_FILENO);
    close(stdin_pipefd[0]);

    // Prepare arguments array (argv)
    char* argv[] = {const_cast<char*>(_script.c_str()), NULL};

    // Set CGI environment variables
    std::vector<char*> envp;
    std::vector<std::string> envVars;
    if (!this->_method.compare("POST")){
        envVars.push_back("REQUEST_METHOD=" + this->_method);
        envVars.push_back("CONTENT_LENGTH=" + std::to_string(_postData.length()));
        envVars.push_back("CONTENT_TYPE=" + this->_contentType);
    }   
    else {
        if (!this->_pathInfo.empty())
            envVars.push_back("PATH_INFO=" + this->_pathInfo);
        else
            envVars.push_back("QUERY_STRING=" + this->_query);
    }

    for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
        envp.push_back(const_cast<char*>(it->c_str()));
    }
    envp.push_back(NULL);

    //redirect script errors to dev/null
    int dev_null_fd = open("/dev/null", O_WRONLY);
    dup2(dev_null_fd, STDERR_FILENO);
    close(dev_null_fd);

    // Execute the CGI script with execve
    std::string scriptExtension = _script.substr(_script.find_last_of('.') + 1);
    if (scriptExtension == "php") {
        const char* php_argv[] = {"/usr/bin/php", _script.c_str(), NULL};
        execve("/usr/bin/php", const_cast<char**>(php_argv), envp.data());
    } else {
        execve(_script.c_str(), argv, envp.data());
    }
    std::cerr << "Exec failed: " << std::strerror(errno) << std::endl;
}

std::string CGI::executeCgiScript( void ){
    int stdout_pipefd[2];
    int stdin_pipefd[2];
    if (pipe(stdout_pipefd) == -1 || pipe(stdin_pipefd) == -1){
        std::cerr << "Pipe failed" << std::endl;
        this->_status = 500;
        return "";
    }

    pid_t pid = fork();
    int child_status;

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

        // Wait for the child process to finish
        pid_t res = 0;
        for (int i = 0; i < _timeout && res == 0; i++)
            res = waitpid(pid, &child_status, WNOHANG);
        if (res == 0){
            std::cerr << "CGI: timeout" << std::endl;
            this->_status = 500;
        } else if (WIFEXITED(child_status)) {
            int exit_status = WEXITSTATUS(child_status);
            if(exit_status == 144){
                std::cerr << "CGI: error with Query or PATH_INFO" << std::endl;
                this->_status = 400;
            }
            else if (exit_status){
                std::cerr << "CGI: error running script" << std::endl;
                this->_status = 500;
            }
        }

        // Read the script's output only if status is not 500
        std::string output;
        if (this->_status == 0){
            char buffer[4096];
            ssize_t count;
            while ((count = read(stdout_pipefd[0], buffer, sizeof(buffer))) > 0) {
                output.append(buffer, count);
            }
        }
        close(stdout_pipefd[0]);
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

int			CGI::getStatus( void ) const {
	return this->_status;
}