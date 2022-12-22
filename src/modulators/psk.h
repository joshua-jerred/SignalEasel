/**
 * @file PSK.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Header file for that defines the PSK class, varicode to ascii
 * map, ascii to varicode loopup table, and convolutional code map.
 * @date 2022-11-23
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#ifndef PSK_H_
#define PSK_H_

#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <math.h>
#include <exception>
#include <sstream>

class PSK {
    public:
        enum Mode { 
            BPSK,
            QPSK
            };

        enum SymbolRate {
            S31, 
            S63, 
            S125, 
            S250, 
            S500,
            S1000
        };

        PSK(std::string file_path, Mode mode, SymbolRate sym_rate);
        PSK(std::string fuile_path, Mode mode, SymbolRate sym_rate, std::string call_sign);
        ~PSK();

        bool encodeTextData(std::string message);
        bool encodeRawData(unsigned char *data, int length);
        void dumpBitStream();
        

    private:
        // Configuration
        void setup(Mode mode, SymbolRate baud);
        const int morse_frequency_ = 600;
        const int morse_dit_length_ = 100; // milliseconds
        const int morse_dah_length_ = 300; // milliseconds
        
        const int preamble_length_ = 64; // symbols
        const int postamble_length_ = 64; // symbols

        std::string file_path_;
        Mode mode_;
        bool morse_callsign_;
        std::string call_sign_;


        // WAV file members and methods
        bool openFile(std::string file_path);
        void writeHeader();
        void writeBytes(int data, int size);
        void finalizeFile();
        void addCallSign();
        
        const int sample_rate_ = 44100; // Sample rate of the WAV file in Hz (44100)
        const int bits_per_sample_ = 16;
        const int max_amplitude_ = pow(2, bits_per_sample_ - 1) - 1;
        std::ofstream wav_file_; // File descriptor for the WAV file
        int data_start_;

        // Bit Stream members and methods
        void addVaricode(char c);
        void addBits(unsigned char *data, int num_bits);
        void pushBufferToBitStream();
        void addPreamble();
        void addPostamble();
        int popNextBit();
        int peakNextBit();
        
        /**
         * @details Bit stream is an array of 32 bit integers.
         * The bits are stored in a 32 bit integer in the following way:
         * 
         * Characters: 
         * a a b b c c
         * 
         * varicode with 2x 0 bit padding added to the end of each character: 
         * '1011' '00' '1011' '00'  '1011111' '00' '1011111' '00' '101111' '00' '101111' '00'
         * 
         * 32 bit array:
         * [0](10110010110010111110010111110010) {a, a, b, b, half of c    }
         * [1](11110010111100000000000000000000) {other half of c, 0 bits..}
         * 
         * Although this may not be the best way to store the bit stream,
         * it makes it easier to understand.
         */
        std::vector<uint32_t> bit_stream_;
        int bit_stream_index_ = 0;
        uint32_t bit_stream_buffer_ = 0;
        int bit_stream_offset_ = 0; // Write left to right. [0 - 31]

        // Modulation members and methods
        void encodeBitStream();
        void addSymbol(double shift, int next_shift);

        double symbol_rate_; // Symbol rate of the PSK modulation in Sym/s (125, 250, 500)
        int carrier_freq_ = 1500; // Carrier frequency in Hz (1500)
        int samples_per_symbol_; // floor(sample_rate_ / symbol_rate_)

        double carrier_wave_angle_ = 0.0f;
        double angle_delta_;
        int last_symbol_end_filtered_ = 1;
        
        int last_bit_ = 0;
        unsigned char conv_code_buffer_ = 0;
};



#endif // PSK_H_

#ifndef VARICODE_H_
#define VARICODE_H_
/**
 * 
 * @brief A lookup table for converting char -> varicode string.
 * Supports all ASCII control characters and printable characters 0-127.
 */
const uint16_t ascii_to_varicode[128] = {
    // ASCII Control Characters (0 - 31)
    0b1010101011, // 0	    [NUL]	Null character
    0b1011011011, // 1	    [SOH]	Start of Header
    0b1011101101, // 2	    [STX]	Start of Text
    0b1101110111, // 3	    [ETX]	End of Text
    0b1011101011, // 4	    [EOT]	End of Transmission
    0b1101011111, // 5	    [ENQ]	Enquiry
    0b1011101111, // 6	    [ACK]	Acknowledgment
    0b1011111101, // 7	    [BEL]	Bell
    0b1011111111, // 8	    [BS]	Backspace
    0b11101111,	  // 9	    [HT]	Horizontal Tab
    0b11101,	  // 10	    [LF]	Line feed
    0b1101101111, // 11	    [VT]	Vertical Tab
    0b1011011101, // 12	    [FF]	Form feed
    0b11111,	  // 13	    [CR]	Carriage return
    0b1101110101, // 14	    [SO]	Shift Out
    0b1110101011, // 15	    [SI]	Shift In
    0b1011110111, // 16	    [DLE]	Data Link Escape
    0b1011110101, // 17	    [DC1]	Device Control 1 (XON)
    0b1110101101, // 18	    [DC2]	Device Control 2
    0b1110101111, // 19	    [DC3]	Device Control 3 (XOFF)
    0b1101011011, // 20	    [DC4]	Device Control 4
    0b1101101011, // 21	    [NAK]	Negative Acknowledgement
    0b1101101101, // 22	    [SYN]	Synchronous Idle
    0b1101010111, // 23	    [ETB]	End of Trans. Block
    0b1101111011, // 24	    [CAN]	Cancel
    0b1101111101, // 25	    [EM]	End of Medium
    0b1110110111, // 26	    [SUB]	Substitute
    0b1101010101, // 27	    [ESC]	Escape
    0b1101011101, // 28	    [FS]	File Separator
    0b1110111011, // 29	    [GS]	Group Separator
    0b1011111011, // 30	    [RS]	Record Separator
    0b1101111111, // 31	    [US]	Unit Separator
    
    // ASCII Printable Characters (32 - 126)
    0b1,          //    32	[Space]
    0b111111111,  //    33	!
    0b101011111,  //    34	"
    0b111110101,  //    35	#
    0b111011011,  //    36	$
    0b1011010101, //    37	%
    0b1010111011, //    38	&
    0b101111111,  //    39	'
    0b11111011,   //    40	(
    0b11110111,   //    41	)
    0b101101111,  //    42	*
    0b111011111,  //    43	+
    0b1110101,    //    44	, 
    0b110101,     //	45	-
    0b1010111,    //	46	.
    0b110101111,  //	47	/
    0b10110111,   //	48	0
    0b10111101,   //	49	1
    0b11101101,   //	50	2
    0b11111111,   //	51	3
    0b101110111,  //	52	4
    0b101011011,  //	53	5
    0b101101011,  //	54	6
    0b110101101,  //	55	7
    0b110101011,  //	56	8
    0b110110111,  //	57	9
    0b11110101,   //	58	:
    0b110111101,  //	59	;
    0b111101101,  //	60	<
    0b1010101,    //	61	=
    0b111010111,  //	62	>
    0b1010101111, //	63	?
    0b1010111101, //	64	@
    0b1111101,    //	65	A
    0b11101011,   //	66	B
    0b10101101,   //	67	C
    0b10110101,   //	68	D
    0b1110111,    //	69	E
    0b11011011,   //	70	F
    0b11111101,   //	71	G
    0b101010101,  //	72	H
    0b1111111,    //	73	I
    0b111111101,  //	74	J
    0b101111101,  //	75	K
    0b11010111,   //	76	L
    0b10111011,   //	77	M
    0b11011101,   //	78	N
    0b10101011,   //	79	O
    0b11010101,   //	80	P
    0b111011101,  //	81	Q
    0b10101111,   //	82	R
    0b1101111,    //	83	S
    0b1101101,    //	84	T
    0b101010111,  //	85	U
    0b110110101,  //	86	V
    0b101011101,  //	87	W
    0b101110101,  //	88	X
    0b101111011,  //	89	Y
    0b1010101101, //	90	Z
    0b111110111,  //	91	[
    0b111101111,  //	92	[backslash]
    0b111111011,  //	93	]
    0b1010111111, //	94	^
    0b101101101,  //	95	_
    0b1011011111, //	96	'
    0b1011,       //	97	a
    0b1011111,    //	98	b
    0b101111,     //	99	c
    0b101101,     //	100	d
    0b11,         //	101	e
    0b111101,     //	102	f
    0b1011011,    //	103	g
    0b101011,     //	104	h
    0b1101,       //	105	i
    0b111101011,  //	106	j
    0b10111111,   //	107	k
    0b11011,      //	108	l
    0b111011,     //	109	m
    0b1111,       //	110	n
    0b111,        //	111	o
    0b111111,     //	112	p
    0b110111111,  //	113	q
    0b10101,      //	114	r
    0b10111,      //	115	s
    0b101,        //	116	t
    0b110111,     //	117	u
    0b1111011,    //	118	v
    0b1101011,    //	119	w
    0b11011111,   //	120	x
    0b1011101,    //	121	y
    0b111010101,  //	122	z
    0b1010110111, //	123	{
    0b110111011,  //	124	|
    0b1010110101, //	125	}
    0b1011010111, //	126	~
    0b1110110101  //	127	[DEL]
};

/**
 * @brief An unordered_map to convert a string representation of varicode
 * to a char. Supports all Varicode/ASCII characters 0-127.
 */
static std::unordered_map<std::string, int> varicode_to_ascii = {
    // ASCII Control Characters (0 - 31)
    {"1010101011"   ,   0   }, // 0	    [NUL]	Null character
    {"1011011011"   ,   1   }, // 1	    [SOH]	Start of Header
    {"1011101101"   ,   2   }, // 2	    [STX]	Start of Text
    {"1101110111"   ,   3   }, // 3	    [ETX]	End of Text
    {"1011101011"   ,   4   }, // 4	    [EOT]	End of Transmission
    {"1101011111"   ,   5   }, // 5	    [ENQ]	Enquiry
    {"1011101111"   ,   6   }, // 6	    [ACK]	Acknowledgment
    {"1011111101"   ,   7   }, // 7	    [BEL]	Bell
    {"1011111111"   ,   8   }, // 8	    [BS]	Backspace
    {"11101111"     ,   9   }, // 9	    [HT]	Horizontal Tab
    {"11101"        ,   10  }, // 10	[LF]	Line feed
    {"1101101111"   ,   11  }, // 11	[VT]	Vertical Tab
    {"1011011101"   ,   12  }, // 12	[FF]	Form feed
    {"11111"        ,   13  }, // 13	[CR]	Carriage return
    {"1101110101"   ,   14  }, // 14	[SO]	Shift Out
    {"1110101011"   ,   15  }, // 15	[SI]	Shift In
    {"1011110111"   ,   16  }, // 16	[DLE]	Data Link Escape
    {"1011110101"   ,   17  }, // 17	[DC1]	Device Control 1 (XON)
    {"1110101101"   ,   18  }, // 18	[DC2]	Device Control 2
    {"1110101111"   ,   19  }, // 19	[DC3]	Device Control 3 (XOFF)
    {"1101011011"   ,   20  }, // 20	[DC4]	Device Control 4
    {"1101101011"   ,   21  }, // 21	[NAK]	Negative Acknowledgement
    {"1101101101"   ,   22  }, // 22	[SYN]	Synchronous Idle
    {"1101010111"   ,   23  }, // 23	[ETB]	End of Trans. Block
    {"1101111011"   ,   24  }, // 24	[CAN]	Cancel
    {"1101111101"   ,   25  }, // 25	[EM]	End of Medium
    {"1110110111"   ,   26  }, // 26	[SUB]	Substitute
    {"1101010101"   ,   27  }, // 27	[ESC]	Escape
    {"1101011101"   ,   28  }, // 28	[FS]	File Separator
    {"1110111011"   ,   29  }, // 29	[GS]	Group Separator
    {"1011111011"   ,   30  }, // 30	[RS]	Record Separator
    {"1101111111"   ,   31  }, // 31	[US]	Unit Separator

    // ASCII Printable Characters (32 - 126)
    {"1"            ,	32	}, //	Space
    {"111111111"    ,   33  }, //	!
    {"101011111"	,	34	}, //	"
    {"111110101"	,	35	}, //	#
    {"111011011"	,	36	}, //	$
    {"1011010101"	,	37	}, //	%
    {"1010111011"	,	38	}, //	&
    {"101111111"	,	39	}, //	'
    {"11111011"	    ,	40	}, //	(
    {"11110111"	    ,	41	}, //	)
    {"101101111"	,	42	}, //	*
    {"111011111"	,	43	}, //	+
    {"1110101"	    ,	44	}, //	,
    {"110101"	    ,	45	}, //	-
    {"1010111"	    ,	46	}, //	.
    {"110101111"	,	47	}, //	/
    {"10110111"	    ,	48	}, //	0
    {"10111101"	    ,	49	}, //	1
    {"11101101"	    ,	50	}, //	2
    {"11111111"	    ,	51	}, //	3
    {"101110111"	,	52	}, //	4
    {"101011011"	,	53	}, //	5
    {"101101011"	,	54	}, //	6
    {"110101101"	,	55	}, //	7
    {"110101011"	,	56	}, //	8
    {"110110111"	,	57	}, //	9
    {"11110101"	    ,	58	}, //	:
    {"110111101"	,	59	}, //	;
    {"111101101"	,	60	}, //	<
    {"1010101"	    ,	61	}, //	=
    {"111010111"	,	62	}, //	>
    {"1010101111"	,	63	}, //	?
    {"1010111101"	,	64	}, //	@
    {"1111101"	    ,	65	}, //	A
    {"11101011"	    ,	66	}, //	B
    {"10101101"	    ,	67	}, //	C
    {"10110101"	    ,	68	}, //	D
    {"1110111"	    ,	69	}, //	E
    {"11011011"	    ,	70	}, //	F
    {"11111101"	    ,	71	}, //	G
    {"101010101"	,	72	}, //	H
    {"1111111"	    ,	73	}, //	I
    {"111111101"	,	74	}, //	J
    {"101111101"	,	75	}, //	K
    {"11010111"	    ,	76	}, //	L
    {"10111011"	    ,	77	}, //	M
    {"11011101"	    ,	78	}, //	N
    {"10101011"	    ,	79	}, //	O
    {"11010101"	    ,	80	}, //	P
    {"111011101"	,	81	}, //	Q
    {"10101111"	    ,	82	}, //	R
    {"1101111"	    ,	83	}, //	S
    {"1101101"	    ,	84	}, //	T
    {"101010111"	,	85	}, //	U
    {"110110101"	,	86	}, //	V
    {"101011101"	,	87	}, //	W
    {"101110101"	,	88	}, //	X
    {"101111011"	,	89	}, //	Y
    {"1010101101"   ,   90  }, //	Z
    {"111110111"    ,   91  }, //	[
    {"111101111"    ,   92  }, //	[backslash]
    {"111111011"    ,   93  }, //	]
    {"1010111111"   ,   94  }, //	^
    {"101101101"    ,   95  }, //	_
    {"1011011111"   ,   96  }, //	'
    {"1011"         ,   97  }, //	a
    {"1011111"      ,   98  }, //	b
    {"101111"       ,   99  }, //	c
    {"101101"	    ,	100	}, //	d
    {"11"	        ,	101	}, //	e
    {"111101"	    ,	102	}, //	f
    {"1011011"	    ,	103	}, //	g
    {"101011"	    ,	104	}, //	h
    {"1101"	        ,	105	}, //	i
    {"111101011"	,	106	}, //	j
    {"10111111"	    ,	107	}, //	k
    {"11011"	    ,	108	}, //	l
    {"111011"	    ,	109	}, //	m
    {"1111"	        ,	110	}, //	n
    {"111"	        ,	111	}, //	o
    {"111111"	    ,	112	}, //	p
    {"110111111"	,	113	}, //	q
    {"10101"	    ,	114	}, //	r
    {"10111"	    ,	115	}, //	s
    {"101"	        ,	116	}, //	t
    {"110111"	    ,	117	}, //	u
    {"1111011"	    ,	118	}, //	v
    {"1101011"	    ,	119	}, //	w
    {"11011111"	    ,	120	}, //	x
    {"1011101"	    ,	121	}, //	y
    {"111010101"	,	122	}, //	z
    {"1010110111"	,	123	}, //	{
    {"110111011"	,	124	}, //	|
    {"1010110101"	,	125	}, //	}
    {"1011010111"	,	126	}, //	~
    {"1110110101"	,	127	}  //	[DEL]
};
#endif // VARICODE_H_

#ifndef CONVOLUTIONAL_H_
#define CONVOLUTIONAL_H_
/**
 * @brief Convolutional Code. Left shifting 5 bit values mapped to their
 * respective phase shift values in degrees. Supports all binary values
 * 0x00 to 0x1F.
 * @cite http://www.arrl.org/psk31-spec
 */
std::map <unsigned char, double> conv_code = {
    {0b00000, M_PI},
    {0b00001, M_PI/2.0 },
    {0b00010, -(M_PI/2.0)},
    {0b00011, 0  },
    {0b00100, -(M_PI/2.0)},
    {0b00101, 0  },
    {0b00110, M_PI},
    {0b00111, M_PI/2.0 },
    {0b01000, 0  },
    {0b01001, -(M_PI/2.0)},
    {0b01010, M_PI/2.0 },
    {0b01011, M_PI},
    {0b01100, M_PI/2.0 },
    {0b01101, M_PI},
    {0b01110, 0 },
    {0b01111, -(M_PI/2.0)},
    {0b10000, M_PI/2.0 },
    {0b10001, M_PI},
    {0b10010, 0  },
    {0b10011, -(M_PI/2.0)},
    {0b10100, 0  },
    {0b10101, -(M_PI/2.0)},
    {0b10110, M_PI/2.0 },
    {0b10111, M_PI},
    {0b11000, -(M_PI/2.0)},
    {0b11001, 0  },
    {0b11010, M_PI},
    {0b11011, M_PI/2.0 },
    {0b11100, M_PI},
    {0b11101, M_PI/2.0 },
    {0b11110, -(M_PI/2.0)},
    {0b11111, 0  }
};
#endif // CONVOLUTIONAL_H_