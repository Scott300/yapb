//
// Yet Another POD-Bot, based on PODBot by Markus Klinge ("CountFloyd").
// Copyright (c) YaPB Development Team.
//
// This software is licensed under the BSD-style license.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     http://yapb.jeefo.net/license
//

#pragma once

#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#include <corelib.h>

// detects the build platform
#if defined (__linux__) || defined (__debian__) || defined (__linux)
   #define PLATFORM_LINUX 1
#elif defined (__APPLE__)
   #define PLATFORM_OSX 1
#elif defined (_WIN32)
   #define PLATFORM_WIN32 1
#endif

// detects the compiler
#if defined (_MSC_VER)
   #define COMPILER_VISUALC _MSC_VER
#elif defined (__MINGW32_MAJOR_VERSION)
   #define COMPILER_MINGW32 __MINGW32_MAJOR_VERSION
#endif

// configure export macros
#if defined (COMPILER_VISUALC) || defined (COMPILER_MINGW32)
   #define SHARED_LIBRARAY_EXPORT extern "C" __declspec (dllexport)
#elif defined (PLATFORM_LINUX) || defined (PLATFORM_OSX) 
   #define SHARED_LIBRARAY_EXPORT extern "C" __attribute__((visibility("default")))
#else
   #error "Can't configure export macros. Compiler unrecognized."
#endif

// operating system specific macros, functions and typedefs
#ifdef PLATFORM_WIN32

   #include <direct.h>
   #include <string.h>

   // exclude windows bloatware
   #define WIN32_LEAN_AND_MEAN
   #define NOWINRES
   #define NOSERVICE
   #define NOMCX
   #define NOIME

   #include "windows.h"
   #include "winsock2.h"

   #define DLL_ENTRYPOINT int __stdcall DllMain (HINSTANCE, DWORD reason, LPVOID)
   #define DLL_DETACHING (reason == DLL_PROCESS_DETACH)
   #define DLL_RETENTRY return 1

   #define STDCALL __stdcall

   #if defined (COMPILER_VISUALC)
      #define DLL_GIVEFNPTRSTODLL extern "C" void STDCALL
   #elif defined (COMPILER_MINGW32)
      #define DLL_GIVEFNPTRSTODLL SHARED_LIBRARAY_EXPORT void STDCALL
   #endif

   // specify export parameter
   #if defined (COMPILER_VISUALC) && (COMPILER_VISUALC > 1000)
      #pragma comment (linker, "/EXPORT:GiveFnptrsToDll=_GiveFnptrsToDll@8,@1")
      #pragma comment (linker, "/SECTION:.data,RW")
   #endif

#elif defined (PLATFORM_LINUX) || defined (PLATFORM_OSX)

   #include <unistd.h>
   #include <dlfcn.h>
   #include <limits.h>
   #include <stdarg.h>
   #include <errno.h>
   #include <fcntl.h>
   #include <sys/stat.h>

   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h>
   #include <arpa/inet.h>

   #define DLL_ENTRYPOINT __attribute__((destructor))  void _fini (void)
   #define DLL_DETACHING 1
   #define DLL_RETENTRY return
   #define DLL_GIVEFNPTRSTODLL extern "C" void __attribute__((visibility("default")))

   #if defined (__ANDROID__)
      #define PLATFORM_ANDROID 1
   #endif
#else
   #error "Platform unrecognized."
#endif

// library wrapper
class Library
{
private:
   void *m_ptr;

public:

   Library (const char *fileName)
   {
      m_ptr = nullptr;

      if (fileName == nullptr)
         return;

      LoadLib (fileName);
   }

   ~Library (void)
   {
      if (!IsLoaded ())
         return;

#ifdef PLATFORM_WIN32
      FreeLibrary (reinterpret_cast <HMODULE> (m_ptr));
#else
      dlclose (m_ptr);
#endif
   }

public:
   inline void *LoadLib (const char *fileName)
   {
#ifdef PLATFORM_WIN32
      m_ptr = LoadLibrary (fileName);
#else
      m_ptr = dlopen (fileName, RTLD_NOW);
#endif

      return m_ptr;
   }

   template <typename R> R GetFuncAddr (const char *function)
   {
      if (!IsLoaded ())
         return nullptr;

#ifdef PLATFORM_WIN32
      return reinterpret_cast <R> (GetProcAddress (static_cast <HMODULE> (m_ptr), function));
#else
      return reinterpret_cast <R> (dlsym (m_ptr, function));
#endif
   }

   template <typename R> R GetHandle (void)
   {
      return (R) m_ptr;
   }

   inline bool IsLoaded (void) const
   {
      return m_ptr != nullptr;
   }
};

#define BOT_MEMORY_FAILURE() AddLogEntry (true, LL_FATAL, "Memory Allocation Failed!\nFile: %s (Line: %d)", __FILE__, __LINE__)
#define BOT_ASSERT(expr) if (!(expr)) { AddLogEntry (true, LL_FATAL, "Assertion Failed! (Expression: %s, File: %s, Line: %d)", #expr, __FILE__, __LINE__); }

#include <goldsrc.h>

