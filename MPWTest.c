#define OLDROUTINENAMES 1

#include <stdio.h>
#include <Quickdraw.h>
#include <MacWindows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <Devices.h>

WindowPtr mainWindowPtr;

Boolean quit = 0;
Boolean isShift = 0;

QDGlobals qd;
TEHandle textH;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define appleID 128
#define fileID 129
#define editID 130

#define rUserAlert 129

const short appleM = 0;
const short fileM = 1;
const short editM = 2;

#define undoCommand 1
#define cutCommand 3
#define copyCommand 4
#define pasteCommand 5
#define clearCommand 6

#ifdef powerc
#define APP_NAME_STRING "\pPowerPC Window"
#else
#define APP_NAME_STRING "\p68K Window"
#endif

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
		case editID:
		{
			if (!SystemEdit(theItem-1)) // call Desk Manager to handle editing command if desk accessory window is the active window
			{
				switch (theItem) {
					case cutCommand:
						TECut(textH);
						break;
					case copyCommand:
						TECopy(textH);
						break;
					case pasteCommand:
						TEPaste(textH);
						break;
					case clearCommand:
						TEDelete(textH);
						break;
					default:
						break;
				}
			}
		}
		default:
			break;
	}
	
	HiliteMenu(0);
}


void RunLoop()
{
	EventRecord    theEvent;
	WindowPtr whichWindow;
	Rect txRect;
	
	txRect = qd.thePort->portRect;
	
	InsetRect(&txRect,4,0);
	textH = TENew(&txRect,&txRect);
	
	while (!quit)
	{
		SystemTask();
		TEIdle(textH);

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
						{
							DoCommand(MenuSelect(theEvent.where));
							break;
						}
						case inDrag:
						{
							DragWindow(whichWindow, theEvent.where, &qd.screenBits.bounds);
							break;
						}
						case inContent:
						{
							if (whichWindow != FrontWindow())
							{
								SelectWindow(whichWindow);
							}
							else
							{
								GlobalToLocal(&theEvent.where);
								if (theEvent.modifiers&shiftKey)
									isShift = true;
								
								TEClick(theEvent.where, isShift, textH);
							}
							break;
						}

						default:
							break;
					}
					break;
				}
				case autoKey:
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
					else
					{
						TEKey(theChar, textH);
					}
					
					break;
				}
				case activateEvt:
				{
					if (theEvent.modifiers&activeFlag)
					{
						TEActivate(textH);
					}
					else
					{
						TEDeactivate(textH);
					}
					break;
				}
				case updateEvt:
				{
					BeginUpdate((WindowPtr)theEvent.message);
					EraseRect(&(qd.thePort->portRect));
					TEUpdate(&(qd.thePort->portRect), textH);
					EndUpdate((WindowPtr)theEvent.message);
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
	
	MenuHandle myMenus[3];
	myMenus[appleM] = GetMenu(appleID);
	AddResMenu(myMenus[appleM],'DRVR');
	myMenus[appleM] = GetMenu(appleID);
	myMenus[fileM] = GetMenu(fileID);
	myMenus[editM] = GetMenu(editID);

	for (i = 0; i < 3; i++)
	{
		InsertMenu(myMenus[i], 0);
	}
	
	DrawMenuBar();
}

void Initialize(void)
{
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);

	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
	
	SetUpMenus();

}

void main()
{
	Rect windowRect;
	
	Initialize();

	SetRect(&windowRect, 50, 50, 50+SCREEN_WIDTH, 50+SCREEN_HEIGHT);
	
	mainWindowPtr = NewWindow(nil, &windowRect, APP_NAME_STRING, true, noGrowDocProc, (WindowPtr)-1L, true, (long)nil);
	SetPort(mainWindowPtr);
	
	RunLoop();
}
