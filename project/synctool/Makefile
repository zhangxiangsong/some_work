#This is father fat makefile.

DIRS=clientsync daemon

linux:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE)) || exit 1; \
	done
	cp -f ./daemon/synctool.sh /etc/init.d/synctool
clean:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) clean) || exit 1; \
	done
	rm -f  ./bin/synctool
	rm -f  ./bin/sync_daemon
	rm -f  ./bin/server_begin
	rm -f  ./bin/server_end
	rm -rf ./bin/log