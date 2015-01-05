#if TARGET_API_MAC_CARBON

#include <Carbon.h>

#else
#define OLDROUTINENAMES 1

#include <stdio.h>
#include <Quickdraw.h>
#include <MacWindows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <Devices.h>

#define GetWindowPort(w) w
QDGlobals qd;

#endif

WindowPtr mainWindowPtr;

Boolean quit = 0;
Boolean isShift = 0;

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
#if TARGET_API_MAC_CARBON
#define APP_NAME_STRING "\pPowerPC Carbon Window"
#else
#define APP_NAME_STRING "\pPowerPC Toolbox Window"
#endif
#else
#if TARGET_API_MAC_CARBON
#define APP_NAME_STRING "\p68K Carbon Window"
#else
#define APP_NAME_STRING "\p68K Toolbox Window"
#endif
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
#if !TARGET_API_MAC_CARBON
			else
			{
				/* all non-About items in this menu are DAs */
				/* type Str255 is an array in MPW 3 */
				GetMenuItemText(GetMenuHandle(appleID), theItem, daName);
				daRefNum = OpenDeskAcc(daName);
			}
#endif
			break;
		}
		case fileID:
		{
			quit = 1;
			break;
		}
		case editID:
		{
#if !TARGET_API_MAC_CARBON
			if (!SystemEdit(theItem-1)) // call Desk Manager to handle editing command if desk accessory window is the active window
#endif
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
	Rect windRect;
	
#if TARGET_API_MAC_CARBON
	BitMap 			theScreenBits;
	GetWindowPortBounds(mainWindowPtr, &txRect);
#else
	txRect = qd.thePort->portRect;
#endif
	
	InsetRect(&txRect,4,0);
	textH = TENew(&txRect,&txRect);
	
	while (!quit)
	{
#if !TARGET_API_MAC_CARBON
		SystemTask();
#endif
		TEIdle(textH);
		
		if (GetNextEvent(everyEvent, &theEvent))
		{
			switch (theEvent.what)
			{
				case mouseDown:
				{
					switch (FindWindow(theEvent.where, &whichWindow))
					{
#if !TARGET_API_MAC_CARBON
						case inSysWindow:
						{
							SystemClick(&theEvent, whichWindow);
							break;
						}
#endif
						case inMenuBar:
						{
							DoCommand(MenuSelect(theEvent.where));
							break;
						}
						case inDrag:
						{
#if TARGET_API_MAC_CARBON
							GetQDGlobalsScreenBits( &theScreenBits ); /* carbon accessor */
							windRect = theScreenBits.bounds;
#else
							windRect = qd.screenBits.bounds;
#endif
							DragWindow(whichWindow, theEvent.where, &windRect);
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
					Rect pr;
					
					BeginUpdate((WindowPtr)theEvent.message);
					
#if TARGET_API_MAC_CARBON
					GetWindowPortBounds(mainWindowPtr, &pr);
#else
					pr = qd.thePort->portRect;
#endif
					EraseRect(&(pr));
					TEUpdate(&(pr), textH);
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
#if !TARGET_API_MAC_CARBON
	AddResMenu(myMenus[appleM],'DRVR');
#endif
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
#if !TARGET_API_MAC_CARBON
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
#endif
	
	FlushEvents(everyEvent, 0);
	
#if !TARGET_API_MAC_CARBON
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
#endif
	InitCursor();
	
	SetUpMenus();
}

void main()
{
	Rect windowRect;
	
	Initialize();
	
	SetRect(&windowRect, 50, 50, 50+SCREEN_WIDTH, 50+SCREEN_HEIGHT);
	
	mainWindowPtr = NewWindow(nil, &windowRect, APP_NAME_STRING, true, noGrowDocProc, (WindowPtr)-1L, true, (long)nil);
	SetPort(GetWindowPort(mainWindowPtr));
	
	RunLoop();
}
