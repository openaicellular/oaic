#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Multi UE Flowgraph
# GNU Radio version: 3.8.1.0

from gnuradio import blocks
#import blocks
from gnuradio import gr
#import gr
from gnuradio.filter import firdes
#from filter import firdes
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
#from eng_arg import eng_float, intx
from gnuradio import eng_notation
#import eng_notation
from gnuradio import zeromq
#import zeromq

class multi_ue(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Multi UE Flowgraph")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 23.04e6
        self.min_gain = min_gain = 0
        self.max_gain = max_gain = 1

        ##################################################
        # Blocks
        ##################################################
        self.zeromq_req_source_1 = zeromq.req_source(gr.sizeof_gr_complex, 1, 'tcp://localhost:2000', 100, False, -1)
        self.zeromq_req_source_0_0 = zeromq.req_source(gr.sizeof_gr_complex, 1, 'tcp://localhost:2007', 100, False, -1)
        self.zeromq_req_source_0 = zeromq.req_source(gr.sizeof_gr_complex, 1, 'tcp://localhost:2010', 100, False, -1)
        self.zeromq_rep_sink_1_0 = zeromq.rep_sink(gr.sizeof_gr_complex, 1, 'tcp://*:2006', 100, False, -1)
        self.zeromq_rep_sink_1 = zeromq.rep_sink(gr.sizeof_gr_complex, 1, 'tcp://*:2008', 100, False, -1)
        self.zeromq_rep_sink_0 = zeromq.rep_sink(gr.sizeof_gr_complex, 1, 'tcp://*:2009', 100, False, -1)
        self.blocks_throttle_0_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_multiply_const_vxx_0_1 = blocks.multiply_const_cc(0.05)
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_cc(0.08)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_cc(0.08)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(0.05)
        self.blocks_add_xx_0 = blocks.add_vcc(1)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_throttle_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.zeromq_rep_sink_1_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_1, 0), (self.zeromq_rep_sink_1, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_multiply_const_vxx_0_1, 0))
        self.connect((self.blocks_throttle_0_0, 0), (self.zeromq_rep_sink_0, 0))
        self.connect((self.zeromq_req_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.zeromq_req_source_0_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.zeromq_req_source_1, 0), (self.blocks_throttle_0, 0))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle_0_0.set_sample_rate(self.samp_rate)

    def get_min_gain(self):
        return self.min_gain

    def set_min_gain(self, min_gain):
        self.min_gain = min_gain

    def get_max_gain(self):
        return self.max_gain

    def set_max_gain(self, max_gain):
        self.max_gain = max_gain



def main(top_block_cls=multi_ue, options=None):
    tb = top_block_cls()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()
    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
