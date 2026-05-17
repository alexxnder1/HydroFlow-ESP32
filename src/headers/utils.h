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
inline std::string transformaMilisecunde(long long ms) {
    if (ms <= 0) return "0 sec";

    long long secunde = ms / 1000;
    long long minute = secunde / 60;
    long long ore = minute / 60;
    long long zile = ore / 24;

    secunde %= 60;
    minute %= 60;
    ore %= 24;

    std::string rezultat = "";

    if (zile > 0) 
        rezultat += std::to_string(zile) + "z";
    
    if (ore > 0) 
        rezultat += std::to_string(ore) + "ore";
    
    if (minute > 0) 
        rezultat += std::to_string(minute) + "min";
    
    if (secunde > 0) 
        rezultat += std::to_string(secunde) + "s";
    
    if (!rezultat.empty() && rezultat.back() == ' ') 
        rezultat.pop_back();
    
    return rezultat.empty() ? "0 sec" : rezultat;
}


inline std::string getSignalQualityText(long rssi) {
  if (rssi == 0 || rssi < -100) {
    return "No Signal";
  }
  
  if (rssi >= -50) {
    return "Excellent";
  } else if (rssi >= -67) {
    return "Good";
  } else if (rssi >= -75) {
    return "Fair";
  } else if (rssi >= -85) {
    return "Weak";
  } else {
    return "Very Poor";
  }
}
