# Making Microsoft Comic Chat DPI-Aware

This document captures everything learned while getting the 1996 Microsoft
Comic Chat MFC client to render correctly on a modern high-DPI Windows
display. It is written for the next person who touches the rendering code, so
it explains *why* the bugs happened, not just *what* changed.

> TL;DR: Comic Chat draws each comic panel into an off-screen DIB and blits it
> to the window. On a high-DPI display the off-screen bitmap was sized from the
> reported DPI (`GetDeviceCaps`) while the on-screen device context actually
> used a *larger* logical→device mapping. `BitBlt` does not scale, so the right
> ~12% of every panel was silently clipped. The fix is to size the bitmap from
> the DC's real `LPtoDP` extent, rebuild it when that extent changes, and blit
> with `StretchBlt` as a safety net.

---

## 1. Background: how Comic Chat draws

- All drawing coordinates are in **TWIPs** (1440 per inch), defined as
  `UNITSPERINCH` in `common.h`. The view runs in `MM_TWIPS` map mode.
- A comic page is a grid of fixed-size **panels** (`CUnitPanelPage`). The panel
  size is `unitWidth × unitHeight` (TWIPs), recomputed from the client area in
  `CPageView::GetProspectivePanelWidth` / `SetPanelsWide`.
- Each panel is rendered **off-screen** first:
  - `CreateRetainedPanel` (in `pageview.cpp`) allocates a **DIB section**
    (`m_retSec` / `m_retDib`) sized to one panel.
  - `CUnitPanelPage::Draw` (in `panel.cpp`) selects that DIB into a memory DC,
    draws the panel (backdrop, avatars, balloons, title) into it, then
    `BitBlt`s it to the destination DC at the panel's on-screen position.
- Text/title layout is measured with a long-lived cached `CClientDC`
  (`GetClientDC()`), created once in `CPageView::OnCreate`.

The title bar of a comic ("MEET MARKET", "VIRTUALLY VACUOUS", …) is a random
pun pulled from `titles.txt`, rendered with the large `m_fontTitle`. Long puns
are the most visible victims of any horizontal clipping.

---

## 2. The two DPI concepts that matter here

Windows exposes display scaling in **two** ways, and Comic Chat's code mixes
them up:

| Concept | API | What it returns |
|--------|-----|-----------------|
| **Reported logical DPI** | `GetDeviceCaps(hdc, LOGPIXELSX/Y)` | A nominal number (e.g. `96` or `144`). |
| **Actual mapping scale** | `LPtoDP` on the DC | The real logical→device conversion the DC will use when it draws/blits. |

For a *pure* `MM_TWIPS` DC these agree: `LPtoDP(1440 twips) == LOGPIXELSX`.
But a `CScrollView` (or anything that sets viewport/window extents) can give a
DC an effective scale that **differs from the reported DPI**. In that case
`GetDeviceCaps` lies relative to what the DC actually draws.

**Comic Chat sized its bitmaps from the reported DPI but drew into them with
the actual mapping scale — and those two disagreed on this machine.**

---

## 3. Symptoms observed

On a 144-DPI (150% scaling) laptop display:

1. **Initial run (no DPI awareness):** the whole app was bitmap-stretched by
   Windows' DPI virtualization. Panels were huge and blurry and the comic
   content was clipped.
2. **After `SetProcessDPIAware()`:** UI chrome (menus, dialogs) rendered crisp
   and correctly sized, and *most* of the comic rendered correctly — but the
   right edge of every panel was still clipped. The clearest tell was the
   random title pun: `YOU SHOULDA BEEN THERE` rendered as `YOU SHOULD‑`
   (the "A" sliced off), while shorter lines like `BEEN THERE` were fine.

The clip was always on the **right/bottom** and was roughly a **fixed
percentage** of the panel — a classic "destination bigger than source" blit
symptom, not a font or word-wrap problem.

---

## 4. How it was diagnosed

The app already emits `TRACE(...)` via the MFC/ATL debug channel, which is
visible in **DebugView (DbgView)**. Temporary traces were added at the exact
draw sites and read back live while connected to `irc.libera.chat`:

1. **Title fit** — `CUnitPanelPage::AddTitle`:
   ```
   TITLEFIT 'YOU SHOULDA BEEN THERE' unitWidth=7365 measuredWidth=6511 ... right=6938
   ```
   The wrapped title measured **6938 < unitWidth 7365**, i.e. layout said it
   fit. So it was *not* a measurement/word-wrap bug.

2. **DC agreement** — `CLabel::Draw`: the drawing DC and the cached client DC
   returned identical text extents (`12266 == 12266`). So it was *not* a
   measure-DC vs draw-DC mismatch either.

3. **Bitmap vs panel** — `CUnitPanelPage::Draw`:
   ```
   PANELBLIT bmpW=737 bmpH=737 unitWidth=7365 dcDpiX=144
   ```
   The bitmap was **737px** (= `7365 / 1440 × 144`).

4. **The smoking gun** — per-line device position in `CLabel::Draw`:
   ```
   TITLELINE line=0 width=6511 rightTwip=6938 rightDev=788 bmpW=737 unitWidth=7365
   ```
   The title's right edge maps to **788px**, but the bitmap is only **737px**
   wide. `788 / 6938 × 1440 ≈ 164` → the DC was effectively drawing at ~164 DPI
   while the bitmap was sized for 144 DPI. The last 51px (the "A") fell off the
   edge of the DIB.

5. **Blit positions** confirmed the same scale on the destination side: panel
   #2's logical x `7509` mapped to device `853px` (pure 144-DPI `MM_TWIPS`
   would give `751px`).

So: **bitmap sized at the reported DPI (144 → 737px); content drawn and panels
positioned at the DC's real scale (~164 → 788px). `BitBlt` copies 1:1, so the
extra ~12% is clipped.**

---

## 5. The fixes

All changes are surgical and live in three files.

### 5a. `chat.cpp` — declare DPI awareness
`CChatApp::InitInstance` calls `SetProcessDPIAware()` (resolved dynamically
from `user32.dll`, since the original 1996 target predates it). This stops
Windows from bitmap-stretching the whole window and lets the app render at
native resolution.

```cpp
HMODULE hUser32 = LoadLibrary("user32.dll");
if (hUser32) {
    typedef BOOL (WINAPI *SetProcessDPIAwareFunc)();
    SetProcessDPIAwareFunc setDPIAware =
        (SetProcessDPIAwareFunc)GetProcAddress(hUser32, "SetProcessDPIAware");
    if (setDPIAware) setDPIAware();
    FreeLibrary(hUser32);
}
```

### 5b. `pageview.cpp` — size the bitmap from the DC's real mapping
`CreateRetainedPanel` now derives the DIB pixel size from the DC's actual
`LPtoDP` extent instead of `GetDeviceCaps(LOGPIXELSX/Y)`:

```cpp
POINT pOrg = { 0, 0 };
POINT pExt = { CUnitPanelPage::unitWidth, CUnitPanelPage::unitHeight };
pDC->LPtoDP(&pOrg);
pDC->LPtoDP(&pExt);
infoHdr->biWidth  = abs(pExt.x - pOrg.x);
infoHdr->biHeight = abs(pExt.y - pOrg.y);
```

This guarantees the bitmap is exactly the device size the same DC will draw and
blit a panel at, regardless of whether the reported DPI matches the mapping.
(Two points are transformed and subtracted so any viewport-origin offset
cancels out.)

### 5c. `pageview.cpp` — rebuild the bitmap when the scale/size changes
`CPageView::OnDraw` used to create the retained section once and keep it
forever. Now it checks the existing bitmap's width against what the current
drawing DC needs and rebuilds if they differ (panel size changes, view-mode
switches, DPI/monitor changes, etc.):

```cpp
if (!pDC->IsPrinting()) {
    POINT po = { 0, 0 };
    POINT pe = { CUnitPanelPage::unitWidth, CUnitPanelPage::unitHeight };
    pDC->LPtoDP(&po);
    pDC->LPtoDP(&pe);
    int needW = abs(pe.x - po.x);
    BOOL needNew = (GetRetSec(pDC) == NULL);
    if (!needNew) {
        BITMAP bm;
        ::GetObject(GetRetSec(pDC), sizeof(bm), &bm);
        if (bm.bmWidth != needW) needNew = TRUE;
    }
    if (needNew) {
        FreeRetainedPanelS();
        CreateRetainedPanel(pDC, &m_retSec, &m_retDib);
    }
}
```

### 5d. `panel.cpp` — blit with `StretchBlt` as a safety net
The panel blit was changed from `BitBlt` to `StretchBlt` so that even if a
residual scale mismatch ever remains, the whole panel bitmap is scaled to
exactly fill the panel slot rather than being clipped:

```cpp
dc->SetStretchBltMode(COLORONCOLOR);
dc->StretchBlt(loc2.x, loc2.y, unitWidth, -unitHeight,
               &memDC, 0, 0, unitWidth, -unitHeight, SRCCOPY);
```

With 5b/5c in place the source and destination sizes match, so `StretchBlt`
behaves like a 1:1 copy in the common case; it only kicks in if the scales ever
diverge again.

---

## 6. Why the "obvious" fixes were wrong

- **"Just shrink the title font."** The title already *measured* as fitting
  (`6938 < 7365`). The font was correctly sized; the bitmap was too small.
- **"It's a measurement DC vs drawing DC mismatch."** Traced and ruled out —
  both DCs returned identical text extents.
- **"Force the app DPI-unaware and let Windows stretch it."** That brought back
  uniform blurriness and still clipped in the original (virtualized) run,
  because the clip is inherent to the bitmap-sizing logic, not to the scaling
  mode.

The only correct lever is to make the **off-screen bitmap size** and the
**on-screen mapping scale** agree, which is exactly what `LPtoDP`-based sizing
does.

---

## 7. Gotchas / things to watch

- **`GetDeviceCaps(LOGPIXELSX)` is not the same as the DC's drawing scale.**
  When a view uses scroll/extent-based mapping, prefer `LPtoDP` to learn the
  real conversion.
- **`BitBlt` never scales.** If the source DIB and the destination logical rect
  imply different device sizes, content is clipped (or garbage is read past the
  end of the source). Use `StretchBlt` when you cannot guarantee equal scales.
- **Long-lived cached DCs** (`GetClientDC()` created in `OnCreate`) can capture
  a stale mapping. If display DPI/monitor changes at runtime, anything sized
  from that cached DC may be wrong. The `OnDraw` size-check (5c) mitigates this
  for the panel bitmap.
- **This is system-DPI awareness, not per-monitor.** `SetProcessDPIAware()`
  fixes the single-display case. Moving the window between monitors of
  different DPI is out of scope; per-monitor v2 awareness would require a
  manifest and `WM_DPICHANGED` handling.
- **TWIPs everywhere.** Keep new drawing code in `MM_TWIPS` and reason about
  device pixels only via `LPtoDP`/`DPtoLP`.

---

## 8. How to verify

1. Build: `nmake /f chat.mak CFG="chat - Win32 Debug"` (see the repo README /
   build notes; ATLMFC spectre libs and `uuid.lib` are required).
2. Run `chat.exe` on a high-DPI display (e.g. 150% scaling).
3. Connect to `irc.libera.chat` (plaintext port 6667 — see note below) and join
   a busy channel so a title/cast panel is generated.
4. Confirm the random title pun renders **in full** (no clipped final letter)
   and that balloons/avatars reach the panel's right edge without being cut.
5. Optional: attach DebugView and confirm there are no panel clip anomalies.

---

## 9. Related, non-DPI notes discovered alongside this work

- **No TLS.** Comic Chat speaks plaintext IRC over `CSocket`; it cannot connect
  to TLS-only ports such as `6697`. Use a local `stunnel`/ZNC bridge if a
  network requires TLS, and point Comic Chat at the local plaintext endpoint.
- **Room-join assertion.** Joining a channel called `SetPathName(channel)`,
  which modern MFC pushes onto the MRU file list, asserting on the non-file
  name. Passing `FALSE` (`SetPathName(channel, FALSE)`) skips the MRU add.
- **Missing TTS wrapper.** `chat.exe` imported `initproc`/`ttsproc`/`exitproc`
  from `lhwrap.dll`, a wrapper around the Lernout & Hauspie text-to-speech
  engine (1996). That DLL is absent, so these are stubbed; speech is a no-op.
