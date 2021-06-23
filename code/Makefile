REMOTE_FILES = remote.c arduino_remote.ino remote.h shared.c shared.h
BASE_FILES = base.c arduino_base.ino base.h shared.c shared.h
XBEE_LIBS = xbee-arduino/XBee.cpp xbee-arduino/XBee.h
ARDUINO_CC ?= ino
PYTEST_LIB ?= lifting_lights_c.so
PYTEST_SRC ?= pywrapper.c

all: pymod arduino_remote arduino_base

arduino_remote: $(REMOTE_FILES)
	rm -rf arduino_remote
ifeq ($(ARDUINO_CC), ino)
	mkdir -p arduino_remote/src
	ln $(REMOTE_FILES) $(XBEE_LIBS) arduino_remote/src/
	cd arduino_remote; $(ARDUINO_CC) build
else
	mkdir arduino_remote
	cp -r $(REMOTE_FILES) arduino_remote/
	cp $(XBEE_LIBS) arduino_remote/
	"$(ARDUINO_CC)" --verify "$(abspath arduino_remote/arduino_remote.ino)"
endif

arduino_base: $(BASE_FILES)
	rm -rf arduino_base
ifeq ($(ARDUINO_CC), ino)
	mkdir -p arduino_base/src
	ln $(BASE_FILES) $(XBEE_LIBS) arduino_base/src/
	cd arduino_base; $(ARDUINO_CC) build
else
	mkdir arduino_base
	cp -r $(BASE_FILES) arduino_base/
	cp $(XBEE_LIBS) arduino_base
	"$(ARDUINO_CC)" --verify "$(abspath arduino_base/arduino_base.ino)"
endif

clean:
	rm -rf arduino_base arduino_remote build py/$(PYTEST_LIB)
	$(MAKE) -C py clean

py/$(PYTEST_LIB): $(BASE_FILES) $(REMOTE_FILES) $(PYTEST_SRC)
	rm -rf build
	python setup.py build
	cp build/lib.*/$(PYTEST_LIB) py

pymod: py/$(PYTEST_LIB)
	$(info "Build python module")

test: py/$(PYTEST_LIB)
	python py/test.py

pyweb: py/$(PYTEST_LIB)
	$(MAKE) -C py


