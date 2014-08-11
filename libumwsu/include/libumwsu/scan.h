#ifndef _LIBUMWSU_SCAN_H_
#define _LIBUMWSU_SCAN_H_

#include <libumwsu/status.h>

struct umwsu;

struct umwsu *umwsu_open(void);

void umwsu_print(struct umwsu *u);

void umwsu_close(struct umwsu *u);

enum umwsu_status umwsu_scan_file(struct umwsu *umwsu_handle, const char *path);

enum umwsu_status umwsu_scan_dir(struct umwsu *umwsu_handle, const char *path, int recurse);

#endif
