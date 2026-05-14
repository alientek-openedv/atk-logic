# Building ATK-Logic on Linux

> ⚠️ The upstream project is officially Windows-only. This document records what
> was needed to produce a working binary on Arch Linux (kernel 7.0, glibc 2.x,
> gcc 16). It is not officially supported by ALIENTEK.

Tested on:

- Arch Linux
- Qt 5.15.18 (`qt5-base`, `qt5-declarative`, `qt5-quickcontrols`,
  `qt5-quickcontrols2`, `qt5-graphicaleffects`, `qt5-x11extras`, `qt5-tools`)
- Python 3.14, glib 2.88, libusb 1.0.29, gcc 16.1.1

## 1. System packages

```bash
sudo pacman -S \
  qt5-base qt5-declarative qt5-quickcontrols qt5-quickcontrols2 \
  qt5-graphicaleffects qt5-x11extras qt5-tools \
  libusb glib2 python autoconf automake libtool pkgconf
```

## 2. Third-party sources

The build needs two repositories that are **not** in this tree:

```bash
mkdir -p third_party && cd third_party

git clone https://github.com/alientek-openedv/atk_libsigrokdecode.git
git clone --recurse-submodules https://github.com/buzzySmile/qBreakpad.git
```

### 2a. Patch & build `qBreakpad`

Google Breakpad's bundled source has not kept up with modern glibc/gcc.
Two small patches:

`third_party/qBreakpad/third_party/breakpad/src/client/linux/handler/exception_handler.cc`

```diff
-  static const unsigned kSigStackSize = std::max(16384, SIGSTKSZ);
+  static const unsigned kSigStackSize = std::max<size_t>(16384, SIGSTKSZ);
```

`third_party/qBreakpad/third_party/breakpad/src/client/linux/handler/minidump_descriptor.h`

```diff
 #include <assert.h>
+#include <stdint.h>
 #include <sys/types.h>
```

Then:

```bash
cd third_party/qBreakpad
qmake-qt5 && make -j$(nproc)
# produces handler/libqBreakpad.a
```

### 2b. Patch & build `atk_libsigrokdecode`

The library marks internal symbols with `__attribute__((visibility("hidden")))`,
but the `atk_decoder.c` shim — which is compiled outside the library and linked
against it — calls those internal symbols. Make them visible:

`third_party/atk_libsigrokdecode/libsigrokdecode.h`:

```diff
-#if defined _WIN32
-#  define SRD_PRIV /* EMPTY */
-#else
-#  define SRD_PRIV __attribute__((visibility("hidden")))
-#endif
+#define SRD_PRIV /* EMPTY */
```

Build:

```bash
cd third_party/atk_libsigrokdecode
sh ./autogen.sh
mkdir -p _build && cd _build
../configure --prefix=$PWD/../_install
# Remove the -fvisibility=hidden flag autoconf inserted:
sed -i 's| -fvisibility=hidden||' Makefile
make -j$(nproc)
```

### 2c. Build `atk_decoder.o` separately

`atk_decoder.c` cannot use the project's precompiled header (`stdafx.h` is C++).
Compile it as a standalone object that the main qmake build then links in:

```bash
cd third_party/atk_libsigrokdecode
gcc -c -fPIC \
    $(pkg-config --cflags glib-2.0 python3-embed) \
    -I. -o atk_decoder.o atk_decoder.c
```

## 3. Build ATK-Logic

```bash
mkdir -p build && cd build
qmake-qt5 ../ATK-Logic.pro
make -j$(nproc)
```

Result: `build/ATK-Logic` (about 22 MB). The path to `libsigrokdecode.so` is
embedded via `QMAKE_RPATHDIR`, so no `LD_LIBRARY_PATH` is needed when running
from this directory.

## 4. Run

```bash
ln -sfn ../runtime/decoders decoders     # protocol decoders the app loads at runtime
QT_QPA_PLATFORM=xcb ./ATK-Logic
```

- The symlink lets the app find the 183 PD modules under `runtime/decoders/`
  (it expects them next to the binary).
- `QT_QPA_PLATFORM=xcb` is recommended on Wayland: the frameless window code
  calls Wayland-unsupported APIs (`QWindow::requestActivate`,
  `setWindowOpacity`) and spams the console otherwise.

## Notes

### `APP_VERSION_NUM` was bumped to 1199

The original source (`2024.5.13`) hard-codes `APP_VERSION_NUM=1066`. Recent
FPGA firmware reports a required minimum of 1108 and the app rejects itself
with "软件版本过低，请升级软件". Bumping the constant lets the source build
talk to current devices. Real fix would be releasing a newer source snapshot.

### Why these patches are not upstream-friendly

- `qBreakpad` / Google Breakpad changes belong upstream in those repos.
- `SRD_PRIV` change in `atk_libsigrokdecode` weakens a deliberate visibility
  decision — the cleaner fix is to add `atk_decoder.c` to the library's own
  `Makefile.am` so it sees internal symbols at compile time.
