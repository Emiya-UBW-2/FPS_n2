#pragma once

//リサイズ
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

	unsigned int red;
	unsigned int green;
	unsigned int write;
	unsigned int yellow;
	unsigned int gray_1;
	unsigned int gray_2;
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

		red = GetColor(255, 0, 0);
		green = GetColor(0, 255, 0);
		write = GetColor(255, 255, 255);
		yellow = GetColor(255, 255, 0);
		gray_1 = GetColor(64, 64, 64);
		gray_2 = GetColor(128, 128, 128);
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
				font18.DrawStringFormat(0, t_disp_y - y_r(70), green, " loading... : %04d/%04d  ", tmp, all);
				font18.DrawStringFormat_RIGHT(t_disp_x, t_disp_y - y_r(70), green, "%s 読み込み中 ", mes);
				DrawBox(0, t_disp_y - y_r(50), int(float(t_disp_x) * bar / float(all)), t_disp_y - y_r(40), green, TRUE);
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

	void draw_HP(int xpos, int ypos, int xsize, int ysize, int now, int will, int max) {
		auto size = y_r(2);
		DrawBox(
			xpos - xsize / 2,
			ypos,
			xpos + xsize / 2,
			ypos + ysize,
			gray_1, TRUE);
		DrawBox(
			xpos - xsize / 2,
			ypos,
			xpos + xsize / 2,
			ypos + ysize,
			gray_2, FALSE);
		DrawBox(
			xpos - xsize / 2 + size,
			ypos + size,
			xpos - xsize / 2 + size + (xsize - size * 2)*now / max,
			ypos + ysize - size,
			green, TRUE);
		DrawBox(
			xpos - xsize / 2 + size + (xsize - size * 2)*now / max,
			ypos + size,
			xpos - xsize / 2 + size + (xsize - size * 2)*will / max,
			ypos + ysize - size,
			yellow, TRUE);

		font24.DrawStringFormat_MID(xpos + size, ypos + size, write, "%d/%d", now, max);
	}

	void set_draw(Mainclass::Chara& chara, bool use_vr = true) {
		int t_disp_x = deskx;
		int t_disp_y = desky;
		if (use_vr) {
			GetScreenState(&t_disp_x, &t_disp_y, nullptr);
		}

		FontHandle* font_big = (use_vr) ? &font36 : &font24;
		FontHandle* font = (use_vr) ? &font24 : &font18;
		const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
		const int fonthight = (use_vr) ? y_r(24) : y_r(18);
		int xs = 0, ys = 0, xp = 0, yp = 0;
		int xp2 = 0, yp2 = 0, xs2 = 0, ys2 = 0;
		{
			//警告
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
					//空警告
					if (chara.gun_stat[chara.gun_ptr->id].ammo_cnt == 0) {
						font->DrawString_MID(xp, yp, "EMPTY", red); yp += fonthight;
					}
				}
			}
			//セレクター
			{
				if (use_vr) {
					xp = t_disp_x / 2 - t_disp_y / 10;
					yp = t_disp_y / 2 + t_disp_y / 8;
				}
				else {
					xp = t_disp_x / 2 + t_disp_y / 4;
					yp = t_disp_y / 2 + t_disp_y / 6;
				}
				if (chara.gun_ptr->select.size() >= 1) {
					switch (chara.gun_ptr->select[std::clamp<size_t>(chara.gun_stat[chara.gun_ptr->id].select, 0, chara.gun_ptr->select.size() - 1)]) {
					case 1:
						font_big->DrawString_MID(xp, yp, "SEMI AUTO", green);
						break;
					case 2:
						font_big->DrawString_MID(xp, yp, "FULL AUTO", green);
						break;
					case 3:
						font_big->DrawString_MID(xp, yp, "3B", green);
						break;
					case 4:
						font_big->DrawString_MID(xp, yp, "2B", green);
						break;
					}
				}
			}
			//アイテムを拾う
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
					if (chara.getmag.second == 1) {
					}
					font->DrawString_MID(xp, yp, chara.canget_mag + "を拾う : " + (use_vr ? "〇" : "F"), yellow);
				}
			}
			//弾薬
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
				//銃
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
				//弾
				{
					font->DrawString(xp, yp, chara.gun_ptr->mod.name, write);
					font->DrawStringFormat_RIGHT(xp + xs, yp + ys + y_r(2), write, "%04d / %04d", chara.gun_stat[chara.gun_ptr->id].ammo_cnt, chara.gun_stat[chara.gun_ptr->id].ammo_total - chara.gun_stat[chara.gun_ptr->id].ammo_cnt);
				}
				//マガジン関連(仮)
				{
					int size = int(chara.gun_stat[chara.gun_ptr->id].mag_in.size());
					if (use_vr) {
						xp = t_disp_x / 2 - x_r(140) + font_bighight * size;
						yp = t_disp_y / 2 + t_disp_y / 6 + y_r(20) - font_bighight * size;
					}
					else {
						xp = x_r(220) + font_bighight * size;
						yp = t_disp_y - x_r(20) - font_bighight * size;
					}
					if (size > 0) {
						DrawBox(xp, yp, xp + font_big->GetDrawWidthFormat("%d/%d", chara.gun_stat[chara.gun_ptr->id].mag_in.front(), chara.gun_ptr->magazine->cap), yp + font_bighight + 1, yellow, FALSE);
					}
					for (auto& a : chara.gun_stat[chara.gun_ptr->id].mag_in) {
						font_big->DrawStringFormat(xp, yp, red, "%d/%d", a, chara.gun_ptr->magazine->cap);
						xp -= font_bighight;
						yp += font_bighight;
					}
				}
			}
			//キル
			if (chara.kill_f) {
				if (use_vr) {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 - t_disp_y / 6;
				}
				else {
					xp = t_disp_x / 2;
					yp = t_disp_y / 2 - t_disp_y / 6;
				}
				int per = std::clamp(int(255.f*((chara.kill_time * 7) / 7.f)) - 255 * 6, 0, 255);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				DrawBox(xp - int(pow(per, 2)) * t_disp_x / 2 / int(pow(255, 2)), yp, xp + int(pow(per, 2)) * t_disp_x / 2 / int(pow(255, 2)), yp + font_bighight + 2, write, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*((chara.kill_time * 2) / 7.f)), 0, 255));
				font_big->DrawStringFormat_MID(xp, yp, red, "プレイヤー%d をキルしました", chara.kill_id); yp += font_bighight * 2;	//キル

				if (chara.kill_streak > 0) {
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					DrawBox(xp - int(pow(per, 4)) * t_disp_x / 2 / int(pow(255, 4)), yp, xp + int(pow(per, 4)) * t_disp_x / 2 / int(pow(255, 4)), yp + font_bighight + 2, write, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*((chara.kill_time * 2) / 7.f)), 0, 255));
					font_big->DrawStringFormat_MID(xp, yp, red, "kill streak! x%d", chara.kill_streak); yp += font_bighight;			//キルストリーク
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//HP
			{
				if (use_vr) {
					xp = t_disp_x / 2 + t_disp_y / 12;
					yp = t_disp_y / 2 + t_disp_y / 6;
					xs = x_r(200);
					ys = y_r(18) + y_r(8);
				}
				else {
					xp = t_disp_x / 2;
					yp = t_disp_y - font_bighight * 2;
					xs = x_r(600);
					ys = y_r(18) + y_r(8);
				}
				draw_HP(xp, yp, xs, ys, chara.HP, int(chara.HP_r), chara.HP_full);
			}
			//スコアその他
			{
				if (use_vr) {
					xp = t_disp_x / 2 + t_disp_y / 6 + font_bighight;
					yp = t_disp_y / 2 - t_disp_y / 12 + font_bighight;
				}
				else {
					xp = t_disp_x - font_bighight;
					yp = t_disp_y / 2 - t_disp_y / 12 + font_bighight;
				}
				font_big->DrawStringFormat_RIGHT(xp, yp, red, "score      : %d", chara.score); yp += font_bighight;			//スコア
				font_big->DrawStringFormat_RIGHT(xp, yp, red, "kill       : %d", chara.kill_count); yp += font_bighight;			//キルデス
				font_big->DrawStringFormat_RIGHT(xp, yp, red, "death      : %d", chara.death_count); yp += font_bighight;			//キルデス
				font_big->DrawStringFormat_RIGHT(xp, yp, red, "kill/death : %3.1f", float(chara.kill_count) / float(std::max(chara.death_count, 1))); yp += font_bighight;			//キルデス
			}
			//終わり
		}
	}
	void item_draw(std::vector<Chara>&chara,std::vector<Items>&item_data, const VECTOR_ref& pos, bool use_vr = true) {
		int xs = 0, ys = 0, xp = 0, yp = 0;
		FontHandle* font_big = &font24;
		FontHandle* font = (use_vr) ? &font12 : &font18;
		const int fonthight = (use_vr) ? y_r(12) : y_r(18);

		SetCameraNearFar(0.01f, 10.f);
		for (auto& g : item_data) {
			if (g.ptr != nullptr && g.cate == 0) {
				VECTOR_ref p = ConvWorldPosToScreenPos(g.pos.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					float r_ = std::max((g.pos - pos).size(), 1.f);
					if (r_ <= 10.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / r_)), 0, 255));
						DrawCircle(int(p.x()), int(p.y()), y_r(36), red, FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24), red);
						font_big->DrawString(int(p.x()) + y_r(36), int(p.y()) + y_r(36), g.ptr->mod.name, red);
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
						DrawCircle(int(p.x()), int(p.y()), y_r(36), red, FALSE, 3);
						DrawCircle(int(p.x()), int(p.y()), y_r(24), red);
						font_big->DrawString(int(p.x()) + y_r(36), int(p.y()) + y_r(36), g.ptr->magazine->mod.name, red);
						font_big->DrawStringFormat(int(p.x()) + y_r(36), int(p.y()) + y_r(36) + y_r(18), red, "%d/%d", g.magazine.cap, g.ptr->magazine->cap);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
		}
		//HP
		{
			SetCameraNearFar(0.01f, 100.f);
			for (auto& c : chara) {
				if (abs(c.HP - int(c.HP_r)) >= 3) {
					VECTOR_ref p = ConvWorldPosToScreenPos((c.body.frame(c.frame_.head_f.first) + VGet(0, 0.3f, 0)).get());
					if (p.z() >= 0.f&&p.z() <= 1.f) {
						xp = int(p.x());
						yp = int(p.y());
						xs = x_r(200);
						ys = y_r(18) + y_r(8);

						draw_HP(xp, yp, xs, ys, c.HP, int(c.HP_r), c.HP_full);
					}
				}
			}
		}
		//
	}

};
