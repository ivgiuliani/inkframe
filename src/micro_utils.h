#ifndef __MICROUTIL_H__
#define __MICROUTIL_H__

#define Q(x) #x
#define QUOTE(x) Q(x)

// The default `yield` method does not allow to specify a time period after
// which to regain control.
#define YIELD(ms) (vTaskDelay(ms / portTICK_PERIOD_MS))

#endif // __MICROUTIL_H__
