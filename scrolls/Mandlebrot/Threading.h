///
/// Author: Xavier Ho (contact@xavierho.com)
///
#pragma once
#ifdef _WIN32
  #include "../include/pthread.h"
#else
  #include <pthread.h>
#endif
#include <vector>
#include <iostream>

class Threading;

struct ThreadingHelper {
  Threading * obj;
  void * args;
};

/// Provides a simple interface for C++ classes to do threading with pthread.
class Threading
{
  /// Tracks the thread ids
  std::vector<pthread_t> threads;
  std::vector<ThreadingHelper> helpers;

public:
  Threading() : threads() {}
  virtual ~Threading() {}

  /// Starts the threads.
  /// You can spawn identical workers, or set them up appropriately with
  /// different inputs.  If one thread fails to run, it stops and returns
  /// false.  See setup_arguments().
  bool threads_start(int count) {
    helpers.reserve(count);
    threads.reserve(count);
    for (int i = 0; i < count; ++i) {
      threads.push_back(pthread_t());
      helpers.push_back(setup_arguments(i));
      // std::cout << "[Main] Helper located at: " << &helpers[i] << "\tPointing to object " << helpers[i].obj << std::endl;
      if (pthread_create(&threads[i], NULL, __run_thread, (void *)&helpers[i]) != 0) {
        std::cerr << "Threading failed to create a thread." << std::endl;
        return false;
      }
    }
    return true;
  }

  /// Waits for all of the threads to finish, blocking.
  bool threads_wait() {
    for (int i = 0; i < (int)threads.size(); ++i) {
      if (pthread_join(threads[i], NULL) != 0) {
        std::cerr << "Threading failed to join a thread." << std::endl;
        return false; 
      }
      cleanup_arguments(&helpers[i]);
    }
    helpers.clear();
    threads.clear();
    return true;
  }

protected:
  /// Code to be run inside each thread.  Override this method.
  virtual void thread_action(void * args) = 0;

  /// Sets up the arguments to be passed into each thread.  Here an index is
  /// passed into the function on calling for convenience.  Override this
  /// method. Set up ThreadingHelper's members: args appropriately by the index
  /// and obj must point to the instance, by copying 'this' pointer.
  virtual ThreadingHelper setup_arguments(int thread_index) = 0;

  /// Similarly, if you claimed any resources for ThreadingHelper, free them.
  virtual void cleanup_arguments(ThreadingHelper * helper) = 0;

private:
  /// Hacky code that runs the thread and evokes thread_action.
  static void * __run_thread(void * helper) {
    ThreadingHelper * __helper = (ThreadingHelper *) helper;
    // std::cout << "[Child] Helper located at: " << __helper << "\tPointing to object " << __helper->obj << std::endl;
    __helper->obj->thread_action(__helper->args);
    pthread_exit(0);
    return NULL;
  }
};

