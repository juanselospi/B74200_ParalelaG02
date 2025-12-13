/* Recursive mutex example
 * A recursive mutex is a lockable object, just like mutex, but allows the same thread
 * to acquire multiple levels of ownership over the mutex object.
 *
 * This allows to lock (or try-lock) the mutex object from a thread that is already
 * locking it, acquiring a new level of ownership over the mutex object: the mutex
 * object will actually remain locked owning the thread until its member unlock is
 * called as many times as this level of ownership.
 */

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

std::recursive_mutex mtx;	// Recursive mutex example

int calls = 0;		// Count calls to a function
//std::atomic_ushort acalls = 0;

int factorial( int n ) {
   int result;

   mtx.lock();	// Same thread can acquire lock many times, other threads must wait
   std::cout << "Thread " << std::this_thread::get_id() << std::endl;
   calls++;
   if ( n <= 1 ) {
      mtx.unlock();	// For every lock it is necessary one unlock
      return 1;
   } else {
      result = n * factorial( n - 1 );
      mtx.unlock();
      return result;
   }

}

int main () {
  std::thread th1( factorial, 5 );
  std::thread th2( factorial, 6 );

  th1.join();
  th2.join();

  std::cout << "Total calls " << calls << std::endl;

  return 0;

}
