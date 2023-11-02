/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uitlParsing.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diegofranciscolunalopez <diegofrancisco    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 11:48:42 by dluna-lo          #+#    #+#             */
/*   Updated: 2023/11/02 08:45:38 by diegofranci      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Class/Config.hpp"

int f_counter_worlds(std::string str, std::string find){
  if (find.empty() || str.empty())
    return -1;

  size_t i_find = 0;
  size_t n_words = 0;
  for (size_t i = 0; i < str.length(); i++)
  {
    if (i_find < find.length() && find[i_find] == str[i]) {
      i_find++;
    } else {
      i_find = 0;
    }

    if (i_find == find.length()) {
      n_words++;
      i_find = 0;
    }
  }
  return (int)n_words;
}

int f_counter_clean_worlds(std::string str, std::string find){
  if (find.empty() || str.empty())
    return -1;

  long int i_find = 0;
  long int n_words = 0;
  for (long int i = 0; i < (long int)str.length() && i_find < (long int)find.length(); i++)
  {
    if (i_find < (long int)find.length() && find[i_find] == str[i] && (i == 0 || (i - 1 - i_find) <= 0 || ((str[i - 1 - i_find] == ' ') || str[i - 1 - i_find] == '\t'))) {
      i_find++;
    } else {
      i_find = 0;
    }
    if (i_find == (long int)find.length() && (str[i + 1] == '\0' || str[i + 1] == ' ' || str[i + 1] == '\t'  )) {
      n_words++;
      i_find = 0;
    }
  }
  return (int)n_words;
}

bool f_string_has_words(std::string str){
  if (str.empty())
    return false;

  for (size_t i = 0; i < str.length(); i++)
  {
    if (str[i] >= 33 && str[i] <= 126 &&  (str[i] != 123 || str[i] != 125) )
    {
      return true;
    }
  }
  return false;
}

size_t f_size_cut_str(std::string path, size_t start, char caracter)
{

  size_t end = 0;

  if (start >= path.length())
  {
    return 0;
  }
  for (size_t i = start; i < path.length(); i++)
  {
    if (path[i] == caracter)
    {
      end = i;
      break;
    }
  }
  return (end - start);
}

std::string f_cut_space(std::string path, size_t start)
{
  size_t cut_end = 0;
  size_t cut_start = 0;

  if (start < 0 || start > path.length())
    throw std::exception();
    // throw formatWrong();
  for (size_t i = start; i < path.length(); i++)
  {
    if (path[i] != ' ' && path[i] != '\t')
    {
      cut_start = i;
      break;
    }
  }
  for (size_t i = cut_start; i < path.length(); i++)
  {
    if (path[i] == ' ' || path[i] == '\t' ||
        path[i] == ';' || path[i] == '\n')
    {
      cut_end = i;
      break;
    }
  }

  return path.substr(cut_start, cut_end - cut_start);
}


bool f_run_each_words(std::string str, int *num, size_t start, bool (*f)(std::string str, int *num))
{
	size_t cut_end = 0;
  size_t cut_start = 0;

  *num = 0;

  // std::cout << "\n\n start{" << start << "}" << std::endl;
  // std::cout << "\n\n str{" << str.substr(start, str.length()) << "}" << std::endl;

	if (str.empty() || start > str.length())
		return false;
	for(size_t i = start; i < str.length(); i++){
		 if (cut_start == 0 && (str[i] != ' ' && str[i] != '\t'))
    {
      cut_start = i;
    }else if (cut_start != 0 && cut_end == 0 && (str[i] == ' ' || str[i] == '\t' ||
        str[i] == ';' || str[i] == '\n'))
    {
      cut_end = i;
      *num = *num + 1;
      if ((*f)(str.substr(cut_start, cut_end - cut_start), num) == false)
        return false;
			cut_end = 0;
      cut_start = 0;

		}
	}
	return true;
}

bool f_run_each_words_two(std::string str, int *num, size_t start, bool (*f)(std::string line , int *num, bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num)), bool (*f_1)(std::string str, int *num), bool (*f_2)(std::string str, int *num))
{
	size_t cut_end = 0;
  size_t cut_start = 0;

  *num = 0;
	if (str.empty() || start > str.length())
		return false;
	for(size_t i = start; i < str.length(); i++){
		 if (cut_start == 0 && (str[i] != ' ' && str[i] != '\t'))
    {
      cut_start = i;
    }else if (cut_start != 0 && cut_end == 0 && (str[i] == ' ' || str[i] == '\t' ||
        str[i] == ';' || str[i] == '\n'))
    {
      cut_end = i;
      *num = *num + 1;
      if ((*f)(str.substr(cut_start, cut_end - cut_start), num, f_1, f_2) == false)
        return false;
			cut_end = 0;
      cut_start = 0;

		}
	}
	return true;
}

bool f_str_is_only_bracket(std::string line){
  if (line.empty()){
    return false;
  }

  for(size_t i = 0; i < line.length(); i++){
    if (!(line[i] == ' ' || line[i] == '\t') && !(line[i] == '{' || line[i] == '}')){
      return false;
    }
  }
  return true;
}

size_t f_count_words_string(std::string str){
  size_t count = 0;
  bool insideWord = false;

  for (size_t i = 0; i < str.length(); i++) {
      if (str[i] == ' ' || str[i] == '\t') {
          insideWord = false;
      } else {
          if (!insideWord) {
              count++;
              insideWord = true;
          }
      }
  }

    return count;
}