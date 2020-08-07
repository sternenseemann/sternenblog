include config.mk

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

sternenblog.cgi: xml.o entry.o index.o cgiutil.o $(TEMPLATE).o main.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: core.h config.h
	$(CC) $(CFLAGS) -c -o main.o main.c

$(TEMPLATE).o: $(TEMPLATE).c core.h config.h xml.h cgiutil.h
	$(CC) $(CFLAGS) -I$(ROOT_DIR) -c -o $@ $<

# only invoked if config.h does not exist
config.h:
	$(CP) config.example.h config.h

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -rf doc/html
	$(RM) -rf doc/man/man3
	$(RM) -f sternenblog.cgi
	$(RM) -f $(TEMPLATE).o
	$(RM) -f *.o
	$(RM) -f assets/favicon.ico

doc:
	doxygen Doxyfile

install: assets/favicon.ico assets/sternenblog.css doc sternenblog.cgi
	$(INSTALL) -Dm755 sternenblog.cgi --target-directory $(PREFIX)$(WEB_PATH)/
	$(INSTALL) -Dm644 assets/sternenblog.css --target-directory $(PREFIX)$(WEB_PATH)/
	$(INSTALL) -Dm644 assets/favicon.ico --target-directory $(PREFIX)$(WEB_PATH)/
	$(INSTALL) -Dm644 doc/man/man3/*.3 --target-directory $(PREFIX)$(MAN_PATH)/man3/
	$(INSTALL) -d $(PREFIX)$(DOC_PATH)
	$(CP) -r doc/html $(PREFIX)$(DOC_PATH)/html
	$(CP) -r doc/examples $(PREFIX)$(DOC_PATH)/examples

assets/favicon.ico:
	$(CONVERT) \
		-size 64x64 \
		canvas:black \
		-font "Noto-Emoji" \
		-pointsize 32 \
		-fill white \
		-draw "text -5,64 '⭐'" \
		-font "Noto-Sans" \
		-pointsize 32 \
		-fill white \
		-draw "text 30,64 'b'" \
		$@

.PHONY: clean doc install
