# kur_infteh_spring2025
Курсовая работа по информационным технологиям весны 2025 года.

# How to make
1. Install `mingw` by manual: https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain
2. Execute `mingw32-make` in project folder
3. Execute `kur.exe` in `bin` folder

# VS Code
For vsc needed:
- https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
- https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools

Paste to your `settings.json` for C/C++ extension:
```json
"C_Cpp.formatting": "vcFormat",
"C_Cpp.vcFormat.newLine.beforeOpenBrace.block": "sameLine",
"C_Cpp.vcFormat.newLine.beforeOpenBrace.function": "sameLine",
"C_Cpp.vcFormat.newLine.beforeOpenBrace.lambda": "sameLine",
"C_Cpp.vcFormat.newLine.beforeOpenBrace.namespace": "sameLine",
"C_Cpp.vcFormat.newLine.beforeOpenBrace.type": "sameLine"
```