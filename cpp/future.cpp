#include <functional>
#include <future>
#include <iostream>
#include <sstream>

/* An idea for continuing computation with C++11 futures.
 * Compile with: clang++ -std=c++11 -stdlib=libc++ future.cpp
 */

std::string format_result(const std::string& str) {
	return "Result is: " + str;
}

std::string to_string(int x) {
	std::stringstream str;
	str << x;
	return str.str();
}

int add_one(int x) {
	return x + 1;
}

template<typename T, typename F>
auto operator<<(std::future<T> fut, F func) -> std::future<decltype(func(fut.get()))> {
	return std::async([func] (std::shared_future<T> fut) { return func(fut.get()); }, fut.share());
}

int main() {
	std::cout << (std::async(add_one, 1) << add_one << add_one << to_string << format_result).get() << std::endl;
	return 0;
}


