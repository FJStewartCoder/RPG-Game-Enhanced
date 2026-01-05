#include <exception>
#include <string>

// taken from "https://www.geeksforgeeks.org/cpp/how-to-throw-custom-exception-in-cpp/"
class CustomException : public std::exception {
    private:
        std::string message;
    public:

        // Constructor accepting const char*
        CustomException(const char* msg) :
        message(msg) {}

        // Override what() method, marked
        // noexcept for modern C++
        const char* what() const noexcept {
            return message.c_str();
        }
};