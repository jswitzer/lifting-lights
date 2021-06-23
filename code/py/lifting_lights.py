import lifting_lights_c

def test():
    print "Running Shared Unit Tests"
    print lifting_lights_c.s_test()
    print "Running Base Unit Tests"
    print lifting_lights_c.b_test()
    print "Running Remote Unit Tests"
    print lifting_lights_c.r_test()

def loop(ms_delta):
    lifting_lights_c.loop(ms_delta)

class Remote(object):
    def __init__(self, position):
        assert(position in ['LEFT', 'RIGHT', 'HEAD'])
        self._id = lifting_lights_c.r_ctx_new(position)

    def get_lights(self):
        return lifting_lights_c.r_get_lights(self._id)
        
    def keypress(self, button):
        return lifting_lights_c.r_keypress(self._id, button)

class Base(object):
    def __init__(self):
        self._id = lifting_lights_c.b_ctx_new()

    def get_lights(self):
        return lifting_lights_c.b_get_lights(self._id)

    def keypress(self, button):
        return lifting_lights_c.b_keypress(self._id, button)
