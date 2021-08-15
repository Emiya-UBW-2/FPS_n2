#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		class LOAD;
		class SELECT;
		class MAINLOOP;
		//
		class TEMPSCENE : public PTR_COMMON {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//引き継ぐ
			std::shared_ptr<OPTION> OPTPTs;
			std::shared_ptr<GUNPARTS_Control> GunPartses;
			std::shared_ptr<key_bind> KeyBind;
			//カメラ
			cam_info camera_main;
			float fov_base = DX_PI_F / 2;
			//
			bool use_lens{ false };
			float lens_zoom = 1.f;
			float lens_size = 1.f;
			//
			bool use_bless{ false };
			float bless_ratio = 0.5f;
			float bless = 0.f;
			//
			virtual void Set_EnvLight(VECTOR_ref Shadow_minpos_t, VECTOR_ref Shadow_maxpos_t, VECTOR_ref Light_vec_t, COLOR_F Light_color_t) noexcept {
				Shadow_minpos = Shadow_minpos_t;
				Shadow_maxpos = Shadow_maxpos_t;
				Light_vec = Light_vec_t;
				Light_color = Light_color_t;
			}
		public:
			scenes Next_scene{ scenes::NONE_SCENE };			//現在のシーン
			std::shared_ptr<Sceneclass::TEMPSCENE> Next_scenes_ptr{ nullptr };

			void Init(
				std::shared_ptr<DXDraw>& DrawPts_t,
				const std::shared_ptr<OPTION>& OPTPTs_t,
				std::shared_ptr<MAPclass::Map>& MAPPTs_t,
				const std::shared_ptr<GUNPARTS_Control>& GunPartses_t,
				const std::shared_ptr<key_bind>& KeyBind_t
			) noexcept {
				PTR_COMMON::Set_Ptr_Common(MAPPTs_t, DrawPts_t);
				OPTPTs = OPTPTs_t;
				GunPartses = GunPartses_t;
				KeyBind = KeyBind_t;
			}
			void Set_Next(const std::shared_ptr<Sceneclass::TEMPSCENE>& Next_scenes_ptr_t, scenes NEXT) {
				Next_scenes_ptr = Next_scenes_ptr_t;
				Next_scene = NEXT;
			}

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return Light_vec; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Set(void) noexcept {
				fov_base = deg2rad(DrawPts->use_vr ? 120 : OPTPTs->Get_Fov());	//fov
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
				SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);											//
				if (DrawPts->use_vr) {
					camera_main.set_cam_info(fov_base, 0.001f, 100.f);//1P
				}
				else {
					camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
				}

				DrawPts->Set_Light_Shadow(Shadow_maxpos, Shadow_minpos, Light_vec, [&] {Shadow_Draw_Far(); });
				SetGlobalAmbientLight(Light_color);
			}


			virtual bool Update(void) noexcept {
				if (use_bless) {
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * Frame_Rate / GetFPS());
					bless_ratio -= (0.03f / 90.f) * Frame_Rate / GetFPS();
					if (bless_ratio <= 0.f) {
						use_bless = false;
					}
				}
				return true;
			}
			virtual void Dispose(void) noexcept {}

			virtual void ReadyDraw(void) noexcept {
				//音位置指定
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//影用意
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}

			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_x, GetColor(192, 192, 192), TRUE);
			}
			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}
			virtual const bool& is_lens(void) { return use_lens; }
			virtual const float& zoom_lens(void) { return lens_zoom; }
			virtual const float& size_lens(void) { return lens_size; }

			virtual const bool& is_bless(void) { return use_bless; }
			virtual const float& ratio_bless(void) { return bless_ratio; }
			virtual const float& time_bless(void) { return bless; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR() noexcept {}
			virtual void KeyOperation() noexcept {}
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
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				UIparts->Set(title);
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				return UIparts->Update();
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw();
			}
			void Dispose(void) noexcept override {
			}
		};
		//パーツデータのセーブ指定
		class SaveControl {
		protected:
			std::vector<save_c> save_parts;
			int P_select = 0;
			int P_select_max = 0;
		protected:
			void ControlSel() {
				if (P_select < 0) { P_select = P_select_max - 1; }
				if (P_select > P_select_max - 1) { P_select = 0; }
			}

			/*パーツデータをロード*/
			void Load(const std::string& Preset_Path) noexcept {
				std::fstream file;
				save_parts.clear();
				file.open(("data/save/" + Preset_Path).c_str(), std::ios::binary | std::ios::in);
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
			/*ベースデータを作成*/
			void CreateData() {
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

			//パーツのセーブ設定
			void Set_PartsSave(size_t chang_t, EnumGunParts parts_cat_t, EnumAttachPoint port_cat_t, EnumGunParts port_type_t) {
				if (save_parts.size() < size_t(P_select_max) + 1) {
					save_parts.resize(size_t(P_select_max) + 1);
				}
				save_parts[P_select_max].cang_ = chang_t;
				save_parts[P_select_max].type_ = parts_cat_t;
				save_parts[P_select_max].pt_cat_ = port_cat_t;
				save_parts[P_select_max].pt_type_ = port_type_t;
				save_parts[P_select_max].pt_sel_ = 0;
			}
			//セーブ
			void Save(std::string& Preset_Path) {
				std::fstream file;
				file.open(("data/save/" + Preset_Path).c_str(), std::ios::binary | std::ios::out);
				for (auto& tmp_save : save_parts) {
					file.write((char*)&tmp_save, sizeof(tmp_save));
				}
				file.close();
			}
		};

		class LOAD : public TEMPSCENE, public SaveControl {
		private:
			//キー
			switchs left;
			switchs right;
			//データ
			GUNPARTs* temp_p{ nullptr };
			//
			std::vector<std::string> Save_Presets;
			//そのまま
			std::unique_ptr<UIclass::UI_LOAD> UIparts;
			std::shared_ptr<PLAYERclass::PLAYER_CHARA> mine;
		public:
			const std::string& putout_Preset(void) const noexcept { return Save_Presets[P_select]; }
			//
			LOAD(void) noexcept {
				UIparts = std::make_unique<UIclass::UI_LOAD>();
			}
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				{
					left.Init(false);
					right.Init(false);
				}
				{
					Save_Presets.clear();
					WIN32_FIND_DATA win32fdt;
					HANDLE hFind;
					hFind = FindFirstFile("data/save/*", &win32fdt);
					if (hFind != INVALID_HANDLE_VALUE) {
						do {
							//if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {}
							if ((win32fdt.cFileName[0] != '.')) {
								Save_Presets.emplace_back(win32fdt.cFileName);
							}
						} while (FindNextFile(hFind, &win32fdt));
					} //else{ return false; }
					FindClose(hFind);
					P_select_max = int(Save_Presets.size());
				}
				P_select = 0;
				/*パーツデータをロード*/
				Load(putout_Preset());
			}
			void Start(std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine_t) {
				mine = mine_t;
			}
			void Dispose(void) noexcept override {
				mine.reset();
			}

			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				bool changef = false;
				//演算
				{
					SetMouseDispFlag(TRUE);
					left.GetInput(mine->Get_key_().akey);
					right.GetInput(mine->Get_key_().dkey);
					if (left.trigger()) {
						Sounds.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						--P_select;
						changef = true;
					}
					if (right.trigger()) {
						Sounds.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						++P_select;
						changef = true;
					}
					if (mine->Get_key_().jamp) {
						Sounds.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					ControlSel();
					if (changef) {
						/*パーツデータをロード*/
						Load(putout_Preset());
					}
				}
				return (!mine->Get_key_().jamp);
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw(GunPartses, save_parts, putout_Preset());
			}
			void KeyOperation_VR() noexcept override {
#ifdef _USE_OPENVR_
				auto& mine_k = mine->set_key_();
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						mine_k.dkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						mine_k.akey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() < -0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						mine_k.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);																							//jamp
					}
				}
#endif // _USE_OPENVR_
			}
			void KeyOperation() noexcept override {
				auto& mine_k = mine->set_key_();
				//設定
				KeyBind->set_Mode(1);
				mine_k.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				mine_k.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				mine_k.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
			}
		};
		//
		class SELECT : public TEMPSCENE, public SaveControl {
		private:
			//sound
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
			PLAYERclass::PLAYER_CHARA::g_parts* port_ptr{ nullptr };
			std::vector<PLAYERclass::PLAYER_CHARA::g_parts*> sight_ptr;
			int sight_p_s = 0;
			//
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
			std::string save_tgt = "1.dat";
			//そのまま
			std::unique_ptr<UIclass::UI_CUSTOM> UIparts;
			//
			std::string Preset;
			std::shared_ptr<PLAYERclass::PLAYER_CHARA> mine;
		private:
			//必須品
			void Set_Parts_Vital(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				if (P_select == P_select_max) {
					port_type = port_type_t;
					port_ptr = mine->Get_parts(port_type);
					port_cat = port_cat_t;
					parts_cat = parts_cat_t;
					{
						change_select_max = (int)GunPartses->Get_Parts_Data(parts_cat).size();
						change_select = std::clamp(change_select, 0, change_select_max - 1);
						parts_p = &(GunPartses->Get_Parts_Data(parts_cat))[change_select];
						/*
						{
							bool aaa = false;
							auto total_t = (int)GunPartses.Get_Parts_Data(parts_cat)->size();
							change_select_max = (int)GunPartses.Get_Parts_Data(parts_cat)->size();
							change_select = std::clamp(change_select, 0, total_t - 1);
							while (true) {
								auto tmp = &(*GunPartses.Get_Parts_Data(parts_cat))[change_select];

								aaa = false;
								for (auto& p : port_ptr->thisparts->can_attach) {
									if (p == tmp->mod.Get_name()) {
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
						mine->Detach_parts(parts_cat);
						mine->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
					}
					Set_PartsSave(size_t(change_select), parts_cat, port_cat, port_type);
				}
				++P_select_max;

				//サイト判定
				if (mine->Get_parts(parts_cat_t)->Set_sight_at(GunPartses->Get_Parts_Data(EnumGunParts::PARTS_SIGHT))) {
					sight_ptr[sight_p_s] = mine->Get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//非必須品
			void Set_Change_Optional(EnumGunParts port_type_t, EnumAttachPoint pot_cat, int sel = 0) noexcept {
				parts_cat = port_type_t;
				change_select_max = (int)GunPartses->Get_Parts_Data(parts_cat).size() + 1;
				change_select = std::clamp(change_select, 0, change_select_max - 1);
				if (change_select == 0) { parts_p = nullptr; }
				else { parts_p = &GunPartses->Get_Parts_Data(port_type_t)[std::max(change_select - 1, 0)]; }

				viewparts_buf = port_ptr->Get_rail_pos(pot_cat);
				mine->Detach_parts(port_type_t, (sel == 0) ? sel : (sel - 1));
				if (change_select != 0) {
					mine->Attach_parts(parts_p, port_type_t, port_ptr, pot_cat, sel);
				}
				Set_PartsSave(size_t(change_select) - 1, parts_cat, port_cat, port_type);
			}
			void Set_Parts_Optional(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				port_type = port_type_t;
				port_ptr = mine->Get_parts(port_type);
				port_cat = port_cat_t;
				if (port_ptr->Get_rail_frame(port_cat).first > 0) {
					if (P_select == P_select_max) {
						Set_Change_Optional(parts_cat_t, port_cat);
					}
					++P_select_max;
				}
				//サイト判定
				if (mine->Get_parts(parts_cat_t)->Set_sight_at(GunPartses->Get_Parts_Data(EnumGunParts::PARTS_SIGHT))) {
					sight_ptr[sight_p_s] = mine->Get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//sight
			void Set_Parts_Sight(PLAYERclass::PLAYER_CHARA::g_parts* s, int sel) {
				if (s != nullptr) {
					port_ptr = s;
					port_type = port_ptr->Get_type();

					port_cat = EnumAttachPoint::POINTS_UPER_RAIL;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (P_select == P_select_max) {
							Set_Change_Optional(EnumGunParts::PARTS_SIGHT, port_cat, sel);
							if (port_type == EnumGunParts::PARTS_MOUNT) {
								save_parts[P_select_max].pt_sel_ = 1;//改善
							}
						}
						++P_select_max;
						return;
					}

					port_cat = EnumAttachPoint::POINTS_SIDEMOUNT;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (P_select == P_select_max) {
							Set_Change_Optional(EnumGunParts::PARTS_SIGHT, port_cat, sel);
							if (port_type == EnumGunParts::PARTS_MOUNT) {
								save_parts[P_select_max].pt_sel_ = 1;//改善
							}
						}
						++P_select_max;
						return;
					}
				}
			}
		public:
			SELECT(void) noexcept {
				UIparts = std::make_unique<UIclass::UI_CUSTOM>();

			}

			void Start(std::string_view set_str, std::shared_ptr<PLAYERclass::PLAYER_CHARA>& mine_t) {
				Preset = set_str;
				mine = mine_t;
			}

			void Set(void) noexcept  override {
				//ライティング
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(1.f, 1.f, 1.f), VECTOR_ref::vget(-1.f, -1.f, -1.f), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				{
					up.Init(false);
					down.Init(false);
					left.Init(false);
					right.Init(false);
					shot.Init(false);
					Rot.Init(false);
					parts_p = nullptr;
					parts_cat = EnumGunParts::PARTS_NONE;

					P_select = 0;
					P_select_max = 0;

					change_select = 0;
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
					shot_se = SoundHandle::Load(mine->Get_Gunaudio().shot_path);
					slide_se = SoundHandle::Load(mine->Get_Gunaudio().slide_path);
					trigger_se = SoundHandle::Load("data/audio/gun/trigger.wav");
					SetCreateSoundTimeStretchRate(1.f);
				}
				/*パーツデータをロード*/
				if (Preset != "") {
					Load(Preset);
				}
				else {
					CreateData();
				}
				//読み込み
				{
					int pppp = 0;
					for (const auto& tmp_save : save_parts) {
						GUNPARTs* temp_p = nullptr;
						PLAYERclass::PLAYER_CHARA::g_parts* temp_ptr = nullptr;
						if (tmp_save.cang_ != SIZE_MAX) {
							auto& vec_data = GunPartses->Get_Parts_Data(tmp_save.type_);
							if (&vec_data != nullptr) {
								temp_p = &vec_data[tmp_save.cang_];
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
								temp_ptr = mine->Get_parts(tmp_save.pt_type_);
								break;
							default:
								break;
							}
						}
						if (tmp_save.type_ == EnumGunParts::PARTS_SIGHT) { ++pppp; }
						mine->Attach_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_, pppp);
					}
					//save_parts.clear();
				}
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				bool changef = false;
				//演算
				{
					{
						moves tmp;
						mine->set_gun_pos(tmp);
						mine->Set_gun();
						mine->set_mag_pos();
						mine->Mag_setpos_Nomal();
						//キー
						up.GetInput(mine->Get_key_().wkey);
						down.GetInput(mine->Get_key_().skey);
						left.GetInput(mine->Get_key_().akey);
						right.GetInput(mine->Get_key_().dkey);
						shot.GetInput(mine->Get_key_().shoot);
						Rot.GetInput(mine->Get_key_().select);
						//
						if (Start_b) {
							Start_b = false;
							//changef = true;
							int pp = mine->Get_parts(EnumGunParts::PARTS_BASE)->thisparts->Select_Chose(EnumSELECTER::SELECT_SEMI);
							if (pp != -1) {
								mine->gun_stat_now->selector_set((EnumSELECTER)pp);
							}
						}
						if (left.trigger()) {
							P_select--;
							change_select = 0;
							changef = true;
						}
						if (right.trigger()) {
							++P_select;
							change_select = 0;
							changef = true;
						}
						ControlSel();
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
							P_select_max = 0;
							//サイト判定
							sight_ptr.resize(5);
							for (auto& s : sight_ptr) {
								s = nullptr;
							}
							sight_p_s = 0;
						}
						Set_Parts_Vital(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_MAGAZINE_BASE, EnumGunParts::PARTS_MAGAZINE);			//magazine
						Set_Parts_Vital(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_GRIP_BASE, EnumGunParts::PARTS_GRIP);					//grip
						Set_Parts_Vital(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_UPER_HANDGUARD, EnumGunParts::PARTS_UPER_HGUARD);		//uperhandguard
						Set_Parts_Vital(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_UNDER_HANDGUARD, EnumGunParts::PARTS_UNDER_HGUARD);	//underhandguard
						Set_Parts_Optional(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::POINTS_UNDER_RAIL, EnumGunParts::PARTS_FOREGRIP);	//foregrip
						Set_Parts_Optional(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::POINTS_LEFTSIDE_RAIL, EnumGunParts::PARTS_LAM);	//lam
						Set_Parts_Optional(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_SIDEMOUNT_BASE, EnumGunParts::PARTS_MOUNT_BASE);	//sidemount
						Set_Parts_Optional(EnumGunParts::PARTS_MOUNT_BASE, EnumAttachPoint::POINTS_SIDEMOUNT, EnumGunParts::PARTS_MOUNT);		//sidemount_2
						Set_Parts_Optional(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_MAZZULE_BASE, EnumGunParts::PARTS_MAZZULE);		//マズル
						Set_Parts_Optional(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_DUSTCOVER_BASE, EnumGunParts::PARTS_DUSTCOVER);	//ダストカバー
						Set_Parts_Optional(EnumGunParts::PARTS_BASE, EnumAttachPoint::POINTS_STOCK_BASE, EnumGunParts::PARTS_STOCK);			//ストック
						//sight
						{
							int ssp = P_select_max;
							for (auto& s : sight_ptr) {
								Set_Parts_Sight(s, (P_select_max - ssp + 1));
							}
						}
						//
						{
							change_per = 1.f;

							Sounds.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);

							xrad_t = viewparts_buf.y() * 1000.f;
							yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z() * 100.f : -90.f + viewparts_buf.z() * 100.f);
						}
					}
					//
					easing_set(&mine->Get_gunanime_trigger()->per, float(shot.press()), 0.5f);
					if (shot.trigger()) {
						if (mine->set_flag_gun()) {
							//todo ディレイつける
							shot_se.play(DX_PLAYTYPE_BACK, TRUE);
							slide_se.play(DX_PLAYTYPE_BACK, TRUE);
							trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
							//薬莢
							mine->create_cart();
							//エフェクト
							mine->calc_shot_effect();
						}
					}
					//薬莢
					mine->calc_cart(rate);
					//銃セレクターアニメ
					mine->Set_select_anime();
					//銃発砲アニメ
					mine->Set_shot_anime(rate, true);
					//銃アニメ更新
					mine->Get_parts(EnumGunParts::PARTS_BASE)->Update_Anim();
					//薬莢の処理
					mine->Update_cart();
					//エフェクトの更新
					mine->Update_effect();
				}
				//campos,camvec,camupの指定
				{}
				{
					{
						int x_m, y_m;
						GetMousePoint(&x_m, &y_m);
						x_m -= DrawPts->disp_x / 2;
						y_m -= DrawPts->disp_y / 2;
						SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);
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
					range_tgt = std::hypotf(sin(deg2rad(yrad_t)) * 0.25f, cos(deg2rad(yrad_t)) * (std::abs((((std::abs(yrad_t) > 90) ? mine->Get_maz().z() : 0.5f) - viewparts.z())) + camera_main.near_ * 2.f));
					range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol()) * 0.1f, range_tgt, 5.f) : range_tgt;

					easing_set(&camparts, VECTOR_ref::vget(cos(deg2rad(xrad_t)) * sin(deg2rad(yrad_t)), sin(deg2rad(xrad_t)), cos(deg2rad(xrad_t)) * cos(deg2rad(yrad_t))) * range_t, 0.8f);
					camera_main.camvec = viewparts;
					camera_main.campos = camera_main.camvec + camparts;
					camera_main.camup = VECTOR_ref::up();
				}
				//表示の如何
				{
					cam_info* camp = &this->camera_main;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						mine->Check_CameraViewClip(false);
					}
				}
				if (mine->Get_key_().jamp) {
					Sounds.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);
					return false;
				}
				return true;
				//
			}
			void Dispose(void) noexcept override {
				//データセーブ
				Save(save_tgt);
				mine->gun_stat_now->selector_set(EnumSELECTER::SELECT_SEMI);
				shot_se.Dispose();
				slide_se.Dispose();
				trigger_se.Dispose();
				mine.reset();
			}
			void UI_Draw(void) noexcept  override {
				UIparts->UI_Draw(GunPartses, parts_cat, Rot.on(), mine, parts_p, change_per);
				easing_set(&change_per, 0.f, 0.5f);
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				mine->Draw_gun();
				mine->Get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
			void Shadow_Draw(void) noexcept override {
				mine->Draw_gun();
				mine->Get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
			void Main_Draw(void) noexcept override {
				mine->Draw_gun();
				mine->Get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
			void KeyOperation_VR() noexcept override {
#ifdef _USE_OPENVR_
				auto& mine_k = mine->set_key_();
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//射撃
						mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						mine_k.wkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.y() > 0.5f && ptr_->touch.x() < 0.5f && ptr_->touch.x() > -0.5f);		//running
						mine_k.skey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.y() < -0.5f && ptr_->touch.x() < 0.5f && ptr_->touch.x() > -0.5f);		//running
						mine_k.dkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						mine_k.akey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() < -0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						mine_k.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);									//jamp
					}
				}
#endif // _USE_OPENVR_
			}
			void KeyOperation() noexcept override {
				auto& mine_k = mine->set_key_();
				//設定
				KeyBind->set_Mode(1);
				mine_k.wkey = KeyBind->Get_key_use(EnumKeyBind::FRONT);
				mine_k.skey = KeyBind->Get_key_use(EnumKeyBind::BACK);
				mine_k.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				mine_k.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				mine_k.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
				mine_k.shoot = KeyBind->Get_mouse_use(EnumMouseBind::SHOOT);
				mine_k.select = KeyBind->Get_mouse_use(EnumMouseBind::CHANGE_SEL);
			}
		};
		//
		class MAINLOOP : public TEMPSCENE {
#define USE_TPS

			//TPS操作
			class TPS_parts : public PTR_COMMON {
				float xr_cam = 0.f;
				float yr_cam = 0.f;
				//カメラ
				cam_info camera_TPS;
			public:
				switchs key_TPS;
				int sel_cam = 0;
				void Set(const float& fov_pc) noexcept {
					this->key_TPS.Init(false);
					this->camera_TPS.campos = VECTOR_ref::vget(0, 1.8f, -10);
					this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
				}
				void Set_info(std::vector<std::shared_ptr<PLAYERclass::PLAYER_CHARA>>& chara) noexcept {
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
							VECTOR_ref vec_2 = ((chara[this->sel_cam]->Get_pos() + VECTOR_ref::vget(0, 1.5f, 0)) - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

							this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50)) * 0.1f;
							this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50)) * 0.1f;
							this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
						}
						//
						this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VECTOR_ref::front(), MATRIX_ref::RotX(this->xr_cam) * MATRIX_ref::RotY(this->yr_cam));
						this->camera_TPS.camup = VECTOR_ref::up();
					}
				}
				const auto ON() const noexcept { return key_TPS.on(); }
				auto& Get_camera() noexcept { return camera_TPS; }
			};
			std::unique_ptr<TPS_parts> TPSparts;
			std::unique_ptr<HostPassEffect> Hostpassparts_TPS;
			//データ
			MV1 body_obj, body_obj_lag, body_col;		//身体モデル
			MV1 hit_pic;								//弾痕  
			GraphHandle light;							//ライトテクスチャ
			std::vector<Meds> meds_data;				//GUNデータ
			std::vector<Grenades> gres_data;			//GUNデータ
			std::vector<std::shared_ptr<PLAYERclass::PLAYER_CHARA>> chara;		//キャラ
			HIT_PASSIVE hit_obj_p;						//静的弾痕
			HIT_BLOOD_PASSIVE hit_b_obj_p;				//静的血痕
			std::unique_ptr<UIclass::UI_MAINLOOP> UIparts;
			std::shared_ptr<RULE_parts> RULEparts;
			std::unique_ptr<MAPclass::MiniMap> MiniMAPPTs;

			std::vector<Vehcs> vehc_data;
			std::vector<std::shared_ptr<PLAYERclass::PLAYER_VEHICLE>> vehicle;
		public:
			std::shared_ptr<PLAYERclass::PLAYER_CHARA>& Get_Mine(void) noexcept { return this->chara[0]; }
			const bool CanGetMine(void) const noexcept { return this->chara.size() > 0; }
			std::vector<Meds>& Get_meds_data(void) noexcept { return this->meds_data; }
			std::vector<Grenades>& Get_gres_data(void) noexcept { return this->gres_data; }
		public:
			MAINLOOP(const std::shared_ptr<MAPclass::Map>& MAPPTs_t, const std::shared_ptr<OPTION>& OPTPTs_t) noexcept {
				//
				MAPPTs = MAPPTs_t;
				OPTPTs = OPTPTs_t;
				//
				TPSparts = std::make_unique<TPS_parts>();
				TPSparts->Set_Ptr_Common(MAPPTs, DrawPts);
				Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
				RULEparts = std::make_shared<RULE_parts>();
				UIparts = std::make_unique<UIclass::UI_MAINLOOP>(RULEparts);
				MiniMAPPTs = std::make_unique<MAPclass::MiniMap>(MAPPTs);					//ミニマップ
				light = GraphHandle::Load("data/light.png");								//ライト
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);				//弾痕モデル
				MV1::Load("data/model/body/model.mv1", &this->body_obj, true);				//身体モデル
				MV1::Load("data/model/body/col.mv1", &this->body_col, true);				//身体colモデル
				MV1::Load("data/model/body/model_lag.mv1", &this->body_obj_lag, true, -1.f);//身体ラグドール
				Meds::Set_Pre(&this->meds_data, "data/Items/medkit/");						//MEDデータ
				Grenades::Set_Pre(&this->gres_data, "data/Items/grenade/");					//グレネード
				Vehcs::Set_Pre(&this->vehc_data, "data/tank/");								//戦車
				this->hit_obj_p.Init();														//弾痕
				this->hit_b_obj_p.Init();													//血痕
			}
			void Start(void) noexcept {
				for (auto& g : this->meds_data) { g.Set(&g - &this->meds_data.front()); }						//MEDデータ2
				for (auto& g : this->gres_data) { g.Set(&g - &this->gres_data.front()); }						//グレネード2
				for (auto& t : this->vehc_data) { t.Set(); }													//戦車2
				this->hit_obj_p.Clear();																		//弾痕
				this->hit_b_obj_p.Clear();																		//弾痕
			}
			void Ready_Chara(const size_t& spawn_total) noexcept {
				//キャラ設定
				this->chara.resize(spawn_total);
				for (auto& c : this->chara) {
					c = std::make_shared<PLAYERclass::PLAYER_CHARA>();
					c->Set_Ptr_Common(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
					c->Set_Ptr(&this->chara, &c, &this->vehicle, nullptr);
					c->Set(GunPartses->Get_Parts_Data(EnumGunParts::PARTS_GUN), 0, body_obj, body_obj_lag, body_col);
				}
			}
			void Ready_Tank(const size_t& spawn_total) {
				//戦車設定
				vehicle.resize(spawn_total);
				for (auto& v : this->vehicle) {
					v = std::make_shared<PLAYERclass::PLAYER_VEHICLE>();
					v->Set_Ptr_Common(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
					v->Set_Ptr(&this->chara, nullptr, &this->vehicle, &v);
					v->Set(vehc_data, int(&v - &this->vehicle.front()), hit_pic);
				}
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				//NPCのカスタムattach
				for (auto& c : this->chara) {
					if (c != Get_Mine()) {
						//magazine
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_MAGAZINE)[0], EnumGunParts::PARTS_MAGAZINE);
						//grip
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_GRIP)[0], EnumGunParts::PARTS_GRIP, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_GRIP_BASE);
						//uperhandguard
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_UPER_HGUARD)[0], EnumGunParts::PARTS_UPER_HGUARD, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_UPER_HANDGUARD);
						//underhandguard
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_UNDER_HGUARD)[0], EnumGunParts::PARTS_UNDER_HGUARD, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_UNDER_HANDGUARD);
						//sidemount + sight
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_MOUNT_BASE)[0], EnumGunParts::PARTS_MOUNT_BASE, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_SIDEMOUNT_BASE);
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_MOUNT)[0], EnumGunParts::PARTS_MOUNT, c->Get_parts(EnumGunParts::PARTS_MOUNT_BASE), EnumAttachPoint::POINTS_SIDEMOUNT);
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_SIGHT)[0], EnumGunParts::PARTS_SIGHT, c->Get_parts(EnumGunParts::PARTS_MOUNT), EnumAttachPoint::POINTS_UPER_RAIL, 1);
						//optional
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_MAZZULE)[0], EnumGunParts::PARTS_MAZZULE, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_MAZZULE_BASE);
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_DUSTCOVER)[0], EnumGunParts::PARTS_DUSTCOVER, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_DUSTCOVER_BASE);
						c->Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::PARTS_STOCK)[0], EnumGunParts::PARTS_STOCK, c->Get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::POINTS_STOCK_BASE);
					}
				}
				//初回スポーン位置設定
				moves temp;
				for (auto& c : this->chara) {
					auto& wp = MAPPTs->Get_spawn_point()[&c - &this->chara[0]];
					temp.pos = wp;
					temp.mat = MATRIX_ref::RotY(atan2f(wp.x(), wp.z()));
					c->SetSpawnPos(temp);
					c->Spawn();
					c->Start();
				}
				for (auto& v : this->vehicle) {
					temp.pos = VECTOR_ref::vget(30.f + 10.f*int(&v - &this->vehicle.front()), 10.f, -30.f);
					temp.mat = MATRIX_ref::RotY(deg2rad(-90.f));
					v->SetSpawnPos(temp);
					v->Spawn();
					v->Start();
				}
				//弾薬設定
				for (auto& c : this->chara) {
					c->SetUp_bullet(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
					c->Set_bullet_Ptr();
				}
				for (auto& v : this->vehicle) {
					v->SetUp_bullet(MAPPTs, DrawPts, &this->hit_obj_p, &this->hit_b_obj_p);
					v->Set_bullet_Ptr();
				}
				//UI
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				//
				TPSparts->Set(OPTPTs->Get_Fov());		//TPS
				RULEparts->Set();						//ルール
				MAPPTs->Set();							//環境
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				//アイテム演算
				{
					auto& item = MAPPTs->item;
					for (auto& g : item) { g->Update(item, MAPPTs->Map_col_line); }
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
							if (i->Detach_gre(this->chara[0], this->chara, MAPPTs->Map_col_line)) {
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
					c->Update(RULEparts->Get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data, this->gres_data, &c == &this->Get_Mine());
					//乗り降り
					c->set_canride_f() = false;
					if (!c->isRide()) {
						for (auto& v : this->vehicle) {
							auto range = c->Get_pos() - v->Get_pos();
							if (range.size() < 3.f) {
								c->set_canride_f() = true;
								if ((&this->Get_Mine() == &c) && this->Get_Mine()->Get_key_().ride) {
									c->Ride_on(&v);	//乗るときの登録
									break;
								}
							}
						}
					}
					else {
						if (this->Get_Mine()->Get_key_().ride) {
							this->Get_Mine()->Ride_on(nullptr);				//乗るときの登録
						}
					}
				}
				for (auto& v : this->vehicle) {
					v->Update(this->camera_main, this->camera_main.fov / this->fov_base);
				}
				MAPPTs->world->Step(1.f, 1, 1);
				for (auto& v : this->vehicle) {
					v->Update_after();
				}
				//弾痕の更新
				this->hit_obj_p.Update();
				this->hit_b_obj_p.Update();
				//campos,camvec,camupの指定
				this->Get_Mine()->Setcamera(this->camera_main, this->fov_base);
				this->camera_main.camup = MATRIX_ref::Vtrans(this->camera_main.camup, MATRIX_ref::RotAxis((this->camera_main.camvec - this->camera_main.campos), deg2rad(20.f * ratio_bless() * sin(bless))));

				//ルール保存
				UIparts->Update();
				RULEparts->Update();
				//TPS描画
				TPSparts->Set_info(this->chara);
				TPSparts->key_TPS.GetInput(this->Get_Mine()->Get_key_().TPS);
				if (TPSparts->ON()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->Get_camera().campos, [&] {Shadow_Draw(); }, [&] {Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.f, 2.5f, 2.f), VECTOR_ref::vget(15.f, 12.5f, 15.f));
					//書き込み
					Hostpassparts_TPS->BUF_Draw([&] { BG_Draw(); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(); }); }, TPSparts->Get_camera(), false);	//被写体深度描画
					Hostpassparts_TPS->Set_MAIN_Draw();																												//最終描画
				}
				//表示の如何
				{
					cam_info* camp = TPSparts->ON() ? &TPSparts->Get_camera() : &this->camera_main;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						for (auto& c : this->chara) {
							c->Check_CameraViewClip(false);
						}
						for (auto& i : MAPPTs->item) {
							i->Check_CameraViewClip(false, MAPPTs->Map_col_line);
						}

						MAPPTs->Check_CameraViewClip(false);
					}
				}
				//終了判定
				if (RULEparts->Get_timer() <= -5.f) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
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
				UIparts->UI_Draw(this->Get_Mine(), KeyBind);
				//ミニマップ
				MiniMAPPTs->UI_Draw(this->chara, (TPSparts->ON()) ? this->chara[TPSparts->sel_cam] : this->Get_Mine());
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
					if ((c->Get_pos() - GetCameraPosition()).size() > 1.f) {
						c->Draw_chara();
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
					if ((c->Get_pos() - GetCameraPosition()).size() <= 2.5f) {
						c->Draw_chara();
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
				this->hit_obj_p.Draw();
				this->hit_b_obj_p.Draw();
				//キャラ
				for (auto& c : this->chara) { c->Draw_chara(); }
				for (auto& v : this->vehicle) { v->Draw(); }
				//レーザー
				for (auto& c : this->chara) { c->Draw_LAM_Effect(light); }
				for (auto& v : this->vehicle) { v->Draw_LAM_Effect(); }
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
				TEMPSCENE::Item_Draw();
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
				if (TPSparts->ON()) {
					Hostpassparts_TPS->MAIN_Draw();
				}
				//minimap
				MiniMAPPTs->Draw();
			}

			void KeyOperation_VR() noexcept override {
#ifdef _USE_OPENVR_
				auto& mine_k = Get_Mine()->set_key_();
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//射撃
						mine_k.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																						//マグキャッチ
						mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);		//マガジン持つ
						mine_k.have_item = ((ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0);	//アイテム取得
						mine_k.sort_magazine = false;									//
						mine_k.view_gun = false;										//
						mine_k.drop_ = false;											//
						mine_k.throw_gre = false;										//
						mine_k.running = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0);		//running
						mine_k.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);				//jamp
					}
				}
#endif // _USE_OPENVR_
			}
			void KeyOperation() noexcept override {
				auto& mine_k = Get_Mine()->set_key_();
				//設定
				KeyBind->set_Mode(0);
				mine_k.have_mag = true;
				mine_k.wkey = KeyBind->Get_key_use(EnumKeyBind::FRONT);
				mine_k.skey = KeyBind->Get_key_use(EnumKeyBind::BACK);
				mine_k.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				mine_k.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				mine_k.qkey = KeyBind->Get_key_use(EnumKeyBind::Q);
				mine_k.ekey = KeyBind->Get_key_use(EnumKeyBind::E);
				mine_k.reload = KeyBind->Get_key_use(EnumKeyBind::RELOAD);
				mine_k.have_item = KeyBind->Get_key_use(EnumKeyBind::GET_ITEM);
				mine_k.throw_gre = KeyBind->Get_key_use(EnumKeyBind::THROW_GRE);
				KeyBind->Get_key_use_ID(EnumKeyBind::SIT).set_key(mine_k.squat);	//記憶
				mine_k.squat = KeyBind->Get_key_use(EnumKeyBind::SIT);
				mine_k.sort_magazine = KeyBind->Get_key_use(EnumKeyBind::SORT_MAG);
				mine_k.running = KeyBind->Get_key_use(EnumKeyBind::RUN);
				mine_k.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
				mine_k.TPS = KeyBind->Get_key_use(EnumKeyBind::CHANGE_VIEW);
				mine_k.view_gun = KeyBind->Get_key_use(EnumKeyBind::LOOKGUN);
				mine_k.drop_ = KeyBind->Get_key_use(EnumKeyBind::MED);
				mine_k.shoot = KeyBind->Get_mouse_use(EnumMouseBind::SHOOT);
				mine_k.select = KeyBind->Get_mouse_use(EnumMouseBind::CHANGE_SEL);
				mine_k.aim = KeyBind->Get_mouse_use(EnumMouseBind::AIM);
				mine_k.ride = KeyBind->Get_key_use(EnumKeyBind::RIDE_ON);
			}

		};
	};
};