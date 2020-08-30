#include <inc.hpp>
bool input::c_input::is_pressed(int vk)
{
	return (GetAsyncKeyState(vk) & 1);
}
input::c_input* input::input;