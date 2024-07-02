#ifndef SHAREDPTR_H
# define SHAREDPTR_H

#include <stddef.h>
#include <iostream>

template <typename T>
class SharedPtr
{
private:

	struct ControlBlock
	{
		T*	m_pObject;
		int	m_iRefCount;

		ControlBlock(T* a_pObj);
		~ControlBlock();
	};
	
	ControlBlock*	m_sControlBlock;

public:
	SharedPtr(void);
	SharedPtr(T* a_pObject);
	SharedPtr(const SharedPtr& other);
	SharedPtr& operator=(const SharedPtr& other);
	~SharedPtr();

	operator T*(void) const;
	operator bool() const;
	T&	operator*() const;
	T*	operator->() const;
};

template <typename T>
SharedPtr<T>::ControlBlock::ControlBlock(T* a_pObj) : m_pObject(a_pObj), m_iRefCount(0) {}

template <typename T>
SharedPtr<T>::ControlBlock::~ControlBlock()
{
	delete m_pObject;
}

template <typename T>
SharedPtr<T>::SharedPtr(void) : m_sControlBlock(NULL) {}

template <typename T>
SharedPtr<T>::SharedPtr(T* a_pObject) : m_sControlBlock(new ControlBlock(a_pObject))
{
	m_sControlBlock->m_iRefCount++;
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T>& other) : m_sControlBlock(other.m_sControlBlock)
{
	m_sControlBlock->m_iRefCount++;
}

template <typename T>
SharedPtr<T>&	SharedPtr<T>::operator= (const SharedPtr<T>& other)
{
	if (this != &other)
	{
		if (--m_sControlBlock->m_iRefCount <= 0)
			delete m_sControlBlock;
		m_sControlBlock = other.m_sControlBlock;
		++m_sControlBlock->m_iRefCount;
	}
	return (*this);
}

template <typename T>
SharedPtr<T>::~SharedPtr()
{
	if (--m_sControlBlock->m_iRefCount <= 0)
	{
		delete m_sControlBlock;
	}
	std::cout << std::endl;
}

template <typename T>
SharedPtr<T>::operator T*(void) const
{
	return (m_sControlBlock->m_pObject);
}

template <typename T>
SharedPtr<T>::operator bool(void) const
{
	return (m_sControlBlock->m_pObject != NULL);
}

template <typename T>
T&	SharedPtr<T>::operator*() const
{
	return (*m_sControlBlock->m_pObject);
}

template <typename T>
T*	SharedPtr<T>::operator->() const
{
	return (m_sControlBlock->m_pObject);
}

#endif // !SHAREDPTR_H
