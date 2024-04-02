#include "errors.h"

void panic(int error) {
 
  switch (error) {
    
    case NO_ERROR: return;

    case ERROR_NOT_ENOUGH_SPACE:      terminal_display_error("Not enough space in RAM :(\n");  break;
    case ERROR_NOT_ENOUGH_DISK_SPACE: terminal_display_error("Not enough space in disk :(\n"); break;
    case ERROR_FILE_TOO_LARGE:        terminal_display_error("File too big bruh\n");           break;
    case ERROR_PATH_INCORRECT:        terminal_display_error("Path is invalid\n");             break;
    case ERROR_FILE_NO_ENTRY:         terminal_display_error("File does not contain an entry point\n"); break;
  }

  for(;;) {}
}

