#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <memory>

/* An idea for continuing computation with C++11 futures.
 * Compile with: clang++ -std=c++11 -stdlib=libc++ future.cpp
 */




template<typename T, typename F>
auto operator<<(std::future<T> fut, const F& func) -> std::future<decltype(func(fut.get()))> {
	// Return a new future (deferred launch, similar to buffering on cout, avoids extraneous threads)
	return std::async(std::launch::deferred, [func] (std::shared_future<T> fut) { return func(fut.get()); }, fut.share());
}

template<typename F>
auto operator<<(std::future<void> fut, const F& func) -> std::future<decltype(func())> {
	// Specialization for void futures.
	return std::async(std::launch::deferred, [func] (std::shared_future<void> fut) { fut.get(); return func(); }, fut.share());
}







struct FutureGetter {};
FutureGetter get;

template<typename T>
auto operator<<(std::future<T> fut, const FutureGetter&) -> decltype(fut.get()) {
	// Blocks the current thread on the previous future(s).
	return fut.get();
}







struct FutureBackgrounder {};
FutureBackgrounder background;

template<typename T>
std::future<T> operator<<(std::future<T> fut, const FutureBackgrounder&) {
	// Spawns a thread that will execute the previous future(s).
	// Can't use std::async here, since std::future's destructure will block.
	std::shared_ptr<std::promise<T>> promise(new std::promise<T>()) ;
	std::thread([promise] (std::shared_future<T> fut) {
		promise->set_value(fut.get());
	}, fut.share()).detach();
	return promise->get_future();
}

std::future<void> operator<<(std::future<void> fut, const FutureBackgrounder&) {
	std::shared_ptr<std::promise<void>> promise(new std::promise<void>()) ;
	std::thread([promise] (std::shared_future<void> fut) {
		fut.get();
		promise->set_value();
	}, fut.share()).detach();
	return promise->get_future();
}









/* ==================================================== */
/* ============ TEST CODE BELOW THIS POINT ============ */
/* ==================================================== */




// A long-running, blocking I/O call that we wouldn't want to stick in our imaginary UI loop.
int get_int() {
	int x;
	std::chrono::milliseconds dura( 2000 );
	std::this_thread::sleep_for( dura );

	std::cin.clear();
	std::cin >> x;                             // sets failbit if invalid
	std::cin.exceptions (std::ios::failbit);   // throw on failbit set
	return x;
}

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

int add(int x, int y) {
	return x + y;
}





int main() {
	using namespace std::chrono;
	auto t1 = high_resolution_clock::now();

	std::cout << "Enter a number: " << std::flush;
	// Prints the typed number + 2
	std::future<void> result = std::async(std::launch::async, get_int) 
		// When that's done, add two to the result:
		<< std::bind(add, std::placeholders::_1, 2)
		// When that's done, convert it to a string:
		<< to_string 
		// When that's done, format the string for output:
		<< format_result 
		// When that's done, send it to the terminal:
		<< println
		// And a nice goodbye message:
		<< goodbye
		// And block the whole thing on a background thread.
		<< background;

	auto t2 = high_resolution_clock::now();
	std::cout << "Finished main() in " << duration_cast<duration<double>>(t2 - t1).count() << " seconds." << std::endl;

	// Block main() on the result, otherwise the process will just terminate.
	result.get();

	return 0;
}


