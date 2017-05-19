#ifndef _SINGLETON_H__
#define _SINGLETON_H__
#include <memory>

namespace fastbase
{
	//single thread
	template <typename T>
	class TSingleton
	{
	public:
		static inline T* Instance();
		TSingleton(void){}
		~TSingleton(void){}
	private:
		
		TSingleton(const TSingleton&){}
		TSingleton & operator= (const TSingleton &){}

	private:
		static typename std::shared_ptr<T> s_objInstance;
	};

	template <typename T> std::shared_ptr<T> TSingleton<T>::s_objInstance;

	template <typename T>
	inline T* TSingleton<T>::Instance()
	{
		if (NULL != s_objInstance.get())
			return s_objInstance.get();

		s_objInstance = std::shared_ptr<T>(new T);
		return s_objInstance.get();
	}
}

#endif/*_SINGLETON_H__*/
