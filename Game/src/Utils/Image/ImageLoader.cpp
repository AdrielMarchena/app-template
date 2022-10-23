#include "pch.h"
#include "ImageLoader.h"

#include "Debug/Intrumentator.h"
#include "Core/Assert.h"
#include "stb_image.h"
#include "lodepng.h"

#include "Utils/Generic.h"

#include <filesystem>

namespace Game
{

ImageInformation GetImageInfo(const std::string& path)
{
	const std::string ext = ToLower(path.substr(path.find_last_of('.') + 1));

	if (ext == "png")
	{
		return GetImageInfoLoadPng(path);
	}

	return GetImageInfoStbImage(path);
}

ImageInformation GetImageInfoStbImage(const std::string& path)
{
	GAME_PROFILE_FUNCTION();
	//TODO: Fix this stbi 'no SOI' error
	ImageInformation info;
	stbi_set_flip_vertically_on_load(1);
	auto px = stbi_load(path.c_str(), &info.Width, &info.Height, &info.Channels, 0);

	const char* er = stbi_failure_reason();
	if (er)
	{
		GAME_CORE_ASSERT(false, "Failed to load image '{0}' | error: '{1}'", path, er);
		if (px)
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
	GAME_PROFILE_FUNCTION();
	ImageInformation info;

	//TODO: Flip vertically
	unsigned w = 0, h = 0;
	std::vector<unsigned char> image;

	unsigned error = lodepng::decode(image, w, h, path);
	if (error) //TODO: Deal properly with this error
		GAME_CORE_ASSERT(false, "Error loading image '{0}': {1}", path, lodepng_error_text(error));

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

Awaiter<ImageInformation> LoadBatchAsync(const std::string& folder)
{
	return LoadBatchAsync(GetFilesFromFolder(folder));
}

Awaiter<ImageInformation> LoadBatchAsync(const std::vector<std::string>& paths)
{
	std::vector<RefFuture<ImageInformation>> futures;
	futures.reserve(paths.size());
	for (const auto& path : paths)
		futures.emplace_back(LoadAsync(path));
	return Awaiter(futures);
}

RefFuture<ImageInformation> LoadAsync(const std::string& path)
{
	return ExecuteAsyncRef(GetImageInfo, path);
}

std::unordered_map<std::string, ImageInformation> LoadBatch(const std::string& folder)
{
	using std::filesystem::directory_iterator;
	std::vector<std::string> paths;
	for (const auto& file : directory_iterator(folder))
		paths.push_back(file.path().string());
	return LoadBatch(paths);
}

std::unordered_map<std::string, ImageInformation> LoadBatch(const std::vector<std::string>& paths)
{
	using std::filesystem::directory_iterator;
	std::unordered_map<std::string, ImageInformation> images;
	for (const auto& path : paths)
	{
		const std::string fileName = ParseFileName(path);
		images[fileName] = GetImageInfo(path);
	}
	return images;
}

ImageBuffer_T* CreateImageBuffer(size_t size)
{
	GAME_CORE_ASSERT(size > 0, "Buffer size can't be 0");
	return (ImageBuffer_T*)malloc(size);
}

ImageBuffer_T* CreateImageBuffer(unsigned int w, unsigned int h, unsigned int channels)
{
	return CreateImageBuffer(static_cast<size_t>(w) * h * channels * sizeof(ImageBuffer_T));
}

void DeleteImageBuffer(ImageBuffer_T* buffer)
{
	if (buffer)
		free(buffer);
}
}
