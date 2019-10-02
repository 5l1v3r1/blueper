  Blueper

Copyright (C) 2009 Shadow Cave LLC

Written 2009 by JP Dunning (.ronin)
ronin@shadowcave.org
<www.hackfromacave.com>
 


  License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation;

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY 
CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS, 
COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS SOFTWARE IS 
DISCLAIMED.



  About

Blueper is a tool designed to abuse Bluetooth file transfering. It uses 
ussp-push to tranfer files from one device to another. The design of this 
tool provides several possable end results. One is an anoyence of continual 
popups of file trasfer requests on the remote device. Another, is to write 
data to a remote device disk without user interaction. It can even lock up 
or crash some devices.



  Install

NOTE: Blueper requires the use of ussp-psuh.  Before installing blueper make sure
ussp-push is installed on your system.  If it is not you can install it by 
following the instruction in "ussp-push-0.11/install".

1. To install blueper run "make".
2. Run "make install" to install on system.



  Help

NAME
   blueper

SYNOPSIS
   blueper [-i iterations] [options] [file] [target]

DESCRIPTION
   [btaddr]             Address of the target device.  Format: XX:XX:XX:XX:XX:XX
   -a                   Push file to all devices in range
   -c [channel]         Bluetooth OBEX channel.  Default is system selected
   -e                   Add counter to the end of the remote file name
   -h                   Help
   -i [iterations]      Number of times to upload file
   -l [local_file]      Local file to be uploaded to the remote device
   -n [temporary_file]  Name and location to create temporary file for sending
   -r [remote_name]     Remote file name
   -s [size]            Size of the file to be generated and uploaded


 Usage

Attack a specific device with the file "my_file" twenty times:
   > blueper -i 20 -l my_file -r your_file 00:11:22:33:44:55

Attack on all device in range with generated file 1,000 bytes in size:
   > bluper -a -e -i 20 -n tempfile -r your_file -s 1000

