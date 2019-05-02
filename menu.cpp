#include "menu.h"

#ifndef menu_cpp
#define menu_cpp

/*
class menu{
  private:
    int items = 0;
    String title;
    
  public:
    int index = 0;
    
    String menuItems[];

    menu();

    void defineMenuItems(String* Items, int numberOfItems);
    void defineTitle(String title);
    
    int len() const;
    void next();
    void back();

    void printMenu();
}
*/

/*
 * Main user interface
 */
void menu::ui(){
  if(items == 0)
    return;
    
  printMenu();
  while(true){
    delay(25);
    char key = keypad.getKey();
  
    //wait for key until pressed
    if(!key)
      continue;
  
    //keypressed actions
    switch (key) {
      case '2':
        previous();
        printMenu();
        break;
      case '8':
        next();
        printMenu();
        break;
      case '#':
        cls(); 
        (menuProgs[index])();
        if(!loop){
          return;
        }
        printMenu();
        
        break;
      case '4':
        return;
     
    }
  }
}

/*
 * Constructors
 */
menu::menu() {
  //define colors
  defaultColor = ST7735_WHITE;
  color = ST7735_YELLOW;
  titleColor = ST7735_GREEN;

  lcd.setTextColor(defaultColor);
  title = "=======Main Menu=======";
  loop = true;
}

/*
 * Definer Functions
 */
void menu::defineMenuItems(String* Items, int numberOfItems){
  for (int i = 0; i < min(numberOfItems, MAX_ITEMS); i++){
    menuItems[i] = Items[i];
    items = i+1;
  }
}

void menu::defineMenuProgs(void (*Programs[])(), int numberOfProgs){
  for (int i = 0; i < min(numberOfProgs, MAX_ITEMS); i++){
    menuProgs[i] = Programs[i];
    items = i+1;
  }
}

void menu::defineTitle(String text){
  title = text;
}


void menu::defineCursorColor(int colorInput){
  color = colorInput;
}

/*
 * Utility Functions
 */
void menu::printMenu() {
  //prep screen
  cls();
  
  //print title
  lcd.setTextColor(titleColor);
  lcd.println(title);

  //print menu
  for(int i = 0; i < min(items, SCR_HEIGHT_IN_CHARS-2); i++)
  {
    if( i == index ){
      lcd.setTextColor(color);
    } else {
      lcd.setTextColor(defaultColor);
    }
    lcd.print(">");
    lcd.println(menuItems[i]);
  }

  //restore to default color
  lcd.setTextColor(defaultColor);

  //print useful info
  lcd.setCursor(0, CHARH*(SCR_HEIGHT_IN_CHARS-1));
  lcd.print("2:UP,8:DWN,#:SEL,4:BACK");
}

void menu::next(){
  index = (index+1)%items;
}

void menu::previous(){
  index = (items + index - 1) % items;
}
#endif
