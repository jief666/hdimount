# hdimount [![Build Status](https://travis-ci.com/jief666/hdimount.svg?branch=wip)](https://travis-ci.com/jief666/hdimount?branch=wip) [![Build Status](https://ci.appveyor.com/api/projects/status/github/jief666/hdimount?branch=wip)](https://ci.appveyor.com/project/jief666/hdimount)<p/> The ultimate drive and disk images mounter for Linux, Windows and Mac.

Mount drives and apple disk images (HFS, APFS, partitioned or not, compressed, encrypted) via Fuse (or Dokany on Windows).

Read only access only.

### How to use

**hdimount** [fuse args] \<file or physical device\> \<mount-point\>

Interesting fuse arg is -f to stay in foreground.
