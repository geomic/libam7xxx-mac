(TODO: Just notes for now - and needs tidying-up!)

= libam7xxx-mac

Simply a name change and a few fixes to make the excellent libam7xxx from the
awesome Antonio Ospite compile on Mac OS X Mavericks (possibly others).

= Compiling on Mac OS X Mavericks

(Make sure you have homebrew installed on Mac OS X)

brew install cmake
brew install ffmpeg
brew install doxygen
brew install libusb

wget http://xcb.freedesktop.org/dist/xcb-proto-1.10.tar.gz
tar -zxvf xcb-proto-1.10.tar.gz
cd xcb-proto-1.10
./configure && make && sudo make install

wget http://xcb.freedesktop.org/dist/libpthread-stubs-0.3.tar.gz
tar -zxvf libpthread-stubs-0.3.tar.gz
cd libpthread-stubs-0.3
./configure && make && sudo make install

wget http://xorg.freedesktop.org/releases/individual/proto/xproto-7.0.25.tar.bz2
tar -zxvf xproto-7.0.25.tar.bz2
cd xproto-7.0.25
./configure && make && sudo make install

wget http://xorg.freedesktop.org/releases/individual/lib/libXau-1.0.8.tar.bz2
tar -zxvf libXau-1.0.8.tar.bz2
cd libXau-1.0.8
./configure && make && sudo make install

wget http://cgit.freedesktop.org/xcb/libxcb/snapshot/libxcb-1.10.tar.gz
tar -zxvf libxcb-1.10.tar.gz
cd libxcb-1.10
./configure && make && sudo make install

git clone git@github.com:geomic/libam7xxx-mac.git
cd libam7xxx
mkdir build
cd build
cmake ../
make

You will get 4 warnings... but it should all work and get you to 100%.

If you want am7xxx-play available everywhere, you can then issue:
sudo make install

Close and re-open your Terminal window for changes to be picked up. Try running am7xxx-play and voila!

Now for the tricky bit...

Mac OS X will insist on seeing the projectos as a Mass Storage Device. In Linux, that's where you'd normally use sudo am7xxx-modeswitch but this isn't working for us (libusb/Mac OS issue here: http://www.libusb.org/ticket/89 - anyone willing to test the suggetions here: http://stackoverflow.com/questions/20253350/unable-to-claim-usb-interface-with-c-libusb-on-mac-os-x or here: http://www.draisberghof.de/usb_modeswitch/bb/viewtopic.php?p=209#209 ?).

What you can do is the below:
1) Install VirtualBox and a Ubuntu guest VM (e.g. 14.04)
2) Install usb-modeswitch using: sudo apt-get update && sudo apt-get install usb-modeswitch
3) Plug the projector (I tried C110) in VirtualBox go to "Devices", "USB Devices" and select "actions Usb Device [0100]". You may need to repeat this step until you see that "actions Usb Device [0100]" has a tick-box in front of it in "USB Devices".
4) Then change the device mode by issuing: sudo usb_modeswitch   --default-vendor 0x1de1   --default-product 0x1101   --message-content 55534243087052890000000000000cff020000000000000000000000000000
5) Go to the same menu on VirtualBox and click on the device again to de-select it this time. Make sure there is no tick-box, and you're done.
6) You can now run am7xxx-play on the Mac! The device should have remained connected and recognised as a Generic Display Device instead.

I've also noticed that after the first time, the device seems to revert to Generic Display Device when it's mounted (twice) in the Ubuntu VM, which means all you have to do is mount-mount-unmount without needing to run usb_modeswitch again.

= libam7xxx

libam7xxx is an Open Source library to communicate via USB with projectors and
Digital Picture Frames based on the Actions Micro AM7XXX family if ICs.

libam7xxx makes it possible to use these devices as USB displays on
non-Windows Operating Systems like GNU/Linux or Android/Linux just to name
a few, and on non-PC platforms like for instance mobile phones, tablets or
game consoles.

The first driver supporting these devices was _acerc11xdrv_ from Reto Schneider:
https://github.com/rettichschnidi/acerc11xdrv

Another project with the same purpose is _acerc110_ from Thomas Baquet:
https://github.com/lordblackfox/acerc110

== AM7XXX

An overview of some AM7XXX chips (AM7211A AM7212 AM7212P AM7318 AM7213D
AM7213V) can be found in http://www.actions-micro.com/dpf_psg.pdf

An example of datasheet illustrating the functionalities of such ICs (AM7531)
is at: http://wenku.baidu.com/view/543c8b4d852458fb770b56ee.html

== Supported devices

The devices supported by libam7xxx use USB bulk transfers and a simple packet
based protocol to exchange data and commands to and from a host system and the
AM7XXX chip.

Devices based on Actions Micro/Action Media designs generally use the USB
VendorID +1de1+, and they can have two operational modes:

 - USB Mass Storage mode (e.g. ProductID: +1101+)
 - Generic bulk data transfer mode, or Display mode (e.g. ProductID: +c101+)

On recent GNU/Linux systems the switch from Mass Storage mode to Display mode
is performed automatically by
http://www.draisberghof.de/usb_modeswitch/[USB_ModeSwitch], or can be done
manually with the command:

 $ sudo usb_modeswitch \
   --default-vendor 0x1de1 \
   --default-product 0x1101 \
   --message-content 55534243087052890000000000000cff020000000000000000000000000000

Alternatively, on systems where libusb works but 'usb-modeswitch' is not
easily available, the switch can be performed using the 'am7xxx-modeswitch'
example program from libam7xxx.

Examples of devices based on AM7XXX are:

  - Acer Series C pico projectors (C20, C110, C112):
      * http://www.acer.it/ac/it/IT/content/models/projector-c
      * http://support.acer.com/product/default.aspx?modelId=3888

  - Philips/SagemCom PicoPix projectors (PPX 1020, PPX 1230, PPX 1430, PPX
    1630, PPX 2055, PPX 2330):
      * http://www.philips.co.uk/c/pocket-projector/179840/cat/
      * http://www.sagemcom.com/EN/products/image-sound/pico-video-projectors.html

  - CEL-TEC MP-01:
      * http://www.kabelmanie.cz/miniprojektor-cel-tec-mp-01/

  - Top-Height/TEC PP700
      * http://www.ishopiwin.com/en/appliances-electronics/electronics/projectors/pico-projector-pp-700.html

  - Royaltek PJU-2100:
      * http://www.royaltek.com/index.php/pju-2100-pico-projector
  
  - Aiptek PocketCinema T25:
      * http://www.aiptek.eu/index.php/en/products/pico-projectors/pocketcinema-t25

  - Other unbranded projectors:
      * http://www.dealextreme.com/p/portable-home-office-mini-usb-2-0-lcos-projector-16-9-45019

  - HannSpree digital picture frames (but it has not been verified yet if
    those can be actually used as USB displays):
      * http://europe.hannspree.net/onweb.jsp?prod_no=3333333621&webno=3333333317
      * http://europe.hannspree.net/onweb.jsp?prod_no=33333337:4&webno=3333333317

Maybe other devices reported as supporting "Display over USB (DoUSB)" like
Acer K330 or some Optoma projectors could be used with this library, but
this needs still needs to be verified.

== Testing libam7xxx on MS Windows

All the needed files need to be in the same location:
  
  - 'libusb-1.0.dll' from http://sourceforge.net/projects/libusbx/files/releases/1.0.11/Windows/libusbx-1.0.11-win.7z

  - 'libssp-0.dll' from MinGW;

  - 'am7xxx-modeswitch.exe', 'libam7xxx.dll' and 'picoproj.exe' which can all
    be built by following the instructions in the HACKING.asciidoc document
    from libam7xxx.

  - 'am7xxx-play.exe' and the '.dll' files from the 'usr/bin/' dir in libav-win32:
    http://win32.libav.org/win32/libav-win32-20120521.7z

In order to use the device on MS Windows the WinUSB drivers must be installed
for both the mass storage device and the display device:

  - Download http://sourceforge.net/projects/libwdi/files/zadig/[Zadig], it is
    a tool to install and replace USB devices filter drivers on MS Windows.

  - From Zadig, select the USB Mass Storage Device relative to the projector
    and replace the +USBSTOR+ driver with the +WinUSB+ one; keep in mind that
    from now on the virtual CD-ROM can't be accessed anymore until the
    +USBSTOR+ Driver is restored.

  - Run 'am7xxx-modeswitch.exe'

  - When the new (display) device shows up, run Zadig and install the +WinUSB+
    driver for it too.

Now it is possible to run 'picoproj.exe' or 'am7xxx-play.exe' on Windows.
