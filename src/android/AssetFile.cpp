#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include "AssetFile.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

AAssetManager* AssetFile::assetManager_ = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AssetFile::AssetFile(const char *filename)
	: IFile(filename), asset_(NULL), startOffset_(0L)
{
	type_ = ASSET_TYPE;

	// Detect fake second extension added to prevent compression
	int firstDotChar = filename_.findFirstChar('.');
	int lastDotChar = filename_.findLastChar('.');
	int extLength = lastDotChar - firstDotChar - 1;
	if (extLength >= 3 && extLength <= 4)
	{
		extension_.copyFrom(filename_, firstDotChar + 1, extLength);
	}
}

AssetFile::~AssetFile()
{
	if (shouldCloseOnExit_)
	{
		close();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void AssetFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0 || asset_ != NULL)
	{
		LOGW_X("File \"%s\" is already opened", filename_.data());
	}
	else
	{
		// Opening with a file descriptor
		if (mode & MODE_FD)
		{
			openFD(mode);
		}
		// Opening as an asset only
		else
		{
			openAsset(mode);
		}
	}
}


/// Closes the file (both opened or fopened)
void AssetFile::close()
{
	if (fileDescriptor_ >= 0)
	{
		int retValue = ::close(fileDescriptor_);
		if (retValue < 0)
		{
			LOGW_X("Cannot close the file \"%s\"", filename_.data());
		}
		else
		{
			LOGI_X("File \"%s\" closed", filename_.data());
			fileDescriptor_ = -1;
		}
	}
	else if (asset_)
	{
		AAsset_close(asset_);
		asset_ = NULL;
		LOGI_X("File \"%s\" closed", filename_.data());
	}
}

long int AssetFile::seek(long int offset, int whence) const
{
	long int seekValue = -1;

	if (fileDescriptor_ >= 0)
	{
		switch (whence)
		{
			case SEEK_SET:
				seekValue = lseek(fileDescriptor_, startOffset_ + offset, SEEK_SET);
				break;
			case SEEK_CUR:
				seekValue = lseek(fileDescriptor_, offset, SEEK_CUR);
				break;
			case SEEK_END:
				seekValue = lseek(fileDescriptor_, startOffset_ + fileSize_ + offset, SEEK_END);
				break;
		}
		seekValue -= startOffset_;
	}
	else if (asset_)
	{
		seekValue = AAsset_seek(asset_, offset, whence);
	}

	return seekValue;
}

long int AssetFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
	{
		tellValue = lseek(fileDescriptor_, 0L, SEEK_CUR) - startOffset_;
	}
	else if (asset_)
	{
		tellValue = AAsset_seek(asset_, 0L, SEEK_CUR);
	}

	return tellValue;
}


long int AssetFile::read(void *buffer, int bytes) const
{
	long int bytesRead = -1;

	if (fileDescriptor_ >= 0)
	{
		int bytesToRead = bytes;

		long int seekValue = lseek(fileDescriptor_, 0L, SEEK_CUR);

		if (seekValue >=  startOffset_ + fileSize_)
		{
			bytesToRead = 0;    // simulating EOF
		}
		else if (seekValue + bytes > startOffset_ + fileSize_)
		{
			bytesToRead = (startOffset_ + fileSize_) - seekValue;
		}

		bytesRead = ::read(fileDescriptor_, buffer, bytesToRead);
	}
	else if (asset_)
	{
		bytesRead = AAsset_read(asset_, buffer, bytes);
	}

	return bytesRead;
}

bool AssetFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || asset_ != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with AAsset_openFileDescriptor()
void AssetFile::openFD(unsigned char mode)
{
	// An asset file can only be read
	if (mode == (MODE_FD | MODE_READ))
	{
		asset_ = AAssetManager_open(assetManager_, filename_.data(), AASSET_MODE_UNKNOWN);
		if (asset_ == NULL)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_.data());
			exit(-1);
		}

		fileDescriptor_ = AAsset_openFileDescriptor(asset_, &startOffset_, &fileSize_);
		lseek(fileDescriptor_, startOffset_, SEEK_SET);
		AAsset_close(asset_);
		asset_ = NULL;

		if (fileDescriptor_ < 0)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_.data());
			exit(-1);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_.data());
		}
	}
	else
	{
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
	}
}

/// Opens the file with AAssetManager_open() only
void AssetFile::openAsset(unsigned char mode)
{
	// An asset file can only be read
	if (mode == MODE_READ || mode == (MODE_READ | MODE_BINARY))
	{
		asset_ = AAssetManager_open(assetManager_, filename_.data(), AASSET_MODE_UNKNOWN);
		if (asset_ == NULL)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_.data());
			exit(-1);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_.data());
		}

		// Calculating file size
		fileSize_ = AAsset_getLength(asset_);
	}
	else
	{
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
	}
}

/// Checks if a file can be accessed with specified mode
/*! It is called by IFile::access() */
bool AssetFile::access(const char *filename, unsigned char mode)
{
	bool isAccessible = false;

	if (mode == IFile::MODE_EXISTS || mode == IFile::MODE_CAN_READ)
	{
		AAsset *asset = AAssetManager_open(assetManager_, filename, AASSET_MODE_UNKNOWN);
		if (asset)
		{
			isAccessible = true;
			AAsset_close(asset);
		}
	}
	else if (mode & MODE_CAN_WRITE)
	{
		LOGE_X("Cannot access the file \"%s\", an asset can only be read", filename);
	}
	else
	{
		LOGE_X("Cannot access the file \"%s\", wrong access mode", filename);
	}

	return isAccessible;
}

}