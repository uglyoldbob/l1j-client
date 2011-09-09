#ifndef __FUNCPTR_H_
#define __FUNCPTR_H_

class funcptr
{
	public:
		funcptr();
		virtual ~funcptr();
		virtual void go() = 0;
};

#endif