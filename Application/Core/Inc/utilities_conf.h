// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "cmsis_compiler.h"
#include "stm32_mem.h"              // definitions to be provided to "sequencer" utility
#include "stm32_tiny_vsnprintf.h"   // definition and callback for tiny_vsnprintf

// Constants

#define VLEVEL_OFF    0  /*!< used to set UTIL_ADV_TRACE_SetVerboseLevel() (not as message param) */
#define VLEVEL_ALWAYS 0  /*!< used as message params, if this level is given
                              trace will be printed even when UTIL_ADV_TRACE_SetVerboseLevel(OFF) */
#define VLEVEL_L 1       /*!< just essential traces */
#define VLEVEL_M 2       /*!< functional traces */
#define VLEVEL_H 3       /*!< all traces */

#define TS_OFF 0         /*!< Log without TimeStamp */
#define TS_ON 1          /*!< Log with TimeStamp */

#define T_REG_OFF  0     /*!< Log without bitmask */

// Memory placement macro
#if defined(__CC_ARM)
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__), zero_init))
#elif defined(__ICCARM__)
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
#else  /* __GNUC__ */
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
#endif

// Memory alignment macro
#undef ALIGN
#ifdef WIN32
#define ALIGN(n)
#else
#define ALIGN(n)             __attribute__((aligned(n)))
#endif

// macro used to initialize the critical section
#define UTIL_SEQ_INIT_CRITICAL_SECTION( )    UTILS_INIT_CRITICAL_SECTION()

// macro used to enter the critical section
#define UTIL_SEQ_ENTER_CRITICAL_SECTION( )   UTILS_ENTER_CRITICAL_SECTION()

// macro used to exit the critical section
#define UTIL_SEQ_EXIT_CRITICAL_SECTION( )    UTILS_EXIT_CRITICAL_SECTION()

// Memset utilities interface to application
#define UTIL_SEQ_MEMSET8( dest, value, size )   UTIL_MEM_set_8( dest, value, size )

// macro used to initialize the critical section
#define UTILS_INIT_CRITICAL_SECTION()

// macro used to enter the critical section
#define UTILS_ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
  __disable_irq()

// macro used to exit the critical section
#define UTILS_EXIT_CRITICAL_SECTION()  __set_PRIMASK(primask_bit)

// ******************************************************************************
// trace\advanced
// the define option
//    UTIL_ADV_TRACE_CONDITIONNAL shall be defined if you want use conditional function
//    UTIL_ADV_TRACE_UNCHUNK_MODE shall be defined if you want use the unchunk mode
// ******************************************************************************

#define UTIL_ADV_TRACE_CONDITIONNAL                                                      /*!< not used */
#define UTIL_ADV_TRACE_UNCHUNK_MODE                                                      /*!< not used */
#define UTIL_ADV_TRACE_DEBUG(...)                                                        /*!< not used */
#define UTIL_ADV_TRACE_INIT_CRITICAL_SECTION( )    UTILS_INIT_CRITICAL_SECTION()         /*!< init the critical section in trace feature */
#define UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION( )   UTILS_ENTER_CRITICAL_SECTION()        /*!< enter the critical section in trace feature */
#define UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION( )    UTILS_EXIT_CRITICAL_SECTION()         /*!< exit the critical section in trace feature */
#define UTIL_ADV_TRACE_TMP_BUF_SIZE                (512U)                                /*!< default trace buffer size */
#define UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE      (15U)                                 /*!< default trace timestamp size */
#define UTIL_ADV_TRACE_FIFO_SIZE                   (1024U)                               /*!< default trace fifo size */
#define UTIL_ADV_TRACE_MEMSET8( dest, value, size) UTIL_MEM_set_8((dest),(value),(size)) /*!< memset utilities interface to trace feature */
#define UTIL_ADV_TRACE_VSNPRINTF(...)              tiny_vsnprintf_like(__VA_ARGS__)      /*!< vsnprintf utilities interface to trace feature */

