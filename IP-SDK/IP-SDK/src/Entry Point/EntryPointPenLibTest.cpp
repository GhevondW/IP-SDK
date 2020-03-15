#include <DirManager.h>
#include <any>

int main()
{

	std::cout << "Hello pen Lib" << std::endl;

	std::any var = 5;
	int d = 6;

	std::cout << typeid(d).name() << std::endl;
	std::cout << var.type().name() << std::endl;

	return 0;
}
