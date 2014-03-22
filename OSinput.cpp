#include "pch.h"


//#include <sys/types.h>
//#include <sys/stat.h>

//#include <X11/extensions/XInput.h>


/// if not using precompiled header use the following
/*
#include <math.h>
 
#ifdef USING_DIRECTINPUT
  #define DIRECTINPUT_VERSION 0x0800
  #include <dinput.h>
#endif
#ifdef USING_XINPUT
  // #include <c:\alec\dxSDK2010j\Include\XInput.h> // USE XINPUT 1.3
#endif
 
#ifdef OS_LINUX
#include <linux/joystick.h>   // it's not x stuff... lots of crap added, keyboard/mouse, that is not needed
#include <fcntl.h>
#endif

#include "../!utilClasses/typeShortcuts.h"
#include "../!utilClasses/stringClass.h"
#include "../!utilClasses/errorHandling.h"
#include "OSchar.h"
#include "OSinput.h"
*/



/* TODO:
 *
 * every COORDONATE UNIFICATION: x0,y0 -> left,bottom (as in OpenGL, MAC.  NOT AS IN: windows, (i think linux too)
      this includes mouseWheel (+y means UP, as in the real cood system)
 
 * 
 
 
 * joystick button history
 * 
 * name the TYPE2 / TYPE3 driver (osi must handle at least that)
 *
 * i think every time variable should be int64... dunno for shure. nanosecs are int64, mili= nano* 1,000,000... still way more data to hold in a int64
 *    they may be a little slower... dunno, but not by much
 *
 * buy a ffeedback joystick
 * buy a wheel
 * 
 */




/*	
    WM_POINTERDOWN <-- pens/watever? should be implemented?
    movement ID ? last movement/ current movement ID, so u know if blabla/ BUT, dont think it's worth it. too much precision for almost nothing
    even wheel ID, maybe a buffer of 4-8 commands, check if they were processed ?
*/

// makes a null cursor LINUX/QUAKE2 SOURCE
/*
static Cursor CreateNullCursor(Display *display, Window root)
{
    Pixmap cursormask; 
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap(display, root, 1, 1, 1); // last var is depth
    xgc.function = GXclear;
    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(display, cursormask, cursormask,
          &dummycolour,&dummycolour, 0,0);
    XFreePixmap(display,cursormask);
    XFreeGC(display,gc);
    return cursor;
}
*/


// Input in;   // PLACE IT HERE????????????????????????????



// EFFECTS TESTING ... must aquire a WORKING forcefeedback / vibration HID, first ffs!
// -----------------------------======================--------------------------------

void Input::vibrate() {
  #ifdef OS_WIN // direct input vibration
  /*
  HRESULT  hr;
  DIEFFECT diEffect;               // parameters for created effect

  DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
  LONG     lDirection[2] = { 18000, 0 };

  DIPERIODIC periodic;
  DICONSTANTFORCE constant;

  constant.lMagnitude= DI_FFNOMINALMAX;
  
  periodic.dwMagnitude= DI_FFNOMINALMAX;
  periodic.dwPeriod= 5000;
  periodic.dwPhase= 0;
  periodic.lOffset= 0;
  

  diEffect.dwSize          = sizeof(DIEFFECT); 
  
  diEffect.dwFlags         = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
  diEffect.dwDuration      = (DWORD)(0.5 * DI_SECONDS);
  diEffect.dwSamplePeriod  = 5880;                 // = default 
  diEffect.dwGain          = DI_FFNOMINALMAX;   // No scaling
  diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
  diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
  diEffect.cAxes                   = 2; 
  diEffect.rgdwAxes                = &dwAxes[0]; 
  diEffect.rglDirection            = &lDirection[0]; 
  diEffect.lpEnvelope              = NULL; 
  */


  // wLeftMotorSpeed
  long hr;
        DWORD    rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };  // X- and y-axis
        LONG rglDirection[2] = { 0, 0 };

        DICONSTANTFORCE cf;
        DIEFFECT eff;

        ZeroMemory( &eff, sizeof( eff ) );
        eff.dwSize = sizeof( DIEFFECT );
        eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        eff.dwDuration = INFINITE;
        eff.dwSamplePeriod = 0;
        eff.dwGain = DI_FFNOMINALMAX;
        eff.dwTriggerButton = DIEB_NOTRIGGER;
        eff.dwTriggerRepeatInterval = 0;
        eff.cAxes = 2; //device.ff.axisffbcount;
        eff.rgdwAxes = rgdwAxes;
        eff.rglDirection = rglDirection;
        eff.lpEnvelope = 0;
        eff.cbTypeSpecificParams = sizeof( DICONSTANTFORCE );
        eff.lpvTypeSpecificParams = &cf;
        eff.dwStartDelay = 0;


        // Create the prepared effect
        hr= gp[4].diDevice->CreateEffect( GUID_ConstantForce, &eff, &gp[4].vibration, NULL );
        

  //diEffect.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
  //diEffect.lpvTypeSpecificParams = &constant;  
  
        /*
  diEffect.cbTypeSpecificParams    = sizeof(DIPERIODIC);
  diEffect.lpvTypeSpecificParams   = &periodic;  
  

  hr= gp[4].diDevice->CreateEffect(GUID_SawtoothUp, &diEffect , &gp[4].vibration, NULL);

  gp[4].diDevice->SendForceFeedbackCommand(DISFFC_SETACTUATORSON);
  */
        if(hr!= DI_OK)
          error.dinput(hr);

  
//	lpdiEffect->Download();
  //gp[4].vibration->Start(1, null);
  //hr = g_lpdid->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect, NULL);
  #endif /// OS_WIN
}
/// end testing

#ifdef OS_MAC
/// all these are callback functions. any pad/stick/wheel added/button press / etc + some kind of egipteean hierogliph func, that should be cleaned (first one)
static CFMutableDictionaryRef hu_CreateDeviceMatchingDictionary(UInt32 inUsagePage, UInt32 inUsage);
static void HIDadded(void *, IOReturn , void *, IOHIDDeviceRef);
static void HIDremoved(void *, IOReturn , void *, IOHIDDeviceRef);
static void HIDchange(void *, IOReturn, void *, IOHIDValueRef);
//static void HIDchange(void *, IOReturn, void *, IOHIDReportType, uint32_t, uint8_t *, CFIndex);
#endif /// OS_MAC


// ------------============= INPUT CLASS ===========--------------------
// =====================================================================
Input::Input() {
  nr.jFound= nr.gpFound= nr.gwFound= 0;
  
  nr.jOS= nr.gpOS= nr.gwOS= 0;
  nr.jT2= nr.gpT2= nr.gwT2= 0;
  nr.jT3= nr.gpT3= nr.gwT3= 0;
  
  #ifdef USING_DIRECTINPUT
  m.diDevice= null;
  dInput= null;
  #endif /// USING_DIRECTINPUT
  
  #ifdef OS_MAC
  k.numLock= true;    /// macs don't handle num locks. this will always be on
  #endif /// OS_MAC
}

Input::~Input() {
  delData();
  
  IOHIDManagerClose(manager, kIOHIDOptionsTypeNone); /// close the HID manager
  CFRelease(manager);                                /// delloc memory
}



void Input::delData() {
  m.delData();
  k.delData();
  short a;
  for(a= 0; a< MAX_JOYSTICKS; a++)
    j[a].delData();
  for(a= 0; a< MAX_JOYSTICKS; a++)
    gp[a].delData();
  for(a= 0; a< MAX_JOYSTICKS; a++)
    gw[a].delData();

  nr.jFound= nr.gpFound= nr.gwFound= 0;
  nr.jOS= nr.gpOS= nr.gwOS= 0;
  nr.jT2= nr.gpT2= nr.gwT2= 0;
  nr.jT3= nr.gpT3= nr.gwT3= 0;

  #ifdef USING_DIRECTINPUT
  if(dInput)
    dInput->Release();
  dInput= null;
  #endif
}

bool Input::init(int mMode, int kMode) {
  m.mode= mMode;
  k.mode= kMode;
  #ifdef USING_DIRECTINPUT
  if(!dInput)
    if(DirectInput8Create(osi.win[0].hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput, NULL)!= DI_OK) {
      error.simple("Could not initialize Direct Input");
      return false;
    }
  #endif

  /// fill in all vars, find all xinput/directinput devices, etc
  populate(true);
  
  /// Kv struct has (almost) all keyboard keys. It has to 'populate' all vars @ start
  Kv.populate();

  #ifdef USING_DIRECTINPUT
  for(short a= 0; a< nr.jT2; a++) {
    getT2j(a)->init(2);     /// 1 init for all 3 HIDs (it is an option how to use the device data, as a wheel/gamepad/joystick)
    getT2gp(a)->mode= 2;
    getT2gw(a)->mode= 2;
  }
  #endif
  
  #ifdef USING_XINPUT
  for(short a= 0; a< nr.jT3; a++) {
    getT3j(a)->init(3);     /// 1 init for all 3 HIDs (it is an option how to use the device data, as a wheel/gamepad/joystick)
    getT3gp(a)->mode= 3;
    getT3gw(a)->mode= 3;
  }
  #endif /// USING_XINPUT
  
  #ifdef OS_MAC
  
  in.manager= IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  
  // COPY PASTE from apple 'documentation'
  
  
  /// create an array of matching dictionaries
  CFMutableArrayRef arr= CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
  
  /// create a device matching dictionary for joysticks
  CFDictionaryRef dic= hu_CreateDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
  CFArrayAppendValue(arr, dic);                 /// add it to the matching array
  CFRelease(dic);                               /// release it
  
  /// create a device matching dictionary for game pads
  dic= hu_CreateDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
  CFArrayAppendValue(arr, dic);                 /// add it to the matching array
  CFRelease(dic);                               /// release it
  
  /// create a device matching dictionary for whatever multi axis controller
  dic= hu_CreateDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_MultiAxisController);
  CFArrayAppendValue(arr, dic);                 /// add it to the matching array
  CFRelease(dic);                               /// release it

  
  /// set the created array of criterias to the manager
  IOHIDManagerSetDeviceMatchingMultiple(manager, arr);
  
  CFRelease(arr);                               // release array
  
  
  IOHIDManagerRegisterDeviceMatchingCallback(manager, HIDadded, NULL);  /// callback function for when a matching HID is added
  IOHIDManagerRegisterDeviceRemovalCallback(manager, HIDremoved, NULL); /// callback function for when a matching HID is removed
  
  // RUN LOOPS? MAYBE WON'T NEED THEM
  IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  
  /// open the manager
  IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone); // option bits: kIOHIDOptionsTypeNone or kIOHIDOptionsTypeSeizeDevice
  
  
  /// register a callback function when a value from any registered HIDs change
  IOHIDManagerRegisterInputValueCallback(manager, HIDchange, NULL);  // this... works... if u have the enigma codebreaker (the one that didn't have the actual machine to crack the enigma code)
  //  IOHIDManagerRegisterInputReportCallback(manager, HIDchange, NULL);  // TRY REPORTS
  
  
  //continue from https://developer.apple.com/library/mac/technotes/tn2187/_index.html
  
  #endif /// OS_MAC
  
  return true;
}


void Input::populate(bool scanMouseKeyboard) {
  //bool chatty= true;
  if(scanMouseKeyboard) {

    // ---===MOUSE SCAN===---
    if(m.mode== 3) {
      m.delData();
      if(!m.init(3))        /// try to use direct input
        m.init(2);          /// else use defaul windows functions
    } else
      m.init(m.mode);

    // ---===KEYBOARD SCAN===---
    if(k.mode== 3) {
      k.delData();
      if(!k.init(3))
        k.init(2);
    }
  } /// scanMouseKeyboard

  // ---===JOYSTICK SCAN===---
  nr.jOS= 0;

  #ifdef OS_WIN
  int b= joyGetNumDevs();
  if(joyGetNumDevs()) {     /// if this func doesn't return a positive number, there is no driver installed
    change me (max 8 joysticks)
/// search normal driver joysticks
    JOYINFOEX jinfo;
    jinfo.dwSize= sizeof(JOYINFOEX);
    if(b> 16) b= 16;          // limit to 16 detectable joysticks
    for(uint a= 0; a< joyGetNumDevs(); a++)
      if(joyGetPosEx(a, &jinfo) != JOYERR_UNPLUGGED) {        /// if it's plugged in-> found a joystick
        j[nr.jOS].id= a;
        j[nr.jOS].mode= 1;
        nr.jOS++;
      }
  }
  #endif /// OS_WIN

  #ifdef USING_DIRECTINPUT
  dInput->EnumDevices(DI8DEVCLASS_GAMECTRL, diDevCallback, NULL, DIEDFL_ATTACHEDONLY);
  #endif /// USING_DIRECTINPUT

  #ifdef OS_LINUX

  // xlib xinput ... after some research, found nothing
  /*
  int ndev;

  XDeviceInfo *dev= XListInputDevices(osi.primWin->dis, &ndev);
  
  for(short a= 0; a< ndev; a++) {
    printf("dev[%d] id[%d] [%s] use[%d]\n", a, dev[a].id, dev[a].name, dev[a].use);
  }
  */
  
  // linux [MODE 1] using "linux/joystick.h". system driver
  
  
  // NOT DONE, must use JSIOGCVERSION
  /*
    JSIOGCVERSION is a good way to check in run-time whether the running
      driver is 1.0+ and supports the event interface. If it is not, the
      IOCTL will fail. For a compile-time decision, you can test the
      JS_VERSION symbol
   
   * just set the joysticks/gpads/gwheels on mode 0, so they are disabled if OS is not using joy driver
  */
  
  
  
  
  
	//#ifdef JS_VERSION
	//#if JS_VERSION > 0xsomething
  
  int f;
  int version, axes= 0, buttons= 0;
  char name[128];
  short id= 0;
  string s= "/dev/input/js";
  
  /// searching for 32 js[X] files
  for(short a= 0; a< 32; a++) {
    /// this limit to 8 can be changed if neccesary... can't predict what will happen in 10-20 years....
    if(id== 8) {
      error.simple("OSInput::init: Maximum number of jSticks/gPads/gWheels reached (where did you plug more than 8?)");
      break;
    }
      
    f= version= axes= buttons=0;
    
    f= open(s+ (a+ 48ul), O_RDONLY| O_NONBLOCK);
    if(f== -1) continue;
    
    ioctl(f, JSIOCGAXES, &axes);
    ioctl(f, JSIOCGBUTTONS, &buttons);
    ioctl(f, JSIOCGVERSION, &version);
    ioctl(f, JSIOCGNAME(sizeof(name)), &name);

    /// check if the joystick is valid. I found out that there are mouses that report as joysticks...
    ///    but they report incredible amounts of axes and buttons...
    ///    still, can't rely only on this, FURTHER CHECKS SHOULD BE MADE  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if((axes> 32) || (buttons> MAX_JOYSTICK_BUTTONS)) {
      close(f);
      continue;
    }

    /// all joysticks/gamepads/gamewheels share the same file (driver). 
    j[id].mode= gp[id].mode= gw[id].mode= 1;
    j[id].id=   gp[id].id=   gw[id].id=   a;
    j[id].file= gp[id].file= gw[id].file= f;
    j[id].name= gp[id].name= gw[id].name= name;
    
    /// better to have lots of vars. The other option would be having complicated search algorithms just to find out how many joysticks found that use driver N
    ///   the purpose is to have the same code run on every OS... game searches for driver type 1 (os driver), type 2(in win is directinput) etc...
    nr.jFound++;  nr.gpFound++; nr.gwFound++; 
    nr.jOS++;     nr.gpOS++;    nr.gwOS++;
    
    id++;
    if(chatty) printf("Name: %s Axes: %d Buttons: %d Version: %d\n", name, axes, buttons, version);
  }
  #endif /// OS_LINUX
  
  #ifdef OS_MAC
  #endif /// OS_MAC
  
  /// all joys/gp/gw will have same directinput/xinput drivers
  // ---===GAMEPAD SCAN===---

  // ---===GAMEWHEEL SCAN===---


}

void Input::resetPressedButtons() {
  m.resetButtons();
  k.resetButtons();
}



void Input::update() {
  //  getDIgp(0)->update();
  // for (each gamepad if it is active, update?)
  // same for everything else
  // need an "active" or "inUse" bool
  
  // !!!!
  // types have changed, so same code can be used on every os
  // a type should have a name (direct input/xinput/os blabla), to pass to the program
  // !!!!
  
  #ifdef OS_LINUX
  /// [MODE 1] joysticks/ gpads/ gwheels
  int n= -1, nev;
  js_event ev[64];
  
  for(short a= 0; a< nr.jOS; a++) {       // for all active joysticks

ReadAgain:
    /// read as many events as possible in 1 go
    n= read(j[a].file, ev, sizeof(ev));
      
    if(n== -1)                            /// no event happened
      continue;

    nev= n/ sizeof(js_event);             /// nr of events read
      
    for(short b= 0; b< nev; b++) {         // for each event
      ev[b].type&= ~JS_EVENT_INIT;        /// disable the init flag... no use i can think of ATM
        
        
      if(ev[b].type == JS_EVENT_BUTTON) {  // ---=== button event ===---
        j[a].b[ev[b].number]=  (uchar)ev[b].value;
        gp[a].b[ev[b].number]= (uchar)ev[b].value;
        gw[a].b[ev[b].number]= (uchar)ev[b].value;
        
        if(ev[b].value== 1) {             /// press
          j[a].bTime[ev[b].number]= ev[b].time;
          gp[a].bTime[ev[b].number]= ev[b].time;
          gw[a].bTime[ev[b].number]= ev[b].time;
        } else {                          /// release
          /// put the button in history
          ButPressed p;
          p.b= ev[b].number;
          p.checked= false;
          p.timeDown= j[a].bTime[ev[b].number];
          p.timeUp= ev[b].time;
          p.timeDT= p.timeUp- p.timeDown;
          
          j[a].log(p);
          gp[a].log(p);
          gw[a].log(p);
        }
          
      } /// button event
        
        
        
      if(ev[b].type== JS_EVENT_AXIS) {     // ---=== axis event ===---
        /// axis order...
        
        // possible to make a[MAX_AXIS] and x/y/rudder/etc would be refrences to a[]
        switch (ev[b].number) {
                                          // [JOYSTICK]  / [GAMEPAD]   / [GAMEWHEEL]
          case 0:                         // [X axis?]   / [l stick X] / [wheel???]
            gp[a].lx= ev[b].value;
            
            break;
          case 1:                         // [Y axis?]   / [l stick Y] / [wheel???]
            gp[a].ly= ev[b].value;
            
            break;
          case 2:                         // [Throttle?] / [r stick X] / [wheel???]
            gp[a].rx= ev[b].value;
            
            break;
          case 3:                         // [extra1 X?] / [l trigger] / [wheel???]
            gp[a].lt= ev[b].value;
            
            break;
          case 4:                         // [extra1 Y?] / [r trigger] / [wheel???]
            gp[a].rt= ev[b].value;
            
            break;
          case 5:                         // [Rudder?]   / [rStick Y]  / [wheel???]
            gp[a].ry= ev[b].value;
            
            break;
          case 6:                         // [POV X?]    / [POV X]     / [wheel???]
          case 7:                         // [POV Y?]    / [POV Y]     / [wheel???]
            long x, y;          // they gotta be integers for exact 0 degrees or 90 degrees, else there are problems
            double pov;
            
            /// get axis from current pov position (wich is in degrees)
            pov= j[a].pov;
            x= y= 0;
            
            if(j[a].pov!= -1) {           /// ... only if it's not on -1 position (nothing selected)
              x= (double)(32767.0* sin(j[a].pov* (M_PI/ 180.0)));
              y= (double)(32767.0* cos(j[a].pov* (M_PI/ 180.0)));
            }
            
            /// update from event
            if(ev[b].number== 6)          /// x axis event
              x= ev[b].value;
            else                          /// y axis event
              y= -ev[b].value;
            
            /// find pov in degrees; there have to be checks for each quadrant, unfortunatelly (bad for speed)
            if(y> 0) {
              if(x>= 0)
                pov= (double) ((atan(x/ y))* (180.0/ M_PI));
              else
                pov= (double) ((2* M_PI+ atan(x/ y))* (180.0/ M_PI));
            } else if(y< 0) {
              pov= (double) (( M_PI+ atan(x/ y))* (180.0/ M_PI));
              
            } else if(y == 0) {
              if(x== 0)
                pov= -1;
              else if(x> 0)
                pov= 90;
              else if(x< 0)
                pov= 270;
            }
            
            /// pov found @ this point
            j[a].pov= pov;
            gp[a].pov= pov;
            // gw is not set<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            
            break;
          case 8:                         // [u axis]    / [u axis]    / [u axis]
            //gp[a].u= ev[b].value;
            
            break;
          case 9:                         // [v axis]    / [v axis]    / [v axis]
            //gp[a].v= ev[b].value;
            
          case 10:
            gp[a].lx= ev[b].value;
          case 11:
            gp[a].ly= ev[b].value;
          case 12:
            gp[a].rx= ev[b].value;
          case 13:
            gp[a].ry= ev[b].value;
            
            break;
            
        }
      } /// axis event

    } /// for each event
      
    // did it actually read 64 events? read another 64 then...
    if(nev == 64)
      goto ReadAgain;
    
    
  } /// for all active joysticks
  
  #endif /// OS_LINUX

  #ifdef USING_XINPUT //          ---- msdn copy/paste ...
  DWORD dwResult;    
  for(DWORD i= 0; i< XUSER_MAX_COUNT; i++ ) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    /// Simply get the state of the controller from XInput.  
    dwResult= XInputGetState(i, &state);

    if(dwResult == ERROR_SUCCESS)
      printf("XInput controller %d CONNECTED\n", i);
    else
      printf("XInput controller %d NOT connected\n", i);
  }
  #endif /// USING_XINPUT

  #ifdef USING_DIRECTINPUT
  #endif /// USING_DIRECTINPUT
  
  // mac has callback functions for stick/pad/wheel updates, they are at the back of the file

}


#ifdef USING_DIRECTINPUT
BOOL CALLBACK diDevCallback(LPCDIDEVICEINSTANCE inst, LPVOID extra) {
  HRESULT hr;
  short n= in.nr.jDI;

  hr= in.dInput->CreateDevice(inst->guidInstance, &in.getDIj(n)->diDevice, NULL);
  if (FAILED(hr))
    return DIENUM_CONTINUE;

  in.getDIgp(n)->diDevice= in.getDIgw(n)->diDevice= in.getDIj(n)->diDevice;

  in.nr.jDI++;
  in.nr.gpDI++;
  in.nr.gwDI++;
  
  in.nr.jFound++;
  in.nr.gpFound++;
  in.nr.gwFound++;

  return DIENUM_CONTINUE;
}
#endif /// USING_DIRECTINPUT


// ------------============= MOUSE CLASS ===========--------------------
// =====================================================================

Mouse::Mouse() {
  mode= 1;
  useDelta= false;

  x= y= dx= dy= oldx= oldy= wheel= 0;
  #ifdef USING_DIRECTINPUT
  diDevice= null;
  diStats= {0};
  #endif /// USING_DIRECTINPUT
}

Mouse::~Mouse() {
  delData();
}

void Mouse::delData() {
  mode= 1;
  useDelta= false;
  x= y= dx= dy= oldx= oldy= wheel= 0;

  #ifdef USING_DIRECTINPUT
  if(diDevice) {
    diDevice->Unacquire();
    diDevice->Release();
    diDevice= null;
  }
  #endif /// USING_DIRECTINPUT

}

bool Mouse::init(short mode) {
  delData();
  this->mode= mode;
  if(mode== 1) {          /// no init required atm
    return true;
  }
  if(mode== 2) {          /// no init required atm
    return true;
  }

  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    long hr= 0;
    DIDATAFORMAT mouseformat;
    mouseformat= c_dfDIMouse;
    mouseformat.dwFlags= DIDF_RELAXIS; 

    hr= in.dInput->CreateDevice(GUID_SysMouse, &diDevice, NULL);

    hr= diDevice->SetDataFormat(&mouseformat);
    hr= diDevice->SetCooperativeLevel(osi.win[0].hWnd, DISCL_EXCLUSIVE| DISCL_FOREGROUND);
    diDevice->Acquire();

    if(hr!= DI_OK) {
      error.simple("Could not initialize the mouse under DirectInput.");
      return false;
    }

    if(diDevice== NULL) {
      return false;
    }

    return true;		/// reached this point, returns a success
    #endif
    #ifndef USING_DIRECTINPUT
    error.simple("USING_DIRECTINPUT not defined; no code for direct input compiled");
    return false;
    #endif
  }
  return false;     /// this point is reaced-> fail
}

bool Mouse::aquire() {
  if(mode== 1) {
    #ifdef OS_LINUX
    XGrabPointer(osi.primWin->dis, osi.primWin->win,
                 True,              // send events or not
                 ButtonPressMask| ButtonReleaseMask| PointerMotionMask,
                 GrabModeSync,      // pointer - GrabMode[Async/Sync]
                 GrabModeSync,      // keyboard- GrabMode[Async/Sync]
                 osi.primWin->win,  // confine cursor to a window
                 None,              // mouse cursor (graphics)
                 CurrentTime);
    #endif /// OS_LINUX
    return true;
  }
  if(mode== 2)                    /// nothing to aquire
    return true;
  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Acquire()== DI_OK)
      return true;
    #endif
  }

  return false;
}

bool Mouse::unaquire() {
  if(mode== 1) {
    #ifdef OS_LINUX
    XUngrabPointer(osi.primWin->dis, CurrentTime);
    #endif
    return true;
  }
  if(mode== 2)
    return true;
  
  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Unacquire()== DI_OK)
      return true;
    #endif
  }
  return false;
}

void Mouse::resetButtons() {
  uint64 present; osi.getMillisecs(&present);
  for(short a= 0; a< MAX_MOUSE_BUTTONS; a++) {
    if(b[a].down) {
      b[a].lastTimeStart= b[a].timeStart;
      b[a].lastTimeEnded= present;
      b[a].lastDT= b[a].lastTimeEnded- b[a].lastTimeStart;
      b[a].timeStart= 0;
      b[a].down= false;
    }
  }
}

void Mouse::update() {
  if(!osi.flags.haveFocus)
    return;
/// os events: nothing to update, atm (i cant think of anything anyways)
  if(mode== 1) {

  }

/// manual update mode
  if(mode== 2) {
    #ifdef OS_WIN

    /// mouse position
    POINT p;
    GetCursorPos(&p);
    oldx= x;
    oldy= y;
    x= p.x;
    y= p.y;
    if(useDelta) {
      dx+= x- oldx;
      dy+= y- oldy;
    }
    
    /// mouse wheel
    
    // well, shit! gotta use window events, unfortunatly. 
    // NO WIN FUNC AVAIBLE TO READ THE WHEEL IN ANY WAY. the only way is too complicated, with hooks to dll-s... /cry

    /// mouse buttons
    uint64 present;
    osi.getMillisecs(&present);
    bool t;

    for(short a= 0; a< 5; a++) {                  /// for all mouse buttons (that windows knows ATM)
      if(a==0) t= GetAsyncKeyState(VK_LBUTTON)? true: false;        // LEFT BUTTON
      if(a==1) t= GetAsyncKeyState(VK_RBUTTON)? true: false;        // RIGHT BUTTON
      if(a==2) t= GetAsyncKeyState(VK_MBUTTON)? true: false;        // MIDDLE BUTTON
      if(a==3) t= GetAsyncKeyState(VK_XBUTTON1)? true: false;       // BUTTON 4
      if(a==4) t= GetAsyncKeyState(VK_XBUTTON2)? true: false;       // BUTTON 5

      if(b[a].down&& !t) {          /// button release
        b[a].lastTimeStart= b[a].timeStart;
        b[a].lastTimeEnded= present;
        b[a].lastDT= b[a].lastTimeEnded- b[a].lastTimeStart;
        b[a].down= t;
      }
      if((!b[a].down)&& t) {        /// button press started
        b[a].timeStart= present;
        b[a].down= t;
      }
    }

    #endif /// OS_WIN

    #ifdef OS_LINUX
    // cant find any funcs. events are the only option i can find
    //error.simple("Mouse cannot be manually updated under linux. use [MODE 1]");
    #endif /// OS_LINUX

    #ifdef OS_MAC       // <-----------------------------------
//    makeme
    #endif /// OS_MAC
  }

  /// direct input
  #ifdef USING_DIRECTINPUT           /// skip some checks. only mode 1 works atm in linux
  if(mode== 3) {
    
    diDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&diStats);
    /// mouse position
    oldx= x;
    oldy= y;
    x= diStats.lX;
    y= diStats.lY;
    if(useDelta) {
      dx+= x- oldx;
      dy+= y- oldy;
    }
    
    /// mouse wheel
    // THIS PART NEEDS TESTING, DUNNO WHAT DIRECT INPUT TRANMITS !!!!!!!!!!!!!!!  ALL DIRECT X PART, even. NEED A WORKING GLPRINT FUNC
    if(diStats.lZ)
      wheel++;
    else
      wheel--;

    /// mouse buttons
    bool t;
    int present= GetTickCount();
    for(short a= 0; a< 8; a++) {
      t= diStats.rgbButtons[a]? true: false;
      if((b[a].down)&& !t) {                  /// a button press ended
        b[a].lastTimeStart= b[a].timeStart;
        b[a].lastTimeEnded= present;
        b[a].lastDT= b[a].lastTimeEnded- b[a].lastTimeStart;
        b[a].down= t;
      }
      if((!b[a].down)&& t) {                  /// a button press starts
        b[a].timeStart= present;
        b[a].down= t;
      }
    }
  }
  #endif /// USING_DIRECTINPUT
}

Mouse::Button::Button() {
    lastDT= lastTimeStart= lastTimeEnded= 0;

    down= false;
    timeStart= 0;
}







// ------------============= KEYBOARD CLASS ===========--------------------
// ========================================================================

Keyboard::Keyboard():
charTyped(40, sizeof(chTyped)),
manipTyped(40, sizeof(chTyped)) {
  mode= 1;
  delData();
}

Keyboard::~Keyboard() {
//	delData();
  #ifdef USING_DIRECTINPUT
  if(diDevice) {
    diDevice->Unacquire();
    diDevice->Release();
    diDevice= null;
  }
  #endif
}

void Keyboard::delData() {
  key= buffer1;
  lastCheck= buffer2;
  capsLock= scrollLock= numLock= false;       /// the 3 toggle locks

  /// clear buffers
  for(short a= 0; a< MAX_KEYBOARD_KEYS; a++) {
    buffer1[a]= buffer2[a]= 0;
    keyTime[a]= 0;
    //    repeat[a]= 0;
  }
  clearTypedBuffer();                         /// character/manip streams too
  
  /// mark initial history keys as checked, so they get ignored
  for(short a= 0; a< MAX_KEYS_LOGGED; a++)
    lastKey[a].checked= true;
}



bool Keyboard::init(short mode) {
  this->mode= mode;
  if(mode== 1)
    return true;
  if(mode== 2)
    return true;
  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    long hr= 0;
        
    hr= in.dInput->CreateDevice(GUID_SysKeyboard, &diDevice, NULL);
    hr= diDevice->SetDataFormat(&c_dfDIKeyboard);
    hr= diDevice->SetCooperativeLevel(osi.win[0].hWnd, DISCL_EXCLUSIVE| DISCL_FOREGROUND);
    diDevice->Acquire();

    if(hr!= DI_OK) {
      error.simple("Could not initialize the keyboard under DirectInput.");
      return false;
    }

    if(diDevice== NULL)
      return false;

    return true;
    #endif /// USING_DIRECTINPUT
  }
  return false;
}


short Keyboard::getFirstKey() {
  for(short a= 0; a< MAX_KEYBOARD_KEYS; a++)
    if(key[a])
      return a;

  return 0;	/// fail
}

// debugging
void Keyboard::printPressed() {
  if((mode== 2) || (mode== 3))
    for(short a= 0; a< MAX_KEYBOARD_KEYS; a++)
      if(key[a]& 0x80)
        printf(" kc%d(%d)", a, key[a]);
}


void Keyboard::update() {
  
  if(!osi.flags.haveFocus)
    return;
  if(mode== 1)    // nothing to update, >>>>>>...... hmm maybe the LOCK STATES....<<<<<<<<<<<
    return;
  
  if(mode== 2) {
    #ifdef OS_WIN
    swapBuffers();			/// ye, swap them buffers
    GetKeyboardState(key);
    /// do not return;
    #endif /// OS_WIN
  }

  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    swapBuffers();
    /*if(*/ diDevice->GetDeviceState(MAX_KEYBOARD_KEYS, key); //== DIERR_INPUTLOST) { setActive(); return; } // alien engine
    /// do not return
    #endif
  }
  
  
  #ifdef OS_WIN // i can't think of a way atm, to use this in linux. the only mode is [mode 1]
  
  uint64 present;
  osi.getMillisecs(&present);

  /// check if a key just started to be pressed or a key was depressed - might be a little time consuming
  for(short a= 0; a< MAX_KEYBOARD_KEYS; a++)
    if((key[a]& 0x80)&& !(lastCheck[a]& 0x80) ) {         // key press start
      keyTime[a]= present;
      KeyPressed k;
      k.code= a;
      k.checked= false;
      k.timeDown= present;
      log(k);
    }
    else if((lastCheck[a]& 0x80)&& !(key[a]& 0x80)) {     // key depressed
      bool found= false;
      for(short b= 0; b< MAX_KEYS_LOGGED; b++)
        if(lastKey[b].code== a) {
          lastKey[b].timeUp= present;
          lastKey[b].timeDT= present- lastKey[b].timeDown;
          found= true;
          break;
        }
      if(!found) {
        KeyPressed k;
        k.code= a;
        k.checked= false;
        k.timeDown= present- 1;
        k.timeUp= present;
        k.timeDT= 1;
        log(k);                                     /// put it in history buffer
      }
    }
  #endif /// OS_WIN
}


void Keyboard::swapBuffers() {
  lastCheck= key;
  key= (key== buffer1)? buffer2: buffer1;
}


bool Keyboard::aquire() {
  if(mode== 1) {
    #ifdef OS_LINUX
    XGrabKeyboard(osi.primWin->dis, osi.primWin->win,
                  True,           // continue to send events or not
                  GrabModeSync,   // pointer - GrabMode[Async/Sync]
                  GrabModeSync,   // keyboard- GrabMode[Async/Sync]
                  CurrentTime);
    #endif /// OS_LINUX
    return true;
  }
  if(mode== 2)                    /// nothing to aquire
    return true;
  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Acquire()== DI_OK)
      return true;
    #endif
  }
  return false;
}


bool Keyboard::unaquire() {
  if(mode== 1) {
    #ifdef OS_LINUX
    XUngrabKeyboard(osi.primWin->dis, CurrentTime);
    #endif /// OS_LINUX
    return true;
  }
  if(mode== 2)
    return true;
  if(mode== 3) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Unacquire()== DI_OK)
      return true;
    #endif
  }
  return false;
}


void Keyboard::resetButtons() {
  uint64 present;
  osi.getMillisecs(&present);// <<----------------- there should be only one present var, set in osi, so there is only 1 call to this func. many calls= time wasted

  for(short a= 0; a< MAX_KEYBOARD_KEYS; a++) {
    if(key[a]) {
      KeyPressed k;
      k.code= a;
      k.checked= false;
      k.timeDown= keyTime[a];
      k.timeUp= present;
      k.timeDT= k.timeUp- k.timeDown;
      log(k);
    }
  }
}


void Keyboard::updateLocks() {
  #ifdef OS_WIN
  capsLock=   GetKeyState(VK_CAPITAL)& 0x01 == 1;
  numLock=    GetKeyState(VK_NUMLOCK)& 0x01 == 1;
  scrollLock= GetKeyState(VK_SCROLL)&  0x01 == 1;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  uint n;
  XkbGetIndicatorState(osi.primWin->dis, XkbUseCoreKbd, &n);
  capsLock=   (n& 0x01) == 1;
  numLock=    (n& 0x02) == 1;
  scrollLock= (n& 0x04) == 1;
  #endif /// OS_LINUX

  #ifdef OS_MAC
//  makeme
  #endif /// OS_MAC
}


void Keyboard::log(const Keyboard::KeyPressed &k) {
  for(short a= MAX_KEYS_LOGGED- 1; a> 0; a--)
    lastKey[a]= lastKey[a- 1];
  lastKey[0]= k;
}

/*
void Keyboard::logd(const Keyboard::KeyDown &k) {
  for(short a= MAX_KEYS_LOGGED- 1; a> 0; a--)
    lastKeyDown[a]= lastKeyDown[a- 1];
  lastKeyDown[0]= k;
}
*/

ulong Keyboard::getChar() {
  if(!charTyped.nrNodes)
    return 0;

  /// clear old typed characters (must have stayed in buffer longer than 1 sec)
  uint64 present;
  osi.getMillisecs(&present);
  
  while(charTyped.first)
    if( (present- ((chTyped*)charTyped.last)->time)> 1000)  // if character stayed in buffer longer than 1 second, delete. IS 1 SEC OK?????
      charTyped.del(charTyped.last);
    else                                                    /// the rest of chars couldn't have stayed in buffer longer than 1 sec if the last char didn't
      break;

  /// return the last character in buffer (usually there is only 1... maybe in cases of low framerate, loading stuff while typeing... etc)
  ulong ret= ((chTyped*)(charTyped.last))->c;
  charTyped.del(charTyped.last);                            /// del character from buffer
  return ret;
}


// basically same func as getChar, but for the other stream of string manipulation keys
ulong Keyboard::getManip() {
  if(!manipTyped.nrNodes)
    return 0;
  
  /// clear old typed characters (must have stayed in buffer longer than 1 sec)
  uint64 present;
  osi.getMillisecs(&present);
  
  while(manipTyped.first)
    if( (present- ((chTyped*)manipTyped.last)->time)> 1000) // if character stayed in buffer longer than 1 second, delete. IS 1 SEC OK?????
      manipTyped.del(manipTyped.last);
    else                                                    /// the rest of chars couldn't have stayed in buffer longer than 1 sec if the last char didn't
      break;

  /// return the last character in buffer (usually there is only 1... maybe in cases of low framerate, loading stuff while typeing... etc)
  ulong ret= ((chTyped*)(manipTyped.last))->c;
  manipTyped.del(manipTyped.last);                          /// del character from buffer
  return ret;
  
}


void Keyboard::addChar(ulong c, uint64 *time) {
  if(!c) return;

  /// clear old typed characters (must have stayed in buffer longer than 1 sec)
  while(charTyped.first)
    if( (*time- ((chTyped*)charTyped.last)->time)> 1000)  // if character stayed in buffer longer than 1 second, delete. IS 1 SEC OK?????
      charTyped.del(charTyped.last);
    else                                                    /// the rest of chars couldn't have stayed in buffer longer than 1 sec if the last char didn't
      break;

  /// add character to buffer
  chTyped *p= (chTyped*)charTyped.addFirst();
  p->c= c;
  p->time= *time;
}


// identical as addChar...
void Keyboard::addManip(ulong c, uint64 *time) {
  if(!c) return;

  /// clear old typed characters (must have stayed in buffer longer than 1 sec)
  while(manipTyped.first)
    if( (*time- ((chTyped*)manipTyped.last)->time)> 1000) // if character stayed in buffer longer than 1 second, delete. IS 1 SEC OK?????
      manipTyped.del(manipTyped.last);
    else                                                    /// the rest of chars couldn't have stayed in buffer longer than 1 sec if the last char didn't
      break;

  /// add character to buffer
  chTyped *p= (chTyped*)manipTyped.addFirst();
  p->c= c;
  p->time= *time;
}



void Keyboard::clearTypedBuffer() {
  while(charTyped.first)
    charTyped.del(charTyped.first);
  while(manipTyped.first)
    manipTyped.del(manipTyped.first);
}







///========================================================================///
// ------------============= JOYSTICK CLASS ===========-------------------- //
///========================================================================///

Joystick::Joystick() {
  mode= 0;

  #ifdef OS_LINUX
  file= -1;
  #endif /// OS_LINUX

  delData();
}

Joystick::~Joystick() {
  delData();
  #ifdef USING_DIRECTINPUT
  if(diDevice) {
    diDevice->Unacquire();
    diDevice->Release();
    diDevice= null;
  }
  #endif
}

void Joystick::delData() {
  mode= 0;
  name.delData();
  
  x= y= throttle= rudder= u= v= pov= 0;

  b= buffer1;
  lastCheck= buffer2;

  /// clear buffers
  for(short a= 0; a< MAX_JOYSTICK_BUTTONS; a++) {
    buffer1[a]= buffer2[a]= 0;
    bTime[a]= 0;
  }
  
  /// mark initial history buttons as checked, so they get ignored
  for(short a= 0; a< MAX_KEYS_LOGGED; a++)
    lastBut[a].checked= true;
  
  #ifdef OS_LINUX
  if(file!= -1)
    close(file);
  id= -1;
  #endif /// OS_LINUX
}

void Joystick::swapBuffers() {
  lastCheck= b;
  b= (b== buffer1)? buffer2: buffer1;
}

void Joystick::log(const ButPressed &k) {
  for(short a= MAX_KEYS_LOGGED- 1; a> 0; a--)
    lastBut[a]= lastBut[a- 1];
  lastBut[0]= k;
}




// ############### JOYSTICK UPDATE #################
void Joystick::update() {
  // TYPE1 joysticks
  if(mode== 1) {
    #ifdef OS_WIN
    JOYINFOEX jinfo;
    jinfo.dwSize= sizeof(JOYINFOEX);
    joyGetPosEx(id, &jinfo);

    x= jinfo.dwXpos;                  /// main stick x axis
    y= jinfo.dwYpos;                  /// main stick y axis

    throttle= jinfo.dwZpos;           // throttle? need a joystick...
    rudder= jinfo.dwRpos;             /// rudder

    pov= jinfo.dwPOV;                 /// pov in degrees* 100
    
    /// buttons
    for(ulong a= 0; a< MAX_JOYSTICK_BUTTONS; a++)
      b[a]= jinfo.dwButtons& (1<< a);           // JOY_BUTTONXX vals hopefully wont change!!!!!

    u= jinfo.dwUpos;                  /// extra axis 5
    v= jinfo.dwVpos;                  /// extra axis 6
    #endif /// OS_WIN

    #ifdef OS_LINUX

/*
    js_event msg[64];
    int n= -1;
    
    for(short a= 0; a< 8; a++) {
      NOPE
      n= read(   &msg, sizeof(msg), 1, jf);
    
      for(short a= 0; a< 16; a++) {
        if(msg.value)
          printf("%ud", msg.value);
      }
    } /// OS_LINUX
  */  
    
    #endif /// OS_LINUX
    
    // mac has callback functions for stick/pad/wheel updates, they are at the back of the file
  }
  
  // TYPE 2 JOYSTICKS
  #ifdef USING_DIRECTINPUT           /// skip some checks. only mode 1 works atm in linux
  if(mode== 2) {
  
    diDevice->GetDeviceState(sizeof(DIJOYSTATE2), (LPVOID)&diStats);
    /// left & right sticks
    x= diStats.lX;
    y= diStats.lY;
    throttle= diStats.lZ;
    rudder= diStats.lRz;
    /// POV...
    pov= diStats.rgdwPOV[0];
    /// buttons
    for(short a= 0; a< MAX_JOYSTICK_BUTTONS; a++)
      b[a]= diStats.rgbButtons[a];
    /// extra axis
    u= diStats.rglSlider[0];
    v= diStats.rglSlider[1];
  }
  #endif /// USING_DIRECTINPUT
  
  // TYPE 3 JOYSTICKS
  #ifdef USING_XINPUT           /// skip some checks. only mode 1 works atm in linux
  if(mode== 3) {
    makeme
  }
  #endif /// USING_XINPUT

  
}


bool Joystick::init(short mode) {
  this->mode= mode;

  if(mode== 1) {
    return true;
  }
  
  if(mode== 2) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->SetDataFormat(&c_dfDIJoystick2)== DI_OK) {
      if(diDevice->SetCooperativeLevel(osi.win[0].hWnd, DISCL_EXCLUSIVE| DISCL_FOREGROUND)!= DI_OK)
        error.simple("cant set cooperative level");
      return true;
    }
    #endif /// USING_DIRECTINPUT
  }
  
  if(mode== 3) {
    #ifdef USING_XINPUT
    #endif /// USING_XIMPUT
    return true;
  }

  return false;
}


#ifdef USING_DIRECTINPUT
BOOL CALLBACK EnumEffectsCallback(LPCDIEFFECTINFO di, LPVOID pvRef)
{
    //DInputFFB * ffb = (DInputFFB*) pvRef;


    // Pointer to calling device
    //ffb->ffbcaps.ConstantForce = DIEFT_GETTYPE(di->dwEffType) == DIEFT_CONSTANTFORCE;
    //ffb->ffbcaps.PeriodicForce = DIEFT_GETTYPE(di->dwEffType) == DIEFT_PERIODIC;
  
    printf(" Effect '%s'. IsConstant = %d, IsPeriodic = %d", di->tszName, DIEFT_GETTYPE(di->dwEffType) == DIEFT_CONSTANTFORCE, DIEFT_GETTYPE(di->dwEffType) == DIEFT_PERIODIC);

    return DIENUM_CONTINUE;
}
#endif






// ------------============= GAMEPAD CLASS ===========--------------------
// ========================================================================

GamePad::GamePad() {
  mode= 0;
  #ifdef USING_DIRECTINPUT
  vibration= null;
  #endif

  #ifdef OS_LINUX
  file= -1;
  #endif /// OS_LINUX
  delData();
}

GamePad::~GamePad() {
  #ifdef USING_DIRECTINPUT
  if(vibration) {
    vibration->Unload();
    vibration->Release();
    vibration= null;
  }
  /*
  if(diDevice) {
    diDevice->Unacquire();
    diDevice->Release();
    diDevice= null;
  }
  */
  #endif

  delData();
}

void GamePad::delData() {
  mode= 0;
  name.delData();
  
  /// clear axis
  lx= ly= 0;
  rx= ry= 0;
  lt= rt= 0;
  u= v= 0;
  pov= 0;


  b= buffer1;
  lastCheck= buffer2;

  /// clear buffers
  for(short a= 0; a< MAX_JOYSTICK_BUTTONS; a++) {
    buffer1[a]= buffer2[a]= 0;
    bTime[a]= 0;
  }
  
  /// mark initial history buttons as checked, so they get ignored
  for(short a= 0; a< MAX_KEYS_LOGGED; a++)
    lastBut[a].checked= true;
  
  #ifdef OS_LINUX
  if(file!= -1)
    close(file);
  id= -1;
  #endif /// OS_LINUX
}

void GamePad::swapBuffers() {
  lastCheck= b;
  b= (b== buffer1)? buffer2: buffer1;
}

void GamePad::log(const ButPressed &k) {
  for(short a= MAX_KEYS_LOGGED- 1; a> 0; a--)
    lastBut[a]= lastBut[a- 1];
  lastBut[0]= k;
}




bool GamePad::aquire() {
  if(mode== 2) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Acquire()== DI_OK)
      return true;
    #endif
  }
  return false;
}


bool GamePad::unaquire() {
  if(mode== 2) {
    #ifdef USING_DIRECTINPUT
    if(diDevice->Unacquire()== DI_OK)
      return true;
    #endif
  }
  return false;
}


void GamePad::update() {
  
  if(mode== 1) {
    // mac has callback functions for stick/pad/wheel updates, they are at the back of the file
    return;
  }
  
  #ifdef OS_WIN           /// skip some checks. only mode 1 works atm in linux
  if(mode== 2) {
  
    #ifdef USING_DIRECTINPUT
    diDevice->GetDeviceState(sizeof(DIJOYSTATE2), (LPVOID)&diStats);
    /// left & right sticks
    lx= diStats.lX;
    ly= diStats.lY;
    rx= diStats.lZ;
    ry= diStats.lRz;
    /// POV...
    pov= diStats.rgdwPOV[0];
    /// triggers
    lt= diStats.lRx;
    rt= diStats.lRy;
    /// buttons
    for(short a= 0; a< MAX_JOYSTICK_BUTTONS; a++)
      b[a]= diStats.rgbButtons[a];
    /// extra axis
    u= diStats.rglSlider[0];
    v= diStats.rglSlider[1];
    return;
    #endif /// USING_DIRECTINPUT
  
  }
  
  if(mode== 3) {
    return;
  }
  #endif /// OS_WIN
}


bool GamePad::init(short mode) {
  this->mode= mode;
  
  if(mode== 1) {
    return true;
  }
  
  if(mode== 2) {
  #ifdef OS_WIN
  #ifdef USING_DIRECTINPUT
    return true;	// joystick initializes, it is the same device
    /*
    if(diDevice->SetDataFormat(&c_dfDIJoystick2)== DI_OK) {
      diDevice->SetCooperativeLevel(osi.win[0].hWnd, DISCL_EXCLUSIVE| DISCL_FOREGROUND);
      return true;
    }
    */
  #endif /// USING_DIRECTINPUT
  #endif /// OS_WIN
  }
  if(mode== 3) {
    return true;
  }
  
  return false;
}






///========================================================================///
// ------------============= GAMEWHEEL CLASS ===========------------------- //
///========================================================================///

GameWheel::GameWheel() {
  mode= 0;
  
  #ifdef OS_LINUX
  id= -1;
  file= null;
  #endif /// OS_LINUX
  delData();
}

GameWheel::~GameWheel() {
  #ifdef USING_DIRECTINPUT
  /*
  if(diDevice) {
    diDevice->Unacquire();
    diDevice->Release();
    diDevice= null;
  }
  */
  #endif

  delData();
}

void GameWheel::delData() {
  mode= 0;
  name.delData();
  
  /// clear axis
  wheel= 0;
  a1= a2= a3= a4= a5= 0;      // THIS NEEDS MORE WORK


  b= buffer1;
  lastCheck= buffer2;

  /// clear buffers
  for(short a= 0; a< MAX_JOYSTICK_BUTTONS; a++) {
    buffer1[a]= buffer2[a]= 0;
    bTime[a]= 0;
  }
  
  /// mark initial history buttons as checked, so they get ignored
  for(short a= 0; a< MAX_KEYS_LOGGED; a++)
    lastBut[a].checked= true;

  #ifdef OS_LINUX
  if(file!= -1)
    close(file);
  id= -1;
  #endif /// OS_LINUX
}

void GameWheel::swapBuffers() {
  lastCheck= b;
  b= (b== buffer1)? buffer2: buffer1;
}

void GameWheel::log(const ButPressed &k) {
  for(short a= MAX_KEYS_LOGGED- 1; a> 0; a--)
    lastBut[a]= lastBut[a- 1];
  lastBut[0]= k;
}





bool GameWheel::init(short mode) {
  this->mode= mode;
  
  if(mode== 1) {
    return true;
  }
  
  if(mode== 2) {
    #ifdef USING_DIRECTINPUT
    return true;		// joystick initializes it is the same device
    /*
    if(diDevice->SetDataFormat(&c_dfDIJoystick2)== DI_OK) {
      diDevice->SetCooperativeLevel(osi.win[0].hWnd, DISCL_EXCLUSIVE| DISCL_FOREGROUND);
      return true;
    }
    */
    #endif /// USING_DIRECTINPUT
  }
  
  if(mode== 3) {
    #ifdef USING_XINPUT

    #endif /// USING_XINPUT
    return true;
  }

  return false;
}



















// MAC specific fuctions. 

#ifdef OS_MAC 

///----------------///
// HIDDriver STRUCT //
///----------------///

/// part of HIDDriver structure. each 'element' can be a button or axis in a stick/pad/wheel
struct HIDElement {
  // SUBJECT OF DELETION  
  char type;                    /// 1= axis, 2= button THIS MIGHT GO AWAY/ usagePage IS A BETTER 'TYPE'
  
  short id;                     /// button or axis number (0 - max buttons / 0 - max axis)
  long usage, usagePage;        /// [most inportant] characteristics about this element (is it an x axis, is it a button etc)
  long logicalMin;              /// minimum value it can have
  long logicalMax;              /// maximum value it can have
  bool hasNULLstate;

  bool isHat;                   /// is it a hat switch / dPad
  bool hatMultiAxis;            /// if the hat has 2 axis (there are some sticks that have complex hats)
  bool hatAxis1;                /// [hat multi axis only] if it is the first axis (x)
  bool hatAxis2;                /// [hat multi axis only] if it is the second axis (y)
  HIDElement(): type(0), id(0), logicalMin(0), logicalMax(0), hasNULLstate(false), isHat(false), hatMultiAxis(false), hatAxis1(false), hatAxis2(false) {}
};

/// macs lack a proper joystick api; the next struct is a helper to 'decode' the mess that is almost raw reading from a HID device
struct HIDDriver {              /// [internal]
  bool inUse;                   /// is this in use?
  IOHIDDeviceRef device;        /// coresponding 'device' that this stick/pad/wheel is tied to
  short nrButtons;              /// number of buttons that this stick/pad/wheel has
  short nrAxis;                 /// number of axis this stick/pad/wheel has
  HIDElement *elem;             /// array of elements the device has
  
  /// standard constructor/destructor/delData(); everything will be set to 0 and memory will be auto-deallocated if allocated
  HIDDriver(): inUse(false), device(null), nrButtons(0), nrAxis(0), elem(0) {}
  void delData() { if(elem) delete[] elem; elem= null; nrButtons= nrAxis= 0; inUse= false; device= null; }
  ~HIDDriver() { delData(); }
} driver[MAX_JOYSTICKS];        /// [internal]




/// NO DAMN CLUE HOW IT IS POSSIBLE TO CREATE SOMETHING LIKE THIS FOR 2 VARIABLES. ARE THEY PAYED FOR THE NUMBER OF CHARACTERS THEY TYPE?
// COPY-PASTE FROM APPLE 'DOCUMENTATION' ...lol
// function to create matching dictionary
static CFMutableDictionaryRef hu_CreateDeviceMatchingDictionary( UInt32 inUsagePage, UInt32 inUsage) {

  // create a dictionary to add usage page/usages to
  CFMutableDictionaryRef result= CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  // Add key for device type to refine the matching dictionary.
  CFNumberRef pageCFNumberRef= CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsagePage);
  CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
  CFRelease(pageCFNumberRef);
        
  // note: the usage is only valid if the usage page is also defined
  CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsage);
  CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);
  
  CFRelease(usageCFNumberRef);
  return result;
} /// hu_CreateDeviceMatchingDictionary



// CALLBACK FUNCTION: when a device is plugged in ---------------------------------
///================================================================================
static void HIDadded(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
  // context:   context from IOHIDManagerRegisterDeviceMatchingCallback
  // result:    the result of the matching operation
  // sender:    the IOHIDManagerRef for the new device
  // device: the new HID device

  // IOHIDDeviceRegisterInputValueCallback(device, HIDchange, &driver[a]); this could further optimize some code, but very little, by passing &driver[a] after it is created...
  
  bool chatty= false;
  if(chatty) printf("%s\n", __FUNCTION__);

  /// find the first non-in-use joystick
  short a;
  for(a= 0; a< MAX_JOYSTICKS; a++)
    if(!driver[a].inUse)
      break;
  
  // start to 'populate' vars inside the helper struct

  /// sticks/pads/wheels 'numbers'
  in.nr.jFound++;  in.nr.jOS++;
  in.nr.gpFound++; in.nr.gpOS++;
  in.nr.gwFound++; in.nr.gwOS++;
  
  /// rest of vars
  driver[a].inUse= true;  
  driver[a].device= device;
  in.j[a].mode= 1;              /// set it's mode to 1. in macs i think only mode 1 will be avaible... there is no freaking support for HIDs
  in.gp[a].mode= 1;
  in.gw[a].mode= 1;
  
  /// stick/pad/wheel name (product name) ... this should be easy, right?...
  CFStringRef name;
  name= (CFStringRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));

  if(name && (CFGetTypeID(name)== CFStringGetTypeID())) {
    CFIndex length;
		
		CFStringGetBytes(name, CFRangeMake(0, CFStringGetLength(name)), kCFStringEncodingUTF8, '?', false, NULL, 100, &length);
    
    in.j[a].name.d= new char[length+ 1];
                             
		CFStringGetBytes(name, CFRangeMake(0, CFStringGetLength(name)), kCFStringEncodingUTF8, '?', false, (UInt8 *) in.j[a].name.d, length+ 1, NULL);
		in.j[a].name.d[length]= 0;  /// terminator
    in.j[a].name.updateLen();
    
    in.gp[a].name= in.j[a].name;
    in.gw[a].name= in.j[a].name;
  } else {
    in.j[a].name= "Unknown";
    in.gp[a].name= in.j[a].name;
    in.gw[a].name= in.j[a].name;
  }
  
  /// not used ATM:
  // IOHIDDeviceGetVendorID(device);
  // IOHIDDeviceGetProductID(device);
  
  
  CFArrayRef elems;               /// array with all elements
	IOHIDElementRef elem;           /// 1 element
	IOHIDElementType type;          /// button / axis
	IOHIDElementCookie cookie;      /// cookies are element IDs basically. they point to first element as 1, not as 0, tho!
  /// get all elements the device has
	elems= IOHIDDeviceCopyMatchingElements(device, NULL, kIOHIDOptionsTypeNone);
  
  driver[a].elem= new HIDElement[CFArrayGetCount(elems)];
  
  /// populate driver struct
  short c= 0, d= 0; /// c will hold counter for axis, d for buttons
  for(short b= 0; b< CFArrayGetCount(elems); b++) {
		elem= (IOHIDElementRef)CFArrayGetValueAtIndex(elems, b);
		type= IOHIDElementGetType(elem);
    cookie= IOHIDElementGetCookie(elem); /// cookies represent a unique identifier for a HID element within a HID device.

    /// unfortunately type for axis is screwed... some axis are put into Misc by mac HIDapi...
		if(type== kIOHIDElementTypeInput_Misc || type== kIOHIDElementTypeInput_Axis) {
      driver[a].elem[b].type= 1;
      driver[a].elem[b].id= c++;
      driver[a].elem[b].usagePage=    IOHIDElementGetUsagePage(elem);
      driver[a].elem[b].usage=        IOHIDElementGetUsage(elem);
      driver[a].elem[b].logicalMin=   IOHIDElementGetLogicalMin(elem);
			driver[a].elem[b].logicalMax=   IOHIDElementGetLogicalMax(elem);
			driver[a].elem[b].hasNULLstate= IOHIDElementHasNullState(elem);
			driver[a].elem[b].isHat=        IOHIDElementGetUsage(elem)== kHIDUsage_GD_Hatswitch;
      
      /// if this is a hat / dPad, check if it is a multi-axis hat
      if(driver[a].elem[b].isHat) {
        if(b!= 0)                                     /// if it's the first element, this will avoid a nice crash
          if(driver[a].elem[b- 1].isHat) {            /// if the prev element is a hat / dPad too, then this is a multi-axis hat / dPad
            driver[a].elem[b- 1].hatMultiAxis= true;  /// set previous element as a multiAxis
            driver[a].elem[b- 1].hatAxis1= true;      /// set previous element as axis 1
            driver[a].elem[b].hatMultiAxis= true;     /// set current element as a multiAxis
            driver[a].elem[b].hatAxis2= true;         /// set current element as axis 2
          }
      } /// if this is a hat
      
    /// buttons
    } else if(type== kIOHIDElementTypeInput_Button) {
      driver[a].elem[b].type= 2;
      driver[a].elem[b].id= d++;
    }
    if(chatty) printf("element[%d]: cookie[%d] id[%d] type[%d] min[%ld] max[%ld] hasNULL[%d] isHat[%d]\n", b, cookie, driver[a].elem[b].id, driver[a].elem[b].type, driver[a].elem[b].logicalMin, driver[a].elem[b].logicalMax, driver[a].elem[b].hasNULLstate, driver[a].elem[b].isHat);

	} /// for each element

	CFRelease(elems);     /// release elements array
	if(chatty) printf("device[%s] nrButtons[%d] nrAxis[%d]\n", in.j[a].name.d, driver[a].nrButtons, driver[a].nrAxis);
  

} /// HIDadded




// CALLBACK FUNCTION: when a device is removed ------------------------------------
///================================================================================
static void HIDremoved(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
  // inContext:   context from IOHIDManagerRegisterDeviceMatchingCallback
  // inResult:    the result of the removing operation
  // inSender:    the IOHIDManagerRef for the device being removed
  // inHIDDevice: the removed HID device

  bool chatty= true;
  if(chatty) printf("%s", __FUNCTION__);

  /// find removed device in 'driver' struct
  short a;
  for(a= 0; a< MAX_JOYSTICKS; a++)
    if(driver[a].device== device)
      break;
  
  if(a== MAX_JOYSTICKS) {     /// if not found... well... something is wrong...
    error.simple("HIDremoved: can't find the requested device");
    return;
  }
  
  driver[a].delData();
  if(chatty) printf(" helper cleared\n");
} /// HIDremoved



// CALLBACK FUNCTION: any value in a device (axis/button) has changed -------------
///================================================================================
static void HIDchange(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef val) {
  // inContext:       context from IOHIDManagerRegisterInputValueCallback
  // IinResult:       completion result for the input value operation
  // inSender:        the IOHIDManagerRef
  // inIOHIDValueRef: the new element value
  
  // REMEMBER: this function must be optimized for speed, as it is called many times over;
  
  // further testing: it seems there is d-pad button pressure measurements... same with most of buttons !!!!
  
  bool chatty= false;
  
  IOHIDElementRef elem= IOHIDValueGetElement(val);        /// get the 'element' thet of the value
  IOHIDDeviceRef device= IOHIDElementGetDevice(elem);     /// get the HID device that a element belongs to
  IOHIDElementCookie cookie= IOHIDElementGetCookie(elem)- 1; /// cookies represent a unique identifier for a HID element (also first element they point to is 1, and the list starts with 0)

  /// find the stick/pad/wheel this change belongs to
  short a;
  for(a= 0; a< in.nr.jOS; a++)
    if(driver[a].device== device)         /// found it
      break;
  
  HIDElement *e= &driver[a].elem[cookie]; /// name shortcut
  
  if(!e->type) return;                    /// only type1(axis) and type2(butons) are handled. rest, return quick
    
  /// value translation to -32767 : +32767
  long amin= e->logicalMin< 0? -e->logicalMin: e->logicalMin; /// faster than calling abs()
  long amax= e->logicalMax< 0? -e->logicalMax: e->logicalMax; /// faster than calling abs()

  if((e->type== 1) && amin+ amax == 0)    /// quick way to eliminate trash (dunno what other 'axis' that mac reports are) / would be div by zero error too
    return;
  
  long v= IOHIDValueGetIntegerValue(val); /// the actual value that changed
  //  double v2= IOHIDValueGetScaledValue(val, kIOHIDValueScaleTypeCalibrated); /// it could be used
  //  double v3= IOHIDValueGetScaledValue(val, kIOHIDValueScaleTypePhysical);   /// i saw there are not big changes from v

  /// Returns the timestamp value associated with this HID value reference.
  /// !!! these timestamps are using mach too, so they are reliable !!!
  uint64_t time= IOHIDValueGetTimeStamp(val)/ 1000000;    /// convert to millisecs too (from nanosecs)
  
  if(chatty)  printf("%s", in.j[a].name.d);

  if(e->usagePage== kHIDPage_GenericDesktop) {            // ---=== axis ===---
    long t= (((v+ amin)* 65534)/ (amin+ amax))- 32767;   /// value scaled to min[-32767] max[+32767], 65535 total possible units (65534+ unit 0)
    
    switch(e->usage) {
      case kHIDUsage_GD_X:                       // [X axis?]   / [l stick X] / [wheel???]
        in.gp[a].lx= (t> -150 && t< 150)? 0: t; /// this is due to bug in mac HID api. center position is not centered.
        in.j[a].x= in.gw[a].wheel= in.gp[a].lx;  // game wheel actual wheel might be this one <<< TEST
        if(chatty) printf(" lStick[%ld]x\n", in.gp[a].lx);
        return;
      case kHIDUsage_GD_Y:                       // [Y axis?]   / [l stick Y] / [wheel???]
        in.gp[a].ly= (t> -150 && t< 150)? 0: t; /// this is due to bug in mac HID api. center position is not centered.
        in.j[a].y= in.gw[a].a1= in.gp[a].ly;
        if(chatty) printf(" lStick[%ld]y\n", in.gp[a].ly);
        return;
      case kHIDUsage_GD_Z:                       // [Throttle?] / [r stick X] / [wheel???]
        in.gp[a].rx= (t> -150 && t< 150)? 0: t; /// this is due to bug in mac HID api. center position is not centered.
        in.j[a].throttle= in.gw[a].a2= in.gp[a].rx;
        if(chatty) printf(" rStick[%ld]x\n", in.gp[a].rx);
        return;
      case kHIDUsage_GD_Rx:                      // [extra1 X?] / [l trigger] / [wheel???]
        in.gp[a].lt= 32767- t;
        in.j[a].u= in.gw[a].a4= (t> -150 && t< 150)? 0: t;
        if(chatty) printf(" lTrigger[%ld]\n", in.gp[a].lt);
        return;
      case kHIDUsage_GD_Ry:                      // [extra1 Y?] / [r trigger] / [wheel???]
        in.gp[a].rt= 32767- t;
        in.j[a].v= in.gw[a].a5= (t> -150 && t< 150)? 0: t;
        if(chatty) printf(" rTrigger[%ld]\n", in.gp[a].rt);
        return;
      case kHIDUsage_GD_Rz:                      // [Rudder?]   / [rStick Y]  / [wheel???]
        in.gp[a].ry= (t> -150 && t< 150)? 0: t; /// this is due to bug in mac HID api. center position is not centered.
        in.j[a].rudder= in.gw[a].a3= in.gp[a].ry;
        if(chatty) printf(" rStick[%ld]y\n", in.gp[a].ry);
        return;
      case kHIDUsage_GD_Hatswitch:
        if(!e->hatMultiAxis) {                   // gamePad dPads have only 1 axis
          if(v> e->logicalMax|| v< e->logicalMin) {
            in.gp[a].pov= -1;
            in.j[a].pov= -1;
            // game wheels can have povs, i guess, dunno
          } else {
            in.gp[a].pov= (360/ (e->logicalMax+ 1))* v; /// in degrees
            in.j[a].pov= in.gp[a].pov;
            // game wheels can have povs, i guess, dunno
          }
          if(chatty) printf(" pov[%ld]\n", in.gp[a].pov);
        } else {                                 // multi-axis hat switch. CAN'T TEST THESE AS I DO NOT HAVE ONE
          //if(chatty) printf(" multi-axis hat switch not handled ATM... gotta buy one first!\n");
          // THIS CODE IS NOT TESTED, as i have no joystick with 2 axis hat switch
          if(e->hasNULLstate)
            if((v< e->logicalMin) && (v> e->logicalMax)) {
              in.j[a].pov= -1;
              in.gp[a].pov= -1;
              if(chatty) printf(" pov[%ld]\n", in.gp[a].pov);
              return;
              // GAMEWHEEL NOT DONE <<<<<<<<<<<<<<<
            }
            
          long x, y;                        /// they gotta be ints for exact 0 degrees or 90 degrees, else there are problems
          double pov;
          
          /// get axis from current pov position (wich is in degrees)
          pov= in.j[a].pov;
          x= y= 0;
          
          if(in.j[a].pov!= -1) {            /// ... only if it's not on -1 position (nothing selected)
            x= (double)(32767.0* sin(in.j[a].pov* (M_PI/ 180.0)));
            y= (double)(32767.0* cos(in.j[a].pov* (M_PI/ 180.0)));
          }
          
          /// update from changed value (v)
          long t= (((v+ amin)* 65534)/ (amin+ amax))- 32767;
          if(e->hatAxis1)                   /// this is x axis
            x= (t> -150 && t< 150)? 0: t;
          else                              /// else is y axis
            y= (t> -150 && t< 150)? 0: -t;  /// 'up' y axis, is negative; this needs to be changed 
          
          /// find pov in degrees; there have to be checks for each quadrant, unfortunatelly (bad for speed)
          if(y> 0) {
            if(x>= 0)
              pov= (double) ((atan(x/ y))* (180.0/ M_PI));
            else
              pov= (double) ((2* M_PI+ atan(x/ y))* (180.0/ M_PI));
          } else if(y< 0) {
            pov= (double) (( M_PI+ atan(x/ y))* (180.0/ M_PI));
            
          } else if(y == 0) {
            if(x== 0)
              pov= -1;
            else if(x> 0)
              pov= 90;
            else if(x< 0)
              pov= 270;
          }
          
          /// pov found @ this point
          in.j[a].pov= pov;
          in.gp[a].pov= pov;
          // gw is not set<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          
          if(chatty) printf(" pov[%ld]\n", in.gp[a].pov);
        } /// if multi-axis hat switch / pov / dPad / watever other name they come up with
        return;
      default:
        if(chatty) printf(" unhandled generic desktop element usage[%lx]\n", e->usage);
    } /// axis usage switch

  } else if(e->type== 2) {                                // ---=== button ===---
    /// have to use the 'type' variable, wich is the only place it actually works... 
    in.j[a].b[e->id]= (uchar)v;
    in.gp[a].b[e->id]= (uchar)v;
    in.gw[a].b[e->id]= (uchar)v;
    
    if(v== 0) {                                           // release
      if(chatty) printf(" button release [%d]\n", e->id);
      in.j[a].bTime[e->id]=  time;
      in.gp[a].bTime[e->id]= time;
      in.gw[a].bTime[e->id]= time;
    } else if(v== 1) {                                    // press
      if(chatty) printf(" button press [%d]\n", e->id);
      /// put the button in history
      ButPressed p;
      p.b= e->id;
      p.checked= false;
      p.timeDown= in.j[a].bTime[e->id];
      p.timeUp= time;
      p.timeDT= p.timeUp- p.timeDown;
      
      in.j[a].log(p);
      in.gp[a].log(p);
      in.gw[a].log(p);
    } /// button press/release if

  } else if(e->usagePage== kHIDPage_VendorDefinedStart) { // ---=== button pressure / other vendor specifics ===---
    long t;
    if(e->usage>= 0x20 && e->usage<= 0x2B)/// button pressure range
      t= (((v+ amin)* 65534)/ (amin+ amax))- 32767;     /// value scaled to min[-32767] max[+32767], 65535 total possible units (65534+ unit 0)    
    switch(e->usage) {
        
      // ---=== button pressures ===---
      case 0x20:                          /// gamepad button pov right
        in.j[a].bPressure[20]= in.gw[a].bPressure[20]= in.gp[a].bPressure[20]= t+ 32767; 
        if(chatty) printf(" but 20 pressure[%ld]\n", in.gp[a].bPressure[20]);
        return;
      case 0x21:                          /// gamepad button pov left
        in.j[a].bPressure[21]= in.gw[a].bPressure[21]= in.gp[a].bPressure[21]= t+ 32767; 
        if(chatty) printf(" but 21 pressure[%ld]\n", in.gp[a].bPressure[21]);
        return;
      case 0x22:                          /// gamepad button pov up
        in.j[a].bPressure[22]= in.gw[a].bPressure[22]= in.gp[a].bPressure[22]= t+ 32767;
        if(chatty) printf(" but 22 pressure[%ld]\n", in.gp[a].bPressure[22]);
        return;
      case 0x23:                          /// gamepad button pov down
        in.j[a].bPressure[23]= in.gw[a].bPressure[23]= in.gp[a].bPressure[23]= t+ 32767;
        if(chatty) printf(" but 23 pressure[%ld]\n", in.gp[a].bPressure[23]);
        return;
      case 0x24:                          /// gamepad button 3
        in.j[a].bPressure[3]= in.gw[a].bPressure[3]= in.gp[a].bPressure[3]= t+ 32767;
        if(chatty) printf(" but 3 pressure[%ld]\n", in.gp[a].bPressure[3]);
        return;
      case 0x25:                          /// gamepad button 2
        in.j[a].bPressure[2]= in.gw[a].bPressure[2]= in.gp[a].bPressure[2]= t+ 32767;
        if(chatty) printf(" but 2 pressure[%ld]\n", in.gp[a].bPressure[2]);
        return;
      case 0x26:                          /// gamepad button 1
        in.j[a].bPressure[1]= in.gw[a].bPressure[1]= in.gp[a].bPressure[1]= t+ 32767;
        if(chatty) printf(" but 1 pressure[%ld]\n", in.gp[a].bPressure[1]);
        return;
      case 0x27:                          /// gamepad button 0
        in.j[a].bPressure[0]= in.gw[a].bPressure[0]= in.gp[a].bPressure[0]= t+ 32767;
        if(chatty) printf(" but 0 pressure[%ld]\n", in.gp[a].bPressure[0]);
        return;
      case 0x28:                          /// gamepad button 4
        in.j[a].bPressure[4]= in.gw[a].bPressure[4]= in.gp[a].bPressure[4]= t+ 32767;
        if(chatty) printf(" but 4 pressure[%ld]\n", in.gp[a].bPressure[4]);
        return;
      case 0x29:                          /// gamepad button 5
        in.j[a].bPressure[5]= in.gw[a].bPressure[5]= in.gp[a].bPressure[5]= t+ 32767;
        if(chatty) printf(" but 5 pressure[%ld]\n", in.gp[a].bPressure[5]);        
        return;
      case 0x2A:                          /// gamepad button 6
        in.j[a].bPressure[6]= in.gw[a].bPressure[6]= in.gp[a].bPressure[6]= t+ 32767;
        if(chatty) printf(" but 6 pressure[%ld]\n", in.gp[a].bPressure[6]);
        return;
      case 0x2B:                          /// gamepad button 7
        in.j[a].bPressure[7]= in.gw[a].bPressure[7]= in.gp[a].bPressure[7]= t+ 32767;
        if(chatty) printf(" but 7 pressure[%ld]\n", in.gp[a].bPressure[7]);
        return;
      default:
        if(chatty) printf(" unhandled vendor specific element usage[%lx]\n", e->usage);
    }
    
  } else if(e->usagePage== kHIDPage_Button) {             // ---=== ofc, this page doesn't work for game HIDs ===---
    /// this page is NOT USED (for gamepads at least, so i guess it is not used for any game HID)
    if(chatty) printf(" unhandled usagePage == button\n");

    
  } else
    if(chatty) printf(" unhandled HID element: usagePage[%lx] usage[%lx]\n", e->usagePage, e->usage);
  
  

  /* SOME DOCUMENTATION:
  // usage and usage pages are defined on the USB website at: <http://www.usb.org> [couldn't find them, ofc. use IOHIDUsageTables.h]
  uint32_t page = IOHIDElementGetUsagePage(elem);
  uint32_t usage = IOHIDElementGetUsage(elem);
  
  // Boolean properties
  Boolean isVirtual = IOHIDElementIsVirtual(elem);
  Boolean isRelative = IOHIDElementIsRelative(elem);
  Boolean isWrapping = IOHIDElementIsWrapping(elem);
  Boolean isArray = IOHIDElementIsArray(elem);
  Boolean isNonLinear = IOHIDElementIsNonLinear(elem);
  Boolean hasPreferred = IOHIDElementHasPreferredState(elem);
  Boolean hasNullState = IOHIDElementHasNullState(elem);
  
  // the HID element name
  CFStringRef name= IOHIDElementGetName(elem);
  
  /// no clue what these are  
  // element report information
  uint32_t reportID = IOHIDElementGetReportID(elem);
  uint32_t reportSize = IOHIDElementGetReportSize(elem);
  uint32_t reportCount = IOHIDElementGetReportCount(elem);
  
  // element unit & exponent
  uint32_t unit = IOHIDElementGetUnit(elem);
  uint32_t unitExp = IOHIDElementGetUnitExponent(elem);

  //There are also functions to determine the device, parent, and child of a specified HID element:
  
  // return the collection element that a HID element belongs to (if any)
  IOHIDElementRef parent= IOHIDElementGetParent(elem);
  
  // return the child elements of a collection element (if any)
  CFArrayRef tCFArrayRef= IOHIDElementGetChildren(elem);
   
   // check more on this (another guy that digged thru this nightmare):
   // http://sacredsoftware.net/svn/misc/StemLibProjects/gamepad/trunk/source/gamepad/Gamepad_macosx.c
*/
    
} /// Handle_IOHIDInputValueCallback


/* NOT USING, can't find no documentation about reports ATM
static void HIDchange(void *context, IOReturn result, void *sender, IOHIDReportType type, uint32_t reportID, uint8_t *report, CFIndex reportLength) {
  // context:       void * pointer to your data, often a pointer to an object.
  // result:        Completion result of desired operation.
  // refcon:        void * pointer to more data.
  // sender:        Interface instance sending the completion routine.
  // type:          The type of the report that was completed.
  // reportID:      The ID of the report that was completed.
  // report:        Pointer to the buffer containing the contents of the report.
  // reportLength:  Size of the buffer received upon completion.

  printf("%s", __FUNCTION__);
  printf(" type[%lu] reportID[%u] reportLength[%d] report[%s]\n", type, reportID, reportLength, report);
}
*/
#endif







