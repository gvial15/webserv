// #include "Server.hpp"
// #include "../../Includes/Parsing.hpp"
#include "../../Class/Server.hpp"
#include "../../Class/Config.hpp"

Server::Server()
{
  this->itr_ports = this->_ports.begin();
  this->itr_error_page = this->_error_page.begin();
  this->itr_methods = this->_methods.begin();
  this->itr_locations = this->v_locations.begin();


  this->_name = "";
  this->_host = "";
  this->_root = "";
  this->_index = "";
  this->_body_size = -1; // no limit




	// for (size_t i = 0; i < this->_ports.size(); i++)
	// {
	// 	listenner tmp = listenner(std::stoi(this->_ports[i]));
	// 	this->_listenners.push_back(tmp);
	// }
}
Server::~Server()
{
}

std::string Server::get_name(){
  return this->_name;
}

void Server::restar_itr_port(){
  this->itr_ports = this->_ports.begin();
}

size_t Server::get_ports_size(){
  return (this->_ports.size());
}

std::string Server::get_ports(){
  std::string port = *this->itr_ports;
  if (this->itr_ports == this->_ports.end())
    this->itr_ports = this->_ports.begin();
  else
    this->itr_ports++;
  return port;
}

std::string Server::get_ports(int index){
  if (index < 0 || (size_t)index > this->_ports.size())
  {
    throw std::invalid_argument("Index out of range (get_ports) ");
  }

  return this->_ports[index];
}

std::string Server::get_host(){
  return this->_host;
}

std::string Server::get_root(){
  return this->_root;
}

std::string Server::get_index(){
  return this->_index;
}

size_t Server::get_error_page_size(){
  return (this->_error_page.size());
}

void Server::restar_itr_error_page()
{
  this->itr_error_page = this->_error_page.begin();
}

std::string Server::get_error_page(){
  std::string error_page_1 = this->itr_error_page->first;
  std::string error_page_2 = this->itr_error_page->second;
  if (this->itr_error_page == this->_error_page.end())
    this->itr_error_page = this->_error_page.begin();
  else
    this->itr_error_page++;
  return (error_page_1 + " " + error_page_2);
}

std::string Server::get_error_page(std::string index){
  if (index.empty())
  {
    throw std::invalid_argument("Index out of range (get_error_page) ");
  }

  return this->_error_page[index];
}

size_t Server::get_methods_size(){
  return (this->_methods.size());
}

std::string Server::get_methods(){
  std::string method = *this->itr_methods;
  if (this->itr_methods == this->_methods.end())
    this->itr_methods = this->_methods.begin();
  else
    this->itr_methods++;
  return method;
}

std::string Server::get_methods(int index){
  if (index < 0 || (size_t)index > this->_methods.size())
  {
    throw std::invalid_argument("Index out of range (get_methods) ");
  }

  return this->_methods[index];
}

size_t Server::get_location_size(){
  return (this->v_locations.size());
}

void Server::restar_itr_location(){
  this->itr_locations = this->v_locations.begin();
}

Location Server::get_location(){
  Location tem = *this->itr_locations;
  if (this->itr_locations == this->v_locations.end())
    this->itr_locations = this->v_locations.begin();
  else
    this->itr_locations++;
  return tem;
}

Location Server::get_location(int index){
  if (index < 0 || (size_t)index > this->v_locations.size())
  {
    throw std::invalid_argument("Index out of range (get_location)");
  }

  return this->v_locations[index];
}

void Server::set_name(std::string name){
  this->_name = name;
}

void Server::set_new_port(std::string port){
  this->_ports.push_back(port);
}

void Server::set_body_size(int size){
  this->_body_size = size;
}

void Server::set_new_location(Location lo){
  this->v_locations.push_back(lo);
}

void Server::set_host(std::string host){
  this->_host = host;
}

void Server::set_root(std::string root){
  this->_root = root;
}

void Server::set_index(std::string index){
  this->_index = index;
}

void Server::set_new_error_page(std::string index, std::string path){
  this->_error_page.insert(std::pair<std::string , std::string>(index, path));
}

void Server::set_new_method(std::string method){
  this->_methods.push_back(method);
}
void Server::restar_itr_methods(){
  this->itr_methods = this->_methods.begin();
}


std::ostream& operator<<(std::ostream& out, Server& src) {
    out << "Server name:" << src.get_name() << "\n";
    src.restar_itr_port();
    for (size_t i = 0; i < src.get_ports_size(); i++)
    {
      out << "Server listen:" << src.get_ports((int)i) << "\n";
    }
    out << "Server host:" << src.get_host() << "\n";
    out << "Server root:" << src.get_root() << "\n";
    out << "Server index:" << src.get_index() << "\n";

    src.restar_itr_error_page();
    src.restar_itr_methods();
    for (size_t i = 0; i < src.get_error_page_size(); i++)
    {
      out << "Server error_page:" << src.get_error_page() << "\n";
    }
    for (size_t i = 0; i < src.get_methods_size(); i++)
    {
      out << "Server methods:" << src.get_methods((int)i) << "\n";
    }

    return out;
}

// NEW
// create socket, make it non-blocking, bind() and listen()
void	Server::init_socket()
{
  struct sockaddr_in address;
	int flags;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        throw SocketException();

    // Set the socket to non-blocking mode
    flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1)
      throw fcntl_F_GETFL_Exception();
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
      throw fcntl_F_SETFL_O_NONBLOCK_Exception();

    // setup sockaddr_in // ip -> host
    // if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    if (inet_pton(AF_INET, get_host().c_str(), &address.sin_addr) <= 0)
      throw inet_pton_Exception();
    address.sin_family = AF_INET;
    // port -> port
    int port = atoi(get_ports(0).c_str());
    address.sin_port = htons(port);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw BindException();

    // Listening for a connection
    if (listen(server_fd, 3) < 0)
        throw ListenException();
}


// void Server::initPorts(void)
// {

//   for (size_t i = 0; i < this->_ports.size(); i++)
//   {
// 	    listenner tmp = listenner(std::stoi(this->_ports[i]), this->v_locations.at(0));

//       // location is weird
      
// 	    this->_listenners.push_back(tmp);
//   }
//   std::cout << this->_listenners.size() << std::endl;
// }

// size_t Server::get_listenners_size()
// {
// 	return this->_listenners.size();
// }
// listenner &Server::get_listenners(int index)
// {
//   	return this->_listenners[index];
// }


std::vector<std::string>& Server::get_ports_ref()
{
    return _ports;
}
