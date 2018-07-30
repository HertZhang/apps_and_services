#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftd2xx.h"
#include "ua376_linux.h"
#define BUF_SIZE 0x10

#define MAX_DEVICES		5

int main()
{
	static FT_PROGRAM_DATA Data;	

	FT_STATUS	ftStatus=0;

	FT_HANDLE	ftHandle[MAX_DEVICES];
	FT_HANDLE ftHandle0;

	int	i=0,retCode=0;

	FT_HANDLE h376;
	FILE *fp,*fpopen,*fpopen2;
	short int fch,chn,ch,gain;
	short int j;
	short int addat[32768];//
	

	printf("Hello World\n");
	//if((ftStatus = FT_Open(0, &ftHandle[0])) != FT_OK){
	if((ftStatus = FT_Open(0, &h376)) != FT_OK){
		printf("Error: FT_OpenEx returned %d for device %d\n", (int)ftStatus, i);
			return 1;
	}
	printf("Opened device success \n");
	
	Data.Signature1 = 0x00000000;
	Data.Signature2 = 0xffffffff;
	Data.Manufacturer = (char *)malloc(256); /* E.g "FTDI" */
	Data.ManufacturerId = (char *)malloc(256); /* E.g. "FT" */
	Data.Description = (char *)malloc(256); /* E.g. "USB HS Serial Converter" */
	Data.SerialNumber = (char *)malloc(256); /* E.g. "FT000001" if fixed, or NULL */
	if (Data.Manufacturer == NULL ||
	    Data.ManufacturerId == NULL ||
	    Data.Description == NULL ||
	    Data.SerialNumber == NULL)
	{
		printf("Failed to allocate memory.\n");
		retCode = 1;
		goto exit;
	}
	
	ftStatus = FT_EE_Read(h376, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Read failed\n");
		retCode = 1;
		goto exit;
	}
	
	
	printf("FT_EE_Read succeeded.\n\n");
		
	printf("Signature1 = %d\n", (int)Data.Signature1);			
	printf("Signature2 = %d\n", (int)Data.Signature2);			
	printf("Version = %d\n", (int)Data.Version);				
								
	printf("VendorId = 0x%04X\n", Data.VendorId);				
	printf("ProductId = 0x%04X\n", Data.ProductId);
	printf("Manufacturer = %s\n", Data.Manufacturer);			
	printf("ManufacturerId = %s\n", Data.ManufacturerId);		
	printf("Description = %s\n", Data.Description);			
	printf("SerialNumber = %s\n", Data.SerialNumber);			
	printf("MaxPower = %d\n", Data.MaxPower);				
	printf("PnP = %d\n", Data.PnP) ;					
	printf("SelfPowered = %d\n", Data.SelfPowered);			
	printf("RemoteWakeup = %d\n", Data.RemoteWakeup);
	ftStatus=FT_Close(h376);
	if(ftStatus==FT_OK)
	{
		printf("FT_Close succeff\n");
	}
	else 
	{
		printf("FT_Close error\n");
	}
	
exit:
	free(Data.Manufacturer);
	free(Data.ManufacturerId);
	free(Data.Description);
	free(Data.SerialNumber);
	FT_Close(h376);
	printf("Returning %d\n", retCode);
/**************************************************************************************************************************/	
	fch=0;chn=8;gain=0;
	OpenUA376(0,h376);


	printf("\ntest 2 \n");
	fch=0; chn=8;gain=0;
	mm376(h376, addat,fch,chn,1000,512,gain); // 多点采集, 1000 Hz, 512点 

	for(i=0;i<4;i++){
		for(ch=0;ch<chn;ch++){
		printf("%2.4f ",addat[i*chn+ch]*0.00030518);
		}
		printf("%4d \n",i);
	}
	printf("\n");

		
	start376(h376,fch,chn,150000,0);

	for(i=0;i<8;i++)
	{	
		if(read376(h376, addat,8192)!=0)
		{
			printf("read376 error i:%d \n",i);
			exit(1);
		} 
		for(i=0;i<8;i++)
		{
		    for(j=0;j<8;j++)
		    {
			printf("%6d ",addat[j+8*i]);
  		    }
		   printf("\n");
		}

		printf("Hello youcai\n");
	}
	
	
	
	stop376(h376);


	printf("\ntest DI \n");


	CloseUA376(h376);
	return 0;
   
}











