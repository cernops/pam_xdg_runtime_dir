CC=gcc
CFLAGS="-fPIC -fno-stack-protector"
INSTALLDIR=/usr/lib64/security
MANPATH=/usr/share/man

all: pam_xdg_runtime_dir.so

src/pam_xdg_runtime_dir.o: src/pam_xdg_runtime_dir.c
	gcc $(EXTRA_CFLAGS) -fPIC -fno-stack-protector -c src/pam_xdg_runtime_dir.c -o src/pam_xdg_runtime_dir.o

pam_xdg_runtime_dir.so: src/pam_xdg_runtime_dir.o
	gcc $(EXTRA_CFLAGS) -shared -o pam_xdg_runtime_dir.so src/pam_xdg_runtime_dir.o -lpam -lselinux

install: all
	install -d $(DESTDIR)$(INSTALLDIR)
	install -p -m 0755 pam_xdg_runtime_dir.so $(DESTDIR)/$(INSTALLDIR)/pam_xdg_runtime_dir.so 
	install -d $(DESTDIR)$(MANPATH)/man8
	install -p -m 0644 man/pam_xdg_runtime_dir.8 $(DESTDIR)$(MANPATH)/man8/pam_xdg_runtime_dir.8

clean:
	rm -rf build/ $(TARFILE) src/*.o *.so

