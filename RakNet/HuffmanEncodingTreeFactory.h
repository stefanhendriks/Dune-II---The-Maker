/// \file
/// \brief \b [Internal] Creates instances of the class HuffmanEncodingTree
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
 
#ifndef __HUFFMAN_ENCODING_TREE_FACTORY
#define __HUFFMAN_ENCODING_TREE_FACTORY

class HuffmanEncodingTree;

/// \brief Creates instances of the class HuffmanEncodingTree
///
/// This class takes a frequency table and given that frequence table, will generate an instance of HuffmanEncodingTree
class HuffmanEncodingTreeFactory
{
public:
	/// Default constructor
	HuffmanEncodingTreeFactory();
	
	/// Reset the frequency table.  You don't need to call this unless you want to reuse the class for a new tree
	void Reset( void );
	
	/// Pass an array of bytes to this to add those elements to the frequency table
	/// \param[in] array the data to insert into the frequency table 
	/// \param[in] size the size of the data to insert 
	void AddToFrequencyTable( unsigned char *array, int size );
	
	/// Copies the frequency table to the array passed
	/// Retrieve the frequency table 
	/// \param[in] _frequency The frequency table used currently
	void GetFrequencyTable( unsigned int _frequency[ 256 ] );
	
	/// Returns the frequency table as a pointer
	/// \return the address of the frenquency table 
	unsigned int * GetFrequencyTable( void );
	
	/// Generate a HuffmanEncodingTree.
	/// You can also use GetFrequencyTable and GenerateFromFrequencyTable in the tree itself
	/// \return The generated instance of HuffmanEncodingTree
	HuffmanEncodingTree * GenerateTree( void );
	
private:

	/// Frequency table
	unsigned int frequency[ 256 ];
};

#endif
