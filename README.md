# 7-Zip on GitHub

7-Zip website: [7-zip.org](https://7-zip.org)

中文文档： [README.zh-CN.md](README.zh-CN.md)

## Overview

This repository is a fork of 7-Zip with several Windows-focused enhancements on top of the upstream codebase.

The source tree mainly includes:

- `CPP/7zip/`: main C++ implementation and Windows application code
- `C/`: low-level codec and utility code
- `Asm/`: hand-written assembly optimizations
- `DOC/`: upstream documentation and format references
- `Lang/`: localization resources

## Custom Changes

### Smart Extract

This fork adds an intelligent extract mode for Windows builds.

- Command line: `7z sx archive.7z`
- Behavior:
  - If the archive expands to a single top-level file or folder, extract directly to the target path.
  - If the archive expands to multiple top-level items, create a folder named after the archive and extract into it.
  - Common multi-volume suffixes such as `.7z.001` and `.part01.rar` are stripped from the created folder name.
- Explorer context menu:
  - Adds `Smart Extract`
  - The entry is placed below `Extract files...`

### Smart Extract and Delete

Same as Smart Extract, but after an archive is extracted successfully, the original archive is moved to the Recycle Bin.

- Command line: `7z sxd archive.7z`
- Explorer context menu: adds `Smart Extract and Delete` (`智能解压并删除`), below `Smart Extract`. It can be toggled on the 7-Zip options page.
- Behavior:
  - The archive is moved to the Recycle Bin (not permanently deleted) only if every item was extracted with no errors and the archive was opened without errors or warnings.
  - For multi-volume archives, all volumes that were used are moved to the Recycle Bin.
  - If extraction fails or reports any error, the archive is kept.
  - If the move to the Recycle Bin fails (for example, path longer than `MAX_PATH`), an error message is shown and the archive is kept.

### Password Book and Bundled Password Plugin

This fork adds a Windows password-book workflow backed by a bundled plugin DLL.

- Runtime files:
  - `7zPasswordPlugins.dll`
  - `7zPasswordBook.db`
- Current public behavior:
  - Stores passwords by archive MD5 in a local SQLite database
  - Lets 7zFM manage the password book from the `Password Book` options page
  - Supports CSV import/export for password-book entries through `7zPasswordBook.csv`
  - Preloads saved passwords for extraction and saves confirmed passwords back to the local password book
  - Adds `Query Password` to the 7zFM context menu
- Notes:
  - The current public build implements local password-book lookup and storage
  - The plugin extension lookup export is reserved, and the public plugin currently returns `E_NOTIMPL`

### Separate Archive for Each File/Folder

This fork adds a GUI compression option that creates one archive per selected input item.

- Compress dialog:
  - Adds `Separate archive for each file/folder`
- Behavior:
  - When multiple files or folders are selected, the GUI creates one archive per selected item
  - Reuses the chosen archive format and compression settings for each generated archive
  - Resolves output name collisions by appending suffixes such as `_2`, `_3`, and so on

### Name Encoding Selector and Auto Detection in 7zFM and CLI

This fork also ports the 7zFM name-encoding selector from [Autori/7zip-codepage](https://github.com/Autori/7zip-codepage).

- File Manager:
  - Adds `Tools -> Name Encoding`
  - Adds an `Auto Detect` mode for archive entry names
  - Supports `UTF-8`, common OEM code pages, and common ANSI Windows code pages
  - Refreshes open panels after switching the selected code page
- Auto detection:
  - Uses the integrated `compact_enc_det` library to detect archive entry name encodings automatically
  - Applies to formats that route through the name code-page detection path, including ZIP and TAR
  - Falls back to internal heuristics when the detector cannot produce a usable result
- Command line:
  - Supports `-mcp=auto` for automatic archive entry name encoding detection on formats that support the `cp` property
  - The `cp` property also accepts `UTF-8`, `WIN`, `DOS`, and numeric code page ids
  - Example: `7z x archive.zip -mcp=auto`
- Runtime behavior:
  - Stores the selected code page or the `auto` mode in the `Z7_FORCE_CODEC` environment variable
  - Applies the selected code page when opening archives in 7zFM
  - Keeps the setting only for the current 7zFM process unless `Z7_FORCE_CODEC` is set outside the app

## Third-Party Licenses

This repository vendors Google's `compact_enc_det` library for archive entry name auto-detection.

- License: Apache License 2.0
- Source license text: [`CPP/Common/CompactEncDet/LICENSE`](CPP/Common/CompactEncDet/LICENSE)
- Third-party notice summary: [`THIRD_PARTY_NOTICES.txt`](THIRD_PARTY_NOTICES.txt)
- The imported `compact_enc_det` source tree contains local modifications for integration and compiler compatibility

The NSIS script decompile build and release automation also references the approach used by [myfreeer/7z-build-nsis](https://github.com/myfreeer/7z-build-nsis).

- Referenced files in this repository:
  - [`.github/workflows/release-windows.yml`](.github/workflows/release-windows.yml)
  - [`.github/scripts/enable-nsis-decompile-build.ps1`](.github/scripts/enable-nsis-decompile-build.ps1)
  - [`.github/scripts/package-windows-installer.ps1`](.github/scripts/package-windows-installer.ps1)
  - [`.github/scripts/package-windows-products.ps1`](.github/scripts/package-windows-products.ps1)
- Upstream project license: LGPL-2.1
- Upstream repository: <https://github.com/myfreeer/7z-build-nsis>
- This repository reimplements the CI flow in PowerShell and GitHub Actions around the same NSIS script decompile enablement idea

## GitHub Actions

Windows builds are published through a manual GitHub Release workflow.

- Runner: `windows-2022`
- Toolchain: Visual Studio 2022 + `nmake`
- Targets: `x64`, `x86`
- Release asset types:
  - Windows installer executables for `x64` and `x86`
  - `all-products.zip`: all produced `.exe`, `.dll`, `.sfx`, language files, and `Licenses/` documentation in one archive for inspection, download, or redistribution
- Asset naming:
  - Normal release assets are named like `7zip-v26.04-windows-all-products.zip`
  - NSIS-enabled release assets are named like `7zip-v26.04-nsis-windows-all-products.zip`

Windows release packaging is defined in [`.github/workflows/release-windows.yml`](.github/workflows/release-windows.yml).

- Trigger: manual `workflow_dispatch`
- Input: `version_tag`, which must be exactly `v<major>.<minor>`, for example `v26.04`
- Behavior:
  - Runs both asset flavors (`standard` and `nsis`) as parallel jobs from a single run
  - Builds `x64` and `x86` Windows binaries
  - Downloads the matching upstream 7-Zip installer skeleton based on the leading `v<major>.<minor>` portion of the tag
  - Replaces the packaged binaries, language files from `Lang/`, and selected distribution documents from `DOC/`
  - Publishes one GitHub pre-release whose tag is exactly `version_tag`, for example `v26.04`
- NSIS flavor notes:
  - Applies a temporary build patch that enables `NSIS_SCRIPT` in `CPP/7zip/Archive/Nsis/NsisIn.h`
  - Removes `-WX` from `CPP/Build.mak` during CI so NSIS script decompile warnings do not fail the build
  - Packages NSIS-enabled installer and zip assets with the `-nsis-` infix in the asset name
  - To verify the feature, test the built `7z.exe` or `7zFM.exe` against a real NSIS installer and confirm that `[NSIS].nsi` is listed or extracted
- Installer payload notes:
  - Includes `Uninstall.exe`
  - Includes `7zPasswordPlugins.dll`, and the bundled uninstaller removes it during uninstall
  - Does not install `Install.exe`
  - Does not install `7zS.sfx`
