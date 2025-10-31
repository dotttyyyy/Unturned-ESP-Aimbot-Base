# Unturned ESP + Aimbot Base  
**Offline Development Tool (Bots & Singleplayer)**

![Unturned ESP Example](https://i.imgur.com/UNTURNED_ESP.png)  
*Red boxes on zombies & players — perfect for testing!*

---

## What Is This?

A **simple, safe, external tool** that:
- Draws **red boxes** around zombies & players
- Lets you **aim at heads** by holding **Left Mouse Button**
- Works **only in offline mode** or **your own LAN server**

> **100% Legal & Safe** — **No bans** — because it's **your game**.

---

## Why Use This?

Perfect for:
- Learning how cheats work
- Testing AI zombies
- Practicing aim in singleplayer
- Debugging player positions

---

## Features (Easy Mode)

| Feature        | Hotkey       | What It Does |
|----------------|--------------|-------------|
| **Toggle All** | `Insert`     | Turn tool ON/OFF |
| **ESP Boxes**  | `Home`       | Toggle red boxes |
| **Aimbot**     | `End` + LMB  | Hold LMB → aim at head |

---

## How to Use (Step-by-Step)

### Step 1: Open Unturned Offline
1. Launch **Unturned**
2. Click **Play → Singleplayer** or **LAN Server**
3. Start a map (e.g., PEI)

### Step 2: Build the Tool
1. Open this folder
2. **Double-click `BUILD.bat`**
   - Wait 10 seconds → `UnturnedDevTool.exe` appears

### Step 3: Run the Tool
1. **Right-click `UnturnedDevTool.exe` → Run as Administrator**
2. Console says “Waiting for Unturned...”
3. Once game is detected → **you’re in!**

### Step 4: Use Hotkeys
- Press `Insert` → tool turns on
- Press `Home` → red boxes appear
- Press `End` + **hold LMB** → aimbot activates

---

## It Stopped Working? (Offsets Changed)

Unturned updates → tool breaks.

**Fix in 2 minutes:**

1. Open **Cheat Engine**
2. Attach to `Unturned.exe`
3. Take damage → search **health** (float)
4. Move → search **X, Y, Z** (3 floats)
5. Update numbers in `src/main.cpp`
6. Double-click `BUILD.bat`

**See `AUTO_UPDATE_GUIDE.txt` for pictures!**

---

## Is This Safe?

| Question | Answer |
|--------|--------|
| Will I get **banned**? | **NO** — singleplayer/LAN only |
| Can I use in **public servers**? | **NEVER** — don’t even try |
| Is this **legal**? | **YES** — for your own games |

---

## Want to Learn More?

- Open `src/main.cpp` — it’s **simple C++**
- Change `smooth = 0.5f` → slower/faster aim
- Change `fov = 90.0f` → bigger aim range

---

## Credits

- Built with love by **Dotty)**
- Auto-finds `GameAssembly.dll`
- GDI+ overlay (safe & external)

---

**Happy surviving!**  
*Never use on public servers. Stay safe. Build cool stuff.*
