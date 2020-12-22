#pragma once

//���T�C�Y
#define x_r(p1 , p2x) (int(p1) * p2x / deskx)
#define y_r(p1 , p2y) (int(p1) * p2y / desky)

class UI : Mainclass {
private:
	//
	int out_disp_x = 1920;
	int out_disp_y = 1080;
	int disp_x = 1920;
	int disp_y = 1080;
	//
	float start_fl = 0.f;

	GraphHandle bufScreen;
	GraphHandle outScreen;

	GraphHandle UI_VIVE;
	GraphHandle UI_safty;
	GraphHandle UI_select;
	GraphHandle UI_trigger;
	GraphHandle UI_mag_fall;
	GraphHandle UI_mag_set;
	GraphHandle UI_get;
	float pt_pl = 0.f, pt_pr = 0.f;
	float pt_pl2 = 0.f, pt_pr2 = 0.f;
	float pt_pe = 0.f;
	float point_r = 0.f;
	float point_uf = 0.f;
	float point_df = 0.f;
	//
	float ready_f = 0.f;
	float ready_yp = 0.f;
	//
	int tgt_pic_sel = -1;
	float tgt_pic_on = 1.f;
	//
	float c_readytimer_old = 3.f;
	bool c_end_f = false;

	float time_tmp = 0.f;
	//�`���[�g���A��
	int display_c1 = 0;
public:
	//font
	FontHandle font36;
	FontHandle font24;
	FontHandle font18;
	FontHandle font12;
	FontHandle font18_o;
	FontHandle font12_o;
	//
	SoundHandle timer, decision, cancel, cursor, whistle, start, hit;
	//
	UI(const int& o_xd, const int& o_yd, const int& xd, const int& yd) {
		out_disp_x = o_xd;
		out_disp_y = o_yd;
		disp_x = xd;
		disp_y = yd;
		SetUseASyncLoadFlag(TRUE);

		font36 = FontHandle::Create(y_r(36, disp_y), DX_FONTTYPE_EDGE);
		font24 = FontHandle::Create(y_r(24, disp_y), DX_FONTTYPE_EDGE);
		font18 = FontHandle::Create(y_r(18, disp_y), DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(y_r(12, disp_y), DX_FONTTYPE_EDGE);
		font18_o = FontHandle::Create(y_r(18, out_disp_y), DX_FONTTYPE_EDGE);
		font12_o = FontHandle::Create(y_r(12, out_disp_y), DX_FONTTYPE_EDGE);

		bufScreen = GraphHandle::Make(disp_x, disp_y, true);
		outScreen = GraphHandle::Make(disp_x, disp_y, true);
		SetTransColor(0, 255, 0);
		UI_VIVE = GraphHandle::Load("data/UI/pic.bmp");
		UI_safty = GraphHandle::Load("data/UI/safty.bmp");
		UI_select = GraphHandle::Load("data/UI/select.bmp");
		UI_trigger = GraphHandle::Load("data/UI/trigger.bmp");
		UI_mag_fall = GraphHandle::Load("data/UI/mag_fall.bmp");
		UI_mag_set = GraphHandle::Load("data/UI/mag_set.bmp");
		UI_get = GraphHandle::Load("data/UI/get.bmp");
		SetTransColor(0, 0, 0);

		timer = SoundHandle::Load("data/audio/timer.wav");
		decision = SoundHandle::Load("data/audio/shot_2.wav");//
		cancel = SoundHandle::Load("data/audio/cancel.wav");
		cursor = SoundHandle::Load("data/audio/cursor.wav");
		start = SoundHandle::Load("data/audio/start.wav");
		whistle = SoundHandle::Load("data/audio/whistle.wav");
		hit = SoundHandle::Load("data/audio/hit.wav");

		SetUseASyncLoadFlag(FALSE);
	}
	~UI() {
	}
	void load_window(const char* mes) {
		SetUseASyncLoadFlag(FALSE);
		float bar = 0.f, cnt = 0.f;
		auto all = GetASyncLoadNum();
		while (ProcessMessage() == 0) {
			const auto fps = GetFPS();
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();
			{
				font18_o.DrawStringFormat(0, out_disp_y - y_r(70, out_disp_y), GetColor(0, 255, 0), " loading... : %04d/%04d  ", all - GetASyncLoadNum(), all);
				font12_o.DrawStringFormat_RIGHT(out_disp_x, out_disp_y - y_r(70, out_disp_y), GetColor(0, 255, 0), "%s �ǂݍ��ݒ� ", mes);
				DrawBox(0, out_disp_y - y_r(50, out_disp_y), int(float(out_disp_x) * bar / float(all)), out_disp_y - y_r(40, out_disp_y), GetColor(0, 255, 0), TRUE);
				easing_set(&bar, float(all - GetASyncLoadNum()), 0.95f);
			}
			ScreenFlip();
			if (GetASyncLoadNum() == 0) {
				cnt += 1.f / GetFPS();
				if (cnt > 1 && bar > float(all - GetASyncLoadNum()) * 0.95f) {
					break;
				}
			}
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		ScreenFlip();
	}
	void set_draw_nomal(Mainclass::Chara& chara) {
		//
		const float fps = GetFPS();
		bufScreen.SetDraw_Screen();
		{
			//FontHandle* font_big = &font36;
			FontHandle* font = &font18;

			const int half = y_r(18, disp_y);

			int xs = 0, ys = 0, xp = 0, yp = 0;
			//���[�h���̑�
			{
				xp = disp_x / 2;
				yp = disp_y / 2 + disp_y / 8;
				//��
				if (time_tmp >= 0.25f) {
					if (chara.ammo_cnt == 0) {
						font->DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0)); yp += half;
					}
				}
				time_tmp += 1.f / fps;
				if (time_tmp >= 0.5f) {
					time_tmp = 0.f;
				}
			}
			//�Z���N�^�[
			{
					xp = disp_x / 2 - disp_y / 10;
					yp = disp_y / 2 + disp_y / 8;
				if (chara.ptr_now->select.size()>=1) {
					switch (chara.ptr_now->select[std::clamp<size_t>(chara.gun_stat[chara.gun_ptr->id].select, 0, chara.ptr_now->select.size() - 1)]) {
					case 1:
						font->DrawString_MID(xp, yp, "SEMI AUTO", GetColor(0, 255, 0));
						break;
					case 2:
						font->DrawString_MID(xp, yp, "FULL AUTO", GetColor(0, 255, 0));
						break;
					case 3:
						font->DrawString_MID(xp, yp, "3B", GetColor(0, 255, 0));
						break;
					case 4:
						font->DrawString_MID(xp, yp, "2B", GetColor(0, 255, 0));
						break;
					}
				}
			}
			//�A�C�e���E��
			{
				xp = disp_x / 2;
				yp = disp_y / 2 + disp_y / 12;
				if (chara.canget_magitem) {
					font->DrawString_MID(xp, yp, chara.canget_mag + "���E�� : F", GetColor(255, 128, 0));
				}
			}
			//�e��
			{
				int xp2;
				int yp2;
				int xs2;
				int ys2;
				{
					xs = x_r(200, disp_y);
					xp = x_r(20, disp_y);
					ys = y_r(18, disp_y);
					yp = disp_y - y_r(50, disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
				{
					int x, y;
					GetGraphSize(chara.ptr_now->mod.UIScreen.get(), &x, &y);
					{
						xs2 = y_r(180, disp_y);
						ys2 = xs2 * y / x;
						xp2 = xp;
						yp2 = yp - ys2;
					}
					chara.ptr_now->mod.UIScreen.DrawExtendGraph(xp2, yp2, xp2 + xs2, yp2+ys2, true);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				{
					font->DrawString(xp, yp, chara.ptr_now->name, GetColor(255, 255, 255));
					font->DrawStringFormat_RIGHT(xp + xs, yp + ys + y_r(2, disp_y), GetColor(255, 255, 255), "%04d / %04d", chara.ammo_cnt, chara.gun_stat[chara.ptr_now->id].in - chara.ammo_cnt);
				}
			}
			//�}�K�W���֘A(��)
			{
				xp = 300;
				yp = 500;
				size_t pp = 0;
				for (auto& a : chara.gun_stat[chara.ptr_now->id].mag_in) {
					pp += a;
					font24.DrawStringFormat(xp, yp, GetColor(255, 0, 0), "%d/%d total:%d",
						a,
						chara.ptr_now->ammo_max,
						pp
					);
					yp += half;
				}
			}
		}
	}
	void set_draw_vr(Mainclass::Chara& chara) {
		//
		const float fps = GetFPS();
		bufScreen.SetDraw_Screen();
		{
			//FontHandle* font_big = &font24;
			FontHandle* font = &font12;
			const int half = y_r(12, disp_y);

			int xs = 0, ys = 0, xp = 0, yp = 0;
			//���[�h���̑�
			{
				xp = disp_x / 2;
				yp = disp_y / 2 + disp_y / 8;
				//��x��
				{
					if (time_tmp >= 0.25f) {
						if (chara.ammo_cnt == 0) {
							font->DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0)); yp += half;
						}
					}
					time_tmp += 1.f / fps;
					if (time_tmp >= 0.5f) {
						time_tmp = 0.f;
					}
				}
			}
			//�Z���N�^�[
			{
				xp = disp_x / 2 - disp_y / 10;
				yp = disp_y / 2 + disp_y / 8;
				if (chara.ptr_now->select.size() >= 1) {
					switch (chara.ptr_now->select[std::clamp<size_t>(chara.gun_stat[chara.gun_ptr->id].select, 0, chara.ptr_now->select.size() - 1)]) {
					case 1:
						font->DrawString_MID(xp, yp, "SEMI AUTO", GetColor(0, 255, 0));
						break;
					case 2:
						font->DrawString_MID(xp, yp, "FULL AUTO", GetColor(0, 255, 0));
						break;
					case 3:
						font->DrawString_MID(xp, yp, "3B", GetColor(0, 255, 0));
						break;
					case 4:
						font->DrawString_MID(xp, yp, "2B", GetColor(0, 255, 0));
						break;
					}
				}
			}
			//�A�C�e���E��
			{
				xp = disp_x / 2;
				yp = disp_y / 2 + disp_y / 12;
				if (chara.canget_magitem) {
					font->DrawString_MID(xp, yp, chara.canget_mag + "���E�� : �Z", GetColor(255, 128, 0));
				}
			}
			//�e��
			{
				int xp2;
				int yp2;
				int xs2;
				int ys2;
				{
					xs = x_r(200, disp_y);
					xp = disp_x / 2 - x_r(20, disp_y) - xs;
					ys = y_r(12, disp_y);
					yp = disp_y / 2 + disp_y / 6 + y_r(20, disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
				{
					int x, y;
					GetGraphSize(chara.ptr_now->mod.UIScreen.get(), &x, &y);
					xs2 = y_r(120, disp_y);
					ys2 = xs2 * y / x;
					xp2 = xp;
					yp2 = yp - ys2;
					chara.ptr_now->mod.UIScreen.DrawExtendGraph(xp2, yp2, xp2 + xs2, yp2 + ys2, true);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				{
					font->DrawString(xp, yp, chara.ptr_now->name, GetColor(255, 255, 255));
					font->DrawStringFormat_RIGHT(xp + xs, yp + ys + y_r(2, disp_y), GetColor(255, 255, 255), "%04d / %04d", chara.ammo_cnt, chara.gun_stat[chara.ptr_now->id].in - chara.ammo_cnt);
				}
			}
			//VR�p�̃`���[�g���A��
			/*
			{
				//�E��
				{
					xs = 500 / 4;
					ys = 408 / 4;
					xp = disp_x / 2 + disp_y / 6 - xs;
					yp = disp_y / 2 + disp_y / 6 - ys;
					if (!chara.reloadf && chara.ammo_cnt == 0) {
						pt_pr = 2.f;
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pr), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pr >= 0.f) {
						if (!chara.reloadf && chara.ammo_cnt == 0) {
							UI_mag_fall.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						}
						pt_pr -= 1.f / fps;
					}
				}
				{
					xs = 500 / 4;
					ys = 408 / 4;
					xp = disp_x / 2 + disp_y / 6 - xs;
					yp = disp_y / 2 - ys / 2;
					if (display_c1 == 0 && !chara.gunf) {
						pt_pr2 = 1.f;
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pr2), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					DrawBox(xp, yp, xp + xs, yp + ys, GetColor(255, 0, 0), FALSE);
					font->DrawString(xp, yp + ys, "Tutorial", GetColor(255, 0, 0));
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pr2 >= 0.f) {
						if (display_c1 == 0 && !chara.gunf) {
							UI_trigger.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
							font->DrawString(xp, yp, "FIRE", GetColor(2555, 128, 0));
						}
						pt_pr2 -= 1.f / fps;
					}
					if (display_c1 == 0 && chara.gunf) {
						display_c1 = 1;
					}
				}
				//����
				{
					xs = 500 / 4;
					ys = 408 / 4;
					xp = disp_x / 2 - disp_y / 6;
					yp = disp_y / 2 + disp_y / 6 - ys;
					if (chara.reloadf && !chara.down_mag) {
						pt_pl = 2.f;
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pl), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pl >= 0.f) {
						if (chara.reloadf && !chara.down_mag) {
							UI_mag_set.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
							font->DrawString(xp, yp, "GET MAG", GetColor(2555, 128, 0));
						}
						pt_pl -= 1.f / fps;
					}
				}
				{
					xs = 500 / 4;
					ys = 408 / 4;
					xp = disp_x / 2 - disp_y / 6;
					yp = disp_y / 2 - ys / 2;
					if (display_c1 <= 0) {
						pt_pl2 = 1.f;
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pl2), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					DrawBox(xp, yp, xp + xs, yp + ys, GetColor(255, 0, 0), FALSE);
					font->DrawString(xp, yp + ys, "Tutorial", GetColor(255, 0, 0));
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pl2 >= 0.f) {
						pt_pl2 -= 1.f / fps;
					}
				}
			}
			*/
			//�}�K�W���֘A(��)
		}
	}
	void draw(void) {
		bufScreen.DrawExtendGraph(0, 0, disp_x, disp_y, true);
	}
	template <class T>
	void item_draw(std::vector<T> &item_data, const VECTOR_ref& pos) {
		for (auto& g : item_data) {
			if (g.ptr != nullptr && g.cate == 0) {
				VECTOR_ref p = ConvWorldPosToScreenPos(g.pos.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					float r_ = std::max((g.pos - pos).size(), 1.f);
					if (r_ <= 10.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / r_)), 0, 255));
						DrawCircle(int(p.x()), int(p.y()), y_r(36, disp_y), GetColor(255, 0, 0), FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24, disp_y), GetColor(255, 0, 0));
						font24.DrawString(int(p.x()) + y_r(36, disp_y), int(p.y()) + y_r(36, disp_y), g.ptr->name, GetColor(255, 0, 0));
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
			else if (g.ptr != nullptr && g.cate == 1) {
				VECTOR_ref p = ConvWorldPosToScreenPos(g.pos.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					float r_ = std::max((g.pos - pos).size(), 1.f);
					if (r_ <= 10.f && g.ptr->mag.name.find("none") == std::string::npos) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / r_)), 0, 255));
						DrawCircle(int(p.x()), int(p.y()), y_r(36, disp_y), GetColor(255, 0, 0), FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24, disp_y), GetColor(255, 0, 0));
						font24.DrawString(int(p.x()) + y_r(36, disp_y), int(p.y()) + y_r(36, disp_y), g.ptr->mag.name, GetColor(255, 0, 0));
						font24.DrawStringFormat(int(p.x()) + y_r(36, disp_y), int(p.y()) + y_r(36, disp_y) + y_r(18, disp_y), GetColor(255, 0, 0), "%d/%d", g.cap, g.ptr->ammo_max);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
		}
	}
};
