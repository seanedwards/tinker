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
	int x;
	std::cin.clear();
	std::cin >> x;                             // sets failbit if invalid
	std::cin.exceptions (std::ios::failbit);   // throw on failbit set
	return x;
}

std::future<int> prompt_int(const std::string& prompt) {
	std::cout << prompt << ": " << std::flush;
	return std::async(get_int);
}

struct FutureGetter {};
FutureGetter get;

template<typename T>
auto operator<<(std::future<T> fut, const FutureGetter&) -> decltype(fut.get()) {
	return fut.get();
}

void operator<<(std::future<void>fut, const FutureGetter&) {
	fut.get();
}

template<typename T, typename F>
auto operator<<(std::future<T> fut, const F& func) -> std::future<decltype(func(fut.get()))> {
	return std::async(std::launch::deferred, [func] (std::shared_future<T> fut) { fut.wait(); return func(fut.get()); }, fut.share());
}

template<typename F>
auto operator<<(std::future<void> fut, const F& func) -> std::future<decltype(func())> {
	return std::async(std::launch::deferred, [func] (std::shared_future<void> fut) { fut.wait(); fut.get(); return func(); }, fut.share());
}


int main() {
	// Prints the typed number + 2
	prompt_int("Enter a number")
		<< add_one 
		<< add_one 
		<< to_string 
		<< format_result 
		<< println 
		<< goodbye
		<< get;
	return 0;
}


