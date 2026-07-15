# LiquidGlass

<p align="center">
<b>A universal Liquid Glass acrylic experience for Windows 11 powered by Windhawk.</b>
</p>

<p align="center">
Bringing translucent blur, depth, and modern glass aesthetics to your Windows desktop.
</p>

---

# Preview

Coming Soon

Preview images will be added here:

- 35% Glass Opacity Preview
- 55% Glass Opacity Preview
- File Explorer
- Windows Settings
- Windows Terminal
- Before / After Comparison

---

# About

LiquidGlass is a Windhawk modification that applies a system-wide acrylic blur effect with a dark glass tint to Windows 11 windows.

Inspired by modern glass-based UI design, LiquidGlass creates a cleaner and more immersive desktop experience while keeping the native Windows interface intact.

---

# Features

✨ Universal Acrylic Blur

Applies Windows acrylic blur effects across supported top-level windows.

✨ Adjustable Glass Opacity

Control the strength of the dark glass tint.

✨ System-Wide Effect

Designed to work across multiple Windows applications.

✨ Lightweight

Uses Windows composition APIs instead of replacing the Windows shell.

✨ Native Windows Experience

Keeps the original Windows interface while adding a glass aesthetic.

---

# Installation

## Requirements

- Windows 11
- Windhawk

## Steps

1. Install Windhawk.
2. Download `LiquidGlass.wh.cpp`.
3. Open Windhawk.
4. Select Create Mod.
5. Import the LiquidGlass code.
6. Compile and enable the mod.
7. Adjust opacity settings.

---

# Configuration

| Setting | Description | Default |
|---|---|---|
| Glass Opacity | Controls darkness of the glass tint | 35% |
35% - Subtle glass
55% - Dark premium glass
80% - Heavy dark glass

---

# Compatibility

Tested on:

- Windows 11 23H2
- Windows 11 24H2

Some applications may not fully support acrylic effects depending on how they render their interface.

Examples:

- Chromium applications
- Electron applications
- Games
- Custom GPU rendered software

---

# Technical Details

LiquidGlass uses Windows composition features:

- SetWindowCompositionAttribute
- ACCENT_ENABLE_ACRYLICBLURBEHIND
- Desktop Window Manager (DWM)

Because these APIs are undocumented, future Windows updates may affect compatibility.

---

# Roadmap

## Version 1.x

Completed:

- [x] Acrylic blur
- [x] Dark tint overlay
- [x] Adjustable opacity
- [x] Universal window support

Planned:

- [ ] Custom tint colors
- [ ] Blur intensity control
- [ ] Mica material option
- [ ] HostBackdrop support
- [ ] Per-application profiles
- [ ] Accent color synchronization

---

# Screenshots

Screenshots will be added here.

Planned:
assets/

preview-35.png
preview-55.png
explorer.png
settings.png
terminal.png
comparison.gif

---

# Contributing

Suggestions, improvements, and contributions are welcome.

Feel free to open an issue or submit a pull request.

---

# Credits

Created by:

Samuel Mwaandwe

Powered by:

- Windhawk
- Windows Composition API

---

# License

MIT License

See the LICENSE file for details.

Example:
