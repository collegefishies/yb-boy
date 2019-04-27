#include "lcd.h"
#include "input.h"

#ifndef menu_h
#define menu_h

#define MAX_ITEMS 20



class menu{
  private:
    int items = 0;
    String title;

  public:
    int color;
    int defaultColor;
    int titleColor;
    int index = 0;
    
    String menuItems[MAX_ITEMS];
    void (*menuProgs[MAX_ITEMS])();
    
    menu();

    //menu interface -- call to run
    void ui();

    //definer functions
    void defineMenuItems(String* Items, int numberOfItems);
    void defineMenuProgs(void (*menuProgsTemp[])(), int numberOfProgs);
    void defineTitle(String title);
    void defineCursorColor(int colorInput);

    //utility functions
    int len() const;
    void next();
    void previous();
    void printMenu();
};

#endif
