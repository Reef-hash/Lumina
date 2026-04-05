# FILE NAMING & ORGANIZATION GUIDE

> Guide untuk memahami penstrukturan file dokumentasi dengan nama Bahasa Inggeris + urutan

---

## 📋 File Mapping (Lama → Baru)

Dokumentasi pembelajaran anda telah di-reorganisasi dengan nama Professional (Bahasa Inggeris) dan urutan numerik untuk kemudahan pembelajaran.

| Lama (Bahasa Malaysia) | Baru (English) | Status | Urutan |
|---|---|---|---|
| INDEX_PANDUAN.md / INDEKS_PANDUAN.md | **01_Index.md** | ✅ Sudah dibuat | 1 |
| STRUKTUR_PEMBELAJARAN.md | **02_ProjectStructure.md** | ⏳ Perlu rename | 2 |
| PANDUAN_COMMANDS.md | **03_CommandsGuide.md** | ⏳ Perlu rename | 3 |
| PANDUAN_MEMORY_HOOKING.md | **04_MemoryAndHooking.md** | ⏳ Perlu rename | 4 |
| ROADMAP_PEMBELAJARAN.md | **05_LearningRoadmap.md** | ⏳ Perlu rename | 5 |
| QUICK_REFERENCE.md | **06_QuickReference.md** | ⏳ Perlu rename | 6 |
| ARSITEKTUR_VISUAL.md | **07_ArchitectureDiagrams.md** | ⏳ Perlu rename | 7 |

---

## 📖 Peranan Setiap File

### 01_Index.md (Master Navigation Hub)
- **Tujuan**: Master index untuk semua dokumentasi
- **Isi**: Navigation hub, learning pathways, external resources
- **Gunakan**: Mulai pembelajaran, cari panduan tertentu
- **Bahasa**: English (Professional)

### 02_ProjectStructure.md (Overview Terperinci)
- **Tujuan**: Gambaran lengkap struktur projek
- **Isi**: Detail setiap folder core/, game/, types/
- **Gunakan**: Pahami architecture & folder purposes
- **Bahasa**: Bahasa Malaysia (untuk pemahaman mudah)
- **Asli Dari**: STRUKTUR_PEMBELAJARAN.md

### 03_CommandsGuide.md (OOP Deep Dive)
- **Tujuan**: Belajar OOP & Polymorphism detail
- **Isi**: Command system, inheritance, design patterns
- **Gunakan**: Memahami sistem perintah & virtual functions
- **Bahasa**: Bahasa Malaysia
- **Asli Dari**: PANDUAN_COMMANDS.md

### 04_MemoryAndHooking.md (Low-Level Programming)
- **Tujuan**: Memory manipulation & function hooking
- **Isi**: Pattern scanning, pointer arithmetic, hooking techniques
- **Gunakan**: Advanced C++ & Windows API
- **Bahasa**: Bahasa Malaysia
- **Asli Dari**: PANDUAN_MEMORY_HOOKING.md

### 05_LearningRoadmap.md (8-12 Month Structured Plan)
- **Tujuan**: Roadmap pembelajaran berstruktur
- **Isi**: 10 fasa, target files, exercises, timeline
- **Gunakan**: Blueprint pembelajaran personal anda
- **Bahasa**: Bahasa Malaysia
- **Asli Dari**: ROADMAP_PEMBELAJARAN.md

### 06_QuickReference.md (Cheat Sheet)
- **Tujuan**: Quick lookup & reference
- **Isi**: File structure, quick navigation, code patterns, troubleshooting
- **Gunakan**: Bookmark untuk referensi cepat
- **Bahasa**: Bahasa Malaysia
- **Asli Dari**: QUICK_REFERENCE.md

### 07_ArchitectureDiagrams.md (Visual Architecture)
- **Tujuan**: Visual & diagram understanding
- **Isi**: ASCII diagrams, dependency graphs, data flow, class hierarchy
- **Gunakan**: Memahami big picture & architecture
- **Bahasa**: Bahasa Malaysia
- **Asli Dari**: ARSITEKTUR_VISUAL.md

---

## 🚀 Cara Menggunakan File Baru

### Opsi 1: Manual Rename (Paling Cepat)
Jika anda mahu, boleh rename file lama ke nama baru:

```
1. STRUKTUR_PEMBELAJARAN.md → rename ke → 02_ProjectStructure.md
2. PANDUAN_COMMANDS.md → rename ke → 03_CommandsGuide.md
3. PANDUAN_MEMORY_HOOKING.md → rename ke → 04_MemoryAndHooking.md
4. ROADMAP_PEMBELAJARAN.md → rename ke → 05_LearningRoadmap.md
5. QUICK_REFERENCE.md → rename ke → 06_QuickReference.md
6. ARSITEKTUR_VISUAL.md → rename ke → 07_ArchitectureDiagrams.md

Lepas itu, delete file duplikat:
- Delete: INDEX_PANDUAN.md (duplicate)
- Delete: INDEKS_PANDUAN.md (duplicate)
- Delete: RUJUKAN_PANTAS.md (jika ada)
- Delete: SENI_RUPA_ARKITEKTUR.md (jika ada)
```

### Opsi 2: Copy Method (Jika Anda Lebih Suka)
Jika anda suka, boleh copy semua content dari file lama ke file baru dengan nama correct, kemudian delete file lama.

### Opsi 3: Biarkan Begini (Repository wise aman)
`.gitignore` sudah definisikan bahawa semua file-file ini tidak akan diupload ke GitHub, jadi tidak masalah jika ada duplikat atau file lama. Anda boleh:
- Tetap guna file lama (tetapi repo akan lebih clean jika didelete)
- Atau rename mengikuti struktur baru

---

## 📂 Struktur Akhir (Diharapkan)

```
Lumina/
├── .gitignore                          (Updated - ignore learning docs)
├── README.md                           (Original - tidak dirubah)
├── CMakeLists.txt
│
├── 📚 LEARNING DOCUMENTATION (English names + sequence)
│   ├── 01_Index.md                     ✅ Master navigation hub
│   ├── 02_ProjectStructure.md          (rename dari STRUKTUR_PEMBELAJARAN.md)
│   ├── 03_CommandsGuide.md             (rename dari PANDUAN_COMMANDS.md)
│   ├── 04_MemoryAndHooking.md          (rename dari PANDUAN_MEMORY_HOOKING.md)
│   ├── 05_LearningRoadmap.md           (rename dari ROADMAP_PEMBELAJARAN.md)
│   ├── 06_QuickReference.md            (rename dari QUICK_REFERENCE.md)
│   └── 07_ArchitectureDiagrams.md      (rename dari ARSITEKTUR_VISUAL.md)
│
├── 📁 src/                             (Project source code)
└── 📁 cmake/                           (Build configuration)
```

---

## ✅ Keuntungan Struktur Baru

1. ✅ **Professional** - English names lebih universal & professional
2. ✅ **Organized** - Numbered sequence memudahkan urutan pembelajaran
3. ✅ **Clear** - Mudah tahu file mana yang perlu baca dahulu
4. ✅ **Scalable** - Jika ada file baru, boleh tambah 08_, 09_, etc
5. ✅ **Git Safe** - Semua terdaftar dalam .gitignore, tidak akan commit

---

## 🎯 Rekomendasi Langkah Seterusnya

1. **Rename file** mengikut mapping di atas (Opsi 1 paling cepat)
2. **Delete file duplikat** (INDEX_PANDUAN.md, INDEKS_PANDUAN.md, etc)
3. **Buka 01_Index.md** untuk memulai pembelajaran
4. **Ikuti urutan 02 → 03 → 04 → dll** mengikut fase pembelajaran anda

---

## 📞 Bantuan Rename di Windows

Jika anda tidak tahu cara rename multiple files di Windows:

### Menggunakan File Explorer:
```
1. Buka folder Lumina
2. Klik kanan pada STRUKTUR_PEMBELAJARAN.md
3. Pilih "Rename"
4. Type: 02_ProjectStructure.md
5. Tekan Enter
6. Ulangi untuk file-file lain
```

### Menggunakan PowerShell (Cepat):
```powershell
# Buka PowerShell di folder Lumina
cd "d:\All About C++\Github-playground\Lumina"

# Rename files
Rename-Item "STRUKTUR_PEMBELAJARAN.md" "02_ProjectStructure.md"
Rename-Item "PANDUAN_COMMANDS.md" "03_CommandsGuide.md"
Rename-Item "PANDUAN_MEMORY_HOOKING.md" "04_MemoryAndHooking.md"
Rename-Item "ROADMAP_PEMBELAJARAN.md" "05_LearningRoadmap.md"
Rename-Item "QUICK_REFERENCE.md" "06_QuickReference.md"
Rename-Item "ARSITEKTUR_VISUAL.md" "07_ArchitectureDiagrams.md"

# Delete duplicates
Remove-Item "INDEX_PANDUAN.md"
Remove-Item "INDEKS_PANDUAN.md"
```

---

## 🎉 Selepas Selesai

Setelah semua file direname:

1. ✅ Anda akan mempunyai struktur yang clean & professional
2. ✅ Numbered sequence membantu navigasi pembelajaran
3. ✅ Semua documentation tetap dalam .gitignore (tidak upload ke GitHub)
4. ✅ Repository anda akan lebih professional & organized

**Mulai dengan baca [01_Index.md](01_Index.md) untuk pembelajaran dimulai!** 🚀

---

**Version: 1.0 | Created: April 2026**

*Selamat mengorganisir dokumentasi pembelajaran anda!* 📚
