/**
   Based on this example:
   https://circuitdigest.com/microcontroller-projects/arduino-touch-screen-calculator-tft-lcd-project-code
*/
/*
 * PROGRAMA DE UN TECLADO ALFANUMERICO IMPLEMENTADO EN UN TOUCHSCREEN 2.4" TFT LCD SHIELD
 */
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GRAY 0x7BEF
#define GRAYK 0XDCE3EB
#define LIGHT_GRAY 0xC618
#define GREEN 0x07E0
#define LIME 0x87E0
#define BLUE 0x001F
#define RED 0xF800
#define AQUA 0x5D1C
#define YELLOW 0xFFE0
#define MAGENTA 0xF81F
#define CYAN 0x07FF
#define DARK_CYAN 0x03EF
#define ORANGE 0xFCA0
#define PINK 0xF97F
#define BROWN 0x8200
#define VIOLET 0x9199
#define SILVER 0xA510
#define GOLD 0xA508
#define NAVY 0x000F
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define VERDE_AZUL 0x0C0C

#define MINPRESSURE 2                                                  //PARAMETROS PARA LA PRESION MAXIMA Y MINIMA DE LA PANTALLA.
#define MAXPRESSURE 500

/*#define TS_MINX 194
#define TS_MINY 891
#define TS_MAXX 846
#define TS_MAXY 114
*/

#define TS_MINX 145
#define TS_MINY 110                                                         //TODOS ESTOS PARAMETROS SON PARA MODIFICAR LA SENSIBILIDAD Y LOS RANGOS DE LA PANTALLA, TENER CUIDADO AL MODIFICAR
#define TS_MAXX 880
#define TS_MAXY 915

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 900);                          //funcion encargada poner los pines de la pantalla ; el parametro 300 es la sensibilidad de la pantalla
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);             //Start communication with LCD

String Numbers[4][3] = {                // Matriz de la parte numerica del teclado
  { "1", "2", "3"},
  { "4","5", "6"},
  {"7","8", "9"},
  {"abc","0", "<"}
};

String symbol[3][10] = {          //Matriz del teclado
  { "Q", "W", "E", "R","T", "Y", "U","I", "O","P" },
  { "A", "S", "D", "F","G", "H", "J","K", "L"," " },
  {"#", "Z", "X", "C","V", "B", "N","M", "<","SPC"}
};
String texto[12]={};            // vector de las 12 caracteres que caben en la pantalla

//Definicion de los parametros de posicion del toque, se usa en la dentro de la funcion waitTouch
TSPoint p;                   
TSPoint tp;


long Number;
int j=0,ind=0 ,ind1=0;
String caracter;
boolean result = false;


void setup() {
  Serial.begin(9600); //Use serial monitor for debugging
  delay(1000);
  tft.reset(); //Always reset at start
  tft.begin(0x9341); // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(1); // Funcion encargada de la direccion de la pantalla --> 0 y 2 para modo vertical      1 y 3 para modo horizontal
  tft.fillScreen(WHITE);
  IntroScreen();
  //Draw the Result Box
  tft.fillRect(0, 0, tft.width(), 80, WHITE);  //dibujar la caja de resultado 
 Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height()); //pedir las dimensiones de la pantalla y escribirlas en la monitor serial
  draw_BoxNButtons(); // funcion que dibuja los botones del teclado

}

void loop() {
  waitTouch();  // funcion que recibe el toque del usuario

  DetectButtons(ind1);// funcion que detecta que boton toco con una variable que indica si esta en modo numerico o modo alfabetico

  DisplayResult(ind);// funcion que muestra en pantalla el boton que se ha presionado

  delay(300);
}

void waitTouch() {
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));
  
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  tp.y=p.x;     // variable creada para mostrar el punto en Y se toco la pantalla
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);       // *************** estas coordenadas estan hechas para la pantalla de manera horizontal y con el *************************
  tp.x=p.y;    // variable creada para mostrar el punto en X se toco la pantalla    **********  cable de alimentacion a la derecha, si se desea cambiar la orientacion de la pantalla tambien se debe cambiar los tp.y=p.y   Y el tp.x=p.x*****************
  
  //tp.x = map(tp.x, TS_MINX, TS_MAXX, tft.width(), 0);
  //tp.y = map(tp.y, TS_MINY, TS_MAXY, tft.height(), 0);
  //Serial.print("->");
  Serial.print(tp.x); Serial.print(","); Serial.println(tp.y); // imprime la coordenadas del toque en el monitor serial
}

void DetectButtons(int ind1) {
  if(ind1==0){                                  //indicativo 0 si se va a detectar los botones alfabeticos 
      if ((tp.y<80)&&(tp.x>0)){ // detecta la caja de resultado, si la toca o toca el boton de borrado no se imprime ningun caracter
        caracter="<";
        return;}
      
      if ((tp.y < 133)&&(tp.y>80)) { // detecta los botones de la columna 1   ***ambos if encasillan la zona de toque, la caja de resultados ocupa los primero 80 pixeles en el eje y
          for (int i = 0; i < 320; i+=32) {                                   //***luego de eso se divide el resto de pantalla en partes iguales en este caso 3 para cada una de las columnas
            if((tp.x>i)&&(tp.x<i+32)){                                        //***por ultimo el se halla el toque en la posicion x , los cuales estan divididos en 10 espacios igual de 32 pixeles cada uno
              caracter=symbol[0][i/32];                                   // se halla el caracter en la columna 1
              Serial.println(caracter);                                 // se imprime en el monitor serial para pruebas
            }
          }
      }
        if ((tp.y < 186)&&(tp.y>133)) { // detecta los botones de la columna 2
          for (int i = 0; i < 288; i+=32) {
            if((tp.x>i)&&(tp.x<i+32)){
              caracter=symbol[1][i/32];
              Serial.println(caracter);
            }
          }
      }
        if ((tp.y < 240)&&(tp.y>186)) { // detecta los botones de la columna 3
          for (int i = 0; i < 256; i+=32) {
            if((tp.x>i)&&(tp.x<i+32)){
              caracter=symbol[2][i/32];
              Serial.println(caracter);
              }
            }
          }
      if(tp.y>133){                     //encasillamiento especial para el boton de espacio o SPC
        if(tp.x>288){
          caracter=symbol[1][9];
          Serial.println(caracter);
        }
      }
      if(tp.y>186){                           // encasillamiento para el boton de borrado o <
        if((tp.x>256)&&(tp.x<288)){
           Serial.println("borrar");
           DisplayResult(1);         //manda a la funcion de borrado
           caracter="<";
        }
      }
 }

 if(ind1==1){                                     //indicativo 1 si se va a detectar los botones numericos 
      if ((tp.y<80)&&(tp.x>0)){ 
        caracter="<";
        return;}
      if ((tp.y < 120)&&(tp.y>80)) { // detecta los botones de la columna 1         *** funciona igual que los botones alfabeticos solos que las divisiones son diferentes
              for (int i = 0; i < 320; i+=106) {                                     //*** son 4 columnas de 40 pixeles cada una y 3 filas de 106 pixeles cada una 
                if((tp.x>i)&&(tp.x<i+106)){
                  caracter=Numbers[0][i/106];
                  Serial.println(caracter);
                }
              }
          }
    
      if ((tp.y < 160)&&(tp.y>120)) { // detecta los botones de la columna 2
              for (int i = 0; i < 320; i+=106) {
                if((tp.x>i)&&(tp.x<i+106)){
                  caracter=Numbers[1][i/106];
                  Serial.println(caracter);
                }
              }
      }
      if ((tp.y < 200)&&(tp.y>160)) { // detecta los botones de la columna 3
          for (int i = 0; i < 320; i+=106) {
            if((tp.x>i)&&(tp.x<i+106)){
              caracter=Numbers[2][i/106];
              Serial.println(caracter);
            }
          }
      }
      if ((tp.y < 240)&&(tp.y>200)) { // detecta los botones de la columna 4
          for (int i = 0; i < 214; i+=106) {
            if((tp.x>i)&&(tp.x<i+106)){
              caracter=Numbers[3][i/106];
              Serial.println(caracter);
            }
          }
      }
      if ((tp.y < 240)&&(tp.y>200)) {   //enasillamiento especial boton de borrar o <
        if((tp.x>214)&&(tp.x<320)){
          Serial.println("borrar");
           DisplayResult(1);      //manda a comando para borrar
           caracter="<";
        }
      }
 }
}
void DisplayResult(int ind)
{
  if(ind==0){                       //indicativo 0 para mostrar en pantalla el caracter seleccionado
    if(j>=12) {                     // 12 caracteres que caben en la caja de resultado
      j=0;
      tft.fillRect(0, 0, tft.width(), 80, WHITE);  //cuando los 12 caracteres sin superados se limpia la caja de resultados
    }
    tft.setCursor(10+(j*25), 20);      // pone el cursor en posicion Y =20 y en la posicion X va cambiando de acuerdo al numero de j
    tft.setTextSize(4);
    tft.setTextColor(BLACK);
    texto[j]=caracter;            //imprime el caracter en la caja de resultado
    if(caracter=="#"){            // si se presiona el caracter # dibuja los botones numericos
      drawNumbers();
      ind1=1;                     // indicativo para detectar los botones numericos
      return;
    }
    if(caracter=="abc"){           // si se presiona el caracter abc dibuja los botones alfabeticos
      draw_BoxNButtons();
      ind1=0;                      // indicativo para detectar los botones alfabeticos
      return;
    }
     if(caracter=="<"){             // para no mostrar en la pantalla el caracter de borrar 
      return;
    }
    if(caracter==" "){             // mostrar cuadrado de que se avanzo un espacio
      tft.fillRect((5+(j)*26), 20,30, 30, BLUE);
      delay(100);
      tft.fillRect((3+(j)*26), 0,tft.width(), 80, WHITE); 
    }
    tft.println(texto[j]); //update new value
      j++;                        // aumenta el contador del vector de la pantalla 
  }
  if(ind==1){                   // indicativo 1 para borrar el ultimo caracter en pantalla
      j--;
      int posx=3+(j*25);          // se posiciona en x el ultimo caracter escrito
      tft.fillRect(posx, 20,30, 30, BLUE);
      delay(100);
      tft.fillRect(posx, 0,tft.width(), 80, WHITE);  //dibuja la un cuadro blanco en la caja de resultados, desde la posicion x hasta el final de la pantalla
      ind=0;                    // cambia el indicativo para volver a poner los caracteres en la caja de resultados
      return; 
  }
}

void IntroScreen()      //funcion de presentacion
{
  tft.setCursor (25, 120);
  tft.setTextSize (3);
  tft.setTextColor(RED);
  tft.println("Teclado Tactil");
  tft.setCursor (30, 160);
  tft.println("v1.0");
  tft.setCursor (30, 220);
  tft.setTextSize (2);
  tft.setTextColor(BLUE);
  tft.println("Mauricio Santis");
  delay(1800);
}
void drawNumbers(){                                 // funcion de dibujar los botones   
    int b=0;                                        // contador para moverse entre las divisiones de los botones
    for (int i = 0; i <= 4; i++) {                  // for que crea las cajas por filas en la pantalla
      b=80*i;
      tft.fillRect(b,80, 80, 53, LIGHT_GRAY);
      tft.fillRect(b,133, 80, 53, LIGHT_GRAY);
      tft.fillRect(b,186,  80, 53, LIGHT_GRAY);
      }
        //Draw Horizontal Lines
    for (int h=240; h >=80 ; h -=40 ){                  //dibuja las divisiones de los botones en forma horizontal
    tft.drawFastHLine(0, h, tft.width(),SILVER);        // desde la altura maxima disminuyendo en botones de 40 pixeles hasta llegar a la caja de resultado en el pixel 80
    }
    
  //Draw Vertical Lines
  for (int v = 0; v <= tft.width(); v += 106){          // dibuja las lines verticales desde el pricipio de la pantalla en botones de 106 pixeles hasta el final de la pantalla
    tft.drawFastVLine(v, 80, tft.width(), SILVER);
  }

  
  //Draw Numbers                                
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
    for (int i = 0; i < 4; i++) {                               //dibuja el caracter del numero sobre el botones de todas las filas 
      for (int j = 0; j < 3; j++) {
      tft.setCursor(53 + (106 * j), 95 +(40 * i));               // se mueve en x cada 106 pixeles, mas 10 pixeles para centrar el caracter.... 
      tft.println(Numbers[i][j]);                                 //y en y se mueve cada 40 pixeles mas 95 para empezar despues de la caja de resultdos centrado
    }
   }
  
}


void draw_BoxNButtons()
{
  int b=0;  
  for (int i = 0; i <= 10; i++) {                     // for que dibuja todos los botones alfabeticos
      b=32*i;
      tft.fillRect(b,80, 32, 53, LIGHT_GRAY);
      tft.fillRect(b,133, 32, 53, LIGHT_GRAY);
      tft.fillRect(b,186, 32, 53, LIGHT_GRAY);
      }
  //Draw Horizontal Lines
  for (int h=133; h >=80 ; h -=53 ){
    tft.drawFastHLine(0, h, tft.width(),SILVER);            //dibuja las lineas horizontales entre botones
  }
    
    tft.drawFastHLine(0, 240, tft.width(), SILVER);         // lineas extra para el boton de espacio o SPC
    tft.drawFastHLine(0, 187, tft.width()-32, SILVER); 
      

  //Draw Vertical Lines
  for (int v = 0; v <= tft.width(); v += 32){                   //dibuja las lineas verticales
    tft.drawFastVLine(v, 80, tft.width(), SILVER);
  }


  for (int i = 0; i < 10; i++) {                  //dibuja los caracteres de la linea 1
      tft.setCursor(10 + (32 * i), 100);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.println(symbol[0][i]);
    }
  for (int j = 1; j < 3; j++) {
    for (int i = 0; i < 9; i++) {                 //dibuja los caracteres de la linea 2 y 3
        tft.setCursor(10 + (32 * i), 100 +(53 * j));
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(symbol[j][i]);
      }
  }
 String space[3]={"S","P","C"};
   for (int i = 0; i < 3; i++) {                  //dibuja los caracteres del boton SPC de manera vertical
        tft.setCursor(10 + (32 * 9), 153+(29* i));
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(space[i]);
      }
  
    
}
