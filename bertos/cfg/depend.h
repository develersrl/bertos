#ifndef CFG_DEPEND_H
#define CFG_DEPEND_H

#include <cfg/compiler.h> // STATIC_ASSERT()
/**
 * Compile-time enforcement of dependencies between configuration options
 */
#define CONFIG_DEPEND(FEATURE, DEPS)  STATIC_ASSERT(!(FEATURE) || !!(DEPS))

#endif // CFG_DEPEND_H
