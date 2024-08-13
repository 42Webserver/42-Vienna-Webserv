#ifndef SINGLETON_HPP
# define SINGLETON_HPP

#include <cstddef>

template <typename T>
class Singleton
{
public:

	Singleton() {}
	~Singleton() {}

	static T&	get(void)
	{
		if (m_pObject == NULL)
			m_pObject = new T;
		return (*m_pObject);
	}

private:
	static T*	m_pObject;
};

template <typename T>
T*	Singleton<T>::m_pObject = NULL;

#endif //!SINGLETON_HPP
