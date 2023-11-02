/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diegofranciscolunalopez <diegofrancisco    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 12:18:08 by dluna-lo          #+#    #+#             */
/*   Updated: 2023/11/02 11:11:29 by diegofranci      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Class/Config.hpp"

Config::Config()
{
  this->itr_servers = this->v_servers.begin();
  this->error = false;
}

Config::~Config()
{
}

void Config::restart_itr_servers(){
  this->itr_servers = this->v_servers.begin();
}
size_t Config::get_server_size(){
  return (this->v_servers.size());
}
Server Config::get_server(int index)
{
  if (index < 0 || (size_t)index > this->v_servers.size())
  {
    throw formatWrong();
  }
  return this->v_servers[index];
}

Server Config::get_server()
{
  if (this->itr_servers == this->v_servers.end())
  {
    this->itr_servers = this->v_servers.begin();
  }
  else
  {
    this->itr_servers++;
  }
  return *this->itr_servers;
}

bool Config::checkData(std::string dataUrl){

  try {
    if (dataUrl.empty() == true)
      return false;

    std::ifstream archivo(dataUrl);
    std::string line;

    size_t point = dataUrl.find_last_of(".");
    size_t found;
    if (point != std::string::npos)
    {
      found = dataUrl.rfind(".config\0", point);
      if (found != std::string::npos && (dataUrl.length() - point) == 7)
      {
        if (!archivo.is_open())
          throw fileNotFoundWrong();
      }else
        throw fileNotFoundWrong();
    }

    bool find_server = false;
    bool find_locacation = false;
    bool find_new_line = false;

    while (std::getline(archivo, line)){
      // check ";"
      f_check_final_line(line);
      // check "{}"
      f_check_close(line, &find_server, &find_locacation, &find_new_line);
      // check proms and values
      f_check_proms(line);
     }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << dataUrl << std::endl;
    this->error = true;
  }
  return (true);
}

bool Config::f_is_file(std::string path)
{
  std::ifstream archivo(path);

  size_t point = path.find_last_of(".");
  size_t found;
  if (point != std::string::npos)
  {
    // TO CHECK example:   config.config.confi
    found = path.find("config", point);
    if (found != std::string::npos && (path.length() - point) == 7)
    {
      if (archivo.is_open())
      {
        return true;
      }
      throw fileNotFoundWrong();
    }
    else
    {
      throw fileNotFoundWrong();
      return false;
    }
  }
  return false;
}

void Config::saveData(std::string dataUrl)
{

  std::string line = "";

  if (this->error == true)
    return;
  try
  {
    std::ifstream archivo(dataUrl);
    (dataUrl.length() == 0 || dataUrl.empty() == true) ? throw formatWrong() : true;
    if (f_is_file(dataUrl) == false)
    {
      // only path. check is the path ok?
      Server s_default;
      Location default_location;

      s_default.set_name("default");
      s_default.set_new_port("8080");
      s_default.set_host("127.0.0.1:8080");
      s_default.set_root("data/www/Pages");

      s_default.set_body_size(500);
      s_default.set_new_error_page("404", "/ErrorPages/404notFound.html");

      s_default.set_new_method("GET");
      s_default.set_new_method("POST");
      s_default.set_new_method("DELETE");

      default_location.set_name("/error");
      default_location.set_root("data/www/Pages/");
      default_location.set_new_method("GET");
      default_location.set_new_method("POST");
      default_location.set_new_method("DELETE");
      default_location.set_index("400badRequest.html");

      s_default.set_new_location(default_location);
      this->v_servers.push_back(s_default);
    }
    else
    {
      bool server = false;
      bool location = false;
      size_t num_location = 0;

      Server *s_tem;
      Location *l_tem;

      if (archivo.is_open())
      {

        while (std::getline(archivo, line))
        {
          // checl=k is open server ot location
          if (line.find("server") != std::string::npos && server == false)
          {
            server = true;
            s_tem = new Server;
            num_location = 0;
          } else if (line.find("server ") != std::string::npos && server == true){
            throw formatWrong();
          }

          if (line.find("location") != std::string::npos && location == false)
          {
            location = true;
            l_tem = new Location;
            num_location++;
          } else if (line.find("location ") != std::string::npos && location == true){
            throw formatWrong();
          }

          // check is close server or location
          if (line.find("}") != std::string::npos && server == true && location == false)
          {
            server = false;
            this->v_servers.push_back(*s_tem);
            delete s_tem;
          }
          else if (line.find("}") != std::string::npos && location == true)
          {
            location = false;
            if (l_tem->get_name().length() == 0)
              l_tem->set_name("Default" + std::to_string(num_location));
            if (l_tem->get_root().length() == 0)
              l_tem->set_root("data/www/Pages");
            if (l_tem->get_index().length() == 0)
              l_tem->set_index("400badRequest.html;");
            if (l_tem->get_methods_size() == 0)
              l_tem->set_new_method("GET");
            s_tem->set_new_location(*l_tem);
            delete l_tem;
          }
          else if (line.find("}") != std::string::npos)
          {
            throw formatWrong();
          }
          // save data
          if (server == true && location == false)
          {
            if (line.find("listen") != std::string::npos)
            {

              f_save_multiple_values_in_server(line, line.find("listen") + 7, s_tem, "ports");
            }
            if (line.find("host") != std::string::npos)
            {
              s_tem->set_host(f_cut_space(line, line.find("host") + 5));
            }
            if (line.find("methods") != std::string::npos)
            {

              f_save_multiple_values_in_server(line, line.find("methods") + 8, s_tem, "methods");
            }
            if (line.find("error_page") != std::string::npos)
            {

              f_save_multiple_values_in_server_2(line, line.find("error_page") + 11, s_tem);
            }

            if (line.find("server_name") != std::string::npos)
            {

              s_tem->set_name(f_cut_space(line, line.find("server_name") + 12));
            }
            if (line.find("body_size") != std::string::npos)
            {

              s_tem->set_body_size(std::stoll( f_cut_space(line, line.find("body_size") + 10)));
            }
            if (line.find("root") != std::string::npos)
            {

              s_tem->set_root(f_cut_space(line, line.find("root") + 5));
            }
            if (line.find("index") != std::string::npos)
            {

              s_tem->set_index(f_cut_space(line, line.find("index") + 6));
            }

            // methods
          }
          else if (server == true && location == true)
          {
            if (line.find("location") != std::string::npos)
            {

              l_tem->set_name(f_cut_space(line, line.find("location") + 9));
            }
            if (line.find("root") != std::string::npos)
            {

              l_tem->set_root(f_cut_space(line, line.find("root") + 5));
            }
            // methods
            if (line.find("methods") != std::string::npos)
            {

              f_save_multiple_values_in_location(line, line.find("methods") + 8, l_tem, "methods");
            }

            if (line.find("index") != std::string::npos)
            {

              l_tem->set_index(f_cut_space(line, line.find("index") + 6));
            }
          }
        }

        this->f_save_default();
        this->f_check_repeat();
      }
      else
      {
        throw formatWrong();
      }
    }
    this->f_check_data_with_path();
    // this->f_organise_listen();
    return;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << dataUrl << std::endl;
    this->error = true;
  }
}

struct IsLocalhostPort
{
    bool operator()(const std::string &port) const
    {
        return port.compare(0, 10, "localhost:") == 0;
    }
};

void Config::f_organise_listen()
{
    size_t tem = 0;
    std::string tem_String;

    for (size_t i = 0; i < get_server_size(); i++)
    {
        Server &s_default = get_ref_server(i); // Usar una referencia
        tem = s_default.get_ports_size();

        // Obtener los puertos de Server
        std::vector<std::string> &ports = s_default.get_ports_ref();

        // Crear un contenedor temporal para puertos con "localhost:"
        std::vector<std::string> localhostPorts;

        // Separar los puertos con "localhost:" y los demás
        for (size_t j = 0; j < tem; j++)
        {
            if (IsLocalhostPort()(ports[j]))
            {
                localhostPorts.push_back(ports[j]);
            }
        }

        // Eliminar los puertos con "localhost:" del contenedor original
        ports.erase(std::remove_if(ports.begin(), ports.end(), IsLocalhostPort()), ports.end());

        // Insertar los puertos con "localhost:" al principio del contenedor original
        ports.insert(ports.begin(), localhostPorts.begin(), localhostPorts.end());
    }
}

void Config::init_sockets(){
  try
  {
    for (size_t i = 0; i < this->get_server_size(); i++)
    {
      Server &s_default = get_ref_server(i);
      s_default.init_socket();
    }
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    this->error = true;
  }
}

void Config::seeData(void){
  if (error == true)
    return;
  try{
    std::cout << "------------------" << std::endl;
    for (size_t i = 0; i < this->get_server_size(); i++)
    {
      Server s_default = get_server(i);
      std::cout << "\n --- server> \n" << s_default << std::endl;
      for (size_t ii = 0; ii < s_default.get_location_size(); ii++)
      {
        Location default_location = s_default.get_location(ii);
        std::cout << default_location << std::endl;
      }
    }
    std::cout << "------------------" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    this->error = true;
  }
}

void Config::f_check_close(std::string line, bool *find_server, bool *find_locacation, bool *find_new_line){

  (void)find_server;
  (void)find_locacation;
  if (f_counter_worlds(line, "}") > 1 || f_counter_worlds(line, "{") > 1){
    throw formatWrong();
  }

  if (f_counter_clean_worlds(line, "server") > 1 || f_counter_clean_worlds(line, "location") > 1){
    throw formatWrong();
  }

  if (f_counter_clean_worlds(line, "server") == 1 && f_counter_worlds(line, "{") == 1 && *find_server == false){
    *find_server = true;
  }
  else if (f_counter_clean_worlds(line, "server{") == 1 && *find_server == false){
    *find_server = true;
  }
  else if (f_counter_clean_worlds(line, "server") == 1 && *find_server == false){
    *find_new_line = true;
  }
  else if (f_counter_clean_worlds(line, "server") == 1){
    throw formatWrong();
  }else if (f_counter_worlds(line, "{") == 1 && *find_new_line == true){
    *find_new_line = false;
    *find_server = true;
  }
  else{
    *find_new_line = false;
  }

  if (f_counter_worlds(line, "location") == 1 && f_counter_worlds(line, "{") == 1 && *find_server == true && *find_locacation == false){
    *find_locacation = true;
  }else if (f_counter_clean_worlds(line, "location") == 1)
  {
    throw formatWrong();
  }
  if (f_counter_worlds(line, "}") == 1 && *find_locacation == true)
  {
    *find_locacation = false;
  }
  else if (f_counter_worlds(line, "}") == 1 && *find_server == true)
  {
    *find_server = false;
  }
  else if (f_counter_worlds(line, "}") == 1){
    throw formatWrong();
  }
}

void Config::f_check_final_line(std::string line){
  if (f_counter_worlds(line, ";") > 1)
  {
    throw formatWrong();
  }

  if (f_string_has_words(line) == true && line.find(";") != std::string::npos && f_string_has_words(line.substr(line.find(";") + 1, line.length())) == true)
    throw formatWrong();
  if (f_string_has_words(line) == true
    && (line.find("{") != std::string::npos || line.find("}") != std::string::npos)
    && ((line.find("{") != (line.length() - 1)  && line.find("{") != std::string::npos)
    || (line.find("}") != (line.length() - 1) && line.find("}") != std::string::npos)))
    throw formatWrong();
}

void    Config::f_save_multiple_values_in_location(std::string path, size_t start, Location *loc, std::string metod){
  size_t words = 0;
  bool word = false;

  if (path.empty() || start >= path.length())
    throw formatWrong();

  for (size_t i = start; i < path.length(); i++)
  {
    if (word == false && path[i] != ' ' && path[i] != '\t')
    {
      word = true;
      words = i;
    }
    if (word == true && (path[i] == ' ' || path[i] == '\t' ||
        path[i] == ';' || path[i] == '\n'))
    {
      word = false;
      if (metod == "methods"){
        loc->set_new_method(f_cut_space(path, words));
      }
      else{
        throw formatWrong();
      }
    }
  }
}

void    Config::f_save_multiple_values_in_server(std::string path, size_t start, Server *ser, std::string metod) {
  size_t words = 0;;
  bool word = false;

  if (path.empty() || start >= path.length())
    throw formatWrong();

  for (size_t i = start; i < path.length(); i++)
  {
    if (word == false && path[i] != ' ' && path[i] != '\t')
    {
      word = true;
      words = i;
    }
    if (word == true && (path[i] == ' ' || path[i] == '\t' ||
        path[i] == ';' || path[i] == '\n'))
    {
      word = false;
      if (metod ==  "ports"){
        ser->set_new_port(f_cut_space(path, words));
      }
      else if (metod ==  "methods")
        ser->set_new_method(f_cut_space(path, words));
      else
        throw formatWrong();
    }
  }
}

void    Config::f_save_multiple_values_in_server_2(std::string path, size_t start, Server *ser) {
  size_t cut_words_1 = 0;
  size_t cut_words_2 = 0;
  bool word = false;
  bool pair = false;

  if (path.empty() || start >= path.length())
    throw formatWrong();

  for (size_t i = start; i < path.length(); i++)
  {
    if (word == false && path[i] != ' ' && path[i] != '\t')
    {
      word = true;
      if (pair == false){
        pair = true;
        cut_words_1 = i;
      }else{
        pair = false;
        cut_words_2 = i;
      }
    }
    if (word == true && (path[i] == ' ' || path[i] == '\t' ||
        path[i] == ';' || path[i] == '\n'))
    {
      word = false;
      if (pair == false){
        ser->set_new_error_page(f_cut_space(path, cut_words_1), f_cut_space(path, cut_words_2));
      }
    }
  }
}

void    Config::f_check_data_with_path(){
  for(size_t i = 0; i <  this->get_server_size(); i++){
    Server server_check = get_server(i);
    if (server_check.get_root().length() == 0)
      throw formatWrong();

    std::string server_root = server_check.get_root();
    int no_important = 0;
    if (server_root[server_root.length() - 1] == '/'){
      if (f_check_path_line(server_root + server_check.get_index(), &no_important) == false){
        throw fileNotFoundWrong();
      }
      server_check.restar_itr_error_page();
      for(size_t ii = 0; ii < server_check.get_error_page_size(); ii++){
        std::string error_info = server_check.get_error_page();

        if (f_check_path_line(server_root.substr(0, server_root.length() -1) + error_info.substr(error_info.find(" ")+ 1, error_info.length()), &no_important) == false){
          throw fileNotFoundWrong();
        }
      }
    }
    else{
      if (f_check_path_line(server_root + "/" + server_check.get_index(), &no_important) == false){
        throw fileNotFoundWrong();
      }
      server_check.restar_itr_error_page();
      for(size_t ii = 0; ii < server_check.get_error_page_size(); ii++){
        std::string error_info = server_check.get_error_page();

        if (f_check_path_line(server_root + error_info.substr(error_info.find(" ")+ 1, error_info.length()), &no_important) == false){
          throw fileNotFoundWrong();
        }
      }
    }
  }
}

void    Config::f_save_default_location(){
  
}

void    Config::f_save_default(){
  for(size_t i = 0; i <  this->get_server_size(); i++){
    Server server_save = get_server(i);
    if (server_save.get_name().length() == 0)
      server_save.set_name("Server" + std::to_string(i));
    if (server_save.get_ports_size() == 0)
      server_save.set_new_port("8" + std::to_string(i));
    if (server_save.get_host().length() == 0)
      server_save.set_host("8" + std::to_string(i));
    if (server_save.get_root().length() == 0)
      server_save.set_root("data/www/Pages");
    if (server_save.get_index().length() == 0)
      server_save.set_index("index.html");
    if (server_save.get_error_page_size() == 0)
      server_save.set_new_error_page("404", "/ErrorPages/404notFound.html");
    if (server_save.get_methods_size() == 0)
      server_save.set_new_method("GET");
    if (server_save.get_methods_size() == 0)
      server_save.set_new_method("GET");
    this->v_servers[i] = server_save;
  }
}

void    Config::f_check_repeat() {
  for(size_t i = 0; i <  this->get_server_size(); i++){
    Server server_check = get_server(i);

    // check de repeat listen in server
    for (size_t p = 0; p < server_check.get_ports_size(); p++){
      std::string port_check = server_check.get_ports(p);
      for (size_t pp = p + 1; pp < server_check.get_ports_size(); pp++){
          if (port_check == server_check.get_ports(pp))
            throw formatWrong();
      }
      for (size_t ii = i + 1; ii < this->get_server_size(); ii++){
        Server server_compare = get_server(ii);
        for (size_t pp = 0; pp < server_compare.get_ports_size(); pp++){
          if (port_check == server_compare.get_ports(pp))
            throw formatWrong();
      }
      }
    }

    // check de repeat name in server
    for (size_t ii = i + 1; ii < this->get_server_size(); ii++){
        Server server_compare = get_server(ii);
        if (server_check.get_name() == server_compare.get_name())
          throw formatWrong();
    }
    // check de repeat name in location
    for (size_t j = 0; j <  server_check.get_location_size(); j++)
    {
      Location location_check = server_check.get_location(j);
      for (size_t jj = j + 1; jj < server_check.get_location_size(); jj++){
        Location location_compare = server_check.get_location(jj);
        if (location_check.get_name() == location_compare.get_name()){
          throw formatWrong();
        }
      }
    }
  }

  // repeat name

}

bool Config::f_check_clean_line(std::string path, std::string find){

  size_t i_find = 0;

  if (path.empty() || find.empty())
    return (false);

  for (size_t i = 0; i < path.length(); i++){
    if (path[i] != ' ' && path[i] != '\t'){
      i_find = i;
      for (size_t ii = 0; ii < find.length(); ii++){
        if (find[ii] != path[i_find]){
          return false;
        }
        i_find++;
      }
      if (path[i_find] == ' ' || path[i_find] == '\t'){
        return true;
      }
      return true;
    }
  }
  return true;
}

// ->  try{} cath{}
const char *Config::formatWrong::what() const throw()
{
  return "Error: bad config file => ";
}
const char *Config::fileNotFoundWrong::what() const throw()
{
  return "Error: in the path => ";
}

bool Config::check_error()
{
  return this->error;
}

Server &Config::get_ref_server(size_t index)
{
	if (index < 0 || index > this->v_servers.size())
	{
		throw std::invalid_argument("Index out of range");
	}
	return this->v_servers[index];
}