#include <iostream>
#include <thread>
#include <random>
#include <queue>
#include <stack>
#include <future>

#include "Versioned.h"

template<typename T>
void ThreadFunction(Versioned<T>& iVersioned, std::function<void(Versioned<T>&, int)> iAction, T iInitValue, std::promise<Revision*>&& oRevision)
{
	iVersioned = iInitValue;

	std::uniform_int_distribution<int> distribution(1, 1000);
	std::mt19937 random_number_engine(std::random_device{}());
	auto roller = std::bind(distribution, random_number_engine);

	for (int i = 0; i < 5; ++i) {
		const auto ms = roller();
		const auto id = std::this_thread::get_id();
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		std::cout << "Thread id: " << id << "; Slept ms: " << ms << "\n";

		iAction(iVersioned, ms);
	}

	oRevision.set_value(Revision::pCurrentRevision);
}

template<typename T>
void RunExample(std::function<void(Versioned<T>&, int)> iAction, std::function<void(Versioned<T>&)> iPrintFunction)
{
	Versioned<T> example;

	std::promise<Revision*> promise1;
	std::promise<Revision*> promise2;

	auto future1 = promise1.get_future();
	auto future2 = promise2.get_future();

	std::thread thread1(ThreadFunction<T>, std::ref(example), iAction, *example, std::move(promise1));
	std::thread thread2(ThreadFunction<T>, std::ref(example), iAction, *example, std::move(promise2));

	thread1.join();
	thread2.join();

	auto fork1 = future1.get();
	auto fork2 = future2.get();

	Revision::pCurrentRevision->Join(fork1);
	std::cout << "Join 1st thread result:\n";
	iPrintFunction(example);

	Revision::pCurrentRevision->Join(fork2);
	std::cout << "Join 2nd thread result:\n";
	iPrintFunction(example);

	std::cout << std::endl;
}

int main()
{
	RunExample<int>(
		[](Versioned<int>& iVersioned, int ms) {
			auto fork = Revision::pCurrentRevision->Fork([&]() {
				iVersioned = ms; 
			});

			Revision::pCurrentRevision->Join(fork);
		}, 
		[](Versioned<int>& iVersioned) {
			std::cout << *iVersioned << std::endl;
	});

	RunExample<std::vector<int>>(
		[](Versioned<std::vector<int>>& iVersioned, int ms) {
			auto fork = Revision::pCurrentRevision->Fork([&]() { 
				auto temp = (*iVersioned);
				temp.push_back(ms);
				iVersioned = temp;
			});

			Revision::pCurrentRevision->Join(fork);
		},
		[](Versioned<std::vector<int>>& iVersioned) {
			for (const auto& elm : *iVersioned) {
				std::cout << elm << " ";
			}
			std::cout << std::endl;
		});

	RunExample<std::queue<int>>(
		[](Versioned<std::queue<int>>& iVersioned, int ms) {
			auto fork = Revision::pCurrentRevision->Fork([&]() {
				auto temp = (*iVersioned);
				temp.push(ms);
				iVersioned = temp;
			});

			Revision::pCurrentRevision->Join(fork);
		},
		[](Versioned<std::queue<int>>& iVersioned) {
			auto queue = (*iVersioned);

			while (!queue.empty()) {
				std::cout << queue.front() << " ";
				queue.pop();
			}
			std::cout << std::endl;
		});

	RunExample<std::stack<int>>(
		[](Versioned<std::stack<int>>& iVersioned, int ms) {
			auto fork = Revision::pCurrentRevision->Fork([&]() {
				auto temp = (*iVersioned);
				temp.push(ms);
				iVersioned = temp;
			});

			Revision::pCurrentRevision->Join(fork);
		},
		[](Versioned<std::stack<int>>& iVersioned) {
			auto stack = (*iVersioned);

			while (!stack.empty()) {
				std::cout << stack.top() << " ";
				stack.pop();
			}
			std::cout << std::endl;
	});

	return 0;
}