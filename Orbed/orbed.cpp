////////////////////////////////////////////////////////////////////////////////
// orbed.cpp
////////////////////////////////////////////////////////////////////////////////

#include "orbed.h"
#include "imgui\imgui.h"
#include "intromem.h"
#include "rocket.h"

//#define IMGUI_DEMO

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static bool sAquired = false;
static bool sUpdated = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static bool sRocketConnected = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool Orbed_Init()
{
	if (!Rocket_Init())
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Orbed_Deinit()
{
	IntroMem_Release();

	Rocket_Deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Orbed_Update()
{
	if (!sRocketConnected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 64, 0, 255));
		if (ImGui::Button("Rocket Connect"))
		{
			sRocketConnected = Rocket_Connect();
		}
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64, 64, 64, 255));
		ImGui::Button("Rocket Connected");
		ImGui::PopStyleColor();
	}

	if (sRocketConnected)
	{
		sRocketConnected = Rocket_Update();
	}

	bool connect = false;
	bool disconnect = false;

	if (!sAquired)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 64, 0, 255));
		connect |= ImGui::Button("Connect");
		ImGui::PopStyleColor();
	}
	else
	{
		if (!sUpdated)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
			connect |= ImGui::Button("Reconnect");
			disconnect |= connect;
			ImGui::PopStyleColor();
		}

		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64, 0, 0, 255));
		disconnect |= ImGui::Button("Disconnect");
		ImGui::PopStyleColor();
	}

	if (disconnect)
	{
		IntroMem_Release();

		sAquired = false;
	}

	if (connect)
	{
		sAquired = IntroMem_Aquire();
	}

	sUpdated = false;

	if (sAquired)
	{
		sUpdated = IntroMem_Update();
	}

	#if defined(IMGUI_DEMO)
	ImGui::ShowDemoWindow();
	#endif
}
