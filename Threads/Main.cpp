#include <iostream>
#include<thread>
using namespace std::literals::chrono_literals;

bool stop = false;

void hello()
{
	while (!stop)
	{
		std::cout << "Hello ";
		std::this_thread::sleep_for(1s);	//приостанавливает выполнение кода на 1 секунду
	}
}

void world()
{
	while (!stop)
	{
		std::cout << "World!";
		std::this_thread::sleep_for(1s);
	}
}

void main()
{
	std::thread thread_hello(hello);
	std::thread thread_world(world);

	std::cin.get();	//ожидает нажатие на 'Enter'
	stop = true;
		
	thread_hello.join();
	thread_world.join();
}