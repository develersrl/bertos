#ifndef I2S_H
#define I2S_H

#include <cfg/compiler.h>

#define CONFIG_PLAY_BUF_LEN 16
#define I2S_FIRST_BUF  1
#define I2S_SECOND_BUF 2

void i2s_init(void);

/* Low level call that returns one of the two buffers or NULL if none is available */
uint8_t *i2s_getBuffer(unsigned buf_num);

/* Returns a buffer that will be played after the current one. Blocking call */
uint8_t *i2s_getFreeBuffer(void);

/* Starts playing from I2S_FIRST_BUFFER. You must have filled both buffers before calling this
 * function. Does nothing if already playing. */
bool i2s_start(void);

#endif /* I2S_H */
