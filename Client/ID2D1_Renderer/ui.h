#pragma once
#include <inc.hpp>
namespace ui
{
	class c_ui
	{
	private:
		bool was_setup = false;
		bool setup();
		//
		bool  is_active = false;
		float menu_x = 5.f; float menu_y = 5.f;
		float cursor_x = 0.f; float cursor_y = 0.f;
		float dpi_x = 0.f; float dpi_y = 0.f;
		//
		void gcursor();
		void input();
		//
		void bg();
		void render();
	public:
		float gdpix(); float gdpiy();
		void work();
	};
	extern c_ui* ui;
}