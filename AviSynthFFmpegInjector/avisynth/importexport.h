#pragma once


#ifdef __cplusplus
#  define EXTERN_C extern "C"
#else
#  define EXTERN_C
#endif


#ifdef AVISYNTH_EXPORTS
#  define AVSI_API(ret, name) EXTERN_C __declspec(dllexport) ret __cdecl name
#else
#    define AVSI_API(ret, name) EXTERN_C __declspec(dllimport) ret __cdecl name
#endif

