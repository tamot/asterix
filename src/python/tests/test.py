#!/usr/bin/python

import asterix


def callback(arg):
	print(arg)

asterix.set_callback(callback)
asterix.hello("World")
#asterix.init("/home/damir/asterix_python2/asterix/install/config/asterix.ini")
#asterix.parse("/home/damir/asterix_python2/asterix/install/sample_data/cat_062_065.pcap")