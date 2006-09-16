/// \file
/// \brief \b [Internal] Array based list.  Usually the Queue class is used instead, since it has all the same functionality and is only worse at random access.
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

#ifndef __LIST_H
#define __LIST_H 

#include <assert.h>

/// Maximum unsigned long
static const unsigned int MAX_UNSIGNED_LONG = 4294967295U;

/// The namespace BasicDataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace BasicDataStructures
{
	/// \brief Array based implementation of a list.
	template <class list_type>
	class List
	{	
	public:
		/// Default constructor
		List();

		/// Destructor
		~List();
		
		/// Copy constructor
		/// \param[in]  original_copy The list to duplicate 
		List( const List& original_copy );
		
		/// Assign one list to another
		List& operator= ( const List& original_copy );
		
		/// Access an element by its index in the array 
		/// \param[in]  position The index into the array. 
		/// \return The element at position \a position. 
		list_type& operator[] ( unsigned int position );
		
		/// Insert an element at position \a position in the list 
		/// \param[in] input The new element. 
		/// \param[in] position The position of the new element. 		
		void insert( list_type input, unsigned int position );
		
		/// Insert at the end of the list.
		/// \param[in] input The new element. 
		void insert( list_type input );
		
		/// Replace the value at \a position by \a input.  If the size of
		/// the list is less than @em position, it increase the capacity of
		/// the list and fill slot with @em filler.
		/// \param[in] input The element to replace at position @em position. 
		/// \param[in] filler The element use to fill new allocated capacity. 
		/// \param[in] position The position of input in the list. 		
		void replace( list_type input, list_type filler, unsigned int position );
		
		/// Replace the last element of the list by \a input .
		/// \param[in] input The element used to replace the last element. 
		void replace( list_type input );
		
		/// Delete the element at position \a position. 
		/// \param[in] position The index of the element to delete 
		void del( unsigned int position );
		
		/// Delete the element at the end of the list 
		void del();
		
		/// Returns the index of the specified item or MAX_UNSIGNED_LONG if not found
		/// \param[in] input The element to check for 
		/// \return The index or position of @em input in the list. 
		/// \retval MAX_UNSIGNED_LONG The object is not in the list
		/// \retval [Integer] The index of the element in the list
		unsigned int getIndexOf( list_type input );
		
		/// \return The number of elements in the list
		unsigned int size( void ) const;
		
		/// Clear the list		
		void clear( void );
		
		/// Frees overallocated members, to use the minimum memory necessary
		/// \attention 
		/// This is a slow operation		
		void compress( void );
		
	private:
		/// An array of user values
		list_type* array;
		
		/// Number of elements in the list 		
		unsigned int list_size;
		
		/// Size of \a array 		
		unsigned int allocation_size;
	};
	
	template <class list_type>
	List<list_type>::List()
	{
		allocation_size = 16;
		array = new list_type[ allocation_size ];
		list_size = 0;
	}
	
	template <class list_type>
	List<list_type>::~List()
	{
		if (allocation_size>0)
			delete [] array;
	}
	
	
	template <class list_type>
	List<list_type>::List( const List& original_copy )
	{
		// Allocate memory for copy
		
		if ( original_copy.list_size == 0 )
		{
			list_size = 0;
			allocation_size = 0;
		}
		
		else
		{
			array = new list_type [ original_copy.list_size ];
			
			for ( unsigned int counter = 0; counter < original_copy.list_size; ++counter )
				array[ counter ] = original_copy.array[ counter ];
				
			list_size = allocation_size = original_copy.list_size;
		}
	}
	
	template <class list_type>
	List<list_type>& List<list_type>::operator= ( const List& original_copy )
	{
		if ( ( &original_copy ) != this )
		{
			clear();
			
			// Allocate memory for copy
			
			if ( original_copy.list_size == 0 )
			{
				list_size = 0;
				allocation_size = 0;
			}
			
			else
			{
				array = new list_type [ original_copy.list_size ];
				
				for ( unsigned int counter = 0; counter < original_copy.list_size; ++counter )
					array[ counter ] = original_copy.array[ counter ];
					
				list_size = allocation_size = original_copy.list_size;
			}
		}
		
		return *this;
	}
	
	
	template <class list_type>
	inline list_type& List<list_type>::operator[] ( unsigned int position )
	{
		assert ( position < list_size );
		return array[ position ];
	}
	
	template <class list_type>
	void List<list_type>::insert( list_type input, unsigned int position )
	{
#ifdef _DEBUG
		assert( position <= list_size );
#endif
		
		// Reallocate list if necessary
		
		if ( list_size == allocation_size )
		{
			// allocate twice the currently allocated memory
			list_type * new_array;
			
			if ( allocation_size == 0 )
				allocation_size = 16;
			else
				allocation_size *= 2;
				
			new_array = new list_type [ allocation_size ];
			
			// copy old array over
			for ( unsigned int counter = 0; counter < list_size; ++counter )
				new_array[ counter ] = array[ counter ];
				
			// set old array to point to the newly allocated and twice as large array
			delete[] array;
			
			array = new_array;
		}
		
		// Move the elements in the list to make room
		for ( unsigned int counter = list_size; counter != position; counter-- )
			array[ counter ] = array[ counter - 1 ];
			
		// Insert the new item at the correct spot
		array[ position ] = input;
		
		++list_size;
		
	}
	
	
	template <class list_type>
	void List<list_type>::insert( list_type input )
	{
		// Reallocate list if necessary
		
		if ( list_size == allocation_size )
		{
			// allocate twice the currently allocated memory
			list_type * new_array;
			
			if ( allocation_size == 0 )
				allocation_size = 16;
			else
				allocation_size *= 2;
				
			new_array = new list_type [ allocation_size ];
			
			// copy old array over
			for ( unsigned int counter = 0; counter < list_size; ++counter )
				new_array[ counter ] = array[ counter ];
				
			// set old array to point to the newly allocated and twice as large array
			delete[] array;
			
			array = new_array;
		}
		
		// Insert the new item at the correct spot
		array[ list_size ] = input;
		
		++list_size;
	}
	
	template <class list_type>
	inline void List<list_type>::replace( list_type input, list_type filler, unsigned int position )
	{
		if ( ( list_size > 0 ) && ( position < list_size ) )
		{
			// Direct replacement
			array[ position ] = input;
		}
		else
		{
			if ( position >= allocation_size )
			{
				// Reallocate the list to size position and fill in blanks with filler
				list_type * new_array;
				allocation_size = position + 1;
				
				new_array = new list_type [ allocation_size ];
				
				// copy old array over
				
				for ( unsigned int counter = 0; counter < list_size; ++counter )
					new_array[ counter ] = array[ counter ];
					
				// set old array to point to the newly allocated array
				delete[] array;
				
				array = new_array;
			}
			
			// Fill in holes with filler
			while ( list_size < position )
				array[ list_size++ ] = filler;
				
			// Fill in the last element with the new item
			array[ list_size++ ] = input;
			
#ifdef _DEBUG
			
			assert( list_size == position + 1 );
			
#endif
			
		}
	}
	
	template <class list_type>
	inline void List<list_type>::replace( list_type input )
	{
		if ( list_size > 0 )
			array[ list_size - 1 ] = input;
	}
	
	template <class list_type>
	void List<list_type>::del( unsigned int position )
	{
#ifdef _DEBUG
		assert( position < list_size );
#endif
		
		if ( position < list_size )
		{
			// Compress the array
			
			for ( unsigned int counter = position; counter < list_size - 1 ; ++counter )
				array[ counter ] = array[ counter + 1 ];
				
			del();
		}
	}
	
	template <class list_type>
	inline void List<list_type>::del()
	{
		// Delete the last element on the list.  No compression needed
#ifdef _DEBUG
		assert(list_size>0);
#endif
		list_size--;
	}
	
	template <class list_type>
	unsigned int List<list_type>::getIndexOf( list_type input )
	{
		for ( unsigned int i = 0; i < list_size; ++i )
			if ( array[ i ] == input )
				return i;
				
		return MAX_UNSIGNED_LONG;
	}
	
	template <class list_type>
	inline unsigned int List<list_type>::size( void ) const
	{
		return list_size;
	}
	
	template <class list_type>
	void List<list_type>::clear( void )
	{
		if ( allocation_size == 0 )
			return;
		
		if (allocation_size>32)
		{
			delete [] array;
			allocation_size = 0;
			array = 0;
		}
		list_size = 0;
	}
	
	template <class list_type>
	void List<list_type>::compress( void )
	{
		list_type * new_array;
		
		if ( allocation_size == 0 )
			return ;
			
		new_array = new list_type [ allocation_size ];
		
		// copy old array over
		for ( unsigned int counter = 0; counter < list_size; ++counter )
			new_array[ counter ] = array[ counter ];
			
		// set old array to point to the newly allocated array
		delete[] array;
		
		array = new_array;
	}
	
} // End namespace

#endif
