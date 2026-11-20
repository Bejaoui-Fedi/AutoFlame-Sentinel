#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialisation avec adresse I2C, colonnes, lignes
LiquidCrystal_I2C lcd(0x27, 20, 4); // Adresse, colonnes, lignes

void setup() {
  Wire.begin(21, 22); // SDA=GPIO21, SCL=GPIO22 pour ESP32

  lcd.init();        // Initialise le LCD (remplace begin() dans certaines versions)
  lcd.backlight();   // Active le rétroéclairage

  lcd.setCursor(0, 0);
  lcd.print("LCD I2C 20x4 Test");
  lcd.setCursor(0, 1);
  lcd.print("Ligne 1 OK");
  lcd.setCursor(0, 2);
  lcd.print("Ligne 2 OK");
  lcd.setCursor(0, 3);
  lcd.print("Ligne 3 OK");
}

void loop() {}
