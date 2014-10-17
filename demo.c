#define NUM 9
#include <os2.h>
#include "xlibos2.h"


struct POS {
   int x,y;
};

struct MY_DEMO {
   struct POS pos;
   int spx,spy;
   char xdir,ydir;
};


void iSprites(struct MY_DEMO *spr,char number)
{
    char i;

    for (i=0;i<number;i++) {
       spr[i].pos.x=rand()%300;
       spr[i].pos.y=rand()%200;
       spr[i].spx=(rand()%6)+1;
       spr[i].spy=(rand()%6)+1;
       spr[i].xdir=1;
       spr[i].ydir=1;
    }
}

void uSprite(struct MY_DEMO *spr)
{
    if (spr->xdir==1) {
       spr->pos.x=spr->pos.x+spr->spx;
    } else {
       spr->pos.x=spr->pos.x-spr->spx;
    }
    if (spr->ydir==1) {
       spr->pos.y=spr->pos.y+spr->spx;
    } else {
       spr->pos.y=spr->pos.y-spr->spx;
    }

    if (spr->pos.x > 290)
       spr->xdir=0;
    if (spr->pos.x < 10)
       spr->xdir=1;
    if (spr->pos.y > 210)
       spr->ydir=0;
    if (spr->pos.y < 10)
       spr->ydir=1;
}

main(int argc, char *argv[], char *envp[])
{
   int i;
   int x=0;
   struct IMAGE_TABLE MySprite;
   struct POS EraseQueue[NUM];
   struct MY_DEMO sprite[NUM];
   struct PAGE_TABLE pageTable;
   CHAR Apage=1,Vpage=0;



   InitModeX(&pageTable);
   iSprites(sprite,NUM);
   XCLoadImage("demo.obv",&MySprite);
   XCls(0);

   for (i=0;i<255;i++) {
       XBoxAt(0,0,rand()%300,rand()%200,rand()%(i+1));
       XWaitRetrace();
   }
   XLoadCel("grantp.cel",pageTable.p[0]);
   XCopyFromPageToPage(0,0,0,320,340,1,0,0);
   XCopyFromPageToPage(1,0,0,320,340,2,0,0);

   for (i=0;i<NUM;i++) 
      XCBltImage(sprite[i].pos.x,sprite[i].pos.y,&MySprite);
   for (i=0;i<NUM;i++) {
      EraseQueue[i]=sprite[i].pos;
   }


   while (!_kbhit()) {
       Apage++;
       Vpage++;
       XSetActivePage(Apage&0x01);
       XSetVisualPage(Vpage&0x01);

       for (i=0;i<NUM;i++) {
          XCopyFromPageToPage(2,EraseQueue[i].x,EraseQueue[i].y,20,16,Apage&0x01,EraseQueue[i].x,EraseQueue[i].y);
       }
       for (i=0;i<NUM;i++) {
          EraseQueue[i]=sprite[i].pos;
       } 
       for (i=0;i<NUM;i++) {
          uSprite(&sprite[i]);
       }
       for (i=0;i<NUM;i++) {
          XCBltImage(sprite[i].pos.x,sprite[i].pos.y,&MySprite);
       }
   } 

   CloseModeX();
}
