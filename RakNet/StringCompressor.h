/// \file
/// \brief \b Compresses/Decompresses ASCII strings and writes/reads them to BitStream class instances.  You can use this to easily serialize and deserialize your own strings.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __STRING_COMPRESSOR_H
#define __STRING_COMPRESSOR_H

/// Forward declaration
namespace RakNet
{
	class BitStream;
};

class HuffmanEncodingTree;

/// \brief Writes and reads strings to and from bitstreams.
///
/// Only works with ASCII strings.  The default compression is for English.
/// You can call GenerateTreeFromStrings to compress and decompress other languages efficiently as well.
class StringCompressor
{
public:
	
 /// Destructor	
	~StringCompressor();
	
 /// static function because only static functions can access static members
 /// \return the unique instance of the StringCompressor 
	static inline StringCompressor* Instance()
	{
		return instance;
	}
	
	/// Given an array of strings, such as a chat log, generate the optimal encoding tree for it.
	/// This function is optional and if it is not called a default tree will be used instead.
	/// \param[in] input An array of bytes which should point to text.
	/// \param[in] inputLength Length of \a input
	void GenerateTreeFromStrings( unsigned char *input, unsigned inputLength );
	
 	/// Writes input to output, compressed.  Takes care of the null terminator for you.
	/// \param[in] input Pointer to an ASCII string
	/// \param[in] maxCharsToWrite The size of \a input 
	/// \param[out] output The bitstream to write the compressed string to
	void EncodeString( char *input, int maxCharsToWrite, RakNet::BitStream *output );
	
	/// Writes input to output, uncompressed.  Takes care of the null terminator for you.
	/// \param[out] output A block of bytes to receive the output
	/// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
	/// \param[in] input The bitstream containing the compressed string
	bool DecodeString( char *output, int maxCharsToWrite, RakNet::BitStream *input );

	/// Used so I can allocate and deallocate this singleton at runtime
	static void AddReference(void);
	
	/// Used so I can allocate and deallocate this singleton at runtime
	static void RemoveReference(void);
	
private:
	
	/// Create the huffman encoding tree (obviously)
	void GenerateHuffmanEncodingTree( void );
	
	/// Private Constructor	
	StringCompressor();
	
	/// Singleton instance
	static StringCompressor *instance;
	
	/// Pointer to the huffman encoding tree.
	HuffmanEncodingTree *huffmanEncodingTree;
	
	/// How many instance of RakPeer have been instantiated
	static int referenceCount;
};

/// Define to more easily reference the string compressor instance.
#define stringCompressor StringCompressor::Instance()

#endif
