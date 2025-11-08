#pragma once

#include <string>
#include <filesystem>

class FileType
{
protected:
	std::string folderName;
	std::vector<std::string> fileExtensions;
	std::filesystem::path folderPath;
	std::vector<std::string> fileNames;
	std::vector<std::filesystem::path> filePaths;


public:
	FileType(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
		folderName(folderName),
		fileExtensions(fileExtensions),
		folderPath(basePath / folderName)
	{
		IndexFiles();
	}

	void IndexFiles()
	{
		fileNames.clear();
		filePaths.clear();

		if (!std::filesystem::exists(folderPath))
			std::filesystem::create_directory(folderPath);

		struct stat fstat;

		if (stat(folderPath.string().c_str(), &fstat) == 0)
		{
			for (const auto& entry : std::filesystem::directory_iterator(folderPath))
			{
				auto extension = entry.path().filename().extension();

				for (const auto& fileExtension : fileExtensions)
				{
					if (fileExtension == extension)
					{
						filePaths.push_back(entry.path());

						break;
					}
				}
			}
		}

		std::sort(filePaths.begin(), filePaths.end());

		for (const auto& entry : filePaths)
		{
			auto filename = entry.filename().replace_extension();

			fileNames.push_back(filename.string());
		}
	}

	const std::string& GetFolderName() const
	{
		return folderName;
	}

	const std::vector<std::string>& GetFileNames() const
	{
		return fileNames;
	}

	const std::vector<std::filesystem::path>& GetFilePaths() const
	{
		return filePaths;
	}
};

template<typename T>
class FileLoader
{
private:
	const FileType& fileType;
	int loadedIndex;
	T* loadedData;
	T* stagedData;
	T* toDeleteData;
	bool haveStagedData;

protected:
	virtual T* LoadFromFile(const std::filesystem::path& loadPath)
	{
		(void)loadPath;

		return nullptr;
	}

public:
	FileLoader(const FileType& fileType) :
		fileType(fileType),
		loadedIndex(-1),
		loadedData(nullptr),
		stagedData(nullptr),
		toDeleteData(nullptr),
		haveStagedData(false)
	{

	}

	~FileLoader()
	{
		if (loadedData != nullptr)
			delete loadedData;

		if (stagedData != nullptr)
			delete stagedData;

		if (toDeleteData != nullptr)
			delete toDeleteData;
	}

	const FileType& GetFileType()
	{
		return fileType;
	}

	void LoadIndex(int index)
	{
		if (index != loadedIndex)
		{
			loadedIndex = index;

			if (index == -1)
			{
				stagedData = nullptr;
			}
			else
			{
				if (toDeleteData != nullptr)
				{
					delete toDeleteData;
					toDeleteData = nullptr;
				}

				stagedData = LoadFromFile(fileType.GetFilePaths()[index]);
			}

			haveStagedData = true;
		}
	}

	T* GetCurrentData()
	{
		if (haveStagedData)
		{
			toDeleteData = loadedData;
			loadedData = stagedData;
			stagedData = nullptr;

			haveStagedData = false;
		}

		return loadedData;
	}
};
