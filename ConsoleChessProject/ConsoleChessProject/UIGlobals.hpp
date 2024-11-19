#pragma once
#include "wx/wx.h"
#include "Globals.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

static const wxColour DARK_GRAY{ 43, 43, 43 };
static const wxColour LIGHT_GREEN{ 127, 180, 89 };
static const wxColour DARKER_LIGHT_GREEN{ 83, 118, 59};

static const wxColour MUDDY_WHITE{ 247, 247, 247 };
static const wxColour DARKER_MUDDY_WHITE{ 220, 220, 220 };

static const wxColour TAN{ 241, 236, 216 };
static const wxColour DARKER_TAN{ 200, 195, 175};

static const wxColour BRIGHT_YELLOW{ 229, 229, 62 };

//COLOR ALIASES
static const wxColour LIGHT_CELL_COLOR = DARKER_TAN;
static const wxColour LIGHT_CELL_HOVER_COLOR = TAN;
static const wxColour LIGHT_CELL_HIGHLIGHT_COLOR = BRIGHT_YELLOW;

static const wxColour DARK_CELL_COLOR = DARKER_LIGHT_GREEN;
static const wxColour DARK_CELL_HOVER_COLOR = LIGHT_GREEN;
static const wxColour DARK_CELL_HIGHLIGHT_COLOR = BRIGHT_YELLOW;