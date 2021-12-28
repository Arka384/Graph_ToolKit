#pragma once
#include "VisualizationManager.hpp"
#include <Windows.h>
#include <WinUser.h>

constexpr int ID_IMPORT = 111;
constexpr int ID_EXPORT = 222;
constexpr int ID_EDITOROPTIONS = 333;

constexpr int ID_ADDEDGE = 10;
constexpr int ID_DELEDGE = 11;
constexpr int ID_ADDVERTEX = 20;
constexpr int ID_DELVERTEX = 21;

constexpr int ID_SETTINGS = 3;
constexpr int ID_CLEAR = 4;
constexpr int ID_WEIGHTED = 5;
constexpr int ID_DIRECTED = 6;
constexpr int ID_STARTWITHZERO = 7;
constexpr int ID_USEALPHA = 8;
constexpr int ID_SAVE = 9;

constexpr int ID_VISUALIZEBFS = 100;
constexpr int ID_VISUALIZEDFS = 110;
constexpr int ID_VISUALSETTINGS = 120;
constexpr int ID_VISUALCLOSE = 130;
constexpr int ID_VS_COLOUR = 500;
constexpr int ID_VS_SPEED = 510;


constexpr int ID_VERTEXCOLORCHANGE = 200;
constexpr int ID_VERTEXNUMCOLORCHANGE = 210;
constexpr int ID_VERTEXSIZECHANGE = 220;
constexpr int ID_VERTEXFONTSIZECHANGE = 230;

constexpr int ID_EDGECOLORCHANGE = 300;
constexpr int ID_EDGEWEIGHTCOLORCHANGE = 310;
constexpr int ID_EDGESIZECHANGE = 320;
constexpr int ID_EDGEWEIGHTSIZECHANGE = 330;

constexpr int ID_SCATTERVERTICES = 400;

constexpr int ID_EDITORCLOSE = 410;