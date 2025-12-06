#include "it.h"
#include "error.h"

#include <cstring>

namespace IT
{
	IdTable Create(int size)
	{
		if (size > TI_MAXSIZE)
		{
			ERROR_THROW(120);
		}
		IdTable idtable;
		idtable.maxsize = size;
		idtable.size = 0;
		idtable.table = new Entry[size];
		return idtable;
	}

	void Add(IdTable& idtable, Entry& entry)
	{
		if (idtable.size >= idtable.maxsize)
		{
			ERROR_THROW(122);
		}
		idtable.table[idtable.size] = entry;
		idtable.size++;
	}

	Entry GetEntry(IdTable& idtable, int n)
	{
		if (n < 0 || n >= idtable.size)
		{
			ERROR_THROW(124);
		}
		return idtable.table[n];
	}

	int IsId(IdTable& idtable, std::string fullName)
	{
		for (int i = 0; i < idtable.size; ++i)
		{
			if (idtable.table[i].fullName == fullName)
			{
				return i;
			}
		}
		return IT_NULLIDX;
	}

	void Delete(IdTable& idtable)
	{
		if (idtable.table != nullptr)
		{
			delete[] idtable.table;
			idtable.table = nullptr;
		}
		idtable.maxsize = 0;
		idtable.size = 0;
	}
}
