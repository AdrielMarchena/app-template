#pragma once

#include <string>

namespace Game
{
    namespace utils
    {

        typedef unsigned char ImageBuffer_T;
        struct ImageInformation
        {
            int32_t Width = 0;
            int32_t Height = 0;
            int32_t Channels = 0;       
            std::string FileName = "";
            std::string FilePath = "";      
            ImageBuffer_T* Buffer = nullptr;   
            bool png = false;

            operator bool() const 
            { 
                return (!Width && !Height && !Channels && FilePath.empty() && FileName.empty());  
            }
        };

        /* The memory is unmanaged */
        ImageInformation GetImageInfo(const std::string& path);
        /* The memory is unmanaged */
        ImageInformation GetImageInfoStbImage(const std::string& path);
        /* The memory is unmanaged */
        ImageInformation GetImageInfoLoadPng(const std::string& path);

        ImageBuffer_T* CreateImageBuffer(size_t size);
        ImageBuffer_T* CreateImageBuffer(unsigned int w, unsigned int h, unsigned int channels);
        void DeleteImageBuffer(ImageBuffer_T* buffer);

    }
}