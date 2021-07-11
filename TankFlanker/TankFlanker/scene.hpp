#pragma once
#include"Header.hpp"

namespace MAIN_ {
	class Sceneclass {
	public:
		class LOAD;
		class SELECT;
		class MAINLOOP;
		//
		class TEMPSCENE {
		private:
			//
		protected:
			//引き継ぐ
			std::shared_ptr<DXDraw> DrawPts;
			std::shared_ptr<OPTION> OPTPTs;
			std::shared_ptr<MAPclass::Map> MAPPTs;
			std::shared_ptr<MAINLOOP> MAINLOOPscene;
			std::shared_ptr<DeBuG> DebugPTs;		//仮
			//UIsound
			SoundHandle decision;
			SoundHandle cancel;
			SoundHandle cursor;
			//カメラ
			cam_info camera_main;
			float fov_base = DX_PI_F / 2;
			//
			bool use_lens = false;
			float lens_zoom = 1.f;
			float lens_size = 1.f;
			//
			bool use_bless = false;
			float bless_ratio = 0.5f;
			float bless = 0.f;
			//
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
			COLOR_F Light_color_ref = GetColorF(0, 0, 0, 0);

			std::vector<EffekseerEffectHandle>* effsorce{ nullptr };
		public:
			void Init(
				const std::shared_ptr<DXDraw>& DrawPts_t,
				const std::shared_ptr<OPTION>& OPTPTs_t,
				const std::shared_ptr<MAPclass::Map>& MAPPTs_t,
				const std::shared_ptr<MAINLOOP>& MAINLOOPscene_t,
				const std::shared_ptr<DeBuG>& DebugPTs_t,
				std::vector<EffekseerEffectHandle>* effsorce_t
			) noexcept {
				DrawPts = DrawPts_t;
				OPTPTs = OPTPTs_t;
				MAPPTs = MAPPTs_t;
				MAINLOOPscene = MAINLOOPscene_t;
				DebugPTs = DebugPTs_t;
				effsorce = effsorce_t;

				this->decision = SoundHandle::Load("data/audio/chara/shot_2.wav");//
				this->cancel = SoundHandle::Load("data/audio/chara/cancel.wav");
				this->cursor = SoundHandle::Load("data/audio/chara/cursor.wav");
			}


			const VECTOR_ref& get_Shadow_minpos(void) const noexcept { return Shadow_minpos; }
			const VECTOR_ref& get_Shadow_maxpos(void) const noexcept { return Shadow_maxpos; }
			const VECTOR_ref& get_Light_vec(void) const noexcept { return Light_vec; }
			const COLOR_F& get_Light_color(void) const noexcept { return Light_color; }
			const COLOR_F& get_Light_color_ref(void) const noexcept { return Light_color_ref; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Set(void) noexcept {
				fov_base = deg2rad(DrawPts->use_vr ? 120 : OPTPTs->Fov);	//fov
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
				SetMousePoint(deskx / 2, desky / 2);											//
				camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
			}
			virtual bool UpDate(void) noexcept {
				if (use_bless) {
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * FRAME_RATE / GetFPS());
					bless_ratio -= (0.03f / 90.f) * FRAME_RATE / GetFPS();
					if (bless_ratio <= 0.f) {
						use_bless = false;
					}
				}
				return true;
			}
			virtual void Dispose(void) noexcept {
			}
			virtual void UI_Draw(void) noexcept {
			}
			virtual void BG_Draw(void) noexcept {
			}
			virtual void Shadow_Draw_Far(void) noexcept {
			}
			virtual void Shadow_Draw_NearFar(void) noexcept {
			}
			virtual void Shadow_Draw(void) noexcept {
			}
			virtual void Main_Draw(void) noexcept {
			}
			virtual const bool& is_lens(void) { return use_lens; }
			virtual const float& zoom_lens(void) { return lens_zoom; }
			virtual const float& size_lens(void) { return lens_size; }

			virtual const bool& is_bless(void) { return use_bless; }
			virtual const float& ratio_bless(void) { return bless_ratio; }
			virtual const float& time_bless(void) { return bless; }

			virtual void Item_Draw(void) noexcept {
			}
			virtual void LAST_Draw(void) noexcept {
			}
		};
		//
		class LOADING : public TEMPSCENE {
		private:
			std::string title;
			//そのまま
			std::unique_ptr<UIclass::UI_LOADING> UIparts;
		public:
			//
			LOADING(void) noexcept {
				UIparts = std::make_unique<UIclass::UI_LOADING>();
			}

			void settitle(const std::string_view mes) {
				title = mes;
			}

			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Init(DrawPts, MAPPTs);
				UIparts->Set(title);
			}
			bool UpDate(void) noexcept override {
				TEMPSCENE::UpDate();
				return UIparts->Update();
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw();
			}
			void Dispose(void) noexcept override {
				UIparts->Dispose();
			}
		};
		//
		class LOAD : public TEMPSCENE {
		private:
			//キー
			switchs left;
			switchs right;
			//データ
			GUNPARTs* temp_p{ nullptr };
			//
			int preset_select = 0;
			int preset_select_max = 0;
			std::vector<save_c> save_parts;
			std::vector<std::string> save_presets;
			//そのまま
			std::unique_ptr<UIclass::UI_LOAD> UIparts;
		public:
			std::string_view putout_preset() { return save_presets[preset_select]; }
			//
			LOAD(void) noexcept {
				UIparts = std::make_unique<UIclass::UI_LOAD>();
			}
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Init(DrawPts, MAPPTs);
				{
					left.Init(false);
					right.Init(false);
				}
				{
					save_presets.clear();
					WIN32_FIND_DATA win32fdt;
					HANDLE hFind;
					hFind = FindFirstFile("data/save/*", &win32fdt);
					if (hFind != INVALID_HANDLE_VALUE) {
						do {
							//if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {}
							if ((win32fdt.cFileName[0] != '.')) {
								save_presets.emplace_back(win32fdt.cFileName);
							}
						} while (FindNextFile(hFind, &win32fdt));
					} //else{ return false; }
					FindClose(hFind);
					preset_select_max = int(save_presets.size());
				}
				/*パーツデータをロード*/
				{
					std::fstream file;
					save_parts.clear();
					file.open(("data/save/" + save_presets[0]).c_str(), std::ios::binary | std::ios::in);
					save_c savetmp;
					while (true) {
						file.read((char*)&savetmp, sizeof(savetmp));
						if (file.eof()) {
							break;
						}
						this->save_parts.emplace_back(savetmp);
					}
					file.close();
				}
			}
			bool UpDate(void) noexcept override {
				TEMPSCENE::UpDate();
				bool changef = false;
				//演算
				{
					SetMouseDispFlag(TRUE);
					left.GetInput(MAINLOOPscene->Get_Mine()->get_key_().akey);
					right.GetInput(MAINLOOPscene->Get_Mine()->get_key_().dkey);
					if (left.trigger()) {
						--preset_select;
						changef = true;
					}
					if (right.trigger()) {
						++preset_select;
						changef = true;
					}
					if (preset_select < 0) { preset_select = preset_select_max - 1; }
					if (preset_select > preset_select_max - 1) { preset_select = 0; }
					if (changef) {
						/*パーツデータをロード*/
						std::fstream file;
						save_parts.clear();
						file.open(("data/save/" + save_presets[preset_select]).c_str(), std::ios::binary | std::ios::in);
						save_c savetmp;
						while (true) {
							file.read((char*)&savetmp, sizeof(savetmp));
							if (file.eof()) {
								break;
							}
							this->save_parts.emplace_back(savetmp);
						}
						file.close();
					}
					//
				}
				if (MAINLOOPscene->Get_Mine()->get_key_().jamp) {
					return false;
				}
				return true;
				//
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw(MAINLOOPscene, save_parts, save_presets[preset_select]);
			}
		};
		//
		class SELECT : public TEMPSCENE {
		private:
			//sound
			SoundHandle assemble;
			SoundHandle shot_se;
			SoundHandle slide_se;
			SoundHandle trigger_se;
			//キー
			switchs up;
			switchs down;
			switchs left;
			switchs right;
			switchs shot;
			//データ
			GUNPARTs* parts_p{ nullptr };
			//
			EnumGunParts parts_cat = EnumGunParts::PARTS_NONE;
			EnumAttachPoint port_cat = EnumAttachPoint::POINTS_NONE;
			EnumGunParts port_type = EnumGunParts::PARTS_NONE;
			PLAYERclass::Chara::g_parts* port_ptr{ nullptr };
			std::vector<PLAYERclass::Chara::g_parts*> sight_ptr;
			int sight_p_s = 0;
			//
			int parts_select = 0;
			int parts_select_max = 0;
			int change_select = 0;
			int change_select_max = 0;
			//
			VECTOR_ref viewparts;
			VECTOR_ref viewparts_buf;
			VECTOR_ref camparts;
			float xrad_t = 30.f;
			float yrad_t = -135.f;
			float range_t = 0.25f;
			float range_tgt = 0.25f;
			switchs Rot;
			bool Start_b{ true };
			float rate = 1.0f;
			//
			float change_per = 1.f;
			//
			std::vector<save_c> save_parts;
			std::string save_tgt = "1";
			//そのまま
			std::unique_ptr<UIclass::UI_CUSTOM> UIparts;
		public:
			std::string preset;
		private:
			//パーツのセーブ設定
			void set_pts_save(size_t chang_t) {
				if (save_parts.size() < size_t(parts_select_max) + 1) {
					save_parts.resize(size_t(parts_select_max) + 1);
				}
				save_parts[parts_select_max].cang_ = chang_t;
				save_parts[parts_select_max].type_ = parts_cat;
				save_parts[parts_select_max].pt_cat_ = port_cat;
				save_parts[parts_select_max].pt_type_ = port_type;
				save_parts[parts_select_max].pt_sel_ = 0;
			}
			//必須品
			void set_pts_haveto(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				if (parts_select == parts_select_max) {
					port_type = port_type_t;
					port_ptr = MAINLOOPscene->Get_Mine()->get_parts(port_type);
					port_cat = port_cat_t;
					parts_cat = parts_cat_t;
					{
						change_select_max = (int)MAINLOOPscene->get_parts_data(parts_cat)->size();
						change_select = std::clamp(change_select, 0, change_select_max - 1);
						parts_p = &(*MAINLOOPscene->get_parts_data(parts_cat))[change_select];
						/*
						{
							bool aaa = false;
							auto total_t = (int)MAINLOOPscene->get_parts_data(parts_cat)->size();
							change_select_max = (int)MAINLOOPscene->get_parts_data(parts_cat)->size();
							change_select = std::clamp(change_select, 0, total_t - 1);
							while (true) {
								auto tmp = &(*MAINLOOPscene->get_parts_data(parts_cat))[change_select];

								aaa = false;
								for (auto& p : port_ptr->thisparts->can_attach) {
									if (p == tmp->mod.get_name()) {
										aaa = true;
										break;
									}
								}
								if (aaa) {
									//ok
									parts_p = tmp;
									change_select_max = total_t;
									break;
								}
								else {
									//NG
									total_t--;
									change_select = std::clamp(change_select++, 0, total_t - 1);
								}
								if (total_t == 0) {
									//NG
									break;
								}
							}
						}
						//*/

						viewparts_buf = port_ptr->Get_rail_pos(port_cat);
						MAINLOOPscene->Get_Mine()->Detach_parts(parts_cat);
						MAINLOOPscene->Get_Mine()->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
					}
					set_pts_save(size_t(change_select));
				}
				++parts_select_max;

				//サイト判定
				if (MAINLOOPscene->Get_Mine()->get_parts(parts_cat_t)->Set_sight_at(*MAINLOOPscene->get_parts_data(EnumGunParts::PARTS_SIGHT))) {
					sight_ptr[sight_p_s] = MAINLOOPscene->Get_Mine()->get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//非必須品
			void Set_chang_needs(EnumGunParts pts_cat, EnumAttachPoint pot_cat, int& chang_t, int sel = 0) noexcept {
				change_select_max = (int)MAINLOOPscene->get_parts_data(parts_cat)->size() + 1;
				chang_t = std::clamp(chang_t, 0, change_select_max - 1);
				if (chang_t == 0) { parts_p = nullptr; }
				else { parts_p = &(*MAINLOOPscene->get_parts_data(pts_cat))[std::max(chang_t - 1, 0)]; }

				viewparts_buf = port_ptr->Get_rail_pos(pot_cat);
				MAINLOOPscene->Get_Mine()->Detach_parts(pts_cat, (sel == 0) ? sel : (sel - 1));
				if (chang_t != 0) {
					MAINLOOPscene->Get_Mine()->Attach_parts(parts_p, pts_cat, port_ptr, pot_cat, sel);
				}
				set_pts_save(size_t(chang_t) - 1);
			}
			void set_pts_need(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				port_type = port_type_t;
				port_ptr = MAINLOOPscene->Get_Mine()->get_parts(port_type);
				port_cat = port_cat_t;
				if (port_ptr->Get_rail_frame(port_cat).first > 0) {
					if (parts_select == parts_select_max) {
						parts_cat = parts_cat_t;
						Set_chang_needs(parts_cat, port_cat, change_select);
					}
					++parts_select_max;
				}
				//サイト判定
				if (MAINLOOPscene->Get_Mine()->get_parts(parts_cat_t)->Set_sight_at(*MAINLOOPscene->get_parts_data(EnumGunParts::PARTS_SIGHT))) {
					sight_ptr[sight_p_s] = MAINLOOPscene->Get_Mine()->get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//sight
			void set_pts_need_sight(PLAYERclass::Chara::g_parts* s, int sel) {
				if (s != nullptr) {
					port_ptr = s;
					port_type = port_ptr->Get_type();

					port_cat = EnumAttachPoint::POINTS_UPER_RAIL;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (parts_select == parts_select_max) {
							parts_cat = EnumGunParts::PARTS_SIGHT;
							Set_chang_needs(parts_cat, port_cat, change_select, sel);
							if (port_type == EnumGunParts::PARTS_MOUNT) {
								save_parts[parts_select_max].pt_sel_ = 1;//改善
							}
						}
						++parts_select_max;
						return;
					}

					port_cat = EnumAttachPoint::POINTS_SIDEMOUNT;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (parts_select == parts_select_max) {
							parts_cat = EnumGunParts::PARTS_SIGHT;
							Set_chang_needs(parts_cat, port_cat, change_select, sel);
							if (port_type == EnumGunParts::PARTS_MOUNT) {
								save_parts[parts_select_max].pt_sel_ = 1;//改善
							}
						}
						++parts_select_max;
						return;
					}
				}
			}
		public:
			SELECT(void) noexcept {
				UIparts = std::make_unique<UIclass::UI_CUSTOM>();
				this->assemble = SoundHandle::Load("data/audio/chara/assemble.wav");
			}

			void Start(std::string_view set_str) {
				preset = set_str;
			}

			void Set(void) noexcept  override {
				TEMPSCENE::Set();
				UIparts->Init(DrawPts, MAPPTs);
				{
					up.Init(false);
					down.Init(false);
					left.Init(false);
					right.Init(false);
					shot.Init(false);
					Rot.Init(false);
					parts_p = nullptr;
					parts_cat = EnumGunParts::PARTS_NONE;
					parts_select = 0;
					change_select = 0;
					parts_select_max = 0;
					change_select_max = 0;
					viewparts_buf.clear();
					camparts.clear();
					xrad_t = 30.f;
					yrad_t = -135.f;
					range_t = 0.25f;
					range_tgt = 0.25f;
					Start_b = true;
					rate = 0.1f;
				}
				{
					SetCreateSoundTimeStretchRate(1.f / std::clamp(rate, 0.9f, 1.1f));
					shot_se = SoundHandle::Load(MAINLOOPscene->Get_Mine()->get_audio().shot_path);
					slide_se = SoundHandle::Load(MAINLOOPscene->Get_Mine()->get_audio().slide_path);
					trigger_se = SoundHandle::Load(MAINLOOPscene->Get_Mine()->get_audio().trigger_path);
					SetCreateSoundTimeStretchRate(1.f);
				}
				/*パーツデータをロード*/
				if (preset != "") {
					std::fstream file;
					save_parts.clear();
					file.open(("data/save/" + preset).c_str(), std::ios::binary | std::ios::in);
					save_c savetmp;
					while (true) {
						file.read((char*)&savetmp, sizeof(savetmp));
						if (file.eof()) { break; }
						this->save_parts.emplace_back(savetmp);
					}
					file.close();
				}
				//*
				else
				{
					save_parts.clear();
					//magazine
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_MAGAZINE;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_MAGAZINE_BASE;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//grip
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_GRIP;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_GRIP_BASE;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//uperhandguard
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_UPER_HGUARD;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_UPER_HANDGUARD;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//underhandguard
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_UNDER_HGUARD;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_UNDER_HANDGUARD;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//マズル
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_MAZZULE;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_MAZZULE_BASE;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//ダストカバー
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_DUSTCOVER;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_DUSTCOVER_BASE;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
					//ストック
					save_parts.resize(save_parts.size() + 1);
					save_parts.back().cang_ = 0;
					save_parts.back().type_ = EnumGunParts::PARTS_STOCK;
					save_parts.back().pt_cat_ = EnumAttachPoint::POINTS_STOCK_BASE;
					save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
					save_parts.back().pt_sel_ = 0;
				}
				//*/
				//読み込み
				{
					int pppp = 0;
					for (auto& tmp_save : save_parts) {
						GUNPARTs* temp_p = nullptr;
						PLAYERclass::Chara::g_parts* temp_ptr = nullptr;
						if (tmp_save.cang_ != SIZE_MAX) {
							auto vec_data = MAINLOOPscene->get_parts_data(tmp_save.type_);
							if (vec_data != nullptr) {
								temp_p = &(*vec_data)[tmp_save.cang_];
							}
						}
						{
							switch (tmp_save.pt_type_) {
							case EnumGunParts::PARTS_BASE:
							case EnumGunParts::PARTS_UPER_HGUARD:
							case EnumGunParts::PARTS_UNDER_HGUARD:
							case EnumGunParts::PARTS_DUSTCOVER:
							case EnumGunParts::PARTS_MOUNT_BASE:
							case EnumGunParts::PARTS_MOUNT:
								temp_ptr = MAINLOOPscene->Get_Mine()->get_parts(tmp_save.pt_type_);
								break;
							default:
								break;
							}
						}
						if (tmp_save.type_ == EnumGunParts::PARTS_SIGHT) {
							++pppp;
						}
						MAINLOOPscene->Get_Mine()->Attach_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_, pppp);
					}
					//save_parts.clear();
				}
				//ライティング
				Shadow_maxpos = VECTOR_ref::vget(1.f, 1.f, 1.f);
				Shadow_minpos = VECTOR_ref::vget(-1.f, -1.f, -1.f);
				Light_vec = VECTOR_ref::vget(0.5f, -0.5f, 0.5f);
				Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
				Light_color_ref = GetColorF(0.20f, 0.20f, 0.23f, 0.0f);
			}
			bool UpDate(void) noexcept override {
				TEMPSCENE::UpDate();
				bool changef = false;
				//演算
				{
					{
						moves tmp;
						MAINLOOPscene->Get_Mine()->set_gun_pos(tmp);
						MAINLOOPscene->Get_Mine()->Set_gun();
						MAINLOOPscene->Get_Mine()->set_mag_pos();
						MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_MAGAZINE)->Setpos_Nomal(MAINLOOPscene->Get_Mine()->Mag_Mat());
						//キー
						up.GetInput(MAINLOOPscene->Get_Mine()->get_key_().wkey);
						down.GetInput(MAINLOOPscene->Get_Mine()->get_key_().skey);
						left.GetInput(MAINLOOPscene->Get_Mine()->get_key_().akey);
						right.GetInput(MAINLOOPscene->Get_Mine()->get_key_().dkey);
						shot.GetInput(MAINLOOPscene->Get_Mine()->get_key_().shoot);
						Rot.GetInput(MAINLOOPscene->Get_Mine()->get_key_().select);
						//
						if (Start_b) {
							Start_b = false;
							//changef = true;
							int pp = MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_BASE)->thisparts->Select_Chose((uint8_t)EnumSELECTER::SELECT_SEMI);
							if (pp != -1) {
								MAINLOOPscene->Get_Mine()->gun_stat_now->selector_set(pp);
							}
						}
						if (left.trigger()) {
							parts_select--;
							change_select = 0;
							changef = true;
						}
						if (right.trigger()) {
							++parts_select;
							change_select = 0;
							changef = true;
						}
						if (parts_select < 0) { parts_select = parts_select_max - 1; }
						if (parts_select > parts_select_max - 1) { parts_select = 0; }

						if (up.trigger()) {
							change_select--;
							changef = true;
						}
						if (down.trigger()) {
							++change_select;
							changef = true;
						}
						if (change_select < 0) { change_select = change_select_max - 1; }
						if (change_select > change_select_max - 1) { change_select = 0; }
					}
					if (changef) {
						{
							Rot.first = false;
							port_ptr = nullptr;
							parts_select_max = 0;
							//サイト判定
							sight_ptr.resize(5);
							for (auto& s : sight_ptr) {
								s = nullptr;
							}
							sight_p_s = 0;
						}
						set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_MAGAZINE_BASE, EnumGunParts::PARTS_MAGAZINE);			//magazine
						set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_GRIP_BASE, EnumGunParts::PARTS_GRIP);					//grip
						set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_UPER_HANDGUARD, EnumGunParts::PARTS_UPER_HGUARD);		//uperhandguard
						set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_UNDER_HANDGUARD, EnumGunParts::PARTS_UNDER_HGUARD);	//underhandguard
						set_pts_need(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::POINTS_UNDER_RAIL, EnumGunParts::PARTS_FOREGRIP);		//foregrip
						set_pts_need(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::POINTS_LEFTSIDE_RAIL, EnumGunParts::PARTS_LAM);			//lam
						set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_SIDEMOUNT_BASE, EnumGunParts::PARTS_MOUNT_BASE);			//sidemount
						set_pts_need(EnumGunParts::PARTS_MOUNT_BASE, EnumAttachPoint::POINTS_SIDEMOUNT, EnumGunParts::PARTS_MOUNT);				//sidemount_2
						set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_MAZZULE_BASE, EnumGunParts::PARTS_MAZZULE);				//マズル
						set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_DUSTCOVER_BASE, EnumGunParts::PARTS_DUSTCOVER);			//ダストカバー
						set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_STOCK_BASE, EnumGunParts::PARTS_STOCK);					//ストック
						//sight
						{
							int ssp = parts_select_max;
							for (auto& s : sight_ptr) {
								set_pts_need_sight(s, (parts_select_max - ssp + 1));
							}
						}
						//
						{
							change_per = 1.f;
							assemble.play(DX_PLAYTYPE_BACK, TRUE);

							xrad_t = viewparts_buf.y() * 1000.f;
							yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z() * 100.f : -90.f + viewparts_buf.z() * 100.f);
						}
					}
					//
					easing_set(&MAINLOOPscene->Get_Mine()->get_gunanime_trigger()->per, float(shot.press()), 0.5f);
					if (shot.trigger()) {
						if (MAINLOOPscene->Get_Mine()->set_flag_gun()) {
							//todo ディレイつける
							{
								shot_se.play(DX_PLAYTYPE_BACK, TRUE);
								slide_se.play(DX_PLAYTYPE_BACK, TRUE);
								trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
								//薬莢
								MAINLOOPscene->Get_Mine()->create_cart();
							}
							//エフェクト
							MAINLOOPscene->Get_Mine()->calc_shot_effect();
						}
					}
					//薬莢
					MAINLOOPscene->Get_Mine()->calc_cart(rate);
					//銃セレクターアニメ
					MAINLOOPscene->Get_Mine()->Set_select_anime();
					//銃発砲アニメ
					MAINLOOPscene->Get_Mine()->Set_shot_anime(rate, true);
					//銃アニメ更新
					MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_BASE)->UpDate_Anim();
					//薬莢の処理
					MAINLOOPscene->Get_Mine()->update_cart();
					//エフェクトの更新
					MAINLOOPscene->Get_Mine()->update_effect(effsorce);
				}
				//campos,camvec,camupの指定
				{}
				{
					{
						int x_m, y_m;
						GetMousePoint(&x_m, &y_m);
						x_m -= deskx / 2;
						y_m -= desky / 2;
						SetMousePoint(deskx / 2, desky / 2);
						SetMouseDispFlag(FALSE);
						xrad_t = std::clamp(xrad_t + float(std::clamp(y_m, -60, 60)) * 0.1f, -80.f, 80.f);
						yrad_t += float(std::clamp(x_m, -60, 60)) * 0.1f;
						yrad_t = rad2deg(atan2f(sin(deg2rad(yrad_t)), cos(deg2rad(yrad_t))));
					}
					{
						if (Rot.on()) {
							viewparts.clear();
						}
						else {
							viewparts = viewparts_buf;
						}
					}
					range_tgt = std::hypotf(sin(deg2rad(yrad_t)) * 0.25f, cos(deg2rad(yrad_t)) * (std::abs((((std::abs(yrad_t) > 90) ? MAINLOOPscene->Get_Mine()->get_maz().z() : 0.5f) - viewparts.z())) + camera_main.near_ * 2.f));
					range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol()) * 0.1f, range_tgt, 5.f) : range_tgt;

					easing_set(&camparts, VECTOR_ref::vget(range_t * cos(deg2rad(xrad_t)) * sin(deg2rad(yrad_t)), range_t * sin(deg2rad(xrad_t)), range_t * cos(deg2rad(xrad_t)) * cos(deg2rad(yrad_t))), 0.8f);
					camera_main.camvec = viewparts;
					camera_main.campos = camera_main.camvec + camparts;
					camera_main.camup = VECTOR_ref::vget(0, 1.f, 0);
				}
				//表示の如何
				{
					cam_info* camp = &this->camera_main;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						MAINLOOPscene->Get_Mine()->Check_CameraViewClip(false);
					}
				}
				if (MAINLOOPscene->Get_Mine()->get_key_().jamp) {
					return false;
				}
				return true;
				//
			}
			void Dispose(void) noexcept override {
				//データセーブ
				{
					std::fstream file;
					file.open(("data/save/" + save_tgt + ".dat").c_str(), std::ios::binary | std::ios::out);
					for (auto& tmp_save : save_parts) {
						file.write((char*)&tmp_save, sizeof(tmp_save));
					}
					file.close();
				}
				MAINLOOPscene->Get_Mine()->gun_stat_now->selector_set(0);
				shot_se.Dispose();
				slide_se.Dispose();
				trigger_se.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				UIparts->UI_Draw(MAINLOOPscene, parts_cat, Rot.on(), MAINLOOPscene->Get_Mine(), parts_p, change_per);
				easing_set(&change_per, 0.f, 0.5f);
			}
			void BG_Draw(void) noexcept override {
				DrawBox(0, 0, 1920, 1080, GetColor(192, 192, 192), TRUE);
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				MAINLOOPscene->Get_Mine()->Draw_gun();
				{
					MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
				}
			}
			void Shadow_Draw(void) noexcept override {
				MAINLOOPscene->Get_Mine()->Draw_gun();
				{
					MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
				}
			}
			void Main_Draw(void) noexcept override {
				MAINLOOPscene->Get_Mine()->Draw_gun();
				{
					MAINLOOPscene->Get_Mine()->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
				}
			}
		};
		//
		class MAINLOOP : public TEMPSCENE {
			//TPS操作
			class TPS_parts {
				std::shared_ptr<MAPclass::Map> MAPPTs;
			public:
				switchs key_TPS;
				float xr_cam = 0.f;
				float yr_cam = 0.f;
				int sel_cam = 0;
				//カメラ
				cam_info camera_TPS;
				TPS_parts(const std::shared_ptr<MAPclass::Map>& MAPPTs_t) noexcept {
					MAPPTs = MAPPTs_t;
				}
				void Set(float& fov_pc) noexcept {
					this->key_TPS.Init(false);
					this->camera_TPS.campos = VECTOR_ref::vget(0, 1.8f, -10);
					this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
				}
				void Set_info(std::vector<std::shared_ptr<PLAYERclass::Chara>>& chara) noexcept {
					if (this->key_TPS.on()) {
						//cam
						for (int i = 0; i < std::min<size_t>(chara.size(), 10); ++i) {
							if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
								this->sel_cam = i;
							}
						}
						//pos
						{
							if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_UP) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * sin(this->yr_cam));
							}
							this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), MAPPTs->map_col_get().mesh_minpos(0).x(), MAPPTs->map_col_get().mesh_maxpos(0).x()));
							this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), MAPPTs->map_col_get().mesh_minpos(0).z(), MAPPTs->map_col_get().mesh_maxpos(0).z()));
						}
						//rad
						{
							VECTOR_ref vec_2 = (chara[this->sel_cam]->get_head_pos() - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

							this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50)) * 0.1f;
							this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50)) * 0.1f;
							this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
						}
						//
						this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, 1), MATRIX_ref::RotX(this->xr_cam) * MATRIX_ref::RotY(this->yr_cam));
						this->camera_TPS.camup = VECTOR_ref::vget(0, 1.f, 0);
					}
				}
			};
			//データ
			MV1 body_obj, body_obj_lag, body_col;		//身体モデル
			MV1 hit_pic;								//弾痕
			GraphHandle light;							//ライトテクスチャ
			std::vector<GUNPARTs> mazzule_data;			//GUNデータ
			std::vector<GUNPARTs> grip_data;			//GUNデータ
			std::vector<GUNPARTs> uperhandguard_data;	//GUNデータ
			std::vector<GUNPARTs> underhandguard_data;	//GUNデータ
			std::vector<GUNPARTs> stock_data;			//GUNデータ
			std::vector<GUNPARTs> dustcover_data;		//GUNデータ
			std::vector<GUNPARTs> sight_data;			//GUNデータ
			std::vector<GUNPARTs> foregrip_data;		//GUNデータ
			std::vector<GUNPARTs> lam_data;				//GUNデータ
			std::vector<GUNPARTs> mount_base_data;		//GUNデータ
			std::vector<GUNPARTs> mount_data;			//GUNデータ
			std::vector<GUNPARTs> gun_data;				//GUNデータ
			std::vector<GUNPARTs> magazine_data;		//GUNデータ
			std::vector<Meds> meds_data;				//GUNデータ
			std::vector<Grenades> gres_data;			//GUNデータ
			std::vector<std::shared_ptr<PLAYERclass::Chara>> chara;		//キャラ
			HIT_PASSIVE hit_obj_p;						//静的弾痕
			HIT_BLOOD_PASSIVE hit_b_obj_p;				//静的血痕
			std::unique_ptr<HostPassEffect> Hostpassparts_TPS;
			std::unique_ptr<TPS_parts> TPSparts;
			std::unique_ptr<UIclass::UI_MAINLOOP> UIparts;
			std::shared_ptr<RULE_parts> RULEparts;
			std::unique_ptr<MAPclass::MiniMap> minimapparts;

			EffekseerEffectHandle gndsmkHndle;		     /*エフェクトリソース*/
			std::vector<Vehcs> vehcs;
			std::vector<std::shared_ptr<PLAYERclass::vehicles>> vehicle;
		public:
			std::shared_ptr<PLAYERclass::Chara>& Get_Mine(void) noexcept { return this->chara[0]; }
			std::vector<GUNPARTs>* get_parts_data(EnumGunParts type_sel) noexcept {
				switch (type_sel) {
				case EnumGunParts::PARTS_MAGAZINE:
					return &this->magazine_data;
				case EnumGunParts::PARTS_GRIP:
					return &this->grip_data;
				case EnumGunParts::PARTS_UPER_HGUARD:
					return &this->uperhandguard_data;
				case EnumGunParts::PARTS_UNDER_HGUARD:
					return &this->underhandguard_data;
				case EnumGunParts::PARTS_MAZZULE:
					return &this->mazzule_data;
				case EnumGunParts::PARTS_DUSTCOVER:
					return &this->dustcover_data;
				case EnumGunParts::PARTS_STOCK:
					return &this->stock_data;
				case EnumGunParts::PARTS_LAM:
					return &this->lam_data;
				case EnumGunParts::PARTS_MOUNT_BASE:
					return &this->mount_base_data;
				case EnumGunParts::PARTS_MOUNT:
					return &this->mount_data;
				case EnumGunParts::PARTS_SIGHT:
					return &this->sight_data;
				case EnumGunParts::PARTS_FOREGRIP:
					return &this->foregrip_data;
				default:
					break;
				}
				return nullptr;
			}
			std::vector<Meds>& get_meds_data(void) noexcept { return this->meds_data; }
			std::vector<Grenades>& get_gres_data(void) noexcept { return this->gres_data; }
			void set_parts_data_from_folder(std::vector<GUNPARTs>* data, std::string file_name, EnumGunParts type_t = EnumGunParts::PARTS_NONE) noexcept {
				data->clear();
				std::string p;
				WIN32_FIND_DATA win32fdt;
				HANDLE hFind;
				hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
							data->resize(data->size() + 1);
							data->back().mod.Ready(file_name, win32fdt.cFileName);
							data->back().Set_type(type_t);
						}
					} while (FindNextFile(hFind, &win32fdt));
				} //else{ return false; }
				FindClose(hFind);
			}
			void set_mads_data_from_folder(std::vector<Meds>* data, std::string file_name) noexcept {
				data->clear();
				std::string p;
				WIN32_FIND_DATA win32fdt;
				HANDLE hFind;
				hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
							data->resize(data->size() + 1);
							data->back().mod.Ready(file_name, win32fdt.cFileName);
						}
					} while (FindNextFile(hFind, &win32fdt));
				} //else{ return false; }
				FindClose(hFind);
			}
			void set_gres_data_from_folder(std::vector<Grenades>* data, std::string file_name) noexcept {
				data->clear();
				std::string p;
				WIN32_FIND_DATA win32fdt;
				HANDLE hFind;
				hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
							data->resize(data->size() + 1);
							data->back().mod.Ready(file_name, win32fdt.cFileName);
						}
					} while (FindNextFile(hFind, &win32fdt));
				} //else{ return false; }
				FindClose(hFind);
			}
		public:
			MAINLOOP(const std::shared_ptr<MAPclass::Map>& MAPPTs_t, const std::shared_ptr<OPTION>& OPTPTs_t) noexcept {
				//
				MAPPTs = MAPPTs_t;
				OPTPTs = OPTPTs_t;
				//
				RULEparts = std::make_shared<RULE_parts>();
				UIparts = std::make_unique<UIclass::UI_MAINLOOP>(RULEparts);
				TPSparts = std::make_unique<TPS_parts>(MAPPTs);
				Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
				minimapparts = std::make_unique<MAPclass::MiniMap>(MAPPTs);															//ミニマップ
				//model
				light = GraphHandle::Load("data/light.png");							//ライト
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);			//弾痕
				//body
				MV1::Load("data/model/body/model.mv1", &this->body_obj, true);			//身体
				MV1::Load("data/model/body/col.mv1", &this->body_col, true);			//身体col
				MV1SetLoadModelPhysicsWorldGravity(-1.f);
				MV1::Load("data/model/body/model_lag.mv1", &this->body_obj_lag, true);	//身体
				MV1SetLoadModelPhysicsWorldGravity(-9.8f);
				//PARTSデータ
				set_parts_data_from_folder(&this->grip_data, "data/Guns/parts/grip/");
				set_parts_data_from_folder(&this->uperhandguard_data, "data/Guns/parts/uper_handguard/");
				set_parts_data_from_folder(&this->underhandguard_data, "data/Guns/parts/under_handguard/");
				set_parts_data_from_folder(&this->mount_base_data, "data/Guns/parts/mount_base/");
				set_parts_data_from_folder(&this->mount_data, "data/Guns/parts/mount/");
				set_parts_data_from_folder(&this->dustcover_data, "data/Guns/parts/dustcover/");
				set_parts_data_from_folder(&this->foregrip_data, "data/Guns/parts/foregrip/");
				set_parts_data_from_folder(&this->mazzule_data, "data/Guns/parts/mazzule/", EnumGunParts::PARTS_MAZZULE);
				set_parts_data_from_folder(&this->stock_data, "data/Guns/parts/stock/", EnumGunParts::PARTS_STOCK);
				set_parts_data_from_folder(&this->sight_data, "data/Guns/parts/sight/", EnumGunParts::PARTS_SIGHT);
				set_parts_data_from_folder(&this->lam_data, "data/Guns/parts/lam/", EnumGunParts::PARTS_LAM);
				set_parts_data_from_folder(&this->gun_data, "data/Guns/gun/", EnumGunParts::PARTS_BASE);					//GUNデータ
				set_parts_data_from_folder(&this->magazine_data, "data/Guns/mag/", EnumGunParts::PARTS_MAGAZINE);			//MAGデータ
				set_mads_data_from_folder(&this->meds_data, "data/Items/medkit/");								//MEDデータ
				set_gres_data_from_folder(&this->gres_data, "data/Items/grenade/");								//グレネード
				this->hit_obj_p.Init();																		//弾痕
				this->hit_b_obj_p.Init();																	//弾痕

				Vehcs::set_vehicles_pre("data/tank/", &vehcs, true);		/**/
				while (ProcessMessage() == 0) {
					clsDx();
					printfDx("ss : %d", GetASyncLoadNum());
					ScreenFlip();
					if (GetASyncLoadNum() == 0) {
						break;
					}
				}
				for (auto& t : vehcs) { t.Set_after(); }
			}
			void Start(void) noexcept {
				for (auto& g : this->mazzule_data) { g.Set(&g - &this->mazzule_data.front()); }					//PARTSデータ2
				for (auto& g : this->grip_data) { g.Set(&g - &this->grip_data.front()); }						//PARTSデータ2
				for (auto& g : this->uperhandguard_data) { g.Set(&g - &this->uperhandguard_data.front()); }		//PARTSデータ2
				for (auto& g : this->underhandguard_data) { g.Set(&g - &this->underhandguard_data.front()); }	//PARTSデータ2
				for (auto& g : this->mount_base_data) { g.Set(&g - &this->mount_base_data.front()); }			//PARTSデータ2
				for (auto& g : this->mount_data) { g.Set(&g - &this->mount_data.front()); }						//PARTSデータ2
				for (auto& g : this->stock_data) { g.Set(&g - &this->stock_data.front()); }						//PARTSデータ2
				for (auto& g : this->dustcover_data) { g.Set(&g - &this->dustcover_data.front()); }				//PARTSデータ2
				for (auto& g : this->lam_data) { g.Set(&g - &this->lam_data.front()); }							//PARTSデータ2
				for (auto& g : this->foregrip_data) { g.Set(&g - &this->foregrip_data.front()); }				//PARTSデータ2
				for (auto& g : this->sight_data) { g.Set(&g - &this->sight_data.front()); }						//PARTSデータ2
				for (auto& g : this->gun_data) { g.Set(&g - &this->gun_data.front()); }							//GUNデータ2
				for (auto& g : this->magazine_data) { g.Set(&g - &this->magazine_data.front()); }				//MAGデータ2
				for (auto& g : this->meds_data) { g.Set(&g - &this->meds_data.front()); }						//MEDデータ2
				for (auto& g : this->gres_data) { g.Set(&g - &this->gres_data.front()); }						//グレネード2
				this->hit_obj_p.Clear();																		//弾痕
				this->hit_b_obj_p.Clear();																		//弾痕
			}
			void Ready_Chara(const size_t& spawn_total) noexcept {
				//キャラ設定
				this->chara.resize(spawn_total);
				for (auto& c : this->chara) {
					c = std::make_shared<PLAYERclass::Chara>(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
					c->Set_Ptr(&this->chara, &c, &this->vehicle, nullptr);
					c->Set(gun_data, 0, body_obj, body_obj_lag, body_col);
				}
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Init(DrawPts, MAPPTs);
				//
				for (auto& c : this->chara) {
					//NPCのカスタムattach
					if (c != Get_Mine()) {
						//magazine
						c->Attach_parts(&magazine_data[0], EnumGunParts::PARTS_MAGAZINE);
						//grip
						c->Attach_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_GRIP_BASE);
						//uperhandguard
						c->Attach_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_UPER_HANDGUARD);
						//underhandguard
						c->Attach_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_UNDER_HANDGUARD);
						//sidemount + sight
						c->Attach_parts(&mount_base_data[0], EnumGunParts::PARTS_MOUNT_BASE, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_SIDEMOUNT_BASE);
						c->Attach_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, c->get_parts(EnumGunParts::PARTS_MOUNT_BASE), EnumAttachPoint::POINTS_SIDEMOUNT);
						//c->Attach_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, c->get_parts(EnumGunParts::PARTS_MOUNT), EnumAttachPoint::POINTS_UPER_RAIL, 1);
						//optional
						c->Attach_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_MAZZULE_BASE);
						c->Attach_parts(&dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_DUSTCOVER_BASE);
						c->Attach_parts(&stock_data[0], EnumGunParts::PARTS_STOCK, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_STOCK_BASE);
					}
					//初回位置設定スポーン
					{
						auto& wp = MAPPTs->get_spawn_point()[&c - &this->chara[0]];
						moves tmp;
						tmp.pos = wp;
						tmp.mat = MATRIX_ref::RotY(atan2f(wp.x(), wp.z()));
						c->Spawn(tmp);
					}
					//プレイヤー操作変数群
					c->Start();
				}
				TPSparts->Set(OPTPTs->Fov);						//TPS
				RULEparts->Set();									//ルール
				//ライティング
				Shadow_maxpos = MAPPTs->map_col_get().mesh_maxpos(0);
				Shadow_minpos = MAPPTs->map_col_get().mesh_minpos(0);
				Light_vec = VECTOR_ref::vget(0.5f, -0.5f, 0.5f);
				Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
				Light_color_ref = GetColorF(0.20f, 0.20f, 0.23f, 0.0f);
				MAPPTs->Set();									//環境

				{
					gndsmkHndle = EffekseerEffectHandle::load("data/effect/gndsmk.efk");
					//共通
					vehicle.resize(2);
					for (auto& v : this->vehicle) {
						v = std::make_shared<PLAYERclass::vehicles>(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
						v->Set_Ptr(&this->chara, nullptr, &this->vehicle, &v);
						v->Set(&vehcs[0], gndsmkHndle);
					}
				}

				//乗るときの登録
				this->Get_Mine()->MINE_v = &this->vehicle[0];
				this->vehicle[0]->MINE_c = &this->Get_Mine();
				//
				for (auto& c : this->chara) {
					c->Set_bullet();
				}
				for (auto& v : this->vehicle) {
					v->Set_bullet();
				}
			}
			bool UpDate(void) noexcept override {
				TEMPSCENE::UpDate();
				//アイテム演算
				{
					auto& item = MAPPTs->item;
					for (auto& g : item) { g->UpDate(MAPPTs->item, MAPPTs); }
					//時間が経ったマガジンを削除する
					bool fal = true;
					while (true) {
						fal = true;
						for (auto& i : item) {
							auto now = &i - &item.front();
							if (i->Detach_mag()) {
								std::iter_swap(item.begin() + now, item.end() - 1);
								item.pop_back();
								fal = false;
								break;
							}
							if (i->Detach_gre(MAPPTs, this->chara[0], this->chara)) {
								std::iter_swap(item.begin() + now, item.end() - 1);
								item.pop_back();
								fal = false;
								break;
							}
						}
						if (fal) {
							break;
						}
					}
				}
				//共通演算//2～3ms
				for (auto& c : this->chara) {
					c->UpDate_(RULEparts->get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data, this->gres_data,
						&c == &this->Get_Mine(), effsorce);
				}
				for (auto& v : this->vehicle) {
					v->UpDate(this->camera_main, this->Get_Mine());
				}
				MAPPTs->world->Step(1.f, 1, 1);
				for (auto& v : this->vehicle) {
					v->UpDate_after();
				}
				//弾痕の更新
				this->hit_obj_p.update();
				this->hit_b_obj_p.update();
				//campos,camvec,camupの指定
				this->Get_Mine()->Set_cam(this->camera_main, this->fov_base);
				//this->chara[1]->Set_cam(this->camera_main, this->fov_base);
				//vehicle[0]->Set_cam(this->camera_main, this->Get_Mine()->GetHMDmat().zvec(), this->fov_base);
				this->camera_main.camup = MATRIX_ref::Vtrans(this->camera_main.camup, MATRIX_ref::RotAxis(
					(this->camera_main.camvec - this->camera_main.campos),
					deg2rad(20.f * bless_ratio * sin(bless))
				));

				//ルール保存
				UIparts->Update();
				RULEparts->UpDate();
				//TPS描画
				TPSparts->key_TPS.GetInput(this->Get_Mine()->get_key_().TPS);
				TPSparts->Set_info(this->chara);
				if (TPSparts->key_TPS.on()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, [&] {Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.f, 2.5f, 2.f), VECTOR_ref::vget(15.f, 12.5f, 15.f));
					//書き込み
					Hostpassparts_TPS->BUF_draw([&](void) noexcept { BG_Draw(); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(); }); }, TPSparts->camera_TPS);	//被写体深度描画
					Hostpassparts_TPS->Set_MAIN_draw();																												//最終描画
				}
				//表示の如何
				{
					cam_info* camp = TPSparts->key_TPS.on() ? &TPSparts->camera_TPS : &this->camera_main;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						for (auto& c : this->chara) {
							c->Check_CameraViewClip(false);
						}
						for (auto& i : MAPPTs->item) {
							i->Check_CameraViewClip(MAPPTs, false);
						}

						MAPPTs->Check_CameraViewClip(MAPPTs, false);
					}
				}
				//終了判定
				if (RULEparts->get_timer() <= -5.f) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
				gndsmkHndle.Dispose();

				for (auto& c : this->chara) {
					c->Dispose();
				}
				this->chara.clear();

				for (auto& v : this->vehicle) {
					v->Dispose();
				}
				this->vehicle.clear();
			}
			void UI_Draw(void) noexcept  override {
				UIparts->UI_Draw(this->Get_Mine());
				//ミニマップ
				minimapparts->UI_Draw(this->chara, (TPSparts->key_TPS.on()) ? this->chara[TPSparts->sel_cam] : this->Get_Mine());
			}
			void BG_Draw(void) noexcept override {
				//map
				MAPPTs->sky_Draw();
			}
			void Shadow_Draw_Far(void) noexcept override {
				//map
				MAPPTs->Shadow_Draw_Far();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				//キャラ
				for (auto& c : this->chara) {
					if ((c->get_pos() - GetCameraPosition()).size() > 1.f) {
						c->Draw_chara(0);
					}
				}
				for (auto& v : this->vehicle) {
					v->Draw();
				}
			}
			void Shadow_Draw(void) noexcept override {
				//map
				MAPPTs->item_Draw();
				//キャラ
				for (auto& c : this->chara) {
					if ((c->get_pos() - GetCameraPosition()).size() <= 2.5f) {
						c->Draw_chara(1);
					}
				}
				for (auto& v : this->vehicle) {
					v->Draw();
				}
			}
			void Main_Draw(void) noexcept override {
				//map
				MAPPTs->main_Draw();
				//命中痕
				this->hit_obj_p.draw();
				this->hit_b_obj_p.draw();
				//キャラ
				for (auto& c : this->chara) { c->Draw_chara(); }
				for (auto& v : this->vehicle) { v->Draw(); }
				//レーザー
				for (auto& c : this->chara) { c->Draw_LAM_Effect(light); }
				//銃弾
				SetFogEnable(FALSE);
				SetUseLighting(FALSE);
				for (auto& c : this->chara) { c->Draw_ammo(); }
				for (auto& v : this->vehicle) { v->Draw_ammo(); }
				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
				//
			}
			void Item_Draw(void) noexcept override {
				//
				lens_zoom = this->Get_Mine()->DrawReticle_UI();
				lens_size = this->Get_Mine()->Get_reticle_size();
				if (lens_zoom > 1.0f) {
					use_lens = this->Get_Mine()->ads_on();
				}
				else {
					use_lens = false;
				}

				//歪み出し
				if (!use_bless && this->Get_Mine()->gre_eff) {
					use_bless = true;
					bless = 0.f;
					bless_ratio = 0.15f;
				}
				this->Get_Mine()->gre_eff = false;

				for (auto& c : this->chara) { c->Set_Draw_bullet(); }
				UIparts->item_Draw(this->chara, this->vehicle, this->Get_Mine());
			}
			void LAST_Draw(void) noexcept override {
				//TPS視点
				if (TPSparts->key_TPS.on()) {
					Hostpassparts_TPS->MAIN_draw();
				}
				//minimap
				minimapparts->Draw();
			}
		};
	};
};