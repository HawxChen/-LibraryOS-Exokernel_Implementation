#!/usr/bin/python
import optparse,sys

class Arg_split:
    splittee = optparse.OptionParser()
    def __init__(self, usage):
        self.splittee.add_option("-d", type = "str", default = ".", dest="dir_entry",help="default value is \".\"")
        self.splittee.add_option("-p", type = "str", default = ".S .s .c .h", dest="prop_file",help="default value is \".S .s .c .h\"")
        self.splittee.add_option("-w", type = "str", default = None, dest="keyword")
        self.splittee.add_option("-A", action = "store_true", default = False, dest="ascii", help="-A show colored keyword ")

    def default_split_out(self, _list = None):
        if(None == _list):
            _list = sys.argv[1:]

        opt, args = self.splittee.parse_args(_list)
        if(self.splittee.has_option("-w") and None == opt.keyword):
            print >> sys.stderr, "Wrong input", _list
            exit();
        return opt, args
