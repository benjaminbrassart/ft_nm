# MERCI JFREMOND ET YMARCILL

Use nm with env variable LANG=C for a sort order that makes sense.

## Executable and Linkable Format (ELF)

### Symbol characters

| Character | Description                                                   |
|-----------|---------------------------------------------------------------|
| `A`       | Absolute symbol                                               |
| `a`       | File name                                                     |
| `B`       | In `.bss` data section (global)                               |
| `b`       | In `.bss` data section (local)                                |
| `C`       | In `.common` section                                          |
| `c`       | In `.common` section (small commons, not supported by x86_64) |
| `D`       | In `.data` (or similar) initialized data section (global)     |
| `d`       | In `.data` (or similar) initialized data section (local)      |
| `N`       | Debug section                                                 |
| `n`       | Read-only + no-exec debug symbol                              |
| `R`       | In `.rodata` (or similar) read-only data section (global)     |
| `r`       | In `.rodata` (or similar) read-only data section (local)      |
| `T`       | In `.text` (or similar) executable code section (global)      |
| `t`       | In `.text` (or similar) executable code section (local)       |
| `U`       | Undefined symbol (defined in another object)                  |
| `u`       | Unique global symbol (GNU extension)                          |
| `V`       | Weak object (global)                                          |
| `v`       | Weak object (local)                                           |
| `W`       | Weak symbol (global)                                          |
| `w`       | Weak symbol (local)                                           |
| `?`       | Not recognized (e.g. file format specific)                    |

## Links

### Documentation

- https://man7.org/linux/man-pages/man1/nm.1.html
- https://man7.org/linux/man-pages/man1/readelf.1.html
- https://man7.org/linux/man-pages/man5/elf.5.html
- https://man7.org/linux/man-pages/man2/mmap.2.html
- https://man7.org/linux/man-pages/man2/fstat.2.html
- https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblj/index.html

### Stack Overflow answers

- https://stackoverflow.com/a/15267767/11212381

### Wikipedia articles

- https://en.wikipedia.org/wiki/Data_segment
- https://en.wikipedia.org/wiki/.bss

### Videos

- [ELF File Format Explained](https://www.youtube.com/watch?v=9uWMr3wdadM)
- [In-depth: ELF - The Extensible & Linkable Format](https://www.youtube.com/watch?v=nC1U1LJQL8o)

### Tools
- [Online ELF Viewer](http://www.sunshine2k.de/coding/javascript/onlineelfviewer/onlineelfviewer.html)
