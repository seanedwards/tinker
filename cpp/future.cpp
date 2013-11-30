#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>

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
std::thread operator<<(std::future<T> fut, const FutureBackgrounder&) {
	// Spawns a thread that will execute the previous future(s).
	return std::thread([] (std::shared_future<T> fut) {
		fut.get();
	}, fut.share());
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
	auto thread = std::async(std::launch::async, get_int) 
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

	// Unfortunately it seems this does not actually run asynchronously. See also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3451.pdf

	auto t2 = high_resolution_clock::now();
	std::cout << "Finished main() in " << duration_cast<duration<double>>(t2 - t1).count() << " seconds." << std::endl;

	thread.join();

	return 0;
}


