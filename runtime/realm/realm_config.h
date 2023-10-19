/* Copyright 2023 Stanford University, NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// configuration settings that control how Realm is built
// this is expected to become an auto-generated file at some point

#ifndef REALM_CONFIG_H
#define REALM_CONFIG_H

// realm_defines.h is auto-generated by both the make and cmake builds
#include "realm_defines.h"

// get macros that spell things the right way for each compiler
#include "realm/compiler_support.h"

// Control the maximum number of dimensions for Realm
#ifndef REALM_MAX_DIM
#define REALM_MAX_DIM 3
#endif

// if set, uses ucontext.h for user level thread switching, otherwise falls
//  back to POSIX threads
// address sanitizer doesn't cope with makecontext/swapcontext either
#if !defined(REALM_USE_NATIVE_THREADS) && !defined(REALM_ON_MACOS) && !defined(ASAN_ENABLED)
// clang on Mac is generating apparently-broken code in the user thread
//  scheduler, so disable this code path for now
#define REALM_USE_USER_THREADS
#endif

// if set, uses Linux's kernel-level io_submit interface, otherwise uses
//  POSIX AIO for async file I/O
#ifdef REALM_ON_LINUX
//define REALM_USE_KERNEL_AIO
#define REALM_USE_LIBAIO
#endif
#if defined(REALM_ON_MACOS) || defined(REALM_ON_FREEBSD)
#define REALM_USE_LIBAIO
#endif

// dynamic loading via dlfcn and a not-completely standard dladdr extension
#ifdef REALM_USE_LIBDL
  #if defined(REALM_ON_LINUX) || defined(REALM_ON_MACOS) || defined(REALM_ON_FREEBSD)
    #define REALM_USE_DLFCN
    #define REALM_USE_DLADDR
  #endif
#endif

// can Realm use exceptions to propagate errors back to the profiling interace?
#define REALM_USE_EXCEPTIONS

// the Realm operation table is needed if you want to be able to cancel operations
#ifndef REALM_NO_USE_OPERATION_TABLE
#define REALM_USE_OPERATION_TABLE
#endif

// maximum size in bytes of Realm error messages
#ifndef REALM_ERROR_BUFFER_SIZE
#define REALM_ERROR_BUFFER_SIZE 1024
#endif

// ASAN has issues with thread local destructors, so disable this path for asan
// Temporarily disable the caching allocator until it is debugged
// TODO: reenable this
#define REALM_USE_CACHING_ALLOCATOR 0
#if !defined(REALM_USE_CACHING_ALLOCATOR) && !defined(ASAN_ENABLED)
  #define REALM_USE_CACHING_ALLOCATOR 1
  #ifndef REALM_TASK_BLOCK_SIZE
    #define REALM_TASK_BLOCK_SIZE (128ULL*1024ULL)
  #endif
#endif

#if defined(REALM_USE_SHM)
// Unfortunately, the use of shared memory requires a named resource on the system, which
// requires coordination of the name before the resources are allocated.  GASNET1 does not
// have the capability to communicate to other nodes before resource allocation is done,
// so we cannot support shared memory with it at this time
#if defined(REALM_USE_GASNET1)
#error Shared memory not supported on GASNET1
#endif
#if defined(REALM_HAS_MEMFD)
#define REALM_USE_ANONYMOUS_SHARED_MEMORY 1
#elif defined(REALM_ON_WINDOWS)
// ANONYMOUS_SHARED_MEMORY requires ipc mailbox support, which is not yet implemented on windows
// TODO: Support this on windows
//#define REALM_USE_ANONYMOUS_SHARED_MEMORY 1
#endif
#endif

#ifdef __cplusplus
// runtime configuration settings
namespace Realm {
  namespace Config {
    // if non-zero, eagerly checks deferred user event triggers for loops up to the
    //  specified limit
    extern int event_loop_detection_limit;

    // if true, worker threads that might have used user-level thread switching
    //  fall back to kernel threading
    extern bool force_kernel_threads;
    // Unique identifier for the job assigned to this instance of the machine.  Useful
    // when dealing with named system resources while running parallel jobs on the same
    // machine
    extern unsigned long long job_id;
  };
};
#endif

#endif
