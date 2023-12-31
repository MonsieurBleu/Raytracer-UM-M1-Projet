#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include <string>
#include <chrono>

/// CONSTANTS
#define PI   3.141592653589793
#define PI2  6.283185307179586
#define iPI  0.318309886183790
#define iPI2 0.159154943091895
#define E    2.718281828459045
#define PHI  1.618033988749894
#define SQR2 1.414213562373095

/// TERMINAL
const std::string TERMINAL_ERROR    = "\e[1;31m"; //"\033[91m";
const std::string TERMINAL_INFO     = "\033[94m";
const std::string TERMINAL_OK       = "\033[92m";
const std::string TERMINAL_RESET    = "\033[0m";
const std::string TERMINAL_TIMER    = "\033[93m";
const std::string TERMINAL_FILENAME = "\033[95m";
const std::string TERMINAL_WARNING  = "\e[38;5;208m";
const std::string TERMINAL_NOTIF    = "\e[1;36m";


/// FILES
/*
    TODO : replace with a faster version, using C functions (like in readOBJ)
*/
std::string readFile(const std::string& filePath);
std::string getFileExtension(const std::string &fileName);


/// CHRONO
typedef std::chrono::high_resolution_clock clockmicro;
typedef std::chrono::duration<float, std::milli> duration;

void startbenchrono();
void endbenchrono();
uint64_t GetTimeMs();

double Get_delta_time();

/// HEAP CORRUPTION
void checkHeap();

/// NOISE
#include <glm/glm.hpp>
using namespace glm;
float gold_noise3(vec3 coordinate, float seed);

#endif