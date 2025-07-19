#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>
#include "prototypes.h"
#include "msdn_bmp_datatypes.h"

int main ()
{
    std::string inPic;
    std::ifstream input;
    bool inUse = true;
    while(inUse)
    {
        // Take in the picture intended for modification
        std::cout << "Input the name of the picture, including \".bmp\". Enter 'quit' without any extensions to end the program: ";
        std::cin >> inPic;

        if (inPic == "quit")
        {
            inUse = false;
            continue;
        }

        // Open the file
        input.open(inPic, std::ios::binary);

        // Handle incorrect input or failure to open.
        while (std::cin.fail() || !input)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "There was an issue with your input, please try again: ";
            std::cin >> inPic;
            if (inPic == "quit")
            {
                inUse = false;
                return 1;
            }
        }

        // Choose filter as a string
        std::string filter;
        std::cout << "Please enter the filter you wish to apply (Blurred, Grayscaled, Edges, or Reflected): ";
        std::cin >> filter;
        // Convert input to lowercase to avoid issues with capitalization
        transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

        // Validate filter input
        while (std::cin.fail() || 
            (filter != "blurred" && filter != "grayscaled" && filter != "edges" && filter != "reflected" && filter != "quit"))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid filter. Please enter Blurred, Grayscaled, Edges, or Reflected: ";
            std::cin >> filter;
            transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
        }

        BITMAPFILEHEADER bmpHeader;
        BITMAPINFOHEADER imageInfo;
        std::vector<std::vector<RGBTRIPLE>> image;
        std::vector<std::vector<RGBTRIPLE>> filteredImage;
            
        // Check if the file is a .BMP, if true: open a new file and copy the info
        input.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
        std::ofstream output;
        if (bmpHeader.bfType == 0x4D42)
        {
            output.open(filter + "_" + inPic, std::ios::binary); // Open output file
            output.write(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
        }
        else
        {
            std::cout << "This file is not a bmp, please restart the program and try again.";
            return 2;
        }

        // Writing the image info to the new file
        input.read(reinterpret_cast<char*>(&imageInfo), sizeof(imageInfo));
        output.write(reinterpret_cast<char*>(&imageInfo), sizeof(imageInfo));

        // Check if there's a color table and copy it
        int headerSize = 14 + imageInfo.biSize; 
        int extraBytes = bmpHeader.bfOffBits - headerSize;

        if (extraBytes > 0)
        {
        std::vector<char> buffer(extraBytes);
        input.read(buffer.data(), extraBytes);
        output.write(buffer.data(), extraBytes);
        }
        
        // Handle negative height (top-down BMPs)
        int actualHeight = abs(imageInfo.biHeight);
        int actualWidth = abs(imageInfo.biWidth);
            
        // Calculate padding
        int padding = (4 - (actualWidth * sizeof(RGBTRIPLE)) % 4) % 4;

        // Resize the image vector to hold all pixels
        image.resize(actualHeight);
        for (int i = 0; i < actualHeight; i++) 
        {
            image[i].resize(actualWidth);
        }

        // Read pixel data (BMP stores pixels bottom-to-top)
        for (int i = actualHeight - 1; i >= 0; i--)
        {
            // Read one row of pixels
            input.read(reinterpret_cast<char*>(image[i].data()), actualWidth * sizeof(RGBTRIPLE));
            // Skip padding bytes
            input.ignore(padding);
        }
        // Apply filters depending on choice
        switch (filter[0])
        { 
            case 'b':
                filteredImage = ImageFilters::blur(actualHeight, actualWidth, image);

                break;

            case 'g':
                filteredImage = ImageFilters::grayscale(actualHeight, actualWidth, image);              
                break;

            case 'r':
                filteredImage = ImageFilters::reflect(actualHeight, actualWidth, image);
                break;

            case 'e':
                filteredImage = ImageFilters::edges(actualHeight, actualWidth, image);
                break;
        }
        makeFile(output, filteredImage, actualWidth, actualHeight);
        input.close();
        output.close();
    }
}