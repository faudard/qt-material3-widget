# Qt Compatibility Contract v1

| Family | Floor | Gate |
|---|---:|---|
| Qt5 | 5.14.2 | Windows x64 exact 5.14.2 |
| Qt6 | 6.4.0 | Linux floor-family + pinned Windows/macOS |

Qt5 hosted:
- archive `win64_msvc2017_64`
- runner `windows-2022`
- v142/14.29, cl 19.29

Qt5 reference:
- MSVC 14.28, cl 19.28

Static lint is supportive; a real failed compile always wins.
