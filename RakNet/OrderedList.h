/// \file
/// \brief \b [Internal] Quicksort ordered list.
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

#ifndef __ORDERED_LIST_H
#define __ORDERED_LIST_H

#include "ArrayList.h"

/// The namespace BasicDataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace BasicDataStructures
{
	/// \brief The ordered list class is a list which keeps its elements in order
	///
	/// Unlike some implementation, the keys are not considered part of the data
	template <class data_type, class key_type>
	class OrderedList
	{
	public:
		static int defaultComparison(data_type a, key_type b) {if (b<a) return -1; if (a==b) return 0; return 1;}
		
		OrderedList();
		~OrderedList();
		/// comparisonFunction must take a key_type and a data_type and return <0, ==0, or >0
		/// If the data type has comparison operators already defined then you can just use defaultComparison
		bool HasData(key_type key, int (*comparisonFunction)(data_type, key_type)=defaultComparison);
		unsigned GetIndexFromKey(key_type key, bool *objectExists, int (*comparisonFunction)(data_type, key_type));
		data_type GetElementFromKey(key_type key, int (*comparisonFunction)(data_type, key_type)=defaultComparison);
		bool InsertElement(data_type data, int (*comparisonFunction)(data_type, key_type)=defaultComparison);
		bool InsertElement(data_type data, key_type key, int (*comparisonFunction)(data_type, key_type)=defaultComparison);
		void RemoveElement(key_type key, int (*comparisonFunction)(data_type, key_type)=defaultComparison);
		data_type& operator[] ( unsigned int position );
		void RemoveElementAtIndex(unsigned index);
		void Clear(void);		
		unsigned Size(void) const;
		

	protected:
		BasicDataStructures::List<data_type> dataList;
	};

	template <class data_type, class key_type>
	OrderedList<data_type, key_type>::OrderedList()
	{
	}

	template <class data_type, class key_type>
	OrderedList<data_type, key_type>::~OrderedList()
	{
		Clear();
	}

	template <class data_type, class key_type>
	bool OrderedList<data_type, key_type>::HasData(key_type key, int (*comparisonFunction)(data_type, key_type))
	{
		bool objectExists;
		unsigned index;
		index = GetIndexFromKey(key, &objectExists, comparisonFunction);
		return objectExists;
	}

	template <class data_type, class key_type>
	data_type OrderedList<data_type, key_type>::GetElementFromKey(key_type key, int (*comparisonFunction)(data_type, key_type))
	{
		bool objectExists;
		unsigned index;
		index = GetIndexFromKey(key, &objectExists, comparisonFunction);
		assert(objectExists);
		return dataList[index];
	}

	template <class data_type, class key_type>
	unsigned OrderedList<data_type, key_type>::GetIndexFromKey(key_type key, bool *objectExists, int (*comparisonFunction)(data_type, key_type))
	{
		int index, upperBound, lowerBound;
		int res;

		if (dataList.size()==0)
		{
			*objectExists=false;
			return 0;
		}

		upperBound=(int)dataList.size()-1;
		lowerBound=0;
		index = (int)dataList.size()/2;

	#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
		while (1)
		{
			res = comparisonFunction(dataList[index],key);
			if (res==0)
			{
				*objectExists=true;
				return index;
			}
			else if (res<0)
			{
				upperBound=index-1;
				index=(upperBound-lowerBound)/2;
			}
			else// if (res>0)
			{
				lowerBound=index+1;
				index=lowerBound+(upperBound-lowerBound)/2;
			}

			if (lowerBound>upperBound)
			{
				*objectExists=false;
				return lowerBound; // No match
			}
		}
	}

	template <class data_type, class key_type>
		bool OrderedList<data_type, key_type>::InsertElement(data_type data, int (*comparisonFunction)(data_type, key_type))
	{
		bool objectExists;
		unsigned index;
		index = GetIndexFromKey(data, &objectExists, comparisonFunction);

		// Duplicate insertion!  Don't insert elements more than once
		assert(objectExists==false);
		if (objectExists)
			return false;

		if (index>=dataList.size())
			dataList.insert(data);
		else
			dataList.insert(data,index);

		return true;
	}


	template <class data_type, class key_type>
	bool OrderedList<data_type, key_type>::InsertElement(data_type data, key_type key, int (*comparisonFunction)(data_type, key_type))
	{
		bool objectExists;
		unsigned index;
		index = GetIndexFromKey(key, &objectExists, comparisonFunction);

		// Duplicate insertion!  Don't insert elements more than once
		assert(objectExists==false);
		if (objectExists)
			return false;

		if (index>=dataList.size())
			dataList.insert(data);
		else
			dataList.insert(data,index);

		return true;
	}

	template <class data_type, class key_type>
	void OrderedList<data_type, key_type>::RemoveElement(key_type key, int (*comparisonFunction)(data_type, key_type))
	{
		bool objectExists;
		unsigned index;
		index = GetIndexFromKey(key, &objectExists, comparisonFunction);

		// Can't find the element to remove if this assert hits
		assert(objectExists==true);
		if (objectExists==false)
			return;

		dataList.del(index);
	}

	template <class data_type, class key_type>
	void OrderedList<data_type, key_type>::RemoveElementAtIndex(unsigned index)
	{
		dataList.del(index);
	}

	template <class data_type, class key_type>
	void OrderedList<data_type, key_type>::Clear(void)
	{
		dataList.clear();
	}

	template <class data_type, class key_type>
	data_type& OrderedList<data_type, key_type>::operator[]( unsigned int position )
	{
		return dataList[position];
	}

	template <class data_type, class key_type>
	unsigned OrderedList<data_type, key_type>::Size(void) const
	{
		return dataList.size();
	}
}

#endif
