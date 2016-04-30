/*  Doga Mine Kaba  */
/*    121044004     */


#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define LCD_DAT PORTK /* Port K drives LCD data pins, E, and RS */ 
#define LCD_DIR DDRK  /* Direction of LCD port */ 
#define LCD_E 0x02  /* LCD E signal */ 
#define LCD_RS 0x01 /* LCD Register Select signal */ 
#define CMD 0 /* Command type for put2lcd */ 
#define DATA 1 /* Data type for put2lcd */
#define PUTTY 1 /* 1: enables putty flag */
#define enable() __asm(cli)
#define disable() __asm(sei)

/* Keypad */
#define KEYA 10
#define KEYB 11
#define KEYC 12
#define KEYD 13
#define KEYASTERISK 14
#define KEYSHARP 15
#define ROW_NUM 4
#define COL_NUM 4
#define NOTAKEY -1
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/* Functions for LCD */ 
void openlcd(void); /* Initialize LCD display */ 
void put2lcd(char c, char type); /*Write command /data to LCD controller */ 
void puts2lcd (char *ptr); /* Write a string to the LCD display */ 
void delay_50us(int n); /* Delay n 50 microsecond intervals */
void delay_1ms(int n) ; /* Delay n 1 millisecond intervals */
void clearLCD(void);

void printLevel(void);
void playTurn(void);
void printScore(int winCond);
void printIntToLcd (int n);
int ReadKey(void);

/* Buzzer */
interrupt (((0x10000 - Vtimch4)/2)-1) void timCh4(void);


/* Globals */

int hardnessLevel = 0;
int score = 0;
int wordIndex = 0;
int soundType = 0;

/* Word database */
char childsplayWords[40][5] = { "ABBA", "AWAY", "BABY", "BACK", "BAKE", 
                                "BEAR", "CAKE", "CODE", "CUTE", "DATA", 
                                "DOVE", "EASY", "EVEN", "ECHO", "FACE", 
                                "FEAR", "FEEL", "FIRE", "GAME", "GONE",
                                "GOOD", "GOLD", "HEAD", "HERO", "IDEA", 
                                "JAVA", "KNOW", "LADY", "MAMA", "MINI",
                                "NINE", "NOON", "OVER", "PINK", "POOR",
                                "RACE", "REAL", "ROOM", "SEEK", "WILL"};
                                
char easyWords[40][6] = { "ALPHA", "ALIEN", "APPLE", "ATLAS", "BERRY", 
                          "BUNNY", "CANDY", "COCOA", "DONUT", "DRESS",  
                          "ERASE", "ERROR", "FENCE", "FEVER", "FLOOR",  
                          "GAMER", "GOODS", "HEART", "HELLO", "HOBBY",
                          "IDEAL", "JAPAN", "JELLY", "KNOCK", "LABEL", 
                          "LEGAL", "LOGIC", "MAGMA", "MICRO", "METAL",
                          "NANNY", "NINJA", "ORBIT", "PAPER", "PEACE",
                          "QOUTE", "RIVER", "SCOPE", "SHARK", "THEME"};                             
                          
char mediumWords[40][7] ={"ACCEPT", "AFRAID", "ASTRAL", "BALLET", "BEFORE", 
                          "BLURRY", "CARROT", "CHERRY", "COSMIC", "DANCER",  
                          "DOCTOR", "EFFECT", "ELEVEN", "FARMER", "FOLLOW",  
                          "GALAXY", "GLOSSY", "HONEST", "INSANE", "IRONIC",
                          "JUNGLE", "KNIGHT", "LAPTOP", "LEGEND", "LESSON", 
                          "MANUAL", "MARKER", "MIRROR", "NEEDLE", "NORDIC",
                          "OPTION", "ORANGE", "OXYGEN", "PASTEL", "PEANUT",
                          "RABBIT", "RANDOM", "SELECT", "TESTER", "WINNER"};
                          
char hardWords[40][8] = { "ABILITY", "ANDROID", "ALLERGY", "ATHEIST", "BALANCE", 
                          "BLOSSOM", "BROWNIE", "CALCIUM", "CHEETAH", "CUPCAKE",  
                          "DECIMAL", "DESSERT", "DOLPHIN", "ELEGANT", "EXTREME",  
                          "FAILURE", "FASHION", "FIREFLY", "FRECKLE", "GRAVITY",
                          "HEALTHY", "HOLIDAY", "ICEBERG", "IMPROVE", "JEWELRY", 
                          "JASMINE", "KINGDOM", "LIBERTY", "LOBSTER", "MERMAID",
                          "NEUTRON", "OCTOPUS", "ORGANIC", "PERFUME", "QUALITY",
                          "RACCOON", "RAINBOW", "SNOWMAN", "TROUBLE", "VICTORY"};
                          
char extremeWords[40][9] = {"ACCIDENT", "AEROBICS", "AIRFIELD", "BARBEQUE", "BETRAYER", 
                            "BIOGENIC", "CAFFEINE", "CHIPOTLE", "COLONIST", "DAUGHTER",  
                            "DETOXIFY", "EMPLOYER", "ENHANCED", "EQUATION", "EXCITING",  
                            "FAITHFUL", "FLAMINGO", "FLAVOURS", "GAMEPLAY", "GEMSTONE",
                            "GENEROUS", "HABITANT", "HAZELNUT", "HYDROSOL", "HYDROXYL", 
                            "IDEOLOGY", "IGNORANT", "INJECTOR", "KANGAROO", "MANICURE",
                            "MEDIEVAL", "MEMORIZE", "MOURNING", "NOMINATE", "NUCLEONS",
                            "OXIDIZER", "PARADOXY", "PSYCHICS", "QUOTIENT", "WASTEFUL"};

void main(void) {

  disable();

  openlcd(); /* Initialize LCD display */
	DDRA = 0x0F;   /* Enable keypad */
	
	/* Enable putty */ 
  SCI0BDL = 156;
  SCI0BDH = 0;
  SCI0CR1 = 0x00;
  SCI0CR2 = 0x0C;
  
  /* Buzzer configurations */
  DDRT = DDRT | BIT5; /* PORTT as output for PT5 buzzer */
	
	TSCR1 = BIT7; /* Turn on timer subsystem */ 
	TSCR2 = 0x05; /* set prescaler */
	TIOS = TIOS | BIT4; /* output compare ch4 */
	TCTL1 = (TCTL1 | BIT0) & ~BIT1; /* toggle ch4 */
	TIE = TIE | BIT4; /* enable interrupt for ch4 */
	TFLG1 = TFLG1 | BIT4; /* enable flag of ch4 */
	
	enable();
	
  
  while(1) {
    playTurn();
  	put2lcd(0x01,CMD); 	/* clear screen, move cursor to home */ 
  	delay_1ms(10); 		/* wait until "clear display" command */ 
						          /* complete */ 
  }

}

/* LCD FUNCTIONS */
/* code taken from pdf */

void openlcd(void) { 
	LCD_DIR = 0xFF;		/* configure LCD_DAT port for output */ 
	delay_1ms(100); 	/* Wait for LCD to be ready */ 
	put2lcd(0x28,CMD); 	/* set 4-bit data, 2-line display, 5x7 font */ 
	put2lcd(0x0F,CMD); 	/* turn on display, cursor, blinking */ 
	put2lcd(0x06,CMD); 	/* move cursor right */ 
	put2lcd(0x01,CMD); 	/* clear screen, move cursor to home */ 
	delay_1ms(2); 		/* wait until "clear display" command */ 
						        /* complete */ 
}

void puts2lcd (char *ptr) { 
	while (*ptr) 
	{  
		put2lcd(*ptr,DATA);	/* Write data to LCD */ 
		delay_50us(DATA);	/* Wait for data to be written */ 
		ptr++;	/* Go to next character */ 
	} 
}

void put2lcd(char c, char type) { 

	char c_lo, c_hi;
	c_hi = (c & 0xF0) >> 2; /* Upper 4 bits of c */
	c_lo = (c & 0x0F) << 2; /* Lower 4 bits of c */
	
	if (type == DATA) LCD_DAT |= LCD_RS; /* select LCD data register */
	else LCD_DAT &= (~LCD_RS); /* select LCD command register */
	
	if (type == DATA)
		LCD_DAT = c_hi|LCD_E|LCD_RS; /* output upper 4 bits, E, RS high */
	else
		LCD_DAT = c_hi|LCD_E; /* output upper 4 bits, E, RS low */
	LCD_DAT |= LCD_E; /* pull E signal to high */
	__asm(nop); /* Lengthen E */
	__asm(nop);
	__asm(nop);
 
	LCD_DAT &= (~LCD_E); /* pull E to low */
	if (type == DATA)
		LCD_DAT = c_lo|LCD_E|LCD_RS; /* output lower 4 bits, E, RS high */
	else
		LCD_DAT = c_lo|LCD_E; /* output lower 4 bits, E, RS low */
	LCD_DAT |= LCD_E; /* pull E to high */
	__asm(nop); /* Lengthen E */
	__asm(nop);
	__asm(nop);
 
	LCD_DAT &= (~LCD_E); /* pull E to low */
	delay_50us(1); /* Wait for command to execute */
}

#define D50US 133	/* Inner loop takes 9 cycles */ 
				        	/* need 50x24 = 1200 cycles */

void delay_50us(int n) { 
	volatile int c;
	for (; n>0; n--) 
		for (c=D50US; c>0; c--); 
}

void delay_1ms(int n) { 
	for (; n>0; n--) 
		delay_50us(200); 
}

void clearLCD(void){

  put2lcd(0x01,CMD); 	/* clear screen, move cursor to home */ 
	delay_1ms(2); 		/* wait until "clear display" command */ 
						        /* complete */  
}

void printLevel(void){
  
	char *msg = "    LEVEL ";
	
	put2lcd(0x01,CMD); 	/* clear screen, move cursor to home */ 
	delay_1ms(2); 		/* wait until "clear display" command */ 
						        /* complete */  
  puts2lcd(msg);
  printIntToLcd (hardnessLevel+1); /* print level num */
  put2lcd(0xC0,CMD); /* move cursor to 2nd row, 1st column */

}

void playTurn(void){
  
  int i, j;
  char key1 = -1, key2 = -1; /* input keys for ascii letters */
  char input;
  char* askedWord;  /* a chosen word to ask in this turn */
  int knownLet = 0; /* known letter count for one input */
  int allKnownLet = 0;  /* known letter count for all inputs */
  char knownLetters[9] = {0}; /* array to contain known letters */
  int letterExists = 0; /* flag of previously known letter */
  int turnOver = 0; /* game turn flag */
  int wrongLetCount = 6; /* decreases with every wrong letter */  

  /* get a random index */
  wordIndex = TCNT % 40;
  
  /* chose a word */
  switch(hardnessLevel){
    case 0: askedWord = childsplayWords[wordIndex];
            break;
    case 1: askedWord = easyWords[wordIndex];
            break;
    case 2: askedWord = mediumWords[wordIndex];
            break;
    case 3: askedWord = hardWords[wordIndex];
            break;
    case 4: askedWord = extremeWords[wordIndex];
            break;
  }
  
  
  put2lcd(0x01,CMD);  /* clear screen, move cursor to home */
  delay_1ms(2); /* wait for command */
  printLevel(); /* print level to the first row */ 
  
  put2lcd(0xC0,CMD);  /* move cursor to 2nd row, 1st column */
      
  for(i=0; i<hardnessLevel+4; ++i)
    put2lcd('-', DATA); /* ask word */
  
  delay_1ms(50);
  
  while(!turnOver){
  
    if(PUTTY){
    
      /* putty input */
      while ((SCI0SR1 & 0x20) == 0);
      input = SCI0DRL;
      SCI0DRL = input;
      
    } else{
 
      /* keypad input */
      while(key1 == -1){
        key1 = ReadKey();
      }
      
      while(key2 == -1){
        key2 = ReadKey();
      }
      
      input = key1*10 + key2;
      key1 = -1;
      key2 = -1;
    }
    
    
    
    put2lcd(0x14,CMD);
    put2lcd(input, DATA);
    delay_1ms(10);
    put2lcd(0x10,CMD); /* move cursor to previous column */
    delay_1ms(10);
    put2lcd(0x10,CMD); /* move cursor to previous column */
    delay_1ms(10);
    
    letterExists = 0;
    /* check if a previously known letter is chosen */
    for(j=0; j<9; ++j){
      if(input == knownLetters[j]){
        letterExists = 1;
      }
    }
    
    knownLet = 0;
    
    if(!letterExists){
    
      put2lcd(0xC0,CMD); /* move cursor to 2nd row, 1st column */  
      for(i=0; i<hardnessLevel+4; ++i) {
      
        if(input == askedWord[i]) {
          ++allKnownLet;
          ++knownLet;
          knownLetters[i] = askedWord[i]; /* contain known letter */
          put2lcd(askedWord[i], DATA);
          
          soundType = 2;
          delay_1ms(10);
          soundType = 0;
        }
        else
          put2lcd(0x14,CMD); /* move cursor to next column */ 
      }
      
      if(knownLet == 0 ){ /* wrong letter input */
      
        --wrongLetCount;
        soundType = 1;
        delay_1ms(10);
        soundType = 0;
        
        if(score >= 5)
          score -= 5;
      }
      
      else
        score = score + knownLet * 10;
    
    } else {  /* same letter input is wrong */
      --wrongLetCount;
      soundType = 1;
      delay_1ms(10);
      soundType = 0;
      
      if(score >= 5)
        score -= 5;
    }
    
    if(wrongLetCount == 0 || allKnownLet == (hardnessLevel+4))
      turnOver = 1;
    
  }
  
  if(wrongLetCount > 0) {
    if(hardnessLevel < 4)
      ++hardnessLevel;
  }
  else{
    if(hardnessLevel > 0)
      --hardnessLevel;
  }
  delay_1ms(50);
  
  printScore(wrongLetCount);
  wrongLetCount = 6;
  delay_1ms(100);
}

void printScore(int winCond){

  put2lcd(0x01,CMD); 	/* clear screen, move cursor to home */
  delay_1ms(2);  /* wait for command */
  
  if(winCond)
    puts2lcd("    YOU WON!");
  else
    puts2lcd("    YOU LOSE!");
  
  put2lcd(0xC0,CMD); /* move cursor to 2nd row, 1st column */ 
  puts2lcd(" SCORE: ");
  printIntToLcd (score);
     
}

void printIntToLcd (int n) {
    if( n < 10 ) {
      put2lcd(n + 48, DATA);
    } else {
      printIntToLcd(n/10); 
      put2lcd(n%10 + 48, DATA);
    }
}

/* code taken from https://gist.github.com/furkantektas/e1ed65ffd13d6d74bdaa */
int ReadKey() {
	int row,
	    column;
	
	const char row_mask[ROW_NUM] = { 0xFE,0xFD,0xFB,0xF7 };
	const char col_mask[COL_NUM] = { BIT4,BIT5,BIT6,BIT7 };        
	const unsigned int keys[ROW_NUM][COL_NUM] = 
	{ 1,2,3,10,
	  4,5,6,11,
	  7,8,9,12,
	  14,0,15,13 };
		
	for(row=0 ; row < ROW_NUM ; ++row) {
		PORTA = 0xFF;// Reset PortA
		for(column=0; column < COL_NUM ;++column) {
			PORTA = PORTA & row_mask[row]; // Set specific row to 0
			
			if( (PORTA & col_mask[column]) == 0 ) {
			// Check if any key is pressed
 
				delay_1ms(10);                      
				// Wait 50ms and check again for make sure key is pressed.
				if( (PORTA & col_mask[column]) == 0 ) {
					return keys[row][column]; 
				}
			}
		}
	}
	return NOTAKEY;
}

interrupt (((0x10000 - Vtimch4)/2)-1) void timCh4(void){
  if(soundType == 1) { // wrong guess
    PTT = PTT ^ BIT5; // Toggle buzzer
    TC4 += 6000;
	} else if(soundType == 2) {  // right guess
	  PTT = PTT ^ BIT5; // Toggle buzzer
    TC4 += 2000;  
	} else {
	  PTT = PTT & ~BIT5; // Disable buzzer
	}
	
	TFLG1 |= BIT4;  
  
}
