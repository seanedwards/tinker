#include <functional>
#include <future>
#include <iostream>
#include <sstream>

/* An idea for continuing computation with C++11 futures.
 * Compile with: clang++ -std=c++11 -stdlib=libc++ future.cpp
 */

void println(const std::string& str) {
	std::cout << str << std::endl;
}

void goodbye() {
	println("Goodbye.");
}

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

int get_int() {
	std::cin.exceptions (std::ios::failbit);   // throw on failbit set
	int x;
	std::cin >> x;                             // sets failbit if invalid
	return x;
}

template<typename T, typename F>
auto operator<<(std::future<T> fut, const F& func) -> std::future<decltype(func(fut.get()))> {
	return std::async([func] (std::shared_future<T> fut) { return func(fut.get()); }, fut.share());
}

template<typename F>
auto operator<<(std::future<void> fut, const F& func) -> std::future<decltype(func())> {
	return std::async([func] (std::shared_future<void> fut) { fut.get(); return func(); }, fut.share());
}


int main() {
	// Prints the typed number + 2
	std::async(println, "Please enter a number: ") << get_int << add_one << add_one << to_string << format_result << println << goodbye;
	return 0;
}


