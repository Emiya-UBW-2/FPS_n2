#pragma once
#include"Header.hpp"
//
namespace FPS_n2 {
	class UIclass {
	public:
		class UI_TEMP {
		private:
			bool first_f{ true };
		protected:
			//引き継ぐ
			std::shared_ptr<DXDraw> DrawPts;
			std::shared_ptr<MAPclass::Map> MAPPTs;
			//font
			FontPool::Fonthave* Font72 = nullptr;
			FontPool::Fonthave* Font48 = nullptr;
			FontPool::Fonthave* Font36 = nullptr;
			FontPool::Fonthave* Font24 = nullptr;
			FontPool::Fonthave* Font18 = nullptr;

			FontPool::Fonthave* Large;
			FontPool::Fonthave* Middle;
			FontPool::Fonthave* Small;
		public:
			void Init(std::shared_ptr<DXDraw>& DrawPts_t, std::shared_ptr<MAPclass::Map>& MAPPTs_t) noexcept {
				if (first_f) {
					first_f = false;
					DrawPts = DrawPts_t;
					MAPPTs = MAPPTs_t;
					this->Font72 = Fonts.Get_haveptr(y_r(72));
					this->Font48 = Fonts.Get_haveptr(y_r(48));
					this->Font36 = Fonts.Get_haveptr(y_r(36));
					this->Font24 = Fonts.Get_haveptr(y_r(24));
					this->Font18 = Fonts.Get_haveptr(y_r(18));
				}
			}
			void set_fonts() {
				Large = (DrawPts->use_vr) ? this->Font72 : this->Font48;
				Middle = (DrawPts->use_vr) ? this->Font36 : this->Font24;
				Small = (DrawPts->use_vr) ? this->Font24 : this->Font18;
			}
			//virtual void UI_Draw(void) noexcept {}
			//virtual void item_Draw(void) noexcept {}
			virtual void Dispose(void) noexcept {
			}
		};
		//ロードするセーブデータを指定する
		class UI_LOAD : public UI_TEMP {
		private:
			int ypos = 0;
			float ypos_real = 0.f;
			void Draw_per_info(int xpos1, int ypos1, int xsize, int ysize, GUNPARTs* parts) noexcept {
				set_fonts();

				int back_siz = y_r(3);
				int title_siz = y_r(6);
				int main_siz = y_r(10);
				if (ypos1 < 100) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(128 * (ypos1 - 50) / 50, 0, 128));
				}
				else if (DrawPts->disp_y - (ypos1 + ysize + 25) < 0) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(128 * ((DrawPts->disp_y - (ypos1 + ysize + 25)) + 100) / 50, 0, 128));
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				}
				DrawBox(xpos1 + back_siz, ypos1 + back_siz, xpos1 + xsize - back_siz, ypos1 + ysize - back_siz, GetColor(0, 0, 0), TRUE);
				if (ypos1 < 100) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * (ypos1 - 50) / 50, 0, 255));
				}
				else if (DrawPts->disp_y - (ypos1 + ysize + 25) < 0) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * ((DrawPts->disp_y - (ypos1 + ysize + 25)) + 100) / 50, 0, 255));
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
				{
					{
						float siz = float(Middle->Get_handle().GetDrawWidth((parts != nullptr) ? parts->per.name : "NONE"));
						int base_siz = (xsize - title_siz * 2);
						if (siz < base_siz) {
							Middle->Get_handle().DrawString(xpos1 + title_siz, ypos1 + title_siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
						}
						else {
							Middle->Get_handle().DrawExtendString(xpos1 + title_siz, ypos1 + title_siz, base_siz / siz, base_siz / siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
						}
					}
					if (parts != nullptr) {
						std::wstring msg = StringToWString(parts->per.info);
						int i = 0, siz = (xsize - main_siz * 2) / (Small->Get_size() * 5 / 4) - 1;//todo
						while (true) {
							if ((title_siz * 2 + Middle->Get_size() + Small->Get_size() * (i + 1)) > ysize) {
								break;
							}
							Small->Get_handle().DrawString(xpos1 + main_siz, ypos1 + title_siz * 2 + Middle->Get_size() + Small->Get_size() * i, WStringToString(msg.substr(0, siz)), GetColor(0, 255, 0));
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
			void UI_Draw(std::shared_ptr<GUNPARTS_Control>& GunPartses, std::vector<save_c>& save_parts, const std::string& set_name) noexcept {
				set_fonts();
				//DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(192, 192, 192), TRUE);
				int xs = 0, ys = 0, xp = 0, yp = 0;
				{
					int i = 0;
					int cnt = 0;
					xs = y_r(800);
					ys = y_r(175);
					ypos += GetMouseWheelRotVol() * y_r(30);
					//*
					if (ypos >= 0) {
						ypos = 0;
					}
					//*/

					//*
					if (ypos <= (DrawPts->disp_y - (y_r(100) + (ys + y_r(25)) * int(save_parts.size())))) {
						ypos = DrawPts->disp_y - (y_r(100) + (ys + y_r(25)) * int(save_parts.size()));
					}
					//*/

					easing_set(&ypos_real, float(ypos), 0.8f);

					while (true) {
						GUNPARTs* temp_p = nullptr;
						std::string parts_type = "";

						auto& tmp_save = save_parts[cnt % save_parts.size()];
						cnt++;

						xp = DrawPts->disp_x - (xs + y_r(100));
						yp = y_r(100) + int(ypos_real) + (ys + y_r(25)) * i;
						i++;
						if (yp + ys < 0) {
							continue;
						}
						if (yp > DrawPts->disp_y) {
							break;
						}
						if (i > save_parts.size()) {
							break;
						}
						if (tmp_save.cang_ != SIZE_MAX) {
							auto& vec_data = GunPartses->Get_Parts_Data(tmp_save.type_);
							if (&vec_data != nullptr) {
								temp_p = &vec_data[tmp_save.cang_];
							}
						}
						{
							if (yp < 100) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * (yp - 50) / 50, 0, 255));
							}
							else if (DrawPts->disp_y - (yp + ys + y_r(25)) < 0) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 * ((DrawPts->disp_y - (yp + ys + y_r(25))) + 100) / 50, 0, 255));
							}
							else {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
							}
							Middle->Get_handle().DrawString(xp, yp, parts_type, GetColor(0, 255, 0));
							Draw_per_info(xp, yp + Middle->Get_size(), xs, ys - Middle->Get_size(), temp_p);
						}
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					Middle->Get_handle().DrawString(xp, y_r(25), set_name, GetColor(255, 0, 0));

					//
					if ((GetNowHiPerformanceCount() / 100000) % 10 <= 5) {
						Small->Get_handle().DrawString(xp + xs, y_r(50), "→", GetColor(255, 0, 0));
						Small->Get_handle().DrawString_RIGHT(xp, y_r(50), "←", GetColor(255, 0, 0));
					}
					Small->Get_handle().DrawString(y_r(100), y_r(575), "SPACE  :GO EDIT", GetColor(255, 0, 0));
				}
			}
		};
		//カスタム画面
		class UI_CUSTOM : public UI_TEMP {
		private:
			void Draw_per_info(int xpos1, int ypos1, int xsize, int ysize, GUNPARTs* parts, const float& change_per) noexcept {
				set_fonts();

				int back_siz = y_r(3);
				int title_siz = y_r(6);
				int main_siz = y_r(10);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				DrawBox(xpos1 + back_siz, ypos1 + back_siz, xpos1 + xsize - back_siz, ypos1 + int(float(ysize) * (1.f - change_per)) - back_siz, GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * (1.f - change_per)));
				{
					{
						float siz = float(Middle->Get_handle().GetDrawWidth((parts != nullptr) ? parts->per.name : "NONE"));
						int base_siz = (xsize - title_siz * 2);
						if (siz < base_siz) {
							Middle->Get_handle().DrawString(xpos1 + title_siz, ypos1 + title_siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
						}
						else {
							Middle->Get_handle().DrawExtendString(xpos1 + title_siz, ypos1 + title_siz, base_siz / siz, base_siz / siz, (parts != nullptr) ? parts->per.name : "NONE", GetColor(0, 255, 0));
						}
					}
					if (parts != nullptr) {
						std::wstring msg = StringToWString(parts->per.info);
						int i = 0, siz = (xsize - main_siz * 2) / Small->Get_size() - 1;
						while (true) {
							if ((title_siz * 2 + Middle->Get_size() + Small->Get_size() * (i + 1)) > ysize * (1.f - change_per)) {
								break;
							}
							Small->Get_handle().DrawString(xpos1 + main_siz, ypos1 + title_siz * 2 + Middle->Get_size() + Small->Get_size() * i, WStringToString(msg.substr(0, siz)), GetColor(0, 255, 0));
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
			void UI_Draw(std::shared_ptr<GUNPARTS_Control>& GunPartses, EnumGunParts parts_cat, const bool& Rot, const std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine, GUNPARTs* parts_p, float& change_per) noexcept {
				set_fonts();
				int xs = 0, ys = 0, xp = 0, yp = 0;
				{
					int i = 0;
					/*
					//Small->Get_handle().DrawStringFormat(100, 300, GetColor(255, 0, 0), "total : %d / %d", sel_p + 1, sel_max);
					//Small->Get_handle().DrawStringFormat(100, 350, GetColor(255, 0, 0), "chang : %d / %d", chang + 1, chang_max);
					i = 0;
					for (auto& m : mine->mount_) {
						Small->Get_handle().DrawString(400, 300 + 25*i, m.name, GetColor(255, 0, 0)); i ++;
					}

					i = 0;
					for (auto& m : mine->sight_) {
						Small->Get_handle().DrawString(700, 300 + 25*i, m.name, GetColor(255, 0, 0)); i ++;
					}

					i = 0;
					for (auto& m : sight_i) {
						Small->Get_handle().DrawStringFormat(1000, 300 + 25*i, GetColor(255, 0, 0), "%d", m); i ++;
					}
					//*/

					if (parts_cat != EnumGunParts::PARTS_NONE) {
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
						parts_t = &GunPartses->Get_Parts_Data(parts_cat);
						//
						xp = DrawPts->disp_x - y_r(600);
						yp = DrawPts->disp_y - y_r(400);

						xs = y_r(500);
						ys = y_r(175);
						{
							Middle->Get_handle().DrawString(xp, yp, parts_type, GetColor(0, 255, 0));
							int xs_1 = y_r(250);
							i = 0;
							for (auto& p : *parts_t) {
								auto ytmp = yp + Middle->Get_size() + Small->Get_size() * i;
								std::string strtmp = p.per.name;
								int base_siz = (xs + y_r(100) - xs_1) / Small->Get_size();//todo 100で合う?
								if (p.per.name.length() > base_siz) {
									strtmp = p.per.name.substr(0, base_siz) + "…";
								}
								Small->Get_handle().DrawString(xp + xs_1, ytmp, strtmp, (&p == parts_p) ? GetColor(255, 0, 0) : GetColor(128, 0, 0));
								if (&p == parts_p) {
									DrawBox(xp + xs_1, ytmp, xp + xs_1 + Small->Get_handle().GetDrawWidth(strtmp), ytmp + Small->Get_size(), GetColor(0, 255, 0), FALSE);
									Draw_per_info(xp, ytmp, xs_1, ys, parts_p, change_per);
								}
								i++;
							}
							if (parts_p == nullptr) {
								Draw_per_info(xp, yp + Middle->Get_size(), xs_1, ys - Middle->Get_size(), parts_p, change_per);
							}
						}
						//
						if ((GetNowHiPerformanceCount() / 100000) % 10 <= 5) {
							Small->Get_handle().DrawString_MID(xp + xs / 2, yp - Small->Get_size(), "↑", GetColor(255, 0, 0));
							Small->Get_handle().DrawString_MID(xp + xs / 2, yp + ys + Middle->Get_size(), "↓", GetColor(255, 0, 0));
							Small->Get_handle().DrawString(xp + xs, yp + ys / 2, "→", GetColor(255, 0, 0));
							Small->Get_handle().DrawString_RIGHT(xp, yp + ys / 2, "←", GetColor(255, 0, 0));
						}
					}

					Small->Get_handle().DrawStringFormat(y_r(100), y_r(700), GetColor(255, 0, 0), "weigt  : %5.2f", mine->Get_per_all().weight);
					Small->Get_handle().DrawStringFormat(y_r(100), y_r(725), GetColor(255, 0, 0), "recoil : %5.2f", mine->Get_per_all().recoil);


					Small->Get_handle().DrawString(y_r(100), y_r(575), "SPACE  :Go Battle", GetColor(255, 0, 0));
					Small->Get_handle().DrawString(y_r(100), y_r(600), (Rot) ? "RANGE  :FREE" : "RANGE  :FIXED", GetColor(255, 0, 0));
				}
			}
		};
		//メイン画面
		class UI_MAINLOOP : public UI_TEMP {
		private:
			GraphHandle aim;
			GraphHandle hit_Graph;
			GraphHandle item;
			GraphHandle dmg;
			GraphHandle hit_rad;
			GraphHandle aim_view;

			GraphHandle aim_ret0;
			GraphHandle aim_ret1;
			GraphHandle aim_ret2;
			float Ready = 0.f;
			float timer = 0.f;
			std::shared_ptr<RULE_parts> RULEparts{ nullptr };
			float dist{ 1.f };
			float ratio{ 1.f };
			float changeview = 0.f;
		private:
			void Draw_HP(int xpos, int ypos, int xsize, int ysize, const std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine) noexcept {
				auto size = y_r(2);
				int x1 = xpos - xsize / 2;
				float size_y = float(ysize - size) / Small->Get_size();
				int nowHP = (int)(mine->Damage.HP_Per() * (xsize - size * 2));
				int willHP = (int)(mine->Damage.HP_r_Per() * (xsize - size * 2));
				//back
				DrawBox(x1, ypos, x1 + xsize, ypos + ysize + size, GetColor(128, 128, 128), FALSE);
				//
				x1 += size;
				if (willHP < xsize) {
					DrawBox(x1 + willHP, ypos + size, x1 + xsize, ypos + ysize + size, GetColor(64, 64, 64), TRUE);
				}
				if (0 < nowHP) {
					DrawBox(x1, ypos + size, x1 + nowHP, ypos + ysize + size, GetColor(0, 255, 0), TRUE);
				}
				if (nowHP < willHP) {
					DrawBox(x1 + nowHP, ypos + size, x1 + willHP, ypos + ysize + size, GetColor(255, 255, 0), TRUE);
				}
				Small->Get_handle().DrawExtendStringFormat_MID(xpos + size, ypos + size, size_y, size_y, GetColor(255, 255, 255), "%d/%d", mine->Damage.Get_HP(), mine->Damage.Get_HP_full());

				if (1.f - mine->Damage.Get_got_damage_f() >= 0.01f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * (1.f - powf(1.f - mine->Damage.Get_got_damage_f(), 5.f))), 0, 255));
					Small->Get_handle().DrawExtendStringFormat_MID(xpos + (xsize / 2 * mine->Damage.Get_got_damage_x() / 255), ypos + size - int(100 * (1.f - mine->Damage.Get_got_damage_f())), size_y, size_y, mine->Damage.Get_got_damage_color(), "%d", mine->Damage.Get_got_damage());
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
			template <typename... Args>
			void Draw_Item_UI(int xp, int yp, const VECTOR_ref& pos_item, const std::string& String, Args&&... args) const noexcept {
				VECTOR_ref p = ConvWorldPosToScreenPos(pos_item.get());
				if (p.z() >= 0.f && p.z() <= 1.f) {
					int xp2 = int(p.x());
					int yp2 = int(p.y());
					float r_ = std::max((VECTOR_ref(pos_item) - GetCameraPosition()).size(), 1.f);
					if (r_ <= 10.f) {
						int xp3 = int(float(xp) / r_);
						int yp3 = int(float(yp) / r_);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * (1.f / std::max(r_ - 5.f, 1.f))), 0, 255));
						item.DrawRotaGraph(xp2, yp2, 3.f / r_, 0.f, true);
						int xs = Middle->Get_handle().GetDrawWidthFormat(String, args...);
						DrawLine(xp2 + xp3, yp2 + yp3, xp2 + xp3 + xs, yp2 + yp3, GetColor(64, 64, 64), 2);
						DrawLine(xp2, yp2, xp2 + xp3, yp2 + yp3, GetColor(64, 64, 64), 2);
						DrawLine(xp2 + xp3, yp2 + yp3, xp2 + xp3 + xs, yp2 + yp3, GetColor(0, 255, 0), 2);
						DrawLine(xp2, yp2, xp2 + xp3, yp2 + yp3, GetColor(0, 255, 0), 2);
						Middle->Get_handle().DrawStringFormat(xp2 + xp3, yp2 + yp3 - Middle->Get_size(), GetColor(0, 255, 0), String, args...);
					}
				}
			}
		public:
			UI_MAINLOOP(std::shared_ptr<RULE_parts>& RULEparts_t) noexcept {
				RULEparts = RULEparts_t;
				SetUseASyncLoadFlag(TRUE);
				this->hit_rad = GraphHandle::Load("data/UI/enemyrad.png");
				this->hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				this->dmg = GraphHandle::Load("data/UI/damage.png");
				this->item = GraphHandle::Load("data/UI/battle_item.bmp");
				this->aim = GraphHandle::Load("data/UI/battle_aim.bmp");
				this->aim_view = GraphHandle::Load("data/UI/battle_scope.png");
				this->aim_ret0 = GraphHandle::Load("data/UI/USSR/0.png");
				this->aim_ret1 = GraphHandle::Load("data/UI/USSR/1.png");
				this->aim_ret2 = GraphHandle::Load("data/UI/USSR/2.png");
				SetUseASyncLoadFlag(FALSE);
			}
			~UI_MAINLOOP(void) noexcept {
				this->hit_rad.Dispose();
				this->hit_Graph.Dispose();
				this->dmg.Dispose();
				this->item.Dispose();
				this->aim.Dispose();
				this->aim_view.Dispose();
				this->aim_ret0.Dispose();
				this->aim_ret1.Dispose();
				this->aim_ret2.Dispose();
			}
			void Update(void) noexcept {
				this->Ready = RULEparts->Get_Ready();
				this->timer = std::max(RULEparts->Get_timer(), 0.f);
			}
			void UI_Draw(const std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine, std::shared_ptr<key_bind>& KeyBind) noexcept {
				set_fonts();
				int xs = 0, ys = 0, xp = 0, yp = 0;
				{
					//スコープ
					if (mine->isRide()) {
						if ((*mine->MINE_v)->ads_on()) {
							aim_view.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, true);

							
							easing_set(&dist, (*mine->MINE_v)->Get_dist(), 0.9f);
							easing_set(&ratio, (*mine->MINE_v)->Get_ratio(), 0.9f);

							aim_ret0.DrawRotaGraph(int(DrawPts->disp_x / 2), int(DrawPts->disp_y / 2), (float)(y_r(300)) / 200.f, 0.f, true);
							aim_ret1.DrawRotaGraph(int(DrawPts->disp_x / 2), int(DrawPts->disp_y / 2) - y_r(dist / 16), (float)(y_r(300)) / 200.f, 0.f, true);
							aim_ret2.DrawRotaGraph(int(DrawPts->disp_x / 2), int(DrawPts->disp_y / 2), (float)(y_r(300)) / 200.f, 0.f, true);
							
							Small->Get_handle().DrawStringFormat(x_r(1056), y_r(594), GetColor(0, 255, 0), "[%05.1f]", dist);
							Small->Get_handle().DrawStringFormat(x_r(1056), y_r(648), GetColor(0, 255, 0), "[x%02.1f]", ratio);
						}
						if ((*mine->MINE_v)->Get_changeview()) {
							changeview = 2.f;
						}
					}

					if (mine->changeRide()) {
						changeview = 2.f;
					}

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * changeview), 0, 255));
					DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					easing_set(&changeview, 0.f, 0.9f);


					//HP表示
					{
						auto ratio_t = (1.f - mine->Damage.HP_Per());
						if (ratio_t > 1.f / 255.f) {
							if (DrawPts->use_vr) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(128.f * ratio_t), 0, 255));
								DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(128, 0, 0), TRUE);
							}
							else {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * ratio_t), 0, 255));
								dmg.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, true);
							}
						}
						//ダメージ
						ratio_t = (float(int(mine->Damage.Get_HP_r()) - mine->Damage.Get_HP()) / 50.f);
						if (ratio_t > 1.f / 255.f) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * ratio_t), 0, 255));
							DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(128, 0, 0), TRUE);
						}
					}
					//
					if (DrawPts->use_vr) {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						aim.DrawRotaGraph(int(DrawPts->disp_x / 2), int(DrawPts->disp_y / 2), (float)(y_r(64)) / 200.f, 0.f, true);
					}
					//タイマー
					{
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						if (Ready >= 0.f) {
							if (DrawPts->use_vr) {
								xp = DrawPts->disp_x / 2;
								yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 3;
							}
							else {
								xp = DrawPts->disp_x / 2;
								yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 8;
							}
							Large->Get_handle().DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "%02d:%02d", int(this->Ready) / 60, int(this->Ready) % 60); yp += Large->Get_size();
						}
						else {
							if (DrawPts->use_vr) {
								xp = DrawPts->disp_x / 2;
								yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 8;
							}
							else {
								xp = DrawPts->disp_x / 2;
								yp = Small->Get_size();
							}
							Middle->Get_handle().DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "%02d:%02d", int(this->timer) / 60, int(this->timer) % 60); yp += Middle->Get_size();
						}
					}
					//警告
					{
						if ((!DrawPts->use_vr && !mine->ads_on()) || DrawPts->use_vr) {
							if ((GetNowHiPerformanceCount() / 100000) % 4 <= 2) {
								//空警告
								if (!mine->gun_stat_now->not_chamber_EMPTY()) {
									if (DrawPts->use_vr) {
										xp = DrawPts->disp_x / 2;
										yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 8;
									}
									else {
										xp = DrawPts->disp_x / 2;
										yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 8;
									}
									Small->Get_handle().DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0));
								}
							}
						}
					}
					//セレクター
					{
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2 - DrawPts->disp_y / 10;
							yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 8;
						}
						else {
							xp = DrawPts->disp_x / 2 + DrawPts->disp_y / 4;
							yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 6;
						}
						switch ((EnumSELECTER)mine->Get_now_selector()) {
						case EnumSELECTER::SELECT_SEMI:
							Middle->Get_handle().DrawString_MID(xp, yp, "SEMI AUTO", GetColor(0, 255, 0));
							break;
						case EnumSELECTER::SELECT_FULL:
							Middle->Get_handle().DrawString_MID(xp, yp, "FULL AUTO", GetColor(0, 255, 0));
							break;
						case EnumSELECTER::SELECT_3B:
							Middle->Get_handle().DrawString_MID(xp, yp, "3B", GetColor(0, 255, 0));
							break;
						case EnumSELECTER::SELECT_2B:
							Middle->Get_handle().DrawString_MID(xp, yp, "2B", GetColor(0, 255, 0));
							break;
						}
					}
					//アイテムを拾う
					{
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 12;
						}
						else {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 12;
						}
						if (mine->Get_canget_mag_f()) {
							Small->Get_handle().DrawString_MID(xp, yp, KeyBind->Get_key_use_ID(EnumKeyBind::GET_ITEM).second + ": " + mine->Get_canget_mag_s(), GetColor(255, 255, 0)); yp += Small->Get_size();
						}

						if (mine->Get_canget_med_f()) {
							Small->Get_handle().DrawString_MID(xp, yp, KeyBind->Get_key_use_ID(EnumKeyBind::GET_ITEM).second + ": " + mine->Get_canget_med(), GetColor(255, 255, 0)); yp += Small->Get_size();
						}

						if (mine->Get_canride_f()) {
							Small->Get_handle().DrawString_MID(xp, yp, KeyBind->Get_key_use_ID(EnumKeyBind::RIDE_ON).second, GetColor(255, 255, 0)); yp += Small->Get_size();
						}
					}
					//弾薬
					{
						if ((!DrawPts->use_vr && !mine->ads_on()) || DrawPts->use_vr) {
							if (DrawPts->use_vr) {
								xp = DrawPts->disp_x / 2 - y_r(20) - y_r(200);
								yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 6 + y_r(20);
							}
							else {
								xp = y_r(20);
								yp = DrawPts->disp_y - y_r(50);
							}
							//銃
							{
								Small->Get_handle().DrawString(xp, yp, mine->Get_parts(EnumGunParts::PARTS_BASE)->thisparts->per.name, GetColor(255, 255, 255));
							}
							//マガジン関連(仮)
							{
								int size = int(mine->Get_mag_in().size());
								if (DrawPts->use_vr) {
									xp = DrawPts->disp_x / 2 - y_r(140) + Middle->Get_size();
									yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 6 + y_r(20) - Middle->Get_size() * size;
								}
								else {
									xp = y_r(220) + Middle->Get_size();
									yp = DrawPts->disp_y - y_r(20) - Middle->Get_size() * size;
								}
								for (auto& a : mine->Get_mag_in()) {
									Middle->Get_handle().DrawStringFormat(xp, yp, GetColor(255, 0, 0), "%d/%d", a.m_cnt, a.cap);
									if (&a == &mine->Get_mag_in().front()) {
										if (mine->gun_stat_now->not_chamber_EMPTY()) {
											Small->Get_handle().DrawString(xp + Middle->Get_handle().GetDrawWidthFormat("%d/%d", a.m_cnt, a.cap), yp + Middle->Get_size() - Small->Get_size(), " +1", GetColor(255, 0, 0));
										}
										DrawTriangle(xp - y_r(10), yp + 5, xp, yp + Middle->Get_size() / 2, xp - y_r(10), yp + Middle->Get_size() - 5, GetColor(255, 255, 0), FALSE);
										xp -= Middle->Get_size() / 3;
									}
									yp += Middle->Get_size();
								}
							}
						}
					}
					//キル
					if (mine->scores.kill_f) {
						auto killt = mine->scores.kill_timer;
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 6;
						}
						else {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 6;
						}
						int per = std::clamp(int(255.f * ((killt * 7) / 7.f)) - 255 * 6, 0, 255);

						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						DrawBox(xp - int(pow(per, 2)) * DrawPts->disp_x / 2 / int(pow(255, 2)), yp, xp + int(pow(per, 2)) * DrawPts->disp_x / 2 / int(pow(255, 2)), yp + Middle->Get_size() + 2, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * ((killt * 2) / 7.f)), 0, 255));
						Middle->Get_handle().DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "プレイヤー%d をキルしました", mine->scores.kill_id); yp += Middle->Get_size() * 2;	//キル

						if (mine->scores.kill_streak > 0) {
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							DrawBox(xp - int(pow(per, 4)) * DrawPts->disp_x / 2 / int(pow(255, 4)), yp, xp + int(pow(per, 4)) * DrawPts->disp_x / 2 / int(pow(255, 4)), yp + Middle->Get_size() + 2, GetColor(255, 255, 255), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * ((killt * 2) / 7.f)), 0, 255));
							Middle->Get_handle().DrawStringFormat_MID(xp, yp, GetColor(255, 0, 0), "kill streak! x%d", mine->scores.kill_streak); yp += Middle->Get_size();			//キルストリーク
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					//被弾方向
					{
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2;
						}
						else {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y / 2;
						}
						for (const auto& d : mine->Damage.Get_got_damage_()) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * (1.f - powf(1.f - d.alpfa, 5.f))), 0, 255));
							this->hit_rad.DrawRotaGraph(xp, yp, float(y_r(100 * 1.25f)) / 100.f * ((1.f - 0.3f) + (d.alpfa * 0.3f)), d.rad, true);
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					//HP
					{
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2 + DrawPts->disp_y / 12;
							yp = DrawPts->disp_y / 2 + DrawPts->disp_y / 6;
							xs = y_r(200);
							ys = y_r(18) + y_r(2);
						}
						else {
							xp = DrawPts->disp_x / 2;
							yp = DrawPts->disp_y - Middle->Get_size() * 2;
							xs = y_r(600);
							ys = y_r(18) + y_r(2);
						}
						Draw_HP(xp, yp, xs, ys, mine);
					}
					//スコアその他
					{
						/*
						if (DrawPts->use_vr) {
							xp = DrawPts->disp_x / 2 + DrawPts->disp_y / 6 + Middle->Get_size();
							yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 12 + Middle->Get_size();
						}
						else {
							xp = DrawPts->disp_x - Middle->Get_size();
							yp = DrawPts->disp_y / 2 - DrawPts->disp_y / 12 + Middle->Get_size();
						}
						Middle->Get_handle().DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "score      : %d", mine->scores.score); yp += Middle->Get_size();			//スコア
						Middle->Get_handle().DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "kill       : %d", mine->scores.kill_cnt); yp += Middle->Get_size();			//キルデス
						Middle->Get_handle().DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "death      : %d", mine->scores.death_cnt); yp += Middle->Get_size();			//キルデス
						Middle->Get_handle().DrawStringFormat_RIGHT(xp, yp, GetColor(255, 0, 0), "kill/death : %3.1f", float(mine->scores.kill_cnt) / float(std::max(mine->scores.death_cnt, 1))); yp += Middle->Get_size();			//キルデス
						//*/
					}
					//終わり
				}
			}
			void item_Draw(
				const std::vector<std::shared_ptr<PLAYERclass::PLAYER_CHARA>>& chara,
				const std::vector<std::shared_ptr<PLAYERclass::PLAYER_VEHICLE>>& vehicle,
				const std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine) noexcept {
				set_fonts();
				//弾インジケーター
				if (DrawPts->use_vr) {
					auto pos_gun = mine->Get_parts(EnumGunParts::PARTS_BASE)->Get_objMatrix().pos();
					VECTOR_ref p = ConvWorldPosToScreenPos(pos_gun.get());
					if (p.z() >= 0.f && p.z() <= 1.f) {
						int xp = int(p.x());
						int yp = int(p.y());
						int cnt = int(mine->Get_mag_in().front().m_cnt);
						int per = 90 * cnt / int(mine->Get_mag_in().front().cap);
						float rad = deg2rad(90 - per);
						int col_r = GetColor(std::clamp(int(255.f * sin(rad) * 2.f), 0, 255), std::clamp(int(255.f * cos(rad) * 2.f), 0, 255), 0);
						if (std::max((pos_gun - GetCameraPosition()).size(), 1.f) <= 10.f) {
							int siz = Small->Get_handle().GetDrawWidthFormat("%04d", cnt);
							for (int i = 0; i < 4; i++) {
								DrawBox(xp - siz / 2 + siz * i / 4 + 2 + 1, yp - y_r(30) - i * 2 + 1, xp - siz / 2 + siz * (i + 1) / 4 - 2 + 1, yp - y_r(10) + 1, GetColor(128, 128, 128), TRUE);
							}
							for (int i = 0; i < std::clamp(int(4 * per / 90) + int(per != 0), 0, 4); i++) {
								DrawBox(xp - siz / 2 + siz * i / 4 + 2, yp - y_r(30) - i * 2, xp - siz / 2 + siz * (i + 1) / 4 - 2, yp - y_r(10), col_r, TRUE);
							}
							if (!mine->gun_stat_now->not_chamber_EMPTY()) {
								//空警告
								if ((GetNowHiPerformanceCount() / 100000) % 4 <= 2) {
									Small->Get_handle().DrawString_MID(xp, yp, "EMPTY", GetColor(255, 0, 0)); yp += Small->Get_size();
								}
							}
							else {
								Small->Get_handle().DrawStringFormat_MID(xp, yp, col_r, "%04d", cnt);
							}
						}
					}
				}
				//アイテム
				{
					SetCameraNearFar(0.01f, 10.f);
					for (auto& g : MAPPTs->item) {
						if (g->flag_canlook_player) {
							//mag
							if (g->Get_ptr_mag() != nullptr) {
								Draw_Item_UI(y_r(144), y_r(144), g->Get_pos_(), "%s %d/%d", g->Get_ptr_mag()->per.name.c_str(), g->Get_magazine().mag_cnt, g->Get_ptr_mag()->mag_cnt);
							}
							//med
							if (g->Get_ptr_med() != nullptr) {
								Draw_Item_UI(y_r(144), y_r(144), g->Get_pos_(), "%s", g->Get_ptr_med()->mod.Get_name().c_str());
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
						auto p = c->Set_HP_UI();
						if (p.z() >= 0.f && p.z() <= 1.f) {
							this->Draw_HP(int(p.x()), int(p.y()), y_r(140), y_r(20), c);
						}
					}
				}
				//ヒット
				{
					SetCameraNearFar(0.01f, 100.f);
					for (auto& c : chara) {
						c->Draw_Hit_UI(hit_Graph);
					}
					for (auto& c : vehicle) {
						c->Draw_Hit_UI(hit_Graph);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					//todo:戦車の表示も
				}
				//
			}
		};
		//ローディング画面
		class UI_LOADING : public UI_TEMP {
		private:
			float bar = 0.f;
			int tmp = 0;
			int all = 0;
			std::string message;
		public:
			UI_LOADING(void) noexcept {
			}
			~UI_LOADING(void) noexcept {
			}
			void Set(const std::string& mes) noexcept {
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
			void UI_Draw(void) noexcept {
				set_fonts();
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
				Small->Get_handle().DrawStringFormat(0, DrawPts->disp_y - y_r(70), GetColor(0, 255, 0), " loading... : %04d/%04d  ", tmp, all);
				Small->Get_handle().DrawStringFormat_RIGHT(DrawPts->disp_x, DrawPts->disp_y - y_r(70), GetColor(0, 255, 0), "%s 読み込み中 ", message.c_str());
				DrawBox(0, DrawPts->disp_y - y_r(50), int(float(DrawPts->disp_x) * bar / float(all)), DrawPts->disp_y - y_r(40), GetColor(0, 255, 0), TRUE);
			}
		};
	};
};
