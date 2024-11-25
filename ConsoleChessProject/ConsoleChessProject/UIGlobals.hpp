#pragma once
#include "wx/wx.h"
#include "Globals.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

static const wxColour DARK_GRAY = wxColour( 43, 43, 43 );
static const wxColour LIGHT_DARK_GRAY= wxColour(53, 53, 53);
static const wxColour LIGHT_GRAY = wxColour(63, 63, 63 );

static const wxColour LIGHT_GREEN = wxColour(127, 180, 89 );
static const wxColour DARKER_LIGHT_GREEN = wxColour(83, 118, 59);
static const wxColour TAN = wxColour(241, 236, 216);
static const wxColour DARKER_TAN = wxColour(200, 195, 175);

static const wxColour MUTED_WHITE = wxColour(247, 247, 247);
static const wxColour DARKER_MUTED_WHITE = wxColour(220, 220, 220);

static const wxColour MUTED_BRIGHT_YELLOW = wxColour(198, 167, 42);
static const wxColour BRIGHT_YELLOW= wxColour(225, 190, 51, 128);

static const wxColour RED = wxColour(167, 70, 71);

static const wxColour LIGHT_GRAY_BLUE = wxColour(152, 201, 236, 128);
static const wxColour GRAY_BLUE= wxColour(114, 166, 204);
static const wxColour DARKER_GRAY_BLUE = wxColour(86, 127, 157);

static const wxColour LIGHT_DEEP_BLUE = wxColour(113, 176, 223, 128);
static const wxColour DEEP_BLUE = wxColour(69, 128, 171);
static const wxColour DARKER_DEEP_BLUE = wxColour(51, 93, 123);

//COLOR ALIASES
static wxColour BACKGROUND_COLOR = DARK_GRAY;
static wxColour LIGHTER_SECONDARY_COLOR = LIGHT_DARK_GRAY;
static wxColour LIGHTER_SECONDARY_COLOR_2 = LIGHT_GRAY;

static const wxColour LIGHT_CELL_COLOR = DARKER_GRAY_BLUE;
static const wxColour LIGHT_CELL_HOVER_COLOR = GRAY_BLUE;
static const wxColour LIGHT_CELL_SELECTED_COLOR = LIGHT_GRAY_BLUE;
static const wxColour LIGHT_CELL_MOVE_COLOR = BRIGHT_YELLOW;
static const wxColour LIGHT_CELL_CAPTURE_MOVE_COLOR = RED;
static const wxColour LIGHT_CELL_PREVIOUS_MOVE_COLOR = MUTED_BRIGHT_YELLOW;

static const wxColour DARK_CELL_COLOR = DARKER_DEEP_BLUE;
static const wxColour DARK_CELL_HOVER_COLOR = DEEP_BLUE;
static const wxColour DARK_CELL_SELECTED_COLOR = LIGHT_DEEP_BLUE;
static const wxColour DARK_CELL_MOVE_COLOR = BRIGHT_YELLOW;
static const wxColour DARK_CELL_CAPTURE_MOVE_COLOR = RED;
static const wxColour DARK_CELL_PREVIOUS_MOVE_COLOR = MUTED_BRIGHT_YELLOW;


//FONT SETTINGS
static constexpr int DEFAULT_FONT_SIZE = 15;
static const wxFontFamily FONT_FAMILY = wxFONTFAMILY_SCRIPT;
static const wxFont TITLE_FONT = wxFont(DEFAULT_FONT_SIZE * 2, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
static const wxFont HEADING_FONT = wxFont(DEFAULT_FONT_SIZE, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);