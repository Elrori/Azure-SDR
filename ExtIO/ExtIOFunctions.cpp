/*

	Written by Andrus Aaslaid, ES1UVB
	andrus.aaslaid(6)gmail.com

	http://uvb-76.net

	This source code is licensed as Creative Commons Attribution-ShareAlike
	(CC BY-SA). 
	
	From http://creativecommons.org:

		This license lets others remix, tweak, and build upon your work even for commercial purposes, as long as they 
		credit you and license their new creations under the identical terms. This license is often compared to 
		“copyleft” free and open source software licenses. All new works based on yours will carry the same license, 
		so any derivatives will also allow commercial use. This is the license used by Wikipedia, and is recommended 
		for materials that would benefit from incorporating content from Wikipedia and similarly licensed projects. 


	This DLL provides an empty core for implementing hardware support functionality
	for the Winrad Software Defined Radio (SDR) application, created by Jeffrey Pawlan (WA6KBL)
	(www.winrad.org) and its offsprings supporting the same ExtIO DLL format,
	most notably the outstanding HDSDR software (hdsdr.org)

	As the Winrad source is written on Borland C-Builder environment, there has been very little 
	information available of how the ExtIO DLL should be implemented on Microsoft Visual Studio 2008
	(VC2008) environment and the likes. 

	This example is filling this gap, providing the empty core what can be compiled as appropriate 
	DLL working for HDSDR

	Note, that Winrad and HDSDR are sometimes picky about the DLL filename. The ExtIO_blaah.dll for example,
	works, while ExtIODll.dll does not. I havent been digging into depths of that. It is just that if your
	custom DLL refuses to be recognized by application for no apparent reason, trying to change the DLL filename
	may be a good idea.

	To have the DLL built with certain name can be achieved changing the LIBRARY directive inside ExtIODll.def


	Revision History:

	30.05.2011	-	Initial 
	22.04.2012	-	Cleaned up for public release

*/
/*
*	modify:HELRORI
*/
#include "ExtIOFunctions.h"

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <winsock.h>
#pragma comment (lib, "wsock32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")
#pragma comment (lib, "SETUPAPI.lib")
#pragma comment (lib, "User32.lib")
#pragma comment (lib, "CyAPI.lib")
#include <stdexcept>
#include "CyAPI.h"

using namespace std;

#define OSC_FREQ 40000000
static unsigned int HWSR = 40000000/20/2;// 40000000/R1/R2

int frequency = 5000000;
void (* ExtIOCallback)(int, int, float, void *) = NULL;

static unsigned	gCustomSamplerate = 48000;
static int		giExtSrateIdx = 0;
static unsigned gExtSampleRate = 1000000;
volatile int	giParameterSetNo = 0;


HANDLE hThread;
CCyUSBDevice*	pUSB;
int             nDeviceCount;
CCyUSBEndPoint* OutEndPt;
CCyUSBEndPoint* InEndPt;

int AzureWR(CCyUSBEndPoint* outpoint, UCHAR* data, LONG len) {
	OVERLAPPED ov;
	ov.hEvent = CreateEvent(NULL, false, false, NULL);

	PUCHAR otxt = outpoint->BeginDataXfer(data, len, &ov);
	outpoint->WaitForXfer(&ov, 100);
	bool st = outpoint->FinishDataXfer(data, len, &ov, otxt);
	if (!st) { printf("mywr error\n"); return -1; }
	CloseHandle(ov.hEvent);
	return 0;
}
int AzureRD(CCyUSBEndPoint* inpoint, UCHAR* data, LONG len) {
	OVERLAPPED ov;
	ov.hEvent = CreateEvent(NULL, false, false, NULL);

	PUCHAR itxt = inpoint->BeginDataXfer(data, len, &ov);
	inpoint->WaitForXfer(&ov, 100);
	bool st = inpoint->FinishDataXfer(data, len, &ov, itxt);
	if (!st) { printf("myrd error\n"); return -1; }
	CloseHandle(ov.hEvent);
	return 0;
}
bool AzureInit() {
	pUSB = new CCyUSBDevice;
	nDeviceCount = pUSB->DeviceCount();
	printf("Device count:%d\n", nDeviceCount);
	if (nDeviceCount > 0) {
		printf("Device name:%s\n", pUSB->FriendlyName);
		if (!pUSB->Open(0)) {//打开第一号设备
			printf("Open error\n");
			return false;
		}
	}
	else {
		printf("No device!\n");
		return false;
	}
	OutEndPt = pUSB->EndPointOf((UCHAR)0x02);
	InEndPt  = pUSB->EndPointOf((UCHAR)0x86);
	return true;
}

ULONG __stdcall ThreadStart(void* lParam)
{
	unsigned char pkt[512*8];
	while(1)
	{			
			//pkt[0]存储i通道32bit低8位
			//pkt[4]存储q通道32bit低8位
			int st = AzureRD(InEndPt, pkt, 512 * 8);
			if (st == -1)break;
			(*ExtIOCallback)(512, 0, 0, pkt);
		
	}
	printf("exit\n");
	return 0;
}

void send_freq(int frequency)
{
	unsigned char pkt[512];

	int freq = frequency * 0x100000000 / OSC_FREQ;
	
	pkt[0] = 0x00;
	pkt[1] = 0x01;
	pkt[5] = (freq >> 0 ) & 0xFF;
	pkt[4] = (freq >> 8 ) & 0xFF;
	pkt[3] = (freq >> 16) & 0xFF;
	pkt[2] = (freq >> 24) & 0xFF;
	AzureWR(OutEndPt, pkt, 512);
	
}

//called once at startup time
extern "C" bool __stdcall InitHW(char *name, char *model, int& type)
{
	type = 6;	//the hardware does its own digitization and the audio data are returned to Winrad
				//via the callback device. Data must be in 32‐bit  (int) format, little endian.
	char* my_sdr_name  = "AZURE SDR";
	char* my_sdr_model = "AZURE SDR v1.0";
	memcpy(name, my_sdr_name, strlen(my_sdr_name)+1);
	memcpy(model,my_sdr_model,strlen(my_sdr_name)+1);

	
	return AzureInit();
}

extern "C" bool __stdcall OpenHW(void)
{
	AllocConsole() ;
	AttachConsole( GetCurrentProcessId() ) ;
	freopen( "CON", "w", stdout ) ;
	printf("AZURE SDR\n");
	return true;
}

extern "C" int __stdcall StartHW(long freq)
{
	DWORD dwID=0;
	hThread=CreateThread(0,64*1024,&ThreadStart,NULL,0,&dwID);////////////////////////////////////////////
	return 512;	// number of complex elements returned each
				// invocation of the callback routine
}

extern "C" void __stdcall StopHW(void)
{
	TerminateThread(hThread,0);
	return; // nothing to do with this specific HW
}

extern "C" void __stdcall CloseHW(void)
{
	return; // nothing to do with this specific HW
}

extern "C" int __stdcall SetHWLO(long LOfreq)
{	
	frequency = (int)LOfreq;

	unsigned char pkt[512];

	int freq = frequency * 0x100000000 / OSC_FREQ;

	pkt[0] = 0x00;
	pkt[1] = 0x01;
	pkt[5] = (freq >> 0) & 0xFF;
	pkt[4] = (freq >> 8) & 0xFF;
	pkt[3] = (freq >> 16) & 0xFF;
	pkt[2] = (freq >> 24) & 0xFF;
	AzureWR(OutEndPt, pkt, 512);

	return 0; // return 0 if the frequency is within the limits the HW can generate
}

extern "C" long __stdcall GetHWLO(void)
{
	return (long)frequency;	//LOfreq;
}

extern "C" long __stdcall GetHWSR(void)
{
	return HWSR;//采样率
}

extern "C" long __stdcall GetTune(void)
{
	return (long)frequency;
}

extern "C" int __stdcall GetStatus(void)
{
	return 0;
}

extern "C" void __stdcall TuneChanged(long freq)
{
	return;
}

extern "C" void __stdcall SetCallback(void (* Callback)(int, int, float, void *))
{
	ExtIOCallback = Callback;
	(*ExtIOCallback)(-1, 101, 0, NULL);			// sync lo frequency on display
	(*ExtIOCallback)(-1, 105, 0, NULL);			// sync tune frequency on display

		return;		// this HW does not return audio data through the callback device
					// nor it has the need to signal a new sampling rate.
}

extern "C" void __stdcall RawDataReady(long samprate, int *Ldata, int *Rdata, int numsamples)
{
	return;
}

//---------------------------------------------------------------------------


extern "C" int __stdcall ExtIoGetSrates(int srate_idx, double* samplerate)
{
	switch (srate_idx)
	{
	case 0:		*samplerate = 48000.0;	return 0;
	case 1:		*samplerate = 96000.0;	return 0;
	case 2:		*samplerate = 192000.0;	return 0;
	case 3:		*samplerate = 384000.0;	return 0;
	case 4:		*samplerate = 768000.0;	return 0;
	case 5:		*samplerate = 1536000.0;	return 0;
	case 6:		*samplerate = 2400000.0;	return 0;
	case 7:		*samplerate = 3072000.0;	return 0;
	case 8:		*samplerate = 6144000.0;	return 0;
	case 9:		*samplerate = gCustomSamplerate;	return 0;
	default:	return 1;	// ERROR
	}
	return 1;	// ERROR
}

extern "C" int  __stdcall ExtIoGetActualSrateIdx(void)
{
	return giExtSrateIdx;
}

extern "C" int  __stdcall ExtIoSetSrate(int srate_idx)
{
	double newSrate = 0.0;
	if (0 == ExtIoGetSrates(srate_idx, &newSrate))
	{
		giExtSrateIdx = srate_idx;
		HWSR = (unsigned)(newSrate + 0.5);
		++giParameterSetNo;
		return 0;
	}
	return 1;	// ERROR
}

extern "C" long __stdcall ExtIoGetBandwidth(int srate_idx)
{
	double newSrate = 0.0;
	long ret = -1L;
	if (0 == ExtIoGetSrates(srate_idx, &newSrate))
	{
		switch (srate_idx)
		{
		case 0:		ret = 40000L;	break;
		case 1:		ret = 80000L;	break;
		case 2:		ret = 160000L;	break;
		case 3:		ret = 320000L;	break;
		case 4:		ret = 640000L;	break;
		case 5:		ret = 1280000L;	break;
		case 6:		ret = 2000000L;	break;
		case 7:		ret = 2560000L;	break;
		case 8:		ret = 5120000L;	break;
		case 9:		ret = (long)(gCustomSamplerate * 0.8);	break;
		default:	ret = -1L;		break;
		}
		return (ret >= newSrate || ret <= 0L) ? -1L : ret;
	}
	return -1L;	// ERROR
}


//---------------------------------------------------------------------------

