#include <r0ketlib/config.h>
#include <r0ketlib/fonts.h>
#include <r0ketlib/fonts/smallfonts.h>
#include <r0ketlib/display.h>
#include <r0ketlib/print.h>
#include <r0ketlib/render.h>
#include <r0ketlib/keyin.h>
#include <r0ketlib/select.h>
#include <r0ketlib/stringin.h>
#include <r0ketlib/execute.h>
#include <r0ketlib/idle.h>
#include <r0ketlib/colorin.h>
#include <r0ketlib/itoa.h>
#include <rad1olib/battery.h>
#include <string.h>

#define BAT_MIN 3500
#define BAT_MAX 4000

#define RGB(r, g, b) ((r << 5) + (g << 2) + (b))

/**************************************************************************/

static void batteryLevel(void) {
    int dy = 0;
    int dx = 0;
    int32_t voltage = 0;
    float purcentage = 0;
    uint8_t color = 0x00;

    setExtFont("normal");
    dy = RESY - getFontHeight();
    dx = DoString(dx, dy, "Bat");

    if (batteryCharging()) {
        dx = DoString(dx, dy, "(+)");
    }
    dx = DoString(dx, dy, ":");

    voltage = batteryGetVoltage();
    purcentage = (voltage - (float)BAT_MIN) / ((float)BAT_MAX - BAT_MIN) * 100.0;

    if (purcentage >= 50) {
        color = RGB(0, 7, 0);
    }
    else if (purcentage >= 10) {
        color = RGB(7, 7, 0);
    }
    else {
        color = RGB(7, 0, 0);
    }
    setTextColor(GLOBAL(nickbg), color);
    dx = DoString(dx, dy, IntToStr(purcentage, 3, F_LONG));
    DoString(dx, dy, "%");
}

static void dataLove(void)
{
    int dx = 50;
    int dy = 0;

    setExtFont(GLOBAL(nickfont));
    dy= (RESY - getFontHeight()) / 3 * 2;
    DoString(dx, dy, "<3");
}

void simpleNickname(void) {
    int dx=0;
	int dy=0;
    static uint32_t ctr=0;
	ctr++;

	setExtFont(GLOBAL(nickfont));
	dx=DoString(0,0,GLOBAL(nickname));
    dx=(RESX-dx)/2;
    if(dx<0)
        dx=0;
    dy=(RESY-getFontHeight())/3;

    lcdFill(GLOBAL(nickbg));
    setTextColor(GLOBAL(nickbg),GLOBAL(nickfg));
	DoString(dx,dy,GLOBAL(nickname));
    dataLove();
    batteryLevel();
	lcdDisplay();

    getInputWaitTimeout(100);
    return;
}

void fancyNickname(void) {
    if(GLOBAL(l0nick)){
        if(execute_file(GLOBAL(nickl0)))
            GLOBAL(l0nick)=0;
        setTextColor(0xFF,0x00);
    }

    if(!GLOBAL(l0nick))
        simpleNickname();
    return;
}

/**************************************************************************/

// every function name starting with init_ is added to generated_init automatically
void init_nick(void){
	readTextFile("nick.cfg",GLOBAL(nickname),MAXNICK);
	readTextFile("font.cfg",GLOBAL(nickfont),FLEN);
	readTextFile("l0nick.cfg",GLOBAL(nickl0),FLEN);
}

//# MENU nick editNick
void doNick(void){
	input("Nickname:", GLOBAL(nickname), 32, 127, MAXNICK-1);
	writeFile("nick.cfg",GLOBAL(nickname),strlen(GLOBAL(nickname)));
	getInputWait();
}

//# MENU nick changeFont
void doFont(void){
    getInputWaitRelease();
    if( selectFile(GLOBAL(nickfont),"F0N") != 0){
        lcdPrintln("No file selected.");
        return;
    };
	writeFile("font.cfg",GLOBAL(nickfont),strlen(GLOBAL(nickfont)));

    lcdClear();
    setIntFont(&Font_7x8);
    lcdPrintln("Test:");
    setExtFont(GLOBAL(nickfont));
    lcdPrintln(GLOBAL(nickname));
    lcdDisplay();
    setIntFont(&Font_7x8);
    while(!getInputRaw())delayms(10);
}

//# MENU nick chooseAnim
void doAnim(void){
    getInputWaitRelease();
    if( selectFile(GLOBAL(nickl0),"N1K") != 0){
        lcdPrintln("No file selected.");
        GLOBAL(l0nick)=0;
        return;
    };
	writeFile("l0nick.cfg",GLOBAL(nickl0),strlen(GLOBAL(nickl0)));
    GLOBAL(l0nick)=1;
    saveConfig();
    getInputWaitRelease();
}

//# MENU nick setFGcolor
void doColorFG(void){
    GLOBAL(nickfg)=colorpicker("Foreground:", GLOBAL(nickfg));
    saveConfig();
    getInputWaitRelease();
}

//# MENU nick setBGcolor
void doColorBG(void){
    GLOBAL(nickbg)=colorpicker("Background:", GLOBAL(nickbg));
    saveConfig();
    getInputWaitRelease();
}
