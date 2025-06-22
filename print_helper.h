#pragma once
#include <iostream>
#include <cstdio>
#include "no_abbreviations.h"
#include <sstream>

inline std::vector<std::string>& get_console_messages() {
    static std::vector<std::string> console_log_messages;
    return console_log_messages;
}

enum class print_level{
    debug,
    info,
    warning,
    error
};

namespace colours {
    constant_expression const char* reset = "\033[0m";
    
    // omelette colours
    constant_expression const char* dark_brown = "\033[38;2;41;30;26m";      // #291e1a
    constant_expression const char* brown = "\033[38;2;90;52;40m";           // #5a3428
    constant_expression const char* light_brown = "\033[38;2;166;101;43m";   // #a6652b
    constant_expression const char* gold = "\033[38;2;217;164;67m";          // #d9a443
    constant_expression const char* orange = "\033[38;2;201;89;55m";         // #c95937
    constant_expression const char* red = "\033[38;2;137;63;67m";            // #893f43
    constant_expression const char* olive = "\033[38;2;57;62;38m";           // #393e26
    constant_expression const char* teal = "\033[38;2;73;102;105m";          // #496669
    constant_expression const char* blue = "\033[38;2;118;135;171m";         // #7687ab
    constant_expression const char* light_gray = "\033[38;2;212;211;209m";   // #d4d3d1
    constant_expression const char* lime = "\033[38;2;116;139;44m";          // #748b2c
    constant_expression const char* dark_green = "\033[38;2;49;84;47m";      // #31542f
}

inline const char* level_to_string(print_level level) {
    switch(level) {
        case print_level::debug:   return "DEBUG";
        case print_level::info:    return "INFO";
        case print_level::warning: return "WARN";
        case print_level::error:   return "ERROR";
        default:                   return "UNKNOWN";
    }
}

template<typename... args>
inline void log(print_level level, const args&... arguments) {
    switch(level) {
        case print_level::debug:   
            std::cout << colours::light_gray << "[DEBUG] " << colours::reset; 
            break;
        case print_level::info:    
            std::cout << colours::blue << "[INFO] " << colours::reset;  
            break;
        case print_level::warning: 
            std::cout << colours::gold << "[WARN] " << colours::reset;  
            break;
        case print_level::error:   
            std::cout << colours::orange << "[ERROR] " << colours::reset; 
            break;
    }
    (std::cout << ... << arguments) << '\n';

    // pass messages to the in-engine console
    std::stringstream string_stream;
    string_stream << "[]" << level_to_string(level) << "]";
    (string_stream << ... << arguments);
    get_console_messages().push_back(string_stream.str());
}

template<typename... args>
inline void log_debug(const args&... arguments) {
    log(print_level::debug, arguments...);
}

template<typename... args>
inline void log_info(const args&... arguments) {
    log(print_level::info, arguments...);
}

template<typename... args>
inline void log_warning(const args&... arguments) {
    log(print_level::warning, arguments...);
}

template<typename... args>
inline void log_error(const args&... arguments) {
    log(print_level::error, arguments...);
}