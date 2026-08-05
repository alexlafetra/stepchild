//BITMAPTS!-----------------------------
/*

	Huge list of bitmaps that are reused by different functions

	search:
	const\s+unsigned\s+char\s+(\w+)\s*\[\]\s*=\s*\{[\s\S\n]*?\};
	replace:
	extern const unsigned char* $1[];

	to convert these automatically from the actual byte arrays
*/

#pragma once

//midi
// 'tiny_midi', 7x7px
extern const unsigned char tiny_midi_bmp[];
// 'tiny_stepchild', 7x7px
extern const unsigned char tiny_stepchild_bmp[];
// 'tiny_pram', 7x7px
extern const unsigned char tinyPram_bmp[];

// 'stepchild', 76x64px
extern const unsigned char stepchild_fullscreen[];
// 'tiny_usb', 10x4px
extern const unsigned char tiny_usb[];
//carriage
//14 x 15
extern const unsigned char carriage_bmp[];
extern const unsigned char carriage_2_bmp [];

extern const unsigned char dither_tile_bmp [];
extern const unsigned char dither_tile_invert_bmp [];
extern const unsigned char dither_tile_2_bmp [];
extern const unsigned char dither_tile_2_invert_bmp [];

// 'childOSv0', 128x64px
extern const unsigned char childOS_bmp[];

// 'arrows_1', 16x16px
extern const unsigned char arrow_1_bmp[];
// 'arrows_2', 16x16px
extern const unsigned char arrow_2_bmp[];
// 'arrows_3', 16x16px
extern const unsigned char arrow_3_bmp[];
// 'arrow_small', 11x11px
extern const unsigned char arrow_small_bmp1[];
// 'arrow_small_bmp2', 9x9px
extern const unsigned char arrow_small_bmp2[];
// 'pen', 15x15px
extern const unsigned char pen_bmp[];
// 'disc1', 15x45px
extern const unsigned char disc1_bmp[];
// 'disc2', 15x45px
extern const unsigned char disc2_bmp[];
// 'disc3', 15x45px
extern const unsigned char disc3_bmp[];
// 'disc4', 15x45px
extern const unsigned char disc4_bmp[];


//loop arrows
// 'loop_arrows1', 7x7px
extern const unsigned char toploop_arrow1[];
// 'loop_arrows2', 7x7px
extern const unsigned char toploop_arrow2[];
// 'play_oneshot', 8x7px
extern const unsigned char oneshot_arrow[];
// 'one_shot_rec', 7x7px
extern const unsigned char oneShot_rec[];
// 'numberArrow', 16x7px
extern const unsigned char numberLoopIcon_bmp[];

// 'swing_1', 7x8px
extern const unsigned char swing_1[];
// 'swing_2', 7x8px
extern const unsigned char swing_2[];
// 'swing_3', 7x8px
extern const unsigned char swing_3[];

// 'latch_big', 7x7px
extern const unsigned char latch_big[];
// 'unmute', 7x7px
extern const unsigned char unmuted[];
// 'mute', 7x7px
extern const unsigned char muted[];
// 'ch_tiny', 6x3px
extern const unsigned char ch_tiny[];
// 'cc_tiny', 5x3px
extern const unsigned char cc_tiny[];
// 'trk_tiny', 10x3px
extern const unsigned char trk_tiny[];
// 'cc_small', 5x5px
extern const unsigned char cc_small[];
// 'rec_tiny', 11x4px
extern const unsigned char rec_tiny[];
// 'small_clock', 5x5px
extern const unsigned char small_clock[];

// 'di', 14x15px
extern const unsigned char top_die_icon[];

// 'arp_tiny', 9x3px
extern const unsigned char tiny_arp_bmp[];

// 'overwrite1_2', 7x7px
extern const unsigned char overwrite_1[];
// 'overwrite1', 7x7px
extern const unsigned char overwrite_2[];

// 'oneshot', 3x5px
extern const unsigned char caution_bmp[];

// 'continuous_rec', 9x5px
extern const unsigned char continuous_bmp[];


// 'VUmeter', 19x14px
extern const unsigned char VUmeter_bmp[];

//echo_bmp -------------------
// 'decay_inverse', 30x12px
extern const unsigned char decay_inverse_bmp[];
// 'decay', 30x12px
extern const unsigned char decay_bmp[];
// 'repetitions_inverse', 66x11px
extern const unsigned char repetitions_inverse_bmp[];
// 'repetitions', 66x11px
extern const unsigned char repetitions_bmp[];
// 'time_inverse', 25x6px
extern const unsigned char time_inverse_bmp[];
// 'time', 25x6px
extern const unsigned char time_bmp[];

//clocks -----------------
// 'clock1', 20x38px
extern const unsigned char clock_1_bmp[];

// 'up_arrow', 9x12px
extern const unsigned char up_arrow[];

// 'down_arrow', 9x12px
extern const unsigned char down_arrow[];

//7 segment ---------
// '7seg_horizontal', 9x3px
extern const unsigned char seg_h[];
// '7segitalic', 4x9px
extern const unsigned char seg_italic[];
// '7seg_vertical', 3x9px
extern const unsigned char seg_v[];
// 'seg_empty_h', 9x3px
extern const unsigned char seg_empty_h[];
// 'seg_empty_v', 3x9px
extern const unsigned char seg_empty_v[];


// 'clock', 12x12px
extern const unsigned char epd_bitmap_clock[];
// 'folder', 12x12px
extern const unsigned char epd_bitmap_folder[];
// 'graph', 12x12px
// extern const unsigned char epd_bitmap_graph[];
// 'data_arrow', 12x12px
extern const unsigned char epd_bitmap_graph[];
// 'keys', 12x12px
extern const unsigned char epd_bitmap_keys[];
// 'loop3', 12x12px
extern const unsigned char epd_bitmap_loop[];
// 'midi', 12x12px
extern const unsigned char epd_bitmap_midi[];
// 'wrench', 12x12px
extern const unsigned char epd_bitmap_wrench[];
// 'arp', 12x12px
extern const unsigned char epd_bitmap_arp[];
// 'heart', 12x12px
extern const unsigned char epd_bitmap_heart[];
// 'CD', 12x12px
extern const unsigned char epd_bitmap_CD[];
// 'fx', 12x12px
extern const unsigned char epd_bitmap_fx[];
// 'rec2_final', 12x12px
extern const unsigned char epd_bitmap_rec2_final[];
// 'rec1_final', 12x12px
extern const unsigned char epd_bitmap_rec1_final[];

// 'empty', 12x12px
extern const unsigned char default_bmp[];

// 'automation', 12x12px
extern const unsigned char epd_bitmap_automation[];
extern const unsigned char playback1_bmp[];
// 'playback2', 12x12px
extern const unsigned char playback2_bmp[];
// 'console', 12x12px
extern const unsigned char epd_bitmap_console[];
// 'hand', 12x12px
extern const unsigned char epd_bitmap_hand[];
// 'rnd2', 12x12px
extern const unsigned char random_bmp[];

//12x12px
extern const unsigned char send_file_bmp[];
// 'arp_idea', 12x12px
extern const unsigned char arp_bmp[];
// 'live_loop', 12x12px
extern const unsigned char live_loop_bmp[];

extern const unsigned char chop2_bmp[];
// 'chop', 12x12px
extern const unsigned char chop1_bmp[];

// 'tri', 16x8px
extern const unsigned char* drum_icon_tri[];
// 'tomS', 16x8px
extern const unsigned char* drum_icon_tomS[];
// 'tomM', 16x8px
extern const unsigned char* drum_icon_tomM[];
// 'tomL', 16x8px
extern const unsigned char* drum_icon_tomL[];
// 'snare', 16x8px
extern const unsigned char* drum_icon_snare[];
// 'shake', 16x8px
extern const unsigned char* drum_icon_shake[];
// 'rim', 16x8px
extern const unsigned char* drum_icon_rim[];
// 'kick', 16x8px
extern const unsigned char* drum_icon_kick[];
// 'hat', 16x8px
extern const unsigned char* drum_icon_hat[];
// 'hat_close', 16x8px
extern const unsigned char* drum_icon_hat_close[];
// 'hat_open', 16x8px
extern const unsigned char* drum_icon_hat_open[];
// 'gun', 16x8px
extern const unsigned char* drum_icon_gun[];
// 'clap', 16x8px
extern const unsigned char* drum_icon_clap[];
// 'cymbal', 16x8px
extern const unsigned char* drum_icon_cymbal[];
// 'bell', 16x8px
extern const unsigned char* drum_icon_bell[];
// '808', 16x8px
extern const unsigned char* drum_icon_808[];

extern const unsigned char* drum_icons[];

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 336)
extern const unsigned char* mainMenu_icons[];

//track menu icons
// 'delete', 9x5px
extern const unsigned char track_delete[];
// 'prime', 11x5px
extern const unsigned char track_prime[];
// 'mute', 11x5px
extern const unsigned char track_mute[];
// 'unmute', 11x5px
extern const unsigned char track_unmute[];
// 'latch', 5x5px
extern const unsigned char track_latch[];
// 'unlatch4', 5x5px
extern const unsigned char track_unlatch[];
// 'pencil', 11x5px
extern const unsigned char track_pencil[];
// 'solo', 11x5px
extern const unsigned char track_solo[];
// 'arrows', 11x5px
extern const unsigned char track_arrows[];
// 'eraser', 11x5px
extern const unsigned char track_eraser[];
//recmenu icons
//--------------------------------
// 'arrow', 13x14px
extern const unsigned char recmenu_arrow[];
// 'new_tracks_plus', 13x13px
extern const unsigned char recmenu_new_tracks_plus[];
// 'new_tracks_tracks', 29x19px
extern const unsigned char recmenu_new_tracks_tracks[];
// 'overwrite_paper', 24x21px
extern const unsigned char recmenu_overwrite_paper[];
// 'overwrite_pencil', 12x12px
extern const unsigned char recmenu_overwrite_pencil[];
// 'rec_for_arrow', 6x5px
extern const unsigned char recmenu_rec_for_arrow[];
// 'rec_for_cassette', 20x27px
extern const unsigned char recmenu_rec_for_cassette[];
// 'stop', 13x14px
extern const unsigned char recmenu_stop[];


// 'down_mode', 11x11px
extern const unsigned char arp_down_mode[];
// 'down_up', 11x11px
extern const unsigned char arp_down_up[];
// 'play_mode', 11x11px
extern const unsigned char arp_play_mode[];
// 'random_mode', 11x11px
extern const unsigned char arp_random_mode[];
// 'up_down', 11x11px
extern const unsigned char arp_up_down[];
// 'up_mode', 11x11px
extern const unsigned char arp_up_mode[];

//autotracks-----------------------
// 'autotracks_title', 49x7px
extern const unsigned char autotracks_bmp[];
// 'auto_label', 51x7px
extern const unsigned char automation_title_bmp[];
// 'autotrack', 44x7px
extern const unsigned char autotrack_bmp[];
// 'autotrack', 10x7px
extern const unsigned char autotrack1[];
// 'autotrack2', 10x7px
extern const unsigned char autotrack2[];


//banner-------
// 'bannerL', 12x9px
extern const unsigned char bannerL_bmp[];
// 'back_arrow_small', 10x9px
extern const unsigned char back_arrow_bmp[];
// 'back_arrow_small_highlight', 14x13px
extern const unsigned char back_arrow_light_bmp[];

//loop title --------------
// 'L', 7x9px
extern const unsigned char loop_L[];
// 'O', 7x9px
extern const unsigned char loop_O[];
// 'P', 7x9px
extern const unsigned char loop_P[];
// 'parenth', 5x9px
extern const unsigned char loop_parenth[];

//loop graphics
// 'semicircle', 3x6px
extern const unsigned char semicircle_bmp[];
// 'loop_arrow_special', 6x6px
extern const unsigned char loop_arrow_special_bmp[];
// 'return', 10x11px
extern const unsigned char loop_return_bmp[];

//loop icons
// 'inf', 9x5px
extern const unsigned char inf_bmp[];
// 'down_arrow', 7x7px
extern const unsigned char down_arrow_bmp[];
// 'rnd_equal', 11x7px
extern const unsigned char rnd_equal_bmp[];
// 'rnd', 7x7px
extern const unsigned char rnd_bmp[];
// 'return', 7x7px
extern const unsigned char return_bmp[];

// 'sine', 10x5px
extern const unsigned char sine_bmp[];
// 'sine_small', 6x4px
extern const unsigned char sine_small_bmp[];
// 'sine_small_reverse', 6x4px
extern const unsigned char sine_small_reverse_bmp[];

//CURSIVE FONT CHARACTERS----------------------------
// 'a', 6x6px
extern const unsigned char cursive_a[];
// 'b', 6x6px
extern const unsigned char cursive_b[];
// 'c', 6x6px
extern const unsigned char cursive_c[];
// 'd', 6x6px
extern const unsigned char cursive_d[];
// 'e', 6x6px
extern const unsigned char cursive_e[];
// 'f', 6x6px
extern const unsigned char cursive_f[];
// 'g', 6x6px
extern const unsigned char cursive_g[];
// 'h', 6x6px
extern const unsigned char cursive_h[];
// 'i', 6x6px
extern const unsigned char cursive_i[];
// 'k', 6x6px
extern const unsigned char cursive_k[];
// 'j', 6x6px
extern const unsigned char cursive_j[];
// 'l', 6x6px
extern const unsigned char cursive_l[];
// 'm', 6x6px
extern const unsigned char cursive_m[];
// 'n', 6x6px
extern const unsigned char cursive_n[];
// 'o', 6x6px
extern const unsigned char cursive_o[];
// 'p', 6x6px
extern const unsigned char cursive_p[];
// 'q', 6x6px
extern const unsigned char cursive_q[];
// 's', 6x6px
extern const unsigned char cursive_s[];
// 'r', 6x6px
extern const unsigned char cursive_r[];
// 't', 6x6px
extern const unsigned char cursive_t[];
// 'v', 6x6px
extern const unsigned char cursive_v[];
// 'x', 6x6px
extern const unsigned char cursive_x[];
// 'w', 6x6px
extern const unsigned char cursive_w[];
// 'z', 6x6px
extern const unsigned char cursive_z[];
// 'y', 6x6px
extern const unsigned char cursive_y[];
// 'u', 6x6px
extern const unsigned char cursive_u[];
// 'big_f', 6x12px
extern const unsigned char cursive_big_f[];
// 'big_g', 6x12px
extern const unsigned char cursive_big_g[];
// 'big_y', 6x12px
extern const unsigned char cursive_big_y[];
// 'big_p', 6x12px
extern const unsigned char cursive_big_p[];
// 'big_j', 6x12px
extern const unsigned char cursive_big_j[];
// 'big_b', 6x8px
extern const unsigned char cursive_big_b[];

// '0', 6x6px
extern const unsigned char cursive_0[];
// '9', 6x6px
extern const unsigned char cursive_9[];
// '8', 6x6px
extern const unsigned char cursive_8[];
// '7', 6x6px
extern const unsigned char cursive_7[];
// '6', 6x6px
extern const unsigned char cursive_6[];
// '5', 6x6px
extern const unsigned char cursive_5[];
// '4', 6x6px
extern const unsigned char cursive_4[];
// '3', 6x6px
extern const unsigned char cursive_3[];
// '2', 6x6px
extern const unsigned char cursive_2[];
// '1', 6x6px
extern const unsigned char cursive_1[];
// 'period', 6x6px
extern const unsigned char cursive_period[];
// 'dash', 6x6px
extern const unsigned char cursive_dash[];
// 'plus', 6x6px
extern const unsigned char cursive_plus[];

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 832)
extern const unsigned char* cursive_font[];

//---------------------------------------------------

// 'autobot2', 13x24px
extern const unsigned char robo2[];
// 'autobot1', 13x24px
extern const unsigned char robo1[];

extern const unsigned char* robo[];

// 'autobot3', 21x24px
extern const unsigned char robo3[];
// 'autobot4', 18x24px
extern const unsigned char robo4[];
// 'autobot5', 18x24px
extern const unsigned char robo5[];
// 'autobot6', 18x24px
extern const unsigned char robo6[];
// 'robo1_mask', 12x17px
extern const unsigned char robo1_mask[];

// 'robo3_mask', 18x17px
extern const unsigned char robo3_mask[];

// 'robo5_mask', 12x17px
extern const unsigned char robo5_mask[];

// 'speech_bubble', 18x22px
extern const unsigned char autobot_speech_bubble_bmp[];


// 'strum_text', 34x9px
extern const unsigned char strum_text[];

// 12x12px
extern const unsigned char file_export_bmp[];
// 'load', 12x12px
extern const unsigned char file_load_bmp[];
// 'del', 12x12px
extern const unsigned char file_delete_bmp[];
// 'overwrite', 12x12px
extern const unsigned char file_overwrite_bmp[];
// 'rename', 12x12px
extern const unsigned char file_rename_bmp[];
extern const unsigned char* file_menu_icons[];


// 'tool_bmp', 19x6px
extern const unsigned char tool_bmp[];

// 'generator_bmp', 44x6px
extern const unsigned char generator_bmp[];


//Stepchild Logo
//60x17px
extern const unsigned char stepchild_logo_bmp[];