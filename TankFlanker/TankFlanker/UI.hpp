#pragma once

//���T�C�Y
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)

class UI : Mainclass {
private:
	//pic
	GraphHandle UI_VIVE;
	GraphHandle UI_safty;
	GraphHandle UI_select;
	GraphHandle UI_trigger;
	GraphHandle UI_mag_fall;
	GraphHandle UI_mag_set;
	GraphHandle UI_get;
	//font
	FontHandle font36;
	FontHandle font24;
	FontHandle font18;
	FontHandle font12;
	//sound
	SoundHandle decision;
	SoundHandle cancel;
	SoundHandle cursor;
public:
	UI() {
		SetUseASyncLoadFlag(TRUE);

		SetTransColor(0, 255, 0);
		UI_VIVE = GraphHandle::Load("data/UI/pic.bmp");
		UI_safty = GraphHandle::Load("data/UI/safty.bmp");
		UI_select = GraphHandle::Load("data/UI/select.bmp");
		UI_trigger = GraphHandle::Load("data/UI/trigger.bmp");
		UI_mag_fall = GraphHandle::Load("data/UI/mag_fall.bmp");
		UI_mag_set = GraphHandle::Load("data/UI/mag_set.bmp");
		UI_get = GraphHandle::Load("data/UI/get.bmp");
		SetTransColor(0, 0, 0);

		font36 = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
		font24 = FontHandle::Create(y_r(24), DX_FONTTYPE_EDGE);
		font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(y_r(12), DX_FONTTYPE_EDGE);

		decision = SoundHandle::Load("data/audio/shot_2.wav");//
		cancel = SoundHandle::Load("data/audio/cancel.wav");
		cursor = SoundHandle::Load("data/audio/cursor.wav");

		SetUseASyncLoadFlag(FALSE);
	}
	~UI() {
	}
	void load_window(const char* mes) {
		SetUseASyncLoadFlag(FALSE);
		float bar = 0.f;
		auto all = GetASyncLoadNum();
		while (ProcessMessage() == 0) {
			int t_disp_x = 1920;
			int t_disp_y = 1080;
			GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			int tmp = all - GetASyncLoadNum();

			GraphHandle::SetDraw_Screen((int32_t)DX_SCREEN_BACK);
			{
				font18.DrawStringFormat(0, t_disp_y - y_r(70), GetColor(0, 255, 0), " loading... : %04d/%04d  ", tmp, all);
				font18.DrawStringFormat_RIGHT(t_disp_x, t_disp_y - y_r(70), GetColor(0, 255, 0), "%s �ǂݍ��ݒ� ", mes);
				DrawBox(0, t_disp_y - y_r(50), int(float(t_disp_x) * bar / float(all)), t_disp_y - y_r(40), GetColor(0, 255, 0), TRUE);
			}
			ScreenFlip();

			if (int(bar * 100) >= (all * 100-1)) {
				break;
			}
			easing_set(&bar, float(tmp), 0.95f);
		}
		GraphHandle::SetDraw_Screen((int32_t)DX_SCREEN_BACK);
		ScreenFlip();
	}
	void set_draw(Mainclass::Chara& chara, bool use_vr = true) {
		int t_disp_x = 1920;
		int t_disp_y = 1080;
		GetScreenState(&t_disp_x, &t_disp_y, nullptr);

		//FontHandle* font_big = &font24;
		FontHandle* font = (use_vr) ? &font12 : &font18;
		const int fonthight = (use_vr) ? y_r(12) : y_r(18);
		int xs = 0, ys = 0, xp = 0, yp = 0;
		int xp2 = 0, yp2 = 0, xs2 = 0, ys2 = 0;
		{
			//�x��
			{
				if (use_vr) {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 + t_disp_y / 8;
				}
				else {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 + t_disp_y / 8;
				}
				if ((GetNowHiPerformanceCount() / 100000) % 4 <= 2) {
					//��x��
					if (chara.ammo_cnt == 0) {
						font->DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0)); yp += fonthight;
					}
				}
			}
			//�Z���N�^�[
			{
				if (use_vr) {
					xp = t_disp_x / 2 - t_disp_y / 10;
					yp = t_disp_y / 2 + t_disp_y / 8;
				}
				else {
					xp = t_disp_x / 2 - t_disp_y / 10;
					yp = t_disp_y / 2 + t_disp_y / 8;
				}
				if (chara.gun_ptr->select.size() >= 1) {
					switch (chara.gun_ptr->select[std::clamp<size_t>(chara.gun_stat[chara.gun_ptr->id].select, 0, chara.gun_ptr->select.size() - 1)]) {
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
			//�A�C�e�����E��
			{
				if (use_vr) {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 + t_disp_y / 12;
				}
				else {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 + t_disp_y / 12;
				}
				if (chara.canget_magitem) {
					if (use_vr) {
						font->DrawString_MID(xp, yp, chara.canget_mag + "���E�� : �Z", GetColor(255, 128, 0));
					}
					else {
						font->DrawString_MID(xp, yp, chara.canget_mag + "���E�� : F", GetColor(255, 128, 0));
					}
				}
			}
			//�e��
			{
				if (use_vr) {
					xs = x_r(200);
					xp = t_disp_x / 2 - x_r(20) - xs;
					ys = y_r(12);
					yp = t_disp_y / 2 + t_disp_y / 6 + y_r(20);
				}
				else {
					xs = x_r(200);
					xp = x_r(20);
					ys = y_r(18);
					yp = t_disp_y - y_r(50);
				}
				//�e
				{
					int x, y;
					GetGraphSize(chara.gun_ptr->mod.UIScreen.get(), &x, &y);
					if (use_vr) {
						xs2 = y_r(120);
						ys2 = xs2 * y / x;
						xp2 = xp;
						yp2 = yp - ys2;
					}
					else {
						xs2 = y_r(180);
						ys2 = xs2 * y / x;
						xp2 = xp;
						yp2 = yp - ys2;
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
					chara.gun_ptr->mod.UIScreen.DrawExtendGraph(xp2, yp2, xp2 + xs2, yp2 + ys2, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				//�e
				{
					font->DrawString(xp, yp, chara.gun_ptr->mod.name, GetColor(255, 255, 255));
					font->DrawStringFormat_RIGHT(xp + xs, yp + ys + y_r(2), GetColor(255, 255, 255), "%04d / %04d", chara.ammo_cnt, chara.gun_stat[chara.gun_ptr->id].in - chara.ammo_cnt);
				}
			}
			//�}�K�W���֘A(��)
			{
				if (use_vr) {
					xp = t_disp_x / 2 - t_disp_y / 12;
					yp = t_disp_y / 2 - t_disp_y / 12;
				}
				else {
					xp = 300;
					yp = 500;
				}
				size_t pp = 0;
				for (auto& a : chara.gun_stat[chara.gun_ptr->id].mag_in) {
					pp += a;
					font24.DrawStringFormat(xp, yp, GetColor(255, 0, 0), "%d/%d total=%d", a, chara.gun_ptr->magazine->cap, pp); yp += fonthight;
				}
			}
			//�I���
		}
	}
	void item_draw(std::vector<Items>&item_data, const VECTOR_ref& pos) {
		SetCameraNearFar(0.01f, 10.f);
		for (auto& g : item_data) {
			if (g.ptr != nullptr && g.cate == 0) {
				VECTOR_ref p = ConvWorldPosToScreenPos(g.pos.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					float r_ = std::max((g.pos - pos).size(), 1.f);
					if (r_ <= 10.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / r_)), 0, 255));
						DrawCircle(int(p.x()), int(p.y()), y_r(36), GetColor(255, 0, 0), FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24), GetColor(255, 0, 0));
						font24.DrawString(int(p.x()) + y_r(36), int(p.y()) + y_r(36), g.ptr->mod.name, GetColor(255, 0, 0));
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
			else if (g.ptr != nullptr && g.cate == 1) {
				VECTOR_ref p = ConvWorldPosToScreenPos(g.pos.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					float r_ = std::max((g.pos - pos).size(), 1.f);
					if (r_ <= 10.f && g.ptr->magazine->mod.name.find("none") == std::string::npos) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / r_)), 0, 255));
						DrawCircle(int(p.x()), int(p.y()), y_r(36), GetColor(255, 0, 0), FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24), GetColor(255, 0, 0));
						font24.DrawString(int(p.x()) + y_r(36), int(p.y()) + y_r(36), g.ptr->magazine->mod.name, GetColor(255, 0, 0));
						font24.DrawStringFormat(int(p.x()) + y_r(36), int(p.y()) + y_r(36) + y_r(18), GetColor(255, 0, 0), "%d/%d", g.cap, g.ptr->magazine->cap);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
		}
	}
};
