#include "debug.h"

u8 txt_scrolly = 8;

void dputs(const char* str) {
  SBB_CLEAR_ROW(31, (txt_scrolly/8-2)&31);
  SBB_CLEAR_ROW(31, txt_scrolly/8);
  se_puts(16, txt_scrolly, str, 0);
  txt_scrolly -= 8;
  REG_BG0VOFS = txt_scrolly - 8;
}

void dstatus(const char* str) {
  SBB_CLEAR_ROW(31, (txt_scrolly/8-2)&31);
  SBB_CLEAR_ROW(31, txt_scrolly/8);
  se_puts(16, txt_scrolly, str, 0);
  // txt_scrolly -= 8;
  REG_BG0VOFS = txt_scrolly - 8;
}
