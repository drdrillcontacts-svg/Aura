/*
 * ============================================================================
 *  Minimal X11 *compile-time* stub headers for the AURA project.
 *
 *  PURPOSE
 *  -------
 *  JUCE's GUI modules require the X11 development headers (libx11-dev) to
 *  compile on Linux. JUCE only *links* X11 at runtime via dlopen()
 *  (see juce_XSymbols_linux.cpp), so at compile time only the type,
 *  function and constant *declarations* are needed.
 *
 *  This stub provides exactly those declarations. It is used ONLY when the
 *  real libx11-dev headers are not installed (headless CI / sandbox
 *  machines). On a machine with the real X11 headers, they take
 *  precedence and this directory is not added to the include path.
 *
 *  The resulting Linux binaries are for CI verification only; they will not
 *  function without a real X server and libX11.so.6 (which is expected on
 *  any real desktop system).
 *
 *  The layout of the structs follows the standard X11 (Xlib.h) layout so
 *  that code compiled against these stubs would also be layout-compatible
 *  with the real headers.
 *
 *  All the X11 sub-headers (Xatom.h, Xutil.h, Xresource.h, Xmd.h, keysym.h,
 *  XKBlib.h, cursorfont.h) simply include this file.
 * ============================================================================
 */
#pragma once

#include <stddef.h>

#if defined (_MSC_VER)
 #define X11_STUB_EXTERN_C
#else
 #define X11_STUB_EXTERN_C extern "C"
#endif

X11_STUB_EXTERN_C
{

/* ----------------------------------------------------------------------------
 * Basic types
 * -------------------------------------------------------------------------- */
typedef int            Bool;
typedef int            Status;
typedef unsigned char* XPointer;
typedef unsigned long  XID;
typedef XID            Window;
typedef XID            Pixmap;
typedef XID            Cursor;
typedef XID            Colormap;
typedef XID            Drawable;
typedef XID            Font;
typedef unsigned long  Atom;
typedef unsigned long  Time;
typedef unsigned long  KeySym;
typedef unsigned char  KeyCode;
typedef unsigned long  XContext;
typedef int            XIM;
typedef int            XIC;
typedef int (*XIMProc) (void*);

struct _XDisplay;
typedef struct _XDisplay Display;
struct _XScreen;
typedef struct _XScreen Screen;
struct _XVisual
{
    long red_mask;
    long green_mask;
    long blue_mask;
    long all_masks;
    int bits_per_rgb;
    int colormap_entries;
    int visual_class;
    int white_point;
    int gamma;
    int black_point;
    int nplanes;
};
struct _XVisual;
typedef struct _XVisual Visual;
struct _XGC;
typedef struct _XGC *GC;
typedef unsigned long VisualID;
struct _XFontStruct;
typedef struct _XFontStruct XFontStruct;

/* None */
#define None 0L

/* Event type numbers */
#define NoEvent            0
#define KeyPress           2
#define KeyRelease         3
#define ButtonPress        4
#define ButtonRelease      5
#define MotionNotify       6
#define EnterNotify        7
#define LeaveNotify        8
#define FocusIn            9
#define FocusOut           10
#define KeymapNotify       11
#define Expose             12
#define GraphicsExpose     13
#define NoExpose           14
#define VisibilityNotify   15
#define CreateNotify       16
#define DestroyNotify      17
#define UnmapNotify        18
#define MapNotify          19
#define MapRequest         20
#define ReparentNotify     21
#define ConfigureNotify    22
#define ConfigureRequest   23
#define GravityNotify      24
#define ResizeRequest      25
#define CirculateNotify    26
#define CirculateRequest   27
#define PropertyNotify     28
#define SelectionClear     29
#define SelectionRequest   30
#define SelectionNotify    31
#define ColormapNotify     32
#define ClientMessage      33
#define MappingNotify      34
#define MappingModifier    0
#define MappingKeyboard    1
#define MappingPointer     2

/* Event masks */
#define NoEventMask             (0L)
#define QueuedAfterFlush          2
#define CWX                         (1L << 0)
#define CWY                         (1L << 1)
#define CWWidth                     (1L << 2)
#define CWHeight                    (1L << 3)
#define CWBorderWidth               (1L << 4)
#define CWBackPixmap                (1L << 5)
#define CWBackPixel                 (1L << 6)
#define CWBorderPixel               CWBackPixel
#define CWOverrideRedirect          (1L << 7)
#define CWSaveUnder                 (1L << 8)
#define CWEventMask                 (1L << 9)
#define CWWinGravity                (1L << 10)
#define CWColormap                  (1L << 11)
#define CWCursor                    (1L << 12)
#define VisualNoMask                0

/* GC value mask bits (X.h) */
#define GCFunction              (1L << 0)
#define GCForeground            (1L << 1)
#define GCBackground            (1L << 2)
#define GCPlaneMask             (1L << 3)
#define GCClipMask              (1L << 4)
#define GCGraphicsExposures     (1L << 13)
#define GCStipple               (1L << 15)
#define GCTile                  (1L << 16)

/* graphics functions (X.h) */
#define GXcopy                  12

#define AllPlanes               ((unsigned long)~0L)
#define AnyPropertyType         ((Atom)0L)
#define AllocNone               0
#define AllocAll                1
#define GrabModeSync            0
#define GrabModeAsync           1
#define GrabSuccess             0
#define Success                 0

/* Xproto fixed types */
typedef unsigned char CARD8;
typedef unsigned short CARD16;
typedef unsigned long CARD32;

#define True                        1
#define False                       0
#define CurrentTime                 0L
#define DefaultScreen(dpy)          XDefaultScreenOfDisplay(dpy)
#define KeymapStateMask             KeyMapStateMask
#define KeyPressMask            (1L << 0)
#define KeyReleaseMask          (1L << 1)
#define ButtonPressMask         (1L << 2)
#define ButtonReleaseMask       (1L << 3)
#define EnterWindowMask         (1L << 4)
#define LeaveWindowMask         (1L << 5)
#define PointerMotionMask       (1L << 6)
#define Button1MotionMask       (1L << 7)
#define Button2MotionMask       (1L << 8)
#define Button3MotionMask       (1L << 9)
#define Button4MotionMask       (1L << 10)
#define Button5MotionMask       (1L << 11)
#define PointerMotionHintMask   (1L << 12)
#define ButtonMotionMask        (1L << 16)
#define KeyMapStateMask         (1L << 17)
#define ExposureMask            (1L << 18)
#define VisibilityChangeMask    (1L << 19)
#define StructureNotifyMask     (1L << 20)
#define ResizeRedirectMask      (1L << 21)
#define SubstructureNotifyMask  (1L << 22)
#define SubstructureRedirectMask (1L << 23)
#define FocusChangeMask         (1L << 24)
#define PropertyChangeMask      (1L << 25)
#define ColormapChangeMask      (1L << 26)
#define OwnerGrabButtonMask     (1L << 27)
#define AllEventsMask           (~0L)

/* Mouse button numbers */
#define Button1 1
#define Button2 2
#define Button3 3
#define Button4 4
#define Button5 5
#define NumButtons 6

/* Modifier masks (state field) */
#define ShiftMask   (1 << 0)
#define LockMask    (1 << 1)
#define ControlMask (1 << 2)
#define Mod1Mask    (1 << 3)
#define Mod2Mask    (1 << 4)
#define Mod3Mask    (1 << 5)
#define Mod4Mask    (1 << 6)
#define Mod5Mask    (1 << 7)
#define ShiftMapIndex  0
#define LockMapIndex   1
#define ControlMapIndex 2
#define Mod1MapIndex   3
#define Mod2MapIndex   4
#define Mod3MapIndex   5
#define Mod4MapIndex   6
#define Mod5MapIndex   7
#define MapModifiersMask (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)
#define Button1Mask (1 << 8)
#define Button2Mask (1 << 9)
#define Button3Mask (1 << 10)
#define Button4Mask (1 << 11)
#define Button5Mask (1 << 12)
#define AnyModifier 0xfffe

/* Input focus */
#define PointerRoot InputFocusPointerRoot
#define InputFocusPointerRoot   0
#define InputFocusPointerWindow 1
#define InputFocusNone          2
#define InputFocusRevertToParent 3
#define InputFocusRevertToNone  4

/* Crossing modes */
#define NotifyNormal             0
#define NotifyAncestor           0
#define NotifyVirtual            1
#define NotifyInferior           2
#define NotifyNonlinear          3
#define NotifyNonlinearVirtual   4
#define NotifyGrab               5
#define NotifyUngrab             6

/* Property modes */
#define PropModeReplace 0
#define PropModePrepend 1
#define PropModeAppend  2

/* Property states */
#define PropertyNewValue 0
#define PropertyDelete   1

/* Map states */
#define IsUnMapped 0
#define IsUnViewable 1
#define IsViewable 2

/* Revert-to values (XSetInputFocus) */
#define RevertToNone         0
#define RevertToParent       1
#define RevertToPointerRoot  2

/* Window classes */
#define InputOutput 0
#define InputOnly   1
#define CopyFromParent -1

/* Cursor shapes (see cursorfont.h) */
typedef int FontCursorShape;
#define XC_XCursor             0
#define XC_arrow               1
#define XC_based_arrow         2
#define XC_bateau              3
#define XC_pirate              4
#define XC_xterm               5
#define XC_sailboat            6
#define XC_icon                7
#define XC_top_left_corner     8
#define XC_top_right_corner    9
#define XC_bottom_left_corner  10
#define XC_bottom_right_corner 11
#define XC_left_ptr            12
#define XC_right_ptr           13
#define XC_top_side            14
#define XC_bottom_side         15
#define XC_left_side           16
#define XC_right_side          17
#define XC_top_left_arrow      18
#define XC_top_right_arrow     19
#define XC_bottom_left_arrow   20
#define XC_bottom_right_arrow  21
#define XC_cross               22
#define XC_cross_reverse       23
#define XC_crosshair           24
#define XC_dagger              25
#define XC_dot                 26
#define XC_dotbox              27
#define XC_double_bar          28
#define XC_draped_box          29
#define XC_exchange            30
#define XC_fleur               31
#define XC_gobbler             32
#define XC_hand1               33
#define XC_hand2               34
#define XC_heart               35
#define XC_man                 36
#define XC_plus                38
#define XC_question_arrow      39
#define XC_target              40
#define XC_ai                  41
#define XC_aim                 42
#define XC_all_arrows          43
#define XC_apple2              44
#define XC_arches              45
#define XC_clock               55
#define XC_clock1              56
#define XC_coaster             57
#define XC_cursor              60
#define XC_customizing         62
#define XC_dollar_sign         63
#define XC_double_arrow        66
#define XC_dropper             67
#define XC_dotted_circle       68
#define XC_dotted_cross        69
#define XC_draftsize           70
#define XC_ferris              73
#define XC_goumiao             76
#define XC_ibm_arrow           81
#define XC_ibm_cursor          82
#define XC_ironcross           83
#define XC_jamboree            84
#define XC_leftside            87
#define XC_ll_angle            88
#define XC_lr_angle            89
#define XC_middlebutton        91
#define XC_mote                92
#define XC_name                93
#define XC_no_bad              94
#define XC_note                95
#define XC_numeral             96
#define XC_oiver               97
#define XC_oval_ptr            98
#define XC_pencil              99
#define XC_roller              102
#define XC_rightside           105
#define XC_rubber_band         106
#define XC_sb_down_arrow       108
#define XC_sb_h_double_arrow   109
#define XC_sb_left_arrow       110
#define XC_sb_right_arrow      111
#define XC_sb_up_arrow         112
#define XC_sb_v_double_arrow   113
#define XC_sizing              114
#define XC_spider              115
#define XC_splatter            116
#define XC_static_grab         117
#define XC_stickypointer       118
#define XC_stream_in           119
#define XC_stream_out          120
#define XC_sun2                121
#define XC_ul_angle            130
#define XC_ur_angle            131
#define XC_watch               127

/* Colour flags (XColor) */
#define DoRed   (1 << 0)
#define DoGreen (1 << 1)
#define DoBlue  (1 << 2)
#define DoAll   (DoRed | DoGreen | DoBlue)

/* --------------------------------------------------------------------------
 * Core structures
 * -------------------------------------------------------------------------- */
struct _XColor
{
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags;  /* DoRed | DoGreen | DoBlue */
    char pad;
};
typedef struct _XColor XColor;

struct _XSizeHints
{
    long flags;
    int x, y;
    int width, height;
    int min_width, min_height;
    int max_width, max_height;
    int width_inc, height_inc;
    struct
    {
        int x;
        int y;
    } min_aspect, max_aspect;
    int base_width, base_height;
    int pad[4];
};
typedef struct _XSizeHints XSizeHints;
#define USPosition (1L << 0)
#define USSize     (1L << 1)
#define USMinSize  (1L << 2)
#define USMaxSize  (1L << 3)
#define USResizeInc (1L << 4)
#define USAspect   (1L << 5)
#define PPosition  (1L << 8)
#define PSize      (1L << 9)
#define PMinSize   (1L << 10)
#define PMaxSize   (1L << 11)
#define PResizeInc (1L << 12)
#define PAspect    (1L << 13)

struct _XWMHints
{
    long flags;
    int input;
    int initial_state;
    Pixmap icon_pixmap;
    Window icon_window;
    int icon_x, icon_y;
    Pixmap icon_mask;
    Window window_group;
};
typedef struct _XWMHints XWMHints;
#define XUtuwmInputModel (1L << 0)
#define XUtuwmStateModel (1L << 1)
#define XUtuwmIconPixmap (1L << 2)
#define XUtuwmIconWindow (1L << 3)
#define XUtuwmIconPosition (1L << 4)
#define XUtuwmWindowGroup (1L << 5)
#define XUtuwmAllHints (XUtuwmInputModel | XUtuwmStateModel | \
                        XUtuwmIconPixmap | XUtuwmIconWindow | \
                        XUtuwmIconPosition | XUtuwmWindowGroup)
#define XUtuwmStrut (1L << 6)
#define XUtuwmStrutPartial (1L << 7)
#define InputHint (1L << 0)
#define StateHint (1L << 1)
#define IconPixmapHint (1L << 2)
#define IconWindowHint (1L << 3)
#define IconPositionHint (1L << 4)
#define IconSizeHint (1L << 5)
#define IconMaskHint (1L << 6)
#define MWM_HINTS_FUNCTIONS   (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_FUNC_ALL          (1L << 0)
#define MWM_FUNC_RESIZE       (1L << 1)
#define MWM_FUNC_MOVE         (1L << 2)
#define MWM_FUNC_MINIMIZE     (1L << 3)
#define MWM_FUNC_MAXIMIZE     (1L << 4)
#define MWM_FUNC_CLOSE        (1L << 5)
#define MWM_DECOR_ALL         (1L << 0)
#define MWM_DECOR_BORDER      (1L << 1)
#define MWM_DECOR_RESIZEH     (1L << 2)
#define MWM_DECOR_TITLE       (1L << 3)
#define MWM_DECOR_MENU        (1L << 4)
#define MWM_DECOR_MINIMIZE    (1L << 5)
#define MWM_DECOR_MAXIMIZE    (1L << 6)
#define NormalState    1
#define IconicState    3
#define WithdrawnState 0

struct _XClassHint
{
    char* res_name;
    char* res_class;
};
typedef struct _XClassHint XClassHint;

struct _XTextProperty
{
    unsigned char* value;
    Atom encoding;
    unsigned long nitems;
    unsigned int format;
    unsigned long length;
};
typedef struct _XTextProperty XTextProperty;

struct _XVisualInfo
{
    Display* display;
    VisualID visualid;
    Visual* visual;
    int screen;
    int depth;
    int c_class;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int bits_per_rgb;
    int offset_red;
    int offset_green;
    int offset_blue;
    int colormaps;
    unsigned long bytes_per_line;
};
typedef struct _XVisualInfo XVisualInfo;
typedef unsigned long VisualID;
#define StaticGray 0
#define GrayScale  1
#define StaticColor 2
#define PseudoColor 3
#define TrueColor  4
#define DirectColor 5
#define VisualStaticGray (1L << 0)
#define VisualGrayScale (1L << 1)
#define VisualStaticColor (1L << 2)
#define VisualPseudoColor (1L << 3)
#define VisualTrueColor (1L << 4)
#define VisualDirectColor (1L << 5)
#define VisualIDMask (1L << 6)
#define VisualScreenMask (1L << 7)
#define VisualDepthMask (1L << 8)
#define VisualClassMask (1L << 9)
#define VisualRedMaskMask (1L << 10)
#define VisualGreenMaskMask (1L << 11)
#define VisualBlueMaskMask (1L << 12)
#define VisualBitsPerRGBMask (1L << 13)
#define VisualColormapMask (1L << 14)
#define VisualAllMask (0xfffffL)

struct _XWindowAttributes
{
    int x, y;
    int width, height;
    int border_width;
    int depth;
    Visual* visual;
    int root;
    int class_;
    long mask;
    Colormap colormap;
    Colormap all_colormaps;
    Bool save_under;
    long colormaps;
    long your_event_mask;
    int map_state;
    long backing_store;
    long backing_planes;
    long backing_pixel;
    Bool override_redirect;
    long screen;
    long subwindows;
};
typedef struct _XWindowAttributes XWindowAttributes;

/* Window attributes (for XCreateWindow) */
struct _XSetWindowAttributes
{
    unsigned long background_pixel;
    unsigned long border_pixel;
    Pixmap background_pixmap;
    int border_width;
    Colormap colormap;
    int cursor;
    int event_mask;
    int do_not_propagate_mask;
    int override_redirect;
    int save_under;
    int backing_store;
    int backing_planes;
    int backing_pixel;
    int border_pixmap;
    int win_gravity;
};
typedef struct _XSetWindowAttributes XSetWindowAttributes;

#define BackPixmapNone 0
#define BackPixmapParentRelative 1
#define BackPixmapOneShot 2
#define BackPixmapAlways 3

/* --------------------------------------------------------------------------
 * Events
 * -------------------------------------------------------------------------- */
typedef struct
{
    int type;
    unsigned long serial;
    Bool send_event;
    Display* display;
    Window window;
} XAnyEvent;

typedef struct
{
    int type;
    Display* display;
    Window event;
    Window root;
    Window window;
    Time time;
    int x, y;
    int x_root, y_root;
    unsigned int state;
    KeyCode keycode;
    Bool same_screen;
} XKeyEvent;

typedef struct
{
    int type;
    Display* display;
    Window event;
    Window root;
    Window window;
    Time time;
    int x, y;
    int x_root, y_root;
    unsigned int state;
    unsigned int button;
    Bool same_screen;
} XButtonEvent;

typedef struct
{
    int type;
    Display* display;
    Window event;
    Window root;
    Window window;
    Time time;
    int x, y;
    int x_root, y_root;
    unsigned int state;
    char is_hint;
    Bool same_screen;
} XMotionEvent;

typedef struct
{
    int type;
    Display* display;
    Window event;
    Window root;
    Window window;
    Window subwindow;
    Time time;
    int x, y;
    int x_root, y_root;
    int mode;
    int detail;
    Bool same_screen;
    unsigned int state;
} XCrossingEvent;

typedef struct
{
    int type;
    Display* display;
    Window event;
    Window window;
    int x, y;
    Bool override_redirect;
} XGravityEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    int x, y;
    int width, height;
    int count;
} XExposeEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    int state;
} XVisibilityEvent;

typedef struct
{
    int type;
    Display* display;
    Window parent;
    Window window;
    int x, y;
    int width, height;
    int border_width;
    Bool override_redirect;
    Time time;
} XCreateWindowEvent;

typedef struct
{
    int type;
    Display* display;
    Bool send_event;
    Display* event;
    Window parent;
    Window window;
    int x, y;
    int time;
    Bool override_redirect;
} XDestroyWindowEvent;

typedef struct
{
    int type;
    Display* display;
    Bool send_event;
    Window window;
    Window event;
    int time;
    Bool from_configure;
} XUnmapEvent;

typedef struct
{
    int type;
    Display* display;
    Bool send_event;
    Window window;
    Window event;
    int time;
    Bool override_redirect;
} XMapEvent;

typedef struct
{
    int type;
    Display* display;
    Window parent;
    Window window;
    int x, y;
    int width, height;
    int border_width;
    Bool override_redirect;
    int map;
    Time time;
} XMapRequestEvent;

typedef struct
{
    int type;
    Display* display;
    Bool send_event;
    Window window;
    Window event;
    int time;
    int map_state;
} XMapNotifyEvent;

typedef struct
{
    int type;
    Display* display;
    Bool send_event;
    Window event;
    Window window;
    int x, y;
    int width, height;
    int border_width;
    Bool override_redirect;
    Window above;
    Bool detail;
} XConfigureEvent;

typedef struct
{
    int type;
    Display* display;
    Window parent;
    Window window;
    int x, y;
    int width, height;
    int border_width;
    Bool above;
    Bool detail;
} XConfigureRequestEvent;

typedef struct
{
    int type;
    Display* display;
    Window parent;
    Window window;
    int x, y;
    int width, height;
    int border_width;
    Bool above;
    Bool detail;
} XCirculateEvent;

typedef struct
{
    int type;
    Display* display;
    Window parent;
    Window window;
    Bool detail;
} XCirculateRequestEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    int mode;
    int detail;
} XFocusChangeEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    char key_vector[32];
} XKeymapEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    Atom atom;
    Time time;
    int state;
} XPropertyEvent;

typedef struct
{
    int type;
    Display* display;
    Window requestor;
    Atom selection;
    Atom target;
    Atom property;
    Time time;
} XSelectionEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    int request;
    unsigned int first_keycode;
    unsigned int key_count;
    unsigned int first_button;
    unsigned int button_count;
    unsigned int first_mod;
    unsigned int mod_count;
} XMappingEvent;

typedef struct
{
    int type;
    Display* display;
    Window window;
    int request;
    unsigned int first_keycode;
    unsigned int key_count;
    unsigned int first_button;
    unsigned int button_count;
    unsigned int first_mod;
    unsigned int mod_count;
} XCreateMappingEvent;

typedef struct
{
    int type;
    Display* display;
    XID resourceid;
    int error_code;
    unsigned char request_code;
    unsigned char minor_code;
} XErrorEvent;

typedef struct
{
    int type;
    unsigned long serial;
    Bool send_event;
    Display* display;
    Window window;
    Atom message_type;
    int format;
    union
    {
        long l[5];
        char b[20];
        unsigned char bytes[20];
    } data;
} XClientMessageEvent;

struct _XReparentEvent
{
    int type;
    unsigned long serial;
    Bool send_event;
    Display* display;
    Window window;
    Window parent;
    int x, y;
    Bool override_redirect;
};

struct _XSelectionRequestEvent
{
    int type;
    unsigned long serial;
    Bool send_event;
    Display* display;
    Window selection;
    Window requestor;
    Atom target;
    Atom property;
    Time time;
};

typedef struct _XReparentEvent XReparentEvent;
typedef struct _XSelectionRequestEvent XSelectionRequestEvent;


typedef struct
{
    int type;
    XAnyEvent xany;
    XKeyEvent xkey;
    XButtonEvent xbutton;
    XMotionEvent xmotion;
    XCrossingEvent xcrossing;
    XGravityEvent xgravity;
    XExposeEvent xexpose;
    XVisibilityEvent xvisibility;
    XCreateWindowEvent xcreatewindow;
    XDestroyWindowEvent xdestroywindow;
    XUnmapEvent xunmap;
    XMapEvent xmap;
    XMapRequestEvent xmaprequest;
    XMapNotifyEvent xmapnotify;
    XConfigureEvent xconfigure;
    XConfigureRequestEvent xconfigurerequest;
    XCirculateEvent xcirculate;
    XCirculateRequestEvent xcirculaterequest;
    XFocusChangeEvent xfocus;
    XKeymapEvent xkeymap;
    XPropertyEvent xproperty;
    XSelectionEvent xselection;
    XSelectionRequestEvent xselectionrequest;
    XCreateMappingEvent xcreate_mapping;
    XReparentEvent xreparent;
    XMappingEvent xmapping;
    XErrorEvent xerror;
    XClientMessageEvent xclient;
    long pad[24];
    Display* display;
} XEvent;

/* JUCE-expected convenience aliases (XButtonEvent carries `button`) */
typedef XButtonEvent  XButtonPressedEvent;
typedef XButtonEvent  XButtonReleasedEvent;
typedef XMotionEvent  XPointerMovedEvent;
typedef XCrossingEvent XEnterWindowEvent;
typedef XCrossingEvent XLeaveWindowEvent;
typedef XConfigureEvent XWindowResizeEvent;
typedef XFocusChangeEvent XFocusEvent;

typedef XKeyEvent XKeyPressEvent;
typedef XKeyEvent XKeyReleaseEvent;

/* --------------------------------------------------------------------------
 * XImage
 * -------------------------------------------------------------------------- */
struct _XImage;
typedef struct _XImage XImage;

typedef unsigned long (*XGetPixelProc) (XImage*, int, int);
typedef void          (*XPutPixelProc) (XImage*, int, int, unsigned long);
typedef unsigned long (*XGetLineProc) (XImage*, int, int, int);
typedef void          (*XPutLineProc) (XImage*, int, int, int, int);
typedef unsigned long (*XGetRowProc) (XImage*, int, int, int);
typedef void          (*XPutRowProc) (XImage*, int, int, int, int);
typedef unsigned long (*XGetPixel16Proc) (XImage*, int, int);
typedef void          (*XPutPixel16Proc) (XImage*, int, int, unsigned long);
typedef unsigned long (*XGetLine16Proc) (XImage*, int, int, int);
typedef void          (*XPutLine16Proc) (XImage*, int, int, int, int);
typedef unsigned long (*XGetRow16Proc) (XImage*, int, int, int);
typedef void          (*XPutRow16Proc) (XImage*, int, int, int, int);

struct _XImage
{
    int width, height;
    int xoffset;
    int format;
    char* data;
    int byte_order;
    int bitmap_unit;
    int bitmap_bit_order;
    int bitmap_pad;
    int bit_order;
    int depth;
    int pixels_per_bit;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int bits_per_pixel;
    int bytes_per_line;
    int oblength;
    XPointer fpriv;
    XGetPixelProc fGetPixel;
    XPutPixelProc fPutPixel;
    XGetLineProc  fGetLine;
    XPutLineProc  fPutLine;
    XGetRowProc   fGetRow;
    XPutRowProc   fPutRow;
    XGetPixel16Proc fGetPixel_16;
    XPutPixel16Proc fPutPixel_16;
    XGetLine16Proc  fGetLine_16;
    XPutLine16Proc  fPutLine_16;
    XGetRow16Proc   fGetRow_16;
    XPutRow16Proc   fPutRow_16;
    int obdata;
};

#define ZPixmap 1
#define XYBitmap 0
#define XYPixmap 2
#define LSBFirst 0
#define MSBFirst 1

/* --------------------------------------------------------------------------
 * Graphics context values
 * -------------------------------------------------------------------------- */
struct _XGCValues
{
    int function;
    unsigned long plane_mask;
    unsigned long foreground;
    unsigned long background;
    Pixmap clip_mask;
    int clip_x_origin;
    int clip_y_origin;
    int dash_offset;
    int fill_style;
    Pixmap stipple;
    Pixmap tile;
    Font font;
    int subwindow_mode;
    Bool graphics_exposures;
    Bool tile_stip_xor;
};
typedef struct _XGCValues XGCValues;
#define CapNotLast 0
#define CapRound   1
#define CapProjecting 2
#define JoinMiter 0
#define JoinBevel 1
#define JoinRound 2
#define Solid 0
#define Stippled 1
#define OpsMask 0x6
#define Inverse 0x8
#define EvenOddRule 0
#define WindingRule 1
#define ChordNotClosed 0
#define ChordTriangle 1
#define ChordPieSlice 2
#define ClipByChildren 0
#define IncludeInferiors 1
#define DashNone 0
#define DashSolid 1

/* --------------------------------------------------------------------------
 * Misc types
 * -------------------------------------------------------------------------- */
typedef struct
{
    int max_keypermod;
    KeyCode modifiermap[8];
} XModifierKeymap;

typedef struct
{
    unsigned int length;
    char* chars;
    int (*stats) (void);
    int (*per_char) (void);
    int succeeded;
} XComposeStatus;

typedef int (*XErrorHandler) (Display*, XErrorEvent*);
typedef int (*XIOErrorHandler) (Display*);

typedef enum
{
    XStringStyle = 0,
    XUTF8StringStyle = 1
} XICCEncodingStyle;

/* X resource manager types (Xresource.h / Xutil.h) */
typedef unsigned long XrmQuark;
typedef struct _XrmValue
{
    int quark;
    unsigned long size;
    XPointer addr;
} XrmValue;
typedef struct _XrmValue XrmValue;
typedef XrmValue* XrmValuePtr;
typedef XrmValue* XrmRepresentation;
typedef struct _XrmDatabase XrmDatabase;
typedef struct _XrmArgList XrmArgList;
typedef unsigned long XrmArgListT;
typedef enum
{
    XrmStringKind = 0,
    XrmBitListKind,
    XrmStringArrayKind,
    XrmIntKind,
    XrmBooleanKind,
    XrmPixelKind,
    XrmColorKind,
    XrmSpecialStringKind,
    XrmRDBFileKind,
    XrmNonAsciiOpaqueKind
} XrmRepresentationKind;

/* XIM (input methods, Xmd.h) */
typedef int XIMPreeditPosition;
#define XIMPreeditNothing 0
#define XIMPreeditPosition 1
#define XIMPreeditCaret 2
#define XIMPreeditGeometry 3
#define XIMPreeditCallback 4
#define XIMPreeditNothingMask 0
#define XIMPreeditPositionMask 1
#define XIMPreeditCaretMask 2
#define XIMPreeditGeometryMask 3
#define XIMPreeditCallbackMask 4
#define XIMStatusNothing 0
#define XIMStatusArea 1
#define XIMStatusNone XIMStatusNothing
#define XNInputStyle "inputStyle"
#define XNClientWindow "clientWindow"
#define XNFocusWindow "focusWindow"
#define XNIC_EnlistNotification "enlistNotification"
#define XNIM_XF86_EnlistNotification "imXf86EnlistNotification"

/* --------------------------------------------------------------------------
 * Functions (prototypes only - JUCE loads them via dlopen at runtime)
 * -------------------------------------------------------------------------- */
Display*   XOpenDisplay (const char*);
int        XCloseDisplay (Display*);
int        XFlush (Display*);
int        XSync (Display*, Bool);
int        XPending (Display*);
int        XEventsQueued (Display*, int);
void       XNextEvent (Display*, XEvent*);
void       XPeekEvent (Display*, XEvent*);
Bool       XCheckTypedWindowEvent (Display*, Window, int, XEvent*);
Bool       XCheckWindowEvent (Display*, Window, long, XEvent*);
int        XSelectInput (Display*, Window, long);
Window     XCreateWindow (Display*, Window, int, int, unsigned int, unsigned int,
                          unsigned int, int, unsigned int, Visual*, unsigned long,
                          XSetWindowAttributes*);
void       XDestroyWindow (Display*, Window);
void       XReparentWindow (Display*, Window, Window, int, int);
void       XMapWindow (Display*, Window);
void       XMapRaised (Display*, Window);
void       XUnmapWindow (Display*, Window);
void       XMoveResizeWindow (Display*, Window, int, int, unsigned int, unsigned int);
void       XResizeWindow (Display*, Window, unsigned int, unsigned int);
int        XRaiseWindow (Display*, Window);
int        XLowerWindow (Display*, Window);
void       XRestackWindows (Display*, Window*, int);
void       XConfigureWindow (Display*, Window, unsigned long, XSetWindowAttributes*);
int        XChangeAttributes (Display*, Window, unsigned long, XSetWindowAttributes*);
int        XIconifyWindow (Display*, Window, int);
int        XWithdrawWindow (Display*, Window, int);
void       XSetInputFocus (Display*, Window, int, Time);
void       XGetInputFocus (Display*, Window*, int*);
int        XGetGeometry (Display*, Drawable, Window*, int*, int*, unsigned int*,
                         unsigned int*, unsigned int*, unsigned int*);
Status     XGetWindowAttributes (Display*, Window, XWindowAttributes*);
int        XClearArea (Display*, Window, int, int, unsigned int, unsigned int, Bool);
Bool       XTranslateCoordinates (Display*, Window, Window, int, int, int*, int*, Window*);
void       XSetClassHint (Display*, Window, XClassHint*);
XClassHint* XAllocClassHint (void);
XSizeHints* XAllocSizeHints (void);
XWMHints*   XAllocWMHints (void);
void       XSetWMHints (Display*, Window, XWMHints*);
XWMHints*   XGetWMHints (Display*, Window);
void       XSetWMNormalHints (Display*, Window, XSizeHints*);
void       XSetWMName (Display*, Window, XTextProperty*);
void       XSetWMIconName (Display*, Window, XTextProperty*);
Status     XStringListToTextProperty (char**, int, XTextProperty*);
int        Xutf8TextListToTextProperty (Display*, char**, int, XICCEncodingStyle, XTextProperty*);
Atom       XInternAtom (Display*, const char*, Bool);
char*      XGetAtomName (Display*, Atom);
void       XChangeProperty (Display*, Window, Atom, Atom, int, int, const unsigned char*, int);
int        XGetWindowProperty (Display*, Window, Atom, long, long, Bool, Atom, Atom*,
                               int*, unsigned long*, unsigned long*, unsigned char**);
void       XDeleteProperty (Display*, Window, Atom);
Atom*      XListProperties (Display*, Window, int*);
void       XConvertSelection (Display*, Atom, Atom, Atom, Window, Time);
void       XSetSelectionOwner (Display*, Atom, Window, Time);
Window     XGetSelectionOwner (Display*, Atom);
Cursor     XCreateFontCursor (Display*, unsigned int);
Cursor     XCreatePixmapCursor (Display*, Pixmap, Pixmap, XColor*, XColor*, unsigned int, unsigned int);
void       XFreeCursor (Display*, Cursor);
int        XDefineCursor (Display*, Window, Cursor);
Pixmap     XCreatePixmap (Display*, Drawable, unsigned int, unsigned int, unsigned int);
void       XFreePixmap (Display*, Pixmap);
Pixmap     XCreatePixmapFromBitmapData (Display*, Drawable, const char*, unsigned int,
                                        unsigned int, unsigned long, unsigned long, unsigned int);
GC         XCreateGC (Display*, Drawable, unsigned long, XGCValues*);
/* XShm */
typedef struct {
    unsigned long shmseg;
    int shmid;
} XShmSegmentInfo;

/* XRender (Xrender.h) */
struct _XRenderPictFormat;
typedef struct _XRenderPictFormat *XRenderPictFormat;

/* Xrandr (xrandr.h) */
typedef unsigned long RROutput;
typedef unsigned long RRCrtc;
struct _XRRScreenResources;
typedef struct _XRRScreenResources *XRRScreenResources;
struct _XRROutputInfo;
typedef struct _XRROutputInfo *XRROutputInfo;
struct _XRRCrtcInfo;
typedef struct _XRRCrtcInfo *XRRCrtcInfo;

void       XFreeGC (Display*, GC);
void       XPutImage (Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int);
XImage*    XGetImage (Display*, Drawable, int, int, unsigned int, unsigned int, unsigned long, int);
XImage*    XCreateImage (Display*, Visual*, unsigned int, int, int, const char*, unsigned int,
                         unsigned int, int, int);
void       XDestroyImage (XImage*);
Status     XInitImage (XImage*);
void       XPutPixel (XImage*, int, int, unsigned long);
int        XBitmapBitOrder (Display*);
int        XBitmapUnit (Display*);
int        XImageByteOrder (Display*);
Colormap   XCreateColormap (Display*, Window, Visual*, int);
void       XFreeColormap (Display*, Colormap);
void       XInstallColormap (Display*, Colormap);
XVisualInfo* XGetVisualInfo (Display*, long, XVisualInfo*, int*);
Visual*    XDefaultVisual (Display*, int);
int        XDefaultScreen (Display*);
Window     XDefaultRootWindow (Display*);
Window     XRootWindow (Display*, int);
Screen*    XDefaultScreenOfDisplay (Display*);
int        XScreenCount (Display*);
int        XScreenNumberOfScreen (Screen*);
int        XDisplayWidth (Display*, int);
int        XDisplayHeight (Display*, int);
int        XDisplayWidthMM (Display*, int);
int        XDisplayHeightMM (Display*, int);
int        XConnectionNumber (Display*);
KeySym     XkbKeycodeToKeysym (Display*, KeyCode, unsigned int, unsigned int);
KeyCode    XKeysymToKeycode (Display*, KeySym);
int        XLookupString (XKeyEvent*, const char*, int, KeySym*, XComposeStatus*);
XModifierKeymap* XGetModifierMapping (Display*);
void       XFreeModifiermap (XModifierKeymap*);
void       XRefreshKeyboardMapping (XMappingEvent*);
Status     XInitThreads (void);
int        XLockDisplay (Display*);
int        XUnlockDisplay (Display*);int        XSynchronize (Display*, Bool);
int        XGrabPointer (Display*, Window, Bool, unsigned int, int, int, Window, Cursor, Time);
int        XUngrabPointer (Display*, Time);
int        XChangeActivePointerGrab (Display*, unsigned int, Cursor, Time);
int        XGrabServer (Display*);
int        XUngrabServer (Display*);
Bool       XQueryPointer (Display*, Window, Window*, Window*, int*, int*, int*, int*, unsigned int*);
Status     XQueryTree (Display*, Window, Window*, Window*, Window**, unsigned int*);
Bool       XQueryExtension (Display*, const char*, int*, int*, int*);
Status     XQueryBestCursor (Display*, Drawable, unsigned int, unsigned int, unsigned int*, unsigned int*);
Status     XSendEvent (Display*, Window, Bool, long, XEvent*);
void       XWarpPointer (Display*, Window, Window, int, int, unsigned int, unsigned int, int, int);
void       XSaveContext (Display*, XID, XContext, XPointer);
int        XFindContext (Display*, XID, XContext, XPointer*);
int        XDeleteContext (Display*, XID, XContext);
void       XSetErrorHandler (XErrorHandler);
XErrorHandler XSetIOErrorHandler (XIOErrorHandler);
void       XGetErrorText (Display*, int, const char*, int);
void       XGetErrorDatabaseText (Display*, const char*, const char*, const char*, const char*, int);
unsigned char* XGetPointerMapping (Display*, unsigned char*, int);
int        XrmUniqueQuark (void);
XrmDatabase XrmGetDatabase (void);
Status     XrmGetResource (XrmDatabase, const char*, const char*, XrmValuePtr);
Status     XrmGetResources (XrmDatabase, XPointer, int, XPointer, XrmValue*);
Status     XrmGetCellPixel (XrmDatabase, const char*, const char*, const char*, unsigned long*);
void       XrmDestroyDatabase (XrmDatabase);
XrmQuark   XrmStringQuark (const char*);
Status     XrmPutStringResource (XrmDatabase*, XrmArgList, ...);
Status     XrmInitialize (void);

} /* extern "C" */

/*
 * NOTE: X11 sub-headers included by JUCE (Xatom.h, Xutil.h, Xresource.h,
 * Xmd.h, keysym.h, XKBlib.h, cursorfont.h) provide the constants below;
 * they are all defined here in this master stub.
 */

/* Xatom.h standard atoms */
#define XA_PRIMARY   1
#define XA_SECONDARY 2
#define XA_ATOM      3
#define XA_PIXMAP    4
#define XA_WINDOW    5
#define XA_DRAWABLE  6
#define XA_FONT      7
#define XA_STRING    8
#define XA_CARDINAL  9
#define XA_TEXT      10
#define XA_ROTATE    11
#define XA_TRANSLATE 12
#define XA_RC        13
#define XA_ZOOM      14
#define XA_NOBUTTON  15
#define XA_CHAR      16
#define XA_ICON      17
#define XA_WM_COMMAND   31
#define XA_WM_HINTS     33
#define XA_WM_CLIENT_MACHINE 34
#define XA_WM_ICON_SIZE 35
#define XA_WM_STATE     36
#define XA_WM_ZOOM      37
#define XA_WM_DELETE_WINDOW 55
#define XA_WM_PROTOCOLS   59
#define XA_WM_TAKE_FOCUS  54
#define XA_NET_WM_NAME    313
#define XA_NET_WM_PID     315
#define XA_NET_WM_STATE   317
#define XA_NET_WM_STATE_HIDDEN 318
#define XA_NET_WM_STATE_ABOVE  319
#define XA_NET_WM_STATE_FULLSCREEN 320
#define XA_NET_WM_STATE_MAXIMIZED_VERT 321
#define XA_NET_WM_STATE_MAXIMIZED_HORZ 322
#define XA_NET_WM_PING      326
#define XA_NET_WM_MOVERESIZE 328
#define XA_MOTIF_WM_HINTS   332
#define XA_NET_WM_WINDOW_TYPE 336
#define XA_NET_WM_WINDOW_TYPE_NORMAL 338
#define XA_NET_WM_WINDOW_TYPE_DIALOG 339
#define XA_NET_WM_BICONIC  340
#define XA_WM_TRANSIENT_FOR 342
#define XA_NET_FRAME_EXTENTS 344
#define XA_XdbeVisual  348
#define XA_XdbeBackPixmap 349
#define XA_XdbeVisualInfo 350

/* cursorfont.h */

/* XKBlib.h */
int XkbSetDetectableAutoRepeat (Display*, Bool, int*);
int XkbUseBigIntegers (Display*, Bool);

/* Xresource.h */

/* keysym.h - the XK_* keysyms used by JUCE */
#define NoSymbol 0L
#define XK_BackSpace     0xff08
#define XK_Tab           0xff09
#define XK_ISO_Left_Tab  0xfe20
#define XK_Linefeed      0xff0a
#define XK_Clear         0xff0b
#define XK_Return        0xff0d
#define XK_Pause         0xff13
#define XK_Scroll_Lock   0xff14
#define XK_Sys_Req       0xff15
#define XK_Escape        0xff1b
#define XK_Delete        0xffff
#define XK_Home          0xff50
#define XK_Left          0xff51
#define XK_Up            0xff52
#define XK_Right         0xff53
#define XC_Down          0xff54
#define XK_Down          0xff54
#define XK_Prior         0xff55
#define XK_Page_Up       0xff55
#define XK_Next          0xff56
#define XK_Page_Down     0xff56
#define XK_End           0xff57
#define XK_Begin         0xff58
#define XK_Select        0xff60
#define XK_Print         0xff61
#define XK_Execute       0xff62
#define XK_Insert        0xff63
#define XK_Undo          0xff65
#define XK_Redo          0xff66
#define XK_Menu          0xff67
#define XK_Find          0xff68
#define XK_Cancel        0xff69
#define XK_Help          0xff6a
#define XK_Break         0xff6b
#define XK_Mode_switch   0xff7e
#define XK_script_switch 0xff7e
#define XK_Num_Lock      0xff7f
#define XK_KP_Space      0xff80
#define XK_KP_Tab        0xff89
#define XK_KP_Enter      0xff0d
#define XK_KP_F1         0xff91
#define XK_KP_Home       0xff74
#define XK_KP_Left       0xff6d
#define XK_KP_Up         0xff6e
#define XK_KP_Right      0xff6f
#define XK_KP_Down       0xff70
#define XK_KP_Prior      0xff71
#define XK_KP_Page_Up    0xff71
#define XK_KP_Next       0xff72
#define XK_KP_Page_Down  0xff72
#define XK_KP_End        0xff73
#define XK_KP_Begin      0xff75
#define XK_KP_Insert     0xff76
#define XK_KP_Delete     0xff77
#define XK_KP_Equal      0xffbd
#define XK_KP_Multiply   0xffaa
#define XK_KP_Add        0xff89
#define XK_KP_Separator  0xffac
#define XK_KP_Subtract   0xffad
#define XK_KP_Decimal    0xffac
#define XK_KP_Divide     0xffbb
#define XK_KP_0          0xffb9
#define XK_KP_1          0xffb8
#define XK_KP_2          0xffb7
#define XK_KP_3          0xffb6
#define XK_KP_4          0xffb5
#define XK_KP_5          0xffb4
#define XK_KP_6          0xffb3
#define XK_KP_7          0xffb2
#define XK_KP_8          0xffb1
#define XK_KP_9          0xffb0
#define XK_F1            0xffbe
#define XK_F2            0xffbf
#define XK_F3            0xffc0
#define XK_F4            0xffc1
#define XK_F5            0xffc2
#define XK_F6            0xffc3
#define XK_F7            0xffc4
#define XK_F8            0xffc5
#define XK_F9            0xffc6
#define XK_F10           0xffc7
#define XK_F11           0xffc8
#define XK_F12           0xffc9
#define XK_F13           0xffca
#define XK_F14           0xffcb
#define XK_F15           0xffcc
#define XK_F16           0xffcd
#define XK_F17           0xffce
#define XK_F18           0xffcf
#define XK_F19           0xffd0
#define XK_F20           0xffd1
#define XK_F21           0xffd2
#define XK_F22           0xffd3
#define XK_F23           0xffd4
#define XK_F24           0xffd5
#define XK_F25           0xffd6
#define XK_F26           0xffd7
#define XK_F27           0xffd8
#define XK_F28           0xffd9
#define XK_F29           0xffda
#define XK_F30           0xffdb
#define XK_F31           0xffdc
#define XK_F32           0xffdd
#define XK_F33           0xffde
#define XK_F34           0xffdf
#define XK_F35           0xffe0
#define XK_Shift_L       0xfe21
#define XK_Shift_R       0xfe22
#define XK_Control_L     0xfe23
#define XK_Control_R     0xfe24
#define XK_Caps_Lock     0xff7d
#define XK_Shift_Lock    0xfe25
#define XK_Meta_L        0xfe26
#define XK_Meta_R        0xfe27
#define XK_Alt_L         0xfe28
#define XK_Alt_R         0xfe29
#define XK_Super_L       0xffeb
#define XK_Super_R       0xffec
#define XK_Hyper_L       0xffed
#define XK_Hyper_R       0xffee
#define XK_space         0x0020
#define XK_exclamation   0x0021
#define XK_quotedbl      0x0022
#define XK_numbersign    0x0023
#define XK_dollar        0x0024
#define XK_percent       0x0025
#define XK_ampersand     0x0026
#define XK_apostrophe    0x0027
#define XK_quoteright    0x0027
#define XK_parenleft     0x0028
#define XK_parenright    0x0029
#define XK_asterisk      0x002a
#define XK_plus          0x002b
#define XK_comma         0x002c
#define XK_hyphen        0x002d
#define XK_minus         0x002d
#define XK_period        0x002e
#define XK_slash         0x002f
#define XK_0             0x0030
#define XK_1             0x0031
#define XK_2             0x0032
#define XK_3             0x0033
#define XK_4             0x0034
#define XK_5             0x0035
#define XK_6             0x0036
#define XK_7             0x0037
#define XK_8             0x0038
#define XK_9             0x0039
#define XK_colon         0x003a
#define XK_semicolon     0x003b
#define XK_less          0x003c
#define XK_equal         0x003d
#define XK_greater       0x003e
#define XK_question      0x003f
#define XK_at            0x0040
#define XK_a             0x0061
#define XK_b             0x0062
#define XK_c             0x0063
#define XK_d             0x0064
#define XK_e             0x0065
#define XK_f             0x0066
#define XK_g             0x0067
#define XK_h             0x0068
#define XK_i             0x0069
#define XK_j             0x006a
#define XK_k             0x006b
#define XK_l             0x006c
#define XK_m             0x006d
#define XK_n             0x006e
#define XK_o             0x006f
#define XK_p             0x0070
#define XK_q             0x0071
#define XK_r             0x0072
#define XK_s             0x0073
#define XK_t             0x0074
#define XK_u             0x0075
#define XK_v             0x0076
#define XK_w             0x0077
#define XK_x             0x0078
#define XK_y             0x0079
#define XK_z             0x007a
#define XK_bracketleft   0x005b
#define XK_backslash     0x005c
#define XK_bracketright  0x005d
#define XK_asciicircum   0x005e
#define XK_underscore    0x005f
#define XK_grave         0x0060
#define XK_quoteleft     0x0060
#define XK_braceleft     0x007b
#define XK_bar           0x007c
#define XK_braceright    0x007d
#define XK_asciitilde    0x007e
#define XK_nobreakspace  0x00a0
#define XK_exclamdown    0x00a1
#define XK_sterling      0x00a3
#define XK_currency      0x00a4
#define XK_section       0x00a7
#define XK_umlaut        0x00a8
#define XK_plusminus     0x00b1
#define XK_mu            0x00b5
#define XK_onesuperior   0x00b9
#define XK_threesuperior 0x00b3
#define XK_twosuperior   0x00b2
#define XK_multiply      0x00d7
#define XK_onequarter    0x00bc
#define XK_onehalf       0x00bd
#define XK_threequarters 0x00be
#define XK_questiondown  0x00bf
#define XK_AGrave        0x00c0
#define XK_Aacute        0x00c1
#define XK_Acircumflex   0x00c2
#define XK_Atilde        0x00c3
#define XK_Adiaeresis    0x00c4
#define XK_Aringabove    0x00c5
#define XK_AE            0x00c6
#define XK_Ccedilla      0x00c7
#define XK_Egrave        0x00c8
#define XK_Eacute        0x00c9
#define XK_Ecircumflex   0x00ca
#define XK_Ediaeresis    0x00cb
#define XK_Igrave        0x00cc
#define XK_Iacute        0x00cd
#define XK_Icircumflex   0x00ce
#define XK_Idiaeresis    0x00cf
#define XK_ETH           0x00d0
#define XK_Eth           0x00d0
#define XK_Ntilde        0x00d1
#define XK_Ograve        0x00d2
#define XK_Oacute        0x00d3
#define XK_Ocircumflex   0x00d4
#define XK_Otilde        0x00d5
#define XK_Odiaeresis    0x00d6
#define XK_Oslash        0x00d8
#define XK_OOblique      0x00d8
#define XK_Ugrave        0x00d9
#define XK_Uacute        0x00da
#define XK_Ucircumflex   0x00db
#define XK_Udiaeresis    0x00dc
#define XK_Yacute        0x00dd
#define XK_THORN         0x00de
#define XK_Thorn         0x00de
#define XK_ssharp        0x00df
#define XK_agrave        0x00e0
#define XK_aacute        0x00e1
#define XK_acircumflex   0x00e2
#define XK_atilde        0x00e3
#define XK_adiaeresis    0x00e4
#define XK_aringabove    0x00e5
#define XK_ae            0x00e6
#define XK_ccedilla      0x00e7
#define XK_egrave        0x00e8
#define XK_eacute        0x00e9
#define XK_ecircumflex   0x00ea
#define XK_ediaeresis    0x00eb
#define XK_igrave        0x00ec
#define XK_iacute        0x00ed
#define XK_icircumflex   0x00ee
#define XK_idiaeresis    0x00ef
#define XK_eth           0x00f0
#define XK_ntilde        0x00f1
#define XK_ograve        0x00f2
#define XK_oacute        0x00f3
#define XK_ocircumflex   0x00f4
#define XK_otilde        0x00f5
#define XK_odiaeresis    0x00f6
#define XK_oslash        0x00f8
#define XK_ooblique      0x00f8
#define XK_ugrave        0x00f9
#define XK_uacute        0x00fa
#define XK_ucircumflex   0x00fb
#define XK_udiaeresis    0x00fc
#define XK_yacute        0x00fd
#define XK_thorn         0x00fe
#define XK_ydiaeresis    0x00ff
#define XK_aogonek       0x01a1
#define XK_ubreve       0x01af
#define XK_ogonek        0x01a6
#define XK_uogonek       0x01b5
#define XK_lstroke       0x0142
#define XK_lcaron        0x013e
#define XK_Scaron        0x0160
#define XK_scaron        0x0161
#define XK_Tcaron        0x0164
#define XK_tcaron        0x0165
#define XK_hungarumlau   0x01b2
#define XK_Sacute        0x015a
#define XK_sacute        0x015b
#define XK_Lacute       0x0139
#define XK_lacute       0x013a
#define XK_Nacute       0x0143
#define XK_nacute       0x0144
#define XK_Odoubleacute 0x0150
#define XK_odoubleacute 0x0151
#define XK_Uringabove    0x016e
#define XK_uringabove    0x016f
#define XK_Abreve        0x0101
#define XK_abreve        0x0103
#define XK_Amacron       0x0101
#define XK_amacron       0x0103
#define XK_Umacron       0x016b
#define XK_umacron       0x016d
#define XK_gbreve        0x011f
#define XK_idotless      0x0131
#define XK_Gcedilla      0x0121
#define XK_gcedilla      0x0123
#define XK_kcomma        0x0137
#define XK_eng           0x014b
#define XK_ENG           0x014a
#define XK_Eogonek       0x011a
#define XK_eogonek       0x011b
#define XK_Dcroat        0x010e
#define XK_dcedilla      0x0111
#define XK_Dcaron        0x010e
#define XK_dcaron        0x010f
#define XK_Hcircumflex   0x0124
#define XK_hcircumflex   0x0125
#define XK_Iogonek       0x0128
#define XK_iogonek       0x0129
#define XK_Gcircumflex   0x011c
#define XK_gcircumflex   0x011d
#define XK_Cacute        0x0106
#define XK_cacute        0x0107
#define XK_Ccaron        0x010c
#define XK_ccaron        0x010d
#define XK_Ccedilla      0x00c7
#define XK_ccedilla      0x00e7
#define XK_Lcedilla      0x013f
#define XK_lcedilla      0x0140
#define XK_Ncedilla      0x0145
#define XK_ncedilla      0x0146
#define XK_Omacron       0x014c
#define XK_omacron       0x014d
#define XK_Kcedilla      0x0136
#define XK_kcedilla      0x0137
#define XK_Otilde        0x00d5
#define XK_otilde        0x00f5
