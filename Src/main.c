#include "../lib/printf.h"
#include "../lib/stmlib.h"
#include "../drivers/spi_tft.h"

void _putchar(char character)
{
    uart_putc(character);
}

#define MAX_OPT 3

btn_t btn_ok;
btn_t btn_dn;
btn_t btn_bk;

struct option_t
{
	uint16_t tcolor[2];
	uint16_t bcolor;
	uint8_t y_pos;
	uint8_t x_pos;
	char* buffer;
	uint8_t select;
	struct option_t* next[2];
	struct option_t* prev[2];
	tft_t* tft;
	void (*action)(struct option_t*);
};

uint16_t color_palette[DEFINED_COLORS] = {BLACK,BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW,WHITE,ORANGE,LIGHTGREEN};
char* color_name[DEFINED_COLORS] = {"Black","Blue","Red","Green","Cyan","Magenta","Yellow","White","Orange","L-Green"};
uint8_t sys_bkg_color_idx = 0;
uint8_t sys_sel_color_idx = 4;
uint8_t sys_txt_color_idx = 6;

void option_init(struct option_t* option, char* name, uint8_t ypos, uint8_t xpos, uint16_t nortext,
		uint16_t seltext, uint16_t bg, struct option_t* prev0, struct option_t* prev1,
		struct option_t* next0, struct option_t* next1, tft_t* tft, void (*action)(struct option_t*)) {
	option->tcolor[0] = nortext;
	option->tcolor[1] = seltext;
	option->bcolor = bg;
	option->y_pos = ypos;
	option->x_pos = xpos;
	option->buffer = name;
	option->tft = tft;
	option->select = 0;
	option->prev[0] = prev0;
	option->prev[1] = prev1;
	option->next[0] = next0;
	option->next[1] = next1;
	option->action = action;
}

void ui_menu(struct option_t* option, uint8_t max_opt) {
	for (uint8_t i = 0; i < max_opt; i++) {
		tft_write_string(option[i].tft,
				option[i].x_pos,
				option[i].y_pos+5,
				option[i].buffer,
				option[i].tcolor[option[i].select],
				option[i].bcolor
				);
	}
}

void ui_select_option(struct option_t* option, uint8_t max_opt, btn_t* btn) {
	if (stm_lib_read_btn(btn)) {
		delayms(200);
		for (uint8_t i = 0; i < max_opt; i++) {
			if (option[i].select == 1) {
				option[i].select = 0;
				if (i == (max_opt-1)) {
					option[0].select = 1;
				}
				else {
					option[i + 1].select = 1;
				}
				break;
			}
		}
	}
	ui_menu(option, max_opt);
}

void ui_update_selector(struct option_t* option, uint8_t max_opt) {
	for (uint8_t i = 0; i < max_opt; i++) {
		if (option[i].select == 1) {
			tft_new_bar(option[i].tft, color_palette[sys_sel_color_idx], option[i].y_pos);
		}
		else {
			tft_new_bar(option[i].tft, BLACK, option[i].y_pos);
		}
	}
}

void ui_execute(struct option_t* option, uint8_t max_opt, btn_t* btn) {
	if (stm_lib_read_btn(btn)) {
		delayms(200);
		for (uint8_t i = 0; i < max_opt; i++) {
			if (option[i].select) {
				option[i].action(&option[i]);
			}
		}
	}
}

void ui_about(struct option_t* opt) {
	char* text = "MediaPlayer v0.1";
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
	tft_write_string(opt->tft, (ST_WIDTH/2)-((strlen(opt->buffer)/2)*9), 0,
			opt->buffer, opt->tcolor[opt->select], opt->bcolor);
	tft_write_string(opt->tft, 0, 32,
				text, opt->tcolor[opt->select], opt->bcolor);
	while (!(stm_lib_read_btn(&btn_bk))) {
	}
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
}

void ui_settings(struct option_t* opt) {
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
	tft_write_string(opt->tft, (ST_WIDTH/2)-((strlen(opt->buffer)/2)*9), 0,
			opt->buffer, opt->tcolor[opt->select], opt->bcolor);
	while (!(stm_lib_read_btn(&btn_bk))) {
		}
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
}

void ui_color_sys_bkg(struct option_t* opt) {
	while (!(stm_lib_read_btn(&btn_dn))) {
		delayms(200);
		if (stm_lib_read_btn(&btn_ok)) {
			sys_bkg_color_idx++;
			if (sys_bkg_color_idx == DEFINED_COLORS) {
				sys_bkg_color_idx = 0;
			}
			tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, "          ",
							opt->tcolor[opt->select], opt->bcolor);
		}
		tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, color_name[sys_bkg_color_idx],
				color_palette[sys_bkg_color_idx], opt->bcolor);
	}
}
void ui_color_sys_sel(struct option_t* opt) {
	while (!(stm_lib_read_btn(&btn_dn))) {
		delayms(200);
		if (stm_lib_read_btn(&btn_ok)) {
			sys_sel_color_idx++;
			if (sys_sel_color_idx == DEFINED_COLORS) {
				sys_sel_color_idx = 0;
			}
			tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, "          ",
							opt->tcolor[opt->select], opt->bcolor);
		}
		tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, color_name[sys_sel_color_idx],
				color_palette[sys_sel_color_idx], opt->bcolor);
	}
}
void ui_color_sys_txt(struct option_t* opt) {
	while (!(stm_lib_read_btn(&btn_dn))) {
		delayms(200);
		if (stm_lib_read_btn(&btn_ok)) {
			sys_txt_color_idx++;
			if (sys_txt_color_idx == DEFINED_COLORS) {
				sys_txt_color_idx = 0;
			}
			tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, "          ",
							opt->tcolor[opt->select], opt->bcolor);
		}
		tft_write_string(opt->tft, 10 + (strlen(opt->buffer)*8), 5 + opt->y_pos, color_name[sys_txt_color_idx],
				color_palette[sys_txt_color_idx], opt->bcolor);
	}
}

void ui_color(struct option_t* opt) {
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
	tft_write_string(opt->tft, (ST_WIDTH/2)-((strlen(opt->buffer)/2)*9), 0, opt->buffer, opt->tcolor[opt->select], opt->bcolor);
	struct option_t sett[3];
	option_init(&sett[0], "BG   ", 24,       3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, opt->tft, ui_color_sys_bkg);
	option_init(&sett[1], "Theme", 24+16,    3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, opt->tft, ui_color_sys_sel);
	option_init(&sett[2], "Font ", 24+16+16, 3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, opt->tft, ui_color_sys_txt);
	ui_menu(sett, 3);
	sett[0].select = 1;
	while (!(stm_lib_read_btn(&btn_bk))) {
		ui_select_option(sett, 3, &btn_dn);
		ui_update_selector(sett, 3);
		ui_execute(sett, 3, &btn_ok);
	}
	tft_fill_rect(opt->tft, 0, 0, ST_WIDTH, ST_HEIGHT, color_palette[sys_bkg_color_idx]);
}


int main(void)
{
	enable_hse();
	systick_init();

	tft_t tft;
	struct option_t option[MAX_OPT];
	//uint8_t selected = 0, idx = 0;

	btn_ok.port = GPIOA;
	btn_ok.pin  = 3;
	btn_dn.port = GPIOA;
	btn_dn.pin  = 4;
	btn_bk.port = GPIOA;
	btn_bk.pin  = 2;

	option_init(&option[0], "Colours ", 0,  3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, &tft, ui_color);
	option_init(&option[1], "Settings", 16, 3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, &tft, ui_settings);
	option_init(&option[2], "About   ", 32, 3, YELLOW, WHITE, BLACK, NULL, NULL, NULL, NULL, &tft, ui_about);

	stm_lib_init_gpio(GPIOA);
	stm_lib_init_gpio(GPIOB);
	stm_lib_init_gpio(GPIOC);
	stm_lib_config_gpio(GPIOC, 13, MODE_OUTPUT_50_GP_OD);
	stm_lib_config_gpio(GPIOA, 4, MODE_INPUT_FLOAT);
	stm_lib_config_gpio(GPIOA, 3, MODE_INPUT_FLOAT);
	stm_lib_config_gpio(GPIOA, 2, MODE_INPUT_FLOAT);
	stm_lib_spi_init(1);
	tft_init(&tft, 1, GPIOB, 10, GPIOB, 1, GPIOB, 0);
	tft_fill_rect(&tft, 0, 0, ST_WIDTH, ST_HEIGHT, BLACK);

	option[0].select = 1;
	tft_new_bar(&tft, WHITE, option[0].y_pos);
	ui_menu(option, MAX_OPT);

	while (1) {
		ui_select_option(option, MAX_OPT, &btn_dn);
		ui_update_selector(option, MAX_OPT);
		ui_execute(option, MAX_OPT, &btn_ok);
	}
}
