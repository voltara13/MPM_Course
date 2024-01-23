#include <iostream>
#include <thread>
#include <random>
#include <queue>
#include <stack>

#include "Versioned.h"

template<typename T>
void ThreadFunction(Versioned<T>& iVersioned, std::function<void(Versioned<T>&, int)> iAction)
{
	std::uniform_int_distribution<int> distribution(1, 1000);
	std::mt19937 random_number_engine(std::random_device{}());
	auto roller = std::bind(distribution, random_number_engine);

	for (int i = 0; i < 5; ++i)
	{
		const auto ms = roller();
		const auto a = std::this_thread::get_id();
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		std::cout << ms << " " << a << "\n";

		iAction(iVersioned, ms);
	}
}

template<typename T>
void RunExample(std::function<void(Versioned<T>&, int)> iAction, std::function<void(Versioned<T>&)> iPrintFunction)
{
	Versioned<T> example;

	std::thread thr(ThreadFunction<T>, std::ref(example), iAction);
	std::thread thr2(ThreadFunction<T>, std::ref(example), iAction);
	thr.join();
	thr2.join();

	iPrintFunction(example);
}

int main()
{
	RunExample<int>(
		[](Versioned<int>& iVersioned, int ms) { 
			iVersioned = ms; 
		}, 
		[](Versioned<int>& iVersioned) {
			std::cout << *iVersioned << "\n"; 
		});

	RunExample<std::vector<int>>(
		[](Versioned<std::vector<int>>& iVersioned, int ms) {
			auto temp = (*iVersioned);
			temp.push_back(ms);
			iVersioned = temp;
		},
		[](Versioned<std::vector<int>>& iVersioned) {
			for (const auto& elm : *iVersioned) {
				std::cout << elm << " ";
			}
			std::cout << "\n";
		});

	RunExample<std::queue<int>>(
		[](Versioned<std::queue<int>>& iVersioned, int ms) {
			auto temp = (*iVersioned);
			temp.push(ms);
			iVersioned = temp;
		},
		[](Versioned<std::queue<int>>& iVersioned) {
			auto queue = (*iVersioned);

			while (!queue.empty()) {
				std::cout << queue.front() << " ";
				queue.pop();
			}
			std::cout << "\n";
		});

	RunExample<std::stack<int>>(
		[](Versioned<std::stack<int>>& iVersioned, int ms) {
			auto temp = (*iVersioned);
			temp.push(ms);
			iVersioned = temp;
		},
		[](Versioned<std::stack<int>>& iVersioned) {
			auto stack = (*iVersioned);

			while (!stack.empty()) {
				std::cout << stack.top() << " ";
				stack.pop();
			}
			std::cout << "\n";
	});

	return 0;
}