#include "../Class/ResponseHeader.hpp"

///////////////////////////////////// CONST & DEST //////////////////////////////////////

ResponseHeader::ResponseHeader(int status, const char *path, int size, const char *extension) {

    std::cout << "\n-- RESPONSE HEADER --\n";
    
    this->initializeValues(status, path, size, extension);
    this->createResponseHeader();
}

ResponseHeader::~ResponseHeader() {}


//////////////////////// CREATING RESPONSEHEADER FUNCTIONS //////////////////////////////

void	ResponseHeader::createHttpStatusMap() {

    // 1xx: Informational
    this->_httpStatusMap["100"] = "Continue";
    this->_httpStatusMap["101"] = "Switching Protocols";
    this->_httpStatusMap["102"] = "Processing"; // WebDAV

    // 2xx: Success
    this->_httpStatusMap["200"] = "OK";
    this->_httpStatusMap["201"] = "Created";
    this->_httpStatusMap["202"] = "Accepted";
    this->_httpStatusMap["203"] = "Non-Authoritative Information";
    this->_httpStatusMap["204"] = "No Content";
    this->_httpStatusMap["205"] = "Reset Content";
    this->_httpStatusMap["206"] = "Partial Content";
    this->_httpStatusMap["207"] = "Multi-Status"; // WebDAV
    this->_httpStatusMap["208"] = "Already Reported"; // WebDAV
    this->_httpStatusMap["226"] = "IM Used";

    // 3xx: Redirection
    this->_httpStatusMap["300"] = "Multiple Choices";
    this->_httpStatusMap["301"] = "Moved Permanently";
    this->_httpStatusMap["302"] = "Found";
    this->_httpStatusMap["303"] = "See Other";
    this->_httpStatusMap["304"] = "Not Modified";
    this->_httpStatusMap["305"] = "Use Proxy";
    this->_httpStatusMap["306"] = "Switch Proxy";
    this->_httpStatusMap["307"] = "Temporary Redirect";
    this->_httpStatusMap["308"] = "Permanent Redirect";

    // 4xx: Client Error
    this->_httpStatusMap["400"] = "Bad Request";
    this->_httpStatusMap["401"] = "Unauthorized";
    this->_httpStatusMap["402"] = "Payment Required";
    this->_httpStatusMap["403"] = "Forbidden";
    this->_httpStatusMap["404"] = "Not Found";
    this->_httpStatusMap["405"] = "Method Not Allowed";
    this->_httpStatusMap["406"] = "Not Acceptable";
    this->_httpStatusMap["407"] = "Proxy Authentication Required";
    this->_httpStatusMap["408"] = "Request Timeout";
    this->_httpStatusMap["409"] = "Conflict";
    this->_httpStatusMap["410"] = "Gone";
    this->_httpStatusMap["411"] = "Length Required";
    this->_httpStatusMap["412"] = "Precondition Failed";
    this->_httpStatusMap["413"] = "Payload Too Large";
    this->_httpStatusMap["414"] = "URI Too Long";
    this->_httpStatusMap["415"] = "Unsupported Media Type";
    this->_httpStatusMap["416"] = "Range Not Satisfiable";
    this->_httpStatusMap["417"] = "Expectation Failed";
    this->_httpStatusMap["418"] = "I'm a teapot"; // April Fools' joke
    this->_httpStatusMap["421"] = "Misdirected Request";
    this->_httpStatusMap["422"] = "Unprocessable Entity"; // WebDAV
    this->_httpStatusMap["423"] = "Locked"; // WebDAV
    this->_httpStatusMap["424"] = "Failed Dependency"; // WebDAV
    this->_httpStatusMap["426"] = "Upgrade Required";
    this->_httpStatusMap["428"] = "Precondition Required";
    this->_httpStatusMap["429"] = "Too Many Requests";
    this->_httpStatusMap["431"] = "Request Header Fields Too Large";
    this->_httpStatusMap["451"] = "Unavailable For Legal Reasons";

    // 5xx: Server Error
    this->_httpStatusMap["500"] = "Internal Server Error";
    this->_httpStatusMap["501"] = "Not Implemented";
    this->_httpStatusMap["502"] = "Bad Gateway";
    this->_httpStatusMap["503"] = "Service Unavailable";
    this->_httpStatusMap["504"] = "Gateway Timeout";
    this->_httpStatusMap["505"] = "HTTP Version Not Supported";
    this->_httpStatusMap["506"] = "Variant Also Negotiates";
    this->_httpStatusMap["507"] = "Insufficient Storage"; // WebDAV
    this->_httpStatusMap["508"] = "Loop Detected"; // WebDAV
    this->_httpStatusMap["510"] = "Not Extended";
    this->_httpStatusMap["511"] = "Network Authentication Required";
}

void ResponseHeader::createStatusLine( void ){

    this->_statusLine += this->_HTTPVersion + " ";
    this->_statusLine += this->_statusCode + " ";
    this->_statusLine += this->_reasonPhrase;
}

void	ResponseHeader::createResponseHeader( void ){

    // append every fucking string together to create the headers to get something like the example below

    // EXAMPLE

    // HTTP/1.1 200 OK
    // Date: Mon, 27 Jul 2020 12:28:53 GMT
    // Server: Apache/2.4.1 (Unix)
    // Last-Modified: Wed, 22 Jul 2020 19:15:56 GMT
    // Content-Length: 88
    // Content-Type: text/html
    // Connection: Closed

    this->createStatusLine();
    this->_responseHeader += this->_statusLine + "\n";
    this->_responseHeader += "Date: " + this->_date + "\n";
    this->_responseHeader += "Server: " + this->_server + "\n";
    if (!this->_lastModified.empty())
        this->_responseHeader += "Last-Modified: " + this->_lastModified + "\n";
    this->_responseHeader += "Content-Length: " + this->_contentLength + "\n";
	if (!this->_contentType.empty())
    	this->_responseHeader += "Content-Type: " + this->_contentType + "\n";
    this->_responseHeader += "Connection: " + this->_connection + "\n";
    this->_responseHeader += "\r\n";

    std::cout << this->_responseHeader;

}

//////////////////////// VALUE INITIALIZERS //////////////////////////////

std::string ResponseHeader::getMimeType(const char* extension){

	if (!extension)
		return "";
    std::map<std::string, std::string> mimeTypes;
	mimeTypes[".html"] = "text/html";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".ico"] = "image/x-icon";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".js"] = "application/javascript";
	// Add more mappings as needed

    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
    if (it != mimeTypes.end()) {
        return it->second;
    } else {
        return "UNKNOWN TYPE"; // default MIME type for unknown types
    }
}


std::string			ResponseHeader::initLastModified(const char* path){
    
	if (path == NULL)
		return "";
	struct stat		stats;
	struct tm		*tm;

	if (stat(path, &stats) == 0)
	{
		tm = gmtime(&stats.st_mtime);
        return (structToTime(tm));
	}
    return "";
}

void  ResponseHeader::initializeValues( int status, const char *path, int size, const char *extension){

    this->_HTTPVersion = "HTTP/1.1";
    this->_statusCode = this->nbToString(status); // received from Response
    this->_reasonPhrase = _httpStatusMap[this->_statusCode];
    this->_date = this->initCurrentDateTimeGMT();
    this->_server = "webserv 0.1";
    this->_lastModified = this->initLastModified(path); // received from Response
    this->_contentLength = this->nbToString(size); // received from Response
    this->_contentType = this->getMimeType( extension); // received from Response
    this->_connection = "close";
}


std::string    ResponseHeader::initCurrentDateTimeGMT( void ){

    // Get current time
    std::time_t currentTime = std::time(NULL);
    std::tm *gmtTime = std::gmtime(&currentTime);
    return (this->structToTime(gmtTime));
}

///////////////////////////////////// UTILS & DEBUG //////////////////////////////////////

std::string ResponseHeader::structToTime(const tm *time){
    const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    std::stringstream dateTimeStream;

    // Format the date and time
    dateTimeStream << weekdays[time->tm_wday] << ", "
                   << std::setfill('0') << std::setw(2) << time->tm_mday << " "
                   << months[time->tm_mon] << " "
                   << (1900 + time->tm_year) << " "
                   << std::setfill('0') << std::setw(2) << time->tm_hour << ":"
                   << std::setfill('0') << std::setw(2) << time->tm_min << ":"
                   << std::setfill('0') << std::setw(2) << time->tm_sec << " GMT";

    return dateTimeStream.str();
}

std::string ResponseHeader::nbToString( int n ){
    // converts an int to a std::string using a stream
    std::stringstream   ss;
    if (n < 0)
        n = 0;
    ss << n;
    return ss.str();
}

///////////////////////////////////// GETTERS //////////////////////////////////////

const std::string &ResponseHeader::getResponseHeader() const{
    return (this->_responseHeader);
}

const std::string &ResponseHeader::getStatusLine() const{
    return (this->_statusLine);
}

const std::string &ResponseHeader::getHTTPVersion() const{
    return (this->_HTTPVersion);
}

const std::string &ResponseHeader::getStatusCode() const{
    return (this->_statusCode);
}

const std::string &ResponseHeader::getReasonPhrase() const{
    return (this->_reasonPhrase);
}

const std::string &ResponseHeader::getDate() const{
    return (this->_date);
}

const std::string &ResponseHeader::getServer() const{
    return (this->_server);
}

const std::string &ResponseHeader::getLastModified() const{
    return (this->_lastModified);
}

const std::string &ResponseHeader::getContentLength() const{
    return (this->_contentLength);
}

const std::string &ResponseHeader::getContentType() const{
    return (this->_contentType);
}

const std::string &ResponseHeader::getConnection() const{
    return (this->_connection);
}
