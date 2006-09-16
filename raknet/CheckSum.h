/// \file
/// \brief \b [Internal] Generates and validates checksums
///
/// \note I didn't write this, but took it from http://www.flounder.com/checksum.htm
///

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

/// Generates and validates checksums
class CheckSum
{

public:
	
 /// Default constructor
	
	CheckSum()
	{
		clear();
	}
	
	void clear()
	{
		sum = 0;
		r = 55665;
		c1 = 52845;
		c2 = 22719;
	}
	
	void add ( unsigned int w );
	
	
	void add ( unsigned short w );
	
	void add ( unsigned char* b, unsigned int length );
	
	void add ( unsigned char b );
	
	unsigned int get ()
	{
		return sum;
	}
	
protected:
	unsigned short r;
	unsigned short c1;
	unsigned short c2;
	unsigned int sum;
};

#endif


