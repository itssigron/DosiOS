#ifndef ERRORS_H
#define ERRORS_H

#include "Drivers/terminal.h"

typedef enum _ERRORS_ENUM {

  NO_ERROR = 0x0,

  ERROR_NOT_ENOUGH_SPACE,
  ERROR_NOT_ENOUGH_DISK_SPACE,
  ERROR_FILE_TOO_LARGE,
  ERROR_PATH_INCORRECT,
  ERROR_FILE_NO_ENTRY,

} error_t;


void panic(int error);

#endif

