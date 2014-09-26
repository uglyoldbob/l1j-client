#include <map>
#include <vector>

template <class T>

class base_resource
{
	public:
		T &operator [](int i);
	protected:
		static std::vector<T> data;
};

template <class T>
class server_resource : public base_resource<T>
{
	public:
		server_resource();
		const T & operator [](int i);
		void override(int i, T with);
	private:
		std::map<int, T> over;
		std::vector<char> present;	//is the element over-ridden?
};
