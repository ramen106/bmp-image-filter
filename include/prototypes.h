// Prototypes and needed definitions for the program

#pragma once
#include "msdn_bmp_datatypes.h"
#include <vector>


// Clamp definition ported because my compiler doesn't let me use it for some reason
template <typename T>
T clamp(T value, T low, T high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}


// Struct representing the Sobel Gradients

struct G
{
    int red;
    int green;
    int blue;
};

// Namespace including the filter functions
namespace ImageFilters
{
    // Convert image to grayscale
    std::vector<std::vector<RGBTRIPLE>> grayscale(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image);

    // Reflect image horizontally
    std::vector<std::vector<RGBTRIPLE>> reflect(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image);

    // Detect edges
    std::vector<std::vector<RGBTRIPLE>> edges(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image);

    // Blur image
    std::vector<std::vector<RGBTRIPLE>> blur(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image); 
}

void makeFile(std::ofstream& output, std::vector<std::vector<RGBTRIPLE>> filteredImage, LONG biWidth, LONG biHeight);


// Horizontal and Vertical Sobel operators pre-defined to ease calculations
const std::vector<std::vector<int>> hs_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const std::vector<std::vector<int>> vs_kernel = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
