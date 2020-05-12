/****************************************************************************
  Copyright (c) 2001 Thomas Trummer
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "audio/fmopl.h"
#include "clue/base/base.h"
#include "clue/sound/fx.h"

namespace Clue {

#define OPL_INTERNAL_FREQ   3579545 /* 3.6 MHz... */
#define OPL_NUM_CHIPS       1
#define OPL_CHIP0           0

static bool OPL_Ok;

static const int index_car[] =
{ 0x03, 0x04, 0x05, 0x0B, 0x0C, 0x0D, 0x13, 0x14, 0x15 };

static const int index_mod[] =
{ 0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12 };

static const int freq_table[] = {
	0x016B, 0x0181, 0x0198, 0x01B0, 0x01CA, 0x01E5,
	0x0202, 0x0220, 0x0241, 0x0263, 0x0287, 0x02AE
};

static struct {
	uint8 car_am_vib_eg_ksr_multi;
	uint8 mod_am_vib_eg_ksr_multi;

	uint8 car_ksl_volume;
	uint8 mod_ksl_volume;

	uint8 car_attack_decay;
	uint8 mod_attack_decay;

	uint8 car_sustain_release;
	uint8 mod_sustain_release;

	uint8 feedback_fm;

	uint8 car_waveform;
	uint8 mod_waveform;

	uint8 slide;
} instrument[128];


static struct {
	uint8 note[9][64];
	uint8 effect[9][64];
} pattern[50];


static uint8 pattern_table[51];



enum voice_type {
	on, off, slide
};


static bool hsc_ch_keyoff[9];
static int hsc_ch_octave[9];
static int hsc_ch_inst[9];
static int hsc_ch_note[9];

static int hsc_index;
static int hsc_line;
static int hsc_speed;
static int hsc_ticks;

static bool hsc_effect03 = false;
static int hsc_effect03_line = 0;

static bool hsc_valid_data = false;

static volatile bool hsc_in_process = false;

static OPL::OPL *opl;



static void ym3812_write(int reg, int val) {
	opl->writeReg(reg, val);
}



static void hsc_set_volume(int channel, int volume_car, int volume_mod) {
	if (volume_car != 0xFF) {
		volume_car |= instrument[hsc_ch_inst[channel]].car_ksl_volume & 0xC0;

		ym3812_write(0x40 + index_car[channel], volume_car);
	}

	if (volume_car == 0xFF && volume_mod != 0xFF) {
		volume_mod |= instrument[hsc_ch_inst[channel]].mod_ksl_volume & 0xC0;

		ym3812_write(0x40 + index_mod[channel], volume_mod);
	}

	if (volume_car != 0xFF && volume_mod != 0xFF
	        && (instrument[hsc_ch_inst[channel]].feedback_fm & 1)) {
		volume_mod |= instrument[hsc_ch_inst[channel]].mod_ksl_volume & 0xC0;

		ym3812_write(0x40 + index_mod[channel], volume_mod);
	}
}


static void hsc_set_instrument(int channel, int new_inst) {
	hsc_ch_inst[channel] = new_inst;

	hsc_set_volume(channel, 0x3F, 0x3F);

	ym3812_write(0x20 + index_car[channel],
	             instrument[new_inst].car_am_vib_eg_ksr_multi);
	ym3812_write(0x20 + index_mod[channel],
	             instrument[new_inst].mod_am_vib_eg_ksr_multi);

	ym3812_write(0x40 + index_car[channel],
	             instrument[new_inst].car_ksl_volume);
	ym3812_write(0x40 + index_mod[channel],
	             instrument[new_inst].mod_ksl_volume);

	ym3812_write(0x60 + index_car[channel],
	             instrument[new_inst].car_attack_decay);
	ym3812_write(0x60 + index_mod[channel],
	             instrument[new_inst].mod_attack_decay);

	ym3812_write(0x80 + index_car[channel],
	             instrument[new_inst].car_sustain_release);
	ym3812_write(0x80 + index_mod[channel],
	             instrument[new_inst].mod_sustain_release);

	ym3812_write(0xC0 + channel, instrument[new_inst].feedback_fm);

	ym3812_write(0xE0 + index_car[channel], instrument[new_inst].car_waveform);
	ym3812_write(0xE0 + index_mod[channel], instrument[new_inst].mod_waveform);
}


static void hsc_set_voice(int channel, voice_type vt) {
	int note_high = (hsc_ch_note[channel] >> 0x08) | (hsc_ch_octave[channel] << 0x02);
	int note_low = hsc_ch_note[channel] & 0xFF;

	switch (vt) {
	case on: {
		ym3812_write(0xB0 + channel, 0x00);
		ym3812_write(0xA0 + channel, note_low);
		ym3812_write(0xB0 + channel, note_high | 0x20);

		hsc_ch_keyoff[channel] = false;
	}
	break;

	case off: {
		ym3812_write(0xB0 + channel, note_high);

		hsc_ch_keyoff[channel] = true;
	}
	break;

	case slide: {
		ym3812_write(0xA0 + channel, note_low);

		if (hsc_ch_keyoff[channel]) {
			ym3812_write(0xB0 + channel, note_high);
		} else {
			ym3812_write(0xB0 + channel, note_high | 0x20);
		}
	}
	break;
	}
}


static void hsc_process_row() {
	bool pat_break = false;

	if (!hsc_valid_data || hsc_in_process)
		return;

	hsc_in_process = true;

	if (hsc_ticks < hsc_speed) {
		hsc_ticks++;
		hsc_in_process = false;
		return;
	}

	hsc_ticks = 1;

	int cur_pattern = pattern_table[hsc_index];

	if (hsc_effect03) {
		hsc_effect03_line++;

		if (hsc_effect03_line == 32) {
			hsc_effect03 = false;
			/*hsc_set_volume(4, 0x03 * 4, 0x03 * 4);*/
			/*hsc_set_volume(3, 0x03 * 4, 0x03 * 4);*/
		} else {
			for (int i = 0; i < 9; i++) {
				hsc_set_volume(i, 0x3F - hsc_effect03_line * 2 + 1,
				               0x3F - hsc_effect03_line * 2 + 1);
			}
		}
	}



	for (int channel = 0; channel < 9; channel++) {
		int note = pattern[cur_pattern].note[channel][hsc_line];
		int effect = pattern[cur_pattern].effect[channel][hsc_line];

		switch (note) {
		case 0: {
		}
		break;

		case 127: {
			hsc_set_voice(channel, off);
		}
		break;

		case 128: {
			hsc_set_instrument(channel, effect);
		}
		break;

		default: {
			note -= 1;

			hsc_ch_octave[channel] = note / 12;
			hsc_ch_note[channel] = freq_table[note % 12];

			if (effect >= 0x10 && effect <= 0x1F) {
				hsc_ch_note[channel] += (int)((effect - 0x10) * 1.31);
			}

			if (effect >= 0x20 && effect <= 0x2F) {
				hsc_ch_note[channel] -= (int)((effect - 0x20) * 1.31);
			}

			if (instrument[hsc_ch_inst[channel]].slide >> 0x04) {
				hsc_ch_note[channel] +=
				    (int)((instrument[hsc_ch_inst[channel]].
				           slide >> 0x04) * 1.31);
			}

			hsc_set_voice(channel, on);
		}
		break;
		}


		if (effect != 0x00) {
			/* Pattern break */

			if (effect == 0x01 && note != 0x80) {
				pat_break = true;   /*cur_line = 0x3F;*/
			}


			if (effect == 0x03 && note != 0x80) {
				hsc_effect03 = true;

				hsc_effect03_line = 0;

				for (int i = 0; i < 9; i++) {
					hsc_set_volume(i, 0x3F, 0x3F);
				}
			}
			/* Slide note up */

			if (effect >= 0x10 && effect <= 0x1F && note == 0x00) {
				hsc_ch_note[channel] += (int)((effect - 0x10) * 1.31);

				hsc_set_voice(channel, slide);
			}
			/* Slide note down */

			if (effect >= 0x20 && effect <= 0x2F && note == 0x00) {
				hsc_ch_note[channel] -= (int)((effect - 0x20) * 1.31);

				hsc_set_voice(channel, slide);
			}
			/* Set carrier volume */

			if (effect >= 0xA0 && effect <= 0xAF) {
				hsc_set_volume(channel, (effect - 0xA0) * 0x04, 0xFF);
			}
			/* Set modulator volume */

			if (effect >= 0xB0 && effect <= 0xBF) {
				hsc_set_volume(channel, 0xFF, (effect - 0xB0) * 0x04);
			}
			/* Set instrument volume */

			if (effect >= 0xC0 && effect <= 0xCF) {
				hsc_set_volume(channel, (effect - 0xC0) * 0x4,
				               (effect - 0xC0) * 0x4);
			}
			/* Set speed */

			if (effect >= 0xF0 && effect <= 0xFF) {
				hsc_speed = effect - 0xEF;
			}
		}
	}


	hsc_line = pat_break ? 0x40 : hsc_line + 0x01;


	if (hsc_line == 0x40) {
		hsc_line = 0x00;
		hsc_index += 0x01;

		if (pattern_table[hsc_index] == 0xFF) {
			hsc_index = 0x00;

			ym3812_write(0x01, 0x20);

			ym3812_write(0x08, 0x40);


			for (int i = 0; i < 9; i++) {
				ym3812_write(0xB0 + i, 0);
				ym3812_write(0xA0 + i, 0);

				hsc_set_instrument(i, i);

				hsc_ch_note[i] = 0;
				hsc_ch_octave[i] = 0;
				hsc_ch_keyoff[i] = false;
			}

			hsc_line = 0;
			hsc_index = 0;
			hsc_ticks = 1;
			hsc_speed = 2;
		} else if (pattern_table[hsc_index] >= 0x80) {
			hsc_index = pattern_table[hsc_index] - 0x80;

			ym3812_write(0x01, 0x20);
			ym3812_write(0x08, 0x40);

			for (int i = 0; i < 9; i++) {
				ym3812_write(0xB0 + i, 0);
				ym3812_write(0xA0 + i, 0);

				hsc_set_instrument(i, i);

				hsc_ch_note[i] = 0;
				hsc_ch_octave[i] = 0;
				hsc_ch_keyoff[i] = false;
			}

			hsc_line = 0;
			hsc_index = 0;
			hsc_ticks = 1;
			hsc_speed = 2;
		}
	}

	hsc_in_process = false;
}

#define HSC_BUFFER_SIZE     (SND_FREQUENCY / 18)

/* Generic audio & audio mixing functions */
static int16 MusicStream[HSC_BUFFER_SIZE];

void hscMusicPlayer() {
	if (OPL_Ok)
		hsc_process_row();
}

void hscReset() {
	if (!OPL_Ok)
		return;

	while (hsc_in_process);

	hsc_in_process = true;
	hsc_valid_data = false;

	opl->reset();

	hsc_in_process = false;
}

bool hscLoad(const char *filename) {
	if (!OPL_Ok)
		return false;

	hscReset();
	hsc_in_process = true;

	uint8 *hsc_file = (uint8 *)dskLoad(filename);
	uint8 *hsc_data = hsc_file;

	for (int i = 0; i < 128; i++) {
		instrument[i].car_am_vib_eg_ksr_multi = *hsc_data++;
		instrument[i].mod_am_vib_eg_ksr_multi = *hsc_data++;

		instrument[i].car_ksl_volume = *hsc_data++;
		instrument[i].mod_ksl_volume = *hsc_data++;

		instrument[i].car_attack_decay = *hsc_data++;
		instrument[i].mod_attack_decay = *hsc_data++;

		instrument[i].car_sustain_release = *hsc_data++;
		instrument[i].mod_sustain_release = *hsc_data++;

		instrument[i].feedback_fm = *hsc_data++;

		instrument[i].car_waveform = *hsc_data++;
		instrument[i].mod_waveform = *hsc_data++;

		instrument[i].slide = *hsc_data++;
	}


	for (int i = 0; i < 51; i++)
		pattern_table[i] = *hsc_data++;


	int num_pattern = (dskFileLength(filename) - 1587) / 1152;

	for (int i = 0; i < num_pattern; i++) {
		for (int line = 0; line < 64; line++) {
			for (int channel = 0; channel < 9; channel++) {
				pattern[i].note[channel][line] = *hsc_data++;
				pattern[i].effect[channel][line] = *hsc_data++;
			}
		}
	}

	free(hsc_file);

	ym3812_write(0x01, 0x20);
	ym3812_write(0x08, 0x40);

	for (int i = 0; i < 9; i++) {
		ym3812_write(0xB0 + i, 0);
		ym3812_write(0xA0 + i, 0);

		hsc_set_instrument(i, i);

		hsc_ch_note[i] = 0;
		hsc_ch_octave[i] = 0;
		hsc_ch_keyoff[i] = false;
	}

	hsc_line = 0;
	hsc_index = 0;
	hsc_ticks = 1;
	hsc_speed = 2;

	hsc_valid_data = true;
	hsc_in_process = false;
	return true;
}

template<class Res>
class Functor0Ptr : public Common::Functor0<Res> {
public:
	typedef Res(*FuncType)();

	Functor0Ptr(const FuncType &func) : _func(func) {}

	bool isValid() const {
		return _func != 0;
	}
	Res operator()() const {
		return (*_func)();
	}
private:
	const FuncType _func;
};

void hscInit() {
	opl = OPL::Config::create();
	if (!opl || !opl->init()) {
		OPL_Ok = false;
		DebugMsg(ERR_WARNING, ERROR_MODULE_SOUND, "OPL error");
	} else {
		OPL_Ok = true;
		opl->start(new Functor0Ptr<void>(hscMusicPlayer), 18);
	}
}

void hscDone() {
	delete opl;
	opl = nullptr;
	OPL_Ok = false;
}

} // End of namespace Clue
