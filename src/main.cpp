#include <lua.hpp>
#include <ncurses/ncurses.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

bool running{true};

WINDOW *win;
int screen_width{0}, screen_height{0};

lua_State *lua;

int getinput;

string dialog{"Type in the words, Press enter to complete the battle..."};
string tips{"And save the little village or something!"};

vector<string> words;
int level = 1;
int experience = 0;

string wordInput{""};
int wordsComplete = 0;

int exp_to_next()
{
	return (int)(level * 2 + ((level * 17) / level) / 8);
}

void get_new_words()
{
	int count = (int)(level * (level + 1 / (level)));
	wordsComplete = 0;
	words.clear();
	for(int i = 0; i < count; i++)
	{
		lua_getglobal(lua, "random_word");
		lua_call(lua, 0, 1);
		
		words.push_back(lua_tostring(lua, -1));
		lua_pop(lua, 1);
	}
}

void process_start()
{
	lua_getglobal(lua, "start");
	lua_call(lua, 0, 0);
}

void process_input()
{
	if(dialog.size() > 0)
	{
		dialog.clear();
		
		lua_getglobal(lua, "next_poem");
		lua_call(lua, 0, 1);
		
		tips = lua_tostring(lua, -1);
		lua_pop(lua, 1);
	}
	else
	{
		if((getinput >= 'A' && getinput <= 'Z') ||
			(getinput >= 'a' && getinput <= 'z'))
		{
			wordInput.insert(wordInput.end(), getinput);
		}
		if(getinput == 8 && wordInput.size() >= 1)
		{
			wordInput.erase(wordInput.end()-1);
		}
		if(getinput == '\n')
		{
			if(wordInput == words[wordsComplete])
			{
				wordsComplete++;
				experience++;
				wordInput.clear();
				if(wordsComplete >= words.size())
				{
					get_new_words();
				}
			}
		}

		if(experience >= exp_to_next())
		{
			level++;
			experience = 0;
			dialog = "Level Up!";
		}
	}

	lua_getglobal(lua, "update");
	lua_call(lua, 0, 0);
	
	if(getinput == (char)27 || level > 5)
	{
		running = false;
	}
}

int main(int argc, char *argv[])
{	
	setlocale(LC_CTYPE, "");
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	
	raw();

	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);

	lua = luaL_newstate();
	luaL_openlibs(lua);
	luaL_loadfile(lua, "main.lua");
	lua_call(lua, 0, 0);

	process_start();

	get_new_words();

	while(running)
	{
		int nscreen_width, nscreen_height;
		getmaxyx(stdscr, nscreen_height, nscreen_width);
		
		if(screen_width != nscreen_width &&
			screen_width != nscreen_width)
		{
			screen_width = nscreen_width;
			screen_height = nscreen_height;
			delwin(win);
			int height = screen_height-0;
			int width = screen_width-0;
			win = newwin(height, width, 0, 0);
		}
		
		clear();
		refresh();
		
		wclear(win);
		box(win, 0, 0);
		if(dialog.size() > 0 && tips.size() > 0)
		{
			wattron(win, COLOR_PAIR(2));
			wmove(win, screen_height / 2, screen_width / 2 - dialog.size() / 2);
			wprintw(win, "%s", dialog.c_str());
			wattroff(win, COLOR_PAIR(2));
			wattron(win, COLOR_PAIR(3));
			wmove(win, screen_height / 2 + 2, screen_width / 2 - tips.size() / 2);
			wprintw(win, "%s", tips.c_str());
			wattroff(win, COLOR_PAIR(3));
		}
		else
		{
			int count = 0;
			
			for(string word : words)
			{
				if(count < wordsComplete)
				{
					wattroff(win, COLOR_PAIR(1));
					wattron(win, COLOR_PAIR(2));
				}
				else
				{
					wattroff(win, COLOR_PAIR(2));
					wattron(win, COLOR_PAIR(1));
				}
				wmove(win, 1 + count, screen_width / 2 - word.size() / 2);
				wprintw(win, "%s", word.c_str());
				count++;
			}
			
			char level_str[48];
			sprintf(level_str, "level: [%i] exp: %i / %i", level, experience, exp_to_next());
			string level_bar{level_str};
			wmove(win, screen_height - 2, 1);
			wprintw(win, "%s", level_bar.c_str());
			if(wordInput.size() > 0)
			{
				wmove(win, screen_height - 1, screen_width / 2 - wordInput.size() / 2);
				wprintw(win, "%s", wordInput.c_str());
			}
		}

		wnoutrefresh(win);
		
		doupdate();
		
		getinput = 0;
		getinput = getch();
		process_input();
	}
	
	if(level > 5)
	{
		lua_getglobal(lua, "maxstory");
		int maxstory = lua_tointeger(lua, -1);
		lua_pop(lua, 1);

		vector<string> gameover;
		for(int i = 0; i < maxstory; i++)
		{
			clear();
			refresh();
			
			wclear(win);
			box(win, 0, 0);

			lua_getglobal(lua, "next_story");
			lua_call(lua, 0, 1);
			
			string story{ lua_tostring(lua, -1) };
			lua_pop(lua, 1);

			wmove(win, screen_height/ 2, screen_width / 2 - story.size() / 2);
			wprintw(win, "%s", story.c_str());
			wnoutrefresh(win);
			
			doupdate();
			getch();
		}
	}

	lua_close(lua);
	endwin();
	return 0;
}