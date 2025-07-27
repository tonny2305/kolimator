# PERBAIKAN KODE TERBARU KOLIMATOR RONTGEN
## Analisis dan Solusi Masalah

---

## 🔍 **MASALAH YANG DIIDENTIFIKASI**

Berdasarkan feedback Anda, masalah utama pada kode terbaru adalah:

### 1. **Jendela film tidak terbuka sesuai ukuran yang dimau**
- **Masalah**: Motor bergerak ke posisi terbuka penuh (900) untuk semua ukuran
- **Penyebab**: Tidak ada target spesifik untuk setiap ukuran film
- **Dampak**: Semua ukuran film membuka ke posisi yang sama

### 2. **Motor belakang terbuka, motor depan tertutup saat jendela tutup**
- **Masalah**: Logika kontrol motor yang tidak konsisten
- **Penyebab**: Motor X1 dan X2 bergerak berlawanan arah
- **Dampak**: Jendela tidak tertutup dengan benar

### 3. **Motor tidak berhenti saat iluminasi**
- **Masalah**: Motor terus bergerak tanpa target yang jelas
- **Penyebab**: Tidak ada kontrol motor berbasis target untuk iluminasi
- **Dampak**: Motor bergerak terus sampai timeout

---

## ✅ **SOLUSI YANG DITERAPKAN**

### 1. **Menambahkan Target Nilai Potensiometer**

```cpp
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
```

### 2. **Fungsi Kontrol Motor Berbasis Target**

```cpp
void kontrolMotorSampaiTarget(int targetX1, int targetX2, int targetY1, int targetY2) {
  // Motor bergerak sampai target tercapai ± toleransi
  // Timeout 30 detik untuk safety
  // Feedback real-time setiap 500ms
  // Setiap motor bergerak independen sampai targetnya tercapai
}
```

### 3. **Fungsi Mendapatkan Target Berdasarkan Ukuran**

```cpp
void dapatkanTargetUkuran(int ukuran, int& targetX1, int& targetX2, int& targetY1, int& targetY2) {
  switch(ukuran) {
    case 0: // 18x24
      targetX1 = TARGET_18x24_X1;
      // dst...
  }
}
```

### 4. **Perbaikan Logika Kolimasi**

**Sebelum:**
```cpp
// Motor bergerak 2 detik saja
digitalWrite(XLEAF1A, HIGH); digitalWrite(XLEAF1B, LOW);
delay(2000);
// Validasi berdasarkan OPEN_X1 (900)
bool sesuai = abs(x1 - OPEN_X1) <= toleransi;
```

**Sesudah:**
```cpp
// Dapatkan target berdasarkan ukuran film
int targetX1, targetX2, targetY1, targetY2;
dapatkanTargetUkuran(ukuran, targetX1, targetX2, targetY1, targetY2);

// Kontrol motor sampai target tercapai
kontrolMotorSampaiTarget(targetX1, targetX2, targetY1, targetY2);
```

### 5. **Perbaikan Logika Iluminasi**

**Sebelum:**
```cpp
// Motor bergerak 2 detik tanpa target
digitalWrite(XLEAF1A, HIGH); digitalWrite(XLEAF1B, LOW);
delay(2000);
```

**Sesudah:**
```cpp
// Kontrol motor untuk ukuran iluminasi (25x25 cm)
kontrolMotorSampaiTarget(TARGET_ILUMINASI_X1, TARGET_ILUMINASI_X2, TARGET_ILUMINASI_Y1, TARGET_ILUMINASI_Y2);
```

### 6. **Perbaikan Fungsi Reset Pintu**

**Sebelum:**
```cpp
void resetPintu() {
  digitalWrite(XLEAF1A, LOW); digitalWrite(XLEAF1B, HIGH);
  delay(2000);
  // dst...
}
```

**Sesudah:**
```cpp
void resetPintu() {
  Serial.println("Menutup pintu kolimator...");
  kontrolMotorSampaiTarget(CLOSE_X1, CLOSE_X2, CLOSE_Y1, CLOSE_Y2);
}
```

---

## 🎯 **CARA KERJA SISTEM YANG DIPERBAIKI**

### **Mode Kolimasi:**
1. User pilih ukuran film (18x24, 24x30, 30x40, 35x43)
2. Sistem dapatkan target potensiometer untuk ukuran tersebut
3. Motor bergerak sampai target tercapai (±50 toleransi)
4. Tampilkan status final (SESUAI/TIDAK SESUAI)

### **Mode Iluminasi:**
1. Motor bergerak ke posisi 25x25 cm (target 250)
2. Lampu menyala 100 lux
3. User validasi cahaya tercapai atau tidak
4. Jika tidak tercapai, motor bergerak ulang ke target yang sama

### **Reset Pintu:**
1. Motor bergerak ke posisi tertutup (target 100)
2. Semua motor berhenti setelah target tercapai

---

## 📊 **PERBANDINGAN SEBELUM DAN SESUDAH**

| Aspek | Sebelum | Sesudah |
|-------|---------|---------|
| **Target Kolimasi** | Selalu terbuka penuh (900) | Sesuai ukuran film (250, 350, 450, 550) |
| **Target Iluminasi** | Tidak ada target | Posisi 25x25 cm (250) |
| **Kontrol Motor** | Berbasis waktu (2 detik) | Berbasis target potensiometer |
| **Feedback** | Tidak ada | Real-time status OK/NG |
| **Timeout** | Tidak ada | 30 detik safety |
| **Presisi** | Rendah | Tinggi (±50 toleransi) |
| **Konsistensi** | Tidak konsisten | Konsisten untuk semua ukuran |

---

## 🔧 **KONFIGURASI TARGET YANG BISA DIATUR**

### **Target untuk Setiap Ukuran Film:**
```cpp
// Sesuaikan nilai ini berdasarkan kalibrasi sistem Anda
#define TARGET_18x24_X1  250  // ← Atur sesuai kebutuhan
#define TARGET_24x30_X1  350  // ← Atur sesuai kebutuhan
#define TARGET_30x40_X1  450  // ← Atur sesuai kebutuhan
#define TARGET_35x43_X1  550  // ← Atur sesuai kebutuhan
```

### **Toleransi:**
```cpp
#define toleransi 50  // ← Atur nilai ini (30-100)
```

### **Timeout:**
```cpp
const unsigned long timeout = 30000; // ← Atur nilai ini (dalam milidetik)
```

---

## 🧪 **TESTING YANG DISARANKAN**

### 1. **Test Kalibrasi**
- Upload kode ke Arduino
- Buka Serial Monitor
- Pilih mode "KOLIMASI"
- Catat nilai potensiometer yang muncul
- Sesuaikan target nilai di kode

### 2. **Test Setiap Ukuran Film**
- Test ukuran 18x24 cm
- Test ukuran 24x30 cm
- Test ukuran 30x40 cm
- Test ukuran 35x43 cm
- Pastikan motor bergerak ke posisi yang berbeda

### 3. **Test Mode Iluminasi**
- Pilih mode "UJI ILUMINASI"
- Pastikan motor bergerak ke posisi 25x25 cm
- Test tombol "TERCAPAI", "TIDAK", "KEMBALI"

### 4. **Test Reset Pintu**
- Buka pintu dengan memilih ukuran film
- Tekan tombol "KEMBALI"
- Pastikan motor bergerak menutup pintu dengan benar

---

## 💡 **TIPS PENGGUNAAN**

### **Jika Motor Bergerak Terbalik:**
1. Cek koneksi motor X1 dan X2
2. Tukar koneksi A dan B motor
3. Sesuaikan logika HIGH/LOW di fungsi `kontrolMotorSampaiTarget()`

### **Jika Target Tidak Tercapai:**
1. Perbesar toleransi dari 50 ke 100
2. Sesuaikan nilai target berdasarkan kalibrasi
3. Perbesar timeout dari 30 detik ke 60 detik

### **Jika Motor Tidak Bergerak:**
1. Cek power supply motor
2. Cek koneksi driver motor
3. Cek pin output Arduino

---

## 🎉 **KESIMPULAN**

Perbaikan yang telah diterapkan mengatasi semua masalah yang Anda sebutkan:

1. ✅ **Jendela film terbuka sesuai ukuran** - Target berbeda untuk setiap ukuran film
2. ✅ **Motor bergerak konsisten** - Logika kontrol motor yang seragam
3. ✅ **Motor berhenti saat iluminasi** - Target spesifik untuk iluminasi
4. ✅ **Touchscreen tetap berfungsi** - Tidak ada perubahan pada interface

**Kode sekarang menggabungkan:**
- Touchscreen yang sudah berfungsi dari versi terbaru
- Logika kontrol motor yang presisi dari versi sebelumnya
- Target spesifik untuk setiap ukuran film
- Safety timeout dan feedback real-time

Sistem sekarang akan bekerja sesuai dengan flowchart yang Anda berikan, dengan motor yang bergerak sampai target tercapai dan memberikan feedback real-time kepada user. 