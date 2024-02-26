#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
	//From raylib.h. Hopefully that ^ means the redefinition is fine
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif

bool isPastTime(long long inputMilliseconds);

#endif