#include        <pin.h>
#include        <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif
void    *log_init();
void    log_increment(volatile unsigned char *data);

#ifdef __cplusplus
}
#endif
