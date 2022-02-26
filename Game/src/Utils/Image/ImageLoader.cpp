#include "pch.h"
#include "ImageLoader.h"

#include "Core/Assert.h"
#include "stb_image.h"
#include "lodepng.h"

#include "Utils/Generic.h"

namespace Game
{
    namespace utils
    {

        ImageInformation GetImageInfo(const std::string& path)
        {
            const std::string ext = utils::ToLower(path.substr(path.find_last_of('.') + 1));
		
		    if (ext == "png")
		    {
			    return GetImageInfoLoadPng(path);
            }
		
		    return GetImageInfoStbImage(path);
        }
        
        ImageInformation GetImageInfoStbImage(const std::string& path)
        {
            //TODO: Fix this stbi 'no SOI' error
		    ImageInformation info;
		    stbi_set_flip_vertically_on_load(1);
		    auto px = stbi_load(path.c_str(), &info.Width, &info.Height, &info.Channels, 0);

		    const char* er = stbi_failure_reason();
		    if (er)
		    {
		    	GAME_CORE_ASSERT(false, "Failed to load image '{0}' | error: '{1}'",path,er);
		    	if(px)
		    		stbi_image_free(px);
		    	info.Buffer = nullptr;
		    }
		    info.Buffer = px;

            info.FilePath = path;
            info.FileName = ParseFileName(path);

		    return info;
        }
        
        ImageInformation GetImageInfoLoadPng(const std::string& path)
        {
            ImageInformation info;

		    //TODO: Flip vertically
		    unsigned w = 0, h = 0;
		    std::vector<unsigned char> image;
    
		    unsigned error = lodepng::decode(image, w, h, path);
		    if (error) //TODO: Deal properly with this error
		    	GAME_CORE_ASSERT(false,"Error loading image '{0}': {1}",path,lodepng_error_text(error));
    
		    info.Width = w;
		    info.Height = h;
		    info.Channels = 4;
		    auto size = image.size();
		    info.Buffer = CreateImageBuffer(image.size());
    
		    std::copy(image.begin(), image.end(), info.Buffer);
		    image.clear();

            info.FilePath = path;
            info.FileName = ParseFileName(path);
			info.png = true;
			
		    return info;
        }

        ImageBuffer_T* CreateImageBuffer(size_t size)
	    {
	    	GAME_CORE_ASSERT(size > 0,"Buffer size can't be 0");
	    	return (ImageBuffer_T*)malloc(size);
	    }

	    ImageBuffer_T* CreateImageBuffer(unsigned int w, unsigned int h, unsigned int channels)
	    {
	    	return CreateImageBuffer(static_cast<size_t>(w) * h * channels * sizeof(ImageBuffer_T));
	    }

        void DeleteImageBuffer(ImageBuffer_T* buffer)
        {
            if(buffer)
                free(buffer);
        }

    }
}
