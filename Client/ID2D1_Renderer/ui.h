#pragma once
#include <inc.hpp>
namespace ui
{
	struct s_menu_button
	{
		s_menu_button(std::string a1, int a2, uint64_t a3)
		{
			name = a1;
			i_page = a2;

			auto gb = [&](int add) -> const uint8_t
			{
				const auto ptr = (uint8_t*)a3 + add;
				const auto opcode = ptr;
				const auto opc = *opcode;
				return opc;
			};
			unsigned long long res = 0;
			auto first_bt = gb(0x0);
			if (first_bt == 0xe9)
			{
				auto last_read = 0;
				auto mempad = 0;
				std::stringstream adr_content;
				while (last_read != 0xe9)
				{
					mempad++;
					last_read = gb(mempad);

					if (last_read != 0xe9) adr_content << std::hex << last_read;
				}
				/*convert*/
				auto final_adr = (DWORD64)a3 + 0x5;

				std::stringstream reorg;
				reorg << adr_content.str().at(2);
				reorg << adr_content.str().at(3);
				reorg << adr_content.str().at(0);
				reorg << adr_content.str().at(1);

				auto relastoi = std::stoul(reorg.str(), nullptr, 16);
				auto funcadr = final_adr + relastoi + 0x20000;
				res = funcadr;
			}
			else res = (unsigned long long)a3;
			printf("resolved %08x -> %08x\n", a3, res);
			func = res;
		}
		std::string name; 
		int i_page;
		uint64_t func;
	};
	enum e_menu_types
	{
		e_invalid = 0,
		e_button
	};
	struct s_menu_pos
	{
		float d_x; float d_y;//draw
		float m_x; float m_y;//cursor
		float p_x; float p_y;//pad
	};
	struct s_menu_var
	{
		s_menu_var(uint64_t a1, ui::e_menu_types a2)
		{
			container = a1;
			i_type = a2;
		}
		uint64_t     container;
		s_menu_pos   pos;
		e_menu_types i_type;
	};
	extern void button_test_func();
	extern void button_test_func2();
	class c_ui
	{
	private:
		std::deque<s_menu_var> vars;
		bool was_setup = false;
		bool setup();
		//
		bool  is_active = false; bool moving = false;
		float menu_x = 50.f; float menu_y = 250.f;
		float cursor_x = 0.f; float cursor_y = 0.f;
		float dpi_x = 0.f; float dpi_y = 0.f;
		//
		void gcursor();
		void input();
		bool isbound(float v, float m, float h);
		//
		void bg();
		void rvars();
		void render();
		//
		void move();
		//
		bool avar(std::string t, int p, e_menu_types mt, uint64_t v);
		bool svars();
		bool cvars();
	public:
		float gdpix(); float gdpiy();
		void work();
		void test_0_fn()
		{
			typedef void(__stdcall* a)();
			a b = (a)((ui::s_menu_button*)this->vars[0].container)->func;
			b();
		}
		void test_1_fn()
		{
			typedef void(__stdcall* a)();
			a b = (a)((ui::s_menu_button*)this->vars[1].container)->func;
			b();
		}
	};
	extern c_ui* ui;
}