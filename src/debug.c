#include "debug.h"

u8 txt_scrolly = 8;

void dputs(const char* str) {
  se_puts(16, txt_scrolly, str, 0);
  txt_scrolly -= 8;
  REG_BG0VOFS = txt_scrolly - 8;
}
