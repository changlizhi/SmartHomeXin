#ifndef _PARAM_H
#define _PARAM_H

#if 1
#define FILEINDEX_TERM            0
#define FILEINDEX_UNI            1
#define FILEINDEX_SCENE            3
#define FILEINDEX_TIMERTASK            4

#define FILEINDEX_MAX            4

#define SAVFLAG_PARATERM    ((unsigned char)1<<FILEINDEX_TERM)
#define SAVFLAG_PARAUNI      ((unsigned char)1<<FILEINDEX_UNI)
#define SAVFLAG_SCENE        ((unsigned char)1<<FILEINDEX_SCENE)
#define SAVFLAG_TIMERTASK      ((unsigned char)1<<FILEINDEX_TIMERTASK)

#define SAVEFLAG_RUNSTATE            ((unsigned int)0x80000000)
#endif

#include "itemop.h"
#include "para_term.h"
#include "para_uni.h"
#endif

