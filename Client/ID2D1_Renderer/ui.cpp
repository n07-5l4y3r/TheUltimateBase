#include <inc.hpp>

bool ui::c_ui::setup()
{
	//
	this->was_setup = true;
	this->gcursor();
	this->svars();
	return this->was_setup;
}
void ui::c_ui::gcursor()
{
	auto p = POINT();
	if (!GetCursorPos(&p)) return;
	this->cursor_x = (float)p.x; this->cursor_y = (float)p.y;
	render_utils::render->gd2drfac()->GetDpi(&this->dpi_x, &this->dpi_y);
	this->dpi_x /= 96.f; this->dpi_y /= 96.f;
}
void ui::c_ui::input()
{
	//input mgr core
	if (input::input->is_pressed(VK_INSERT) && input::input->is_pressed(VK_SHIFT)) this->is_active = !this->is_active;
	if (input::input->is_pressed(VK_NUMPAD0)) this->test_0_fn();
	if (input::input->is_pressed(VK_NUMPAD1)) this->test_1_fn();
}
bool ui::c_ui::isbound(float v, float m, float h)
{
	return !(v < m) && !(h < v);
}
void ui::c_ui::bg()
{
	render_utils::render->text("menu is active", 0.6f, 15.f, RGBA({ 0,255,0,255 }), "Consolas0.8");
	//
	render_utils::render->frect(this->menu_x / this->dpi_x, this->menu_y / this->dpi_y, 18.f, 18.f, RGBA({ 147,112,219,255 }));
	//
	render_utils::render->frect(this->cursor_x / this->dpi_x, this->cursor_y / this->dpi_y, 0.4f, 0.4f, RGBA({ 255,255,255,255 }));
}
void ui::c_ui::rvars()
/*
renders given objects registered and precalculated for the current menu layout and position
*/
{
	for (auto&& b : this->vars)
	{
		switch (b.i_type)
		{
		case ui::e_menu_types::e_invalid: break;
		case ui::e_menu_types::e_button:
		{
			auto tt = (ui::s_menu_button*)(b.container);
			//TODO: align width from all objects on page to max text size+pad to make all objects the same general size
			render_utils::render->orect(b.pos.d_x, b.pos.d_y, b.pos.p_x, b.pos.p_y, 0.1f, RGBA({ 155,255,0,255 }));
			render_utils::render->text(tt->name, b.pos.d_x, b.pos.d_y, RGBA({ 0,255,0,255 }), "Consolas0.6");
			break;
		}
		default: break;
		}
	}
}
void ui::c_ui::render()
{
	this->bg();
	this->rvars();
}
void ui::c_ui::move()
{
	//
	auto x1 = this->menu_x / this->dpi_x;
	auto y1 = (this->menu_y) / this->dpi_y; auto y2 = (this->menu_x / 2) / this->dpi_y;
	if (this->isbound(this->cursor_x / this->dpi_x, x1, x1 + y2) && this->isbound(this->cursor_y / this->dpi_y, y1, y1 + y2))
		render_utils::render->orect(x1, y1,
									x1, y2, 0.1f, RGBA({ 0,255,0,255 }));
	else
		render_utils::render->orect(x1, y1,
			x1, y2, 0.1f, RGBA({ 255,0,0,255 }));
}
bool ui::c_ui::avar(std::string t, int p, e_menu_types mt, uint64_t v)
/*
supports for data types (int,float...) and function passthrough
*/
{
	switch (mt)
	{
	case e_menu_types::e_invalid:
		return false;
		break;
	case e_menu_types::e_button:/*to trigger a function given*/
	{
		auto tvar = new ui::s_menu_button(t, p, v);
		this->vars.emplace_back((uint64_t)tvar, mt);
		printf("added %s -> %i -> e_button\n", t.c_str(), p);
		break;
	}
	default: return false;
	}
	return true;
}
bool ui::c_ui::svars()
{
	if (!this->avar("test-01", 0, e_menu_types::e_button, (uint64_t)button_test_func)) return false;
	if (!this->avar("test-02", 0, e_menu_types::e_button, (uint64_t)button_test_func2)) return false;
	if (!this->avar("test-03", 0, e_menu_types::e_button, (uint64_t)button_test_func)) return false;
	if (!this->avar("test-04", 0, e_menu_types::e_button, (uint64_t)button_test_func2)) return false;
	if (!this->avar("test-05", 0, e_menu_types::e_button, (uint64_t)button_test_func)) return false;
	if (!this->avar("test-06", 0, e_menu_types::e_button, (uint64_t)button_test_func2)) return false;
	if (!this->avar("test-07", 0, e_menu_types::e_button, (uint64_t)button_test_func)) return false;
	this->cvars();
	return true;
}
bool ui::c_ui::cvars()
/*
calculates relative positions for each variable added prior to fit in the menu layout&bounds
x l/r -
y u/d |
*/
{
	auto x_pad = 50; auto y_pad = 20;
	auto x_continuous = 5; auto y_continuous = 10; auto cur_count = 0; auto lines_filled = 1;//1=0(1*50=50, 50 default pad)
	for (auto&& v : this->vars)
	{
		if (cur_count == 5) 
		{ 
			cur_count = 0; 
			y_continuous = 10;
			lines_filled++;
			x_continuous += x_pad + 15; 
		}
		y_continuous += y_pad + 10;
		v.pos = ui::s_menu_pos();
		v.pos.d_x = (x_continuous + this->menu_x) / this->dpi_x;
		v.pos.d_y = (y_continuous + this->menu_y) / this->dpi_y;
		v.pos.p_x = x_pad / this->dpi_x; v.pos.p_y = y_pad / this->dpi_y;
		/*
			auto x1 = this->menu_x / this->dpi_x;
			auto y1 = (this->menu_y) / this->dpi_y; auto y2 = (this->menu_x / 2) / this->dpi_y;

			if (this->isbound(this->cursor_x / this->dpi_x, x1, x1 + y2) 
				this->isbound(this->cursor_y / this->dpi_y, y1, y1 + y2))

		*/
		cur_count++;
	}
	return true;
}
float ui::c_ui::gdpix()
{
	return this->dpi_x;
}
float ui::c_ui::gdpiy()
{
	return this->dpi_y;
}
void ui::c_ui::work()
{
	if (!this->was_setup) if (!this->setup()) return;
	//
	this->gcursor();
	this->input();
	//
	if (!this->is_active) return;
	this->render();
	//
	this->move();
}
ui::c_ui* ui::ui;

void ui::button_test_func()
{
	printf("trigger button_test_func\n");
}
void ui::button_test_func2()
{
	printf("trigger button_test_func2\n");
}