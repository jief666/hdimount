/*
Copyright (C) 2018 Jief Luce

This file is originally based on the work of Tor Arne Vestbø (https://github.com/torarnv/sparsebundlefs).

You should have received a copy of the GNU General Public License
along with hdimount.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparsebundleReader.h"

#include <cstring>
#include <vector>
#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h> // for strtoumax
#include <fcntl.h> // open, read, close...

#include "Utils.h"
#include "../darling-dmg/src/exceptions.h"
#include "../darling-dmg/src/FileReader.h"

#include "../darling-dmg/src/SubReader.h"

static uint64_t xml_get_integer(const char* filename, const char* elem)
{
	FILE *f = fopen(filename, "rb");
	if ( fseek(f, 0, SEEK_END) != 0 ) return 0;
	size_t fsize = ftell(f);
	if ( fseek(f, 0, SEEK_SET) != 0 ) return 0;  //same as rewind(f);

	char token[fsize + 1];
	if ( fread(token, 1, fsize, f) != fsize ) return 0;
	fclose(f);
	token[fsize] = 0;

	char* bufferPtr = token;

	while (bufferPtr)
	{
		char* posKey = strstr(bufferPtr, "<key>");
		if ( !posKey ) return 0;
		posKey += strlen("<key>");
		while ( *posKey == ' ' || *posKey == '\r' || *posKey == '\n' || *posKey == '\t' ) posKey++;

		char* posKeyEnd = strstr(bufferPtr, "</key>");
		if ( !posKeyEnd ) return 0;
		bufferPtr = posKeyEnd + strlen("</key>");
		while ( *(posKey-1) == ' ' || *(posKey-1) == '\r' || *(posKey-1) == '\n' || *(posKey-1) == '\t' ) posKey--;
		*posKeyEnd = 0;

		if ( strcmp(posKey, elem) == 0 )
		{
			char* posValue = bufferPtr;
			while ( *posValue == ' ' || *posValue == '\r' || *posValue == '\n' || *posValue == '\t' ) posValue++;
			if ( strncmp(posValue, "<integer>", strlen("<integer>")) != 0 ) return 0;
			posValue += strlen("<integer>");
			while ( *posValue == ' ' || *posValue == '\r' || *posValue == '\n' || *posValue == '\t' ) posValue++;

			char* posValueEnd = strstr(bufferPtr, "</integer>");
			if ( !posValueEnd ) return 0;
			bufferPtr = posValueEnd + strlen("</integer>"); // for just after
			while ( *(posValueEnd-1) == ' ' || *(posValueEnd-1) == '\r' || *(posValueEnd-1) == '\n' || *(posValueEnd-1) == '\t' ) posValueEnd--;
			*posValueEnd = 0;

			uint64_t rv = strtoumax(posValue, 0, 10);
			return rv;

		}
	}
	return 0;
}

SparsebundleReader::SparsebundleReader(const std::string& path)
{
  char printf_zerobuf[1] = {};
  int printf_size;

    if (path.length() == 0) {
		throw io_error("Name is needed to create a SparsebundleReader");
    }

    m_path = realpath(path.c_str(), NULL);
    if (!m_path) {
		throw io_error(stprintf("Realpath not found for '%s'", path.c_str()));
    }
    size_t m_band_path_len = strlen(m_path) + 7 + 20 + 1;  // 7 for "/bands/" and 20 is the max nb of digits of 64 bits unsigned int.
    m_band_path = (char*)malloc(m_band_path_len);
    m_band_path_band_number_start = m_band_path + sprintf(m_band_path, "%s/bands/", m_path);

    m_blocksize = 512;
    m_band_size = 0;
    m_opened_file_fd = -1;
    m_opened_file_band_number = -1;

	// Read plist
    printf_size = snprintf(printf_zerobuf, 0, "%s/Info.plist", m_path);
    char plist_path[printf_size+1];
    snprintf(plist_path, printf_size+1, "%s/Info.plist", m_path);
	m_band_size = xml_get_integer(plist_path, "band-size");
	m_size = xml_get_integer(plist_path, "size");
	if ( m_band_size == 0 ) {
		free(m_path);
		throw io_error(stprintf("Cannot get band-size from plist '%s'", plist_path));
	}
	if ( m_size == 0 ) {
		free(m_path);
		throw io_error(stprintf("Cannot get size from plist '%s'", plist_path));
	}

printf("Initialized %s, band size %zu, total size %" PRId64 "\n", m_path, m_band_size, m_size);

	std::string tokenFilename = stprintf("%s/token", m_path);
	m_tokenReader = std::make_shared<FileReader>(tokenFilename);
//
//	DmgCryptHeaderV2 hdr;
//	int32_t bytesRead = tokenReader->read((void*)&hdr, sizeof(hdr), 0);
//	if (bytesRead >=sizeof(hdr)) {
//		if (memcmp(hdr.signature, "encrcdsa", 8) == 0) {
//			m_tokenReader = std::make_shared<SubReader>(tokenReader, 0, hdr.encrypted_data_offset);
//		}
//	}

//	m_size += tokenReader->length();
	
	
//	std::ifstream tokenStream;
//	tokenStream.open(token_filename, std::ios::binary);
//	if ( tokenStream.is_open() )
//	{
//		char signature[8];
//		tokenStream.seekg(0);
//		tokenStream.read(signature, 8);
//
//		if (!memcmp(signature, "encrcdsa", 8))
//		{
//			m_is_encrypted = true;
//		}
//	}
}

SparsebundleReader::~SparsebundleReader()
{
	if ( m_path ) {
		free(m_path);
		m_path = NULL;
		if ( m_opened_file_fd != -1 ) close(m_opened_file_fd);
	}
}
//
//std::shared_ptr<Reader> SparsebundleReader::getTokenReader()
//{
//  char printf_zerobuf[1] = {};
//  int printf_size;
//
//	// Check token file for encryption
//	printf_size = snprintf(printf_zerobuf, 0, "%s/token", m_path);
//	char token_filename[printf_size+1];
//	snprintf(token_filename, printf_size+1, "%s/token", m_path);
//
//	std::string token_filename2 = stprintf("%s/token", m_path);
//	return std::make_shared<FileReader>(token_filename2);
//
//
//	std::ifstream tokenStream;
//	tokenStream.open(token_filename, std::ios::binary);
//	if ( tokenStream.is_open() )
//	{
//		char signature[8];
//		tokenStream.seekg(0);
//		tokenStream.read(signature, 8);
//
//		if (!memcmp(signature, "encrcdsa", 8))
//		{
//			m_is_encrypted = true;
//		}
//	}
//}

int32_t SparsebundleReader::read(void* buffer, int32_t nbytes, uint64_t offset)
{
    if (nbytes < 0)
        throw io_error("SparsebundleReader::read nbytes : 0");
    if (offset >= length())
        throw io_error("SparsebundleReader::read offset >= size");

    if ( offset + (off_t)nbytes > length()) {
        nbytes = length() - offset;
    }

    int32_t bytes_read = 0;

	if ( offset < m_tokenReader->length() )
	{
		int32_t bytes_to_read = std::min(uint64_t(nbytes), m_tokenReader->length() - offset);
        bytes_read = m_tokenReader->read((uint8_t*)buffer, bytes_to_read, offset);
        if (bytes_read != bytes_to_read)
            throw io_error(stprintf("SparsebundleReader::read Cannot read %d bytes in token file at '%s', read %d bytes", bytes_to_read, m_path, bytes_read));
		offset = m_tokenReader->length();
	}
	uint64_t offsetInBands = offset - m_tokenReader->length();
    while (bytes_read < nbytes) {
        off_t band_number = (offsetInBands + bytes_read) / m_band_size;
        off_t band_offset = (offsetInBands + bytes_read) % m_band_size;

        size_t to_read = nbytes - bytes_read;
        if ( to_read > m_band_size - band_offset )  to_read = m_band_size - band_offset;

        // Caching opened file desciptor to avoid open and close.
        if ( m_opened_file_band_number != band_number )
        {
        	if ( m_opened_file_fd != -1 ) close(m_opened_file_fd);
			sprintf(m_band_path_band_number_start, "%" PRIx64, band_number);
        	m_opened_file_fd = open(m_band_path, O_RDONLY);
        	if ( m_opened_file_fd == -1 ) {
        		m_opened_file_band_number = -1;
        		throw io_error(stprintf("SparsebundleReader::read cannot open band %lld (at '%s')", band_number, m_band_path));
        	}
        	m_opened_file_band_number = band_number;
        }

        ssize_t nb_read = pread(m_opened_file_fd, ((uint8_t*)buffer)+bytes_read, to_read, band_offset);
        if (nb_read < 0) {
            throw io_error(stprintf("SparsebundleReader::read read at offset %lld returns %zd", band_offset, nb_read));
        }

        if (size_t(nb_read) < to_read) { // nb_read is > 0 so cast is safe
            ssize_t to_pad = to_read - nb_read;
            if ( to_pad+bytes_read+nb_read > nbytes ) {
            	exit(1);
            }
if (nbytes < 0)
	throw io_error("");
            memset(((uint8_t*)buffer)+bytes_read+nb_read, 0, to_pad);
            nb_read += to_pad;
        }

        bytes_read += nb_read;
    }
	return bytes_read;
//	if (bytes_read != nbytes) throw io_error(stprintf("DeviceSparsebundle::ReadRaw read only %zd bytes instead of %zd", bytes_read, nbytes));
}

uint64_t SparsebundleReader::length()
{
	return m_size;
}
//
//int32_t SparsebundleReader::getTokenLength()
//{
//	if (!m_tokenReader) return 0;
//	return m_tokenReader->length();
//}
//
//uint64_t SparsebundleReader::getTokenOffset()
//{
//	return 0;
//}
//
//void SparsebundleReader::getToken(uint8_t* token)
//{
//	int32_t bytesRead = m_tokenReader->read(token, m_tokenReader->length(), 0);
//	if (bytesRead != m_tokenReader->length()) // *len is 0x1000 max, so cast is safe.
//		throw io_error("Can't read");
//}
