/// \file
/// \brief \b [Internal] A queue implemented as a linked list.
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

#ifndef __QUEUE_LINKED_LIST_H
#define __QUEUE_LINKED_LIST_H

#include "LinkedList.h" 

/// The namespace BasicDataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace BasicDataStructures
{
	/// \brief A queue implemented using a linked list.  Rarely used.
	template <class QueueType>
	class QueueLinkedList
	{
	
	public:
		QueueLinkedList();
		QueueLinkedList( const QueueLinkedList& original_copy );
		bool operator= ( const QueueLinkedList& original_copy );
		QueueType pop( void );
		QueueType& peek( void );
		QueueType& end_peek( void );
		void push( const QueueType& input );
		unsigned int size( void );
		void clear( void );
		void compress( void );
		
	private:
		LinkedList<QueueType> data;
	};
	
	template <class QueueType>
	QueueLinkedList<QueueType>::QueueLinkedList()
	{
	}
	
	template <class QueueType>
	inline unsigned int QueueLinkedList<QueueType>::size()
	{
		return data.size();
	}
	
	template <class QueueType>
	inline QueueType QueueLinkedList<QueueType>::pop( void )
	{
		data.beginning();
		return ( QueueType ) data.pop();
	}
	
	template <class QueueType>
	inline QueueType& QueueLinkedList<QueueType>::peek( void )
	{
		data.beginning();
		return ( QueueType ) data.peek();
	}
	
	template <class QueueType>
	inline QueueType& QueueLinkedList<QueueType>::end_peek( void )
	{
		data.end();
		return ( QueueType ) data.peek();
	}
	
	template <class QueueType>
	void QueueLinkedList<QueueType>::push( const QueueType& input )
	{
		data.end();
		data.add( input );
	}
	
	template <class QueueType>
	QueueLinkedList<QueueType>::QueueLinkedList( const QueueLinkedList& original_copy )
	{
		data = original_copy.data;
	}
	
	template <class QueueType>
	bool QueueLinkedList<QueueType>::operator= ( const QueueLinkedList& original_copy )
	{
		if ( ( &original_copy ) == this )
			return false;
			
		data = original_copy.data;
	}
	
	template <class QueueType>
	void QueueLinkedList<QueueType>::clear ( void )
	{
		data.clear();
	}
} // End namespace

#endif


