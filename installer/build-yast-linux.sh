#!/bin/sh
export QTDIR=/usr/local/Trolltech/Qt-4.6.3
export CVSROOT=:pserver:ian@tsunami:/home/sitedata/cvsroot
export PATH=$QTDIR/bin:$PATH
#set version
rm -rf /tmp/yasst
buildid=`uname`-`uname -m`-`date +%Y%m%d%H%M`
echo Building: $buildid
mkdir /tmp/yasst
cd /tmp/yasst
cvs co yasst > /dev/null
cd yasst
echo Tagging
cvs tag $buildid 
qmake -config release
make -j6 
cp Yasst .. || exit
cd /tmp/yasst
rm -rf yasst

rm -fr yasst-linux
mkdir -p yasst-linux/plugins
cd yasst-linux
cvs export -r $buildid yasst
mv yasst src
cp ../Yasst .
cat > Yasst.sh <<!
#!/bin/sh
appname=\`basename \$0 | sed s,\.sh\$,,\`
dirname=\`dirname \$0\`

if [ "\`echo \$dirname | cut -c1\`" != "/" ]; then
	dirname=\$PWD/\$dirname
fi
LD_LIBRARY_PATH=\$dirname
export LD_LIBRARY_PATH
\$dirname/\$appname \$*
!
chmod +x Yasst.sh
cat > qt.conf <<!QT
[Paths]
plugins=plugins
!QT

cp src/*.txt .
cp -R src/installer/installfiles/examples .
rm -rf src
#wget http://www.gnu.org/licenses/gpl-2.0.txt
wget -O Qt_licence.txt http://www.gnu.org/licenses/gpl.txt
cp $QTDIR/lib/libphonon.so.4 $QTDIR/lib/libQtGui.so.4 $QTDIR/lib/libQtXml.so.4 $QTDIR/lib/libQtNetwork.so.4 $QTDIR/lib/libQtCore.so.4 .
cd plugins
cp -r $QTDIR/plugins/imageformats .
cp -r $QTDIR/plugins/phonon_backend .
rm */*.debug
cd /tmp/yasst
tar -cjf yasst-$buildid.tbz yasst-linux
tar -czf yasst-$buildid.tgz yasst-linux
