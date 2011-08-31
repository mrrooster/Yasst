#!/usr/local/bin/bash

buildid=`date +%Y%m%d`
echo Building: $buildid

if [ -e yasst ]; then
	echo Please remove/delete yasst directory first.
	exit 1
fi
cvs export -D"`date`" yasst >/dev/null 2>&1

wget -O yasst/COPYING http://www.gnu.org/licenses/gpl.txt

rm -rf yasst/installer
rm yasst/*vcproj*

for thing in `find yasst -name "*.h" -o -name "*.cpp"`;do
	mv $thing $thing.orig
	cat <<!EOL > $thing 
/*
(c) 2009,2010 Ian Clark

This file is part of Yasst.

Yasst is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Yasst is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Yasst.  If not, see <http://www.gnu.org/licenses/>.

*/
!EOL
	cat $thing.orig >> $thing
	rm $thing.orig

done

tar -cjf yasst-$buildid.tbz yasst
rm -rf yasst
