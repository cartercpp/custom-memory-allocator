#pragma once

#include <stdexcept>
#include <utility>
#include <concepts>
#include <cstdint>
#include <cstddef>

class allocator
{
public:

	//CONSTRUCTORS & STUFF

	allocator()
	{
		m_freeList = reinterpret_cast<Header*>(m_heap);
		m_freeList->m_size = s_bytesInHeap - sizeof(Header);
	}

	//METHODS & STUFF

	template <typename ValueType>
	ValueType* allocate(std::size_t count = 1)
	{
		if (count == 0)
			throw std::runtime_error{ "Cannot allocate 0 bytes of data" };

		std::size_t bytesToAllocate = sizeof(ValueType) * count;
		bytesToAllocate = (bytesToAllocate + 7) & ~7uz;
		Header* currentHeader = m_freeList;

		while (currentHeader)
		{
			if (bytesToAllocate <= currentHeader->m_size)
			{
				std::uint8_t* startOfPayload = reinterpret_cast<std::uint8_t*>(currentHeader + 1);
				currentHeader->m_isFull = true;

				const std::size_t remainder = currentHeader->m_size - bytesToAllocate;
				if (remainder >= sizeof(Header) + 16)
				{
					Header* newBlock = reinterpret_cast<Header*>(startOfPayload + bytesToAllocate);
					newBlock->m_size = remainder - sizeof(Header);
					newBlock->m_next = currentHeader->m_next;
					currentHeader->m_next = newBlock;
					currentHeader->m_size = bytesToAllocate;
				}

				return reinterpret_cast<ValueType*>(startOfPayload);
			}

			currentHeader = currentHeader->m_next;
		}

		throw std::runtime_error{ "Allocation failed" };
	}

	void deallocate(void* ptr)
	{
		Header* header = reinterpret_cast<Header*>(ptr) - 1;
		header->m_isFull = false;

		if (header->m_next && !header->m_next->m_isFull)
		{
			header->m_size += sizeof(Header) + header->m_next->m_size;
			header->m_next = header->m_next->m_next;
		}
	}

private:

	struct Header
	{
		std::size_t m_size;
		bool m_isFull = false;
		Header* m_next = nullptr;
	};

	static constexpr std::size_t s_bytesInHeap = 100 * 1024;
	std::uint8_t m_heap[s_bytesInHeap];
	Header* m_freeList;
};
