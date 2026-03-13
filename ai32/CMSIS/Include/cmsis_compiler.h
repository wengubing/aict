/**************************************************************************//**
 * @file     cmsis_compiler.h
 * @brief    CMSIS compiler generic header file
 * @version  V5.1.0
 * @date     09. October 2018
 ******************************************************************************/
#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/* ARMCC v5 */
#if defined ( __CC_ARM )
  #define __STATIC_INLINE         static __inline
  #define __STATIC_FORCEINLINE    static __forceinline
  #define __NO_RETURN             __declspec(noreturn)
  #define __USED                  __attribute__((used))
  #define __WEAK                  __attribute__((weak))
  #define __PACKED                __attribute__((packed))
  #define __PACKED_STRUCT         __packed struct
  #define __PACKED_UNION          __packed union
  #define __UNALIGNED_UINT16_WRITE(addr, val) (*((__packed uint16_t*)(addr)) = (val))
  #define __UNALIGNED_UINT16_READ(addr)       (*((__packed uint16_t*)(addr)))
  #define __UNALIGNED_UINT32_WRITE(addr, val) (*((__packed uint32_t*)(addr)) = (val))
  #define __UNALIGNED_UINT32_READ(addr)       (*((__packed uint32_t*)(addr)))
  #define __ALIGNED(x)            __attribute__((aligned(x)))
  #define __RESTRICT              __restrict
  #define __COMPILER_BARRIER()    __memory_changed()

/* ARMCLANG v6 */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __STATIC_INLINE         static __inline
  #define __STATIC_FORCEINLINE    __attribute__((always_inline)) static __inline
  #define __NO_RETURN             __attribute__((__noreturn__))
  #define __USED                  __attribute__((used))
  #define __WEAK                  __attribute__((weak))
  #define __PACKED                __attribute__((packed, aligned(1)))
  #define __PACKED_STRUCT         struct __attribute__((packed, aligned(1)))
  #define __PACKED_UNION          union  __attribute__((packed, aligned(1)))
  #define __UNALIGNED_UINT16_WRITE(addr, val)  (void)((((struct __attribute__((packed)) {uint16_t v;}*)(void*)(addr))->v) = (val))
  #define __UNALIGNED_UINT16_READ(addr)        (((const struct __attribute__((packed)) {uint16_t v;}*)(const void*)(addr))->v)
  #define __UNALIGNED_UINT32_WRITE(addr, val)  (void)((((struct __attribute__((packed)) {uint32_t v;}*)(void*)(addr))->v) = (val))
  #define __UNALIGNED_UINT32_READ(addr)        (((const struct __attribute__((packed)) {uint32_t v;}*)(const void*)(addr))->v)
  #define __ALIGNED(x)            __attribute__((aligned(x)))
  #define __RESTRICT              __restrict
  #define __COMPILER_BARRIER()    __asm volatile("":::"memory")

/* GCC */
#elif defined ( __GNUC__ )
  #define __STATIC_INLINE         static __inline
  #define __STATIC_FORCEINLINE    static __inline
  #define __NO_RETURN             __attribute__((__noreturn__))
  #define __USED                  __attribute__((used))
  #define __WEAK                  __attribute__((weak))
  #define __PACKED                __attribute__((packed, aligned(1)))
  #define __PACKED_STRUCT         struct __attribute__((packed, aligned(1)))
  #define __PACKED_UNION          union  __attribute__((packed, aligned(1)))
  #define __UNALIGNED_UINT16_WRITE(addr, val)  (void)((((struct __attribute__((packed)) {uint16_t v;}*)(void*)(addr))->v) = (val))
  #define __UNALIGNED_UINT16_READ(addr)        (((const struct __attribute__((packed)) {uint16_t v;}*)(const void*)(addr))->v)
  #define __UNALIGNED_UINT32_WRITE(addr, val)  (void)((((struct __attribute__((packed)) {uint32_t v;}*)(void*)(addr))->v) = (val))
  #define __UNALIGNED_UINT32_READ(addr)        (((const struct __attribute__((packed)) {uint32_t v;}*)(const void*)(addr))->v)
  #define __ALIGNED(x)            __attribute__((aligned(x)))
  #define __RESTRICT              __restrict
  #define __COMPILER_BARRIER()    __asm volatile("":::"memory")

/* IAR */
#elif defined ( __ICCARM__ )
  #define __STATIC_INLINE         static inline
  #define __STATIC_FORCEINLINE    static inline
  #define __NO_RETURN             __noreturn
  #define __USED                  __root
  #define __WEAK                  __weak
  #define __PACKED                __packed
  #define __PACKED_STRUCT         __packed struct
  #define __PACKED_UNION          __packed union
  #define __ALIGNED(x)            _Pragma("data_alignment=" #x)
  #define __RESTRICT              restrict
  #define __COMPILER_BARRIER()    __DMB()

/* fallback */
#else
  #define __STATIC_INLINE         static inline
  #define __STATIC_FORCEINLINE    static inline
  #define __NO_RETURN
  #define __USED
  #define __WEAK
  #define __PACKED
  #define __PACKED_STRUCT         struct
  #define __PACKED_UNION          union
  #define __ALIGNED(x)
  #define __RESTRICT
  #define __COMPILER_BARRIER()
#endif

#endif /* __CMSIS_COMPILER_H */
