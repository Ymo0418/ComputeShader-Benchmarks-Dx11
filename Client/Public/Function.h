#ifndef Function_h__
#define Function_h__

namespace Client
{
	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int		dwRefCnt = { 0 };

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		dwRefCnt = { 0 };

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}
}

#endif // Function_h__