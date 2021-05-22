#pragma once
//
class UIclass : Mainclass {
public:
	//ロードするセーブデータを指定する
	class UI_LOAD {
	private:
		int t_disp_x = 1920;
		int t_disp_y = 1080;
		//font
		FontHandle font72;
		FontHandle font48;
		FontHandle font36;
		FontHandle font24;
		FontHandle font18;
		FontHandle font12;
		//
		int ypos = 0;
		float ypos_real = 0.f;
		void Draw_per_info(int xpos1, int ypos1, int xsize, int ysize, GUNPARTs* parts, bool use_vr = true) noexcept {
			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			//const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			const int fonthight = (use_vr) ? y_r(24) : y_r(18 + 2);

			int back_siz = y_r(3);
			int title_siz = y_r(6);
			int main_siz = y_r(10);
			if (ypos1 < 100) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(64 * (ypos1 - 50) / 50, 0, 64));
			}
			else if (t_disp_y - (ypos1 + ysize + 25) < 0) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(64 * ((t_disp_y - (ypos1 + ysize + 25)) + 100) / 50, 0, 64));
			}
			else {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
			}
			DrawBox(xpos1 + back_siz, ypos1 + back_siz, xpos1 + xsize - back_siz, ypos1 + ysize - back_siz, GetColor(0, 0, 0), TRUE);
			if (ypos1 < 100) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * (ypos1 - 50) / 50, 0, 255));
			}
			else if (t_disp_y - (ypos1 + ysize + 25) < 0) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * ((t_disp_y - (ypos1 + ysize + 25)) + 100) / 50, 0, 255));
			}
			else {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
			{
				{
					float siz = float(font_big->GetDrawWidth((parts != nullptr) ? parts->per.name : "NONE"));
					int base_siz = (xsize - title_siz * 2);
					if (siz < base_siz) {
						font_big->DrawString(xpos1 + title_siz, ypos1 + title_siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
					}
					else {
						font_big->DrawExtendString(xpos1 + title_siz, ypos1 + title_siz, base_siz / siz, base_siz / siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
					}
				}
				if (parts != nullptr) {
					std::wstring msg = StringToWString(parts->per.info);
					int i = 0, siz = (xsize - main_siz * 2) / fonthight - 1;
					while (true) {
						if ((title_siz * 2 + font_bighight + fonthight * (i + 1)) > ysize) {
							break;
						}
						font->DrawString(xpos1 + main_siz, ypos1 + title_siz * 2 + font_bighight + fonthight * i, WStringToString(msg.substr(0, siz)), GetColor(0, 255, 0));
						i++;
						if (msg.length() < siz) {
							break;
						}
						else {
							msg = msg.substr(siz);
						}
					}
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	public:
		UI_LOAD(void) noexcept {

			t_disp_x = deskx;
			t_disp_y = desky;

			SetUseASyncLoadFlag(TRUE);
			this->font72 = FontHandle::Create(y_r(72), DX_FONTTYPE_EDGE);
			this->font48 = FontHandle::Create(y_r(48), DX_FONTTYPE_EDGE);
			this->font36 = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
			this->font24 = FontHandle::Create(y_r(24), DX_FONTTYPE_EDGE);
			this->font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
			this->font12 = FontHandle::Create(y_r(12), DX_FONTTYPE_EDGE);
			SetUseASyncLoadFlag(FALSE);
		}
		~UI_LOAD(void) noexcept {
		}
		template<class Y, class D>
		void UI_Draw(std::unique_ptr<Y, D>& MAINLOOPscene, std::vector<save_c> save_parts, std::string& set_name, bool use_vr = true) noexcept {
			if (use_vr) {
				GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			}

			DrawBox(0, 0, t_disp_x, t_disp_y, GetColor(192, 192, 192), TRUE);

			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			//const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			const int fonthight = (use_vr) ? y_r(24) : y_r(18);
			int xs = 0, ys = 0, xp = 0, yp = 0;
			{
				int i = 0;
				int cnt = 0;
				xs = 800;
				ys = 175;
				ypos += GetMouseWheelRotVol() * 30;
				//*
				if (ypos >= 0) {
					ypos = 0;
				}
				//*/

				//*
				if (ypos <= (t_disp_y - (100 + (ys + 25) * int(save_parts.size())))) {
					ypos = t_disp_y - (100 + (ys + 25) * int(save_parts.size()));
				}
				//*/

				easing_set(&ypos_real, float(ypos), 0.8f);

				while (true) {
					GUNPARTs* temp_p = nullptr;
					std::string parts_type = "";

					auto&tmp_save = save_parts[cnt%save_parts.size()];
					cnt++;

					xp = t_disp_x - (xs + 100);
					yp = 100 + int(ypos_real) + (ys + 25) * i;
					i++;
					if (yp + ys < 0) {
						continue;
					}
					if (yp > t_disp_y) {
						break;
					}
					if (i > save_parts.size()) {
						break;
					}
					if (tmp_save.cang_ != SIZE_MAX) {
						auto vec_data = MAINLOOPscene->get_parts_data(tmp_save.type_);
						if (vec_data != nullptr) {
							temp_p = &(*vec_data)[tmp_save.cang_];
						}
					}
					{
						if (yp < 100) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * (yp - 50) / 50, 0, 255));
						}
						else if (t_disp_y - (yp + ys + 25) < 0) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * ((t_disp_y - (yp + ys + 25)) + 100) / 50, 0, 255));
						}
						else {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
						}
						font_big->DrawString(xp, yp, parts_type, GetColor(0, 255, 0));
						Draw_per_info(xp, yp + font_bighight, xs, ys - font_bighight, temp_p, use_vr);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				font_big->DrawString(xp, 25, set_name, GetColor(255, 0, 0));

				//
				if ((GetNowHiPerformanceCount() / 100000) % 10 <= 5) {
					font->DrawString(xp + xs, 50, "→", GetColor(255, 0, 0));
					font->DrawString_RIGHT(xp, 50, "←", GetColor(255, 0, 0));
				}
				font->DrawString(100, 575, "SPACE  :GO EDIT", GetColor(255, 0, 0));
			}

		}
		void item_Draw(void) noexcept {
			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			//FontHandle* font_big = (use_vr) ? &font36 : &font24;
			//FontHandle* font = (use_vr) ? &font24 : &font18;
			//int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			//int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			//int fonthight = (use_vr) ? y_r(24) : y_r(18);
			//
		}
		void Dispose(void) noexcept {
		}
	};
	//カスタム画面
	class UI_CUSTOM {
	private:
		int t_disp_x = 1920;
		int t_disp_y = 1080;
		//font
		FontHandle font72;
		FontHandle font48;
		FontHandle font36;
		FontHandle font24;
		FontHandle font18;
		FontHandle font12;
		void Draw_per_info(int xpos1, int ypos1, int xsize, int ysize, GUNPARTs* parts, const float&change_per, bool use_vr = true) noexcept {
			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			//const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			const int fonthight = (use_vr) ? y_r(24) : y_r(18 + 2);

			int back_siz = y_r(3);
			int title_siz = y_r(6);
			int main_siz = y_r(10);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(xpos1 + back_siz, ypos1 + back_siz, xpos1 + xsize - back_siz, ypos1 + int(float(ysize) * (1.f - change_per)) - back_siz, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f*(1.f - change_per)));
			{
				{
					float siz = float(font_big->GetDrawWidth((parts != nullptr) ? parts->per.name : "NONE"));
					int base_siz = (xsize - title_siz * 2);
					if (siz < base_siz) {
						font_big->DrawString(xpos1 + title_siz, ypos1 + title_siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
					}
					else {
						font_big->DrawExtendString(xpos1 + title_siz, ypos1 + title_siz, base_siz / siz, base_siz / siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
					}
				}
				if (parts != nullptr) {
					std::wstring msg = StringToWString(parts->per.info);
					int i = 0, siz = (xsize - main_siz * 2) / fonthight - 1;
					while (true) {
						if ((title_siz * 2 + font_bighight + fonthight * (i + 1)) > ysize * (1.f - change_per)) {
							break;
						}
						font->DrawString(xpos1 + main_siz, ypos1 + title_siz * 2 + font_bighight + fonthight * i, WStringToString(msg.substr(0, siz)), GetColor(0, 255, 0));
						i++;
						if (msg.length() < siz) {
							break;
						}
						else {
							msg = msg.substr(siz);
						}
					}
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	public:
		UI_CUSTOM(void) noexcept {

			t_disp_x = deskx;
			t_disp_y = desky;

			SetUseASyncLoadFlag(TRUE);
			this->font72 = FontHandle::Create(y_r(72), DX_FONTTYPE_EDGE);
			this->font48 = FontHandle::Create(y_r(48), DX_FONTTYPE_EDGE);
			this->font36 = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
			this->font24 = FontHandle::Create(y_r(24), DX_FONTTYPE_EDGE);
			this->font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
			this->font12 = FontHandle::Create(y_r(12), DX_FONTTYPE_EDGE);
			SetUseASyncLoadFlag(FALSE);
		}
		~UI_CUSTOM(void) noexcept {
		}
		template<class Y, class D>
		void UI_Draw(std::unique_ptr<Y, D>& MAINLOOPscene, size_t& parts_cat, switchs &Rot, PLAYERclass::Chara* mine_ptr, GUNPARTs* parts_p, float& change_per, bool use_vr = true) noexcept {
			if (use_vr) {
				GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			}

			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			//const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			const int fonthight = (use_vr) ? y_r(24) : y_r(18);
			int xs = 0, ys = 0, xp = 0, yp = 0;
			{
				int i = 0;
				/*
				//font->DrawStringFormat(100, 300, GetColor(255, 0, 0), "total : %d / %d", sel_p + 1, sel_max);
				//font->DrawStringFormat(100, 350, GetColor(255, 0, 0), "chang : %d / %d", chang + 1, chang_max);
				i = 0;
				for (auto& m : mine_ptr->mount_) {
					font->DrawString(400, 300 + 25*i, m.name, GetColor(255, 0, 0)); i ++;
				}

				i = 0;
				for (auto& m : mine_ptr->sight_) {
					font->DrawString(700, 300 + 25*i, m.name, GetColor(255, 0, 0)); i ++;
				}

				i = 0;
				for (auto& m : sight_i) {
					font->DrawStringFormat(1000, 300 + 25*i, GetColor(255, 0, 0), "%d", m); i ++;
				}
				//*/

				if (parts_cat != SIZE_MAX) {
					std::string parts_type = "";
					std::vector<GUNPARTs>* parts_t = nullptr;

					switch (parts_cat) {
					case EnumGunParts::PARTS_MAZZULE:
					{
						parts_type = "MAZZULE";
						break;
					}
					case EnumGunParts::PARTS_MAGAZINE:
					{
						parts_type = "MAGAZINE";
						break;
					}
					case EnumGunParts::PARTS_GRIP:
					{
						parts_type = "GRIP";
						break;
					}
					case EnumGunParts::PARTS_UPER_HGUARD:
					{
						parts_type = "UPER HANDGUARD";
						break;
					}
					case EnumGunParts::PARTS_UNDER_HGUARD:
					{
						parts_type = "UNDER HANDGUARD";
						break;
					}
					case EnumGunParts::PARTS_DUSTCOVER:
					{
						parts_type = "DUSTCOVER";
						break;
					}
					case EnumGunParts::PARTS_STOCK:
					{
						parts_type = "STOCK";
						break;
					}
					case EnumGunParts::PARTS_LAM:
					{
						parts_type = "LASER/LIGHT";
						break;
					}
					case EnumGunParts::PARTS_FOREGRIP:
					{
						parts_type = "FOREGRIP";
						break;
					}
					case EnumGunParts::PARTS_MOUNT:
					{
						parts_type = "MOUNT";
						break;
					}
					case EnumGunParts::PARTS_SIGHT:
					{
						parts_type = "SIGHT";
						break;
					}
					default:
						break;
					}
					parts_t = MAINLOOPscene->get_parts_data(parts_cat);
					//
					xp = t_disp_x - 600;
					yp = t_disp_y - 400;

					xs = 500;
					ys = 175;
					{
						font_big->DrawString(xp, yp, parts_type, GetColor(0, 255, 0));
						int xs_1 = 250;
						i = 0;
						for (auto& p : *parts_t) {
							auto ytmp = yp + font_bighight + fonthight * i;
							auto strtmp = p.per.name;
							int base_siz = (xs + 100 - xs_1) / fonthight;//todo 100で合う???????
							if (p.per.name.length() > base_siz) {
								strtmp = p.per.name.substr(0, base_siz) + "…";
							}
							font->DrawString(xp + xs_1, ytmp, strtmp, (&p == parts_p) ? GetColor(255, 0, 0) : GetColor(128, 0, 0));
							if (&p == parts_p) {
								DrawBox(xp + xs_1, ytmp, xp + xs_1 + font->GetDrawWidth(strtmp), ytmp + fonthight, GetColor(0, 255, 0), FALSE);

								Draw_per_info(xp, ytmp, xs_1, ys, parts_p, change_per, use_vr);
							}
							i++;
						}
						if (parts_p == nullptr) {
							Draw_per_info(xp, yp + font_bighight, xs_1, ys - font_bighight, parts_p, change_per, use_vr);
						}
					}
					//
					if ((GetNowHiPerformanceCount() / 100000) % 10 <= 5) {
						font->DrawString_MID(xp + xs / 2, yp - fonthight, "↑", GetColor(255, 0, 0));
						font->DrawString_MID(xp + xs / 2, yp + ys + font_bighight, "↓", GetColor(255, 0, 0));
						font->DrawString(xp + xs, yp + ys / 2, "→", GetColor(255, 0, 0));
						font->DrawString_RIGHT(xp, yp + ys / 2, "←", GetColor(255, 0, 0));
					}
				}

				font->DrawStringFormat(100, 700, GetColor(255, 0, 0), "weigt  : %5.2f", mine_ptr->get_per_all().weight);
				font->DrawStringFormat(100, 725, GetColor(255, 0, 0), "recoil : %5.2f", mine_ptr->get_per_all().recoil);


				font->DrawString(100, 575, "SPACE  :Go Battle", GetColor(255, 0, 0));
				font->DrawString(100, 600, (Rot.on()) ? "RANGE  :FREE" : "RANGE  :FIXED", GetColor(255, 0, 0));
			}

		}
		void item_Draw(void) noexcept {
			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			//FontHandle* font_big = (use_vr) ? &font36 : &font24;
			//FontHandle* font = (use_vr) ? &font24 : &font18;
			//int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			//int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			//int fonthight = (use_vr) ? y_r(24) : y_r(18);
			//
		}
		void Dispose(void) noexcept {
		}
	};
	//メイン画面
	class UI_MAINLOOP {
	private:
		int t_disp_x = 1920;
		int t_disp_y = 1080;
		//pic
		GraphHandle aim;
		GraphHandle hit_Graph;
		GraphHandle item;
		GraphHandle dmg;
		GraphHandle hit_rad;
		//font
		FontHandle font72;
		FontHandle font48;
		FontHandle font36;
		FontHandle font24;
		FontHandle font18;
		FontHandle font12;
		//
		float Ready = 0.f;
		float timer = 0.f;
		//
		std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>>* RULEparts;
		//
		void Draw_HP(int xpos, int ypos, int xsize, int ysize, PLAYERclass::Chara& chara) noexcept {
			int will = int(chara.HP_r);
			auto size = y_r(2);
			DrawBox(xpos - xsize / 2, ypos, xpos + xsize / 2, ypos + ysize, GetColor(64, 64, 64), TRUE);
			DrawBox(xpos - xsize / 2, ypos, xpos + xsize / 2, ypos + ysize, GetColor(128, 128, 128), FALSE);
			DrawBox(xpos - xsize / 2 + size, ypos + size, xpos - xsize / 2 + size + (xsize - size * 2)*chara.HP / chara.HP_full, ypos + ysize - size, GetColor(0, 255, 0), TRUE);
			DrawBox(xpos - xsize / 2 + size + (xsize - size * 2)*chara.HP / chara.HP_full, ypos + size, xpos - xsize / 2 + size + (xsize - size * 2)*will / chara.HP_full, ypos + ysize - size, GetColor(255, 255, 0), TRUE);
			font18.DrawStringFormat_MID(xpos + size, ypos + size, GetColor(255, 255, 255), "%d/%d", chara.HP, chara.HP_full);


			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f - powf(1.f - chara.got_damage_f, 5.f))), 0, 255));
			font18.DrawStringFormat_MID(xpos + (xsize / 2 * chara.got_damage_x / 255), ypos + size - int(100 * (1.f - chara.got_damage_f)), chara.got_damage_color, "%d", chara.got_damage);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		template <typename... Args>
		void Draw_Item_UI(FontHandle* font_big, int font_bighight, int xp, int yp, const VECTOR_ref& pos_item, const VECTOR_ref& cam_pos, std::string String, Args&&... args) const noexcept {
			VECTOR_ref p = ConvWorldPosToScreenPos(pos_item.get());
			if (p.z() >= 0.f&&p.z() <= 1.f) {
				int xp2 = int(p.x());
				int yp2 = int(p.y());
				float r_ = std::max((VECTOR_ref(pos_item) - cam_pos).size(), 1.f);
				if (r_ <= 10.f) {
					int xp3 = int(float(xp) / r_);
					int yp3 = int(float(yp) / r_);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f / std::max(r_ - 5.f, 1.f))), 0, 255));
					item.DrawRotaGraph(xp2, yp2, 3.f / r_, 0.f, true);
					int xs = font_big->GetDrawWidthFormat(String, args...);
					DrawLine(xp2 + xp3, yp2 + yp3, xp2 + xp3 + xs, yp2 + yp3, GetColor(64, 64, 64), 2);
					DrawLine(xp2, yp2, xp2 + xp3, yp2 + yp3, GetColor(64, 64, 64), 2);
					DrawLine(xp2 + xp3, yp2 + yp3, xp2 + xp3 + xs, yp2 + yp3, GetColor(0, 255, 0), 2);
					DrawLine(xp2, yp2, xp2 + xp3, yp2 + yp3, GetColor(0, 255, 0), 2);
					font_big->DrawStringFormat(xp2 + xp3, yp2 + yp3 - font_bighight, GetColor(0, 255, 0), String, args...);
				}
			}
		}
	public:
		UI_MAINLOOP(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t, std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>>* RULEparts_t) noexcept {
			MAPPTs = MAPPTs_t;
			RULEparts = RULEparts_t;
			SetUseASyncLoadFlag(TRUE);
			this->hit_rad = GraphHandle::Load("data/UI/enemyrad.png");
			this->hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			this->dmg = GraphHandle::Load("data/UI/damage.png");
			this->item = GraphHandle::Load("data/UI/battle_item.bmp");
			this->aim = GraphHandle::Load("data/UI/battle_aim.bmp");

			this->font72 = FontHandle::Create(y_r(72), DX_FONTTYPE_EDGE);
			this->font48 = FontHandle::Create(y_r(48), DX_FONTTYPE_EDGE);
			this->font36 = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
			this->font24 = FontHandle::Create(y_r(24), DX_FONTTYPE_EDGE);
			this->font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
			this->font12 = FontHandle::Create(y_r(12), DX_FONTTYPE_EDGE);

			SetUseASyncLoadFlag(FALSE);

			t_disp_x = deskx;
			t_disp_y = desky;
		}
		~UI_MAINLOOP(void) noexcept {
		}
		void Update(void) noexcept {
			this->Ready = (*RULEparts)->get_Ready();
			this->timer = std::max((*RULEparts)->get_timer(),0.f);
		}
		void UI_Draw(PLAYERclass::Chara& chara, bool use_vr = true) noexcept {
			if (use_vr) {
				GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			}

			FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			const int fonthight = (use_vr) ? y_r(24) : y_r(18);
			int xs = 0, ys = 0, xp = 0, yp = 0;
			{
				//HP表示
				{
					auto ratio = (1.f - float(float(chara.HP) / chara.HP_full));
					if (ratio > 1.f / 255.f) {
						if (use_vr) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(128.f*ratio), 0, 255));
							DrawBox(0, 0, t_disp_x, t_disp_y, GetColor(128, 0, 0), TRUE);
						}
						else {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*ratio), 0, 255));
							dmg.DrawExtendGraph(0, 0, t_disp_x, t_disp_y, true);
						}
					}
					//ダメージ
					ratio = (float(int(chara.HP_r) - chara.HP) / 50.f);
					if (ratio > 1.f / 255.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*ratio), 0, 255));
						DrawBox(0, 0, t_disp_x, t_disp_y, GetColor(128, 0, 0), TRUE);
					}
				}
				//
				if (use_vr) {
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					aim.DrawRotaGraph(int(t_disp_x / 2), int(t_disp_y / 2), y_r(64) / 200.f, 0.f, true);
				}
				//タイマー
				{
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (Ready >= 0.f) {
						if (use_vr) {
							xp = t_disp_x / 2;
							yp = t_disp_y / 2 - t_disp_y / 3;
						}
						else {
							xp = t_disp_x / 2;
							yp = t_disp_y / 2 - t_disp_y / 8;
						}
						font_large->DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "%02d:%02d", int(this->Ready) / 60, int(this->Ready) % 60); yp += font_largehight;
					}
					else {
						if (use_vr) {
							xp = t_disp_x / 2;
							yp = t_disp_y / 2 - t_disp_y / 8;
						}
						else {
							xp = t_disp_x / 2;
							yp = fonthight;
						}
						font_big->DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "%02d:%02d", int(this->timer) / 60, int(this->timer) % 60); yp += font_bighight;
					}
				}
				//警告
				{
					if ((!use_vr && !chara.ads_on()) || use_vr) {
						if ((GetNowHiPerformanceCount() / 100000) % 4 <= 2) {
							//空警告
							if (!chara.gun_stat_now->not_chamber_EMPTY()) {
								if (use_vr) {
									xp = t_disp_x / 2;
									yp = t_disp_y / 2 + t_disp_y / 8;
								}
								else {
									xp = t_disp_x / 2;
									yp = t_disp_y / 2 + t_disp_y / 8;
								}
								font->DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0));
							}
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
					switch (chara.get_now_selector()) {
					case EnumSELECTER::SELECT_SEMI:
						font_big->DrawString_MID(xp, yp, "SEMI AUTO", GetColor(0, 255, 0));
						break;
					case EnumSELECTER::SELECT_FULL:
						font_big->DrawString_MID(xp, yp, "FULL AUTO", GetColor(0, 255, 0));
						break;
					case EnumSELECTER::SELECT_3B:
						font_big->DrawString_MID(xp, yp, "3B", GetColor(0, 255, 0));
						break;
					case EnumSELECTER::SELECT_2B:
						font_big->DrawString_MID(xp, yp, "2B", GetColor(0, 255, 0));
						break;
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
					if (chara.get_canget_magitem()) {
						font->DrawString_MID(xp, yp, chara.get_canget_mag() + "を拾う", GetColor(255, 255, 0)); yp += fonthight;
					}

					if (chara.get_canget_meditem()) {
						font->DrawString_MID(xp, yp, chara.get_canget_med() + "を拾う", GetColor(255, 255, 0)); yp += fonthight;
					}
				}
				//弾薬
				{
					if ((!use_vr && !chara.ads_on()) || use_vr) {
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
							font->DrawString(xp, yp, chara.get_parts(EnumGunParts::PARTS_BASE)->thisparts->per.name, GetColor(255, 255, 255));
						}
						//マガジン関連(仮)
						{
							int size = int(chara.gun_stat_now->get_magazine_in().size());
							if (use_vr) {
								xp = t_disp_x / 2 - x_r(140) + font_bighight;
								yp = t_disp_y / 2 + t_disp_y / 6 + y_r(20) - font_bighight * size;
							}
							else {
								xp = x_r(220) + font_bighight;
								yp = t_disp_y - x_r(20) - font_bighight * size;
							}
							for (auto& a : chara.gun_stat_now->get_magazine_in()) {
								font_big->DrawStringFormat(xp, yp, GetColor(255, 0, 0), "%d/%d", a, chara.get_mag_g_parts()->thisparts->cap);
								if (&a - &chara.gun_stat_now->get_magazine_in()[0] == 0) {
									if (chara.gun_stat_now->not_chamber_EMPTY()) {
										font->DrawString(xp + font_big->GetDrawWidthFormat("%d/%d", a, chara.get_mag_g_parts()->thisparts->cap), yp, " +1", GetColor(255, 0, 0));
									}
									DrawBox(xp, yp, xp + font_big->GetDrawWidthFormat("%d/%d", a, chara.get_mag_g_parts()->thisparts->cap), yp + font_bighight + 1, GetColor(255, 255, 0), FALSE);
									xp -= font_bighight / 3;
								}
								yp += font_bighight;
							}
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
					DrawBox(xp - int(pow(per, 2)) * t_disp_x / 2 / int(pow(255, 2)), yp, xp + int(pow(per, 2)) * t_disp_x / 2 / int(pow(255, 2)), yp + font_bighight + 2, GetColor(255, 255, 255), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*((chara.kill_time * 2) / 7.f)), 0, 255));
					font_big->DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "プレイヤー%d をキルしました", chara.kill_id); yp += font_bighight * 2;	//キル

					if (chara.kill_streak > 0) {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						DrawBox(xp - int(pow(per, 4)) * t_disp_x / 2 / int(pow(255, 4)), yp, xp + int(pow(per, 4)) * t_disp_x / 2 / int(pow(255, 4)), yp + font_bighight + 2, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*((chara.kill_time * 2) / 7.f)), 0, 255));
						font_big->DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "kill streak! x%d", chara.kill_streak); yp += font_bighight;			//キルストリーク
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				//被弾方向
				{
					if (use_vr) {
						xp = t_disp_x / 2;
						yp = t_disp_y / 2;
					}
					else {
						xp = t_disp_x / 2;
						yp = t_disp_y / 2;
					}
					for (auto& d : chara.got_damage_) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*(1.f - powf(1.f - d.alpfa, 5.f))), 0, 255));
						this->hit_rad.DrawRotaGraph(xp, yp, float(y_r(100 * 1.25f)) / 100.f*((1.f - 0.3f) + (d.alpfa*0.3f)), d.rad, true);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				//HP
				{
					if (use_vr) {
						xp = t_disp_x / 2 + t_disp_y / 12;
						yp = t_disp_y / 2 + t_disp_y / 6;
						xs = x_r(200);
						ys = y_r(18) + y_r(2);
					}
					else {
						xp = t_disp_x / 2;
						yp = t_disp_y - font_bighight * 2;
						xs = x_r(600);
						ys = y_r(18) + y_r(2);
					}
					Draw_HP(xp, yp, xs, ys, chara);
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
					font_big->DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "score      : %d", chara.get_score()); yp += font_bighight;			//スコア
					font_big->DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "kill       : %d", chara.get_kill_cnt()); yp += font_bighight;			//キルデス
					font_big->DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "death      : %d", chara.get_death_cnt()); yp += font_bighight;			//キルデス
					font_big->DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "kill/death : %3.1f", float(chara.get_kill_cnt()) / float(std::max(chara.get_death_cnt(), 1))); yp += font_bighight;			//キルデス
				}
				//終わり
			}
		}
		void item_Draw(std::vector<PLAYERclass::Chara>&chara, PLAYERclass::Chara&mine, const VECTOR_ref& cam_pos, bool use_vr = true) noexcept {
			//FontHandle* font_large = (use_vr) ? &font72 : &font48;
			FontHandle* font_big = (use_vr) ? &font36 : &font24;
			FontHandle* font = (use_vr) ? &font24 : &font18;
			//int font_largehight = (use_vr) ? y_r(72) : y_r(48);
			int font_bighight = (use_vr) ? y_r(36) : y_r(24);
			int fonthight = (use_vr) ? y_r(24) : y_r(18);
			//弾インジケーター
			if (use_vr) {
				auto pos_gun = mine.get_parts(EnumGunParts::PARTS_BASE)->obj.GetMatrix().pos();
				VECTOR_ref p = ConvWorldPosToScreenPos(pos_gun.get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					int xp = 0, yp = 0;
					xp = int(p.x());
					yp = int(p.y());
					int cnt = int(mine.gun_stat_now->get_magazine_in().front());
					int per = 90 * cnt / int(mine.get_mag_g_parts()->thisparts->cap);
					float rad = deg2rad(90 - per);
					int col_r = GetColor(std::clamp(int(255.f*sin(rad)*2.f), 0, 255), std::clamp(int(255.f*cos(rad)*2.f), 0, 255), 0);
					if (std::max((pos_gun - cam_pos).size(), 1.f) <= 10.f) {
						int siz = font->GetDrawWidthFormat("%04d", cnt);
						for (int i = 0; i < 4; i++) {
							DrawBox(xp - siz / 2 + siz * i / 4 + 2 + 1, yp - y_r(30) - i * 2 + 1, xp - siz / 2 + siz * (i + 1) / 4 - 2 + 1, yp - y_r(10) + 1, GetColor(128, 128, 128), TRUE);
						}
						for (int i = 0; i < std::clamp(int(4 * per / 90) + int(per != 0), 0, 4); i++) {
							DrawBox(xp - siz / 2 + siz * i / 4 + 2, yp - y_r(30) - i * 2, xp - siz / 2 + siz * (i + 1) / 4 - 2, yp - y_r(10), col_r, TRUE);
						}
						if (!mine.gun_stat_now->not_chamber_EMPTY()) {
							//空警告
							if ((GetNowHiPerformanceCount() / 100000) % 4 <= 2) {
								font->DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0)); yp += fonthight;
							}
						}
						else {
							font->DrawStringFormat_MID(xp, yp, col_r, "%04d", cnt);
						}
					}
				}
			}
			//アイテム
			{
				SetCameraNearFar(0.01f, 10.f);
				for (auto& g : (*MAPPTs)->item) {
					if (g.flag_canlook_player) {
						//mag
						if (g.get_ptr_mag() != nullptr) {
							Draw_Item_UI(font_big, font_bighight, y_r(144.f), y_r(144.f), g.get_pos_(), cam_pos, "%s %d/%d", g.get_ptr_mag()->per.name.c_str(), g.get_magazine().cap, g.get_ptr_mag()->cap);
						}
						//med
						if (g.get_ptr_med() != nullptr) {
							Draw_Item_UI(font_big, font_bighight, y_r(144.f), y_r(144.f), g.get_pos_(), cam_pos, "%s", g.get_ptr_med()->mod.get_name().c_str());
						}
						//
					}
				}
				//
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//HP
			{
				SetCameraNearFar(0.01f, 100.f);
				for (auto& c : chara) {
					auto p = c.Set_HP_UI(cam_pos);
					if (p.z() >= 0.f && p.z() <= 1.f) {
						this->Draw_HP(int(p.x()), int(p.y()), x_r(140), y_r(20), c);
					}
				}
			}
			//ヒット
			{
				SetCameraNearFar(0.01f, 100.f);
				for (auto& c : chara) {
					c.Draw_Hit_UI(hit_Graph);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//
		}
	};
	//ローディング画面
	class UI_LOADING {
	private:
		FontHandle font18;
		float bar = 0.f;
		int tmp = 0;
		int all = 0;
		int t_disp_x = 1920;
		int t_disp_y = 1080;
		std::string message;
	public:
		UI_LOADING(void) noexcept {
			t_disp_x = deskx;
			t_disp_y = desky;

			SetUseASyncLoadFlag(TRUE);

			font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);

			SetUseASyncLoadFlag(FALSE);
		}
		~UI_LOADING(void) noexcept {
		}
		void Set(const char* mes) noexcept {
			SetUseASyncLoadFlag(FALSE);
			bar = 0.f;
			all = GetASyncLoadNum();
			message = mes;
		}
		bool Update(void) noexcept {
			tmp = all - GetASyncLoadNum();
			if (int(bar * 100) >= (all * 100 - 1)) {
				return false;
			}
			easing_set(&bar, float(tmp), 0.95f);
			return true;
		}
		void UI_Draw(bool use_vr = true) noexcept {
			if (use_vr) {
				GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			}

			DrawBox(0, 0, t_disp_x, t_disp_y, GetColor(0, 0, 0), TRUE);
			font18.DrawStringFormat(0, t_disp_y - y_r(70), GetColor(0, 255, 0), " loading... : %04d/%04d  ", tmp, all);
			font18.DrawStringFormat_RIGHT(t_disp_x, t_disp_y - y_r(70), GetColor(0, 255, 0), "%s 読み込み中 ", message.c_str());
			DrawBox(0, t_disp_y - y_r(50), int(float(t_disp_x) * bar / float(all)), t_disp_y - y_r(40), GetColor(0, 255, 0), TRUE);
		}
		void Dispose(void) noexcept {
		}
	};
};
