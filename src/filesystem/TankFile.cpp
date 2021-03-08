
// ================================================================================================
// -*- C++ -*-
// File: tank_file.cpp
// Author: Guilherme R. Lampert, OpenSiege
// Created on: 22/07/14
// Brief: Contains the binary structure and reader for Dungeon Siege / GPG "Tank" files.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "TankFile.hpp"
#include <cmath> // For std::ceil()
#include <filesystem>
#include <cassert>

namespace ehb
{

// ========================================================
// Local helpers:
// ========================================================

inline uint16_t alignToDword(const uint16_t size) noexcept
{
	const uint16_t offset = 4 - (size % 4);
	return size + offset;
}

// ========================================================
// TankFile::Header:
// ========================================================

TankFile::Header::Header()
{
	setDefaults();
}

void TankFile::Header::setDefaults()
{
	productId      = {};
	tankId         = {};
	headerVersion  = 0;
	dirsetOffset   = 0;
	filesetOffset  = 0;
	indexSize      = 0;
	dataOffset     = 0;
	productVersion = {};
	minimumVersion = {};
	priority       = TankFile::Priority::Factory;
	flags          = 0;
	creatorId      = {};
	guid           = {};
	indexCrc32     = 0;
	dataCrc32      = 0;
	utcBuildTime   = {};

	clearArray(copyrightText);
	clearArray(buildText);
	clearArray(titleText);
	clearArray(authorText);

	descriptionText.clear();
}

// ========================================================
// TankFile::FileEntryChunkHeader:
// ========================================================

TankFile::FileEntryChunkHeader::FileEntryChunkHeader(const uint32_t nUncompressedSize,
                                                     const uint32_t nCompressedSize,
                                                     const uint32_t nExtraBytes,
                                                     const uint32_t nOffset)
	: uncompressedSize(nUncompressedSize)
	, compressedSize(nCompressedSize)
	, extraBytes(nExtraBytes)
	, offset(nOffset)
{
}

bool TankFile::FileEntryChunkHeader::isCompressed() const noexcept
{
	return uncompressedSize != compressedSize;
}

// ========================================================
// TankFile::CompressedFileEntryHeader:
// ========================================================

TankFile::CompressedFileEntryHeader::CompressedFileEntryHeader(const uint32_t nCompressedSize,
                                                               const uint32_t nChunkSize,
                                                               const uint32_t nFileSize)
	: compressedSize(nCompressedSize)
	, chunkSize(nChunkSize)
	, numChunks((chunkSize != 0 && nFileSize != 0) ?
	            uint32_t(std::ceil(double(nFileSize) / double(chunkSize))) : 0)
{
	if (numChunks != 0)
	{
		chunkHeaders.reserve(numChunks);
	}

	constexpr uint32_t Win32PageSize = 4;
	if ((chunkSize % Win32PageSize) != 0)
	{
		spdlog::get("filesystem")->warn("Compressed chunk size is not rounded to the size of a 4KB page!");
	}
}

// ========================================================
// TankFile::FileEntry:
// ========================================================

TankFile::FileEntry::FileEntry(const uint32_t nParentOffs, const uint32_t nSize,
                               const uint32_t nOffset, const uint32_t crc, const FileTime ft,
                               const DataFormat dataFormat, const uint16_t fileFlags, std::string filename)
	: parentOffset(nParentOffs)
	, size(nSize)
	, offset(nOffset)
	, crc32(crc)
	, fileTime(ft)
	, format(dataFormat)
	, flags(fileFlags)
	, name(std::move(filename))
{
	if (name.empty()) { spdlog::get("filesystem")->warn("Empty FileEntry name!"); }
}

void TankFile::FileEntry::setCompressedHeader(std::unique_ptr<TankFile::CompressedFileEntryHeader> header)
{
	assert(header != nullptr);
	compressedHeader = std::move(header);
}

TankFile::CompressedFileEntryHeader & TankFile::FileEntry::getCompressedHeader()
{
	assert(compressedHeader != nullptr);
	return *compressedHeader.get();
}

const TankFile::CompressedFileEntryHeader & TankFile::FileEntry::getCompressedHeader() const
{
	assert(compressedHeader != nullptr);
	return *compressedHeader.get();
}

const TankFile::FileEntryChunkHeader & TankFile::FileEntry::getChunkHeader(const uint32_t index) const
{
	assert(compressedHeader != nullptr);
	assert(compressedHeader->chunkSize != 0);
	assert(index <= (size / compressedHeader->chunkSize));
	return compressedHeader->chunkHeaders[index];
}

bool TankFile::FileEntry::isInvalidFile() const noexcept
{
	return !!(flags & FileFlagInvalid);
}

bool TankFile::FileEntry::isCompressed() const noexcept
{
	return isDataFormatCompressed(format);
}

uint32_t TankFile::FileEntry::getUncompressedSize() const
{
	return size;
}

uint32_t TankFile::FileEntry::getCompressedSize() const
{
	return isCompressed() ? getCompressedHeader().compressedSize : size;
}

uint32_t TankFile::FileEntry::getChunkSize() const
{
	return isCompressed() ? getCompressedHeader().chunkSize : 0;
}

uint32_t TankFile::FileEntry::getChunkIndex(const uint32_t offs) const
{
	assert(getChunkSize() != 0);
	return offs / getChunkSize();
}

// ========================================================
// TankFile::FileSet:
// ========================================================

TankFile::FileSet::FileSet(const uint32_t numEntries)
	: numFiles(numEntries)
{
	if (numFiles != 0)
	{
		fileOffsets.reserve(numFiles);
		fileEntries.reserve(numFiles);
	}
}

// ========================================================
// TankFile::DirEntry:
// ========================================================

TankFile::DirEntry::DirEntry(const uint32_t nParentOffs, const uint32_t nChildCount,
                             const FileTime ft, std::string dirName)
	: parentOffset(nParentOffs)
	, childCount(nChildCount)
	, fileTime(ft)
	, name(std::move(dirName))
{
	if (childCount != 0)
	{
		childOffsets.reserve(childCount);
	}

	if (name.empty()) { spdlog::get("filesystem")->warn("Empty DirEntry name!"); }
}

TankFile::DirEntry::DirEntry(const uint32_t nParentOffs, const uint32_t nChildCount, const FileTime ft,
                             std::string dirName, std::vector<uint32_t> && childOffs)
	: parentOffset(nParentOffs)
	, childCount(nChildCount)
	, fileTime(ft)
	, name(std::move(dirName))
	, childOffsets(std::forward<std::vector<uint32_t>>(childOffs))
{
	if (name.empty()) { spdlog::get("filesystem")->warn("Empty DirEntry name!"); }
}

// ========================================================
// TankFile::DirSet:
// ========================================================

TankFile::DirSet::DirSet(const uint32_t numEntries)
	: numDirs(numEntries)
{
	if (numDirs != 0)
	{
		dirOffsets.reserve(numDirs);
		dirEntries.reserve(numDirs);
	}
}

// ========================================================
// Static properties of TankFile:
// ========================================================

const FourCC TankFile::ProductId     = { 'D', 'S', 'i', 'g' }; // 'DSig'
const FourCC TankFile::TankId        = { 'T', 'a', 'n', 'k' }; // 'Tank'
const FourCC TankFile::CreatorIdGPG  = { '!', 'G', 'P', 'G' }; // '!GPG'
const FourCC TankFile::CreatorIdUser = { 'U', 'S', 'E', 'R' }; // 'USER'

// ========================================================
// Static methods of TankFile:
// ========================================================

std::string TankFile::priorityToString(const Priority priority)
{
	switch (priority)
	{
	case Priority::Factory   : return "Factory";
	case Priority::Language  : return "Language";
	case Priority::Expansion : return "Expansion";
	case Priority::Patch     : return "Patch";
	case Priority::User      : return "User";
	default: break;
	} // switch (priority)

	return "Unknown priority";
}

TankFile::Priority TankFile::priorityFromString(const std::string & str)
{
	if (str == "Factory")   return Priority::Factory;
	if (str == "Language")  return Priority::Language;
	if (str == "Expansion") return Priority::Expansion;
	if (str == "Patch")     return Priority::Patch;
	if (str == "User")      return Priority::User;
	else return Priority::Unk;
}

std::string TankFile::dataFormatToString(const DataFormat format)
{
	switch (format)
	{
	case DataFormat::Raw  : return "Raw";
	case DataFormat::Zlib : return "Zlib";
	case DataFormat::Lzo  : return "Lzo";
	} // switch (format)

	return "Unknown format";
}

TankFile::DataFormat TankFile::dataFormatFromString(const std::string & str)
{
	if (str == "Raw")  return DataFormat::Raw;
	if (str == "Zlib") return DataFormat::Zlib;
	if (str == "Lzo")  return DataFormat::Lzo;
	else return DataFormat::Unk;
}

// ========================================================
// TankFile instance methods:
// ========================================================

void TankFile::openForReading(std::string filename)
{
	log = spdlog::get("filesystem");

	if (filename.empty())
	{
		log->critical("No filename provided to TankFile for reading!");
		return;
	}

	if (isOpen())
	{
		log->error("File [{}] already open!", filename);
		return;
	}

	file.exceptions(0);
	file.open(filename, std::ios::binary | std::ios::in);

	fileName     = std::move(filename);
	fileOpenMode = std::ios::in | std::ios::binary;

	queryFileSize();
	readAndValidateHeader();

	log->debug("Successfully opened Tank file [{}] for reading. File size: [{}]", fileName, fileSizeBytes);
}

void TankFile::close()
{
	if (file.is_open())
	{
		file.close();
	}

	fileSizeBytes = 0;
	fileOpenMode  = 0;

	fileName.clear();
	fileHeader.setDefaults();
}

bool TankFile::isOpen() const noexcept
{
	return file.is_open();
}

bool TankFile::isReadOnly() const noexcept
{
	return (fileOpenMode & std::ios::in) &&
	      !(fileOpenMode & std::ios::out);
}

size_t TankFile::getFileSizeBytes() const noexcept
{
	return fileSizeBytes;
}

const TankFile::Header & TankFile::getFileHeader() const noexcept
{
	return fileHeader;
}

const std::string & TankFile::getFileName() const noexcept
{
	return fileName;
}

void TankFile::queryFileSize()
{
	assert(isOpen());

	fileSizeBytes = std::filesystem::file_size(fileName);
}

void TankFile::readAndValidateHeader()
{
	// Read in the header:
	fileHeader.productId      = readFourCC();
	fileHeader.tankId         = readFourCC();
	fileHeader.headerVersion  = readU32();
	fileHeader.dirsetOffset   = readU32();
	fileHeader.filesetOffset  = readU32();
	fileHeader.indexSize      = readU32();
	fileHeader.dataOffset     = readU32();
	fileHeader.productVersion = readProductVersion();
	fileHeader.minimumVersion = readProductVersion();
	fileHeader.priority       = static_cast<TankFile::Priority>(readU32());
	fileHeader.flags          = readU32();
	fileHeader.creatorId      = readFourCC();
	fileHeader.guid           = readGuid();
	fileHeader.indexCrc32     = readU32();
	fileHeader.dataCrc32      = readU32();
	fileHeader.utcBuildTime   = readSystemTime();
	readBytes(fileHeader.copyrightText, sizeof(fileHeader.copyrightText));
	readBytes(fileHeader.buildText,     sizeof(fileHeader.buildText));
	readBytes(fileHeader.titleText,     sizeof(fileHeader.titleText));
	readBytes(fileHeader.authorText,    sizeof(fileHeader.authorText));
	fileHeader.descriptionText = readWNString();

	// Optional debug printing:
	log->debug("====== TANK HEADER FOR FILE: {} ======", fileName);
	log->debug("productId.........: {}" , fileHeader.productId);
	log->debug("tankId............: {}" , fileHeader.tankId);
	log->debug("headerVersion.....: {}" , fileHeader.headerVersion);
	log->debug("dirsetOffset......: 0x{:x}, ({})", fileHeader.dirsetOffset, StringTool::formatMemoryUnit(fileHeader.dirsetOffset));
	log->debug("filesetOffset.....: 0x{:x}, ({})", fileHeader.filesetOffset, StringTool::formatMemoryUnit(fileHeader.filesetOffset));
	log->debug("indexSize.........: {}" , StringTool::formatMemoryUnit(fileHeader.indexSize));
	log->debug("dataOffset........: 0x{:x}, ({})", fileHeader.dataOffset, StringTool::formatMemoryUnit(fileHeader.dataOffset));
	log->debug("productVersion....: {}", fileHeader.productVersion);
	log->debug("minimumVersion....: {}", fileHeader.minimumVersion);
	log->debug("priority..........: {}", priorityToString(fileHeader.priority));
	log->debug("flags.............: {}", fileHeader.flags);
	log->debug("creatorId.........: {}", fileHeader.creatorId);
	log->debug("Guid..............: {}", fileHeader.guid);
	log->debug("indexCrc32........: 0x{:x}" , fileHeader.indexCrc32);
	log->debug("dataCrc32.........: 0x{:x}" , fileHeader.dataCrc32);
	log->debug("utcBuildTime......: {}", fileHeader.utcBuildTime);
	log->debug("copyrightText.....: {}", StringTool::toString(fileHeader.copyrightText));
	log->debug("buildText.........: {}", StringTool::toString(fileHeader.buildText));
	log->debug("titleText.........: {}", StringTool::toString(fileHeader.titleText));
	log->debug("authorText........: {}", StringTool::toString(fileHeader.authorText));
	log->debug("descriptionText...: {}", StringTool::toString(fileHeader.descriptionText));
	log->debug("====== END TANK HEADER ======");

	// Fatal errors:
	if (fileHeader.productId != TankFile::ProductId)
	{
		log->critical("[{}]: Header product id doesn't match the expected value!", fileName);
		return;
	}
	if (fileHeader.tankId != TankFile::TankId)
	{
		log->critical("[{}]: Header Tank id doesn't match the expected value!", fileName);
		return;
	}

	// Warnings:
	if (fileHeader.creatorId != TankFile::CreatorIdGPG &&
	    fileHeader.creatorId != TankFile::CreatorIdUser)
	{		
		log->warn("Tank creator id is unknown: [{}]", fileHeader.creatorId);
	}
	if (fileHeader.headerVersion != Header::ExpectedVersion)
	{
		log->warn("Unknown Tank header version: [{}]", fileHeader.headerVersion);
	}
}

void TankFile::seekAbsoluteOffset(const size_t offsetInBytes)
{
	assert(isOpen());
	// Seek absolute offset relative to the beginning of the file.
	if (!file.seekg(offsetInBytes, std::ifstream::beg))
	{
		log->critical("Failed to seek file offset on TankFile::seekAbsoluteOffset()!");
		return;
	}
}

void TankFile::readBytes(void * buffer, const size_t numBytes)
{
	assert(buffer   != nullptr);
	assert(numBytes != 0);
	assert(isOpen());

	if (!file.read(reinterpret_cast<char *>(buffer), numBytes))
	{
		log->critical("Only {} bytes of {} could be read from {}!", file.gcount(), numBytes,fileName);

		log->critical("Failed to read {} from Tank file {}", StringTool::formatMemoryUnit(numBytes), fileName);

		return;
	}
}

uint16_t TankFile::readU16()
{
	uint16_t x = 0;
	readBytes(&x, sizeof(x));
	return x;
}

uint32_t TankFile::readU32()
{
	uint32_t x = 0;
	readBytes(&x, sizeof(x));
	return x;
}

std::string TankFile::readNString()
{
	auto lenInChars = readU16();
	if (lenInChars == 0)
	{
		readU16(); // Waste another word to make this a dword
		return std::string();
	}

	// NSTRINGs are stored aligned at dword boundary:
	lenInChars = alignToDword(lenInChars + 2) - 2; // 2 for the word we've just read
	assert(((lenInChars + 2) % sizeof(uint32_t)) == 0);

	if (lenInChars >= 2048)
	{
		log->critical("String overflow in TankFile::readNString()! lenInChars >= 2048");
	}

	char buffer[2048];
	readBytes(buffer, lenInChars);
	buffer[lenInChars] = 0;

	return buffer;
}

WideString TankFile::readWNString()
{
	auto lenInChars = readU16();
	if (lenInChars == 0)
	{
		readU16(); // Waste another word to make this a dword
		return WideString();
	}

	// NSTRINGs are stored aligned at dword boundary:
	lenInChars = alignToDword(lenInChars + 2) - 2; // 2 for the word we've just read
	assert(((lenInChars + 2) % sizeof(uint32_t)) == 0);

	if (lenInChars >= 2048)
	{
		log->critical("String overflow in TankFile::readWNString()! {} >= 2048", lenInChars);
		return {};
	}

	WideChar buffer[2048];
	readBytes(buffer, lenInChars * sizeof(WideChar));
	buffer[lenInChars] = 0;

	return buffer;
}

FileTime TankFile::readFileTime()
{
	FileTime ft;
	readBytes(&ft, sizeof(ft));
	return ft;
}

SystemTime TankFile::readSystemTime()
{
	SystemTime st;
	readBytes(&st, sizeof(st));
	return st;
}

ProductVersion TankFile::readProductVersion()
{
	ProductVersion pv;
	readBytes(&pv, sizeof(pv));
	return pv;
}

FourCC TankFile::readFourCC()
{
	FourCC fcc;
	readBytes(&fcc, sizeof(fcc));
	return fcc;
}

Guid TankFile::readGuid()
{
	Guid guid;
	readBytes(&guid, sizeof(guid));
	return guid;
}

} // namespace ehb {}
