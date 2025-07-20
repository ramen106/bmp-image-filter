#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>
#include "prototypes.h"
#include "msdn_bmp_datatypes.h"

std::vector<std::vector<RGBTRIPLE>> ImageFilters::grayscale(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image)
{
    std::cout << "Grayscale function called with height=" << height << ", width=" << width << std::endl;
    
    // Validate input parameters
    if (height <= 0 || width <= 0 || height > 50000 || width > 50000) {
        std::cout << "Invalid dimensions in grayscale function" << std::endl;
        return std::vector<std::vector<RGBTRIPLE>>();
    }
    
    // Check if input image has correct dimensions
    if (image.size() != height)
    {
        std::cout << "Input image height mismatch: expected " << height << ", got " << image.size() << std::endl;
        return std::vector<std::vector<RGBTRIPLE>>();
    }
    
    std::vector<std::vector<RGBTRIPLE>> filteredImage;
    
    if (height > 0 && height < 50000)  // some reasonable upper limit
    {
        filteredImage.resize(height);
    }
    else
    {
        std::cout << "Invalid height: " << height << std::endl;
        return {};
    }
    
    // Convert the colors to linear to easily apply illuminated grayscale
    // Code snippet ported from Stack Overflow

    auto to_linear = [](double c) -> double {
        c /= 255.0;
        return (c <= 0.04045) ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4);
    };

    // Convert the colors back to SRGB
    // Code snippet ported from Stack Overflow

    auto to_srgb = [](double c) -> uint8_t {
        c = (c <= 0.0031308) ? c * 12.92 : 1.055 * pow(c, 1.0 / 2.4) - 0.055;
        int result = static_cast<int>(round(c * 255.0));
        return clamp(result, 0, 255);
    };

    // Process one row at a time to avoid large memory allocation
    for (int i = 0; i < height; ++i)
    {
        // Check if the row index exists and the row has correct amount of pixels
        if (i >= image.size() || image[i].size() != width)
        {
            std::cout << "Input image row " << i << " size mismatch: expected " << width;
            if (i < image.size()) 
            {
            std::cout << ", got " << image[i].size();
            }
            std::cout << std::endl;
            return std::vector<std::vector<RGBTRIPLE>>();
        }

        filteredImage[i].resize(width);

        for (int j = 0; j < width; ++j)
        {
            const RGBTRIPLE& pixel = image[i][j];

            // Convert to linear light
            double r_lin = to_linear(pixel.rgbtRed);
            double g_lin = to_linear(pixel.rgbtGreen);
            double b_lin = to_linear(pixel.rgbtBlue);

            // Weighted grayscale (linear space)
            double gray_lin = 0.2126 * r_lin + 0.7152 * g_lin + 0.0722 * b_lin;

            // Convert back to sRGB
            uint8_t gray = to_srgb(gray_lin);

            // Set pixel to grayscale
            filteredImage[i][j].rgbtRed = gray;
            filteredImage[i][j].rgbtGreen = gray;
            filteredImage[i][j].rgbtBlue = gray;
        }
    }
    return filteredImage;
}

std::vector<std::vector<RGBTRIPLE>> ImageFilters::reflect(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image)
{
    RGBTRIPLE temp;
    float halfWidth = width/2;
    for (int i = 0; i < height; i++)
    {
        for(int j = 0; j < halfWidth; j++)
        {
            temp = image[i][width-j-1];
            image[i][width-j-1] = image[i][j];
            image[i][j] = temp; 
        }
    }
    return image;
}

std::vector<std::vector<RGBTRIPLE>> ImageFilters::edges(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image)
{
    std::vector<std::vector<RGBTRIPLE>> original = image;
    image = blur(height, width, original);
    
    for (int i = 0; i < height-1; i++)
    {
        for (int j = 0; j < width-1; j++)
        {
            G Gx;
            G Gy;
            G Gt;
            Gx.red = Gx.green = Gx.blue = Gy.red = Gy.green = Gy.blue = Gt.red = Gt.green = Gt.blue = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    // Boundary checks
                    if (ni < 0 || ni >= height || nj < 0 || nj >= width)
                    {
                        continue;
                    }

                    // Apply Sobel Operator
                    Gx.red += original[ni][nj].rgbtRed * hs_kernel[di + 1][dj + 1];
                    Gy.red += original[ni][nj].rgbtRed * vs_kernel[di + 1][dj + 1];
                    
                    Gx.green += original[ni][nj].rgbtGreen * hs_kernel[di + 1][dj + 1];
                    Gy.green += original[ni][nj].rgbtGreen * vs_kernel[di + 1][dj + 1];

                    Gx.blue += original[ni][nj].rgbtBlue * hs_kernel[di + 1][dj + 1];
                    Gy.blue += original[ni][nj].rgbtBlue * vs_kernel[di + 1][dj + 1];
                    
                }
            }

            Gt.red = clamp(static_cast<int>(sqrt(Gx.red * Gx.red + Gy.red * Gy.red)), 0, 255);
            Gt.green = clamp(static_cast<int>(sqrt(Gx.green * Gx.green + Gy.green * Gy.green)), 0, 255);
            Gt.blue = clamp(static_cast<int>(sqrt(Gx.blue * Gx.blue + Gy.blue * Gy.blue)), 0, 255);

            // Update image
            image[i][j].rgbtBlue = Gt.blue;
            image[i][j].rgbtRed = Gt.red;
            image[i][j].rgbtGreen = Gt.green;

        }
    }
    return image;
}

std::vector<std::vector<RGBTRIPLE>> ImageFilters::blur(int height, int width, std::vector<std::vector<RGBTRIPLE>>& image)
{
    // Perform blur on a copy of the image rather than in-place to avoid un-even blur 

    std::vector<std::vector<RGBTRIPLE>> result = image;
    for (int i = 1; i < height-1; i++)
    {
        for (int j = 0; j < width-1; j++)
        {
            int redSum = 0;
            int greenSum = 0;
            int blueSum = 0; 
            int count = 0;

            // The following loops will displace the pixels to access the neighbouring ones
            // and sum up their RGB values

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    
                    // Boundary checks
                    if (ni < 0 || ni >= height || nj < 0 || nj >= width)
                    {
                        continue;
                    }

                    redSum += result[ni][nj].rgbtRed;
                    greenSum += result[ni][nj].rgbtGreen;
                    blueSum += result[ni][nj].rgbtBlue;
                    count++;
                }
            }

            // Average the color values of all pixels in a grid and apply them to the image
            BYTE redBlurVal = BYTE(redSum / count);
            BYTE greenBlurVal = BYTE(greenSum / count);
            BYTE blueBlurVal = BYTE(blueSum / count); 
            result[i][j].rgbtRed = redBlurVal;
            result[i][j].rgbtGreen = greenBlurVal;
            result[i][j].rgbtBlue = blueBlurVal;
        }
    }
    return result;
}

void makeFile(std::ofstream& output, std::vector<std::vector<RGBTRIPLE>> filteredImage, LONG biWidth, LONG biHeight)
{
    std::cout << "Writing filtered image to file...\n";

    // Calculate padding
    int padding = (4 - (biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
        
    // Write pixel data (bottom-to-top for BMP format)
    for (int i = biHeight - 1; i >= 0; i--)
    {
        // Write one row of pixels
        output.write(reinterpret_cast<const char*>(filteredImage[i].data()), biWidth * sizeof(RGBTRIPLE));

        // Write padding bytes
        for (int p = 0; p < padding; p++)
        {
            output.put(0);
        }
    }
}