#define NOMINMAX
#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include<fstream>
#include "DXLib_ref/DXLib_ref.h"

#include "Box2D/Box2D.h"

#include "sub.hpp"
#include "map.hpp"
#include "chara.hpp"
#include "UI.hpp"
#include "scene.hpp"

#include "route.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	auto Main_c = std::make_shared<main_c>();
	return 0;
}
