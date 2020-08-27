#pragma once
#include <inc.hpp>
namespace input
{
	class c_input
	{
	public:
		bool is_pressed(int vk);
	};
	extern c_input* input;
}