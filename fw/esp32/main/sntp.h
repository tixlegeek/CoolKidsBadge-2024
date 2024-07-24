#ifndef _SNTP_H_
#define _SNTP_H_
#include <esp_sntp.h>
#include <sys/time.h>
#include <time.h>

void initialize_sntp(void);
void deinitialize_sntp(void);
char *get_current_date();

#endif /* end of include guard: _SNTP_H_ */
