# stlib
import logging
import threading
import time
import json
import sys

# pip
from flask import Flask
from flask import render_template

# local modules
import lifting_lights

app = Flask(__name__)
log = logging.getLogger(__name__)
class TestDisplay(object):
    def __init__(self):
        self.lights = {}
        self.button_presses = []
        self.remote_l = lifting_lights.Remote('LEFT')
        self.remote_h = lifting_lights.Remote('HEAD')
        self.remote_r = lifting_lights.Remote('RIGHT')
        self.base = lifting_lights.Base()
        self.in_process = False
        self.last_processed = time.time()

    def __str__(self):
        return "[Display]"

    def get_lights(self):
        return self.base.get_lights()

    def press_button(self, button):
        log.info("%s button pressed: %s", self, button)
        self.button_presses.append(button)

    def process(self):
        # For flask reloader
        if self.in_process:
            return
        self.in_process = True

        while(True):
            if len(self.button_presses):
                button = self.button_presses.pop(0)
                try:
                    ignore, where, what = button.split('_')
                    if where == 'left':
                        actor = self.remote_l
                    elif where == 'head':
                        actor = self.remote_h
                    elif where == 'right':
                        actor = self.remote_r
                    elif where == 'base':
                        actor = self.base
                    else:
                        actor = None
                    what = what.upper()
                    if actor and what in ['RESET', "GOOD", "BAD"]:
                        actor.keypress(what)
                except:
                    log.exception("%s: Failure during button press", self)

            try:
                lifting_lights.loop(int(1000*(time.time() - self.last_processed)))
                self.last_processed = time.time()
            except:
                log.exception("%s: Failure during loop", self)
            time.sleep(.01)

display = TestDisplay()


@app.route('/lights')
def get_light_status():
    return json.dumps({'lights':display.get_lights()})

@app.route('/button/<button_id>', methods=['POST'])
def button_pushed(button_id):
    display.press_button(button_id)
    return json.dumps({'lights':display.get_lights()})

@app.route('/')
def serve_html():
    return render_template('index.html')

if __name__ == '__main__':

    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    logging.getLogger('').addHandler(ch)
    logging.getLogger('').setLevel(logging.INFO)

    background_thread = threading.Thread(target=display.process)
    background_thread.daemon = True
    background_thread.start()

    if len(sys.argv) > 1 and sys.argv[1] == 'server':
      log.info("Running as server")
      app.run(debug=False, host='0.0.0.0')
    else:
      log.info("Running as local debug")
      app.run(debug=True)


