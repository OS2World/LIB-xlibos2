#define INCL_BASE
#define INCL_SUB
#define HANDLE 0
#define XRES 320
#define XRES_BYTES 80
#define YRES 240
#define NUM 5

#define SC_INDEX 0x3c4
#define GC_INDEX 0x3ce
#define CRTC_INDEX 0x3d4
#define CRT_C 0x3d5
#define MISC_OUT 0x3c2
#define MISC_OUTPUT 0x3c2

#include <stdlib.h>
#include <stdio.h>
#include <os2.h>



extern _Far16 _Pascal outp(USHORT, UCHAR);
extern _Far16 _Pascal outpw(USHORT, USHORT);
extern UCHAR _Far16 _Pascal inp(USHORT);
extern USHORT _Far16 _Pascal inpw(USHORT);
extern void draw(PCH,ULONG);



PCH screen;
CHAR ActivePage;
CHAR VisualPage;
USHORT SCREEN_Width=20;
struct _VIOPHYSBUF phys;
struct _VIOMODEINFO orig,moda;
PCH XPage[4];

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


int InitModeX(struct PAGE_TABLE *page_table)
{
  char tmp;
  int i;
  USHORT ModeXTable[]={0x0d06,0x3e07,0x4109,0xea10,0xac11,0xdf12,\
           0x0014,0xe715,0x0616,0xe317};

  phys.pBuf=(unsigned char *) 0xA0000;
  phys.cb=128000;
  moda.cb=12;
  moda.fbType=3;
  moda.color=8;
  moda.col=40;
  moda.row=25;
  moda.hres=320;
  moda.vres=200;
  VioGetMode(&orig, HANDLE);
  VioSetMode(&moda, HANDLE);
  VioGetPhysBuf(&phys,0);
  screen=MAKEP(phys.asel[0], 0);

  outpw((USHORT) SC_INDEX ,0x0604);
  outpw((USHORT) SC_INDEX ,0x0100);
  outp((USHORT) MISC_OUTPUT,0xe3);
  outpw((USHORT) SC_INDEX,0x0300);
  outp((USHORT) CRTC_INDEX,0x11);
  tmp=inp((USHORT)CRT_C);
  tmp=tmp & 0x7f;
  outp(CRT_C,tmp);

  for (i=0;i<10;i++)
      outpw((USHORT)CRTC_INDEX,ModeXTable[i]);

  outpw((USHORT)SC_INDEX,0x0f02);
  XPage[0]=screen;
  XPage[1]=screen+19200;
  XPage[2]=screen+38400;
  XPage[3]=XPage[2]+19200;

  for (i=0;i<4;i++) 
     page_table->p[i]=XPage[i];
  

  ActivePage=0;
  VisualPage=0;
  DosSetPriority(0,PRTYC_TIMECRITICAL,30,0);
}


void XPixel(int x,int y, char Color)
{
  int loc,tmp;
  char c;

  loc=((y * (SCREEN_Width<<2))+(x>>2))+(ActivePage*19200);
  x=x & 0x03;
  c=0x01;
  c=c<<x;
  tmp=(c<<8)+0x02;
  outpw(SC_INDEX,tmp);
  screen[loc]=Color;
}


int XReadPixel(int x, int y)
{
  int loc,tmp;
  char c;

  loc=((y * 80)+(x>>2))+(ActivePage*19200);
  x=x & 0x03;
  tmp=(x<<8) & 0x04;
  outpw(GC_INDEX,tmp);
  return screen[loc];
}


int CloseModeX(void)
{
  VioSetMode(&orig, HANDLE);
}


void XSelectPlane(char plane)
{
  int tmp;

  plane=plane & 0x0f;
  outpw((USHORT)SC_INDEX,(plane<<8) & 0x02);
  tmp=(plane<<8) & 0x04;
  outpw(GC_INDEX,tmp);
}


int XCls(char color)
{
  int count;

  outpw((USHORT)SC_INDEX,0x0f02);
  memset(screen,color,64000);
}


XBoxAt(int x1,int y1,int x2,int y2,char color)
{
  unsigned int y,lx,ly,ux,uy;
  unsigned long tmp;
  int size;
  ULONG pageadder;
 
  x1=x1>>2;
  x2=x2>>2;
  size=x2-x1;
  lx=min(x1,x2);
  ux=max(x1,x2);
  ly=min(y1,y2);
  uy=max(y1,y2);
  pageadder=19200*ActivePage;

  tmp=SCREEN_Width<<2;
  outpw((USHORT)SC_INDEX,0x0f02); /* all pages */
  for (y=(ly*tmp)+x1; y<=uy*tmp; y+=tmp) {
       memset(&screen[pageadder+y],color,size);
  }
}



void XWaitRetrace(void)
{
   char ret;
/* some prob here I'll fix it later */
   while ((inp(0x3da)&0x08)!=0);
   while ((inp(0x3da)&0x08)==0);
}

void XWaitPeriod(void)
{
   char ret;

   while ((inp(0x3da)&0x08)==0);
   while ((inp(0x3da)&0x08)!=0);
}

void XCopyFromPageToPage(char SourcePage,int x,int y,int xs,int ys,char DestPage,int dx,int dy)
{
   ULONG i,a,source,dest;
   char *td,*ts;

   dest=(dx>>2)+(dy*80)+(19200*DestPage);
   source=(x>>2)+(y*80)+(19200*SourcePage);
   xs=(xs>>2);

   outpw(0x03ce,0x0008);
   outpw(SC_INDEX,0x0f02);

   for (i=0;i<ys;i++ ) {
      for (a=0;a<xs;a++) 
         screen[dest+a]=screen[source+a];
      dest=dest+80;
      source=source+80;
   }

   outp(0x03cf,0xff);
}

void XSetVisualPage(char page)
{
   char tmp;
   char low,hi;
   USHORT offset;

   offset=page*19200;
   low=(offset & 0x0ff);
   hi=(offset >> 8);
   VisualPage=page;
 
   do{
    tmp=inp(0x3da);
   } while ((tmp & 0x08)!=0);

   outp(0x03d4,0x0c);
   outp(0x03d5,hi);
   outp(0x03d4,0x0d);
   outp(0x03d5,low);

   do{
    tmp=inp(0x3da);
   } while ((tmp & 0x08)==0);
}

void XSetActivePage(char page)
{
   ActivePage=page;
}

void XSetPan(USHORT pan)
{
   pan=pan>>2;
   outpw(0x03d4,0x000d);
   outpw(0x03d5,(pan & 0x0ff));
}

void XBltImage(int x,int y,char xs, char ys, char *image)
{
  int loc,tmp,tx,ty;
  char c,StartPlane;
  int a=0,pagenum;
  USHORT page[4]={0x102,0x0202,0x0402,0x0802};

  StartPlane=(x & 0x03);
  loc=((y * (SCREEN_Width<<2))+(x>>2));
  loc=loc+(ActivePage*19200);
 
  for (pagenum=0;pagenum<4;pagenum++){
     tmp=loc;
     outpw(SC_INDEX,page[StartPlane]);  /*pagenum*/
     StartPlane=StartPlane + 1;
     if (StartPlane==4) {
        StartPlane=0;
        loc++;
     }
     for (ty=0;ty<(ys);ty++) {
        for(tx=0;tx<(xs>>2);tx++){
           screen[tmp+tx]=image[a++];
        }
        tmp=tmp+80;
     }
  }
}

PCHAR XLoadImage(char filename[50],char *xs,char *ys)
{
    FILE *img;
    int i2=0;
    char ix,iy;
    char *image;
 
    img=fopen(filename,"rb");
    if (img==NULLHANDLE){
       return NULL;
       DosBeep(1000,100);
    }
    ix=fgetc(img);iy=fgetc(img);
    image=malloc(ix*iy);
    for (i2=0;i2<64;i2++) {
       image[0+i2]=fgetc(img);
       image[64+i2]=fgetc(img);
       image[128+i2]=fgetc(img);
       image[(128+64)+i2]=fgetc(img);
    }
    *xs=ix;
    *ys=iy;
    return image;
}


void XCBltImage(int x,int y, struct IMAGE_TABLE *image)
{
  USHORT loc,tmp,tx,ty;
  char c,StartPlane;
  USHORT a=0,pagenum;
  USHORT page[4]={0x102,0x0202,0x0402,0x0802};

  StartPlane=(x & 0x03);
  loc=((y * (SCREEN_Width<<2))+(x>>2))+(19200*ActivePage);

  for (pagenum=0;pagenum<4;pagenum++){
     tmp=loc;
     outpw(SC_INDEX,page[StartPlane]);  
     StartPlane=StartPlane + 1;
     if (StartPlane==4) {
        StartPlane=0;
        loc++;
     }
     draw(image->img[pagenum],screen + tmp);
  }
}

XSetRGB(char color,char red,char green,char blue)
{
   outp(0x3c8,color);
   outp(0x3c9,red);
   outp(0x3c9,green);
   outp(0x3c9,blue);
}

#define pn(x) (797-((255-x)*3))

XLoadCel(char filename[60],char *where)
{
   char *tmp,c;
   int i;
   char pal[800];
   FILE *cel;

   cel=fopen(filename,"rb");
   tmp=malloc(64000);
   for (i=0;i<800;i++) {
      pal[i]=fgetc(cel);
   } 
   for (i=255;i>=0;i--) {
      XSetRGB(i,pal[pn(i)],pal[pn(i)+1],pal[pn(i)+2]);
   } 

   for (i=0;i<64000;i++) {
      tmp[i]=fgetc(cel);
   } 

   outpw(SC_INDEX,0x102);
   for (i=0;i<64000;i=i+4) {
      where[(i>>2)]=tmp[i];
   } 
   outpw(SC_INDEX,0x202);
   for (i=1;i<64000;i=i+4) {
      where[(i>>2)]=tmp[i];
   }
   outpw(SC_INDEX,0x402);
   for (i=2;i<64000;i=i+4) {
      where[(i>>2)]=tmp[i];
   }
   outpw(SC_INDEX,0x802);
   for (i=3;i<64000;i=i+4) {
      where[(i>>2)]=tmp[i];
   }
}


XCLoadImage(char filename[60],struct IMAGE_TABLE *image)
{
    USHORT size[4],tmp;
    FILE *sprite;
    char hi,lo,i,*spr;

    sprite=fopen(filename,"rb");
    for (i=0;i<4;i++) {
        lo=fgetc(sprite);
        hi=fgetc(sprite);
        size[i]=(hi<<8)+lo;
    }
    for (i=0;i<4;i++) {
        image->img[i]=malloc(size[i]);
        spr=image->img[i];
        for (tmp=0;tmp<size[i];tmp++) {
            spr[tmp]=fgetc(sprite);
        }
    } 
    return 0;
}


