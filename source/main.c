/*
    Nop90 - 2014

    Demo application using swkbd 3DS - Custom Software Keyboard for 3DS
*/


#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include "utils.h"
#include "swkbd.h"

void system_checkPolls();
int LoadBitmap(char* path, u32 width, u32 height, void* buf, u32 alpha, u32 startx, u32 stride, u32 flags);

// Global variables

FS_archive sdmcArchive;

int main()
{
u32 lastKeysPressed;
u32 keysPressed=0;
u8* tfb;
u8* bfb;
int x,y;
int updatescreen=0;
int customskin;

	// Initialize services
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInit();
	fsInit();

	sdmcArchive = (FS_archive){0x9, FS_makePath(PATH_EMPTY, "")};
	FSUSER_OpenArchive(NULL, &sdmcArchive);

	swkbd_Init();

	lastKeysPressed = 0;
	char buf[256];
	strcpy(buf,"Hello world!");

	customskin=swkbd_AddSkin("/keyboard.bmp");
	swkbd_SetSkin(1);
	swkbd_SetBgColor(0x0064AF64);

	gspWaitForVBlank();
	tfb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	bfb = gfxGetFramebuffer(GFX_BOTTOM, GFX_BOTTOM, NULL, NULL);

// load bitmap ad use top framebuffer as temp buf to swap pixel order
	u8* imgbuf = (u8*) memalign(0x10, 400*240*3);
	if (LoadBitmap("/MarioBG.bmp", 400, 240, imgbuf, 0xFF, 0, 64, 0x1))
	{
	  for(x=0; x < 400; x++)  
	    for(y=0; y < 240; y++)
            {
              tfb[3*(y+x*240)] = imgbuf[3 * y * 400 + 3 * x ]; //red
	      tfb[3*(y+x*240)+1] = imgbuf[3 * y * 400 + 3 * x + 1]; //green
	      tfb[3*(y+x*240)+2] = imgbuf[3 * y * 400 + 3 * x + 2]; //blue
	    }
	  memcpy( imgbuf, tfb, 240*400*3);
	  swkbd_SetTopScreenSourceBuf(imgbuf);
	}

	memset(tfb,255,240*400*3);

	paint_word(tfb,"Press 'A' to edit string.", 10, 30, 0, 0, 0);
	paint_word(tfb,"String:", 10, 46, 0, 0, 0);
	paint_word(tfb,buf, 72, 46, 0, 0, 0);
	if (customskin)
	{
	  paint_word(tfb,"Press 'B' to change keyboard layout.", 10, 224, 0, 0, 0);
	  paint_word(tfb,"Layout: Custom", 10, 232, 0, 0, 0);
	}
	memset(bfb,0,240*320*3);
	gfxFlushBuffers();
	gfxSwapBuffers();

	// Main loop
	while (aptMainLoop())
	{
	  gspWaitForVBlank();
	  hidScanInput();
	  keysPressed = hidKeysHeld();
	  if ((keysPressed & KEY_A) && !(lastKeysPressed & KEY_A))
	  {
	    swkbd_GetStr(buf, 256);
	    updatescreen=1;
	  }
	  if ((keysPressed & KEY_B) && !(lastKeysPressed & KEY_B))
	  {
	    swkbd_SetSkin((swkbd_GetSkin() + 1) % 2);
	    updatescreen=1;
	  }

	  lastKeysPressed = keysPressed;
	  if(updatescreen)
	  {
	    tfb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	    bfb = gfxGetFramebuffer(GFX_BOTTOM, GFX_BOTTOM, NULL, NULL);
	    memset(tfb,255,240*400*3);
	    paint_word(tfb,"Press 'A' to edit string.", 10, 30, 0, 0, 0);
	    paint_word(tfb,"String:", 10, 46, 0, 0, 0);
	    paint_word(tfb,buf, 72, 46, 0, 0, 0);
	    if (customskin)
	    {
	      paint_word(tfb,"Press 'B' to change keyboard layout.", 10, 224, 0, 0, 0);
	      paint_word(tfb,"Layout:", 10, 232, 0, 0, 0);
	      if (swkbd_GetSkin()) paint_word(tfb,"Custom", 74, 232, 0, 0, 0);
	      else paint_word(tfb,"Default", 74, 232, 0, 0, 0);
	    }
	    memset(bfb,255,240*320*3);
	    gfxFlushBuffers();
	    gfxSwapBuffers();
	  }
	  system_checkPolls(); 
	}

	return 0;
}

void system_checkPolls() {
    APP_STATUS status;

	while((status=aptGetStatus()) != APP_RUNNING) {

        if(status == APP_SUSPENDING)
        {
            aptReturnToMenu();
        }
        else if(status == APP_PREPARE_SLEEPMODE)
        {
	    aptSignalReadyForSleep();
            aptWaitStatusEvent();
        }
        else if (status == APP_SLEEPMODE) {

        }
        else if (status == APP_EXITING) {

	    swkbd_Exit();
            gfxExit();
            hidExit();
            aptExit();
            srvExit();
        }

        gspWaitForVBlank();
    }

}

int LoadBitmap(char* path, u32 width, u32 height, void* buf, u32 alpha, u32 startx, u32 stride, u32 flags)
{
	Handle file;
	FS_path filePath;
	filePath.type = PATH_CHAR;
	filePath.size = strlen(path) + 1;
	filePath.data = (u8*)path;
	
	Result res = FSUSER_OpenFile(NULL, &file, sdmcArchive, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
	if (res) 
		return 0;
		
	u32 bytesread;
	u32 temp;
	
	// magic
	FSFILE_Read(file, &bytesread, 0, (u32*)&temp, 2);
	if ((u16)temp != 0x4D42)
	{
		FSFILE_Close(file);
		return 0;
	}
	
	// width
	FSFILE_Read(file, &bytesread, 0x12, (u32*)&temp, 4);
	if (temp != width)
	{
		FSFILE_Close(file);
		return false;
	}
	
	// height
	FSFILE_Read(file, &bytesread, 0x16, (u32*)&temp, 4);
	if (temp != height)
	{
		FSFILE_Close(file);
		return false;
	}
	
	// bitplanes
	FSFILE_Read(file, &bytesread, 0x1A, (u32*)&temp, 2);
	if ((u16)temp != 1)
	{
		FSFILE_Close(file);
		return 0;
	}
	
	// bit depth
	FSFILE_Read(file, &bytesread, 0x1C, (u32*)&temp, 2);
	if ((u16)temp != 24)
	{
		FSFILE_Close(file);
		return 0;
	}
	
	
	u32 bufsize = width*height*3;
	
	FSFILE_Read(file, &bytesread, 0x36, buf, bufsize);
	FSFILE_Close(file);
	
	return 1;
}

