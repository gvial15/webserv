/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_utils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diegofranciscolunalopez <diegofrancisco    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 12:05:10 by dluna-lo          #+#    #+#             */
/*   Updated: 2023/11/02 09:30:18 by diegofranci      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Class/Config.hpp"

void Config::f_check_proms(std::string line){
  int num_worlds = 0;

  if (f_counter_clean_worlds(line, "server") == 1 && f_check_clean_line(line, "server") == true
    && f_count_words_string(line) <= 2){
    return ;
  }else if (f_counter_clean_worlds(line, "location") == 1 && f_check_clean_line(line, "location") == true &&
    f_count_words_string(line) == 3){
    return ;
  }else if (f_counter_clean_worlds(line, "listen") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("listen") + 7, &f_check_port) == true &&
    num_worlds > 0 && f_check_clean_line(line, "listen") == true){
    return ;
  }

  else if (f_counter_clean_worlds(line, "host") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("host") + 7, &f_check_port) == true &&
    num_worlds > 0 && f_check_clean_line(line, "host") == true){
    return ;
  }

  else if (f_counter_clean_worlds(line, "server_name") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("server_name") + 12, &f_check_nothing) == true &&
    num_worlds == 1 && f_check_clean_line(line, "server_name") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "root") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("root") + 5, &f_check_path_line) == true &&
    num_worlds == 1 && f_check_clean_line(line, "root") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "index") == 1 && f_check_clean_line(line, "index") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "methods") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("methods") + 8, &f_check_methods_line) == true &&
    num_worlds > 0 && f_check_clean_line(line, "methods") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "error_page") == 1 &&
    f_run_each_words_two(line, &num_worlds, line.find("error_page") + 11, &f_check_par, &f_check_number, &f_check_nothing) == true &&
    num_worlds % 2 == 0 && f_check_clean_line(line, "error_page") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "autoindex") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("autoindex") + 10, &f_check_on) == true &&
    num_worlds == 1 && f_check_clean_line(line, "autoindex") == true){
    return ;
  }else if (f_counter_clean_worlds(line, "body_size") == 1 &&
    f_run_each_words(line, &num_worlds, line.find("body_size") + 10, &f_check_number) == true &&
    num_worlds == 1 && f_check_clean_line(line, "body_size") == true){
    return ;
  }else if(f_string_has_words(line) == true &&
    f_counter_clean_worlds(line, "}") == 0 &&
    f_counter_clean_worlds(line, "{") == 0){
    throw formatWrong();
  }
  else if(f_string_has_words(line) == true && f_str_is_only_bracket(line) == false){
    throw formatWrong();
  }
  // }else if(f_string_has_words(line) == true &&
  //   f_str_is_only_bracket(line) == false){
  //   throw formatWrong();
  // }
}

bool f_check_on(std::string line, int *num){
  (void)num;
  if (line.empty())
    return false;
  if (line == "on"){
    return true;
  }
  if (line == "off"){
    return true;
  }
  return false;
}

bool f_check_number(std::string line, int *num){
  (void)num;
  if (line.empty())
    return false;
  for(size_t i = 0; i < line.length(); i++){
    if (!(line[i] >= '0' && line[i] <= '9'))
      return false;
  }
  return true;
}

bool f_check_path_line(std::string line, int *num){

  (void)num;
  if (line.empty())
    return false;
  std::ifstream archivo(line);
  if (!archivo.is_open()){
    return false;
  }
  return true;
}

bool f_check_port(std::string line, int *num){
  (void)num;
  bool is_local = false;
  bool is_point = false;
  if (( f_counter_worlds(line, "localhost") > 1 || f_counter_worlds(line, "localhost:") > 1 || f_counter_worlds(line, "http://localhost:") > 1))
    return false;
  if ((f_counter_worlds(line, "localhost:") == 1 ||f_counter_worlds(line, "http://localhost:") == 1))
    is_local = true;

  for (size_t i = 0; i < line.length(); i++)
  {
    if (line[i] == '.' && i == (line.length() - 1))
      return false;
    if (is_point == false && line[i] == '.')
      is_point = true;
    else if (is_point == true && line[i] == '.')
      return false;
    else if (line[i] != '.')
      is_point = false;
    if (!(line[i] >= '0' && line[i] <= '9') && line[i] != '.'){
      if (is_local == false)
        return false;
      if (i == 0 &&
      line.length() > 10 &&
      line.substr(i, i + 10) == "localhost:" &&
      f_check_port(line.substr(10, line.length()), num) == true
      ){
      // if (i == 0 && (
      //   (line[i] == 'h' && line.length() > 10  && line.substr(0, 10) == "localhost:" && f_check_port(line.substr(10, line.length()), num) == true)
      //   || (line[i] == 'l' && line.length() > 17 && line.substr(0, 17) == "http://localhost:" && f_check_port(line.substr(17, line.length()), num) == true))){
          // is_local = false;
          continue;
      }
      else if (i == 0) {
        return false;
      }
    }
  }
  return true;
}

// bool Config::f_check_methods_line(std::string line){
bool f_check_methods_line(std::string line, int *num){
  (void) num;
  if (line == "GET" || line == "POST" || line == "DELETE")
    return true;
  return false;
}


bool f_check_par(std::string line , int *num, bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num)){

  if (line.empty())
    return false;

  if ((((*num) + 1) % 2) == 1 && f_2(line, num) == false)
    return false;

  if ((((*num) + 1) % 2) == 0 && f_1(line, num) == false)
    return false;

  return true;
}

bool f_check_nothing(std::string line , int *num){

  (void) num;
  if (line.empty())
    return false;
  return true;
}
