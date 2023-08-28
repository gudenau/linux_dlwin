#ifndef __dlwin_misc__
#define __dlwin_misc__

#ifdef __dlwin_build__
#define dlwin_export __attribute__ ((visibility ("default")))
#else
#define dlwin_export
#endif

#endif
