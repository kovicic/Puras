#ifndef __INPUT_H__
#define __INPUT_H__

#include "dashboard.h"  //!< Dashboard_t

#define LOG_FILE "20121011_bmw_stopngo_5min.raw"

//!< Function prototypes

void* inputMain(void *);
int  canParseBMW(struct can_frame *, dashboard_t *);

#endif /* __INPUT_H__ */
