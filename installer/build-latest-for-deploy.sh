oldpath=$PATH
QTVER=4.6.3
QT_INTEL=/usr/local/Trolltech/Qt-$QTVER-intel
QT_UNIVERSAL=/usr/local/Trolltech/Qt-$QTVER-universal
export CVSROOT=:pserver:ian@tsunami:/home/sitedata/cvsroot

PATH=$QT_INTEL/bin:$oldpath
buildid=OSX-`uname -m`-`date +%Y%m%d`

rm -rf yasst-$buildid
mkdir yasst-$buildid
rm -rf Yasst.app
rm -rf yasst
cvs export -D"`date`" yasst > /dev/null || exit 1
cd yasst
for thing in *.ui;do echo ---- $thing;cat $thing | sed s/font-size:8pt/font-size:12pt/g | sed "s/MS Shell Dlg 2/Lucida Grande/g"> $thing.new;mv $thing.new $thing;done
qmake -config release 
time make -j12 >/dev/null || exit 1
mv Yasst.app ..
cd ..
mv yasst src
mv src yasst-$buildid
cp -R Yasst.app yasst-$buildid/
cd yasst-$buildid
cp src/Changes.txt .
cp src/ReadMe.txt .
cp -R src/installer/installfiles/examples .
ftp -o Qt_licence.txt http://www.gnu.org/licenses/gpl.txt
rm -rf src
macdeployqt Yasst.app
cd ..
tar -cjf yasst-$buildid-intel.tbz yasst-$buildid
buildid=OSX-`uname -m`-`date +%Y%m%d`
rm -rf yasst-$buildid
mkdir yasst-$buildid
rm -rf Yasst.app


cvs export -D"`date`" yasst >/dev/null || exit 1
PATH=$QT_UNIVERSAL/bin:$oldpath
cd yasst
for thing in *.ui;do echo ---- $thing;cat $thing | sed s/font-size:8pt/font-size:12pt/g | sed "s/MS Shell Dlg 2/Lucida Grande/g"> $thing.new;mv $thing.new $thing;done
echo CONFIG+=x86 ppc >> Ya_st.pro
qmake -config release
time make -j12 >/dev/null || exit 1
mv Yasst.app ..
cd ..
mv yasst src
mv src yasst-$buildid
cp -R Yasst.app yasst-$buildid/
cd yasst-$buildid
cp src/Changes.txt .
cp src/ReadMe.txt .
cp -R src/installer/installfiles/examples .
ftp -o Qt_licence.txt http://www.gnu.org/licenses/gpl.txt
rm -rf src
macdeployqt Yasst.app
cd ..
tar -cjf yasst-$buildid-universal.tbz yasst-$buildid
#./macdeployqt Yasst.app -dmg && rm -r Yasst.app
