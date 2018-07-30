#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftd2xx.h"


int  outb(FT_HANDLE ftHandle, DWORD port ,DWORD dat1);

int  OpenUA376(int n, FT_HANDLE ftHandle);

int  CloseUA376(FT_HANDLE ftHandle);

void  dout(FT_HANDLE ftHandle,BYTE dd);

int  ss376(FT_HANDLE ftHandle,short *addat,short fch,short chn,short gn);

int  mm376(FT_HANDLE ftHandle,short *addat,short fch,short chn,long fr,long leg,short gn);

int  get_status(FT_HANDLE ftHandle,unsigned long *RxBytes,unsigned long *TxBytes);

int  stop376(FT_HANDLE ftHandle);

int start376(HANDLE ftHandle,short fch,short chn,long fr,short gn);

int  read376(HANDLE ftHandle,short *addat,long leg);



