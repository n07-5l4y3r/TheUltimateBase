#include <inc.hpp>
render_utils::s_font::s_font()
{
}
bool render_utils::c_render::setup(ID2D1Factory* rinst, ID2D1RenderTarget* rtinst, IDWriteFactory* iwinst)
{
	this->d2d_factory = rinst; this->d2d_render_target = rtinst; this->dwrite_factory = iwinst;
	this->mfont("Consolas", 0.8f);
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
	auto r = this->d2d_render_target->CreateSolidColorBrush({ (FLOAT)clr.sRGBA.r,(FLOAT)clr.sRGBA.g,(FLOAT)clr.sRGBA.b,1.f / 255.f * (FLOAT)clr.sRGBA.a }, &b);
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
void render_utils::c_render::text(std::string t, float x, float y, RGBA clr, std::string f)
{
	auto fnt = this->gfont(f); if (fnt == nullptr) return;
	auto brs = this->gbrush(clr); if (brs == nullptr) return;
	auto rec = D2D1::RectF(x, y, 1920, 1080);//TODO:	make global w/h<<<
	this->d2d_render_target->DrawTextW(std::wstring(t.begin(), t.end()).c_str(), (int)t.size(), fnt->tformat, rec, brs);
}

render_utils::c_render* render_utils::render;
