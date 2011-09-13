///
/// Author: Xavier Ho (contact@xavierho.com)
///
#pragma once
#ifdef _WIN32
  #include "../include/pthread.h"
#else
  #include <pthread>
#endif
#include <vector>
#include <iostream>

/// Provides a simple interface for C++ classes to do threading with pthread.
class Threading
{
protected:
  std::vector<pthread_t> threads;

public:
  Threading() : threads() {}
  virtual ~Threading() {}

  bool threads_start(int count) {
    for (int i = 0; i < count; ++i) {
      threads.push_back(pthread_t());
      if (pthread_create(&threads[i], NULL, __run_thread, this) != 0) {
        std::cerr << "Threading failed to create a thread." << std::endl;
        return false;
      }
    }
    return true;
  }

  bool threads_wait() {
    for (int i = 0; i < (int)threads.size(); ++i) {
      if (pthread_join(threads[i], NULL) != 0) {
        std::cerr << "Threading failed to join a thread." << std::endl;
        return false; 
      }
    }
    threads.clear();
    return true;
  }

protected:
  virtual void thread_action() = 0;

private:
  static void * __run_thread(void * obj) {
    ((Threading *) obj)->thread_action();
    pthread_exit(0);
    return NULL;
  }
};