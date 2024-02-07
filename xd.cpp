#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentDate() {
    // Obtain current time
    std::time_t currentTime = std::time(NULL);

    // Convert to local time
    std::tm* localTime = std::localtime(&currentTime);

    // Create a stringstream to format the date
    std::stringstream dateStream;

    // Format the date as "Day Mon DD YYYY" (e.g., "Mon Jul 27 2020")
    dateStream << std::setfill('0')
               << std::setw(2) << localTime->tm_mday << '-'
               << std::setw(2) << localTime->tm_mon + 1 << '-'
               << std::setw(4) << localTime->tm_year + 1900;

    return dateStream.str();
}

int main() {
    std::string currentDate = getCurrentDate();
    std::cout << "Current date: " << currentDate << std::endl;
    return 0;
}