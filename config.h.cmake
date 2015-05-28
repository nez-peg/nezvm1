#ifndef CONFIG_H_CMAKE
#define CONFIG_H_CMAKE

/* Define to 1 if you have the <stdbool.h> header file. */
#cmakedefine HAVE_STDBOOL_H 1

/* build revision */
#cmakedefine NEZVM_REVISION  "${NEZVM_REVISION}"

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT ${PACKAGE_BUGREPORT}

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME "${PACKAGE_NAME}"

/* Define to the full name and version of this package. */
#cmakedefine PACKAGE_STRING "${PACKAGE_STRING}"

/* Define to the home page for this package. */
#cmakedefine PACKAGE_URL "${PACKAGE_URL}"

/* Define to the version of this package. */
#cmakedefine PACKAGE_VERSION "${PACKAGE_VERSION}"

/* Define to the full path of this build dir. */
#cmakedefine PACKAGE_BUILD_DIR "${PACKAGE_BUILD_DIR}"

/* The size of `int', as computed by sizeof. */
#cmakedefine SIZEOF_INT ${SIZEOF_INT}

/* The size of `long', as computed by sizeof. */
#cmakedefine SIZEOF_LONG ${SIZEOF_LONG}

/* The size of `void*', as computed by sizeof. */
#cmakedefine SIZEOF_VOIDP ${SIZEOF_VOIDP}

/* Define to 1 if you have the `posix_memalign' function. */
#cmakedefine HAVE_POSIX_MEMALIGN 1

/* Define to 1 if you have the `memalign' function. */
#cmakedefine HAVE_MEMALIGN 1

/* Define to 1 if you have the `__builtin_ctzl' function. */
#cmakedefine HAVE_BUILTIN_CTZL 1

/* Define to 1 if you have the `bzero' function. */
#cmakedefine HAVE_BZERO 1 

#endif /* end of include guard */
