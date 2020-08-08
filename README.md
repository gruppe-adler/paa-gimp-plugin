<p align="center">
  <img src="./AdlerGIMP.png">
</p>

# Gruppe Adler PAA GIMP Plugin

paa-gimp-plugin is a GIMP File Format Plugin for opening and saving PAA images in GIMP. For more information on the PAA file format check out the  [public PAA file format specification](https://community.bistudio.com/wiki/PAA_File_Format).

# Installation

**Requires GIMP Version 2.10.20 or higher**

## Windows
- Download the latest `paa-gimp-plugin.exe` from https://github.com/gruppe-adler/paa-gimp-plugin/releases.
- Move the `paa-gimp-plugin.exe` into `C:\Users\<username>\AppData\Roaming\GIMP\<GIMP version>\plug-ins`.


# Features
- `Open`, `Open As` menu commands can be used to read `*.paa` files.
- `Export`, `Export As` menu commands can be used to write `*.paa` files.

# Limitations
- As per the PAA file format specification only images where the height and width is a power of 2 (`2^n`) are supported.
- Only the DXT1/DXT5 PAA Formats are currently supported.

# Dependencies
* libgimp-2.0
* [grad_aff](https://github.com/gruppe-adler/grad_aff/tree/dev)

# Building
For Windows using msys2/mingw-w64 is recommended.
- Build and install libgimp-2.0 and [grad_aff](https://github.com/gruppe-adler/grad_aff/tree/dev)
- `git clone` this repository
- Change into the project directory and run:
```
mkdir build
cd build
cmake ..
make
make install
```