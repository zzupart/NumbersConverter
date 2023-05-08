#include <ncurses.h>
#include <form.h>
#include <string>
#include <cmath>

//min base is 2 (binary)
//max base is 36
const char* g_symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int find_index (char arg_symbol, int arg_base = 36) {
    for (int i = 0; i < arg_base; i++) { 
        if (g_symbols[i] == arg_symbol) {
            return i;
        }
    }
    return -1;
}

char* trim (char *arg_str) {
	char *end;

	// trim leading space
	while (isspace(*arg_str)) {
		arg_str++;
    }

	if (*arg_str == 0) { // all spaces?
		return arg_str;
    }

	// trim trailing space
	end = arg_str + strnlen(arg_str, 128) - 1;

	while (end > arg_str && isspace(*end)) {
		end--;
    }

	// write new null terminator
	*(end+1) = '\0';

	return arg_str;
}

// Big(O) difficulty of this algorithm is T(n + ) and S()
void convert_num (int arg_current_base, int arg_final_base, char* arg_current_num,
                                                  std::string& arg_converted_num) {
    if (arg_current_base > 36 || arg_current_base < 2) {  
        arg_converted_num = "---INVALID CURRENT BASE---";
        return;
    }
    if (arg_final_base > 36 || arg_final_base < 2) {
        arg_converted_num = "---INVALID FINAL BASE---";
        return;
    }
    int num_size = strlen(arg_current_num);
    const char* invalid_num = "---INVALID NUMBER---";

    int point_count = 0;
    for (int i = 0; i < num_size; i++) {
        bool is_invalid = (find_index(arg_current_num[i], arg_current_base) == -1) && (arg_current_num[i] != '.');
        bool is_minus = (i == 0) && (arg_current_num[0] == '-');
        if (is_invalid && !is_minus) {
            arg_converted_num = invalid_num;
            return;
        }
        else if (arg_current_num[i] == '.') {
            point_count++;
        }
    }
    if (point_count > 1) {
        arg_converted_num = invalid_num;
        return;
    }
    // no need to convert (bases are the same)
    if (arg_current_base == arg_final_base) {
        arg_converted_num = arg_current_num;
        return;
    }

    bool is_float = point_count == 1;
    if (is_float) {
        long double decimal_num = 0;
        // do smth
    }
    else {
        unsigned long long int decimal_num = 0;
        // converts currentNum to decimal
        for (int i = 0; i < num_size; i++) {
            if (arg_current_num[i] != '-') {
                decimal_num += find_index(arg_current_num[i]) * pow(arg_current_base, num_size - i - 1);
            }
        }
        if (arg_final_base == 10) {
            arg_converted_num = std::to_string(decimal_num);
            return;
        }
        // converts from decimal num to final base
        while (true) {
            int div_remain = decimal_num % arg_final_base;
            decimal_num = std::floor(decimal_num / arg_final_base);
            arg_converted_num = g_symbols[div_remain] + arg_converted_num;
            if (decimal_num < arg_final_base) {
                if (decimal_num != 0) {
                    arg_converted_num = g_symbols[decimal_num] + arg_converted_num;
                }
                break;
            }
        }
        if (currentNum[0] == '-') {
            convertedNum = '-' + convertedNum;
        }
    }
}

int main () {
    initscr();
    noecho();
    keypad(stdscr, true);
    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);
    // middle of the screens height
    int scr_midh = scr_height / 2;
    // middle of the screens width
    int scr_midw = scr_width / 2; 

	WINDOW* win_body = newwin(34, 60, scr_midh - 17, scr_midw - 30);
	box(win_body, 0, 0);
	WINDOW* win_form = derwin(win_body, 30, 58, 2, 1);
	box(win_form, 0, 0);
    WINDOW* win_label = derwin(win_body, 3, 21, 0, 20);
    box(win_label, 0, 0);
    mvwprintw(win_label, 1, 2, "NUMBERS CONVERTER");
    
	FIELD* fields[5];
	fields[0] = new_field(1, 40, 4, 8, 0, 0);
	fields[1] = new_field(1, 4, 8, 26, 0, 0);
	fields[2] = new_field(1, 4, 12, 26, 0, 0);
	fields[3] = new_field(3, 40, 20, 8, 0, 0);
	fields[4] = NULL;

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_STATIC);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_STATIC);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_STATIC);
    set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_AUTOSKIP);
    field_opts_off(fields[3], O_WRAP);

    set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[2], A_UNDERLINE);
    set_field_back(fields[3], A_UNDERLINE);

	FORM *form = new_form(fields);
	set_form_win(form, win_form);
	set_form_sub(form, derwin(win_form, 28, 56, 1, 1));
	post_form(form);

	refresh();
	wrefresh(win_body);
	wrefresh(win_form);
    wrefresh(win_label);
    pos_form_cursor(form);

    int ch;
	while ((ch = getch()) != KEY_F(1)){
		switch(ch){
    		case KEY_DOWN:
			    form_driver(form, REQ_NEXT_FIELD);
		    	form_driver(form, REQ_END_LINE);
	    		break;
    		case KEY_UP:
			    form_driver(form, REQ_PREV_FIELD);
		    	form_driver(form, REQ_END_LINE);
	    		break;
    		case KEY_LEFT:
		    	form_driver(form, REQ_PREV_CHAR);
	    		break;
    		case KEY_RIGHT:
			    form_driver(form, REQ_NEXT_CHAR);
		    	break;
	    	// Delete the char before cursor (BACKSPACE)
    		case 127:
		    	form_driver(form, REQ_DEL_PREV);
	    		break;
    		default:
                form_driver(form, ch);
                // if current and final bases are not empty
                if(*trim(field_buffer(fields[1], 0)) != 0 && *trim(field_buffer(fields[2], 0)) != 0){
                    form_driver(form, REQ_VALIDATION);
                    std::string convertedNum;
                    convert_num(atoi(trim(field_buffer(fields[1], 0))), 
                                atoi(trim(field_buffer(fields[2], 0))),
                              trim(field_buffer(fields[0], 0)), convertedNum);                        
                    set_field_buffer(fields[3], 0, convertedNum.c_str());
                    form_driver(form, REQ_VALIDATION);
                }

	    		break;
    	}
        wrefresh(win_form);
    }

	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(win_form);
	delwin(win_body);
	endwin();

    return 0;
}
