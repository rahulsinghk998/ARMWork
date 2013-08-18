 /*
  * Wrapper for FatFs by ChaN
  */
  
#include "SDFatFs.h"

void SDFatFs::begin(void) {
  pinMode(pin_cs, OUTPUT);
  if ( pin_detect != NOT_A_PIN || pin_detect != PIN_NOT_DEFINED )
    pinMode(pin_detect, INPUT);
  if ( pin_busyled != NOT_A_PIN || pin_busyled != PIN_NOT_DEFINED )
    pinMode(pin_busyled, OUTPUT);
  f_mount(0, &fatfs);		/* Register volume work area (never fails) */
}
  
// for get_fattime
uint32_t SDFatFs::fattime(uint32_t cal, uint32_t time) {
  uint8_t y,m,d, hh, mm, ss;
  y = 20 + (cal>>16&0x0f) + (cal>>20&0x0f)*10;
  m = (cal>>8&0x0f) + (cal>>12&0x0f)*10;
  d = (cal&0x0f) + (cal>>4&0x0f)*10;
  hh = (time>>16&0x0f) + (time>>20&0x0f)*10;
  mm = (time>>8&0x0f) + (time>>12&0x0f)*10;
  ss = (time&0x0f) + (time>>4&0x0f)*10;
  
  return ((uint32_t)y<<25) | m<<21 | d<<16 | hh << 11 | mm<<5 | ss>>1;
}


void SDFatFile::open(const char * fname, const uint8_t mode) {
  digitalWrite(PIO1_19, LOW);
  peeked = false;
  sdfs.rescode = f_open(&file, fname, mode);
  if (mode == FILE_WRITE && !sdfs.rescode ) {
    sdfs.rescode = f_lseek(&file, f_size(&file));
  }
}

void SDFatFile::close(void) {
  sdfs.rescode = f_close(&file);
  digitalWrite(PIO1_19, LOW);
}

size_t SDFatFile::write(const uint8_t * buf, size_t num) {
  UINT count;
  sdfs.rescode = f_write(&file, buf, num, &count);
  return count;
}

int16_t SDFatFile::read(void) {
  UINT n = 0;
  if ( !peeked )
    sdfs.rescode = f_read(&file, &rbuf, 1, &n);
  peeked = false;
  return rbuf;
}

int16_t SDFatFile::peek(void) {
  UINT n = 0;
  if ( !peeked ) 
    sdfs.rescode = f_read(&file, &rbuf, 1, &n);
  peeked = true;
  return rbuf;
}

char * SDFatFile::gets(char * buff, size_t sz) {
  if ( peeked ) {
    peeked = false;
    *buff = rbuf;
    return f_gets((TCHAR*)buff+1, sz-1, &file);      
  } else {
    return f_gets((TCHAR*)buff, sz, &file);
  }
}