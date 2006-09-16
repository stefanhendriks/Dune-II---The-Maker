/// \file
/// \brief \b [Internal] A single node in the Huffman Encoding Tree.
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

#ifndef __HUFFMAN_ENCODING_TREE_NODE
#define __HUFFMAN_ENCODING_TREE_NODE

struct HuffmanEncodingTreeNode
{
	unsigned char value;
	unsigned weight;
	HuffmanEncodingTreeNode *left;
	HuffmanEncodingTreeNode *right;
	HuffmanEncodingTreeNode *parent;
};

#endif


