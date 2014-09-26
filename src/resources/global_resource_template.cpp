#include "global_resource_template.h"

template <class T>
T &base_resource<T>::operator [](int i)
{
	return base_resource::data[i];
}

template <class T>
server_resource<T>::server_resource()
{
	for (int i = 0; i < base_resource<T>::data.size(); i++)
	{
		present[i] = 0;
	}
}

template <class T>
const T &server_resource<T>::operator [](int i)
{
	if (present[i] == 0)
	{
		return base_resource<T>::data[i];
	}
	else
	{
		return over[i];
	}
}

template <class T>
void server_resource<T>::override(int i, T with)
{
	present[i] = 1;
	over[i] = with;
}
