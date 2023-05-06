#include <ncurses.h>
#include <form.h>
#include <string>
#include <cmath>

//min base is 2 (binary)
//max base is 36
const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int find_index(char symbol, int base = 36){
    for(int i = 0; i < base; i++){ 
        if(symbols[i] == symbol){
            return i;
        }
    }
    return -1;
}

char* trim(char *str)
{
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

// Big(O) difficulty of this algorithm is T(n + ) and S()
void convert_num(int currentBase, int finalBase, const char* currentNum, std::string& convertedNum){
    if(currentBase > 36 || currentBase < 2){ 
        convertedNum = "---INVALID CURRENT BASE---";
        return;
    }
    if(finalBase > 36 || finalBase < 2){
        convertedNum = "---INVALID FINAL BASE---";
        return;
    }
    int numSize = strlen(currentNum);
    const char* invalidNum = "---INVALID NUMBER---";

    int pointCount = 0;
    for(int i = 0; i < numSize; i++){
        bool is_invalid = (find_index(currentNum[i], currentBase) == -1) && (currentNum[i] != '.');
        bool is_minus = (i == 0) && (currentNum[0] == '-');
        if(is_invalid && !is_minus){
            convertedNum = invalidNum;
            return;
        }
        else if(currentNum[i] == '.') pointCount++;
    }
    if(pointCount > 1){
        convertedNum = invalidNum;
        return;
    }
    // no need to convert (bases are the same)
    if(currentBase == finalBase){
        convertedNum = currentNum;
        return;
    }

    bool isFloat = pointCount == 1;
    if(isFloat){
        long double decimalNum = 0;
        // do smth
    }
    else{
        unsigned long long int decimalNum = 0;
        // converts currentNum to decimal
        for(int i = 0; i < numSize; i++){
            if (currentNum[i] != '-'){
                decimalNum += find_index(currentNum[i]) * pow(currentBase, numSize - i - 1);
            }
        }
        if(finalBase == 10){
            convertedNum = std::to_string(decimalNum);
            return;
        }
        // converts from decimal to final base
        while(true){
            int divRemain = decimalNum % finalBase;
            decimalNum = std::floor(decimalNum / finalBase);
            convertedNum = symbols[divRemain] + convertedNum;
            if(decimalNum < finalBase){
                if (decimalNum != 0) convertedNum = symbols[decimalNum] + convertedNum;
                break;
            }
        }
        if(currentNum[0] == '-') convertedNum = '-' + convertedNum;
    }
}

int main(){
    initscr();
    noecho();
    keypad(stdscr, true);
    int scrHeight, scrWidth;
    getmaxyx(stdscr, scrHeight, scrWidth);
    int scrMidH = scrHeight / 2;
    int scrMidW = scrWidth / 2;

	WINDOW* win_body = newwin(34, 60, scrMidH - 17, scrMidW - 30);
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
