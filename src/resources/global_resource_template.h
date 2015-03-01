#ifndef _GLOBAL_RESOURCE_TEMPLATE_H_
#define _GLOBAL_RESOURCE_TEMPLATE_H_

#include <map>
#include <stdexcept>

template <class T>
class base_resource
{
	public:
		T &operator [](int i);
	protected:
		static std::map<int, T> data;
};

template <class T>
std::map<int, T> base_resource<T>::data;

template <class T>
class server_resource : public base_resource<T>
{
	public:
		const T & operator [](int i);
		void override(int i, T with);
	private:
		std::map<int, T> over;
};

template <class T>
T &base_resource<T>::operator [](int i)
{
	return base_resource<T>::data[i];
}

template <class T>
const T &server_resource<T>::operator [](int i)
{
	typedef typename std::map<int,T>::iterator iter;
	iter it;
	it = over.find(i);
	if (it != over.end())
	{
		return it->second;
	}
	else
	{
		return base_resource<T>::data[i];
	}
}

template <class T>
void server_resource<T>::override(int i, T with)
{
	over[i] = with;
}

#endif
