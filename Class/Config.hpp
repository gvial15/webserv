/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diegofranciscolunalopez <diegofrancisco    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 12:18:11 by dluna-lo          #+#    #+#             */
/*   Updated: 2023/11/02 08:09:34 by diegofranci      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_CPP
#define CONFIG_CPP

#include "./Server.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>

class Config
{

private:
    bool error;
    std::vector<Server> v_servers;
    std::vector<Server>::iterator itr_servers;

    // -> check line
    bool f_check_clean_line(std::string path, std::string find);
    bool f_is_file(std::string path);
    void f_check_final_line(std::string line);
    void f_check_close(std::string line, bool *find_server, bool *find_locacation, bool *find_new_line);
    void f_check_proms(std::string line);
    void f_check_repeat();
    void f_check_data_with_path();

    // -> functions save
    void    f_organise_listen();
    void    f_save_default();
    void    f_clean_listen();
    void    f_save_default_location();
    void    f_save_multiple_values_in_location(std::string path, size_t start, Location *loc, std::string metod);
    void    f_save_multiple_values_in_server(std::string path, size_t start, Server *ser, std::string metod);
    void    f_save_multiple_values_in_server_2(std::string path, size_t start, Server *ser);

    // > dedault path
    bool    f_check_path(std::string path);
    void    f_default_config(std::string dataUrl);



public:
    Config();
    ~Config();

    std::vector<Server> getServers() const { return v_servers; }

    bool run(std::string data);

    bool check_error();

    void saveData(std::string dataUrl); // save in clases
    bool checkData(std::string dataUrl);
    void init_sockets();

    void seeData(void);

    void restart_itr_servers();
    size_t get_server_size();
    Server get_server();
    Server get_server(int index);

	Server &get_ref_server(size_t index);


    // Error -> try cath
	class formatWrong : public std::exception {
    public:
        virtual const char* what() const throw();
    };
	class fileNotFoundWrong : public std::exception {
    public:
        virtual const char* what() const throw();
    };

    class emptyFile : public std::exception {
    public:
        virtual const char* what() const throw();
    };

};

/********COLORS*********/
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"


#endif

// -> functions check
size_t f_count_words_string(std::string str);
int f_counter_worlds(std::string str, std::string find);
int f_counter_clean_worlds(std::string str, std::string find);
bool f_string_has_words(std::string str);
bool f_run_each_words(std::string str, int *num, size_t start, bool (*f)(std::string str, int *num));
bool f_run_each_words_two(std::string str, int *num, size_t start, bool (*f)(std::string line , int *num, bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num)), bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num));

// -> functions save
// bool    f_is_file(std::string path);
size_t  f_size_cut_str(std::string path, size_t start, char caracter);
std::string  f_cut_space(std::string path, size_t start);

// check funcion
bool f_check_nothing(std::string line , int *num);
bool f_check_path_line(std::string line , int *num);
bool f_check_methods_line(std::string line , int *num);
bool f_check_port(std::string line , int *num);
bool f_check_port_listen(std::string line, int* num);
bool f_check_on(std::string line , int *num);
bool f_check_number(std::string line , int *num);
bool f_check_par(std::string line , int *num, bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num));

// verifi
bool f_str_is_only_bracket(std::string line);