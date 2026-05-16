#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <utility> 

inline void mini_format_impl(std::ostringstream& ss, const std::string& fmt, size_t pos) {
    ss << fmt.substr(pos);
}

template<typename T, typename... Args>
void mini_format_impl(std::ostringstream& ss, const std::string& fmt, size_t pos, T&& arg, Args&&... args) {
    size_t brace = fmt.find("{}", pos);
    if (brace == std::string::npos) {
        throw std::runtime_error("Too many arguments provided to format string.");
    }
    
    ss << fmt.substr(pos, brace - pos);
    ss << std::forward<T>(arg); // Fixed: Added std::forward for perfect forwarding
    mini_format_impl(ss, fmt, brace + 2, std::forward<Args>(args)...);
}

template<typename... Args>
std::string basic_format(const std::string& fmt, Args&&... args) {
    std::ostringstream ss;
    mini_format_impl(ss, fmt, 0, std::forward<Args>(args)...);
    return ss.str();
}