/****************************************
XLibOS/2  --  Graeme Blackley

28/5/93


/****************************************/
struct SPRITE_INFO {
     PCHAR image;
     char xs,ys;
     int x,y;
     int spx,spy;
     char xdir,ydir;
};

struct IMAGE_TABLE {
     PCHAR img[4];
};

struct PAGE_TABLE {
     PCHAR p[4];
};

int InitModeX(struct PAGE_TABLE *page_table);
void XSelectPlane(char plane);
int XCls(char color);
int CloseModeX(void);


void XWaitRetrace(void);
void XWaitPeriod(void);
void XSetVisualPage(char page);
void XSetVisualPage(char page);
void XSetPan(USHORT pan);



void XPixel(int x,int y, char Color);
int  XReadPixel(int x, int y);
void XBoxAt(int x1,int y1,int x2,int y2,char color);

void XCopyFromPageToPage(char SourcePage,int x,int y,int xs,int ys,char DestPage,int dx,int dy);
void XBltImage(int x,int y,char xs, char ys, char *image);
PCHAR XLoadImage(char filename[50],char *xs,char *ys);
void XCBltImage(int x,int y, struct IMAGE_TABLE *image);
void XSetRGB(char color,char red,char green,char blue);
void XLoadCel(char filename[60],char *where);
void XCLoadImage(char filename[60],struct IMAGE_TABLE *image);


