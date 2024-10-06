@all: ./builddir/main.exe

./builddir/main.exe: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

run: ./builddir/main.exe
	./builddir/main.exe example.yaml

install: ./builddir/main.exe
	install -d $(DESTDIR)/usr/include
	install ./include/ts-yaml.h $(DESTDIR)/usr/include
	install -d $(DESTDIR)/usr/lib64
	install ./builddir/libts-yaml.so $(DESTDIR)/usr/lib64
	install -d $(DESTDIR)/usr/share/pkgconfig
	install ts-yaml.pc $(DESTDIR)/usr/share/pkgconfig
