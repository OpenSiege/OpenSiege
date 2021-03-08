
// ================================================================================================
// -*- C++ -*-
// File: tank_file_reader.cpp
// Author: Guilherme R. Lampert, OpenSiege
// Created on: 23/07/14
// Brief: TankFile::Reader inner class implementation.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "TankFile.hpp"
#include "miniz.h"
#include <algorithm>
#include <cassert>

namespace ehb
{
	static std::string removeTrailingFloatZeros(const std::string& floatStr)
	{
		// Only process if the number is decimal (has a dot somewhere):
		if (floatStr.find_last_of('.') == std::string::npos)
		{
			return floatStr;
		}

		std::string trimmed(floatStr);

		// Remove trailing zeros:
		while (!trimmed.empty() && (trimmed.back() == '0'))
		{
			trimmed.pop_back();
		}

		// If the dot was left alone at the end, remove it too:
		if (!trimmed.empty() && (trimmed.back() == '.'))
		{
			trimmed.pop_back();
		}

		return trimmed;
	}
	static std::string formatMemoryUnit(uint64_t memorySizeInBytes, bool abbreviated = false)
	{
		const char* memUnitStr;
		double adjustedSize;
		char numStrBuf[128];

		if (memorySizeInBytes < 1024)
		{
			memUnitStr = (abbreviated ? "B" : "Bytes");
			adjustedSize = static_cast<double>(memorySizeInBytes);
		}
		else if (memorySizeInBytes < (1024 * 1024))
		{
			memUnitStr = (abbreviated ? "KB" : "Kilobytes");
			adjustedSize = (memorySizeInBytes / 1024.0);
		}
		else if (memorySizeInBytes < (1024 * 1024 * 1024))
		{
			memUnitStr = (abbreviated ? "MB" : "Megabytes");
			adjustedSize = (memorySizeInBytes / 1024.0 / 1024.0);
		}
		else
		{
			memUnitStr = (abbreviated ? "GB" : "Gigabytes");
			adjustedSize = (memorySizeInBytes / 1024.0 / 1024.0 / 1024.0);
		}

		// We only care about the first 2 decimal digits.
		std::snprintf(numStrBuf, sizeof(numStrBuf), "%.2f", adjustedSize);

		// Remove trailing zeros if no significant decimal digits:
		return removeTrailingFloatZeros(numStrBuf) + std::string(" ") + memUnitStr;
	}

	static uint32_t computeCrc32(const void* data, size_t sizeBytes) noexcept
	{
		assert(data != nullptr);
		assert(sizeBytes != 0);

		//
		// This compact CRC 32 algo was adapted from miniz.c, which in turn was taken from
		// "A compact CCITT crc16 and crc32 C implementation that balances processor cache usage against speed"
		// By Karl Malbrain.
		//
		static const uint32_t crcTable[16] =
		{
			0,
			0x1DB71064,
			0x3B6E20C8,
			0x26D930AC,
			0x76DC4190,
			0x6B6B51F4,
			0x4DB26158,
			0x5005713C,
			0xEDB88320,
			0xF00F9344,
			0xD6D6A3E8,
			0xCB61B38C,
			0x9B64C2B0,
			0x86D3D2D4,
			0xA00AE278,
			0xBDBDF21C
		};

		const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
		uint32_t crcu32 = 0;

		crcu32 = ~crcu32;
		while (sizeBytes--)
		{
			uint8_t b = *ptr++;
			crcu32 = (crcu32 >> 4) ^ crcTable[(crcu32 & 0xF) ^ (b & 0xF)];
			crcu32 = (crcu32 >> 4) ^ crcTable[(crcu32 & 0xF) ^ (b >> 4)];
		}
		return ~crcu32;
	}

// ========================================================
// TankFile::Reader:
// ========================================================

TankFile::Reader::Reader(TankFile & tank)
{
	indexFile(tank);
}

void TankFile::Reader::indexFile(TankFile & tank)
{
	if (log == nullptr)
	{
		log = spdlog::get("filesystem");
	}

	if (!tank.isOpen())
	{
		log->critical("Tank file {} is not open!", tank.getFileName());
		return;
	}

	if (!tank.isReadOnly())
	{
		log->critical("Tank file {} must be opened before TankFile::Reader can index it!", tank.getFileName());
		return;
	}

	log->debug("Preparing to index Tank file {}...", tank.getFileName());

	// Discard current metadata, if any, before loading new.
	dirSet  = nullptr;
	fileSet = nullptr;
	fileTable.clear();

	readDirSet(tank);
	readFileSet(tank);
}

void TankFile::Reader::readDirSet(TankFile & tank)
{
	const auto fileSize = tank.getFileSizeBytes();
	const auto & header = tank.getFileHeader();

	tank.seekAbsoluteOffset(header.dirsetOffset); // Seek DirSet position.

	const auto numDirectories = tank.readU32();
	dirSet.reset(new DirSet(numDirectories));

	log->debug("====== readDirSet() ======");
	log->debug("numDirectories = {}", numDirectories);
	log->debug("-");

	// Scan dir offset list:
	for (uint32_t d = 0; d < numDirectories; ++d)
	{
		const auto dirOffs = tank.readU32();
		if (dirOffs == TankFile::InvalidOffset || (header.dirsetOffset + dirOffs) > fileSize)
		{
			//SiegeThrow(TankFile::Error, "Invalid directory offset: " << dirOffs);
			return;
		}
		dirSet->dirOffsets.push_back(dirOffs);
	}

	// Scan list of DirEntries:
	std::vector<uint32_t> childOffsets;
	std::string dirEntryName;
	for (uint32_t d = 0; d < numDirectories; ++d)
	{
		const auto dirOffs = dirSet->dirOffsets[d];
		tank.seekAbsoluteOffset(header.dirsetOffset + dirOffs);

		const auto dirParentOffset = tank.readU32();
		const auto dirChildCount   = tank.readU32();
		const auto dirFileTime     = tank.readFileTime();
		dirEntryName               = tank.readNString();

		// Validate parent offset:
		if (dirParentOffset == TankFile::InvalidOffset || (header.dirsetOffset + dirParentOffset) > fileSize)
		{
			// SiegeThrow(TankFile::Error, "Invalid directory parent offset: " << dirParentOffset);
			return;
		}

		// Check for directory root and give it the proper root name if so:
		if (dirParentOffset == 0 && dirEntryName.empty())
		{
			// This must be the root directory, which is a dummy entry.
			dirEntryName = "/";
		}

		// Scan list of offsets to the children of this directory (files and other dirs):
		childOffsets.reserve(dirChildCount);
		for (uint32_t c = 0; c < dirChildCount; ++c)
		{
			const auto childOffs = tank.readU32();
			if (childOffs == TankFile::InvalidOffset || (header.dirsetOffset + childOffs) > fileSize)
			{
				// SiegeThrow(TankFile::Error, "Invalid directory child offset: " << childOffs);
				return;
			}
			childOffsets.push_back(childOffs);
		}

		log->debug("dirEntry.parentOffset..: {}", dirParentOffset);
		log->debug("dirEntry.myOffset......: {}", dirOffs);
		log->debug("dirEntry.childCount....: {}", dirChildCount);
		log->debug("dirEntry.fileTime......: {}", dirFileTime);
		log->debug("dirEntry.childOffsets..: {}", childOffsets.size());
		log->debug("dirEntry.name..........: {}", dirEntryName);
		log->debug("-");

		dirSet->dirEntries.emplace_back(dirParentOffset, dirChildCount, dirFileTime, dirEntryName, std::move(childOffsets));
		assert(childOffsets.empty());
	}

	// Fill 'fileTable' with the full directory paths:
	buildDirPaths();
}

void TankFile::Reader::readFileSet(TankFile & tank)
{
	const auto fileSize = tank.getFileSizeBytes();
	const auto & header = tank.getFileHeader();

	tank.seekAbsoluteOffset(header.filesetOffset); // Seek FileSet position.

	const auto numFiles = tank.readU32();
	fileSet.reset(new FileSet(numFiles));

	log->debug("====== readFileSet() ======");
	log->debug("numFiles = {}", numFiles);
	log->debug("-");

	// Scan file offset list:
	for (uint32_t f = 0; f < numFiles; ++f)
	{
		const auto fileOffs = tank.readU32();
		if (fileOffs == TankFile::InvalidOffset || (header.filesetOffset + fileOffs) > fileSize)
		{
			// SiegeThrow(TankFile::Error, "Invalid file offset: " << fileOffs);
			return;
		}
		fileSet->fileOffsets.push_back(fileOffs);
	}

	// Scan list of FileEntries:
	std::string fileEntryName;
	for (uint32_t f = 0; f < numFiles; ++f)
	{
		const auto fileOffs = fileSet->fileOffsets[f];
		tank.seekAbsoluteOffset(header.filesetOffset + fileOffs);

		const auto fileParentOffset = tank.readU32();
		const auto fileEntrySize    = tank.readU32();
		const auto fileDataOffset   = tank.readU32();
		const auto fileCrc32        = tank.readU32();
		const auto fileTime         = tank.readFileTime();
		const auto fileFormat       = tank.readU16();
		const auto fileFlags        = tank.readU16();
		fileEntryName               = tank.readNString();

		// Validate parent offset:
		if (fileParentOffset == TankFile::InvalidOffset ||
		   (header.filesetOffset + fileParentOffset) > fileSize)
		{
			// SiegeThrow(TankFile::Error, "Invalid file parent offset: " << fileParentOffset);
			return;
		}

		const auto fileDataFormat = static_cast<TankFile::DataFormat>(fileFormat);

		log->debug("fileEntry.parentOffset..: {}", fileParentOffset);
		log->debug("fileEntry.myOffset......: {}", fileOffs);
		log->debug("fileEntry.size..........: {}", formatMemoryUnit(fileEntrySize));
		log->debug("fileEntry.offset........: {}", fileDataOffset);
		log->debug("fileEntry.crc32.........: 0x{:x}", fileCrc32);
		log->debug("fileEntry.fileTime......: {}", fileTime);
		log->debug("fileEntry.format........: {}", TankFile::dataFormatToString(fileDataFormat));
		log->debug("fileEntry.flags.........: {}", fileFlags);
		log->debug("fileEntry.name..........: {}", fileEntryName);
		log->debug("fileEntry.isCompressed..: {}", (TankFile::isDataFormatCompressed(fileDataFormat) ? "yes" : "no"));
		log->debug("-");

		fileSet->fileEntries.emplace_back(fileParentOffset, fileEntrySize, fileDataOffset,
				fileCrc32, fileTime, fileDataFormat, fileFlags, fileEntryName);

		// We need to grab the compressed header for the compressed file entries.
		if (TankFile::isDataFormatCompressed(fileDataFormat) && fileEntrySize != 0)
		{
			const auto compressedSize = tank.readU32();
			const auto chunkSize      = tank.readU32();

			assert(compressedSize < fileSize);
			FileEntry & fileEntry = fileSet->fileEntries.back();

			fileEntry.setCompressedHeader(std::unique_ptr<CompressedFileEntryHeader>(
					new CompressedFileEntryHeader(compressedSize, chunkSize, fileEntrySize)));

			// Might have to read in some chunk headers:
			const auto numChunks = fileEntry.getCompressedHeader().numChunks;
			for (uint32_t c = 0; c < numChunks; ++c)
			{
				const auto uncompressedBytes = tank.readU32();
				const auto compressedBytes   = tank.readU32();
				const auto extraBytes        = tank.readU32();
				const auto offset            = tank.readU32();

				// Add to compressedHeader:
				fileEntry.getCompressedHeader().chunkHeaders.emplace_back(
						uncompressedBytes, compressedBytes, extraBytes, offset);
			}
		}
	}

	// Add full file paths to 'fileTable':
	buildFilePaths();
}

// ========================================================
// Local helpers:
// ========================================================

namespace
{

void buildPathRecursive(const size_t entryIndex, const TankFile::DirSet & dirSet, std::string & path)
{
	if (dirSet.dirEntries[entryIndex].parentOffset == 0)
	{
		return; // Is the root directory.
	}

	const auto parentIter = std::find(std::begin(dirSet.dirOffsets),
			std::end(dirSet.dirOffsets), dirSet.dirEntries[entryIndex].parentOffset);

	if (parentIter == std::end(dirSet.dirOffsets))
	{
		spdlog::get("filesystem")->critical("Found an orphan directory entry! '{}'", dirSet.dirEntries[entryIndex].name);
	}

	const auto parentIndex = std::distance(std::begin(dirSet.dirOffsets), parentIter);

	buildPathRecursive(parentIndex, dirSet, path);
	path += "/" + dirSet.dirEntries[entryIndex].name;
}

} // namespace {}

// ========================================================

void TankFile::Reader::buildDirPaths()
{
	log->debug("Building master directory table...");

	std::string fullPath;
	for (uint32_t d = 0; d < dirSet->numDirs; ++d)
	{
		fullPath.clear();
		buildPathRecursive(d, *dirSet, fullPath);
		fullPath += "/";

		fileTable.emplace(fullPath, TankEntry(&dirSet->dirEntries[d]));

		log->debug("Dir: {}", fullPath.c_str());
	}
}

void TankFile::Reader::buildFilePaths()
{
	log->debug("Building master file table...");

	std::string fullPath;
	for (uint32_t f = 0; f < fileSet->numFiles; ++f)
	{
		fullPath.clear();

		// We don't need to do all that work if the file is at the root.
		if (fileSet->fileEntries[f].parentOffset != 0)
		{
			const auto parentIter = std::find(std::begin(dirSet->dirOffsets),
					std::end(dirSet->dirOffsets), fileSet->fileEntries[f].parentOffset);

			if (parentIter == std::end(dirSet->dirOffsets))
			{
				log->critical("Found an orphan file entry '{}' (parentOffset = {})", fileSet->fileEntries[f].name, fileSet->fileEntries[f].parentOffset);
				return;
			}

			const auto parentIndex = std::distance(std::begin(dirSet->dirOffsets), parentIter);
			buildPathRecursive(parentIndex, *dirSet, fullPath);
		}

		fullPath += "/" + fileSet->fileEntries[f].name;
		fileTable.emplace(fullPath, TankEntry(&fileSet->fileEntries[f]));

		log->debug("File: {}", fullPath.c_str());
	}
}

ByteArray TankFile::Reader::extractResourceToMemory(TankFile & tank, const std::string & resourcePath, const bool validateCRCs) const
{
	if (!tank.isOpen())
	{
		log->critical("Tank file {} is not open!", tank.getFileName());

		return {};
	}

	if (!tank.isReadOnly())
	{
		log->critical("Tank file {} must be opened for reading before you can extract data from it!", tank.getFileName());
		return {};
	}

	const auto it = fileTable.find(resourcePath);
	if (it == std::end(fileTable))
	{
		// the below is commented out because currently OpenSiege loops through tanks to find what it needs and this floods the log
		// log->critical("Resource {} not found in Tank file {}", resourcePath, tank.getFileName());
		return {};
	}

	assert(it->first == resourcePath);
	const Reader::TankEntry & entry = it->second;

	if (entry.type != TankEntry::TypeFile)
	{
		log->critical("Resource {} in Tank file is a directory and cannot be decompressed to file!", resourcePath);
		return {};
	}

	assert(entry.ptr.file != nullptr);
	const TankFile::FileEntry & resFile = *(entry.ptr.file);

	if (resFile.isInvalidFile())
	{
		// NOTE: Not sure if this should be a hard error...
		log->warn("Resource file entry {} is flagged as invalid!", resFile.name);
	}

	const auto fileOffset  = resFile.offset;
	const auto fileSize    = resFile.size;
	const auto dataOffset  = tank.getFileHeader().dataOffset;
	ByteArray fileContents;

	if (!resFile.isCompressed()) // Simple raw resource file:
	{
		log->debug("Extracting UNCOMPRESSED Tank resource {}...", resourcePath);

		// 'devlogic.dsres' (one of our test Tanks from the game) has
		// a few empty uncompressed dummy files. This check handles those.
		if (fileSize != 0)
		{
			tank.seekAbsoluteOffset(dataOffset + fileOffset);
			fileContents.resize(fileSize);
			tank.readBytes(fileContents.data(), fileContents.size());
		}
	}
	else // LZO/Zlib compressed:
	{
		log->debug("Extracting COMPRESSED Tank resource {}\nUncompressed size: {}, compression fmt:{}", 
			resourcePath, formatMemoryUnit(fileSize, true), dataFormatToString(resFile.format));

		const auto & compressedHeader = resFile.getCompressedHeader();

		ByteArray compressedData;
		ByteArray uncompressedData;
		unsigned long uncompressedLen;

		fileContents.reserve(fileSize);

		for (uint32_t c = 0; c < compressedHeader.numChunks; ++c)
		{
			const TankFile::FileEntryChunkHeader & chunk = compressedHeader.chunkHeaders[c];

			// Individual chunks of data inside a compressed file might
			// be stored without compression. So this check is necessary.
			if (chunk.isCompressed())
			{
				tank.seekAbsoluteOffset(dataOffset + fileOffset + chunk.offset);
				compressedData.resize(chunk.compressedSize + chunk.extraBytes);
				tank.readBytes(compressedData.data(), compressedData.size());

				uncompressedData.resize(chunk.uncompressedSize + chunk.extraBytes);
				uncompressedLen = static_cast<unsigned long>(uncompressedData.size());

				log->debug("Attempting to decompress resource chunk #{} of {}...", (c + 1), compressedHeader.numChunks);

				const int errorCode = mz_uncompress(uncompressedData.data(), &uncompressedLen, 
							compressedData.data(), static_cast<unsigned long>(compressedData.size() - chunk.extraBytes));

				assert(uncompressedLen != 0 && "Nothing was decompressed!");
				assert(uncompressedLen <= uncompressedData.size() && "Buffer overrun!");

				if (errorCode != 0)
				{
					log->critical("Failed to decompress resource {}! Mini-Z error: {}", resourcePath, errorCode);
				}
			}
			else
			{
				log->debug("Chunk #{} of {} is stored without compression...", (c + 1), compressedHeader.numChunks);

				compressedData.clear();
				assert(chunk.uncompressedSize == chunk.compressedSize);

				tank.seekAbsoluteOffset(dataOffset + fileOffset + chunk.offset);
				uncompressedData.resize(chunk.uncompressedSize);
				tank.readBytes(uncompressedData.data(), uncompressedData.size());

				uncompressedLen = static_cast<unsigned long>(uncompressedData.size());
			}

			// Append decompressed chunk to the whole resource:
			//
			// extraBytes are not decompressed, they should be copied unchanged to the
			// end of the decompressed chunk. Refer to "gpg/TankStructure.h" for a nice
			// ASCII drawing of the process.
			//
			fileContents.insert(std::end(fileContents), std::begin(uncompressedData),
					std::begin(uncompressedData) + uncompressedLen);

			// Append extraBytes at the end of this chunk:
			// (compressedData is only empty for an uncompressed chunk)
			if (chunk.extraBytes != 0 && !compressedData.empty())
			{
				fileContents.insert(std::end(fileContents), std::begin(compressedData) + chunk.compressedSize,
						std::begin(compressedData) + chunk.compressedSize + chunk.extraBytes);
			}
		}
	}

	if (validateCRCs && !fileContents.empty())
	{
		// mini-z issue to work out later
		#undef crc32
		const auto expectedCrc = resFile.crc32;
		const auto contentsCrc = computeCrc32(fileContents.data(), fileContents.size());

		if (contentsCrc != expectedCrc)
		{
			log->critical("Tank resource {} CRC 0x{:x} does not match the expected (0x{:x})!", resourcePath, contentsCrc, expectedCrc);
		}
	}

	log->debug("Tank resource {} extracted without errors"", resourcePath");
	return fileContents;
}

std::vector<std::string> TankFile::Reader::getFileList() const
{
	std::vector<std::string> fileList;
	fileList.reserve(getFileCount());

	for (const auto & entry : fileTable)
	{
		if (entry.second.type == TankEntry::TypeDir)
		{
			continue;
		}
		fileList.push_back(entry.first);
	}

	return fileList;
}

std::vector<std::string> TankFile::Reader::getDirectoryList() const
{
	std::vector<std::string> dirList;
	dirList.reserve(getDirectoryCount());

	for (const auto & entry : fileTable)
	{
		if (entry.second.type == TankEntry::TypeFile)
		{
			continue;
		}
		dirList.push_back(entry.first);
	}

	return dirList;
}

#undef TankReaderLog

} // namespace ehb {}
