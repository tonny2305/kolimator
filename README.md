# PANDUAN MENGATUR SISTEM KONTROL MOTOR KOLIMATOR RONTGEN
## Step by Step Guide

---

## 📋 **DAFTAR ISI**
1. [Persiapan Hardware](#persiapan-hardware)
2. [Kalibrasi Potensiometer](#kalibrasi-potensiometer)
3. [Menentukan Target Nilai](#menentukan-target-nilai)
4. [Mengatur Kode Arduino](#mengatur-kode-arduino)
5. [Testing dan Validasi](#testing-dan-validasi)
6. [Troubleshooting](#troubleshooting)
7. [Optimasi Lanjutan](#optimasi-lanjutan)

---

## 🔧 **PERSIAPAN HARDWARE**

### Hardware yang Diperlukan:
- ✅ Arduino Mega 2560
- ✅ TFT LCD dan Remote LCD
- ✅ 4 Motor DC dengan driver
- ✅ 4 Potensiometer (XREAD1, XREAD2, YREAD1, YREAD2)
- ✅ Power supply yang stabil
- ✅ Kabel jumper

### Koneksi Hardware:
```
Motor X1: Pin 10 (XLEAF1A), Pin 9 (XLEAF1B)
Motor X2: Pin 8 (XLEAF2A), Pin 7 (XLEAF2B)
Motor Y1: Pin 6 (YLEAF1A), Pin 5 (YLEAF1B)
Motor Y2: Pin 4 (YLEAF2A), Pin 3 (YLEAF2B)

Potensiometer:
XREAD1: Pin A0
XREAD2: Pin A1
YREAD1: Pin A2
YREAD2: Pin A3
```

---

## 📊 **KALIBRASI POTENSIOMETER**

### Langkah 1: Upload Kode Dasar
1. Upload kode `main_code_xxxxxxxx.ino` ke Arduino
2. Buka Serial Monitor (115200 baud)
3. Pastikan hardware terhubung dengan benar

### Langkah 2: Baca Nilai Potensiometer
1. Pilih mode **"KOLIMASI"** di LCD
2. Pilih ukuran film apapun (misal: 18x24)
3. Perhatikan nilai yang muncul di Serial Monitor:
   ```
   === MENGONTROL MOTOR SAMPAI TARGET ===
   Target X1: 250
   Target X2: 250
   Target Y1: 250
   Target Y2: 250
   ```

### Langkah 3: Catat Nilai Min-Max
1. **Putar potensiometer manual** ke posisi paling kiri (pintu tertutup)
2. **Catat nilai minimum** yang muncul
3. **Putar potensiometer manual** ke posisi paling kanan (pintu terbuka)
4. **Catat nilai maksimum** yang muncul

### Contoh Hasil Kalibrasi:
```
Potensiometer X1:
- Minimum (tertutup): 50
- Maksimum (terbuka): 950
- Range: 50-950

Potensiometer X2:
- Minimum (tertutup): 45
- Maksimum (terbuka): 945
- Range: 45-945

Potensiometer Y1:
- Minimum (tertutup): 55
- Maksimum (terbuka): 955
- Range: 55-955

Potensiometer Y2:
- Minimum (tertutup): 40
- Maksimum (terbuka): 940
- Range: 40-940
```

---

## 🎯 **MENENTUKAN TARGET NILAI**

### Langkah 1: Tentukan Posisi Referensi
Berdasarkan kalibrasi, tentukan:
- **Posisi tertutup** = nilai minimum (misal: 50)
- **Posisi terbuka** = nilai maksimum (misal: 950)
- **Posisi tengah** = (min + max) / 2 (misal: 500)

### Langkah 2: Tentukan Ukuran Film
Berdasarkan kebutuhan, tentukan:
- **18x24 cm** = posisi kecil (misal: 200)
- **24x30 cm** = posisi sedang (misal: 350)
- **30x40 cm** = posisi besar (misal: 500)
- **35x43 cm** = posisi terbesar (misal: 650)

### Langkah 3: Tentukan Target Iluminasi
- **25x25 cm** = posisi standar untuk iluminasi (misal: 250)

### Contoh Target Berdasarkan Kalibrasi:
```cpp
// Posisi tertutup
#define CLOSE_X1  50
#define CLOSE_X2  45
#define CLOSE_Y1  55
#define CLOSE_Y2  40

// Target ukuran film
#define TARGET_18x24_X1  200
#define TARGET_18x24_X2  200
#define TARGET_18x24_Y1  200
#define TARGET_18x24_Y2  200

#define TARGET_24x30_X1  350
#define TARGET_24x30_X2  350
#define TARGET_24x30_Y1  350
#define TARGET_24x30_Y2  350

#define TARGET_30x40_X1  500
#define TARGET_30x40_X2  500
#define TARGET_30x40_Y1  500
#define TARGET_30x40_Y2  500

#define TARGET_35x43_X1  650
#define TARGET_35x43_X2  650
#define TARGET_35x43_Y1  650
#define TARGET_35x43_Y2  650

// Target iluminasi
#define TARGET_ILUMINASI_X1  250
#define TARGET_ILUMINASI_X2  250
#define TARGET_ILUMINASI_Y1  250
#define TARGET_ILUMINASI_Y2  250
```

---

## ⚙️ **MENGATUR KODE ARDUINO**

### Langkah 1: Buka File Kode
1. Buka file `main_code_xxxxxx.ino` di Arduino IDE
2. Cari bagian **"Target nilai potensiometer"** (sekitar baris 54-75)

### Langkah 2: Atur Target Nilai
Ganti nilai target sesuai kalibrasi Anda:

```cpp
// Ganti nilai ini sesuai kalibrasi Anda
#define TARGET_18x24_X1  200  // ← Sesuaikan dengan sistem Anda
#define TARGET_18x24_X2  200  // ← Sesuaikan dengan sistem Anda
#define TARGET_18x24_Y1  200  // ← Sesuaikan dengan sistem Anda
#define TARGET_18x24_Y2  200  // ← Sesuaikan dengan sistem Anda

#define TARGET_24x30_X1  350  // ← Sesuaikan dengan sistem Anda
#define TARGET_24x30_X2  350  // ← Sesuaikan dengan sistem Anda
#define TARGET_24x30_Y1  350  // ← Sesuaikan dengan sistem Anda
#define TARGET_24x30_Y2  350  // ← Sesuaikan dengan sistem Anda

#define TARGET_30x40_X1  500  // ← Sesuaikan dengan sistem Anda
#define TARGET_30x40_X2  500  // ← Sesuaikan dengan sistem Anda
#define TARGET_30x40_Y1  500  // ← Sesuaikan dengan sistem Anda
#define TARGET_30x40_Y2  500  // ← Sesuaikan dengan sistem Anda

#define TARGET_35x43_X1  650  // ← Sesuaikan dengan sistem Anda
#define TARGET_35x43_X2  650  // ← Sesuaikan dengan sistem Anda
#define TARGET_35x43_Y1  650  // ← Sesuaikan dengan sistem Anda
#define TARGET_35x43_Y2  650  // ← Sesuaikan dengan sistem Anda

#define TARGET_ILUMINASI_X1  250  // ← Sesuaikan dengan sistem Anda
#define TARGET_ILUMINASI_X2  250  // ← Sesuaikan dengan sistem Anda
#define TARGET_ILUMINASI_Y1  250  // ← Sesuaikan dengan sistem Anda
#define TARGET_ILUMINASI_Y2  250  // ← Sesuaikan dengan sistem Anda
```

### Langkah 3: Atur Toleransi
Cari baris `#define toleransi 50` dan sesuaikan:

```cpp
#define toleransi 50  // ← Atur nilai ini
// Nilai yang disarankan:
// - 30: Sangat presisi (motor bergerak lebih lama)
// - 50: Presisi sedang (default)
// - 100: Longgar (motor berhenti lebih cepat)
```

### Langkah 4: Atur Timeout
Cari baris `const unsigned long timeout = 30000;` dan sesuaikan:

```cpp
const unsigned long timeout = 30000; // ← Atur nilai ini (dalam milidetik)
// Nilai yang disarankan:
// - 15000: 15 detik (cepat)
// - 30000: 30 detik (default)
// - 60000: 60 detik (lambat)
```

### Langkah 5: Atur Posisi Tertutup
Cari bagian `CLOSE_X1`, `CLOSE_X2`, dll dan sesuaikan:

```cpp
#define CLOSE_X1  50   // ← Sesuaikan dengan nilai minimum X1
#define CLOSE_X2  45   // ← Sesuaikan dengan nilai minimum X2
#define CLOSE_Y1  55   // ← Sesuaikan dengan nilai minimum Y1
#define CLOSE_Y2  40   // ← Sesuaikan dengan nilai minimum Y2
```

### Langkah 6: Upload Kode
1. Klik **"Verify"** untuk memastikan tidak ada error
2. Klik **"Upload"** untuk upload ke Arduino
3. Tunggu sampai upload selesai

---

## 🧪 **TESTING DAN VALIDASI**

### Test 1: Kalibrasi Dasar
**Tujuan**: Memastikan nilai potensiometer terbaca dengan benar

**Langkah**:
1. Upload kode ke Arduino
2. Buka Serial Monitor (115200 baud)
3. Pilih mode "KOLIMASI"
4. Pilih ukuran film apapun
5. Perhatikan nilai X1, X2, Y1, Y2 yang ditampilkan
6. Catat nilai minimum dan maksimum untuk setiap potensiometer

**Expected Result**:
- ✅ Nilai potensiometer terbaca (0-1023)
- ✅ Nilai berubah saat motor bergerak
- ✅ Tidak ada nilai yang stuck atau tidak masuk akal

### Test 2: Target Configuration
**Tujuan**: Memastikan target nilai sesuai dengan ukuran film

**Langkah**:
1. Pilih ukuran "18x24 cm"
2. Perhatikan target yang digunakan
3. Pilih ukuran "24x30 cm"
4. Perhatikan target yang digunakan
5. Lakukan untuk semua ukuran

**Expected Result**:
- ✅ Target sesuai dengan ukuran film
- ✅ Nilai target masuk akal (tidak terlalu kecil atau besar)

### Test 3: Motor Movement Control
**Tujuan**: Memastikan motor bergerak sampai target tercapai

**Langkah**:
1. Pilih ukuran film
2. Perhatikan gerakan motor
3. Monitor nilai potensiometer di LCD
4. Tunggu sampai semua motor berhenti
5. Periksa status final

**Expected Result**:
- ✅ Motor bergerak sampai nilai potensiometer mencapai target ± toleransi
- ✅ Status OK/NG ditampilkan untuk setiap motor
- ✅ Status final "SESUAI" jika semua target tercapai

### Test 4: Timeout Safety
**Tujuan**: Memastikan motor berhenti jika target tidak tercapai dalam waktu tertentu

**Langkah**:
1. Pilih ukuran film
2. Jika motor bergerak terlalu lama (>30 detik), pastikan berhenti
3. Periksa status di Serial Monitor

**Expected Result**:
- ✅ Motor berhenti setelah timeout jika target tidak tercapai
- ✅ Pesan timeout ditampilkan di Serial Monitor

### Test 5: Toleransi Testing
**Tujuan**: Memastikan motor berhenti saat dalam range toleransi

**Langkah**:
1. Pilih ukuran film
2. Monitor nilai potensiometer
3. Pastikan motor berhenti saat nilai = target ± toleransi

**Expected Result**:
- ✅ Motor berhenti saat nilai dalam range toleransi
- ✅ Status OK ditampilkan saat dalam toleransi

### Test 6: Mode Iluminasi
**Tujuan**: Memastikan motor bergerak ke posisi 25x25 cm untuk iluminasi

**Langkah**:
1. Pilih mode "UJI ILUMINASI"
2. Monitor gerakan motor
3. Pastikan target yang digunakan sesuai (misal: 250)
4. Test tombol "TERCAPAI", "TIDAK", "KEMBALI"

**Expected Result**:
- ✅ Motor bergerak ke target yang ditentukan untuk semua axis
- ✅ Lampu menyala setelah motor selesai
- ✅ Tombol validasi berfungsi dengan benar

### Test 7: Reset Function
**Tujuan**: Memastikan fungsi reset pintu berfungsi

**Langkah**:
1. Buka pintu dengan memilih ukuran film
2. Tekan tombol "KEMBALI"
3. Monitor gerakan motor menutup pintu

**Expected Result**:
- ✅ Motor bergerak menutup pintu sampai target CLOSE
- ✅ Semua motor berhenti setelah pintu tertutup

---

## 🔧 **TROUBLESHOOTING**

### Problem: Motor Tidak Bergerak
**Gejala**: Motor tidak bergerak sama sekali

**Solusi**:
1. **Cek koneksi motor**:
   - Pastikan kabel motor terhubung dengan benar
   - Cek pin XLEAF1A, XLEAF1B, dll
2. **Cek power supply**:
   - Pastikan motor mendapat tegangan yang cukup
   - Cek driver motor
3. **Cek pin Arduino**:
   - Pastikan pin output terhubung dengan benar
   - Test dengan LED untuk memastikan pin berfungsi

### Problem: Potensiometer Tidak Terbaca
**Gejala**: Nilai potensiometer tidak berubah atau stuck

**Solusi**:
1. **Cek koneksi potensiometer**:
   - Pastikan terhubung ke pin analog yang benar (A0, A1, A2, A3)
   - Cek VCC dan GND potensiometer
2. **Cek nilai di Serial Monitor**:
   - Pastikan nilai berubah saat potensiometer diputar
   - Cek apakah nilai dalam range 0-1023

### Problem: Target Tidak Tercapai
**Gejala**: Motor bergerak terus tanpa berhenti

**Solusi**:
1. **Cek nilai target**:
   - Pastikan target tidak terlalu tinggi atau rendah
   - Sesuaikan dengan kalibrasi sistem
2. **Cek toleransi**:
   - Perbesar toleransi jika motor tidak berhenti
   - Contoh: ubah dari 50 ke 100
3. **Cek kalibrasi**:
   - Pastikan nilai potensiometer sesuai dengan posisi fisik

### Problem: Motor Bergerak Terbalik
**Gejala**: Motor bergerak ke arah yang salah

**Solusi**:
1. **Cek koneksi motor**:
   - Tukar koneksi A dan B motor
   - Contoh: XLEAF1A ↔ XLEAF1B
2. **Cek logika kontrol**:
   - Pastikan HIGH/LOW untuk maju/mundur sudah benar
   - Sesuaikan di fungsi `kontrolMotorSampaiTarget()`

### Problem: Timeout Terlalu Cepat
**Gejala**: Motor berhenti sebelum target tercapai

**Solusi**:
1. **Perbesar timeout**:
   ```cpp
   const unsigned long timeout = 60000; // Ubah ke 60 detik
   ```
2. **Cek apakah ada masalah mekanis**:
   - Pastikan motor tidak macet
   - Cek apakah ada beban yang terlalu berat

### Problem: Toleransi Terlalu Ketat
**Gejala**: Motor bergerak terlalu lama untuk mencapai target

**Solusi**:
1. **Perbesar toleransi**:
   ```cpp
   #define toleransi 100 // Ubah dari 50 ke 100
   ```
2. **Atur target yang lebih realistis**:
   - Sesuaikan target dengan kemampuan sistem

---

## ⚡ **OPTIMASI LANJUTAN**

### Optimasi 1: Fine Tuning Target
Setelah testing dasar berhasil, Anda bisa:

1. **Atur target lebih presisi**:
   ```cpp
   // Contoh: Jika 18x24 cm sebenarnya butuh 220, bukan 200
   #define TARGET_18x24_X1  220
   #define TARGET_18x24_X2  220
   #define TARGET_18x24_Y1  220
   #define TARGET_18x24_Y2  220
   ```

2. **Atur toleransi yang berbeda untuk setiap motor**:
   ```cpp
   // Jika motor X lebih presisi dari motor Y
   #define toleransi_X 30
   #define toleransi_Y 50
   ```

### Optimasi 2: Kecepatan Motor
Jika motor bergerak terlalu lambat atau cepat:

1. **Atur delay dalam loop**:
   ```cpp
   delay(10); // Ubah dari 10 ke 5 untuk lebih cepat, atau 20 untuk lebih lambat
   ```

2. **Atur update tampilan**:
   ```cpp
   if (millis() - lastUpdate > 500) { // Ubah dari 500 ke 200 untuk update lebih cepat
   ```

### Optimasi 3: Safety Features
Tambahkan fitur keamanan:

1. **Limit switch** untuk mencegah motor bergerak terlalu jauh
2. **Emergency stop** untuk menghentikan motor dalam keadaan darurat
3. **Overload protection** untuk mencegah motor terbakar

### Optimasi 4: User Interface
Perbaiki tampilan user:

1. **Progress bar** untuk menunjukkan progress motor
2. **Countdown timer** untuk menunjukkan sisa waktu
3. **Error messages** yang lebih informatif

---

## 📝 **CHECKLIST FINAL**

Sebelum sistem siap digunakan, pastikan:

### Hardware:
- ✅ Arduino Mega 2560 terhubung dengan benar
- ✅ Motor DC dan driver berfungsi
- ✅ Potensiometer terbaca dengan benar
- ✅ TFT LCD dan remote berfungsi
- ✅ Power supply stabil

### Software:
- ✅ Kode terupload tanpa error
- ✅ Target nilai sesuai kalibrasi
- ✅ Toleransi sesuai kebutuhan
- ✅ Timeout tidak terlalu cepat/lambat

### Testing:
- ✅ Semua ukuran film berfungsi
- ✅ Mode iluminasi berfungsi
- ✅ Reset pintu berfungsi
- ✅ Timeout safety berfungsi
- ✅ Tampilan real-time berfungsi

### Dokumentasi:
- ✅ Catat nilai kalibrasi final
- ✅ Catat target nilai final
- ✅ Catat setting toleransi dan timeout
- ✅ Simpan backup kode

---

## 🎉 **SELAMAT!**

Sistem kontrol motor kolimator rontgen Anda sudah siap digunakan dengan pengaturan yang optimal. 

**Tips penggunaan**:
- Monitor sistem secara berkala
- Lakukan kalibrasi ulang jika diperlukan
- Backup kode dan setting secara rutin
- Dokumentasikan perubahan yang dilakukan

**Untuk pertanyaan lebih lanjut**, silakan konsultasikan dengan tim pengembang atau referensi dokumentasi yang telah dibuat. 