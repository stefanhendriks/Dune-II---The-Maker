/// \file
/// \brief \b [Internal] Generates a huffman encoding tree, used for string and global compression.
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

#ifndef __HUFFMAN_ENCODING_TREE
#define __HUFFMAN_ENCODING_TREE

#include "HuffmanEncodingTreeNode.h"
#include "BitStream.h"

#include "LinkedList.h" 

/// This generates special cases of the huffman encoding tree using 8 bit keys with the additional condition that unused combinations of 8 bits are treated as a frequency of 1
class HuffmanEncodingTree
{

public:
	HuffmanEncodingTree();
	~HuffmanEncodingTree();
	
	/// Pass an array of bytes to array and a preallocated BitStream to receive the output
	/// \param [in] input Array of bytes to encode
	/// \param [in] sizeInBytes size of \a input
	/// \param [out] output The bitstream to write to
	void EncodeArray( unsigned char *input, int sizeInBytes, RakNet::BitStream * output );
	
	// Decodes an array encoded by EncodeArray()
	int DecodeArray( RakNet::BitStream * input, int sizeInBits, int maxCharsToWrite, unsigned char *output );
	void DecodeArray( unsigned char *input, int sizeInBits, RakNet::BitStream * output );
	
	/// Given a frequency table of 256 elements, all with a frequency of 1 or more, generate the tree
	void GenerateFromFrequencyTable( unsigned int frequencyTable[ 256 ] );
	
	/// Free the memory used by the tree
	void FreeMemory( void );
	
private:
	
 /// The root node of the tree 
	
	HuffmanEncodingTreeNode *root;
	
 /// Used to hold bit encoding for one character
	
	
	struct CharacterEncoding
	{
		unsigned char* encoding;
		unsigned short bitLength;
	};
	
	CharacterEncoding encodingTable[ 256 ];
	
	void InsertNodeIntoSortedList( HuffmanEncodingTreeNode * node, BasicDataStructures::LinkedList<HuffmanEncodingTreeNode *> *huffmanEncodingTreeNodeList ) const;
};

#endif


