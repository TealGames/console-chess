#pragma once
#include <wx/wx.h>
#include "GameManager.hpp"

void CreateCaptureDisplay(Core::GameManager& manager, wxWindow* parent);
void CreateWinChanceDisplay(Core::GameManager& manager, wxWindow* parent);
