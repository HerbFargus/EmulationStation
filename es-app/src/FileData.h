#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "MetaData.h"

class SystemData;

enum FileType
{
	GAME = 1,   // Cannot have children.
	FOLDER = 2,
	PLACEHOLDER = 3
};

enum FileChangeType
{
	FILE_ADDED,
	FILE_METADATA_CHANGED,
	FILE_REMOVED,
	FILE_SORTED
};

// Used for loading/saving gamelist.xml.
const char* fileTypeToString(FileType type);
FileType stringToFileType(const char* str);

// Remove (.*) and [.*] from str
std::string removeParenthesis(const std::string& str);

// A tree node that holds information for a file.
class FileData
{
public:
	FileData(FileType type, const boost::filesystem::path& path, SystemData* system);
	virtual ~FileData();

	inline const std::string& getName() const { return metadata.get("name"); }
	inline FileType getType() const { return mType; }
	inline const boost::filesystem::path& getPath() const { return mPath; }
	inline FileData* getParent() const { return mParent; }
	inline const std::unordered_map<std::string, FileData*>& getChildrenByFilename() const { return mChildrenByFilename; }
	inline const std::vector<FileData*>& getChildren() const { return mChildren; }
	inline SystemData* getSystem() const { return mSystem; }
	
	virtual const std::string& getThumbnailPath() const;
	virtual const std::string& getVideoPath() const;
	virtual const std::string& getMarqueePath() const;

	const std::vector<FileData*>& getChildrenListToDisplay();
	std::vector<FileData*> getFilesRecursive(unsigned int typeMask, bool displayedOnly = false) const;	

	void addChild(FileData* file); // Error if mType != FOLDER
	void removeChild(FileData* file); //Error if mType != FOLDER

	inline bool isPlaceHolder() { return mType == PLACEHOLDER; };

	// Returns our best guess at the "real" name for this file (will attempt to perform MAME name translation)
	std::string getDisplayName() const;

	// As above, but also remove parenthesis
	std::string getCleanName() const;

	typedef bool ComparisonFunction(const FileData* a, const FileData* b);
	struct SortType
	{
		ComparisonFunction* comparisonFunction;
		bool ascending;
		std::string description;

		SortType(ComparisonFunction* sortFunction, bool sortAscending, const std::string & sortDescription) 
			: comparisonFunction(sortFunction), ascending(sortAscending), description(sortDescription) {}
	};

	void sort(ComparisonFunction& comparator, bool ascending = true);
	void sort(const SortType& type);

	MetaDataList metadata;

private:
	FileType mType;
	boost::filesystem::path mPath;
	SystemData* mSystem;
	FileData* mParent;
	std::unordered_map<std::string,FileData*> mChildrenByFilename;
	std::vector<FileData*> mChildren;
	std::vector<FileData*> mFilteredChildren;
};
