#include <16F877A.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=4000000)

#define DIN    PIN_B0
#define LOAD   PIN_B1
#define CLK    PIN_B2
#define BUZZER PIN_B3

#define BUTTON_UP     PIN_D2
#define BUTTON_DOWN   PIN_D4
#define BUTTON_LEFT   PIN_D1
#define BUTTON_RIGHT  PIN_D3
#define BUTTON_SELECT PIN_D0


#include <MAX7219.c>


void buzz() {
    output_high(BUZZER); // Buzzer'ý aç
    delay_ms(250);
    output_low(BUZZER); // Buzzer'ý kapat
}


void send_data(unsigned char address, unsigned char data) {
   unsigned char i;
   output_low(LOAD); // Veri gönderimini baþlat
   for(i = 0; i < 8; i++) { // Adres gönderimi
      output_bit(DIN, (address >> (7 - i)) & 0x01);
      output_high(CLK); // Saat sinyali
      output_low(CLK);
   }
   for(i = 0; i < 8; i++) { // Veri gönderimi
      output_bit(DIN, (data >> (7 - i)) & 0x01);
      output_high(CLK); // Saat sinyali
      output_low(CLK);
   }
   output_high(LOAD); // Veri gönderimini durdur
}

void settingsPorts()
{
   display_init(0);
   set_tris_b(0x00); // B portunu çýkýþ olarak ayarla
   output_b(0x00); // B portunu temizle

   set_tris_d(0xFF); // D portunu giriþ olarak ayarla
   output_d(0x00); // D portunu temizle

   send_data(0x0F, 0x00); // Test modunu kapat
   send_data(0x0C, 0x01); // Normal çalýþma

   send_data(0x0B, 0x07); // Tarama limiti = 8 haneli
   send_data(0x0A, 0x0F); // Yoðunluk ayarý
   send_data(0x09, 0x00); // Çözümleme modu yok
}

unsigned char led_matrix[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void turn_off_all_leds() {
    for (unsigned char row = 1; row <= 8; row++) {
        led_matrix[row - 1] = 0x00; // Tüm LED'leri kapatmak için veri
        send_data(row, led_matrix[row - 1]);
    }
}



void turn_on_led(unsigned char row, unsigned char column) {
    led_matrix[row - 1] |= 1 << (column - 1); // Belirli LED'i açmak için
    send_data(row, led_matrix[row - 1]); // Belirtilen satýr ve sütun deðerine göre belirli bir LED'i yak
}


void turn_off_led(unsigned char row, unsigned char column) {
    led_matrix[row - 1] &= ~(1 << (column - 1)); // Belirli LED'i kapatmak için
    send_data(row, led_matrix[row - 1]); // Belirtilen satýr ve sütun deðerine göre belirli bir LED'i söndür
}

void turn_led(unsigned char row, unsigned char column, unsigned char led_state) {
    if (led_state != 0) {
        led_matrix[row - 1] |= 1 << (column - 1); // Belirli LED'i açmak için
    } else {
        led_matrix[row - 1] &= ~(1 << (column - 1)); // Belirli LED'i kapatmak için
    }
    send_data(row, led_matrix[row - 1]); // Belirtilen satýr ve sütun deðerine göre belirli bir LED'i yak veya söndür
}


/*
void turn_led(unsigned char row, unsigned char column, unsigned char led_state) {
    unsigned char LED_ON = (1 << (column - 1));
    unsigned char LED_OFF = 0x00;
  
    if (led_state != 0) {
        send_data(row, LED_ON); // Belirtilen satýr ve sütun deðerine göre belirli bir LED'i yak
    } else {
        send_data(row, LED_OFF); // Belirtilen satýr ve sütun deðerindeki LED'i söndür
    }
}

*/

unsigned char level = 1;
unsigned char randLEDs[16][2];
unsigned char selected_leds[16][2];
unsigned char selected_led_count = 0;
unsigned char isItPLAYERsTurn = 0;
unsigned char row = 1;
unsigned char column = 1;

void generateRandomLEDs() {   // follows lollipop

    srand(level);

    for (int j = 0; j < level; j++) {
        unsigned char a, b;
        int positionAlreadySelected;

        do {
            positionAlreadySelected = 0;
            a = 1 + rand() % 8;
            b = 1 + rand() % 8;

            // Seçilen konumu daha önce seçilmiþ konumlarla kontrol et
            for (int k = 0; k < j; k++) {
                if (randLEDs[k][0] == a && randLEDs[k][1] == b) {
                    positionAlreadySelected = 1;
                    break;
                }
            }
        } while (positionAlreadySelected);

        randLEDs[j][0] = a;
        randLEDs[j][1] = b;

    }

    for (int i = 0; i < level; i++) {
        delay_ms(75);
        turn_on_led(randLEDs[i][0], randLEDs[i][1]);
        delay_ms(125);
        turn_off_led(randLEDs[i][0], randLEDs[i][1]);

    }
    
    isItPLAYERsTurn = 1;
}



int check_selected_leds() {
    for (unsigned char i = 0; i < level; i++) {
        if (selected_leds[i][0] != randLEDs[i][0] || selected_leds[i][1] != randLEDs[i][1]) {
            return 0;
        }
    }
    return 1;
}




// LED'in seçilip seçilmediðini kontrol eden fonksiyon ve mr. beast çok iyi
int is_selected(unsigned char row, unsigned char column) {
   for (unsigned char i = 0; i < selected_led_count; i++) {
      if (selected_leds[i][0] == row && selected_leds[i][1] == column) {
         return 1;
      }
   }
   return 0;
}

void playerMovement() {
      // Sað (RIGHT) butonuna basýldýðýnda
      if (input(BUTTON_RIGHT)) {
         
         if (!is_selected(row, column)) {
            turn_off_led(row,column);
         }
         column++;
         if (column > 8) { 
            column = 1;
         }
         turn_on_led(row, column);
         delay_ms(75);
      }
      
      // Sol (LEFT) butonuna basýldýðýnda
      if (input(BUTTON_LEFT)) {
      
         if (!is_selected(row, column)) {
            turn_off_led(row, column);
         }
         column--;
         if (column < 1) { 
            column = 8;
         }
         turn_on_led(row, column);
         delay_ms(75);
      }
      
      // Yukarý (UP) butonuna basýldýðýnda
      if (input(BUTTON_UP)) {
         
         if (!is_selected(row, column)) {
            turn_off_led(row, column);
         }
         row--;
         if (row < 1) { 
            row = 8;
         }
         turn_on_led(row, column);
         delay_ms(75);
      }
      
      // Aþaðý (DOWN) butonuna basýldýðýnda
      if (input(BUTTON_DOWN)) {
         
         if (!is_selected(row, column)) {
            turn_off_led(row, column);
         }
         row++;
         if (row > 8) { 
            row = 1;
         }
         turn_on_led(row, column);
         delay_ms(75);
      }
      
      if(input(BUTTON_SELECT))
      {
      
         //led_matrix[row - 1] |= 1 << (column - 1);
         //send_data(row, led_matrix[row - 1]);
         turn_on_led(row,column);
         
         // Konumu diziye kaydet
         selected_leds[selected_led_count][0] = row;
         selected_leds[selected_led_count][1] = column;
         selected_led_count++;
         delay_ms(75);
         if (selected_led_count == level) 
         {
            if(!check_selected_leds())
            {
               isItPLAYERsTurn = 0;
               buzz();
            }
            else if (check_selected_leds()) 
            {
               level++;
               isItPLAYERsTurn = 0;
            }
            
            delay_ms(75);
         }
      }
}


void main() {

   settingsPorts();

   while(1){
   
      selected_led_count = 0;
      
      turn_off_all_leds();
      row = 1;
      column = 1;

      generateRandomLEDs();
      turn_on_led(row,column);

   
      while (isItPLAYERsTurn) {
         playerMovement();
      }
   }
}


