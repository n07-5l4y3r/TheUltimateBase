#include <inc.hpp>
render_utils::s_font::s_font()
{
}
bool render_utils::c_render::setup(ID2D1Factory* rinst, ID2D1RenderTarget* rtinst, IDWriteFactory* iwinst)
{
	this->d2d_factory = rinst; this->d2d_render_target = rtinst; this->dwrite_factory = iwinst;
	this->mfont("Consolas", 0.8f); this->mfont("Consolas", 0.4f); this->mfont("Consolas", 0.6f);
	this->mbrush(RGBA({ 0, 255, 0, 255 }));
	this->was_setup = true;
	return true;
}
bool render_utils::c_render::mfont(std::string font, float size)
{
	if (this->fonts.find(font) != this->fonts.end()) { printf("font %s %f already exists\n", font.c_str(), size); return false; }
	IDWriteTextFormat* f = nullptr;
	auto r = this->dwrite_factory->CreateTextFormat(std::wstring(font.begin(), font.end()).c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &f);
	if ((bool)(r != S_OK)) { printf("failed making font %s %f\n", font.c_str(), size); return false; }
	auto s_f = new render_utils::s_font(); s_f->init(font, size, f); this->fonts[s_f->iname] = s_f; 
	return true;
}
render_utils::s_font* render_utils::c_render::gfont(std::string iname)
{
	if (this->fonts.find(iname) != this->fonts.end()) return this->fonts[iname];
	return nullptr;
}
bool render_utils::c_render::mbrush(RGBA clr)
{
	if (this->brushes.find(clr.dwRGBA) != this->brushes.end()) { printf("brush already exists\n"); return false; }
	ID2D1SolidColorBrush* b = nullptr;
	auto r = this->d2d_render_target->CreateSolidColorBrush({ (FLOAT)clr.sRGBA.r / 255.f,(FLOAT)clr.sRGBA.g / 255.f,(FLOAT)clr.sRGBA.b / 255.f, (FLOAT)clr.sRGBA.a / 255.f }, &b);
	if (r != S_OK) { printf("failed making brush\n"); return false; }
	this->brushes[clr.dwRGBA] = b;
	return true;
}
ID2D1SolidColorBrush* render_utils::c_render::gbrush(RGBA clr)
{
	if (this->brushes.find(clr.dwRGBA) != this->brushes.end()) return this->brushes[clr.dwRGBA];
	if (this->mbrush(clr)) return this->brushes[clr.dwRGBA];
	return nullptr;
}
ID2D1Factory* render_utils::c_render::gd2dfac()
{
	return this->d2d_factory;
}
ID2D1RenderTarget* render_utils::c_render::gd2drfac()
{
	return this->d2d_render_target;
}
void render_utils::c_render::text(std::string t, float x, float y, RGBA clr, std::string f)
{
	auto fnt = this->gfont(f); if (fnt == nullptr) return;
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto rec = D2D1::RectF(x, y, this->w, this->h);//TODO:	make global w/h<<<
	this->d2d_render_target->DrawTextW(std::wstring(t.begin(), t.end()).c_str(), (int)t.size(), fnt->tformat, rec, brs);
}
void render_utils::c_render::line(float x, float y, float x2, float y2, float w, RGBA clr)
{
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto r1 = D2D_POINT_2F(); r1.x = x; r1.y = y;
	auto r2 = D2D_POINT_2F(); r2.x = x2; r2.y = y2;
	this->d2d_render_target->DrawLine(r1, r2, brs, w);
}
void render_utils::c_render::orect(float x, float y, float x2, float y2, float w, RGBA clr)
{
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto r1 = D2D1::RectF(x, y, x2+x, y2+y);
	this->d2d_render_target->DrawRectangle(r1, brs, w);
}
void render_utils::c_render::frect(float x, float y, float x2, float y2, RGBA clr)
{
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto r1 = D2D1::RectF(x, y, x2+x, y2+y);
	this->d2d_render_target->FillRectangle(r1, brs);
}
void render_utils::c_render::orrect(float x, float y, float x2, float y2, float r, float w, RGBA clr)
{
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto r1 = D2D1_ROUNDED_RECT(); r1.radiusX = r; r1.radiusY = r;
	r1.rect = D2D1::RectF(x,y,x2+x,y2+y);
	this->d2d_render_target->DrawRoundedRectangle(r1, brs, w);
}
void render_utils::c_render::frrect(float x, float y, float x2, float y2, float r, RGBA clr)
{
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto r1 = D2D1_ROUNDED_RECT(); r1.radiusX = r; r1.radiusY = r;
	r1.rect = D2D1::RectF(x, y, x2+x, y2+y);
	this->d2d_render_target->FillRoundedRectangle(r1, brs);
}
render_utils::c_render* render_utils::render;
