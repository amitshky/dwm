/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 1;   /* border pixel of windows */
static const unsigned int snap     = 32;  /* snap pixel */
static const int showbar           = 1;   /* 0 means no bar */
static const int topbar            = 1;   /* 0 means bottom bar */
static const char *fonts[]         = { "JetBrainsMonoNLNerdFont:size=12" };
static const char dmenufont[]      = "JetBrainsMonoNLNerdFont:size=12";
static unsigned int baralpha       = OPAQUE;
static unsigned int borderalpha    = OPAQUE;
static const char col_gray1[]      = "#000000";
static const char col_gray2[]      = "#3c3836";
static const char col_gray3[]      = "#ebdbb2";
static const char col_gray4[]      = "#fbf1c7";
static const char col_gray5[]      = "#7c6f64";
static const char col_blue[]       = "#076678";
static const char *colors[][3]     = {
	/*                   fg         bg          border   */
	[SchemeNorm]     = { col_gray3, col_gray1,  col_gray2  },
	[SchemeSel]      = { col_gray4, col_gray1,  col_blue   },
	[SchemeStatus]   = { col_gray3, col_gray1,  "#000000"  }, // Statusbar right {text,background,not used but cannot be empty}
	[SchemeTagsSel]  = { col_gray4, col_blue,   "#000000"  }, // Tagbar left selected {text,background,not used but cannot be empty}
	[SchemeTagsNorm] = { col_gray5, col_gray1,  "#000000"  }, // Tagbar left unselected {text,background,not used but cannot be empty}
	[SchemeInfoSel]  = { col_gray4, col_gray1,  "#000000"  }, // infobar middle  selected {text,background,not used but cannot be empty}
	[SchemeInfoNorm] = { col_gray3, col_gray1,  "#000000"  }, // infobar middle  unselected {text,background,not used but cannot be empty}
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance      title       tags mask   isfloating   monitor */
	{ "Gimp",         NULL,         NULL,       0,          1,           -1 },
	{ "copyq",        "copyq",      NULL,       0,          1,           -1 },
	{ "Gpick",        "gpick",      NULL,       0,          1,           -1 },
	{ "discord",      "discord",    NULL,       1 << 8,     0,           -1 },
	{ "qalculate-qt", "qalculate",  NULL,       0,          1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol  arrange function */
	{ "",     tile },    /* first entry is default */
	{ "",     NULL },    /* no layout function means floating behavior */
	{ "",     monocle },
};

/* key definitions */
#define MODKEY       Mod4Mask
#define ALTKEY       Mod1Mask
#define PRINTSCREEN  0xff61
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "/usr/bin/env", "bash", "-c", "$HOME/dev/config/scripts/linux/dmenu.sh", NULL };
static const char *termcmd[]  = { "st", NULL };

/* quick launch */
static const char *filemancmd[] = { "pcmanfm", NULL };
static const char *browsercmd[] = { "firefox", NULL };

/* tools */
static const char *screenshotcmd[]  = { "flameshot", "gui", NULL };
static const char *clipboardcmd[]   = { "copyq", "show", NULL };
static const char *colorpickercmd[] = { "gpick", "-p", NULL };
static const char *calculatorcmd[]  = { "/usr/bin/env", "bash", "-c", "$HOME/dev/config/scripts/linux/calculator.sh", NULL };

/* applications */
/*static const char *soundmixercmd[] = { "st", "-e", "pulsemixer", NULL };*/
static const char *calendarcmd[]   = { "st", "-e", "calcure", NULL };
static const char *processmgrcmd[] = { "st", "-e", "btop", NULL };
static const char *colorpickerwincmd[] = { "gpick", NULL };

/* scripts */
static const char *powermenucmd[] = { "/usr/bin/env", "bash", "-c", "$HOME/dev/config/scripts/linux/powermenu", NULL };
static const char *cleanupcmd[]   = { "/usr/bin/env", "bash", "-c", "$HOME/dev/config/scripts/linux/cleanup.sh", NULL };

/* audio controls */
static const char *volupcmd[]   = { "/usr/bin/env", "bash", "-c", "pactl set-sink-volume 0 +5% && pkill -RTMIN+10 dwmblocks", NULL };
static const char *voldowncmd[] = { "/usr/bin/env", "bash", "-c", "pactl set-sink-volume 0 -5% && pkill -RTMIN+10 dwmblocks", NULL };
static const char *volmutecmd[] = { "/usr/bin/env", "bash", "-c", "pactl set-sink-mute 0 toggle && pkill -RTMIN+10 dwmblocks", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ ALTKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_z,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask|ControlMask, XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = powermenucmd } },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	/* custom keymaps */
	{ MODKEY,                       XK_a,          spawn,      {.v = filemancmd } },
	{ MODKEY,                       XK_s,          spawn,      {.v = browsercmd } },
	{ MODKEY|ShiftMask,             XK_s,          spawn,      {.v = screenshotcmd } },
	{ 0,                            PRINTSCREEN,   spawn,      {.v = screenshotcmd } },
	{ MODKEY,                       XK_v,          spawn,      {.v = clipboardcmd } },
	{ ALTKEY,                       XK_v,          spawn,      {.v = clipboardcmd } },
	{ MODKEY,                       XK_c,          spawn,      {.v = calculatorcmd } },
	{ MODKEY|ControlMask,           XK_c,          spawn,      {.v = calendarcmd } },
	{ MODKEY|ALTKEY,                XK_c,          spawn,      {.v = cleanupcmd } },
	{ MODKEY|ShiftMask,             XK_c,          spawn,      {.v = colorpickercmd } },
	{ MODKEY|ShiftMask|ControlMask, XK_c,          spawn,      {.v = colorpickerwincmd } },
	{ MODKEY,                       XK_Escape,     spawn,      {.v = processmgrcmd } },

	/* audio controls */
	{ 0,                  XF86XK_AudioRaiseVolume, spawn,      {.v = volupcmd } },
	{ 0,                  XF86XK_AudioLowerVolume, spawn,      {.v = voldowncmd } },
	{ 0,                  XF86XK_AudioMute,        spawn,      {.v = volmutecmd } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} }, // scroll up
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} }, // scroll down
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

