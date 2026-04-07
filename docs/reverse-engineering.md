# Reverse Engineering Guide untuk Lumina (GTA V Enhanced)

> Panduan ini untuk reference bila perlu cari/update byte patterns dalam Lumina.
> Ditulis khusus untuk GTA V Enhanced Edition (DX12).

---

## Daftar Isi

1. [Konsep Asas](#1-konsep-asas)
2. [Tools yang Diperlukan](#2-tools-yang-diperlukan)
3. [Memahami Sistem Pattern dalam Lumina](#3-memahami-sistem-pattern-dalam-lumina)
4. [Memahami BytePatches](#4-memahami-bytepatches)
5. [Cara Mencari Pattern Baru (x64dbg)](#5-cara-mencari-pattern-baru-x64dbg)
6. [Cara Mencari Pattern Baru (Ghidra)](#6-cara-mencari-pattern-baru-ghidra)
7. [Status Bypass Patterns Semasa](#7-status-bypass-patterns-semasa)
8. [Cara Baca Log untuk Diagnose](#8-cara-baca-log-untuk-diagnose)
9. [Quick Reference: x86-64 Opcodes](#9-quick-reference-x86-64-opcodes)
10. [Tips dan Peringatan](#10-tips-dan-peringatan)

---

## 1. Konsep Asas

### Apa itu Pattern Scanning?

Program (GTA5_Enhanced.exe) adalah koleksi bytes dalam memory. Contoh:

```
48 8B C8 FF 52 30 84 C0 74 05 48 8B 45 10
```

Setiap instructions x86-64 terdiri dari bytes. `74 05` bermaksud "jump 5 bytes jika zero flag set" (JZ +5).

**Pattern** = urutan bytes unik yang mengenal pasti lokasi tertentu dalam binary.

**Wildcards** (`?`) digunakan untuk bytes yang berubah setiap game update:
```
E8 ? ? ? ? 48 8B 78 48
```
`E8` = CALL instruction. `? ? ? ?` = 4 bytes relative address (berubah setiap update). `48 8B 78 48` = bytes tetap selepas call.

### Static vs Dynamic Analysis

| Jenis | Tool | Cara |
|-------|------|------|
| **Static** | Ghidra, IDA Pro | Baca binary dari disk tanpa run program |
| **Dynamic** | x64dbg, CheatEngine | Attach ke program yang sedang berjalan |

### Kenapa Pattern Boleh Gagal?

1. **Game update** — Rockstar ubah code, bytes bertukar
2. **Edition berbeza** — Enhanced (DX12) vs Legacy (DX11) binary berbeza
3. **Obfuscation** — Rockstar/Arxan boleh scramble code
4. **Pattern terlalu pendek** — Match banyak lokasi, pilih yang salah

---

## 2. Tools yang Diperlukan

### Wajib

| Tool | Guna | Link |
|------|------|------|
| **x64dbg** | Dynamic analysis, breakpoints, trace | https://x64dbg.com |
| **Ghidra** | Static analysis (percuma dari NSA) | https://ghidra-sre.org |

### Optional tapi Berguna

| Tool | Guna |
|------|------|
| **IDA Free** | Static analysis (lebih polished UI dari Ghidra) |
| **HxD** | Hex editor untuk view raw bytes |
| **Reclass.NET** | View memory structures secara visual |
| **CE (Cheat Engine)** | Memory scanning, quick pointer finding |

### Setup x64dbg

1. Download dan extract
2. Run `x64dbg.exe` → pilih `x64` (bukan x32, GTA Enhanced is 64-bit)
3. File → Attach → cari `GTA5_Enhanced.exe`
4. Atau File → Open → buka `GTA5_Enhanced.exe` terus

### Setup Ghidra

1. Perlu Java 17+ (download Adoptium/Temurin JDK)
2. Download Ghidra, extract
3. Run `ghidraRun.bat`
4. New Project → Import File → pilih `GTA5_Enhanced.exe`
5. **AMARAN:** Import GTA Enhanced ambil masa lama (10-30 minit) sebab binary besar (~200MB+)
6. Selepas import, double-click untuk buka CodeBrowser
7. Analyze → Auto Analyze (biarkan default options, klik Analyze)

---

## 3. Memahami Sistem Pattern dalam Lumina

### Fail Utama

| Fail | Fungsi |
|------|--------|
| `src/core/memory/Pattern.hpp` | Compile-time pattern template |
| `src/core/memory/PatternScanner.hpp/.cpp` | Scanner engine |
| `src/core/memory/PointerCalculator.hpp` | `.Add()`, `.Rip()`, `.As<T>()` helper |
| `src/core/memory/BytePatches.hpp/.cpp` | Patch system (Apply/Restore) |
| `src/game/pointers/Pointers.hpp` | Declare semua pointer/patch |
| `src/game/pointers/Pointers.cpp` | Define semua pattern + callbacks |

### Cara Pattern Didefinisikan

```cpp
// 1. Define pattern string (IDA-style, ? = wildcard)
constexpr auto myPtrn = Pattern<"48 8B C8 FF 52 30 84 C0 74 05 48">("MyPattern");

// 2. Tambah ke scanner dengan callback
scanner.Add(myPtrn, [this](PointerCalculator ptr) {
    // ptr = alamat pertama yang match pattern
    // Dari sini, navigate ke target sebenar
    MyPointer = ptr.Add(8).As<uint8_t*>();
});
```

### PointerCalculator Operations

```cpp
ptr.Add(N)       // Tambah N bytes ke alamat
ptr.Sub(N)       // Tolak N bytes
ptr.Rip()        // Resolve RIP-relative address (untuk CALL/JMP)
                 // Reads 4 bytes at ptr, adds to ptr+4
ptr.As<T>()      // Cast ke type T
```

#### Contoh `.Rip()`:

```
Address 0x1000: E8 AB CD EF 00    (CALL +0x00EFCDAB)
```

`ptr.Add(1).Rip()` = baca 4 bytes di 0x1001 (`AB CD EF 00` = `0x00EFCDAB`), tambah ke `0x1001 + 4 = 0x1005`, hasil = `0x00EFCDAB + 0x1005 = target function`.

### Scanner Logging (Sedia Ada)

PatternScanner.cpp sudah log:
```
[INFO]    Found pattern [ModelSpawnBypass] : [0x7FF612345678]
[WARNING] Failed to find pattern [PseudoObjectCheck]
```

---

## 4. Memahami BytePatches

### Apa yang BytePatch Buat

```cpp
// Tulis 1 byte (0xEB = JMP short) di alamat target
BytePatches::Add(address, 0xEB);

// Tulis multiple bytes (NOP sled)
BytePatches::Add(address, std::to_array<uint8_t>({0x90, 0x90, 0x90}));
```

BytePatch simpan **original bytes** sebelum patch. Boleh `Apply()` dan `Restore()` bila-bila.

### Kenapa NOP dan JMP?

**NOP (0x90)** — "No Operation". CPU skip instruction ni. Guna untuk "padamkan" code.

**JMP short (0xEB)** — Unconditional jump. Tukar conditional jump (`74` = JZ, `75` = JNZ) ke unconditional jump. Ini "paksa" code sentiasa/tak pernah jump.

### Contoh Sebenar: ModelSpawnBypass

**Sebelum patch** (dalam GTA binary):
```asm
74 05       ; JZ +5 (jump over jika result = zero/false)
48 8B 45 10 ; MOV RAX, [RBP+10]
...
```

**Selepas patch** (kita tulis 0xEB):
```asm
EB 05       ; JMP +5 (SENTIASA jump, tak kira result)
48 8B 45 10 ; MOV RAX, [RBP+10]  (never reached)
```

Ini bypass check "is model allowed?" — sentiasa jump ke "yes, allowed".

---

## 5. Cara Mencari Pattern Baru (x64dbg — Dynamic)

> Ini cara paling praktikal untuk cari bypass patterns.

### Langkah 1: Cari Native Handler Address

GTA natives dipanggil melalui table. Kita perlu cari address `CREATE_OBJECT_NO_OFFSET` handler.

**Cara mudah:** Lumina sudah log semua pointer addresses. Cari dalam log:
```
Found pattern [InitNativeTables] : [0x7FF6XXXXXXXX]
```

Atau guna x64dbg:
1. Attach ke GTA5_Enhanced.exe
2. Ctrl+G → masukkan address dari Lumina log
3. Set breakpoint (F2)

### Langkah 2: Trigger Spawn dan Break

1. Set breakpoint pada native handler
2. Dalam game, try spawn object (guna Lumina crash feature)
3. x64dbg akan break pada handler
4. Tekan F8 (Step Over) untuk trace execution

### Langkah 3: Cari Conditional Jumps

Masa trace, cari pattern macam:
```asm
84 C0           ; TEST AL, AL (check return value)
74 XX           ; JZ +XX (jump jika false — this is the block!)
```
atau
```asm
85 C0           ; TEST EAX, EAX
0F 84 XX XX XX XX  ; JZ near (6-byte jump — this is the block!)
```

**Ini yang kita nak patch!** — tukar JZ ke JMP (0x74→0xEB) atau NOP.

### Langkah 4: Extract Bytes Sebagai Pattern

1. Klik pada instruction yang nak dijadikan anchor
2. Highlight ~10-15 bytes sebelum dan selepas
3. Right-click → Binary → Copy
4. Tukar bytes yang mungkin berubah (addresses, offsets) ke `?`

### Langkah 5: Test Pattern

Di x64dbg: Ctrl+B (Search for Pattern) → paste pattern → pastikan cuma 1 result.

Kalau ada banyak results, tambah lebih bytes untuk buat pattern lebih unik.

### Contoh Workflow Penuh

```
Objective: Cari PseudoObjectCheck untuk Enhanced

1. Breakpoint pada CREATE_OBJECT_NO_OFFSET handler
2. Step through execution
3. Jumpa:
     0x7FF612340100: 48 85 C0          TEST RAX, RAX
     0x7FF612340103: 0F 84 XX XX XX XX JZ [somewhere far]
     0x7FF612340109: 8B 48 50          MOV ECX, [RAX+50]
     
4. Pattern = "48 85 C0 0F 84 ? ? ? ? 8B 48 50"
   (? ? ? ? kerana JZ offset berubah setiap update)
   
5. Patch = NOP 24 bytes bermula dari 48 85 C0
```

---

## 6. Cara Mencari Pattern Baru (Ghidra — Static)

### Langkah 1: Import Binary

1. Ghidra → New Project → Import `GTA5_Enhanced.exe`
2. Analyze (auto) — tunggu siap
3. Ini ambil masa lama. Binary GTA Enhanced ~200MB+

### Langkah 2: Cari Fungsi Target

**Search → For Strings:** Cari string macam "CREATE_OBJECT" — GTA mungkin ada debug strings

**Search → Memory:** Cari bytes sequence. Contoh: native hash untuk `CREATE_OBJECT_NO_OFFSET`

**Function Graph:** Klik fungsi → Window → Function Graph untuk visualize control flow

### Langkah 3: Trace dari Native Table

1. Cari native registration function
2. Navigate ke handler untuk `CREATE_OBJECT_NO_OFFSET` (hash: `0x509D5878`)
3. Baca decompiled code
4. Cari `if` checks yang validate model

### Langkah 4: Extract Pattern

1. Klik pada instruction target di Listing view
2. Bytes ditunjukkan di sebelah kiri
3. Tulis pattern, ganti variable bytes dengan `?`

### Ghidra Tips

- **Ctrl+Shift+E** — Search for bytes/pattern
- **L** — Label/rename function (untuk track progress)
- **;** — Add comment
- **Ctrl+Shift+F** — Search for strings
- Double-click pada address/reference untuk jump ke sana

---

## 7. Status Bypass Patterns Semasa

### 3 Bypass Patches yang Diperlukan

#### 1. ModelSpawnBypass (Script Handler Check)

| | Legacy (Stand) | Enhanced (Lumina — BELUM VERIFY) |
|---|---|---|
| **Pattern** | `48 8B C8 FF 52 30 84 C0 74 05 48` | `E8 ? ? ? ? 48 8B 78 48` |
| **Offset** | +8 | +1 → Rip → +0x2B |
| **Patch** | NOP 2 bytes | Write 0xEB (JMP) |
| **Purpose** | Bypass `GetCurrentGtaScriptHandlerNetwork` check | Same intent |

**Apa yang dibypass:** Sebelum spawn, game check "adakah script thread ini ada valid network handler?" Kalau gagal, spawn dibatalkan.

**Cara cari dalam Enhanced:**
- Cari fungsi yang panggil `GetCurrentGtaScriptHandlerNetwork` (or virtual call pada vtable offset 0x30)
- Pattern bytes: `FF 52 30` (call [rdx+0x30]) diikuti `84 C0` (test al,al) diikuti `74` (jz)

#### 2. WorldModelSpawnBypass (Model Blacklist Check)

| | Legacy (Stand) | Enhanced (Lumina — BELUM VERIFY) |
|---|---|---|
| **Pattern** | `3B CB 74 09 45 84 D2 0F 85` | `4C 8B 2C 01 4D 85 ED 0F 84 ? ? ? ?` |
| **Offset** | +7 | +4 |
| **Patch** | NOP 6 bytes | Write `{0xEB, 0x12, 0x90}` |
| **Purpose** | Bypass model blacklist check | Same intent |

**Apa yang dibypass:** Game ada senarai model yang di-blacklist (macam `prop_log_aa`). Check ni tentukan "adakah model ini dalam blacklist?"

**Cara cari dalam Enhanced:**
- Cari fungsi yang compare model hash dengan blacklist array
- Biasanya ada `CMP` atau `TEST` diikuti conditional jump besar (`0F 84` atau `0F 85`)

#### 3. PseudoObjectCheck (Object Validation)

| | Legacy (Stand) | Enhanced (Lumina — BELUM VERIFY) |
|---|---|---|
| **Pattern** | `48 85 C0 0F 84 ? ? ? ? 8B 48 50` | Same (copy dari Stand) |
| **Offset** | +0 | +0 |
| **Patch** | NOP 24 bytes | NOP 24 bytes |
| **Purpose** | Bypass pseudo-object validation | Same intent |

**Apa yang dibypass:** Selepas object dibuat, game validate sama ada ia "pseudo object" yang sah. Kalau gagal, object didelete.

**Cara cari dalam Enhanced:**
- Cari code yang check `[rax+0x50]` (offset 0x50 dalam object struct) 
- `48 85 C0` = test rax, rax; `0F 84` = jz (near)
- Biasanya dalam fungsi `CREATE_OBJECT` atau dipanggil olehnya

### Macam Mana Nak Confirm Pattern Betul?

Selepas inject Lumina, check log file:

```
✅ Found pattern [ModelSpawnBypass] : [0x...]     → Pattern JUMPA
❌ Failed to find pattern [PseudoObjectCheck]      → Pattern TAK JUMPA, perlu reverse

✅ ModelSpawnBypass: APPLIED                        → Patch berjaya di-apply
❌ PseudoObjectCheck: NOT FOUND (pattern failed)   → Tak boleh patch
```

---

## 8. Cara Baca Log untuk Diagnose

### Lokasi Log File

Log Lumina biasanya di:
```
%APPDATA%/Lumina/cout.log
```

### Apa yang Perlu Dicari

#### Fasa 1: Pattern Scanning (masa init)

```log
[INFO] Found pattern [ModelSpawnBypass] : [0x7FF612345678]
[INFO] ModelSpawnBypass patch address: 0x7FF612345ABC
[INFO] Found pattern [WorldModelSpawnBypass] : [0x7FF612346789]
[INFO] WorldModelSpawnBypass patch address: 0x7FF612346DEF
[INFO] Found pattern [PseudoObjectCheck] : [0x7FF612347890]
[INFO] PseudoObjectCheck patch address: 0x7FF612347890
```

Kalau nampak `Failed to find pattern [X]` → pattern tak match binary Enhanced.

#### Fasa 2: Masa Spawn (bila trigger crash)

```log
[INFO] IranianDroneCrash: Applying bypass patches...
[INFO]   ModelSpawnBypass: APPLIED
[INFO]   WorldModelSpawnBypass: APPLIED  
[INFO]   PseudoObjectCheck: APPLIED
[INFO]   SpectatePatch: APPLIED
[INFO] IranianDroneCrash: Requesting model 0xC50A4285
[INFO] IranianDroneCrash: Model loaded successfully
[WARNING] IranianDroneCrash: CREATE_OBJECT_NO_OFFSET returned 0 for object 0
[WARNING] IranianDroneCrash: CREATE_OBJECT_NO_OFFSET returned 0 for object 1
...
[INFO] IranianDroneCrash: Spawned 0/20 objects
```

### Diagnosis Table

| Log Output | Maksud | Tindakan |
|-----------|--------|----------|
| `Failed to find pattern [X]` | Pattern tak match Enhanced | Reverse cari pattern baru |
| `X: NOT FOUND (pattern failed)` | Patch tak boleh applied | Same as above |
| `X: APPLIED` + still `returned 0` | Patch applied tapi tak betul lokasi | Pattern jumpa tapi salah lokasi, reverse semula |
| `Model load TIMED OUT` | Model tak boleh load langsung | Model mungkin tak wujud dalam Enhanced |
| `Spawned 0/20 objects` | Semua spawn gagal | Check which patches failed above |
| `Spawned 20/20 objects` | Semua berjaya! | Feature working ✅ |

---

## 9. Quick Reference: x86-64 Opcodes

### Opcodes yang Selalu Jumpa

| Bytes | Instruction | Nota |
|-------|-------------|------|
| `90` | NOP | No operation (1 byte) |
| `0F 1F 44 00 00` | NOP (5 bytes) | Multi-byte NOP |
| `EB XX` | JMP short | Unconditional jump (2 bytes, ±127) |
| `E9 XX XX XX XX` | JMP near | Unconditional jump (5 bytes) |
| `74 XX` | JZ / JE short | Jump if zero/equal |
| `75 XX` | JNZ / JNE short | Jump if not zero |
| `0F 84 XX XX XX XX` | JZ near | Jump if zero (6 bytes) |
| `0F 85 XX XX XX XX` | JNZ near | Jump if not zero (6 bytes) |
| `84 C0` | TEST AL, AL | Check if AL is zero |
| `85 C0` | TEST EAX, EAX | Check if EAX is zero |
| `48 85 C0` | TEST RAX, RAX | Check if RAX is zero (64-bit) |
| `E8 XX XX XX XX` | CALL | Function call (relative) |
| `FF 52 XX` | CALL [RDX+XX] | Virtual function call |
| `C3` | RET | Return from function |
| `48 8B XX` | MOV (64-bit) | Move data between registers |
| `3B` | CMP | Compare two values |

### Common Patch Patterns

**Bypass conditional check (2 bytes):**
```
Before: 74 05       (JZ +5 — skip if false)
After:  EB 05       (JMP +5 — always skip)
After:  90 90       (NOP NOP — never skip) 
```

**Bypass conditional check (6 bytes):**
```
Before: 0F 84 XX XX XX XX  (JZ near)
After:  90 90 90 90 90 90  (6x NOP — erase the jump entirely)
After:  0F 85 XX XX XX XX  (JNZ — reverse the condition)
```

**Return early from function:**
```
C3                          (RET — return immediately)
48 31 C0 C3                (XOR RAX,RAX; RET — return 0/false)
48 C7 C0 01 00 00 00 C3   (MOV RAX,1; RET — return 1/true)
```

---

## 10. Tips dan Peringatan

### Do's ✅

- **Sentiasa backup original bytes** — BytePatches dah buat ni auto untuk kita
- **Test satu pattern sekali** — jangan ubah semua sekaligus
- **Guna log** — check sama ada pattern jumpa sebelum debug deeper
- **Cari daripada open source menu lain dulu** — mungkin ada orang dah port ke Enhanced
- **Wildcards (`?`) untuk variable bytes** — address offsets, relative jumps
- **Pattern minimum 8-10 bytes** — terlalu pendek = banyak false matches
- **Clear pattern cache** kalau update patterns — delete cache file supaya scanner rescan

### Don'ts ❌

- **Jangan copy patterns dari Legacy menu terus** — binary Enhanced berbeza
- **Jangan patch terlalu banyak bytes** kalau tak perlu — boleh corrupt unrelated code
- **Jangan tinggal patches applied permanent** kalau tak perlu — boleh cause instability
- **Jangan panic kalau pattern gagal** — normal, binary update selalu break patterns
- **Jangan lupa Arxan obfuscation** — sesetengah fungsi GTA Enhanced di-obfuscate, bytes mungkin tak nampak straightforward

### Resources untuk Belajar

| Resource | Link | Nota |
|----------|------|------|
| **x86-64 Instruction Reference** | https://www.felixcloutier.com/x86/ | Rujukan lengkap semua instructions |
| **Ghidra Tutorial (Official)** | https://ghidra-sre.org/CheatSheet.html | Cheat sheet Ghidra |
| **Unknown Cheats Forum** | https://www.unknowncheats.me/forum/grand-theft-auto-v/ | Community GTA reversing |
| **GTA V Native DB** | https://nativedb.dotindustries.dev/gta5 | Senarai semua natives dan hashes |
| **Abdul Bari C++ Course** | (course anda sedang ikuti) | Asas C++ yang penting |

### Workflow Ringkas: Update Pattern Selepas Game Update

```
1. Game update → build Lumina → inject → check log
2. Cari "Failed to find pattern" dalam log
3. Untuk setiap pattern yang gagal:
   a. Buka x64dbg → Attach GTA
   b. Cari fungsi yang sama (guna context dari pattern lama)
   c. Extract bytes baru sebagai pattern
   d. Update Pointers.cpp
4. Test → Verify → Commit
```

---

## Appendix: Lumina Pattern Locations

Semua patterns defined dalam `src/game/pointers/Pointers.cpp`. Edit fail ini untuk update/tambah patterns.

Semua BytePatch declarations dalam `src/game/pointers/Pointers.hpp`. Tambah field baru di sini kalau nak tambah patch baru.

Pattern cache di `%APPDATA%/Lumina/`. Delete cache files kalau patterns dah berubah supaya force rescan.
