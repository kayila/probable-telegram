#ifndef DISPATCH_KLUDGE_BUCKET_HPP
#define DISPATCH_KLUDGE_BUCKET_HPP

#ifdef __GLIBC__
    // HACK: The C++ standard library used by g++ and clang++ relies on
    // GNU extensions. As such, it automatically defines _GNU_SOURCE.
    // And undefining it breaks the standard library.
    // Thus, I have to do this garbage.
    extern "C" {
        extern int __xpg_strerror_r(int errcode,char* buffer,size_t length);
    }
#   define posix_strerror_r __xpg_strerror_r
#else
#   define posix_strerror_r strerror_r
#endif

#endif
