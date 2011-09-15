///
/// Author: Xavier Ho (contact@xavierho.com)
///
#pragma once
#ifdef _WIN32
  #include "windows/pthread.h"
#else
  #include <pthread.h>
#endif
#include <iostream>

class Threading;

/// object to be passed to pthread_create.
struct ThreadingHelper {
  Threading * obj;
  int index;
};

/// Provides a simple interface for C++ classes to do threading with pthread.
/// This class does not provide thread synchronisation, as it is the programmer
/// who is resonsible for doing so inside thread_action().
class Threading
{
  /// Tracks the thread ids
  pthread_t * threads;
  ThreadingHelper * helpers;

protected:
  /// Number of threads currently running.  Do NOT directly modify this value.
  int thread_count;

public:
  Threading()
    : thread_count(0) {}
  virtual ~Threading() {}

protected:
  /// Code to be run inside each thread.  
  /// Override this method.
  virtual void thread_action(int index) = 0;

  /// Starts the threads.
  /// You can spawn identical workers.  They come with an identifying index, 
  /// with the first child thread being 0, the second being 1, and so on.
  bool threads_start(int count) {
    thread_count = count;
    threads = new pthread_t[count];
    helpers = new ThreadingHelper[count];
    for (int i = 0; i < count; ++i) {
      helpers[i].obj = this;
      helpers[i].index = i;
      if (pthread_create(&threads[i], NULL, __run_thread,(void *)&helpers[i]) != 0) {
        std::cerr << "Threading failed to create a thread." << std::endl;
        return false;
      }
    }
    return true;
  }

  /// Waits for all of the threads to finish, blocking.
  void threads_wait() {
    for (int i = 0; i < thread_count; ++i) {
      if (pthread_join(threads[i], NULL) != 0) {
        std::cerr << "Threading failed to join a thread." << std::endl;
      }
    }
    delete[] helpers;
    delete[] threads;
  }

private:
  /// Hacky code that runs the thread and evokes thread_action.
  static void * __run_thread(void * helper) {
    ThreadingHelper * __helper = (ThreadingHelper *) helper;
    __helper->obj->__thread_action((void *)&__helper->index);
    pthread_exit(0);
    return NULL;
  }

  void __thread_action(void * index) {
    this->thread_action(*(int *)index);
  }
};

