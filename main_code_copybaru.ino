// FINAL CODE: PROTOTIPE KOLIMATOR RONTGEN THORAX BERBASIS ARDUINO MEGA 2560 + TFT LCD + MOTOR DC
// DENGAN TOUCHSCREEN, VALIDASI POTENSIOMETER, KONFIRMASI, MODULARISASI, INPUT SERIAL, DAN SINKRONISASI LCD & SERIAL
// -----------------------------------------------------------------------------
// Kode ini merupakan implementasi sistem kolimator rontgen thorax berbasis Arduino Mega 2560
// dengan fitur utama: pengendalian motor DC, tampilan TFT LCD, touchscreen, dan validasi proses.
// Setiap bagian kode telah diberikan komentar ilmiah dan humanis untuk memudahkan pemahaman dan pelaporan.
// -----------------------------------------------------------------------------

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>  

#include <Fonts/TomThumb.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// === PIN ===
// Deklarasi pin-pin yang digunakan untuk koneksi ke TFT, touchscreen, motor, dan tombol
#define TFT_CS    41
#define TFT_RST   43
#define TFT_DC    45
#define TOUCH_CS  47
#define TOUCH_IRQ 49
#define resolusiX 480
#define resolusiY 320

#define TFTR_CS   39
#define TFTR_RST  37
#define TFTR_DC   31
#define TOUCHR_CS 33
#define TOUCH_IRQR 35
#define lampu     2

#define XLEAF1A   10
#define XLEAF1B   9
#define XLEAF2A   8
#define XLEAF2B   7
#define YLEAF1A   6
#define YLEAF1B   5
#define YLEAF2A   4
#define YLEAF2B   3

#define XREAD1    A0
#define XREAD2    A1
#define YREAD1    A2
#define YREAD2    A3
#define toleransi 50

// Target nilai potensiometer untuk setiap ukuran film
#define TARGET_18x24_X1  250
#define TARGET_18x24_X2  250
#define TARGET_18x24_Y1  250
#define TARGET_18x24_Y2  250

#define TARGET_24x30_X1  350
#define TARGET_24x30_X2  350
#define TARGET_24x30_Y1  350
#define TARGET_24x30_Y2  350

#define TARGET_30x40_X1  450
#define TARGET_30x40_X2  450
#define TARGET_30x40_Y1  450
#define TARGET_30x40_Y2  450

#define TARGET_35x43_X1  550
#define TARGET_35x43_X2  550
#define TARGET_35x43_Y1  550
#define TARGET_35x43_Y2  550

// Target untuk iluminasi (25x25 cm)
#define TARGET_ILUMINASI_X1  250
#define TARGET_ILUMINASI_X2  250
#define TARGET_ILUMINASI_Y1  250
#define TARGET_ILUMINASI_Y2  250

#define OPEN_X1  900
#define CLOSE_X1  100
#define OPEN_X2  900
#define CLOSE_X2  100
#define OPEN_Y1  900
#define CLOSE_Y1  100
#define OPEN_Y2  900
#define CLOSE_Y2  100

// Push Button Pins (Opsional)
// Pin untuk tombol fisik sebagai alternatif input selain touchscreen
#define BTN_KOLIMASI    22
#define BTN_ILUMINASI   23
#define BTN_TERCAPAI    24
#define BTN_TIDAK       25
#define BTN_KEMBALI     26


Adafruit_ST7796S layar(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ILI9341 remot(TFTR_CS, TFTR_DC, TFTR_RST);
XPT2046_Touchscreen touch(TOUCH_CS);

unsigned long lastMillis = 0;
int mode = 0;
int ukuran = 0;
const char* labelUkuran[] = {"18x24", "24x30", "30x40", "35x43"};
String serialCommand = "";
bool menungguPilihanUkuran = false;
bool sedangKolimasi = false;
bool menungguValidasiIluminasi = false;
bool menungguAksiSetelahKolimasi = false;
bool statusKolimasiBerhasil = false;

// Fungsi untuk mengontrol motor berdasarkan target potensiometer
void kontrolMotorSampaiTarget(int targetX1, int targetX2, int targetY1, int targetY2) {
  Serial.println("=== MENGONTROL MOTOR SAMPAI TARGET ===");
  Serial.print("Target X1: "); Serial.println(targetX1);
  Serial.print("Target X2: "); Serial.println(targetX2);
  Serial.print("Target Y1: "); Serial.println(targetY1);
  Serial.print("Target Y2: "); Serial.println(targetY2);
  
  bool x1Sesuai = false, x2Sesuai = false, y1Sesuai = false, y2Sesuai = false;
  unsigned long startTime = millis();
  const unsigned long timeout = 30000; // 30 detik timeout
  
  while (!(x1Sesuai && x2Sesuai && y1Sesuai && y2Sesuai) && (millis() - startTime < timeout)) {
    int x1 = analogRead(XREAD1);
    int x2 = analogRead(XREAD2);
    int y1 = analogRead(YREAD1);
    int y2 = analogRead(YREAD2);
    
    // Cek apakah sudah sesuai target
    x1Sesuai = abs(x1 - targetX1) <= toleransi;
    x2Sesuai = abs(x2 - targetX2) <= toleransi;
    y1Sesuai = abs(y1 - targetY1) <= toleransi;
    y2Sesuai = abs(y2 - targetY2) <= toleransi;
    
    // Kontrol motor X1
    if (!x1Sesuai) {
      if (x1 < targetX1 - toleransi) {
        digitalWrite(XLEAF1A, HIGH); digitalWrite(XLEAF1B, LOW);
      } else if (x1 > targetX1 + toleransi) {
        digitalWrite(XLEAF1A, LOW); digitalWrite(XLEAF1B, HIGH);
      } else {
        digitalWrite(XLEAF1A, LOW); digitalWrite(XLEAF1B, LOW);
      }
    } else {
      digitalWrite(XLEAF1A, LOW); digitalWrite(XLEAF1B, LOW);
    }
    
    // Kontrol motor X2
    if (!x2Sesuai) {
      if (x2 < targetX2 - toleransi) {
        digitalWrite(XLEAF2A, HIGH); digitalWrite(XLEAF2B, LOW);
      } else if (x2 > targetX2 + toleransi) {
        digitalWrite(XLEAF2A, LOW); digitalWrite(XLEAF2B, HIGH);
      } else {
        digitalWrite(XLEAF2A, LOW); digitalWrite(XLEAF2B, LOW);
      }
    } else {
      digitalWrite(XLEAF2A, LOW); digitalWrite(XLEAF2B, LOW);
    }
    
    // Kontrol motor Y1
    if (!y1Sesuai) {
      if (y1 < targetY1 - toleransi) {
        digitalWrite(YLEAF1A, HIGH); digitalWrite(YLEAF1B, LOW);
      } else if (y1 > targetY1 + toleransi) {
        digitalWrite(YLEAF1A, LOW); digitalWrite(YLEAF1B, HIGH);
      } else {
        digitalWrite(YLEAF1A, LOW); digitalWrite(YLEAF1B, LOW);
      }
    } else {
      digitalWrite(YLEAF1A, LOW); digitalWrite(YLEAF1B, LOW);
    }
    
    // Kontrol motor Y2
    if (!y2Sesuai) {
      if (y2 < targetY2 - toleransi) {
        digitalWrite(YLEAF2A, HIGH); digitalWrite(YLEAF2B, LOW);
      } else if (y2 > targetY2 + toleransi) {
        digitalWrite(YLEAF2A, LOW); digitalWrite(YLEAF2B, HIGH);
      } else {
        digitalWrite(YLEAF2A, LOW); digitalWrite(YLEAF2B, LOW);
      }
    } else {
      digitalWrite(YLEAF2A, LOW); digitalWrite(YLEAF2B, LOW);
    }
    
    // Update tampilan setiap 500ms
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 500) {
      layar.setCursor(50, 100);
      remot.setCursor(20, 60);
      layar.print("X1:"); layar.print(x1); layar.print("("); layar.print(x1Sesuai ? "OK" : "NG"); layar.print(")");
      remot.print("X1:"); remot.print(x1); remot.print("("); remot.print(x1Sesuai ? "OK" : "NG"); remot.print(")");
      
      layar.setCursor(50, 140);
      remot.setCursor(20, 80);
      layar.print("X2:"); layar.print(x2); layar.print("("); layar.print(x2Sesuai ? "OK" : "NG"); layar.print(")");
      remot.print("X2:"); remot.print(x2); remot.print("("); remot.print(x2Sesuai ? "OK" : "NG"); remot.print(")");
      
      layar.setCursor(50, 180);
      remot.setCursor(20, 100);
      layar.print("Y1:"); layar.print(y1); layar.print("("); layar.print(y1Sesuai ? "OK" : "NG"); layar.print(")");
      remot.print("Y1:"); remot.print(y1); remot.print("("); remot.print(y1Sesuai ? "OK" : "NG"); remot.print(")");
      
      layar.setCursor(50, 220);
      remot.setCursor(20, 120);
      layar.print("Y2:"); layar.print(y2); layar.print("("); layar.print(y2Sesuai ? "OK" : "NG"); layar.print(")");
      remot.print("Y2:"); remot.print(y2); remot.print("("); remot.print(y2Sesuai ? "OK" : "NG"); remot.print(")");
      
      lastUpdate = millis();
    }
    
    delay(10); // Delay kecil untuk stabilitas
  }
  
  // Matikan semua motor setelah selesai
  digitalWrite(XLEAF1A, LOW); digitalWrite(XLEAF1B, LOW);
  digitalWrite(XLEAF2A, LOW); digitalWrite(XLEAF2B, LOW);
  digitalWrite(YLEAF1A, LOW); digitalWrite(YLEAF1B, LOW);
  digitalWrite(YLEAF2A, LOW); digitalWrite(YLEAF2B, LOW);
  
  // Baca nilai final
  int finalX1 = analogRead(XREAD1);
  int finalX2 = analogRead(XREAD2);
  int finalY1 = analogRead(YREAD1);
  int finalY2 = analogRead(YREAD2);
  
  bool semuaSesuai = abs(finalX1 - targetX1) <= toleransi && 
                     abs(finalX2 - targetX2) <= toleransi && 
                     abs(finalY1 - targetY1) <= toleransi && 
                     abs(finalY2 - targetY2) <= toleransi;
  
  Serial.println("=== HASIL KONTROL MOTOR ===");
  Serial.print("Final X1: "); Serial.print(finalX1); Serial.print(" (Target: "); Serial.print(targetX1); Serial.println(")");
  Serial.print("Final X2: "); Serial.print(finalX2); Serial.print(" (Target: "); Serial.print(targetX2); Serial.println(")");
  Serial.print("Final Y1: "); Serial.print(finalY1); Serial.print(" (Target: "); Serial.print(targetY1); Serial.println(")");
  Serial.print("Final Y2: "); Serial.print(finalY2); Serial.print(" (Target: "); Serial.print(targetY2); Serial.println(")");
  Serial.print("Status: "); Serial.println(semuaSesuai ? "SESUAI" : "TIDAK SESUAI");
  
  statusKolimasiBerhasil = semuaSesuai;
}

// Fungsi untuk mendapatkan target berdasarkan ukuran film
void dapatkanTargetUkuran(int ukuran, int& targetX1, int& targetX2, int& targetY1, int& targetY2) {
  switch(ukuran) {
    case 0: // 18x24
      targetX1 = TARGET_18x24_X1;
      targetX2 = TARGET_18x24_X2;
      targetY1 = TARGET_18x24_Y1;
      targetY2 = TARGET_18x24_Y2;
      break;
    case 1: // 24x30
      targetX1 = TARGET_24x30_X1;
      targetX2 = TARGET_24x30_X2;
      targetY1 = TARGET_24x30_Y1;
      targetY2 = TARGET_24x30_Y2;
      break;
    case 2: // 30x40
      targetX1 = TARGET_30x40_X1;
      targetX2 = TARGET_30x40_X2;
      targetY1 = TARGET_30x40_Y1;
      targetY2 = TARGET_30x40_Y2;
      break;
    case 3: // 35x43
      targetX1 = TARGET_35x43_X1;
      targetX2 = TARGET_35x43_X2;
      targetY1 = TARGET_35x43_Y1;
      targetY2 = TARGET_35x43_Y2;
      break;
    default:
      targetX1 = TARGET_18x24_X1;
      targetX2 = TARGET_18x24_X2;
      targetY1 = TARGET_18x24_Y1;
      targetY2 = TARGET_18x24_Y2;
      break;
  }
}

void setup() {
  // Fungsi setup() dijalankan sekali saat perangkat dinyalakan
  // Inisialisasi komunikasi serial, layar, touchscreen, dan pin I/O
  Serial.begin(115200);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  layar.init(resolusiY, resolusiX, 0, 0, ST7796S_BGR);
  layar.setRotation(1);
  layar.invertDisplay(true);
  remot.begin();
  touch.begin();
  touch.setRotation(1);

  // Konfigurasi pin tombol dengan mode INPUT_PULLUP untuk mencegah floating
  pinMode(BTN_KOLIMASI, INPUT_PULLUP);
  pinMode(BTN_ILUMINASI, INPUT_PULLUP);
  pinMode(BTN_TERCAPAI, INPUT_PULLUP);
  pinMode(BTN_TIDAK, INPUT_PULLUP);
  pinMode(BTN_KEMBALI, INPUT_PULLUP);

  // Inisialisasi semua pin motor sebagai OUTPUT dan matikan lampu
  for (int i = 3; i <= 10; i++) pinMode(i, OUTPUT);
  pinMode(lampu, OUTPUT);
  digitalWrite(lampu, LOW);

  // Menampilkan layar inisialisasi dan menu utama
  inisialisasi();
  tampilMenu();
}

void loop() {
  // Fungsi utama yang berjalan terus-menerus
  // Berisi logika utama untuk menangani input touchscreen, serial, dan tombol fisik
  if (touch.touched()) {
    // Deteksi sentuhan pada layar touchscreen
    TS_Point p = touch.getPoint();
    int x = map(p.x, 200, 3800, 0, 320);
    int y = map(p.y, 200, 3800, 0, 480);
    Serial.print("x = "); Serial.print(x);
  Serial.print(", y = "); Serial.println(y);

    if (!menungguPilihanUkuran && !sedangKolimasi && !menungguValidasiIluminasi && !menungguAksiSetelahKolimasi) {
      // Menu utama: pilih mode kolimasi atau iluminasi
      if (x > 120 && x < 200 && y > 330 && y < 410) {
        kolimasiMode();
      } else if (x > 150 && x < 280 && y > 100 && y < 200) {
        iluminasiMode();
      }
    } else if (menungguPilihanUkuran && !sedangKolimasi) {
      // Mode pemilihan ukuran film (grid 2x2)
      int boxW = 120, boxH = 80, marginX = 40, marginY = 40, gapX = 40, gapY = 20;
      // Ukuran film 1
if (x > 180 && x < 230 && y >320 && y < 400) {
  ukuran = 0;
  selesaiKolimasi();
}
// Ukuran film 2
else if (x > 70 && x < 100 && y > 320 && y < 400) {
  ukuran = 1;
  selesaiKolimasi();
}
// Ukuran film 3
else if (x > 180 && x < 230 && y > 140 && y < 220) {
  ukuran = 2;
  selesaiKolimasi();
}
// Ukuran film 4
else if (x > 70 && x < 100 && y > 140 && y < 220) {
  ukuran = 3;
  selesaiKolimasi();
}

      // Tombol kembali ke menu utama
      if (x > 120 && x < 200 && y > 60 && y < 140) {
        tampilMenu();
        menungguPilihanUkuran = false;
      }
    } else if (menungguAksiSetelahKolimasi) {
      // Setelah kolimasi: hanya tombol kembali yang aktif
      // Area: (60, 260, 200x50)
      if (x > 160 && x < 200 && y > 60 && y < 100) {
        // Selalu tutup pintu dan matikan lampu sebelum kembali ke pemilihan ukuran
        digitalWrite(lampu, LOW);
        resetPintu();
        kolimasiMode();
        menungguAksiSetelahKolimasi = false;
      }
    } else if (menungguValidasiIluminasi) {
      // Validasi hasil iluminasi: tombol tercapai, tidak, atau kembali
      if (x > 150 && x < 200 && y > 250 && y < 320) {  // Tercapai button area
        validasiIluminasi(true);
      } 
      else if (x > 150 && x < 200 && y >200  && y < 250) {  // Tidak button area
        validasiIluminasi(false);
      } else if (x > 150 && x < 200 && y > 50 && y < 100) { // Kembali button area
        // Tutup pintu dan matikan lampu sebelum kembali ke menu utama
        digitalWrite(lampu, LOW);
        resetPintu();
        tampilMenu();
        menungguValidasiIluminasi = false;
      }
    }
  }

  if (Serial.available()) {
    // Penanganan perintah dari Serial Monitor (untuk debugging atau remote control)
    char c = Serial.read();
    if (c == '\n') {
      serialCommand.trim();
      if (serialCommand.equalsIgnoreCase("kolimasi")) {
        kolimasiMode();
      } else if (serialCommand.equalsIgnoreCase("iluminasi")) {
        iluminasiMode();
      } else if ((serialCommand == "0" || serialCommand == "1" || serialCommand == "2" || serialCommand == "3") && menungguPilihanUkuran && !sedangKolimasi) {
        ukuran = serialCommand.toInt();
        selesaiKolimasi();
      } else if (serialCommand.equalsIgnoreCase("kembali") && menungguPilihanUkuran && !sedangKolimasi) {
        tampilMenu();
        menungguPilihanUkuran = false;
      } else if (menungguAksiSetelahKolimasi) {
        if (serialCommand.equalsIgnoreCase("kembali")) {
          digitalWrite(lampu, LOW);
          resetPintu();
          kolimasiMode();
          menungguAksiSetelahKolimasi = false;
        }
      } else if (menungguValidasiIluminasi) {
        if (serialCommand.equalsIgnoreCase("tercapai")) {
          validasiIluminasi(true);
        } else if (serialCommand.equalsIgnoreCase("tidak")) {
          validasiIluminasi(false);
        } else if (serialCommand.equalsIgnoreCase("kembali")) {
          digitalWrite(lampu, LOW);
          resetPintu();
          tampilMenu();
          menungguValidasiIluminasi = false;
        }
      }
      serialCommand = "";
    } else {
      serialCommand += c;
    }
  }

  // === PUSH BUTTON HANDLING ===
  // Penanganan tombol fisik sebagai alternatif input selain touchscreen
  if (!digitalRead(BTN_KOLIMASI) && !menungguPilihanUkuran && !sedangKolimasi && !menungguValidasiIluminasi && !menungguAksiSetelahKolimasi) {
    delay(200); // debounce sederhana
    kolimasiMode();
  }

  if (!digitalRead(BTN_ILUMINASI) && !menungguPilihanUkuran && !sedangKolimasi && !menungguValidasiIluminasi && !menungguAksiSetelahKolimasi) {
    delay(200);
    iluminasiMode();
  }

  if (!digitalRead(BTN_TERCAPAI) && menungguValidasiIluminasi) {
    delay(200);
    validasiIluminasi(true);
  }

  if (!digitalRead(BTN_TIDAK) && menungguValidasiIluminasi) {
    delay(200);
    validasiIluminasi(false);
  }

  if (!digitalRead(BTN_KEMBALI)) {
    delay(200);
    if (menungguPilihanUkuran && !sedangKolimasi) {
      tampilMenu();
      menungguPilihanUkuran = false;
    }
    else if (menungguAksiSetelahKolimasi) {
      resetPintu();
      kolimasiMode();
      menungguAksiSetelahKolimasi = false;
    }
    else if (menungguValidasiIluminasi) {
      resetPintu();
      tampilMenu();
      menungguValidasiIluminasi = false;
    }
  }
}

void kolimasiMode() {
  // Fungsi untuk menampilkan dan mengatur mode kolimasi (pemilihan ukuran film)
  // Menampilkan grid ukuran film pada layar utama dan remote
  layar.fillScreen(ST77XX_WHITE);
  remot.fillScreen(ST77XX_WHITE);
  layar.setCursor(40, 30);
  remot.setCursor(05, 20);
  layar.setTextColor(ST77XX_BLACK);
  remot.setTextColor(ST77XX_BLACK);
  layar.setTextSize(2);
  remot.setTextSize(0);
  layar.print("PILIH UKURAN FILM:");
  remot.setFont(&FreeSans9pt7b);
  remot.setCursor(5, 15);  // Posisi bisa disesuaikan karena font lebih kecil
  remot.setTextColor(ST77XX_BLACK);
  remot.print("\n");
  remot.print("PILIH UKURAN FILM:");
  remot.print("\n=====================\n");

  Serial.println("\n=== MODE KOLIMASI ===");
  Serial.println("Pilih Ukuran Film:");

  // Grid 2x2
  // Warna kalem (dark theme)
    const uint16_t DARK_YELLOW = 0x9A60;  // Kuning gelap (agak olive)
    const uint16_t DARK_GREEN  = 0x03E0;  // Hijau gelap
    const uint16_t MAROON      = 0x8000;  // Merah maroon
    const uint16_t NAVY        = 0x000F;  // Biru navy gelap


  const uint16_t colors[4] = {DARK_YELLOW, DARK_GREEN, MAROON, NAVY};
  int boxW = 200, boxH = 90, marginX = 45, marginY = 40, gapX = 20, gapY = 20;
  int rboxW = 100, rboxH = 40, rmarginX = 10, rmarginY = 70, rgapX = 15, rgapY = 10;
  for (int i = 0; i < 4; i++) {
    int row = i / 2;
    int col = i % 2;
    int x = marginX + col * (boxW + gapX);
    int y = marginY + row * (boxH + gapY);
    int rx = rmarginX + col * (rboxW + rgapX);
    int ry = rmarginY + row * (rboxH + rgapY);
    layar.fillRoundRect(x, y, boxW, boxH, 10, colors[i]);
    layar.drawRoundRect(x, y, boxW, boxH, 10, ST77XX_BLACK);
    remot.fillRoundRect(rx, ry, rboxW, rboxH, 5, colors[i]);
    remot.drawRoundRect(rx, ry, rboxW, rboxH, 5, ST77XX_BLACK);
    layar.setCursor(x + (boxW/2) - (strlen(labelUkuran[i])*6) - 20, y + boxH/2 + 8);
    remot.setCursor(rx + (rboxW/2) - (strlen(labelUkuran[i])*3) - 10, ry + rboxH/2 + 4);
    layar.setTextColor(ST77XX_WHITE);
    remot.setTextColor(ST77XX_WHITE);
    layar.setTextSize(2);
    remot.setTextSize(1);
    layar.print(labelUkuran[i]);
    remot.print(labelUkuran[i]);
    Serial.print(i); Serial.print(". "); Serial.println(labelUkuran[i]);
  }
  // Tombol Kembali di bawah
  // Tombol digeser ke kanan dan dipertinggi
  layar.fillRoundRect(150, 250, 240, 60, 10, ST77XX_BLACK);   // X=80 (geser kanan), height=70
  layar.drawRoundRect(150, 250, 240, 60, 10, ST77XX_BLACK);

  // Sesuaikan posisi teks agar tetap di tengah tombol
  layar.setCursor(180, 290); // Y disesuaikan dari 285 ke 290 agar tetap tengah secara vertikal
  layar.setTextColor(ST77XX_WHITE);
  layar.print("KEMBALI");

 int geserX = 40;
  int geserY = 30;
  remot.fillRoundRect(10 + geserX, 160 + geserY, 120, 30, 5, ST77XX_BLACK);
  remot.drawRoundRect(10 + geserX, 160 + geserY, 120, 30, 5, ST77XX_BLACK);
  remot.setCursor(30 + geserX, 180 + geserY);
  remot.print("KEMBALI");



  Serial.println("---------------------");
  Serial.println("Ketik 0-3 di Serial Monitor");
  Serial.println("atau sentuh pilihan di LCD");
  Serial.println("Ketik 'kembali' untuk kembali ke menu utama");
  Serial.println("---------------------");

  menungguPilihanUkuran = true;
  sedangKolimasi = false;
  menungguAksiSetelahKolimasi = false;
}

void resetPintu() {
  // Fungsi untuk menutup semua pintu kolimator secara otomatis
  // Kontrol motor sampai posisi tertutup tercapai
  Serial.println("Menutup pintu kolimator...");
  kontrolMotorSampaiTarget(CLOSE_X1, CLOSE_X2, CLOSE_Y1, CLOSE_Y2);
}

void selesaiKolimasi() {
  // Fungsi yang dijalankan setelah ukuran film dipilih
  // Membuka pintu kolimator, membaca sensor posisi, dan menampilkan status
  layar.fillScreen(0xFFFF);
  remot.fillScreen(0xFFFF);
  layar.setCursor(50, 30);
  remot.setCursor(20, 20);
  layar.setTextColor(0x0000);
  remot.setTextColor(0x0000);
  layar.setTextSize(2);
  remot.setTextSize(1);
  layar.print("Ukuran: "); layar.print(labelUkuran[ukuran]);
  remot.print("Ukuran: "); remot.print(labelUkuran[ukuran]);
  Serial.print("Ukuran yang dipilih: "); Serial.println(labelUkuran[ukuran]);
  // Dapatkan target berdasarkan ukuran film
  int targetX1, targetX2, targetY1, targetY2;
  dapatkanTargetUkuran(ukuran, targetX1, targetX2, targetY1, targetY2);
  
  layar.setCursor(50, 70);
  remot.setCursor(20, 40);
  layar.print("Mengontrol motor...");
  remot.print("Mengontrol motor...");
  Serial.println("Mengontrol motor sampai target tercapai...");

  // Kontrol motor sampai target tercapai
  kontrolMotorSampaiTarget(targetX1, targetX2, targetY1, targetY2);

  layar.setCursor(50, 250);
  remot.setCursor(20, 140);
  layar.print("Status: "); layar.print(statusKolimasiBerhasil ? "SESUAI" : "TIDAK SESUAI");
  remot.print("Status: "); remot.print(statusKolimasiBerhasil ? "SESUAI" : "TIDAK SESUAI");
  Serial.print("Status: "); Serial.println(statusKolimasiBerhasil ? "SESUAI" : "TIDAK SESUAI");

  // Hanya tampilkan tombol KEMBALI


  int btnY = 260;
  int btnW = 250; // Lebar tombol diperbesar dari 200 ke 250
  int btnH = 50;  // Tinggi tombol diperbesar dari 50 ke 60
  int btnX = 130; // Geser tombol ke kanan (dari 60 ke 100)

  // Gambar tombol pada layar
  layar.fillRoundRect(btnX, btnY, btnW, btnH, 10, ST77XX_BLUE);
  layar.drawRoundRect(btnX, btnY, btnW, btnH, 10, ST77XX_BLACK);

  // Atur posisi teks di tengah tombol
  layar.setCursor(btnX + 40, btnY + 35); // Sesuaikan agar teks berada di tengah secara visual
  layar.setTextColor(ST77XX_WHITE);
  layar.print("KEMBALI");

  // Tombol di remot juga bisa disesuaikan (jika ingin disamakan):
  remot.fillRoundRect(50, 180, 140, 40, 5, ST77XX_BLUE); // Geser ke kanan dan perbesar
  remot.drawRoundRect(50, 180, 140, 40, 5, ST77XX_BLACK);
  remot.setCursor(80, 200);
  remot.setTextColor(ST77XX_WHITE);
  remot.print("KEMBALI");


  Serial.println("---------------------");
  Serial.println("Ketik 'kembali' untuk kembali ke pilihan ukuran kolimasi");
  Serial.println("---------------------");

  menungguAksiSetelahKolimasi = true;
  menungguPilihanUkuran = false;
  sedangKolimasi = false;
  menungguValidasiIluminasi = false;
}

void validasiIluminasi(bool tercapai) {
  // Fungsi untuk memvalidasi hasil uji iluminasi berdasarkan input user
  // Jika tercapai, sistem kembali ke menu utama; jika tidak, pintu dibuka ulang
  menungguValidasiIluminasi = false;
  if (tercapai) {
    // Jika tercapai, langsung kembali ke menu utama
    digitalWrite(lampu, LOW);
    resetPintu();
    tampilMenu();
  } else {
    // Jika tidak tercapai, tutup pintu dan matikan lampu
    digitalWrite(lampu, LOW);
    resetPintu();
    // Buka pintu dan nyalakan lampu kembali
    digitalWrite(lampu, HIGH);
    kontrolMotorSampaiTarget(TARGET_ILUMINASI_X1, TARGET_ILUMINASI_X2, TARGET_ILUMINASI_Y1, TARGET_ILUMINASI_Y2);
    // Tetap di halaman uji iluminasi, user bisa validasi ulang
    menungguValidasiIluminasi = true;
  }
}

void iluminasiMode() {
  // Fungsi untuk menampilkan mode uji iluminasi pada layar utama dan remote
  // Menyalakan lampu, membuka pintu, dan menunggu validasi user
  // --- LAYAR UTAMA (TFT LCD) ---
  int offsetX = 20; // geser ke kanan
  int offsetY = 20; // geser ke atas
  layar.fillScreen(ST77XX_WHITE);
  layar.setTextColor(ST77XX_BLACK);

  // Judul
  layar.setFont(&FreeSansBold12pt7b);
  layar.setTextSize(1);
  int16_t x1, y1; uint16_t w1, h1;
  layar.getTextBounds("MODE ILUMINASI", 0, 0, &x1, &y1, &w1, &h1);
  layar.setCursor((resolusiX - w1) / 2 + offsetX, 60 - offsetY);
  layar.print("MODE ILUMINASI");

  // Status lampu
  layar.setFont(&FreeSansBold9pt7b);
  layar.setTextSize(1);
  int16_t x2, y2; uint16_t w2, h2;
  layar.getTextBounds("Lampu: ON (100 lux)", 0, 0, &x2, &y2, &w2, &h2);
  layar.setCursor((resolusiX - w2) / 2 + offsetX, 110 - offsetY);
  layar.print("Lampu: ON (100 lux)");

  // Pertanyaan
  int16_t x3, y3; uint16_t w3, h3;
  layar.getTextBounds("Cahaya tercapai?", 0, 0, &x3, &y3, &w3, &h3);
  layar.setCursor((resolusiX - w3) / 2 + offsetX, 150 - offsetY);
  layar.print("Cahaya tercapai?");

  // Tombol di layar utama: tinggi diperkecil
  int btnW = 240, btnH = 38;
  int btnX = (resolusiX - btnW) / 2 + offsetX;
  int btnY1 = 180 - offsetY;
  int btnY2 = btnY1 + btnH + 12;
  int btnY3 = btnY2 + btnH + 12;

  // Tombol TERCAPAI
  layar.fillRoundRect(btnX, btnY1, btnW, btnH, 10, ST77XX_GREEN);
  layar.drawRoundRect(btnX, btnY1, btnW, btnH, 10, ST77XX_BLACK);
  layar.setFont();
  layar.setTextSize(2);
  const char* txtTercapai = "TERCAPAI";
  int16_t tx1, ty1; uint16_t tw1, th1;
  layar.getTextBounds(txtTercapai, 0, 0, &tx1, &ty1, &tw1, &th1);
  layar.setCursor(btnX + (btnW - tw1) / 2, btnY1 + (btnH + th1) / 2 - 15);
  layar.setTextColor(ST77XX_WHITE);
  layar.print(txtTercapai);

  // Tombol TIDAK
  layar.fillRoundRect(btnX, btnY2, btnW, btnH, 10, ST77XX_RED);
  layar.drawRoundRect(btnX, btnY2, btnW, btnH, 10, ST77XX_BLACK);
  const char* txtTidak = "TIDAK";
  int16_t tx2, ty2; uint16_t tw2, th2;
  layar.getTextBounds(txtTidak, 0, 0, &tx2, &ty2, &tw2, &th2);
  layar.setCursor(btnX + (btnW - tw2) / 2, btnY2 + (btnH + th2) / 2 - 15);
  layar.setTextColor(ST77XX_WHITE);
  layar.print(txtTidak);

  // Tombol KEMBALI
  layar.fillRoundRect(btnX, btnY3, btnW, btnH, 10, ST77XX_BLUE);
  layar.drawRoundRect(btnX, btnY3, btnW, btnH, 10, ST77XX_BLACK);
  const char* txtKembali = "KEMBALI";
  int16_t tx3, ty3; uint16_t tw3, th3;
  layar.getTextBounds(txtKembali, 0, 0, &tx3, &ty3, &tw3, &th3);
  layar.setCursor(btnX + (btnW - tw3) / 2, btnY3 + (btnH + th3) / 2 - 15);
  layar.setTextColor(ST77XX_WHITE);
  layar.print(txtKembali);

  // --- REMOTE ---
  remot.fillScreen(ST77XX_WHITE);
  // Judul, status, pertanyaan di tengah layar remote
  int rWidth = 240; // asumsikan lebar remote
  int rHeight = 320; // asumsikan tinggi remote
  int rBtnW = 160, rBtnH = 50;
  int rBtnX = (rWidth - rBtnW) / 2;
  int rBtnY1 = 120;
  int rBtnY2 = rBtnY1 + rBtnH + 12;
  int rBtnY3 = rBtnY2 + rBtnH + 12;

  remot.setTextColor(ST77XX_BLACK);
  remot.setTextSize(1);
  remot.setFont(&FreeSansBold12pt7b);
  int16_t rx, ry; uint16_t rw, rh;
  remot.getTextBounds("MODE ILUMINASI", 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor((rWidth - rw) / 2, 30 + rh/2);
  remot.print("MODE ILUMINASI");
  remot.setFont(&FreeSansBold9pt7b);
  remot.getTextBounds("Lampu: ON (100 lux)", 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor((rWidth - rw) / 2, 55 + rh/2);
  remot.print("Lampu: ON (100 lux)");
  remot.getTextBounds("Cahaya tercapai?", 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor((rWidth - rw) / 2, 80 + rh/2);
  remot.print("Cahaya tercapai?");
  remot.setFont();
  remot.setTextSize(2);

  // Tombol TERCAPAI
  remot.fillRoundRect(rBtnX, rBtnY1, rBtnW, rBtnH, 10, ST77XX_GREEN);
  remot.drawRoundRect(rBtnX, rBtnY1, rBtnW, rBtnH, 10, ST77XX_BLACK);
  remot.getTextBounds(txtTercapai, 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor(rBtnX + (rBtnW - rw) / 2, rBtnY1 + (rBtnH + rh) / 2 - 15);
  remot.setTextColor(ST77XX_WHITE);
  remot.print(txtTercapai);

  // Tombol TIDAK
  remot.fillRoundRect(rBtnX, rBtnY2, rBtnW, rBtnH, 10, ST77XX_RED);
  remot.drawRoundRect(rBtnX, rBtnY2, rBtnW, rBtnH, 10, ST77XX_BLACK);
  remot.getTextBounds(txtTidak, 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor(rBtnX + (rBtnW - rw) / 2, rBtnY2 + (rBtnH + rh) / 2 - 15);
  remot.setTextColor(ST77XX_WHITE);
  remot.print(txtTidak);

  // Tombol KEMBALI
  remot.fillRoundRect(rBtnX, rBtnY3, rBtnW, rBtnH, 10, ST77XX_BLUE);
  remot.drawRoundRect(rBtnX, rBtnY3, rBtnW, rBtnH, 10, ST77XX_BLACK);
  remot.getTextBounds(txtKembali, 0, 0, &rx, &ry, &rw, &rh);
  remot.setCursor(rBtnX + (rBtnW - rw) / 2, rBtnY3 + (rBtnH + rh) / 2 - 15);
  remot.setTextColor(ST77XX_WHITE);
  remot.print(txtKembali);

  // --- LOGIKA ---
  digitalWrite(lampu, HIGH);
  
  // Kontrol motor untuk ukuran iluminasi (25x25 cm)
  Serial.println("Mengontrol motor untuk iluminasi (25x25 cm)...");
  kontrolMotorSampaiTarget(TARGET_ILUMINASI_X1, TARGET_ILUMINASI_X2, TARGET_ILUMINASI_Y1, TARGET_ILUMINASI_Y2);

  Serial.println("\n=== MODE ILUMINASI ===");
  Serial.println("Lampu ON (100 lux)");
  Serial.println("Status Cahaya:");
  Serial.println("1. Ketik 'tercapai' jika cahaya sesuai");
  Serial.println("2. Ketik 'tidak' jika cahaya tidak sesuai");
  Serial.println("3. Ketik 'kembali' untuk kembali ke menu utama");
  Serial.println("---------------------");
  menungguValidasiIluminasi = true;
}

void inisialisasi() {
  // Fungsi untuk menampilkan layar pembuka (judul proyek, identitas, dosen pembimbing)
  layar.fillScreen(0xFFFFFF);
  remot.fillScreen(0xFFFFFF);
  layar.setFont(&FreeSansBold12pt7b);
  remot.setFont(&FreeSansBold12pt7b);
  layar.setTextColor(0x000000);
  remot.setTextColor(0x000000);
  layar.setTextSize(1);
  remot.setTextSize(1);

  // ===== LAYAR 1: Judul Proyek =====
  layar.setCursor(30, 130); layar.print("PROTOTIPE KOLIMATOR RONTGEN");
  remot.setCursor(50, 70); remot.print("PROTOTIPE");
  remot.setCursor(47, 100); remot.print("KOLIMATOR");
  remot.setCursor(60, 130); remot.print("RONTGEN");
  layar.setCursor(15, 160); layar.print("THORAX MENGGUNAKAN MOTOR DC");
  remot.setCursor(65, 160); remot.print("THORAX");
  remot.setCursor(20, 190); remot.print("MENGGUNAKAN");
  remot.setCursor(50, 220); remot.print("MOTOR DC");
  layar.setCursor(145, 190); layar.print("BERBASIS MEGA2560");
  remot.setCursor(18, 250); remot.print("BERBASIS MEGA2560");
  delay(1500);

  // ===== LAYAR 2: Identitas Mahasiswa =====
  layar.fillScreen(0xFFFFFF);
  remot.fillScreen(0xFFFFFF);
  layar.setCursor(80, 140); layar.print("MUHAMMAD ARIEF ALHADI");
  remot.setCursor(45, 80); remot.print("MUHAMMAD");
  remot.setCursor(35, 110); remot.print("ARIEF ALHADI");
  layar.setCursor(130, 200); layar.print("P2.20.301.21.032");
  remot.setCursor(25, 190); remot.print("P2.20.301.21.032");
  delay(1500);

  // ===== LAYAR 3: Dosen Pembimbing I =====
  layar.fillScreen(0xFFFFFF);
  remot.fillScreen(0xFFFFFF);
  layar.setCursor(80, 130); layar.print("VITA NURDINAWATI, ST, MT");
  remot.setCursor(95, 80); remot.print("VITA");
  remot.setCursor(35, 110); remot.print("NURDINAWATI,");
  remot.setCursor(85, 140); remot.print("ST, MT");
  layar.setCursor(85, 200); layar.print("NIP. 198309032018012001");
  remot.setCursor(100, 200); remot.print("NIP.");
  remot.setCursor(0, 230); remot.print("198309032018012001");
  delay(1500);

  // ===== LAYAR 4: Dosen Pembimbing II =====
  layar.fillScreen(0xFFFFFF);
  remot.fillScreen(0xFFFFFF);
  layar.setCursor(90, 130); layar.print("WINDA WIRASA, ST, MT");
  remot.setCursor(85, 80); remot.print("WINDA");
  remot.setCursor(75, 110); remot.print("WIRASA,");
  remot.setCursor(90, 140); remot.print("ST, MT");
  layar.setCursor(85, 200); layar.print("NIP. 196603271990031004");
  remot.setCursor(100, 200); remot.print("NIP.");
  remot.setCursor(0, 230); remot.print("196603271990031004");
  delay(1500);
}

void tampilMenu() {
  // Fungsi untuk menampilkan menu utama pada layar utama dan remote
  layar.fillScreen(0xFFFFFF);
  remot.fillScreen(0xFFFFFF);
  layar.setFont();
  remot.setFont();
  layar.setTextColor(0x000000);
  remot.setTextColor(0x000000);
  layar.setTextSize(2);
  remot.setTextSize(2);

  layar.drawRect(90, 35, 300, 100, 0x000000);
  remot.drawRect(20, 45, 200, 80, 0x000000);
  layar.setFont(&FreeSansBold9pt7b);
  remot.setFont(&FreeSansBold12pt7b);
  layar.setCursor(160, 100);
  remot.setCursor(60, 95);
   layar.setTextSize(2);
  remot.setTextSize(1);
  layar.print("KOLIMASI");
  remot.print("KOLIMASI");

  layar.setFont(&FreeSansBold9pt7b);
  remot.setFont(&FreeSansBold12pt7b);
  layar.drawRect(90, 200, 300, 100, 0x000000);
  remot.drawRect(20, 200, 200, 80, 0x000000);
  layar.setCursor(215, 240);
  remot.setCursor(100, 230);
  layar.setTextSize(2);
  remot.setTextSize(1);
  layar.print("UJI");
  remot.print("UJI");
  layar.setCursor(145, 280);
  remot.setCursor(55, 260);
  layar.print("ILUMINASI");
  remot.print("ILUMINASI");
  menungguPilihanUkuran = false;
  sedangKolimasi = false;
  menungguAksiSetelahKolimasi = false;
  menungguValidasiIluminasi = false;
}