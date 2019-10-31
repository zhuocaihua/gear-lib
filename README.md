# Gear-Lib

English | [简体中文](README.cn.md)

[![Build](https://travis-ci.org/gozfree/gear-lib.svg?branch=master)](https://travis-ci.org/gozfree/gear-lib)
[![Release](https://img.shields.io/github/release/gozfree/gear-lib.svg)](https://github.com/gozfree/gear-lib/releases)
[![License](https://img.shields.io/github/license/gozfree/gear-lib.svg)](https://github.com/gozfree/gear-lib/blob/master/LICENSE.MIT)

This is a collection of basic libraries.
* All are written in POSIX C, aim to used compatibility on x86, arm, android, ios.
* Aim to reuse for IOT, embedded and network service development

## Data Struct
* libdict: Hash key-value dictonary library
* libhash: Hash key-value library based on hlist from kernel
* libringbuffer:
* libqueue: queue library, support memory hook
* librbtree: comes from linux kernel rbtree.
* libsort:
* libvector:
* libmacro: Basic Macro define library, include kernel list and so on

## Network
* librtsp: Real Time Streaming Protocol server for ipcamera or NVR
* librtmp: Real Time Messaging Protocol client for liveshow
* libskt: socket warpper api for easily use
* librpc: Remote Procedure Call library
* libipc: Inter-Process Communication, support mqueue/netlink/shm
* libp2p: High level p2p punch hole library, easy API to use

## Async
* libgevent: Reactor event, like libevent
* libthread: Thread wrapper
* libworkq: Work queue in userspace

## I/O
* libbase64: Base64/32 encode/decode
* libconfig: Support ini/json
* liblog: Support console/file/rsyslog
* libfile: File operations
* libstrex:
* libsubmask:

## Misc
* libdebug: Help to trace crash like gdb
* libhal: hardware Abstraction Layer
* libplugin: Dynamic link plugin
* libtime: Time wrapper
* libposix4win: posix adapter for Windows

## Muiti-Media
* libuvc: USB video class V4L2/dshow
* libmp4parser: MP4 format parser
* libjpeg-ex:

## How To Build
Please refer to [INSTALL.md](https://github.com/gozfree/gear-lib/blob/master/INSTALL.md) file for detailed information.

## License
Please refer to the [LICENSE](https://github.com/gozfree/gear-lib/blob/master/LICENSE.MIT) file for detailed information.

## Contacts
* Email: gozfree@163.com
* QQ Group: 695515645
* Github: [gear-lib](https://github.com/gozfree/gear-lib)
* Gitee: [gear-lib](https://gitee.com/gozfreee/gear-lib)
