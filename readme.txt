ds2sdk

/******************************************************************************/
1. information
2. install
3. using
4. bug and improve
/******************************************************************************/

1.informaiton
The ds2sdk are collection of GCC compiler, ds2 hardware layer library, some open
source library and a example, are released to help devopling program run on DSTWO.
the directory including following files:
	include: header files of the library
	lib: library file lay in it
	libsrc: some open source code may be used, such as file system, console
	example: a example explaning how to applying the library
	specs: some special file in this directory, such as link script file
	gcc: GCC compiler in this directory
	tools: a tool converting the plane binary formate execuable file to the
		formate	that can run on DSTWO
	other files

2. install
Linux operation system are recomended to build the cross-comiling evironment.just
tar the compressed file in gcc directory to the work directory you wanted, then
setup the enviroment path of GCC. for example, the work directory is /opt, ds2sdk
directory on ~/, OS is unbuntu, commands as following:

sudo mkdir -p /opt
cd /opt
sudo tar xjf ~/ds2sdk/gcc/mipsel-4.1.2-nopic.tar.bz2
export PATH=$PATH:/opt/mipsel-4.1.2-nopic/bin

Prefix of the cross-compiler is "mipsel-linux-". after setup compiler, then
entering tools directory and allow the makeplug to be executable.

In fact, enter ds2sdk directory, just: make. the makefile will decompress 
compiler to /opt directory and compiling the open source library libds2a.a and
example files.

3. using
review ds2sdk/example/readme.txt for more informaiton.

4. bug and improve
	reporting bugs and improving advise are welcomed.

mail: dking024@gmail.com

license:

ds2sdk
Copyright (C) 2010 dking <dking024@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

Some source code with this sdk belong to the source's author, copyright 
information are in the source files.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

