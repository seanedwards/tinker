#include <functional>
#include <future>
#include <iostream>

/* An idea for continuing computation with C++11 futures.
 * Compile with: clang++ -std=c++11 -stdlib=libc++ future.cpp
 */

int add_one(int x) {
	return x + 1;
}

template<typename T, typename F>
auto operator<<(std::future<T> fut, F func) -> decltype(std::async(func, fut.get())) {
	return std::async(func, fut.get());
}

int main() {
	std::cout << (std::async(add_one, 1) << add_one << add_one).get() << std::endl;
	return 0;
}


