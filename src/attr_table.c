// Attr stuff
WINDOW *attrWindow;
PANEL *attrPanel;

#define ATTR_HEIGHT 13
#define ATTR_WIDTH 23
#define ATTR_TEST_Y (ATTR_HEIGHT - 2)
#define ATTR_TEST_X 11
#define ATTR_COLOR_X 11
#define ATTR_BOLD_X 8
#define ATTR_UNDERLINE_X 13

void InitAttrTable () {
	attrWindow = CreateCenteredBoxedTitledWindow (ATTR_HEIGHT, ATTR_WIDTH,
			"PALETTE");
	attrPanel = new_panel (attrWindow);

	const char *colors[] = {
		"1  normal",
		"2   black",
		"3     red",
		"4   green",
		"5  yellow",
		"6    blue",
		"7 magenta",
		"8    cyan",
		"9   white"
	};

	int i;
	// for each color with normal background
	for (i = 0; i < COLORS_STEP; i ++) {
		// print the color name, colored
		wattron (attrWindow, COLOR_PAIR (i * COLORS_STEP));
		mvwaddstr (attrWindow, 1 + i, 1, colors[i]);
	}

	wattron (attrWindow, A_UNDERLINE);
	mvwaddstr (attrWindow, ATTR_TEST_Y - 1, 1, "0 Underline");
	wstandend (attrWindow);
	waddstr (attrWindow, "[ ]");
	wattron (attrWindow, A_BOLD);
	mvwaddstr (attrWindow, ATTR_TEST_Y, 1, "a Bold");
	wstandend (attrWindow);
	waddstr (attrWindow, "[ ]  TestArea");
}


int chooseAttr (mos_attr current_color) {
	// input from user
	int c = 0;

	// is current_color bold?
	mos_attr current_bold = extractBold (&current_color);
	mos_attr current_underline = extractUnderline (&current_color);
	// current chosen foreground, current chosen background, bold
	mos_attr attrs[] = {
		GetFore (current_color),
		GetBack (current_color),
		current_underline,
		current_bold
	};
	int moving = 0;

	PrintHud (FALSE, "Choose colors with arrows. '0' for underline, 'a' for bold");
	do {
		switch (c) {
			// switch moving
			case KEY_LEFT:
			case KEY_RIGHT:
				moving = !moving;
				break;

			// switch values
			case KEY_UP:
				// erase } moving, before moving it
				mvwaddstr (attrWindow, 1 + attrs[moving],
						ATTR_COLOR_X, "           ");
				attrs[moving]--;
				if (attrs[moving] > COLORS_STEP) {
					attrs[moving] = COLORS_STEP - 1;
				}
				break;

			case KEY_DOWN:
				// erase } moving, before moving it
				mvwaddstr (attrWindow, 1 + attrs[moving],
						ATTR_COLOR_X, "           ");
				attrs[moving]++;
				attrs[moving] %= COLORS_STEP;
				break;

			// toggle UNDERLINE
			case '0':
				attrs[2] ^= UNDERLINE;
				break;

			// toggle BOLD
			case 'a':
				attrs[3] ^= BOLD;
				break;

			// cancelled, so return what came
			case KEY_ESC:
				// erase } fore back
				mvwaddstr (attrWindow, 1 + attrs[0],
						ATTR_COLOR_X, "           ");
				mvwaddstr (attrWindow, 1 + attrs[1],
						ATTR_COLOR_X, "           ");
				return (current_color | current_bold | current_underline);

			default:
				if (isdigit (c)) {
					// erase } moving, before moving it
					mvwaddstr (attrWindow, 1 + attrs[moving],
							ATTR_COLOR_X, "           ");
					attrs[moving] = c - '1';
					attrs[moving] %= COLORS_STEP;
				}
		}

		// we write back before fore, so when they're both
		// on the same line, both of them appear
		mvwaddstr (attrWindow, 1 + attrs[1], ATTR_COLOR_X, "}      back");
		mvwaddstr (attrWindow, 1 + attrs[0], ATTR_COLOR_X, "} fore");
		mvwaddch (attrWindow, ATTR_TEST_Y - 1, ATTR_UNDERLINE_X,
				attrs[2] ? 'X' : ' ');
		mvwaddch (attrWindow, ATTR_TEST_Y, ATTR_BOLD_X,
				attrs[3] ? 'X' : ' ');

		// Underline the moving attr
		if (moving) {
			mvwchgat (attrWindow, 1 + attrs[moving], ATTR_COLOR_X + 7, 4,
					A_UNDERLINE, Normal, NULL);
		}
		else {
			mvwchgat (attrWindow, 1 + attrs[moving], ATTR_COLOR_X + 2, 4,
					A_UNDERLINE, Normal, NULL);
		}
		// changes " TestArea " attributes
		mvwchgat (attrWindow, ATTR_TEST_Y, ATTR_TEST_X, 10,
				(attrs[2] ? A_UNDERLINE : 0) | (attrs[3] ? A_BOLD : 0),
				attrs[0] * COLORS_STEP + attrs[1], NULL);

		wrefresh (attrWindow);

		c = getch ();
	} while (c != '\n');

	// erase } fore back
	mvwaddstr (attrWindow, 1 + attrs[0], ATTR_COLOR_X, "           ");
	mvwaddstr (attrWindow, 1 + attrs[1], ATTR_COLOR_X, "           ");

	return ((attrs[0] * COLORS_STEP + attrs[1]) | attrs[2] | attrs[3]);
}


mos_attr AttrTable (CURS_MOS *current, Cursor cur) {
	if (!attrPanel) {
		InitAttrTable ();
	}

	show_panel (attrPanel);
	update_panels ();
	doupdate ();

	mos_attr value = chooseAttr (mosGetAttr (current->img, cur.y, cur.x));

	hide_panel (attrPanel);
	update_panels ();
	doupdate ();

	return value;
}


