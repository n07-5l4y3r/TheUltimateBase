#pragma once
#include <inc.hpp>

union RGBA {
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE a;
	} sRGBA;
	DWORD dwRGBA;
};

namespace render_utils
{
	struct s_font
	{
	public:
		s_font();
		s_font* init(std::string f, float s, IDWriteTextFormat* t)
		{
			fsize = s; rname = f; tformat = t;
			std::stringstream x; x << std::fixed << std::setprecision(1) << s;
			iname = f.append(x.str());
			return this;
		}
		IDWriteTextFormat* tformat = nullptr;
		std::string iname = "";
		std::string rname = "";
		float fsize       = 0.f;
	};
	class c_render
	{
	private:
		ID2D1Factory	 * d2d_factory = nullptr; 
		ID2D1RenderTarget* d2d_render_target = nullptr;
		IDWriteFactory	 * dwrite_factory = nullptr;
		std::unordered_map<std::string, s_font        *> fonts   = std::unordered_map<std::string, s_font*>();
		std::unordered_map<DWORD, ID2D1SolidColorBrush*> brushes = std::unordered_map<DWORD, ID2D1SolidColorBrush*>();
		float w = 1920; float h = 1080;
	public:
		bool					was_setup = false;
		bool					setup(ID2D1Factory* rinst, ID2D1RenderTarget* rtinst, IDWriteFactory* iwinst);
		bool					mfont(std::string font, float size);
		s_font*					gfont(std::string iname);
		bool					mbrush(RGBA clr);
		ID2D1SolidColorBrush*	gbrush(RGBA clr);
		ID2D1Factory*			gd2dfac();
		ID2D1RenderTarget*		gd2drfac();
		//draw fns
		void					text(std::string t, float x, float y, RGBA clr, std::string f);
		void					line(float x, float y, float x2, float y2, float w, RGBA clr);
		void					orect(float x, float y, float x2, float y2, float w, RGBA clr);
		void					frect(float x, float y, float x2, float y2, RGBA clr);
		void					orrect(float x, float y, float x2, float y2, float r, float w, RGBA clr);
		void					frrect(float x, float y, float x2, float y2, float r, RGBA clr);
	};	
	extern c_render* render;
}