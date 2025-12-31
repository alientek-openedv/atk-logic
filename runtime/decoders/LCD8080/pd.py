import sigrokdecode as srd

class Ann:
    CMD, DATA = range(2)

class Decoder(srd.Decoder):
    api_version = 3
    id = 'lcd8080'
    name = 'LCD8080'
    longname = '8080 Parallel LCD Interface'
    desc = 'Decode CMD/DATA writes on 8080 8-bit parallel LCD'
    license = 'gplv3+'
    inputs = ['logic']
    outputs = []
    tags = ['Display']
    options = (
        {'id': 'a0dc_polarity', 'desc': 'Data Polarity (Low = Data, High = Data)',
            'default': 'low', 'values': ('low', 'high')},
        {'id': 'offset_samples', 'desc': 'Delay after WR falling edge (samples)', 'default': 0},
    )
    channels = tuple({
            'id': 'd%d' % i,
            'name': 'D%d' % i,
            'desc': 'Data bus line %d' % i
            } for i in range(8)
    ) + (
        {'id': 'wr', 'name': '/WR', 'desc': 'Write strobe (active low)'},
        {'id': 'a0dc', 'name': 'A0/DC', 'desc': 'Command/Data select'},
    )
    optional_channels = (
        {'id': 'cs', 'name': '/CS', 'desc': 'Chip Select (active low)'},
    )
    annotations = (
        ('cmd', 'Command'),
        ('data', 'Data'),
    )
    annotation_rows = (
        ('cmd_', 'Command', (Ann.CMD,)),
        ('data_', 'Data', (Ann.DATA,)),
    )

    def __init__(self):
        self.reset()

    def reset(self):
        self.prev_wr = 1

    def start(self):
        self.out_ann = self.register(srd.OUTPUT_ANN)

    def decode(self):
        while True:
            pins = self.wait()
            curr_wr = pins[8]

            if self.prev_wr == 1 and curr_wr == 0:
                count = 0

                cs_connected = len(pins) > 10
                if cs_connected and pins[10] != 0:
                    self.prev_wr = curr_wr
                    continue
                
                while count < self.options['offset_samples']:
                    pins = self.wait()
                    count += 1

                # After N sample steps, sample data bus
                data_bits = pins[0:8]
                if 0xFF in data_bits:
                    continue  # Skip if any undefined bits

                value = 0
                for i, bit in enumerate(data_bits):
                    value |= (bit << i)

                a0_dc = pins[9]
                if self.options['a0dc_polarity'] == 'low': 
                    ann_id = Ann.CMD if a0_dc else Ann.DATA
                else:
                    ann_id = Ann.DATA if a0_dc else Ann.CMD
                label = '0x%02X' % value

                self.put(self.samplenum, self.samplenum, self.out_ann, [ann_id, [label]])

            self.prev_wr = curr_wr