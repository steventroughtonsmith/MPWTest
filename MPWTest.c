#define OLDROUTINENAMES 1

#include <stdio.h>
#include <Quickdraw.h>
#include <MacWindows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <Devices.h>

WindowPtr emulatorWindowPtr;

Boolean quit = 0;
QDGlobals qd;

RGBColor black = {0x0000, 0x0000, 0x0000};
RGBColor white = {0xffff, 0xffff, 0xffff};
RGBColor pink = {0xffff, 0x0000, 0xffff};

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define appleID 128
#define fileID 129

#define rUserAlert 129

const short appleM = 0;
const short fileM = 1;

void Repaint()
{
	short i;
	char *hello = "Hello 68k World!";
	char *hello2 = "Compiled in OS X";
	
	MoveTo(50,50+23);
	
	for (i = 0; i < strlen(hello); i++)
	{
		DrawChar(hello[i]);
	}
	
	MoveTo(50,50+23+16);
	for (i = 0; i < strlen(hello2); i++)
	{
		DrawChar(hello2[i]);
	}
}

void DoCommand(long mResult)
{
	short theItem;
	short theMenu;
	Str255		daName;
	short		daRefNum;
	
	theItem = LoWord(mResult);
	theMenu = HiWord(mResult);
	
	switch (theMenu)
	{
		case appleID:
		{
			if (theItem == 1)
			{
				Alert(rUserAlert, nil);
			}
			else
			{
				/* all non-About items in this menu are DAs */
				/* type Str255 is an array in MPW 3 */
				GetMenuItemText(GetMenuHandle(appleID), theItem, daName);
				daRefNum = OpenDeskAcc(daName);
			}
			break;
		}
		case fileID:
		{
			quit = 1;
			break;
		}
		default:
			break;
	}
	
	HiliteMenu(0);
}

void RunLoop()
{
	while (!quit)
	{
	 	EventRecord    theEvent;
		WindowPtr whichWindow;

		if (GetNextEvent(everyEvent, &theEvent))
		{
			switch (theEvent.what)
			{
				case mouseDown:
				{
					switch (FindWindow(theEvent.where, &whichWindow))
					{
						case inSysWindow:
						{
							SystemClick(&theEvent, whichWindow);
							break;
						}
						case inMenuBar:
							DoCommand(MenuSelect(theEvent.where));
							break;
						case inDrag:
							DragWindow(whichWindow, theEvent.where, &qd.screenBits.bounds);
							break;

						default:
							break;
					}
					break;
				}
				case keyDown:
				{
					char theChar = (theEvent.message&charCodeMask);
					
					if (theEvent.modifiers&cmdKey)
					{
						DoCommand(MenuKey(theChar));
						
						if (theChar == 'm')
						{
							quit = true;
						}
					}
					
					break;
				}
				case activateEvt:
				{
					if (theEvent.modifiers&activeFlag)
					{
						// activate
					}
					else
					{
						// deactivate
					}
					break;
				}
				case updateEvt:
				{
					Repaint();
//					BeginUpdate((WindowPtr)theEvent.message);
//					EraseRect(&((WindowPtr)theEvent.message)->portRect);
//					EndUpdate((WindowPtr)theEvent.message);
					break;
				}
				default:
					break;
			}
			
		}
	}
}


void SetUpMenus()
{
	short i;
	
	MenuHandle myMenus[2];
	myMenus[appleM] = GetMenu(appleID);
	AddResMenu(myMenus[appleM],'DRVR');
	myMenus[appleM] = GetMenu(appleID);
	myMenus[fileM] = GetMenu(fileID);

	for (i = 1; i <= 2; i++)
	{
		InsertMenu(myMenus[i-1], 0);
	}
	
	DrawMenuBar();
}

void Initialize(void)
{
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
	
	FlushEvents(everyEvent, 0);
	
	SetUpMenus();

} /*Initialize*/

void main()
{
	Rect windowRect;
	
	Initialize();
	
	SetRect(&windowRect, 50, 50, 50+SCREEN_WIDTH, 50+SCREEN_HEIGHT);
	
	emulatorWindowPtr = NewWindow(nil, &windowRect, "\pMain Window", true, noGrowDocProc, (WindowPtr)-1L, true, (long)nil);
	
	RunLoop();
}
