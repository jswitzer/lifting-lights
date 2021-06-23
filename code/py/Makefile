

run: .venv lifting_lights.so
	local/run.sh

clean: FORCE
	rm -rf .venv
	rm -rf lifting_lights_c.so

lifting_lights.so: 
	make -C ../ py/lifting_lights_c.so

.venv: FORCE
	local/setup.sh

install: .venv lifting_lights.so
	rm -rf /opt/lifting_lights
	mkdir -p /opt/lifting_lights
	cp -r * /opt/lifting_lights/
	/opt/lifting_lights/local/setup.sh

FORCE: 
