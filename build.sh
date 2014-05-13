apt-get build-dep pidgin
tar -xvjf pidgin-2.10.9.tar.bz2
ln -s `pwd`/hubbub-pidgin.c pidgin-2.10.9/libpurple/plugins/hubbub-pidgin.c
cd pidgin-2.10.9
./configure
make
cd libpurple/plugins/
make hubbub-pidgin.so
cd ../../..
cp pidgin-2.10.9/libpurple/plugins/hubbub-pidgin.so ./
