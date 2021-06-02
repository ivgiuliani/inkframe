#ifndef __MICROUTIL_H__
#define __MICROUTIL_H__

#define Q(x) #x
#define QUOTE(x) Q(x)

#if defined(__GNUC__)
#  if !defined(__FORCE_INLINE)
#    define __FORCE_INLINE __attribute__((always_inline))
#  else
#    define __FORCE_INLINE inline
#  endif
#  if !defined(__FORCE_NO_INLINE)
#    define __FORCE_NO_INLINE __attribute__((noinline))
#  else
#    define __FORCE_NO_INLINE
#  endif
#endif

// The default `yield` method does not allow to specify a time period after
// which to regain control.
#define YIELD(ms) (vTaskDelay(ms / portTICK_PERIOD_MS))

#endif // __MICROUTIL_H__
